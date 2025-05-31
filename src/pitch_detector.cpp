#include "pitch_detector.hpp"
#include "utils.hpp"
#include <algorithm>

PitchDetector::PitchDetector(double sampleRate, int frameSize, double threshold)
    : sampleRate_(sampleRate), frameSize_(frameSize), threshold_(threshold) {
  tauMin_ =
      static_cast<int>(sampleRate_ / 1000.0);     // Frecuencia máxima: 1000 Hz
  tauMax_ = static_cast<int>(sampleRate_ / 50.0); // Frecuencia mínima: 50 Hz
  tauMax_ = std::min(tauMax_, frameSize_ / 2);
}

std::vector<double>
PitchDetector::computeDifference(const std::vector<double> &frame) const {
  std::vector<double> diff(tauMax_, 0.0);

  for (int tau = 0; tau < tauMax_; ++tau) {
    for (int j = 0; j < frameSize_ - tau; ++j) {
      double delta = frame[j] - frame[j + tau];
      diff[tau] += delta * delta;
    }
  }
  return diff;
}

std::vector<double> PitchDetector::cumulativeMeanNormalizedDifference(
    const std::vector<double> &diff) const {
  std::vector<double> normDiff(diff.size(), 1.0);
  double runningSum = 0.0;

  for (int tau = 1; tau < (int)diff.size(); ++tau) {
    runningSum += diff[tau];
    normDiff[tau] = diff[tau] * tau / runningSum;
  }
  return normDiff;
}

int PitchDetector::absoluteThreshold(
    const std::vector<double> &normDiff) const {
  for (int tau = tauMin_; tau < tauMax_; ++tau) {
    if (normDiff[tau] < threshold_) {
      while (tau + 1 < tauMax_ && normDiff[tau + 1] < normDiff[tau]) {
        tau++;
      }
      return tau;
    }
  }

  // Fallback: mínimo global
  return static_cast<int>(
      std::min_element(normDiff.begin() + tauMin_, normDiff.end()) -
      normDiff.begin());
}

double PitchDetector::parabolicInterpolation(const std::vector<double> &diff,
                                             int tau) const {
  double betterTau = static_cast<double>(tau);

  if (tau > 0 && tau < (int)diff.size() - 1) {
    double denom = 2 * (2 * diff[tau] - diff[tau - 1] - diff[tau + 1]);
    if (denom != 0) {
      double delta = diff[tau - 1] - diff[tau + 1];
      betterTau += delta / denom;
    }
  }
  return betterTau;
}

double PitchDetector::detect(const std::vector<double> &frame) const {
  // Verificar silencio
  if (isSilence(frame))
    return 0.0;

  std::vector<double> diff = computeDifference(frame);
  std::vector<double> normDiff = cumulativeMeanNormalizedDifference(diff);
  int tau = absoluteThreshold(normDiff);
  double betterTau = parabolicInterpolation(diff, tau);

  return (betterTau > 0) ? sampleRate_ / betterTau : 0.0;
}
