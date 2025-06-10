#include "note_converter.hpp"
#include "sound_delimiters.hpp"
#include <cmath>
#include <vector>

std::string NoteConverter::frequencyToNote(double freq) {
  if (freq <= 0)
    return sound_delimiters::statement_delimiter.data();

  const double A4 = 440.0;
  // Calcular nota MIDI (69 = A4)
  double midiNote = 69.0 + 12.0 * log2(freq / A4);
  int midiNoteRounded = static_cast<int>(std::round(midiNote));

  // Manejar valores fuera de rango
  if (midiNoteRounded < 0)
    midiNoteRounded = 0;
  if (midiNoteRounded > 127)
    midiNoteRounded = 127;

  int octave = (midiNoteRounded / 12) - 1;
  int noteIndex = midiNoteRounded % 12;

  // Ajustar índices negativos
  if (noteIndex < 0) {
    noteIndex += 12;
    octave -= 1;
  }

  const std::vector<std::string> noteNames = {
      "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};

  return noteNames[noteIndex] + std::to_string(octave);
}
