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
};

class FilterTraining {
public:
	void GenerateFilter();
	void PrepareSamples(const std::string& original_wave_path,
		const std::string& degraded_wave_path);
	void CalculateThreshold();
private:
	std::vector<SamplePair> sample_pairs;
	std::vector<Filter> filters;
	std::vector<int> thresholds; // Thresholds for filters.
};