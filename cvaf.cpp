#include <iostream>
#include "filter.h"
#include "filter-training.h"

using namespace std;

int main() {
	FilterTraining ft;
	string original_wav = "E:\\yangguang\\cvaf\\data\\training\\original_wav";
	string degraded_wav = "E:\\yangguang\\cvaf\\data\\training\\32kbps_wav";
	ft.GenerateFilter();
	ft.PrepareSamples(original_wav, degraded_wav);
	ft.CalculateThreshold();
	cout << "Hello, world!" << endl;
	return 0;
}
