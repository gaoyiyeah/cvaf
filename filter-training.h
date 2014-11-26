#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "filter.h"

class Sample {
public:
	int song_id;
	int frame_id;
	std::vector<std::vector<int>> image;

	Sample(){};
	Sample(int s_id, int f_id) : song_id(s_id), frame_id(f_id) {
		image.resize(82);
		for (int i = 0; i < 82; i++)
			image[i].resize(33);
	}
};

class SamplePair {
public:
	Sample sample1;
	Sample sample2;
	bool label; // True means they belong to the same song.
	double weight;
};

class SampleEnergy {
public:
	double energy1;
	double energy2;
};

class FilterTraining {
public:
	std::vector<Filter> Training(const std::string& original_wave_path,
		const std::string& degraded_wave_path);
private:
	void _GenerateFilter();
	void _PrepareSamples(const std::string& original_wave_path,
		const std::string& degraded_wave_path);
	void _CalculateThreshold();
	void _PreComputeEnergy();
	void _Training(std::vector<Filter>* selected_filters);

	std::vector<SamplePair> sample_pairs;
	std::vector<Filter> filters;
	std::vector<int> thresholds; // Thresholds for filters.
	// energy[i][j] denotes the energy of jth sample in ith filter.
	std::vector<std::vector<SampleEnergy>> energy;
};