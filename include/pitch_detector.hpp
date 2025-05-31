#pragma once
#include <vector>

class PitchDetector {
public:
  PitchDetector(double sampleRate, int frameSize, double threshold = 0.15);
  double detect(const std::vector<double> &frame) const;

private:
  double sampleRate_;
  int frameSize_;
  double threshold_;
  int tauMin_;
  int tauMax_;

  std::vector<double> computeDifference(const std::vector<double> &frame) const;
  std::vector<double>
  cumulativeMeanNormalizedDifference(const std::vector<double> &diff) const;
  int absoluteThreshold(const std::vector<double> &normDiff) const;
  double parabolicInterpolation(const std::vector<double> &diff, int tau) const;
};
