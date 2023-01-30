#ifndef GENERATE_SAGE_H_
#define GENERATE_SAGE_H_

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "encrypt.h"
using std::ifstream;
using std::ios;
using std::ofstream;
using std::sort;
using std::string;
using std::stringstream;
using std::unique;
using std::vector;

int generateSage(int round);

#endif  // GENERATE_SAGE_H_