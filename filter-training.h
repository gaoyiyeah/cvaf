#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "filter.h"

class Sample {
public:
	int song_id;
	int frame_id;
	int image[82][33];

	Sample(){};
	Sample(int s_id, int f_id) : song_id(s_id), frame_id(f_id){}
};

class SamplePair {
public:
	Sample sample1;
	Sample sample2;
	bool label; // True means they belong to the same song.
};

class FilterTraining {
public:
	std::vector<Filter> filters;
	void GenerateFilter();
	void PrepareSamples(const std::string& original_wave_path,
		const std::string& degraded_wave_path);
private:
	std::vector<SamplePair> sample_pairs;
};