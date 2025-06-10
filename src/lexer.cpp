#include "lexer.hpp"
#include <cctype>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

TokenMapper::TokenMapper()
    : keywordMap{{"C#4D#4", "if"},    {"C#4E4", "else"},
                 {"C#4F#4", "for"},   {"D#4F4", "in"},
                 {"E4G#4", "print"},  {"F#4B4", "fn"},
                 {"A#4C#5", "end"},   {"C#5D5", "("},
                 {"D5E5", ")"},       {"E5F#5", ":"},
                 {"F#5G#5", "+"},     {"G#5A#5", "-"},
                 {"A#5B5", "*"},      {"B5C#6", "/"},
                 {"C#6D6", "="},      {"C#6C#6", "=="},
                 {"B#6B#6", "!="},    {"D6E6", "<"},
                 {"E6F#6", ">"},      {"F#6G#6", ","},
                 {"A#6C#6", "while"}, {"G#4C#5", "return"},
                 {"D#4G#4", "break"}, {"E4C#5", "continue"},
                 {"E4D#5", "true"},   {"F#4E5", "false"},
                 {"D#7", "."}},
      digitMap{{"A0", '0'},  {"A#0", '1'}, {"B0", '2'},  {"C1", '3'},
               {"C#1", '4'}, {"D1", '5'},  {"D#1", '6'}, {"E1", '7'},
               {"F1", '8'},  {"F#1", '9'}},
      stringDelimiter("C#7") {}

Lexer::Lexer()
    : stringOpen(false), isDecimal(false),
      operatorSet{"(", ")",  ":",  "+", "-", "*", "/",
                  "=", "==", "!=", "<", ">", ",", "."} {}

Lexer::~Lexer() { finalize(); }

void Lexer::reset() {
  stringOpen = false;
  currentString.clear();
  currentStringTokens.clear();
  numberValue.clear();
  numberTokens.clear();
  isDecimal = false;
}

void Lexer::processFile(const std::string &filename) {
  reset();

  std::ifstream file(filename);
  if (!file) {
    std::cerr << "Error al abrir el archivo: " << filename << std::endl;
    return;
  }

  std::string line;
  while (std::getline(file, line)) {
    processLine(line);
  }

  finalize();
}

std::vector<std::string> Lexer::splitLine(const std::string &line) {
  std::vector<std::string> tokens;
  size_t i = 0;
  while (i < line.size()) {
    // Token de 3 caracteres: [A-G]#d
    if (i + 2 < line.size() && (line[i] >= 'A' && line[i] <= 'G') &&
        line[i + 1] == '#' && std::isdigit(line[i + 2])) {
      tokens.push_back(line.substr(i, 3));
      i += 3;
    }
    // Token de 2 caracteres: [A-G]d
    else if (i + 1 < line.size() && (line[i] >= 'A' && line[i] <= 'G') &&
             std::isdigit(line[i + 1])) {
      tokens.push_back(line.substr(i, 2));
      i += 2;
    } else {
      ++i;
    }
  }
  return tokens;
}

void Lexer::processLine(const std::string &line) {
  auto smallTokens = splitLine(line);
  std::vector<std::string> tokens;
  size_t i = 0;

  // Fusionar tokens para palabras clave compuestas
  while (i < smallTokens.size()) {
    if (i + 1 < smallTokens.size()) {
      std::string twoTokens = smallTokens[i] + smallTokens[i + 1];
      if (dict.keywordMap.find(twoTokens) != dict.keywordMap.end()) {
        tokens.push_back(twoTokens);
        i += 2;
        continue;
      }
    }
    tokens.push_back(smallTokens[i]);
    i += 1;
  }

  // Procesar cada token
  for (const auto &token : tokens) {
    processToken(token);
  }
  flushNumberBuffer(); // Vaciar buffer al final de cada línea
}

void Lexer::processToken(const std::string &token) {
  // Manejar delimitador de cadena
  if (token == dict.stringDelimiter) {
    flushNumberBuffer();
    if (stringOpen) {
      std::cout << "STRING \"" << currentString << "\" (" << currentStringTokens
                << ")\n";
      currentString.clear();
      currentStringTokens.clear();
      stringOpen = false;
    } else {
      stringOpen = true;
      currentStringTokens = token;
    }
    return;
  }

  // Dentro de una cadena
  if (stringOpen) {
    currentStringTokens += token;
    auto digitIt = dict.digitMap.find(token);
    if (digitIt != dict.digitMap.end()) {
      currentString += digitIt->second;
    } else {
      currentString += token;
    }
    return;
  }

  // Manejar punto decimal
  if (token == "D#7") {
    if (!isDecimal && !numberValue.empty()) {
      numberValue += '.';
      numberTokens += token;
      isDecimal = true;
      return;
    }
  }

  // Manejar dígitos
  auto digitIt = dict.digitMap.find(token);
  if (digitIt != dict.digitMap.end()) {
    numberValue += digitIt->second;
    numberTokens += token;
    return;
  }

  // Vaciar buffer de números si hay contenido
  flushNumberBuffer();

  // Manejar palabras clave, operadores y booleanos
  auto keywordIt = dict.keywordMap.find(token);
  if (keywordIt != dict.keywordMap.end()) {
    std::string_view value = keywordIt->second;

    if (value == "true" || value == "false") {
      std::cout << "BOOLEAN " << value << " (" << token << ")\n";
    } else if (operatorSet.find(value) != operatorSet.end()) {
      std::cout << "OPERATOR " << value << " (" << token << ")\n";
    } else {
      std::cout << "KEYWORD " << value << " (" << token << ")\n";
    }
    return;
  }

  // Identificador por defecto
  std::cout << "IDENTIFIER " << token << " (" << token << ")\n";
}

void Lexer::flushNumberBuffer() {
  if (!numberValue.empty()) {
    std::cout << "NUMBER " << numberValue << " (" << numberTokens << ")\n";
    numberValue.clear();
    numberTokens.clear();
    isDecimal = false;
  }
}

void Lexer::finalize() {
  flushNumberBuffer();
  if (stringOpen) {
    std::cout << "STRING \"" << currentString << "\" (" << currentStringTokens
              << ")\n";
    currentString.clear();
    currentStringTokens.clear();
    stringOpen = false;
  }
}
