#include <fstream>
#include <iterator>
#include <string>
#include <algorithm>
#include <iostream>

using namespace std;

bool compareFiles(const std::string& p1, const std::string& p2) {
  std::ifstream f1(p1.c_str(), std::ifstream::binary|std::ifstream::ate);
  std::ifstream f2(p2.c_str(), std::ifstream::binary|std::ifstream::ate);

  if (f1.fail() || f2.fail()) {
    return false; //file problem
  }

  if (f1.tellg() != f2.tellg()) {
    return false; //size mismatch
  }

  //seek back to beginning and use std::equal to compare contents
  f1.seekg(0, std::ifstream::beg);
  f2.seekg(0, std::ifstream::beg);
  return std::equal(std::istreambuf_iterator<char>(f1.rdbuf()),
                    std::istreambuf_iterator<char>(),
                    std::istreambuf_iterator<char>(f2.rdbuf()));
}

int main(int argc, char* argv[]){
  if(argc != 3)
    cout << "Usage ./" << argv[0] << " result.<any extension> expected_result.<any extension>" << endl;
  
  cout << "Testing " << argv[1] << ".....";
  if(compareFiles(argv[1], argv[2]))
    cout << "\033[1;32mpassed\033[0m\n" << endl;
  else
    cout << "\033[1;31mfailed\033[0m\n" << endl;
}