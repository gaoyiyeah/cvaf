#include <ctime>
#include <iostream>
#include <vector>
#include "filter.h"
#include "filter-training.h"

using namespace std;

int main() {
	time_t start, end;
	start = clock();
	FilterTraining ft;
	string original_wav = "E:\\yangguang\\cvaf\\data\\training\\original_wav";
	string degraded_wav = "E:\\yangguang\\cvaf\\data\\training\\32kbps_wav";
	vector<Filter> filters = ft.Training(original_wav, degraded_wav);
	/*
	int i = 0;
	for (const auto& f : filters) {
		cout << i << endl;
		cout << "Type: " << f.type << "\tTime end: " << f.time_end << endl;
		cout << "Band start: " << f.band_start << "\tBand end: " << f.band_end << endl;
		cout << endl;
		i++;
	}
	*/
	end = clock();
	cout << "Time: " << (end - start) / CLOCKS_PER_SEC << endl;
	cout << "Hello, world!" << endl;
	getchar();
	return 0;
}
