#pragma once
#include <vector>

bool isSilence(const std::vector<double> &frame, double thresholdDb = -60.0);
void applyHannWindow(std::vector<double> &frame);
