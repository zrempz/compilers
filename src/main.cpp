#include "audio_processor.hpp"
#include <iomanip>
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Uso: " << argv[0] << " <archivo_audio>" << std::endl;
    return 1;
  }

  AudioProcessor processor;
  if (!processor.processFile(argv[1])) {
    return 1;
  }

  std::cout << "\nNOTAS DEL ARCHIVO\n";
  for (const auto &event : processor.getNoteEvents()) {
    double duration = event.endTime - event.startTime;
    std::cout << std::fixed << std::setprecision(3) << "Nota: " << std::setw(8)
              << std::left << event.note << " Inicio: " << std::setw(6)
              << event.startTime << "s"
              << " Duración: " << std::setw(6) << duration << "s"
              << " Confianza: " << std::setw(5) << std::setprecision(1)
              << event.confidence << "%\n";
  }

  return 0;
}
