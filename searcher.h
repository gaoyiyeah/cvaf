#include <vector>
#include <string>
#include <bitset>
#include "global.h"

class Searcher {
private:
	FILE* fp;
	std::vector<std::string> allFiles;
	int _insert_one_item(unsigned int key, MusicInfo& m);
	int _build_one_file_index(const std::string filepath);
	int _inner_search(unsigned long item, std::bitset<32>* finger_block,
		int block_size, int i, int& temp_dif);
	long long _binary_search(unsigned int key);
	int _loadFingerFromOneFile(std::string filepath_prefix, unsigned int fileNum);
	int _outputFingerToOneFile(std::string filepath_prefix,
		unsigned int databaseSize, unsigned int fileNum);

public:
	IndexType index;
	std::vector<std::vector<std::bitset<32>>> finger_database;
	Searcher(){};
	int build_index(std::string dirPath);
	int search(std::bitset<32>* finger_block, int size, int& temp_diff);
	int compare_bitsets(int id, std::bitset<32>* finger_block, int block_size,
		int i_frame_in_block, int i_frame_in_file);
	int LoadIndex(std::string filepath);
	int LoadFingerDatabase(std::string filepath);
	int OutputIndexToFile(std::string filepath);
	int OutputFingerToFile(std::string filepath);
	int Clear();
};