#include <bitset>
#include <map>
#include <string>
#include <vector>
#include "global.h"

class Searcher {
public:
	Searcher(){};
	std::vector<std::vector<std::bitset<32>>> finger_database;
	int BuildIndex(std::string dirPath);
	int Search(int queryId, std::bitset<32>* finger_block, int size);
	int LoadIndex(std::string filepath);
	int LoadFingerDatabase(std::string filepath);
	int OutputIndexToFile(std::string filepath);
	int OutputFingerToFile(std::string filepath);
	int Clear();
	void DoStatistics();
private:
	IndexType _index;
	int _InsertOneItem(unsigned int key, MusicInfo& m);
	int _BuildOneFileIndex(const std::string filepath);
	int _InnerSearch(int queryId, unsigned long item, std::bitset<32>* finger_block,
		int block_size, int i, std::map<double, int>*);
	int _LoadFingerFromOneFile(std::string filepath_prefix, unsigned int fileNum);
	int _OutputFingerToOneFile(std::string filepath_prefix,
		unsigned int databaseSize, unsigned int fileNum);
	long long _BinarySearch(unsigned int key);
	double _CompareBitsets(int id, std::bitset<32>* finger_block, int block_size,
		int i_frame_in_block, int i_frame_in_file);
};