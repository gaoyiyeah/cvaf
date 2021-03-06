#include <ctime>
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>
#include "filter.h"
#include "filter-training.h"
#include "fingerprint-extractor.h"
#include "searcher.h"
#include "util.h"

using namespace std;

int yes = 0;
int not_found = 0;
vector<Filter> filters;
Searcher searcher;
FilterTraining ft;
time_t start_time, end_time;

void ExtractFingerprint(vector<string>& all_files) {
	FingerprintExtractor extractor;
	for (const string& f : all_files) {
		size_t pos = f.find_last_of("\\");
		string tmp = f.substr(pos + 1, f.size() - pos);
		string filename = tmp.substr(0, tmp.find("."));
		extractor.CalcFingerprint(f, filters);
		extractor.PrintFingerToFile(FINGER_ROOTPATH + "\\" + filename + ".txt");
		cerr << "File: " << f << " done." << endl;
	}
}

void SearchOneFile(vector<string>& allQueryFiles) {
	FingerprintExtractor extractor;
	for (int i = 0; i < (signed)allQueryFiles.size(); i++) {
		extractor.CalcFingerprint(allQueryFiles[i], filters);
		bitset<32> finger_block[QUERY_FINGER_NUM];
		int size = 0;
		int queryId = extractor.GetFileId();
		//cout << queryId << endl;
		extractor.GetQueryFinger(finger_block, size);		
		int result = searcher.Search(queryId, finger_block, size);
		if (result == -1) {
			//cout<<"file: "<<queryId<<" Not found"<<endl;
			not_found++;
		} else if (result == queryId) {
			yes++;
		} else {
			//cout << queryId << "\t" << result << "\t" << endl;
		}
	}
}

void Training() {
	string original_wav = "E:\\yangguang\\cvaf\\data\\training\\original";
	string degraded_wav = "E:\\yangguang\\cvaf\\data\\training\\32kbps";
	start_time = clock();
	filters = ft.Training(original_wav, degraded_wav);
	ft.PringFiltersToFile(FILTER_PATH);
	end_time = clock();
	cout << "Time: " << (double)(end_time - start_time) / CLOCKS_PER_SEC << endl;
}

void Testing() {
	string original_wav = "E:\\yangguang\\cvaf\\data\\training\\original";
	string degraded_wav = "E:\\yangguang\\cvaf\\data\\training\\32kbps";
	filters = ft.LoadFilters("E:\\yangguang\\cvaf\\data\\filters.dat");
	ft.TestClassifier(original_wav, degraded_wav, filters);
}

void Fingerprinting() {
	filters = ft.LoadFilters(FILTER_PATH + "filters.dat");
	vector<thread> threads;
	start_time = clock();
	vector<string> allFiles = Util::LoadDir(WAVE_ROOTPATH, "wav");
	vector<vector<string>> allQueryFiles(THREAD_NUM);
	for (int i = 0; i < allFiles.size(); i++)
		allQueryFiles[i%THREAD_NUM].push_back(allFiles[i]);
	for (int i = 0; i < THREAD_NUM; i++)
		threads.push_back(thread(ExtractFingerprint, allQueryFiles[i]));
	for (int i = 0; i < THREAD_NUM; i++)
		threads[i].join();

	cout << "Extract Done!" << endl;
	end_time = clock();
	cout << "Time: " << (end_time - start_time) / CLOCKS_PER_SEC << endl;
}

void Searching() {
	start_time = clock();
	searcher.BuildIndex(FINGER_ROOTPATH);
	end_time = clock();
	cout << "Load index: " << (double)(end_time - start_time) / CLOCKS_PER_SEC << endl;
	vector<string> path{ "128kbps", "96kbps", "64kbps", "32kbps" };
	for (auto p : path) {
		vector<thread> threads;
		yes = 0;
		not_found = 0;
		start_time = clock();
		vector<vector<string>> query_files(THREAD_NUM);
		Util::LoadDirSpecific(query_files, QUERY_WAVE_PATH + p, "wav");
		for (int i = 0; i < THREAD_NUM; i++)
			threads.push_back(thread(SearchOneFile, query_files[i]));
		for (int i = 0; i < THREAD_NUM; i++)
			threads[i].join();
		end_time = clock();
		cout << "Bps: " << p << endl;
		cout << "Yes: " << yes << endl;
		cout << "Not found: " << not_found << endl;
		cout << "Time: " << (double)(end_time - start_time) / CLOCKS_PER_SEC << endl;
	}
}

int main() {
	// Training process.
	// Training();

	// Testing process.
	// Testing();

	// Fingerprinting();

	// Searching();
	return 0;
}
