// Programing Language SRU
// Copyright(C) 2005-2010 IKeJI
// 

#include <string>

namespace sru {
class BasicObjectPtr;
class symbol;
} // namespace sru

namespace sru_parser {

sru::BasicObjectPtr CreateFalse(
    const sru::BasicObjectPtr& pos,
    const sru::BasicObjectPtr& error_message);
sru::BasicObjectPtr CreateFalse(
    const sru::BasicObjectPtr& pos,
    const char* error_message);
sru::BasicObjectPtr CreateFalse(
    int pos,
    const char* error_message);

sru::BasicObjectPtr CreateTrue(
    const sru::BasicObjectPtr& pos,
    const sru::BasicObjectPtr& ast);
sru::BasicObjectPtr CreateTrue(
    int pos,
    const sru::BasicObjectPtr& ast);

sru::BasicObjectPtr CreateAst(
    const sru::BasicObjectPtr& src,
    const sru::BasicObjectPtr& pos,
    const sru::BasicObjectPtr& self,
    const sru::symbol& method);

sru::BasicObjectPtr CreateAst(
    const sru::BasicObjectPtr& src,
    const sru::BasicObjectPtr& pos,
    const sru::BasicObjectPtr& self,
    const sru::symbol& method,
    const sru::BasicObjectPtr& arg1);

sru::BasicObjectPtr CreateAst(
    const sru::BasicObjectPtr& src,
    const sru::BasicObjectPtr& pos,
    const sru::BasicObjectPtr& self,
    const sru::symbol& method,
    const sru::BasicObjectPtr& arg1,
    const sru::BasicObjectPtr& arg2);

#define PARSER_CHECK(cond, pos, message) \
  if (cond) {} else \
    return CreateFalse(pos, message)

}  // namespace sru_parser
