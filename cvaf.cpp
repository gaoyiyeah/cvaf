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
extern int hit_number;
extern int list_contain_id;
extern long long list_len;
extern int sub_fingerprint;

void ExtractFingerprint(vector<string>& all_files) {
	FingerprintExtractor extractor;
	for (const string& f : all_files) {
		size_t pos = f.find_last_of("\\");
		string tmp = f.substr(pos + 1, f.size() - pos);
		string filename = tmp.substr(0, tmp.find("."));
		if (stoi(filename) > 30200)
			break;
		extractor.CalcFingerprint(f, filters);
		extractor.PrintFingerToFile(FINGER_ROOTPATH + "\\" + filename + ".txt");
		cerr << "File: " << f << " done." << endl;
	}
	cerr << "Extract done." << endl;
}

void SearchOneFile(vector<string>& allQueryFiles) {
	FingerprintExtractor extractor;
	for (int i = 0; i < (signed)allQueryFiles.size(); i++) {
		extractor.CalcFingerprint(allQueryFiles[i], filters);
		bitset<32> finger_block[QUERY_FINGER_NUM];
		int size = 0;
		int queryId = extractor.GetFileId();
		cout << queryId << endl;
		extractor.getQueryFinger(finger_block, size);		
		int result = searcher.Search(queryId, finger_block, size);
		if (result == -1) {
			//cout<<"file: "<<queryId<<" Not found"<<endl;
			not_found++;
		} else if (result == queryId) {
			//cout<<"Match!  "<<result<<endl;
			yes++;
		} else {
			cout << queryId << "\t" << result << "\t" << endl;
		}
	}
}

void PrintFilters(vector<Filter>& filters) {
	ofstream fout;
	fout.open("E:\\yangguang\\cvaf\\data\\filters.txt", ios::out);
	fout << "Type\tTime end\tBand start\tBand end\tError rate" << endl;
	for (const auto& f : filters)
		fout << f.id << "\t\t" << f.type << "\t\t" << f.time_end << "\t\t"
			<< f.band_start << "\t\t" << f.band_end << "\t\t" << f.error_rate
			<< endl;
	fout.close();
}

int main() {
	time_t start, end;
	FilterTraining ft;
	vector<thread> threads;
	//ft.GetDistribution();
	//getchar();
	filters = ft.LoadFilters("E:\\yangguang\\cvaf\\data\\filters.dat");
	/*
	string original_wav = "E:\\yangguang\\cvaf\\data\\training\\original";
	string degraded_wav = "E:\\yangguang\\cvaf\\data\\training\\recorded";
	//ft.TestClassifier(original_wav, degraded_wav, filters);
	//getchar();
	
	start = clock();
	filters = ft.Training(original_wav, degraded_wav);
	PrintFilters(filters);
	ft.PringFiltersToFile("E:\\yangguang\\cvaf\\data\\filters.dat");
	end = clock();
	cerr << "Print filters done!" << endl;
	cout << "Time: " << (double)(end - start) / CLOCKS_PER_SEC << endl;
	getchar();
	*/
	/*
	start = clock();
	vector<string> allFiles = Util::load_dir(WAVE_ROOTPATH, "wav");
	vector<vector<string>> allQueryFiles(THREAD_NUM);
	for (int i = 0; i < allFiles.size(); i++)
		allQueryFiles[i%THREAD_NUM].push_back(allFiles[i]);
	for (int i = 0; i < THREAD_NUM; i++)
		threads.push_back(thread(ExtractFingerprint, allQueryFiles[i]));
	for (int i = 0; i < THREAD_NUM; i++)
		threads[i].join();
	
	cout << "Extract Done!" << endl;
	end = clock();
	cout << "Time: " << (end - start) / CLOCKS_PER_SEC << endl;
	getchar();
	*/
	start = clock();
	searcher.BuildIndex(FINGER_ROOTPATH);
	searcher.DoStatistics();
	getchar();
	//searcher.OutputIndexToFile(INDEX_FILE_PATH);
	//searcher.OutputFingerToFile(WHOLE_FINGER_PATH);
	//searcher.LoadIndex(INDEX_FILE_PATH);
	//searcher.LoadFingerDatabase(WHOLE_FINGER_PATH);
	end = clock();
	cout << "Load index: " << (double)(end - start) / CLOCKS_PER_SEC << endl;
	hit_number = 0;
	yes = 0;
	not_found = 0;
	start = clock();
	vector<vector<string>> query_files(THREAD_NUM);
	Util::load_dir_specific(query_files, QUERY_WAVE_PATH, "wav");
	for (int i = 0; i < THREAD_NUM; i++)
		threads.push_back(thread(SearchOneFile, query_files[i]));
	for (int i = 0; i < THREAD_NUM; i++)
		threads[i].join();
	end = clock();
	cout << "Threshold: " << BIT_ERROR_RATE << endl;
	cout << "Yes: " << yes << endl;
	cout << "Not found: " << not_found << endl;
	cout << "Time: " << (double)(end - start) / CLOCKS_PER_SEC << endl;
	cout << "Hit number: " << hit_number << endl;
	cout << "Sub-fingerprint: " << sub_fingerprint << endl;
	cout << "List len: " << list_len << endl;
	cout << "List contains id: " << list_contain_id << endl;
	getchar();
	return 0;
}
