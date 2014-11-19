#include <iostream>
#include <vector>
#include "Filter.h"

class FilterTraining {
public:
	std::vector<Filter> filters;
	void GenerateFilter();
};