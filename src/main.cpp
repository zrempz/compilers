#include "compiler.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Uso: " << argv[0] << " <archivo_audio>" << std::endl;
    return 1;
  }
  Compiler compiler;
  compiler.processFile(argv[1]);
  return 0;
}
