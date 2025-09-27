#include "src/Compiler.hpp"

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: ./main [filename]" << std::endl;
    return 1;
  }
  Compiler c(argv[1]);
  std::vector<std::string> res = c.compile_all();
  c.save_to_file(res);
  std::cout << "Done" << std::endl;
  return 0;
}