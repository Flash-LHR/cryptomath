#include "solveCNF.h"

int solveCNF(int round, uint64_t key, uint64_t *guessKey) {
  char cnfFileName[FILE_NAME_SIZE];
  snprintf(cnfFileName, FILE_NAME_SIZE, CNF_FILE, round);
  FILE *cnfFile = fopen(cnfFileName, "r");
  if (cnfFile == NULL) {
    return -1;
  }

  int nvar, nequ;
  fscanf(cnfFile, "%*s %*s %d %d", &nvar, &nequ);
  SATSolver solver;
  vector<Lit> clause;

  solver.set_num_threads(8);
  solver.new_vars(nvar);

  for (int i = 0; i < nequ; i++) {
    clause.clear();
    int var;
    while (fscanf(cnfFile, "%d", &var) && var != 0) {
      if (var > 0)
        clause.push_back(Lit(var - 1, false));
      else
        clause.push_back(Lit(-var - 1, true));
    }
    solver.add_clause(clause);
  }
  fclose(cnfFile);

  lbool ret = l_False;
  if (round > 8) {
    vector<Lit> assumptions(8);
    for (int i = 0; i < 8; i++) {
      assumptions[i] = Lit(i, (key >> i) & 1 ? false : true);
    }
    ret = solver.solve(&assumptions);
  } else {
    ret = solver.solve();
  }

  if (ret == l_False) return -1;

  *guessKey = 0;
  int keyBit = round > 8 ? 64 : ((round + 1) >> 1) << 4;
  for (int i = 0; i < keyBit; i++) {
    if (solver.get_model()[i] == l_True) *guessKey |= 1ull << i;
  }

  char keyFileName[FILE_NAME_SIZE];
  snprintf(keyFileName, FILE_NAME_SIZE, KEY_FILE, round);
  FILE *keyFile = fopen(keyFileName, "w");
  if (keyFile == NULL) {
    return -1;
  }
  fprintf(keyFile, "%.16lX\n", *guessKey);
  fclose(keyFile);

  return 0;
}
