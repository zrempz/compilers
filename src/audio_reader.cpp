#include "audio_reader.hpp"
#include <iostream>
#include <sndfile.h>
#include <vector>

bool AudioReader::read(const std::string &filename,
                       std::vector<double> &audioData, double &sampleRate) {
  SF_INFO sfInfo;
  SNDFILE *file = sf_open(filename.c_str(), SFM_READ, &sfInfo);
  if (!file) {
    std::cerr << "Error: No se pudo abrir el archivo: " << filename
              << std::endl;
    return false;
  }

  audioData.resize(sfInfo.frames * sfInfo.channels);
  sf_read_double(file, audioData.data(), audioData.size());
  sampleRate = static_cast<double>(sfInfo.samplerate);

  // Convertir a mono si es estéreo
  if (sfInfo.channels > 1) {
    std::vector<double> monoData(sfInfo.frames);
    for (int i = 0; i < sfInfo.frames; ++i) {
      double sum = 0.0;
      for (int c = 0; c < sfInfo.channels; ++c) {
        sum += audioData[i * sfInfo.channels + c];
      }
      monoData[i] = sum / sfInfo.channels;
    }
    audioData = monoData;
  }

  sf_close(file);
  return true;
}
