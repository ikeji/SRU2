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

#ifdef DEBUG

#define PARSER_CHECK(cond, pos, message) \
  if (cond) {} else {\
    LOG_ERROR << "An internal error happened at " << \
        __FILE__ << ":" << __LINE__; \
    return CreateFalse(pos, message); \
  }

#define PARSER_CHECK_SMASH(cond, pos, message) \
  if (cond) {} else { \
    LOG_ERROR << "An internal error happened at " << \
        __FILE__ << ":" << __LINE__; \
    PushResult(CreateFalse(pos, message)); \
    return; \
  }

#else

#define PARSER_CHECK(cond, pos, message) \
  if (cond) {} else \
    return CreateFalse(pos, message); \

#define PARSER_CHECK_SMASH(cond, pos, message) \
  if (cond) {} else { \
    PushResult(CreateFalse(pos, message)); \
    return; \
  }

#endif

#define PARGCHK() \
  PARSER_CHECK(args.size() >= 3, SRUNumeric::NewInt(0), "Internal parser error.")
#define PARGCHK_SMASH() \
  PARSER_CHECK_SMASH(args.size() >= 3, SRUNumeric::NewInt(0), "Internal parser error.")

#define PARGNCHK(n) \
  PARSER_CHECK(args.size() >= (n), args[2], "Internal parser error.")
#define PARGNCHK_SMASH(n) \
  PARSER_CHECK_SMASH(args.size() >= (n), args[2], "Internal parser error.")

}  // namespace sru_parser
