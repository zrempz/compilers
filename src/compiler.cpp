#include "compiler.hpp"
#include <string>
#include <string_view>

constexpr std::string_view notes_filename = "notes.txt";

Compiler::Compiler() = default;
Compiler::Compiler(const std::string &filename) {
  processor.processFile(filename);
  processor.saveNoteEventsToFile(notes_filename.data());
  lexer.processFile(notes_filename.data());
}

bool Compiler::processFile(const std::string &filename) {
  try {
    lexer.processFile(filename);
    return true;
  } catch (...) {
    return false;
  }
}
