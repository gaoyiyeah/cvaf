#include <bitset>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include "filter-training.h"
#include "fingerprint-extractor.h"
#include "global.h"
#include "util.h"

using namespace std;

// 33 binds.
const double freq_bind[] =
{ 
	300.000, 317.752, 336.554, 356.469, 377.563,
	399.904, 423.568, 448.632, 475.178, 503.296,
	533.078, 564.622, 598.032, 633.419, 670.901,
	710.600, 752.648, 797.185, 844.357, 894.320,
	947.240, 1003.29, 1062.66, 1125.54, 1192.14,
	1262.68, 1337.40, 1416.54, 1500.36, 1589.14,
	1683.17, 1782.77, 1888.27, 2000.00
}; // Bands in [0, 32]

void FingerprintExtractor::CreateImage(const string& filepath) {
	this->_wavepath = filepath;
	_wp.OpenWaveFile(_wavepath.c_str());
	_wp.MakeTargetSamplesData();
	unsigned long all_time_data_size = 0;
	_wp.GetSamplesVector(_all_time_data, all_time_data_size);
	_wp.CloseWaveFile();
	_Energying(all_time_data_size);
	return ;
}

void FingerprintExtractor::CalcFingerprint(const string& filepath,
	vector<Filter>& filters) {
	this->CreateImage(filepath);
	for (int frame_idx = 0; frame_idx < _frame_number; frame_idx++) {
		for (size_t i = 0; i < filters.size(); i++) {
			double sign = filters[i].GetEnergy(_energy, frame_idx) - filters[i].threshold;
			if (sign > 0)
				_fingers[frame_idx][i] = '1';
			else
				_fingers[frame_idx][i] = '0';
		}
	}
}

void FingerprintExtractor::GetSamples(vector<Sample>* samples) {
	samples->clear();
	int frame_idx = 0;
	// Get a sample every 100 frames.
	for (int start_pos = 0; start_pos < _frame_number; start_pos += 100) {
		int song_id = this->GetFileId();
		Sample sample(song_id, frame_idx++);
		for (int i = 0; i < FRAME_LENGTH; i++) {
			for (int j = 0; j < BINDS_NUM; j++) {
				sample.image[i][j] = _energy[i + start_pos][j];
			}
		}
		samples->push_back(sample);
	}
	return ;
}

void FingerprintExtractor::GetQueryFinger(bitset<32>* new_finger, int& size) {
	size = _frame_number;
	for (int i = 0; i < _frame_number; i++) {
		bitset<32> item(_fingers[i]);
		new_finger[i] = item;
	}
	return;
}

int FingerprintExtractor::_Energying(long all_time_data_size) {
	memset(_energy, 0, sizeof(double)* QUERY_FINGER_NUM * BINDS_NUM);
	_frame_number = 0;
	int start = 0;
	int jump_samples = (int)(sampleRate * TIME_INTERVAL);

	while (start + NumSamplesPerFrameM < all_time_data_size) {
		short time_data[1850];
		cpxv_t freq_data[2048];
		double bind_energy[BINDS_NUM];
		memset(bind_energy, 0, sizeof(double)* BINDS_NUM);
		for (int i = 0; i < NumSamplesPerFrameM; i++) {
			time_data[i] = _all_time_data[i + start];
		}

		DoFFT(time_data, freq_data);
		double point_freq = 0;
		for (int j = 0; j < NumBinsInFftWinM; j++) {
			//FFT结果第n个点代表的频率值
			point_freq = (j + 1) * sampleRate / NumBinsInFftWinM;
			if (point_freq < 300 || point_freq > 2000) {
				continue;
			} else {
				int bind = _SelectBind(point_freq); // [0,32]
				bind_energy[bind] += (freq_data[j].re * freq_data[j].re + freq_data[j].im * freq_data[j].im);
			}
		}
		for (int i = 0; i < BINDS_NUM; i++)
			_energy[_frame_number][i] = log(bind_energy[i] + 1); // Get log after sum up all the energies.

		//下一帧
		_frame_number++;
		start += jump_samples;
	}
	_frame_number -= FRAME_LENGTH;
	return _frame_number;
}

int FingerprintExtractor::_SelectBind(double point_freq) {
	int start = 0;
	int end = BINDS_NUM;
	while (start <= end) {
		int mid = start + (end - start) / 2;
		if (point_freq < freq_bind[mid])
			end = mid - 1;
		else if (point_freq > freq_bind[mid + 1])
			start = mid + 1;
		else
			return mid;
	}
	return -1;
}

int FingerprintExtractor::GetFrameNumber() {
	return _frame_number;
}

int FingerprintExtractor::GetFileId() {
	string originFile = _wavepath.substr(_wavepath.find_last_of("\\") + 1, _wavepath.size());
	return stoi(originFile.substr(0, originFile.find(".")));
}

void FingerprintExtractor::PrintFingerToFile(const string& fingerFile) {
	FILE *fp = fopen(fingerFile.c_str(), "w");
	string sub_finger;
	for (int i = 0; i < _frame_number; i++) {
		sub_finger = "";
		for (int j = 0; j < 32; j++)
			sub_finger.push_back(_fingers[i][j]);
		bitset<32> b(sub_finger);
		fprintf(fp, "%lu\n", b.to_ulong());
	}
	fclose(fp);
	return;
}