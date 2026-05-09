#include "cxFileIO.hh"
#include <iostream>
#include <string>

auto main(int argc, char* argv[]) -> int {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <file>\n";
    std::cerr << "Supports: plain, .gz, .bz2, .xz, .zst, .zlib\n";
    return 1;
  }

  try {
    cxfunc::cxFileIO file(argv[1]);
    std::string line;
    while (std::getline(file.stream(), line)) {
      std::cout << line << '\n';
    }
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << '\n';
    return 1;
  }

  return 0;
}
