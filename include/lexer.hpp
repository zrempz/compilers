#pragma once
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

enum class TokenType {
  KEYWORD,
  NUMBER,
  STRING,
  IDENTIFIER,
  STRING_DELIMITER,
  OPERATOR,
  BOOLEAN,
  UNKNOWN
};

class TokenMapper {
public:
  const std::unordered_map<std::string_view, std::string_view> keywordMap;
  const std::unordered_map<std::string_view, char> digitMap;
  const std::string_view stringDelimiter;

  TokenMapper();
};

class Lexer {
  TokenMapper dict;
  bool stringOpen;
  std::string currentString;
  std::string currentStringTokens;
  std::string numberValue;
  std::string numberTokens;
  bool isDecimal;

  std::unordered_set<std::string_view> operatorSet;

  std::vector<std::string> splitLine(const std::string &line);
  void processLine(const std::string &line);
  void processToken(const std::string &token);
  void flushNumberBuffer();
  void reset();

public:
  Lexer();
  ~Lexer();
  void processFile(const std::string &filename);
  void finalize();
};
