#include <iostream>
#include <vector>
#include "filter-training.h"

using namespace std;

// 5 kinds of filters, totally 26301 filters.
void FilterTraining::GenerateFilter() {
	std::vector<int> time_range = { 1, 2, 3, 5, 7, 11, 17, 25, 38, 57, 82 }; // time from 1 frame to 82 frame in exponential steps of 1.5

	// Generate filters of type 1. Totally 5808 filters.
	for (int time_end = 0; time_end < time_range.size(); time_end++) {
		for (int band_start = 0; band_start < 32; band_start++) {
			for (int band_end = band_start + 2; band_end <= 33; band_end++) {
				Filter filter;
				filter.type = 1;
				filter.time_end = time_range[time_end];
				filter.band_start = band_start;
				filter.band_end = band_end;
				filters.push_back(filter);
			}
		}
	}

	// Generate filters of type 2. Totally 5610 filters.
	for (int time_end = 1; time_end < time_range.size(); time_end++) {
		for (int band_start = 0; band_start < 33; band_start++) {
			for (int band_end = band_start + 1; band_end <= 33; band_end++) {
				Filter filter;
				filter.type = 2;
				filter.time_end = time_range[time_end];
				filter.band_start = band_start;
				filter.band_end = band_end;
				filters.push_back(filter);
			}
		}
	}

	// Generate filters of type 3. Totally 5280 filters.
	for (int time_end = 1; time_end < time_range.size(); time_end++) {
		for (int band_start = 0; band_start < 32; band_start++) {
			for (int band_end = band_start + 2; band_end <= 33; band_end++) {
				Filter filter;
				filter.type = 3;
				filter.time_end = time_range[time_end];
				filter.band_start = band_start;
				filter.band_end = band_end;
				filters.push_back(filter);
			}
		}
	}

	// Generate filters of type 4. Totally 5115 filters.
	for (int time_end = 0; time_end < time_range.size(); time_end++) {
		for (int band_start = 0; band_start < 30; band_start++) {
			for (int band_end = band_start + 4; band_end <= 33; band_end++) {
				Filter filter;
				filter.type = 4;
				filter.time_end = time_range[time_end];
				filter.band_start = band_start;
				filter.band_end = band_end;
				filters.push_back(filter);
			}
		}
	}

	// Generate filters of type 5. Totally 4488 filters.
	for (int time_end = 3; time_end < time_range.size(); time_end++) {
		for (int band_start = 0; band_start < 33; band_start++) {
			for (int band_end = band_start + 1; band_end <= 33; band_end++) {
				Filter filter;
				filter.type = 5;
				filter.time_end = time_range[time_end];
				filter.band_start = band_start;
				filter.band_end = band_end;
				filters.push_back(filter);
			}
		}
	}
}