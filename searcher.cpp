#include <algorithm>
#include <cmath>
#include <ctime>
#include <bitset>
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>
#include "global.h"
#include "searcher.h"
#include "util.h"

using namespace std;

int hit_number = 0;

bool comp(pair<unsigned int, MusicInfo> a,
	pair<unsigned int, MusicInfo> b) {
	return a.first != b.first ?
		a.first < b.first : a.second.id < b.second.id;
}

int Searcher::build_index(string dirPath) {
	finger_database.clear();
	finger_database.resize(DATABASE_SIZE);
	time_t sort_start, sort_end;
	allFiles = Util::load_dir(FINGER_ROOTPATH, "txt");
	for (int i = 0; i < (signed)allFiles.size(); i++) {
		if (i % 1000 == 0)
			cout << "Index: " << i << endl;
		_build_one_file_index(allFiles[i]);
	}

	//cout<<"index size: "<<index.size()<<endl;
	sort_start = clock();
	sort(index.begin(), index.end(), comp);

	sort_end = clock();
	double sort_time = (double)(sort_end - sort_start) / CLOCKS_PER_SEC;
	cout << "Sort time: " << sort_time << endl;
	return 0;
}

int Searcher::_build_one_file_index(const string filepath) {
	vector<unsigned int> audio_file;
	Util::load_one_file(filepath, audio_file);
	string originFile = filepath.substr(filepath.find_last_of("\\") + 1, filepath.find_last_of("."));
	int finger_id = stoi(originFile);
#ifdef SUB_SAMPLING
	vector<bitset<32>> temp_v = Util::VectorIntToVectorBitset(audio_file);
	vector<bitset<32>> fingers_block;
	for (int i = 0; i < temp_v.size(); i++)	{
		if (i % M == 0)
			fingers_block.push_back(temp_v[i]);
	}
	finger_database[finger_id] = fingers_block;
#else
	finger_database[finger_id] = Util::VectorIntToVectorBitset(audio_file);
#endif
	MusicInfo m(finger_id, 0);
	int i = 0;

#ifdef SUB_SAMPLING
	for (i = 0; i < (signed)audio_file.size(); i++) {
		if (i % M == 0)	{
			m.i_frame = i / M;
			_insert_one_item(audio_file[i], m);
		}
	}
#else
	for (i = 0; i < (signed)audio_file.size(); i++)	{
		m.i_frame = i;
		_insert_one_item(audio_file[i], m);
	}
#endif
	return i;
}

int Searcher::_insert_one_item(unsigned int key, MusicInfo& m) {
	if (key == 0)
		return 0;
	index.push_back(make_pair(key, m));
	return 0;
}

int Searcher::search(bitset<32>* finger_block, const int block_size) {
	std::map<double, int> result_map;
	//exact match
	//从查询指纹块的第一条指纹开始
	for (int i = 0; i < block_size; i++) {
		unsigned long key = finger_block[i].to_ulong();
		if (key == 0)
			continue;
		int result = _inner_search(key, finger_block, block_size, i, &result_map);
		if (result > 0)
			return result;
	}

#ifdef ONE_BIT_SEARCH
	for (int i = 0; i < block_size; i++) {
		for (int j = 0; j < 32; j++) {
			bitset<32> item = finger_block[i];
			item.flip(j);
			unsigned long key = item.to_ulong();
			if (key == 0)
				continue;
			int result = _inner_search(key, finger_block, block_size, i, &result_map);
			if (result > 0)
				return result;
		}
	}
#endif

#ifdef TWO_BIT_SEARCH
	for (int i = 0; i < block_size; i++) {
		for (int j = 0; j < 31; j++) {
			for (int k = j + 1; k < 32; k++) {
				bitset<32> item = finger_block[i];
				item.flip(j);
				item.flip(k);
				unsigned long key = item.to_ulong();
				if (key == 0)
					continue;
				_inner_search(key, finger_block, block_size, i, &result_map);

			}
		}
	}
#endif

#ifdef THREE_BIT_SEARCH
	for (int i = 0; i < block_size; i++) {
		for (int j = 0; j < 30; j++)	{
			for (int k = j + 1; k < 31; k++)	{
				for (int m = k + 1; m < 32; m++)	{
					bitset<32> item = finger_block[i];
					item.flip(j);
					item.flip(k);
					item.flip(m);
					unsigned long key = item.to_ulong();
					if (key == 0)
						continue;
					result = _inner_search(key, finger_block, block_size, i, temp_dif);
					if (result != -1)
						return result;
				}
			}
		}
	}
#endif
	if (result_map.size() > 0) {
		//cerr << result_map.begin()->first << endl;
		return result_map.begin()->second;
	}
	else
		return -1;
}

int Searcher::_inner_search(unsigned long key, bitset<32>* finger_block,
	const int block_size, const int i, map<double, int>* result_map) {
	time_t search_start, search_end;
	search_start = clock();
	long long result = _binary_search(key);
	if (result == -1)
		return -1;
	long long start = result;
	long long end = result;
	do {
		start--;
	} while (start >= 0 && index[start].first == key);
	start++;
	do {
		end++;
	} while (end < (signed)index.size() && index[end].first == key);
	end--;
	search_end = clock();
	//duration_search += (double)(search_end - search_start) / CLOCKS_PER_SEC;
	for (long long iter = start; iter <= end; iter++) {
		double diffbits = compare_bitsets(index[iter].second.id, finger_block,
			block_size, i, index[iter].second.i_frame);
		hit_number++;
		if (diffbits <= BIT_ERROR_RATE) {
			//cerr << diffbits << endl;
			return index[iter].second.id;
			(*result_map)[diffbits] = index[iter].second.id;
			/*
			if (diffbits <= MUST_RIGHT) {
				return index[iter].second.id;
			}
			*/
		}
	}
	return -1;
}

