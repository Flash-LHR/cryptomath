#include <stdio.h>
#include <stdlib.h>

#include "generateANF.h"
#include "generateSage.h"
#include "solveCNF.h"

#define CMD_SIZE 1024

int compareKey(uint64_t key, uint64_t guessKey, int round) {
  fprintf(stderr, "真实密钥: 0X%.16lX\n", key);
  fprintf(stderr, "猜测密钥: 0X%.16lX\n", guessKey);
  int keyBit = round > 8 ? 64 : ((round + 1) >> 1) << 4;
  if (keyBit == 64) return key != guessKey;
  uint64_t mask = (1ull << keyBit) - 1;
  return (key & mask) != (guessKey & mask);
}

int main(int argc, char *argv[]) {
  uint64_t key, guessKey;
  clock_t mainStart = clock();
  if (argc != 2) {
    fprintf(stderr, "参数非法\n");
    fprintf(stderr, "用法：./main <round>\n");
    fprintf(stderr, "例如：./main 8\n");
    return -1;
  }
  int round = atoi(argv[1]);
  fprintf(stderr, "加密轮数：%d\n", round);
  int testCount = round == 1 ? 4 : 3;

  fprintf(stderr, "测试向量与ANF文件生成中\r");
  clock_t generatorANFStart = clock();
  if (generateANF(testCount, round, &key)) {
    fprintf(stderr, "测试向量与ANF文件生成失败\n");
    return -1;
  }
  clock_t generatorANFFinish = clock();
  fprintf(stderr, "测试向量与ANF文件生成成功, 耗时%.3fs\n",
          double(generatorANFFinish - generatorANFStart) / CLOCKS_PER_SEC);

  fprintf(stderr, "sage程序生成中\r");
  clock_t generatorSageStart = clock();
  if (generateSage(round)) {
    fprintf(stderr, "sage程序生成失败\n");
    return -1;
  }
  clock_t generatorSageFinish = clock();
  fprintf(stderr, "sage程序生成成功, 耗时%.3fs\n",
          double(generatorSageFinish - generatorSageStart) / CLOCKS_PER_SEC);

  char cmd[CMD_SIZE];
  char sageFileName[FILE_NAME_SIZE];
  snprintf(sageFileName, FILE_NAME_SIZE, SAGE_FILE, round);
  snprintf(cmd, CMD_SIZE, "sage %s", sageFileName);
  fprintf(stderr, "sage程序运行中\r");
  clock_t sageStart = clock();
  if (system(cmd)) {
    fprintf(stderr, "sage程序运行失败\n");
    return -1;
  }
  clock_t sageFinish = clock();
  fprintf(stderr, "sage程序运行成功, 耗时%.3fs\n",
          double(sageFinish - sageStart) / CLOCKS_PER_SEC);

  fprintf(stderr, "CNF求解中\r");
  clock_t solveCNFStart = clock();
  if (solveCNF(round, &guessKey)) {
    fprintf(stderr, "CNF求解失败\n");
    return -1;
  }
  clock_t solveCNFFinish = clock();
  fprintf(stderr, "CNF求解成功, 耗时%.3fs\n",
          double(solveCNFFinish - solveCNFStart) / CLOCKS_PER_SEC);

  clock_t mainFinish = clock();
  printf("总耗时 %.3fs\n", double(mainFinish - mainStart) / CLOCKS_PER_SEC);

  if (compareKey(key, guessKey, round)) {
    fprintf(stderr, "密钥破解失败\n");
    return -1;
  }
  fprintf(stderr, "密钥破解成功\n");
  return 0;
}