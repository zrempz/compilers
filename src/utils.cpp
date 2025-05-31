#include "utils.hpp"
#include <cmath>

bool isSilence(const std::vector<double> &frame, double thresholdDb) {
  double rms = 0.0;
  for (double sample : frame) {
    rms += sample * sample;
  }
  rms = sqrt(rms / frame.size());
  double db = 20 * log10(rms + 1e-10);
  return db < thresholdDb;
}

void applyHannWindow(std::vector<double> &frame) {
  const int N = frame.size();
  for (int n = 0; n < N; ++n) {
    frame[n] *= 0.5 * (1 - cos(2 * M_PI * n / (N - 1)));
  }
}
