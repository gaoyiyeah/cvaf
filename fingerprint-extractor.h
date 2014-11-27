#pragma once
#include <bitset>
#include <iostream>
#include <string>
#include <vector>
#include "fft.h"
#include "filter-training.h"
#include "global.h"
#include "wave-processor.h"

extern const int NumBinsInFftWinM; // 2048
extern const int NumSamplesPerFrameM; //1850
const double timeInterval = 0.0116;
const double frameInterval = 0.37;
const int sampleRate = 5000;

class ImageAnalyzer {
public:
	ImageAnalyzer(const std::string& filepath) : _wp(5000), _wavepath(filepath){};
	~ImageAnalyzer();
	void CreateImage();
	void GetSamples(std::vector<Sample>* samples);
	int GetFrameNumber();
	void SetWavepath(const std::string& filepath);
	int GetFileId();

private:
	std::string _wavepath;
	WaveProcessor _wp;
	int _SelectBind(double point_freq);
	int _Energying(long all_time_data_size);
	void _Fingerprinting();

	
	int _frame_number; // The total number of frames in the wave file.
	short _all_time_data[SamplesVectorSize];
	int _energy[SUB_FINGER_NUM][33]; // energy[n,m] indicates the energy in nth frame and mth frequency band.
};