#pragma once
#include <iostream>
#include <vector>
#include "global.h"

class Filter {
public:
	int id;
	int type; // type can only be from 1 to 5
	// All time and frequency band intervals are [0, 1)-like intervals.
	int time_start = 0;
	int time_end;
	int band_start;
	int band_end;
	double threshold;
	double confidence;
	double error_rate;

	Filter(){}
	Filter(int id) : id(id){}
	// Must insure that start_row + FRAME_LENGTH is smaller than the total rows of image.
	// The image size is alwayls FRAME_LENGTH * 33.
	double GetEnergy(double image[][33], int start_row = 0);
};