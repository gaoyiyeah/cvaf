#include <iostream>
#include <vector>
#include "filter.h"

using namespace std;

int Filter::GetEnergy(const std::vector<std::vector<int>>& image) {
	int energy_black = 0, energy_white = 0;
	if (type == 1) {
		int band_mid = (band_start + band_end) / 2;
		for (int i = time_start; i < time_end; i++) {
			for (int j = band_start; j < band_mid; j++)
				energy_white += image[i][j];
			for (int j = band_mid; j < band_end; j++)
				energy_black += image[i][j];
		}
	} else if (type == 2) {
		int time_mid = time_end / 2;
		for (int j = band_start; j < band_end; j++) {
			for (int i = time_start; i < time_mid; i++)
				energy_white += image[i][j];
			for (int i = time_mid; i < time_end; i++)
				energy_black += image[i][j];
		}
	} else if (type == 3) {
		int time_mid = time_end / 2;
		int band_mid = (band_start + band_end) / 2;
		for (int i = time_start; i < time_mid; i++) {
			for (int j = band_start; j < band_mid; j++)
				energy_white += image[i][j];
			for (int j = band_mid; j < band_end; j++)
				energy_black += image[i][j];
		}
		for (int i = time_mid; i < time_end; i++) {
			for (int j = band_start; j < band_mid; j++)
				energy_black += image[i][j];
			for (int j = band_mid; j < band_end; j++)
				energy_white += image[i][j];
		}
	} else if (type == 4) {
		int band_lower = band_start + (band_end - band_start) / 4;
		int band_upper = band_start + (band_end - band_start) / 4 * 3;
		for (int i = time_start; i < time_end; i++) {
			for (int j = band_start; j < band_lower; j++)
				energy_white += image[i][j];
			for (int j = band_lower; j < band_upper; j++)
				energy_black += image[i][j];
			for (int j = band_upper; j < band_end; j++)
				energy_white += image[i][j];
		}
	} else if (type == 5) {
		int time_lower = time_start + (time_end - time_start) / 4;
		int time_upper = time_start + (time_end - time_start) / 4 * 3;
		for (int j = band_start; j < band_end; j++) {
			for (int i = time_start; i < time_lower; i++)
				energy_white += image[i][j];
			for (int i = time_lower; i < time_upper; i++)
				energy_black += image[i][j];
			for (int i = time_upper; i < time_end; i++)
				energy_white += image[i][j];
		}
	}

	return energy_black;
}