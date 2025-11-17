/*
 * Rule: FIO51-C
 * Source: wiki
 * Status: FAIL - Should trigger FIO51-C violation
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
  std::terminate();
}