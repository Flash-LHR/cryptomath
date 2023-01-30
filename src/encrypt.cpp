#include "encrypt.h"

void generateRK(uint64_t key, int round, uint16_t RK[]) {
  uint16_t keys[4] = {
      (uint16_t)(key & 0xffff),
      (uint16_t)((key >> 16) & 0xffff),
      (uint16_t)((key >> 32) & 0xffff),
      (uint16_t)((key >> 48) & 0xffff),
  };
  if (round < 8) {
    for (int i = 0; i < round; i++) RK[i] = i & 1 ? ~RK[i - 1] : keys[i >> 1];
  } else {
    for (int i = 0; i < 8; i++) RK[i] = i & 1 ? ~RK[i - 1] : keys[i >> 1];
    for (int i = 8; i < round; i++)
      RK[i] = i & 1 ? ~RK[i - 1] : ~RK[i - 8] ^ ((i - 8) >> 1);
  }
}

uint32_t encryptWithProbe(uint32_t plain, int round, uint16_t RK[],
                          uint16_t L[], uint16_t R[], uint16_t *T[4]) {
  L[0] = plain & 0xffff, R[0] = plain >> 16;
  for (int i = 1; i <= round; i++) {
    T[i][1] = (RK[i - 1] & L[i - 1]) ^ R[i - 1] ^ (i - 1);
    T[i][2] = ((T[i][1] << 2) & (T[i][1] << 1)) ^ T[i][1];
    T[i][3] = LCS16(T[i][2], 3) ^ LCS16(T[i][2], 9) ^ LCS16(T[i][2], 14);
    L[i] = R[i - 1] ^ (RK[i - 1] & T[i][3]);
    R[i] = L[i - 1] ^ T[i][3];
  }
  return (R[round] << 16) | L[round];
}

uint32_t encrypt(uint32_t plain, int round, uint16_t RK[]) {
  uint16_t *L = (uint16_t *)malloc((round + 1) * sizeof(uint16_t));
  uint16_t *R = (uint16_t *)malloc((round + 1) * sizeof(uint16_t));
  uint16_t **T = (uint16_t **)malloc((round + 1) * sizeof(uint16_t *));
  for (int i = 1; i <= round; i++)
    T[i] = (uint16_t *)malloc(4 * sizeof(uint16_t));

  uint32_t cipher = encryptWithProbe(plain, round, RK, L, R, T);

  for (int i = 1; i <= round; i++) free(T[i]);
  free(T);
  free(R);
  free(L);

  return cipher;
}