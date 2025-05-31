#include "note_tracker.hpp"
#include <algorithm>

NoteTracker::NoteTracker(double minNoteDuration, double maxGap,
                         double silenceThreshold)
    : minNoteDuration_(minNoteDuration), maxGap_(maxGap),
      silenceThreshold_(silenceThreshold), currentNote_(""), currentStart_(0.0),
      lastNoteTime_(0.0), currentNoteFrames_(0), currentSegmentFrames_(0) {}

void NoteTracker::processFrame(double time, const std::string &note) {
  // Incrementamos el contador de frames para el segmento actual
  currentSegmentFrames_++;

  // Manejar silencios
  const bool isSilence = (note == "Silence");

  // Detectar cambio de nota
  if (note != currentNote_) {
    // Finalizar nota anterior si cumple con duración mínima
    if (!currentNote_.empty()) {
      double duration = time - currentStart_;
      if (duration >= minNoteDuration_) {
        finishCurrentNote(time);
      }
    }

    // Iniciar nueva nota (o silencio)
    currentNote_ = note;
    currentStart_ = time;
    currentNoteFrames_ = 1;
  } else {
    // Continuación de la misma nota
    currentNoteFrames_++;
  }

  // Actualizar último tiempo de nota válida
  if (!isSilence) {
    lastNoteTime_ = time;
  }
}

void NoteTracker::finishCurrentNote(double endTime) {

  // Calcular confianza como porcentaje de frames consistentes
  double confidence = (currentSegmentFrames_ > 0)
                          ? (static_cast<double>(currentNoteFrames_) /
                             currentSegmentFrames_ * 100.0)
                          : 0.0;

  events_.push_back({currentStart_, endTime, currentNote_, confidence});

  // Resetear contadores para el próximo segmento
  currentNote_ = "";
  currentNoteFrames_ = 0;
  currentSegmentFrames_ = 0;
}

void NoteTracker::finalize(double totalTime) {
  if (!currentNote_.empty()) {
    double duration = totalTime - currentStart_;
    if (duration >= minNoteDuration_) {
      finishCurrentNote(totalTime);
    }
  }

  // Consolidar notas cercanas del mismo tipo
  for (size_t i = 0; i < events_.size();) {
    if (events_[i].note == "Silence") {
      i++;
      continue;
    }

    size_t j = i + 1;
    while (j < events_.size()) {
      // Verificar si son la misma nota con gap pequeño
      if (events_[j].note == events_[i].note &&
          (events_[j].startTime - events_[i].endTime) <= maxGap_) {

        // Calcular confianza combinada (promedio ponderado)
        double totalDuration = events_[j].endTime - events_[i].startTime;
        double w1 = (events_[i].endTime - events_[i].startTime) / totalDuration;
        double w2 = (events_[j].endTime - events_[j].startTime) / totalDuration;
        double combinedConfidence =
            (events_[i].confidence * w1) + (events_[j].confidence * w2);

        // Extender la nota actual
        events_[i].endTime = events_[j].endTime;
        events_[i].confidence = combinedConfidence;

        // Eliminar el evento fusionado
        events_.erase(events_.begin() + j);
      } else {
        break;
      }
    }
    i = j;
  }

  // Eliminar silencios demasiado cortos
  events_.erase(std::remove_if(events_.begin(), events_.end(),
                               [this](const NoteEvent &e) {
                                 return e.note == "Silence" &&
                                        (e.endTime - e.startTime) <
                                            silenceThreshold_;
                               }),
                events_.end());
}

std::vector<NoteEvent> NoteTracker::getNoteEvents() const { return events_; }
