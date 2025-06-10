#include "audio_processor.hpp"
#include "audio_reader.hpp"
#include "note_converter.hpp"
#include "sound_delimiters.hpp"
#include "utils.hpp"
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>

const int HISTORY_SIZE = 5;

AudioProcessor::AudioProcessor(int frameSize, int hopSizeDivider)
    : frameSize_(frameSize), hopSize_(frameSize / hopSizeDivider) {
  // Inicializamos la cola con HISTORY_SIZE elementos "Silence"
  noteHistory_ = std::deque<std::string>(
      HISTORY_SIZE, sound_delimiters::statement_delimiter.data());
}

bool AudioProcessor::processFile(const std::string &filename) {
  // Leer audio
  AudioReader reader;
  std::vector<double> audioData;
  double sampleRate;
  if (!reader.read(filename, audioData, sampleRate)) {
    return false;
  }

  PitchDetector detector(sampleRate, frameSize_);
  NoteTracker noteTracker;

  // Procesar por frames
  for (size_t start = 0; start + frameSize_ <= audioData.size();
       start += hopSize_) {
    std::vector<double> frame(audioData.begin() + start,
                              audioData.begin() + start + frameSize_);

    applyHannWindow(frame);
    double currentTime = static_cast<double>(start) / sampleRate;

    processFrame(currentTime, frame, detector, noteTracker);
  }

  // Finalizar procesamiento
  double totalDuration = audioData.size() / sampleRate;
  noteTracker.finalize(totalDuration);
  noteEvents_ = noteTracker.getNoteEvents();

  return true;
}

void AudioProcessor::processFrame(double time, const std::vector<double> &frame,
                                  PitchDetector &detector,
                                  NoteTracker &tracker) {
  double freq = detector.detect(frame);
  std::string note = NoteConverter::frequencyToNote(freq);

  std::string smoothedNote = smoothNote(note);
  tracker.processFrame(time, smoothedNote);
}

std::string AudioProcessor::smoothNote(const std::string &currentNote) {
  noteHistory_.pop_front();
  noteHistory_.push_back(currentNote);

  // Contar ocurrencias de cada nota
  std::map<std::string, int> noteCount;
  for (const auto &n : noteHistory_) {
    noteCount[n]++;
  }

  // Seleccionar nota más frecuente
  auto bestNote = std::max_element(
      noteCount.begin(), noteCount.end(),
      [](const auto &a, const auto &b) { return a.second < b.second; });

  return bestNote->first;
}

const std::vector<NoteEvent> &AudioProcessor::getNoteEvents() const {
  return noteEvents_;
}

bool AudioProcessor::saveNoteEventsToFile(const std::string &filename) const {
  std::ofstream file(filename);
  if (!file.is_open()) {
    return false;
  }

  for (const auto &event : noteEvents_) {
    if (event.note == sound_delimiters::space_delimiter) {
      file << " ";
      continue;
    }

    if (event.note == sound_delimiters::statement_delimiter) {
      file << "\n";
      continue;
    }
    file << event.note;
  }

  return true;
}

void AudioProcessor::printNoteEvents() {
  for (const auto &event : noteEvents_) {
    double duration = event.endTime - event.startTime;
    std::cout << std::fixed << std::setprecision(3) << "Nota: " << std::setw(8)
              << std::left << event.note << " Inicio: " << std::setw(6)
              << event.startTime << "s"
              << " Duración: " << std::setw(6) << duration << "s"
              << " Confianza: " << std::setw(5) << std::setprecision(1)
              << event.confidence << "%\n";
  }
}
