#pragma once
#include <iostream>
#include <vector>
#include "global.h"

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
	// Must insure that start_row + 82 is smaller than the total rows of image.
	// The image size is alwayls 82 * 33.
	int GetEnergy(int image[][33], int start_row = 0);
};