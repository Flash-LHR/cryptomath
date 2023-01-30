#ifndef SOLVE_CNF_H_
#define SOLVE_CNF_H_

#include <assert.h>
#include <cryptominisat5/cryptominisat.h>

#include <cstdio>
#include <vector>

#include "encrypt.h"

using std::vector;
using namespace CMSat;

int solveCNF(int round, uint64_t *key);

#endif  // SOLVE_CNF_H