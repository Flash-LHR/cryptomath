#include "generateANF.h"

FILE *gAnfFile;

uint16_t rand16() { return rand() % 0x10000; }

uint32_t rand32() { return ((uint32_t)rand16() << 16) | rand16(); }

uint64_t rand64() { return ((uint64_t)rand32() << 32) | rand32(); }

void generateKeyANF(int round) {
  if (round < 8) {
    for (int i = 0; i < round; i++) {
      for (int j = 0; j < 16; j++) {
        if (i & 1)
          fprintf(gAnfFile, "rk%.2d_%.2d + rk%.2d_%.2d + 1\n", i, j, i - 1, j);
        else
          fprintf(gAnfFile, "rk%.2d_%.2d + k%.2d\n", i, j, ((i >> 1) << 4) + j);
      }
    }
  } else {
    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 16; j++) {
        if (i & 1)
          fprintf(gAnfFile, "rk%.2d_%.2d + rk%.2d_%.2d + 1\n", i, j, i - 1, j);
        else
          fprintf(gAnfFile, "rk%.2d_%.2d + k%.2d\n", i, j, ((i >> 1) << 4) + j);
      }
    }
    for (int i = 8; i < round; i++) {
      for (int j = 0; j < 16; j++) {
        if (i & 1)
          fprintf(gAnfFile, "rk%.2d_%.2d + rk%.2d_%.2d + 1\n", i, j, i - 1, j);
        else
          fprintf(gAnfFile, "rk%.2d_%.2d + rk%.2d_%.2d + %.2d + 1\n", i, j,
                  i - 8, j, ((i - 8) >> 1) & 1);
      }
    }
  }
}

void generateTextANF(int testID, int round, uint32_t plain, uint32_t cipher) {
  for (int i = 0; i < 32; i++) {
    fprintf(gAnfFile, "test%.2d_p%.2d + %.2d\n", testID, i, (plain >> i) & 1);
    fprintf(gAnfFile, "test%.2d_c%.2d + %.2d\n", testID, i, (cipher >> i) & 1);
  }
  for (int i = 0; i < 16; i++) {
    fprintf(gAnfFile, "test%.2d_l00_%.2d + test%.2d_p%.2d\n", testID, i, testID,
            i);
    fprintf(gAnfFile, "test%.2d_r00_%.2d + test%.2d_p%.2d\n", testID, i, testID,
            i + 16);
    fprintf(gAnfFile, "test%.2d_l%.2d_%.2d + test%.2d_c%.2d\n", testID, round,
            i, testID, i);
    fprintf(gAnfFile, "test%.2d_r%.2d_%.2d + test%.2d_c%.2d\n", testID, round,
            i, testID, i + 16);
  }
}

void generateRoundANF(int testID, int round) {
  // T_{round,2} 的边界情况
  fprintf(gAnfFile, "test%.2d_t%.2d_2_00 + test%.2d_t%.2d_1_00\n", testID,
          round, testID, round);
  fprintf(gAnfFile, "test%.2d_t%.2d_2_01 + test%.2d_t%.2d_1_01\n", testID,
          round, testID, round);
  for (int j = 0; j < 16; j++) {
    // T_{round,1} = (RK_{round-1} & L_{round-1}) xor R_{round-1} xor (round -
    // 1)
    fprintf(gAnfFile,
            "test%.2d_t%.2d_1_%.2d + rk%.2d_%.2d * test%.2d_l%.2d_%.2d + "
            "test%.2d_r%.2d_%.2d + %.2d\n",
            testID, round, j, round - 1, j, testID, round - 1, j, testID,
            round - 1, j, ((round - 1) >> j) & 1);
    // T_{round,2} = ((T_{round,1} << 2) & (T_{round,1} << 1)) xor T_{round,1}
    if (j > 1) {
      fprintf(gAnfFile,
              "test%.2d_t%.2d_2_%.2d + test%.2d_t%.2d_1_%.2d * "
              "test%.2d_t%.2d_1_%.2d + "
              "test%.2d_t%.2d_1_%.2d\n",
              testID, round, j, testID, round, j - 2, testID, round, j - 1,
              testID, round, j);
    }
    // T_{round,3} = (T_{round,2} <<< 3) xor (T_{round,2} <<< 9) xor
    // (T_{round,2} <<< 14)
    fprintf(
        gAnfFile,
        "test%.2d_t%.2d_3_%.2d + test%.2d_t%.2d_2_%.2d + test%.2d_t%.2d_2_%.2d "
        "+ "
        "test%.2d_t%.2d_2_%.2d\n",
        testID, round, j, testID, round, (j - 3 + 16) % 16, testID, round,
        (j - 9 + 16) % 16, testID, round, (j - 14 + 16) % 16);
    // L_{round} = R_{round-1} xor (RK_{round-1} & T_{round,3})
    fprintf(gAnfFile,
            "test%.2d_l%.2d_%.2d + test%.2d_r%.2d_%.2d + rk%.2d_%.2d * "
            "test%.2d_t%.2d_3_%.2d\n",
            testID, round, j, testID, round - 1, j, round - 1, j, testID, round,
            j);
    // R_{round} = L_{round-1} xor T_{round,3}
    fprintf(
        gAnfFile,
        "test%.2d_r%.2d_%.2d + test%.2d_l%.2d_%.2d + test%.2d_t%.2d_3_%.2d\n",
        testID, round, j, testID, round - 1, j, testID, round, j);
  }
}

int generateANF(int testCount, int round, uint64_t *key) {
  char anfFileName[FILE_NAME_SIZE], testVectorFileName[FILE_NAME_SIZE];
  snprintf(anfFileName, FILE_NAME_SIZE, ANF_FILE, round);
  snprintf(testVectorFileName, FILE_NAME_SIZE, TEST_VECTOR_FILE, round);
  gAnfFile = fopen(anfFileName, "w");
  FILE *testVectorFile = fopen(testVectorFileName, "w");
  if (gAnfFile == NULL || testVectorFile == NULL) {
    if (gAnfFile != NULL) fclose(gAnfFile);
    if (testVectorFile != NULL) fclose(testVectorFile);
    fprintf(stderr, "open file failed\n");
    return -1;
  }

  srand(time(NULL));
  *key = rand64();
  fprintf(testVectorFile, "0X%.16lX\n", *key);
  uint16_t *RK = (uint16_t *)malloc(round * sizeof(uint16_t));
  generateRK(*key, round, RK);

  generateKeyANF(round);
  for (int i = 0; i < testCount; i++) {
    uint32_t plain = i ? rand32() : 0;
    uint32_t cipher = encrypt(plain, round, RK);
    fprintf(testVectorFile, "0X%.8X 0X%.8X\n", plain, cipher);
    generateTextANF(i, round, plain, cipher);
    for (int j = 1; j <= round; j++) generateRoundANF(i, j);
  }

  free(RK);
  fclose(testVectorFile);
  fclose(gAnfFile);

  return 0;
}
