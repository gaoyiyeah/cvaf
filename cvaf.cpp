#include <ctime>
#include <iostream>
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
		
		int tmp_dif = 0;
		int result = searcher.search(finger_block, size);

		if (result == -1) {
			cout<<"file: "<<queryId<<" Not found"<<endl;
			not_found++;
		} else if (result == queryId) {
			cout<<"Match!  "<<result<<endl;
			yes++;
		}
	}
}

int main() {
	time_t start, end;
	start = clock();
	FilterTraining ft;
	string original_wav = "E:\\yangguang\\cvaf\\data\\training\\original_wav";
	string degraded_wav = "E:\\yangguang\\cvaf\\data\\training\\32kbps_wav";
	filters = ft.LoadFilters("E:\\yangguang\\cvaf\\data\\filters.dat");
	//filters = ft.Training(original_wav, degraded_wav);
	//ft.PringFiltersToFile("E:\\yangguang\\cvaf\\data\\filters.dat");
	//cerr << "Print filters done!" << endl;
	//getchar();
	/*
	int i = 0;
	for (const auto& f : filters) {
		cout << i << endl;
		cout << "Type: " << f.type << "\tTime end: " << f.time_end << endl;
		cout << "Band start: " << f.band_start << "\tBand end: " << f.band_end << endl;
		cout << endl;
		i++;
	}
	
	vector<string> all_files = Util::load_dir(
		"E:\\yangguang\\cvaf\\data\\database", "wav");
	ExtractFingerprint(all_files);
	getchar();
	end = clock();
	cout << "Time: " << (end - start) / CLOCKS_PER_SEC << endl;
	getchar();
	*/

	searcher.build_index(FINGER_ROOTPATH);
	cout << searcher.index.size() << endl;
	vector<string> query_files = Util::load_dir(QUERY_WAVE_PATH, "wav");
	SearchOneFile(query_files);
	cout << "Hello, world!" << endl;
	getchar();
	return 0;
}
