#pragma once
#include "audio_processor.hpp"
#include "lexer.hpp"
#include <string>

class Compiler {
public:
  Compiler();
  Compiler(const std::string &filename);

  bool processFile(const std::string &filename);

private:
  AudioProcessor processor;
  Lexer lexer;
};
