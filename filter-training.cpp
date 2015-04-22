#include <algorithm>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>
#include "filter-training.h"
#include "fingerprint-extractor.h"
#include "util.h"

using namespace std;

vector<SamplePair> FilterTraining::_sample_pairs;

vector<Filter> FilterTraining::Training(const string& original_wave_path,
	const string& degraded_wave_path) {
	_GenerateFilter();
	_PrepareSamples(original_wave_path, degraded_wave_path);
	_PreComputeEnergy();
	_CalculateThreshold();
	_Training();
	return _selected_filters;
}

void FilterTraining::TestClassifier(const string& original_wave_path,
	const string& degraded_wave_path, const vector<Filter>& filters) {
	_PrepareSamples(original_wave_path, degraded_wave_path);
	int total_yes = 0;
	int positive_yes = 0;
	int negative_yes = 0;
	for (auto sample : _sample_pairs) {
		double result = 0;
		for (auto filter : filters) {
			bool sign1 = filter.GetEnergy(sample.sample1.image) - filter.threshold > 0 ? true : false;
			bool sign2 = filter.GetEnergy(sample.sample2.image) - filter.threshold > 0 ? true : false;
			bool re = !(sign1 ^ sign2);
			if (re)
				result += filter.confidence;
			else
				result -= filter.confidence;
		}
		bool classification = result > 0 ? true : false;
		if (classification == sample.label) {
			total_yes++;
			if (sample.label == true)
				positive_yes++;
			else
				negative_yes++;
		}
	}
	cout << "Total classification rate: " << (double)total_yes / _sample_pairs.size() << endl;
	cout << "Positive classification rate: " << (double)2.0 * positive_yes / _sample_pairs.size() << endl;
	cout << "Negative classification rate: " << (double)2.0 * negative_yes / _sample_pairs.size() << endl;
	return;
}

void FilterTraining::PringFiltersToFile(const string& filepath) {
	ofstream fout(filepath + "filters.dat", ios::out | ofstream::binary);
	fout.write(reinterpret_cast<char *>(&_selected_filters[0]),
		32 * sizeof(_selected_filters[0]));
	fout.close();

	fout.open(filepath + "filters.txt", ios::out);
	fout << "Type\tTime end\tBand start\tBand end\tError rate" << endl;
	for (const auto& f : _selected_filters)
		fout << f.id << "\t\t" << f.type << "\t\t" << f.time_end << "\t\t"
		<< f.band_start << "\t\t" << f.band_end << "\t\t" << f.error_rate
		<< endl;
	fout.close();
	return;
}

vector<Filter> FilterTraining::LoadFilters(const string& filepath) {
	vector<Filter> filters(32);
	ifstream fin(filepath, ios::in | ifstream::binary);
	fin.read(reinterpret_cast<char *>(&filters[0]),	32 * sizeof(filters[0]));
	fin.close();
	return filters;
}

// 5 kinds of filters, totally 26301 filters.
void FilterTraining::_GenerateFilter() {
	// time from 1 frame to FRAME_LENGTH frame in exponential steps of 1.5
	std::vector<int> time_range = { 1, 2, 3, 5, 7, 11, 17, 25, 38, 57, 82};
	//std::vector<int> time_range = { 1, 2, 3, 5, 7, 11, 17, 25, 38, 41};
	//vector<int> time_range = { 1, 2, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21 };
	int id = 0;
	// Generate filters of type 1. Totally 5808 filters.
	for (int time_end = 0; time_end < time_range.size(); time_end++) {
		for (int band_start = 0; band_start < BINDS_NUM - 1; band_start++) {
			for (int band_end = band_start + 2; band_end <= BINDS_NUM; band_end++) {
				Filter filter(id++);
				filter.type = 1;
				filter.time_end = time_range[time_end];
				filter.band_start = band_start;
				filter.band_end = band_end;
				_filters.push_back(filter);
			}
		}
	}

	// Generate filters of type 2. Totally 5610 filters.
	for (int time_end = 1; time_end < time_range.size(); time_end++) {
		for (int band_start = 0; band_start < BINDS_NUM; band_start++) {
			for (int band_end = band_start + 1; band_end <= BINDS_NUM; band_end++) {
				Filter filter(id++);
				filter.type = 2;
				filter.time_end = time_range[time_end];
				filter.band_start = band_start;
				filter.band_end = band_end;
				_filters.push_back(filter);
			}
		}
	}

	// Generate filters of type 3. Totally 5280 filters.
	for (int time_end = 1; time_end < time_range.size(); time_end++) {
		for (int band_start = 0; band_start < BINDS_NUM - 1; band_start++) {
			for (int band_end = band_start + 2; band_end <= BINDS_NUM; band_end++) {
				Filter filter(id++);
				filter.type = 3;
				filter.time_end = time_range[time_end];
				filter.band_start = band_start;
				filter.band_end = band_end;
				_filters.push_back(filter);
			}
		}
	}

	// Generate filters of type 4. Totally 5115 filters.
	for (int time_end = 0; time_end < time_range.size(); time_end++) {
		for (int band_start = 0; band_start < BINDS_NUM - 3; band_start++) {
			for (int band_end = band_start + 4; band_end <= BINDS_NUM; band_end++) {
				Filter filter(id++);
				filter.type = 4;
				filter.time_end = time_range[time_end];
				filter.band_start = band_start;
				filter.band_end = band_end;
				_filters.push_back(filter);
			}
		}
	}

	// Generate filters of type 5. Totally 4488 filters.
	for (int time_end = 3; time_end < time_range.size(); time_end++) {
		for (int band_start = 0; band_start < BINDS_NUM; band_start++) {
			for (int band_end = band_start + 1; band_end <= BINDS_NUM; band_end++) {
				Filter filter(id++);
				filter.type = 5;
				filter.time_end = time_range[time_end];
				filter.band_start = band_start;
				filter.band_end = band_end;
				_filters.push_back(filter);
			}
		}
	}
	cout << "Filters: " << _filters.size() << endl;
}

