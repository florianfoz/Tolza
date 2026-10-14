#include "debug/test_assert.hpp"

#include "compiler/compilation_unit.hpp"
#include "compiler/file_info.hpp"
#include "id/nodeid.hpp"
#include "id/tokid.hpp"

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <print>
#include <string>
#include <string_view>

void test::assert_tolza(std::string_view file, int line, std::string_view func, std::string_view expr,
                        std::string_view msg, std::string_view tlz_file, size_t tlz_line, size_t tlz_col,
                        size_t tlz_size, std::string_view tlz_file_line)
{
  std::string cursor = std::string(tlz_col <= 1 ? 1 : tlz_col - 1, ' ') + std::string(tlz_size, '^');

  std::println(
      R"(
{}:{}: {}: 
  assertion `{}` failed: {}
  {}:{}:{}:
  {:04} | {}
       | {})",
      file, line, func, expr, msg, tlz_file, tlz_line, tlz_col, tlz_line, tlz_file_line, cursor);

  std::abort();
}

void test::assert_token(std::string_view file, int line, std::string_view func, std::string_view expr,
                        std::string_view msg, token::ID tokid)
{
  if (!tokid) assert(false && std::string(msg).c_str());
  assert_tolza(file, line, func, expr, msg, tokid.cu().get().file_info.path, tokid.line() + 1, tokid.col() + 1,
               tokid.str().size(), tokid.line_str());
}

void test::assert_node(std::string_view file, int line, std::string_view func, std::string_view expr,
                       std::string_view msg, ast::ID nodeid)
{
  if (!nodeid) assert(false && std::string(msg).c_str());
  assert_token(file, line, func, expr, msg, nodeid.token());
}