/*
*i_frame_in_block: 在query指纹块中命中的index
*i_frame_in_file: 在file中命中的index
*/
double Searcher::compare_bitsets(int id, bitset<32>* finger_block, const int block_size, \
	const int i_frame_in_block, int i_frame_in_file) {
	if (i_frame_in_file - i_frame_in_block < 0)
		return INT_MAX;//表示错误，返回1.0
	int diff_bits = 0;
	vector<bitset<32>>& full_audio_fingers = finger_database[id];

	if (i_frame_in_file + block_size - i_frame_in_block > full_audio_fingers.size())
		return INT_MAX;
	i_frame_in_file -= i_frame_in_block;

	//time_compare_start = clock();

	for (int i = 0; i < block_size; i++) {
		bitset<32> subfinger_xor = finger_block[i] ^ full_audio_fingers[i_frame_in_file];
		i_frame_in_file++;
		diff_bits += (int)subfinger_xor.count();
	}

	//time_compare_finish = clock();
	//duration_compare += (double)(time_compare_finish - time_compare_start)/CLOCKS_PER_SEC;
	return (double)diff_bits / (32 * QUERY_FINGER_NUM);
}

long long Searcher::_binary_search(unsigned int key) {
	long long start = 0;
	long long end = index.size() - 1;
	long long mid;
	while (end >= start) {
		mid = start + (end - start) / 2;
		if (key < index[mid].first)
			end = mid - 1;
		else if (key > index[mid].first)
			start = mid + 1;
		else
			return mid;
	}
	return -1;
}

int Searcher::LoadIndex(string filepath) {
	index.clear();
	unsigned int index_size = 0;
	ifstream fin(filepath, ios::in | ifstream::binary);
	fin.read(reinterpret_cast<char *>(&index_size), sizeof(int));
	if (index_size == 0) {
		fin.close();
		return -1;
	}
	index.resize(index_size);
	fin.read(reinterpret_cast<char *>(&index[0]), index.size() * sizeof(index[0]));
	fin.close();
	return 0;
}

int Searcher::_loadFingerFromOneFile(string filepath_prefix, unsigned int fileNum) {
	ifstream fin(filepath_prefix + to_string(fileNum), ios::in | ifstream::binary);
	int databaseSize = 0;
	fin.read(reinterpret_cast<char *>(&databaseSize), sizeof(databaseSize));
	finger_database.resize(databaseSize);
	unsigned int songID = 0;
	unsigned int fingerSize = 0;
	vector<unsigned int> iv;
	while (true) {
		fin.read(reinterpret_cast<char *>(&songID), sizeof(songID));
		if (fin.eof())
			break;
		fin.read(reinterpret_cast<char *>(&fingerSize), sizeof(fingerSize));
		if (fingerSize != 0) {
			iv.resize(fingerSize);
			fin.read(reinterpret_cast<char *>(&iv[0]), fingerSize * sizeof(iv[0]));
			finger_database[songID] = Util::VectorIntToVectorBitset(iv);
		}
	}
	fin.close();
	return 0;
}

int Searcher::LoadFingerDatabase(string filepath_prefix) {
	finger_database.clear();
	vector<thread> threads(OUTPUT_THREAD);
	for (int i = 0; i < OUTPUT_THREAD; i++) {
		threads[i] = thread(&Searcher::_loadFingerFromOneFile, this, filepath_prefix, i);
	}

	for (int i = 0; i < OUTPUT_THREAD; i++)	{
		threads[i].join();
	}
	return 0;
}

int Searcher::_outputFingerToOneFile(string filepath_prefix,
	unsigned int databaseSize, unsigned int fileNum) {
	ofstream fout(filepath_prefix + to_string(fileNum), ios::out | ofstream::binary);
	fout.write(reinterpret_cast<char *>(&databaseSize), sizeof(unsigned int));
	unsigned int fingerSize = 0;
	vector<unsigned int> intVector;
	for (unsigned int i = 0; i < databaseSize; i++) {
		if (i % OUTPUT_THREAD == fileNum) {
			intVector = Util::VectorBitsetToVectorInt(finger_database[i]);
			fingerSize = (unsigned int)intVector.size();
			fout.write(reinterpret_cast<char *>(&i), sizeof(i));
			fout.write(reinterpret_cast<char *>(&fingerSize), sizeof(fingerSize));
			if (fingerSize != 0)
				fout.write(reinterpret_cast<char *>(&intVector[0]), fingerSize * sizeof(intVector[0]));
		}
	}
	fout.close();
	return 0;
}

int Searcher::OutputFingerToFile(string filepath_prefix) {
	unsigned int databaseSize = (unsigned int)finger_database.size();
	vector<thread> threads(OUTPUT_THREAD);

	for (int i = 0; i < OUTPUT_THREAD; i++)	{
		threads[i] = thread(&Searcher::_outputFingerToOneFile, this, filepath_prefix, databaseSize, i);
	}

	for (int i = 0; i < OUTPUT_THREAD; i++)	{
		threads[i].join();
	}
	return 0;
}

int Searcher::OutputIndexToFile(string filepath) {
	ofstream fout(filepath, ios::out | ofstream::binary);
	size_t index_size = index.size();
	fout.write(reinterpret_cast<char *>(&index_size), sizeof(int));
	if (index_size == 0) {
		fout.close();
		return -1;
	}
	fout.write(reinterpret_cast<char *>(&index[0]), index.size() * sizeof(index[0]));
	fout.close();
	return 0;
}

int Searcher::Clear() {
	index.clear();
	finger_database.clear();
	return 0;
}
