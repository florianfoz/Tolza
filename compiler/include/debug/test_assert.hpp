#pragma once

#include "nexus/forward.hpp"

#include <cassert>
#include <cstdlib>
#include <print>
#include <string>
#include <string_view>
#include <type_traits>

namespace test
{
template <typename T>
std::string print_value(const T& value)
{
  if constexpr (std::is_enum_v<T>) {
    using underlying_type = std::underlying_type_t<T>;
    return static_cast<underlying_type>(value);
  } else if constexpr (requires { std::string(value); }) {
    return value;
  } else {
    return "<value not printable>";
  }
}


template <typename A, typename B>
[[noreturn]]
void assert_eq(const A& actual, const B& expected, std::string_view actual_expr, std::string_view expected_expr,
               std::string_view file, int line)
{
  std::println(
      R"({}:{}: assertion failed `{} == {}`
  actual: {}
  expected: {})",
      file, line, actual_expr, expected_expr, print_value(actual), print_value(expected));

  std::abort();
}

[[noreturn]]
void assert_tolza(std::string_view file, int line, std::string_view func, std::string_view expr, std::string_view msg,
                  std::string_view tlz_file, size_t tlz_line, size_t tlz_col, size_t tlz_size,
                  std::string_view tlz_file_line);

[[noreturn]]
void assert_token(std::string_view file, int line, std::string_view func, std::string_view expr, std::string_view msg,
                  token::ID tokid);
[[noreturn]]
void assert_node(std::string_view file, int line, std::string_view func, std::string_view expr, std::string_view msg,
                 ast::ID nodeid);

} // namespace test

#ifdef NDEBUG

#define ASSERT_EQ(actual, expected)          ((void)0)
#define ASSERT_NODE(comparison, node, msg)   ((void)0)
#define ASSERT_TOKEN(comparison, token, msg) ((void)0)

#else

#define ASSERT_EQ(actual, expected)                                                                                    \
  (static_cast<bool>(actual == expected))                                                                              \
      ? void(0)                                                                                                        \
      : ::test::assert_eq((actual), (expected), #actual, #expected, __FILE__, __LINE__)

#define ASSERT_NODE(comparison, node, msg)                                                                             \
  (static_cast<bool>(comparison)) ? void(0) : ::test::assert_node(__FILE__, __LINE__, __func__, #comparison, msg, node)

#define ASSERT_TOKEN(comparison, token, msg)                                                                           \
  (static_cast<bool>(comparison)) ? void(0)                                                                            \
                                  : ::test::assert_token(__FILE__, __LINE__, __func__, #comparison, msg, token)

#endif