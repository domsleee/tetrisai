#include <bits/stdc++.h>
#include <unistd.h>
using namespace std;

int main() {
  vector<string> filenames {"I_PIECE", "J_PIECE", "L_PIECE", "O_PIECE", "S_PIECE", "T_PIECE", "Z_PIECE"};
  ofstream fout{"generated.hpp"};

  chdir("pieces");

  fout << "#include <unordered_map>\n";
  fout << "#include \"src/common/BlockType.hpp\"\n";
  fout << "using T = std::vector<std::vector<std::vector<int>>>;\n";
  fout << "std::unordered_map<BlockType, T, std::hash<int>> blockToOffsetMap = {\n";

  for (auto filename: filenames) {
    ifstream fin{filename};
    cout << "processing: " << filename << '\n';
    int N, R, C;
    fin >> N >> R >> C;
    int t1, t2;
    fin >> t1 >> t2;
    fout << "  {BlockType::" << filename << ", {\n";
    vector<vector<vector<char>>> res(N, vector<vector<char>>(C, vector<char>(C)));
    for (int n = 0; n < N; n++) {
      for (int r = 0; r < R; r++) {
        for (int c = 0; c < C; c++) fin >> res[n][r][c];
      }
    }
    
    for (int n = 0; n < 4; n++) {
      fout << "    {\n";
      for (int r = 0; r < R; r++) {
        fout << "      {";
        for (int c = 0; c < C; c++) {
          fout << res[n%N][r][c];
          if (c != C-1) fout << ',';
        }
        fout << "},\n";
      }
      fout << "    },\n";
    }
    fout << "  }";
    fout << "}";
    if (filename != *filenames.rbegin()) fout << ",";
    fout << '\n';

  }
  fout << "};\n";
    
}