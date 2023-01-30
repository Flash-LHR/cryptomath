#include "generateSage.h"

int generateSage(int round) {
  char anfFileName[FILE_NAME_SIZE];
  snprintf(anfFileName, FILE_NAME_SIZE, ANF_FILE, round);
  ifstream anfFile;
  anfFile.open(anfFileName, ios::in);
  if (!anfFile.is_open()) {
    return -1;
  }

  string line, entry;
  stringstream ss;
  vector<string> lines;
  vector<string> entrys;
  while (getline(anfFile, line)) {
    ss.clear();
    ss << line;
    lines.push_back(line);
    while (ss >> entry) {
      if (isalpha(entry[0])) entrys.push_back(entry);
    }
  }
  anfFile.close();
  sort(entrys.begin(), entrys.end());
  entrys.erase(unique(entrys.begin(), entrys.end()), entrys.end());

  char sageFileName[FILE_NAME_SIZE], cnfFileName[FILE_NAME_SIZE];
  snprintf(sageFileName, FILE_NAME_SIZE, SAGE_FILE, round);
  snprintf(cnfFileName, FILE_NAME_SIZE, CNF_FILE, round);
  ofstream sageFile;
  sageFile.open(sageFileName, ios::out);
  if (!sageFile.is_open()) {
    return -1;
  }

  sageFile << "B.<";
  for (size_t i = 0; i < entrys.size(); i++)
    sageFile << entrys[i] << (i == entrys.size() - 1 ? '>' : ',');
  sageFile << " = BooleanPolynomialRing()\n"
              "from sage.sat.converters.polybori import CNFEncoder\n"
              "from sage.sat.solvers.dimacs import DIMACS\n"
              "fn = tmp_filename()\nsolver = DIMACS(filename=fn)\n"
              "e = CNFEncoder(solver, B)\n";
  for (const string &line : lines)
    sageFile << "e.clauses_sparse(" << line << ")\n";
  sageFile << "_ = solver.write()\n"
              "with open('"
           << cnfFileName
           << "', 'w') as file:\n"
              "    file.write(open(fn).read())\n";

  sageFile.close();
  return 0;
}