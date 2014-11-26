#pragma once

#include <iostream>
#include <vector>

class Filter {
public:
	int type; // type can only be from 1 to 5
	// All time and frequency band intervals are [0, 1)-like intervals.
	int time_start = 0;
	int time_end;
	int band_start;
	int band_end;
	int threshold;
	double confidence;
	int GetEnergy(const std::vector<std::vector<int>>& image);
};