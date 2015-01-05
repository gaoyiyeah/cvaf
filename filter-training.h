#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "filter.h"
#include "global.h"

class Sample {
public:
	int song_id;
	int frame_id;
	double image[FRAME_LENGTH][33];

	Sample(){};
	Sample(int s_id, int f_id) : song_id(s_id), frame_id(f_id) {}
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
	void PringFiltersToFile(const std::string& filepath);
	std::vector<Filter> LoadFilters(const std::string& filepath);
	void GetDistribution();
	void TestClassifier(const std::string& original_wave_path,
		const std::string& degraded_wave_path, const std::vector<Filter>& filters);
private:
	void _GenerateFilter();
	void _PrepareSamples(const std::string& original_wavepath,
		const std::string& degraded_wavepath);
	static void _PreparePositiveSamples(const std::vector<std::string>& filenames,
		const std::string& original_wavepath);
	void _CalculateThreshold();
	void _PreComputeEnergy();
	void _Training();

	static std::vector<SamplePair> _sample_pairs;
	std::vector<Filter> _filters;
	std::vector<Filter> _selected_filters;
	std::vector<int> _thresholds; // Thresholds for filters.
	// energy[i][j] denotes the energy of jth sample in ith filter.
	std::vector<std::vector<SampleEnergy>> _energy;
};