void FilterTraining::_PreparePositiveSamples(const vector<string>& degraded_files,
	const string& original_wavepath) {
	for (const auto file : degraded_files) {
		cout << file << endl;
		vector<Sample> original_samples, degraded_samples;

		FingerprintExtractor extractor;
		extractor.CreateImage(file);
		extractor.GetSamples(&degraded_samples);
		string filename = file.substr(file.find_last_of("\\") + 1, file.size());
		extractor.CreateImage(original_wavepath + "\\" + filename);
		extractor.GetSamples(&original_samples);
		// Add positive samples.
		for (size_t i = 0; i < degraded_samples.size(); i++) {
			SamplePair pair;
			pair.sample1 = degraded_samples[i];
			pair.sample2 = original_samples[i];
			pair.label = true;
			_sample_pairs.push_back(pair);
		}
	}
}

void FilterTraining::_PrepareSamples(const string& original_wavepath,
	const string& degraded_wavepath) {
	_sample_pairs.clear();
	vector<string> all_degraded_files;
	all_degraded_files = Util::LoadDir(degraded_wavepath, "wav");
	
	vector<thread> threads;
	vector<vector<string>> degraded_files(THREAD_NUM);
	for (int i = 0; i < all_degraded_files.size(); i++)
		degraded_files[i%THREAD_NUM].push_back(all_degraded_files[i]);
	for (int i = 0; i < THREAD_NUM; i++)
		threads.push_back(thread(_PreparePositiveSamples, degraded_files[i], original_wavepath));
	for (int i = 0; i < THREAD_NUM; i++)
		threads[i].join();

	// Prepare negative samples.
	srand((unsigned)time(NULL));
	size_t pos_num = _sample_pairs.size();
	for (size_t neg_num = 0; neg_num < pos_num;) {
		int neg1 = rand() % pos_num;
		int neg2 = rand() % pos_num;
		// Generate a negative sample with different song ids.
		if (_sample_pairs[neg1].sample1.song_id != _sample_pairs[neg2].sample2.song_id) {
			SamplePair pair;
			pair.sample1 = _sample_pairs[neg1].sample1;
			pair.sample2 = _sample_pairs[neg2].sample2;
			pair.label = false;
			_sample_pairs.push_back(pair);
			neg_num++;
		}
	}
	cout << "Totally generate " << _sample_pairs.size() << " samples." << endl;
	return;
}

void FilterTraining::_PreComputeEnergy() {
	cout << _filters.size() << endl;
	time_t start, end;
	start = clock();
	_energy.resize(_filters.size());
	for (size_t i = 0; i < _filters.size(); i++)
		_energy[i].resize(_sample_pairs.size());
	
	for (size_t i = 0; i < _filters.size(); i++) {
		for (size_t j = 0; j < _sample_pairs.size(); j++) {
			_energy[i][j].energy1 = _filters[i].GetEnergy(_sample_pairs[j].sample1.image);
			_energy[i][j].energy2 = _filters[i].GetEnergy(_sample_pairs[j].sample2.image);
		}
	}
	end = clock();
	cout << "Pre compute takes " << (end - start) / CLOCKS_PER_SEC << "s." << endl;
}

void FilterTraining::_CalculateThreshold() {
	for (size_t i = 0; i < _filters.size(); i++) {
		long long threshold = 0;
		vector<double> response;
		for (size_t j = 0; j < _sample_pairs.size(); j++) {
			if (_sample_pairs[j].label == false)
				break;
			response.push_back(_energy[i][j].energy1);
			response.push_back(_energy[i][j].energy2);
		}
		sort(response.begin(), response.end());
		size_t size = response.size();
		if (size % 2)
			_filters[i].threshold = response[size / 2];
		else
			_filters[i].threshold = (response[size / 2 - 1] + response[size / 2]) / 2;
	}
	cout << "Generate thresholds done!" << endl;
	return;
}

