/*
 * Rule: FIO50-C
 * Source: wiki
 * Status: FAIL - Should trigger FIO50-C violation
 */

#include <fstream>
#include <string>

void f(const std::string &fileName) {
  std::fstream file(fileName);
  if (!file.is_open()) {
    // Handle error
    return;
  }
  
  file << "Output some data";
  std::string str;
  file >> str;
}