#include "audio_reader.hpp"
#include "note_converter.hpp"
#include "pitch_detector.hpp"
#include "utils.hpp"
#include <iomanip>
#include <iostream>
#include <vector>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Uso: " << argv[0] << " <archivo_audio>" << std::endl;
    return 1;
  }

  // Leer audio
  AudioReader reader;
  std::vector<double> audioData;
  double sampleRate;
  if (!reader.read(argv[1], audioData, sampleRate)) {
    return 1;
  }

  // Parámetros de procesamiento
  const int frameSize = 2048;
  const int hopSize = frameSize / 4;
  PitchDetector detector(sampleRate, frameSize);

  // Procesar por frames
  for (size_t start = 0; start + frameSize <= audioData.size();
       start += hopSize) {
    std::vector<double> frame(audioData.begin() + start,
                              audioData.begin() + start + frameSize);

    applyHannWindow(frame);
    double freq = detector.detect(frame);
    std::string note = NoteConverter::frequencyToNote(freq);

    double time = start / sampleRate;
    std::cout << std::fixed << std::setprecision(3) << time << "\t" << freq
              << "\t" << note << std::endl;
  }

  return 0;
}
