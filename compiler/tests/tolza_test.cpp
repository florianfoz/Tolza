#include "compiler/compiler.hpp"

#include <cassert>


int main(int argc, const char* argv[])
{
  assert(COMPILER.run_tolza(argc, argv) && "Compilation failed");
}