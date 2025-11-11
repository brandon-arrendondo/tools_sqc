/*
 * Rule: FIO51-C
 * Source: wiki
 * Status: PASS - Should NOT trigger FIO51-C violation
 */

#include <exception>
#include <fstream>
#include <string>

void f(const std::string &fileName) {
  std::fstream file(fileName);
  if (!file.is_open()) {
    // Handle error
    return;
  }
  // ...
  file.close();
  if (file.fail()) {
    // Handle error
  }
  std::terminate();
}