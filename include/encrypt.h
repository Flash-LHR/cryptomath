#ifndef ENCRYPT_H_
#define ENCRYPT_H_

#include <cinttypes>
#include <cstdio>
#include <cstdlib>

#define FILE_NAME_SIZE 128
#define ANF_FILE "r%.2d.anf"
#define TEST_VECTOR_FILE "r%.2d.tv"
#define SAGE_FILE "r%.2d.sage"
#define CNF_FILE "r%.2d.cnf"
#define KEY_FILE "r%.2d.key"

#define LCS16(val, bit) ((val << bit) | (val >> (16U - bit)))

void generateRK(uint64_t key, int round, uint16_t RK[]);
uint32_t encryptWithProbe(uint32_t plain, int round, uint16_t RK[],
                          uint16_t L[], uint16_t R[], uint16_t* T[4]);
uint32_t encrypt(uint32_t plain, int round, uint16_t RK[]);
#endif  // ENCRYPT_H_