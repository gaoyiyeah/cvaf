#pragma once
#include <bitset>
#include <iostream>
#include <string>
#include <vector>
#include "fft.h"
#include "filter.h"
#include "filter-training.h"
#include "global.h"
#include "wave-processor.h"

extern const int NumBinsInFftWinM; // 2048
extern const int NumSamplesPerFrameM; //1850
const double frameInterval = 0.37;
const int sampleRate = 5000;

class FingerprintExtractor {
public:
	FingerprintExtractor() : _wp(sampleRate){};
	void CreateImage(const std::string& filepath);
	void GetSamples(std::vector<Sample>* samples);
	void CalcFingerprint(const std::string& filepath,
		std::vector<Filter>& filters);
	void GetQueryFinger(bitset<32>* new_finger, int& size);
	void PrintFingerToFile(const std::string& filepath);
	int GetFrameNumber();
	int GetFileId();

private:
	std::string _wavepath;
	WaveProcessor _wp;
	int _SelectBind(double point_freq);
	int _Energying(long all_time_data_size);
	
	int _frame_number; // The total number of frames in the wave file.
	short _all_time_data[SamplesVectorSize];
	// energy[n,m] indicates the energy in nth frame and mth frequency band.
	double _energy[SUB_FINGER_NUM][BINDS_NUM];
	char _fingers[SUB_FINGER_NUM][32]; // final fingerprint
};