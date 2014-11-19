#include <iostream>
#include "filter.h"
#include "filter-training.h"

using namespace std;

int main() {
	FilterTraining ft;
	ft.GenerateFilter();
	cout << ft.filters.size() << endl;
	cout << "Hello, world!" << endl;
	return 0;
}

