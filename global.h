#pragma once
#include <bitset>
#include <string>
#include <vector>

#define ONE_BIT_SEARCH
#define TWO_BIT_SEARCH

class MusicInfo {
public:
	int id;
	int i_frame;
	MusicInfo(int ID, int FID) :id(ID), i_frame(FID){};
	MusicInfo(){};
};
typedef std::vector<std::pair<unsigned int, MusicInfo>> IndexType;

const int FRAME_LENGTH = 21;
const int DATABASE_SIZE = 174000;
const double TIME_INTERVAL = 0.0116;
const double BIT_ERROR_RATE = 0.2;
const double MUST_RIGHT = 0.1;
const int THREAD_NUM = 10;
const int OUTPUT_THREAD = 10;
const int SUB_FINGER_NUM = 380000; // 380000, there are 186056 subfingerprints in 90408 with 23.2
// Hop 11.6ms, 82 frames: 765 for 10 seconds, 327 for 5 seconds.
// Hop 11.6ms, 41 frames: 804 for 10 seconds.
// Hop 11.6ms, 21 frames: 828 for 10 seconds.
const int QUERY_FINGER_NUM = 828;

const std::string WAVE_ROOTPATH = "E:\\yangguang\\cvaf\\data\\tmp";
const std::string FINGER_ROOTPATH = "E:\\yangguang\\cvaf\\data\\fingers21";
const std::string QUERY_WAVE_PATH = "E:\\yangguang\\cvaf\\data\\query\\32kbps\\small";
const std::string INDEX_FILE_PATH = "E:\\yangguang\\cvaf\\data\\index\\20k.index";
const std::string WHOLE_FINGER_PATH = "E:\\yangguang\\cvaf\\data\\index\\20k.finger";