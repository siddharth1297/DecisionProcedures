#include <cassert>
#include <cctype>
#include <fstream>

#include "fe_driver.hpp"

FE::Driver::~Driver() {
  delete (scanner);
  scanner = nullptr;
  delete (parser);
  parser = nullptr;
}

Ast *FE::Driver::parse(const char *const filename) {
  assert(filename != nullptr);
  std::ifstream in_file(filename);
  if (!in_file.good()) {
    exit(EXIT_FAILURE);
  }
  return parse_helper(in_file);
}

Ast *FE::Driver::parse(std::istream &stream) {
  if (!stream.good() && stream.eof()) {
    std::cerr << "Empty file or not good" << std::endl;
    abort();
  }
  return parse_helper(stream);
}

Ast *FE::Driver::parse_helper(std::istream &stream) {

  delete (scanner);
  try {
    scanner = new Scanner(&stream);
  } catch (std::bad_alloc &ba) {
    std::cerr << "Failed to allocate scanner: (" << ba.what()
              << "), exiting!!\n";
    exit(EXIT_FAILURE);
  }

  delete (parser);
  try {
    parser = new Parser((*scanner) /* scanner */, (*this) /* driver */);
  } catch (std::bad_alloc &ba) {
    std::cerr << "Failed to allocate parser: (" << ba.what()
              << "), exiting!!\n";
    exit(EXIT_FAILURE);
  }
  const int accept(0);
  if (parser->parse() != accept) {
    std::cerr << "Parse failed!!" << std::endl;
    abort();
  }
  return new Ast(root_);
}
