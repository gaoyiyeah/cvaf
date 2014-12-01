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

extern double duration_get_energy;
extern int hit_size;

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
	cerr << "Extract done." << endl;
}

void SearchOneFile(vector<string>& allQueryFiles) {
	FingerprintExtractor extractor;
	for (int i = 0; i < (signed)allQueryFiles.size(); i++) {
		cout<<allQueryFiles[i]<<endl;
		extractor.CalcFingerprint(allQueryFiles[i], filters);
		bitset<32> finger_block[QUERY_FINGER_NUM];
		int size = 0;
		int queryId = extractor.GetFileId();
		extractor.getQueryFinger(finger_block, size);
		//extractor.PrintFingerToFile(QUERY_WAVE_PATH + "\\" + to_string(queryId) + ".txt");
		
		int result = searcher.search(finger_block, size);

		if (result == -1) {
			cout<<"file: "<<queryId<<" Not found"<<endl;
			not_found++;
		} else if (result == queryId) {
			cout<<"Match!  "<<result<<endl;
			yes++;
		} else {
			cout << queryId << "\t" << result << endl;
		}
	}
}

void PrintFilters(vector<Filter>& filters) {
	ofstream fout;
	fout.open("E:\\yangguang\\cvaf\\data\\tmp.txt", ios::out);
	fout << "Type\tTime end\tBand start\tBand end\t" << endl;
	for (const auto& f : filters)
		fout << f.type << "\t" << f.time_end << "\t" << f.band_start << "\t" << f.band_end << endl;
	fout.close();
}

int main() {
	time_t start, end;
	FilterTraining ft;
	string original_wav = "E:\\yangguang\\cvaf\\data\\training\\original_wav";
	string degraded_wav = "E:\\yangguang\\cvaf\\data\\training\\32kbps_wav";
	filters = ft.LoadFilters("E:\\yangguang\\cvaf\\data\\filters.dat");
	/*
	filters = ft.Training(original_wav, degraded_wav);
	PrintFilters(filters);
	ft.PringFiltersToFile("E:\\yangguang\\cvaf\\data\\filters.dat");
	cerr << "Print filters done!" << endl;
	cout << "Time: " << duration << endl;
	getchar();
	*/
	/*
	start = clock();
	vector<string> allFiles = Util::load_dir(WAVE_ROOTPATH, "wav");
	vector<vector<string>> allQueryFiles(THREAD_NUM);
	for (int i = 0; i < allFiles.size(); i++)
		allQueryFiles[i%THREAD_NUM].push_back(allFiles[i]);
	vector<thread> threads;
	for (int i = 0; i < THREAD_NUM; i++)
		threads.push_back(thread(ExtractFingerprint, allQueryFiles[i]));
	for (int i = 0; i < THREAD_NUM; i++)
		threads[i].join();
	
	cout << "Extract Done!" << endl;
	end = clock();
	cout << "Time: " << (end - start) / CLOCKS_PER_SEC << endl;
	getchar();
	*/

	searcher.build_index(FINGER_ROOTPATH);
	start = clock();
	vector<string> query_files = Util::load_dir(QUERY_WAVE_PATH, "wav");
	SearchOneFile(query_files);
	end = clock();
	cout << "Yes: " << yes << endl;
	cout << "Time: " << (double)(end - start) / CLOCKS_PER_SEC << endl;
	cout << "Time for getting energy: " << duration_get_energy << endl;
	cout << "Hit size: " << hit_size << endl;
	getchar();
	return 0;
}
