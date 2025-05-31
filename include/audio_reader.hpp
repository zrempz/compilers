#pragma once
#include <string>
#include <vector>

class AudioReader {
public:
  bool read(const std::string &filename, std::vector<double> &audioData,
            double &sampleRate);
};
