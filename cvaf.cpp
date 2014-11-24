#include <iostream>
#include "filter.h"
#include "filter-training.h"

using namespace std;

int main() {
	FilterTraining ft;
	string original_wav = "E:\\yangguang\\cvaf\\data\\training_samples\\original_wav";
	string degraded_wav = "E:\\yangguang\\cvaf\\data\\training_samples\\32kbps_wav";
	ft.PrepareSamples(original_wav, degraded_wav);
	cout << "Hello, world!" << endl;
	return 0;
}

