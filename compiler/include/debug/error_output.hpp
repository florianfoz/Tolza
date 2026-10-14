#pragma once

#include "id/cuid.hpp"
#include "id/nodeid.hpp"
#include "nexus/forward.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <enumlite/enumlite.h>
#include <print>
#include <string>
#include <string_view>
#include <utility>


#ifdef TOLZA_TESTS
#define CHECK_TOLZA_TESTS                                                                                              \
  std::println(stderr, "{}", this->print_error());                                                                     \
  assert(false && "Error added")
#else
#define CHECK_TOLZA_TESTS void(0);
#endif


DEFINE_ENUM(EErrorSeverity, uint8_t, //
            debug, 1,                //
            warning, 2,              //
            error, 3,                //
            fatal, 4                 //
)

[[nodiscard]] std::string_view ESeverity_to_color(EErrorSeverity severity) noexcept;

[[nodiscard]] std::string      escapeChar(unsigned char c) noexcept;
[[nodiscard]] std::string_view trim(std::string_view str) noexcept;

struct Error_Info {
  // classic error 0000 - 0999 internal error 1000 - 1999
  ErrorCode        code = 0;
  std::string      msg;
  std::string      hint;
  compiler::EPhase phase;
  EErrorSeverity   severity = EErrorSeverity::error;
};

struct Error_Elem {
  cu::ID cuid;

  size_t start_pos = 0;
  size_t end_pos   = 0;

  Error_Info error_info;

  Error_Elem(cu::ID _cuid, ErrorCode _code, ast::ID nodeid, compiler::EPhase _phase, std::string_view _msg,
             std::string_view _hint);

  Error_Elem(cu::ID _cuid, ErrorCode _code, size_t _start_pos, size_t _end_pos, compiler::EPhase _phase,
             std::string_view _msg, std::string_view _hint)
    : cuid(_cuid)
    , start_pos(_start_pos)
    , end_pos(_end_pos)
    , error_info{
          .code  = _code,
          .msg   = std::string(_msg),
          .hint  = std::string(_hint),
          .phase = _phase,
      }
  {
    assert((!_cuid || start_pos <= end_pos) && "Illegal error bounds");
  }

  Error_Elem(cu::ID _cuid, ErrorCode _code, size_t _start_pos, size_t _end_pos, compiler::EPhase _phase,
             EErrorSeverity _severity, std::string_view _msg, std::string_view _hint)
    : cuid(_cuid)
    , start_pos(_start_pos)
    , end_pos(_end_pos)
    , error_info{
          .code  = _code,
          .msg   = std::string(_msg),
          .hint  = std::string(_hint),
          .phase = _phase,
      }
  {
    assert((!_cuid || start_pos <= end_pos) && "Illegal error bounds");
  }


  [[nodiscard]] std::string      print_error() const noexcept;
  [[nodiscard]] std::string      print_code() const noexcept;
  [[nodiscard]] std::string      print_messages() const noexcept;
  [[nodiscard]] std::string      print_cursor() const noexcept;
  [[nodiscard]] std::string_view get_raw_line() const noexcept;
  [[nodiscard]] std::string      print_line() const noexcept;
  [[nodiscard]] std::string      print_line_cursor() const noexcept;
  [[nodiscard]] std::string      print_source() const noexcept;
  [[nodiscard]] std::string      print_link_error() const
  {
    return {};
  }
};


struct Error_Diagnostic {
  Error_Diagnostic(Error_Elem first, Error_Elem second)
    : elem_first(std::move(first))
    , elem_second(std::move(second))
  {
    CHECK_TOLZA_TESTS;
  }
  Error_Diagnostic(cu::ID _cuid, ErrorCode _code, ast::ID nodeid1, ast::ID nodeid2, compiler::EPhase _phase,
                   std::string_view _msg, std::string_view _hint);
  Error_Diagnostic(cu::ID _cuid, ErrorCode _code, ast::ID nodeid, compiler::EPhase _phase, std::string_view _msg,
                   std::string_view _hint);

  Error_Diagnostic(cu::ID _cuid, ErrorCode _code, size_t _start_pos, size_t _end_pos, compiler::EPhase _phase,
                   std::string_view _msg, std::string_view _hint)
    : elem_first(_cuid, _code, _start_pos, _end_pos, _phase, _msg, _hint)
    , elem_second({}, 0, 0, 0, _phase, "", "")
  {
    assert(_cuid && "Illegal error on unknown script");
    CHECK_TOLZA_TESTS;
  }

  Error_Elem elem_first;
  Error_Elem elem_second;

  /* One line error
   * [file] file:LL:CC
   * [code] | code line
   *        |      ^^^^
   * [error] [AAwxyz] blabla
   * [hint] blabla
   */
  /* Multi line error same file
   * [file] file:LL:CC
   *        |      vvvv
   * [from] | code line
   *        | ...
   * [to]   | code line
   *        |      ^^^^
   * [error] [AAwxyz] blabla
   * [hint] blabla
   */
  /* Multi line error different file
   * [file] file:LL:CC
   *        |      vvvv
   * [from] | code line
   *        | ...
   * [to]   | code line
   *        |      ^^^^
   * [file] file:LL:CC
   * [error] [AAwxyz] blabla
   * [hint] blabla
   */
  [[nodiscard]] std::string print_userfriendly_error() const noexcept;
  [[nodiscard]] std::string print_json_error() const noexcept;
  [[nodiscard]] std::string print_github_error() const noexcept;
  [[nodiscard]] std::string print_error() const noexcept;
};


inline std::string escape_json(std::string_view text) noexcept
{
  std::string result;
  result.reserve(text.size());

  for (char c : text) {
    switch (c) {
    case '"':  result += "\\\""; break;

    case '\\': result += "\\\\"; break;

    case '\b': result += "\\b"; break;

    case '\f': result += "\\f"; break;

    case '\n': result += "\\n"; break;

    case '\r': result += "\\r"; break;

    case '\t': result += "\\t"; break;

    default:
      if (static_cast<unsigned char>(c) < 0x20) {
        char buffer[7];
        std::snprintf(buffer, sizeof(buffer), "\\u%04X", c);
        result += buffer;
      } else {
        result += c;
      }
    }
  }

  return result;
}