void FilterTraining::_Training() {
	time_t start, end;
	vector<size_t> selected_filters_idx;

	// Initialize the weight of each filter as 1/n.
	for (size_t i = 0; i < _sample_pairs.size(); i++)
		_sample_pairs[i].weight = 1.0 / _sample_pairs.size();

	// 32 times interation, 32 weak classifiers.
	for (int number = 0; number < 32; number++) {
		start = clock();
		// 1. Find the hypothesis that minimizes weighted error.
		size_t selected_filter = -1;
		double min_err = INT_MAX;
		for (size_t i = 0; i < _filters.size(); i++) {
			double local_min_err = 0;
			for (size_t j = 0; j < _sample_pairs.size(); j++) {
				bool sign1 = (_energy[i][j].energy1 - _filters[i].threshold) > 0 ? true : false;
				bool sign2 = (_energy[i][j].energy2 - _filters[i].threshold) > 0 ? true : false;
				bool re = !(sign1 ^ sign2);
				if (re != _sample_pairs[j].label)
					local_min_err += _sample_pairs[j].weight;
			}
			if (local_min_err < min_err &&
				find(selected_filters_idx.begin(), selected_filters_idx.end(), i) == selected_filters_idx.end()) {
				min_err = local_min_err;
				selected_filter = i;
			}
		}
		_selected_filters.push_back(_filters[selected_filter]);
		selected_filters_idx.push_back(selected_filter);
		cout << "Select filter: " << selected_filter << endl;
		// 2. Calculate weight error.
		double weight_error = 0;
		int wrong_sample = 0;
		Filter& filter = _selected_filters[number];
		vector<size_t> error_samples;
		for (size_t i = 0; i < _sample_pairs.size(); i++) {
			bool sign1 = (_energy[selected_filter][i].energy1 - filter.threshold) > 0 ? true : false;
			bool sign2 = (_energy[selected_filter][i].energy2 - filter.threshold) > 0 ? true : false;
			bool re = !(sign1 ^ sign2);
			if (re != _sample_pairs[i].label) {
				wrong_sample++;
				weight_error += _sample_pairs[i].weight;
				error_samples.push_back(i);
			}
		}
		filter.error_rate = (double)wrong_sample / _sample_pairs.size();
		// 3. Assign confidence to hm.
		double confidence = log((1 - weight_error) / weight_error);
		filter.confidence = confidence;

		// 4. Update weights for matching pairs.
		for (const size_t i : error_samples) {
			if (_sample_pairs[i].label == true)
				_sample_pairs[i].weight *= ((1 - weight_error) / weight_error);
		}

		// 5. Normalize weights.
		double matching_weight = 0, non_matching_weight = 0;
		for (const SamplePair& pair : _sample_pairs) {
			if (pair.label == true)
				matching_weight += pair.weight;
			else
				non_matching_weight += pair.weight;
		}
		for (SamplePair& pair : _sample_pairs) {
			if (pair.label == true)
				pair.weight /= matching_weight;
			else
				pair.weight /= non_matching_weight;
			pair.weight /= 2;
		}
		end = clock();
		//cerr << "Round " << number << " done. Using: " << (end - start) / CLOCKS_PER_SEC << "s." << endl;
	}
	return;
}

void FilterTraining::GetDistribution() {
	vector<Filter> filters = this->LoadFilters("E:\\yangguang\\cvaf\\data\\filters.dat");
	string original_wav = "E:\\yangguang\\cvaf\\data\\training\\original_wav";
	string degraded_wav = "E:\\yangguang\\cvaf\\data\\training\\degraded_wav";
	string output_path = "E:\\yangguang\\cvaf\\data\\filter-distribution\\";
	this->_PrepareSamples(original_wav, degraded_wav);
	for (size_t i = 0; i < filters.size(); i++) {
		vector<double> response;
		for (size_t j = 0; j < _sample_pairs.size(); j++) {
			if (_sample_pairs[j].label == false)
				break;
			response.push_back(filters[i].GetEnergy(_sample_pairs[j].sample1.image));
			response.push_back(filters[i].GetEnergy(_sample_pairs[j].sample2.image));
		}
		sort(response.begin(), response.end());
		int size = response.size();
		double gap = (response[size - 1] - response[0]) / 99;
		vector<pair<double, int>> distribution(100);
		for (size_t j = 0; j < 100; j++)
			distribution[j].first = response[0] + gap * j;
		for (size_t j = 0; j < response.size(); j++) {
			distribution[(response[j] - response[0]) / gap].second++;
		}
		string file_path = output_path + to_string(filters[i].id) + ".txt";
		fstream fout;
		fout.open(file_path, fstream::out);
		for (auto dis : distribution)
			fout << dis.first << "\t" << (double)dis.second / response.size() << endl;
		fout.close();
		cerr << "Distribution: " << i << " done!" << endl;
	}
}