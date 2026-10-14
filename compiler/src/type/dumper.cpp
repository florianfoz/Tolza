#include "type/dumper.hpp"

#include "ast/data.hpp"
#include "ast/dumper.hpp"
#include "ast/node/declaration_sfm.hpp"
#include "debug/test_assert.hpp"
#include "id/typeid.hpp"
#include "type/data.hpp"
#include "type/type.hpp"

#include <cassert>
#include <format>
#include <iterator>
#include <string>


std::string type::dump(type::ID tyid) noexcept
{
  const auto& ptr_ty = tyid.get();

  switch (tyid.kind()) {
  case type::ETypeKind::NONE: return {};
  case type::ETypeKind::Primitive:
    return std::string(EPrimitiveTypeKind_to_str(tyid.as<type::Primitive>()->primitive).substr(1));
  case type::ETypeKind::String: return std::string(ETextType_to_str(tyid.as<type::String>()->kind).substr(1));
  case type::ETypeKind::Tuple:  {
    std::string out;
    const auto* tu = tyid.as<type::Tuple>();
    out.reserve(tu->elems.size() * 32);
    for (const auto& tyid : tu->elems) {
      std::format_to(std::back_inserter(out), "{}, ", tyid.dump());
    }

    return std::format("({})", out.substr(0, out.size() - 2));
  }
  case type::ETypeKind::Array: {
    const auto* ptr = tyid.as<type::Array>();
    return std::format("[{}; {}]", ptr->inner.dump(), std::to_string(ptr->size));
  }
  case type::ETypeKind::Buffer: {
    const auto* ptr = tyid.as<type::Buffer>();
    return std::format("[{}; _]", ptr->inner.dump());
  }
  case type::ETypeKind::Slice: {
    const auto* ptr = tyid.as<type::Slice>();
    return std::format("[{}; {}]", ptr->inner.dump(), ptr->is_c_table ? "c" : "..");
  }
  case type::ETypeKind::Ptr: {
    const auto* ptr = tyid.as<type::Ptr>();
    return std::format("ptr'{}", ptr->inner.dump());
  }
  case type::ETypeKind::Range: {
    const auto* ptr = tyid.as<type::Range>();
    return "..";
  }
  case type::ETypeKind::Prototype: {
    std::string params;
    params.reserve(params.size() * 32);
    const auto* proto = tyid.as<type::Prototype>();
    for (const auto& param : proto->params) {
      std::format_to(std::back_inserter(params), "{} {}, ", ast::EPassMode_to_str(param.passmode), param.type.dump());
    }
    params = params.substr(0, params.size() - 2);

    return std::format("fn({}) -> {}", params, proto->ret.dump());
  }
  case type::ETypeKind::Facet: {
    const auto* facet = tyid.as<type::Facet>();
    const auto* node  = facet->def.node().as<ast::SFM_Facet>();
    assert(node && "type node must be a facet");

    std::string fields;
    fields.reserve(node->fields.size() * 32);
    for (const auto& field : node->fields) {
      const auto* f_node = field.as<ast::SFM_Facet_Field>();
      ASSERT_NODE(f_node, node->nodeid(), "A facet must have field nodes");

      std::format_to(std::back_inserter(fields), "{}: {}\n", f_node->name, f_node->type.dump());
    }

    return std::format("facet {}{{\n{}}}\n", node->name, fields);
  }
  case type::ETypeKind::View: {
    const auto* view = tyid.as<type::View>();
    assert(view->def && "a view type must refer to a symbol");
  }
  case type::ETypeKind::Identifier: {
    const auto* id = tyid.as<type::Identifier>();
    return ast::get_decl_name(id->nodeid);
  }
  case type::ETypeKind::Form:
  case type::ETypeKind::Enum:
  case type::ETypeKind::Flag:
  case type::ETypeKind::Union: break;
  }

  return {};
}
