// Programing Language SRU
// Copyright(C) 2005-2010 IKeJI
// 

#include <string>
#include "native_proc.h"
#include "basic_object.h"
#include "constants.h"
#include "library.h"
#include "numeric.h"
#include "parser_util.h"

// TODO: remove this dependency
#include "testing_ast.h"

using namespace sru;
using namespace std;
using namespace sru_test;

namespace sru_parser {


DEFINE_SRU_PROC(expression_begin){ // this, src, pos, bool_term
  PARSER_CHECK(args.size() >= 4, args[2], "Internal parser error.");
  LOG << "expression_begin";

  return args[3];
}

DEFINE_SRU_PROC(expression_pipepipe){ // this, src, pos, expression_begin, bool_term
  PARSER_CHECK(args.size() >= 5, args[2], "Internal parser error.");
  LOG << "expression_pipepipe";
  // TODO: Lazy evaluation second arg.
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  BasicObjectPtr ast = CreateAst(args[1], args[2],
                                 args[3]->Get(sym::ast()), sym::pipepipe(),
                                 args[4]->Get(sym::ast()));
  args[3]->Set(sym::ast(), ast);
  return CreateTrue(args[2], ast);
}

DEFINE_SRU_PROC(expression_end){ // this, src, pos, expression_begin
  PARSER_CHECK(args.size() >= 4, args[2], "Internal parser error.");
  LOG << "expression_end";

  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  return CreateTrue(args[2], args[3]->Get(sym::ast()));
}

DEFINE_SRU_PROC(bool_term_begin){ // this, src, pos, comp
  PARSER_CHECK(args.size() >= 4, args[2], "Internal parser error.");
  LOG << "bool_term_begin";

  return args[3];
}

DEFINE_SRU_PROC(bool_term_ampamp){ // this, src, pos, bool_term_begin, comp
  PARSER_CHECK(args.size() >= 5, args[2], "Internal parser error.");
  LOG << "bool_term_ampamp";
  // TODO: Lazy evaluation second arg.
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  BasicObjectPtr ast = CreateAst(args[1], args[2],
                                 args[3]->Get(sym::ast()), sym::ampamp(),
                                 args[4]->Get(sym::ast()));
  args[3]->Set(sym::ast(), ast);
  return CreateTrue(args[2], ast);
}

DEFINE_SRU_PROC(bool_term_end){ // this, src, pos, bool_term_begin
  PARSER_CHECK(args.size() >= 4, args[2], "Internal parser error.");
  LOG << "bool_term_end";
  
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  return CreateTrue(args[2], args[3]->Get(sym::ast()));
}

DEFINE_SRU_PROC(comp_begin){ // this, src, pos, bit_sim
  PARSER_CHECK(args.size() >= 4, args[2], "Internal parser error.");
  LOG << "comp_begin";

  return args[3];
}

DEFINE_SRU_PROC(comp_equal){ // this, src, pos, comp_begin, bit_sim
  PARSER_CHECK(args.size() >= 5, args[2], "Internal parser error.");
  LOG << "comp_equal";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  BasicObjectPtr ast = CreateAst(args[1], args[2],
                                 args[3]->Get(sym::ast()), sym::equal(),
                                 args[4]->Get(sym::ast()));
  args[3]->Set(sym::ast(), ast);
  return CreateTrue(args[2], ast);
}

DEFINE_SRU_PROC(comp_super_equal){ // this, src, pos, comp_begin, bit_sim
  PARSER_CHECK(args.size() >= 5, args[2], "Internal parser error.");
  LOG << "comp_super_equal";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  BasicObjectPtr ast = CreateAst(args[1], args[2],
                                 args[3]->Get(sym::ast()), sym::superEqual(),
                                 args[4]->Get(sym::ast()));
  args[3]->Set(sym::ast(), ast);
  return CreateTrue(args[2], ast);
}

DEFINE_SRU_PROC(comp_not_equal){ // this, src, pos, comp_begin, bit_sim
  PARSER_CHECK(args.size() >= 5, args[2], "Internal parser error.");
  LOG << "comp_not_equal";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  BasicObjectPtr ast = CreateAst(args[1], args[2],
                                 args[3]->Get(sym::ast()), sym::notEqual(),
                                 args[4]->Get(sym::ast()));
  args[3]->Set(sym::ast(), ast);
  return CreateTrue(args[2], ast);
}

DEFINE_SRU_PROC(comp_greater_than){ // this, src, pos, comp_begin, bit_sim
  PARSER_CHECK(args.size() >= 5, args[2], "Internal parser error.");
  LOG << "comp_greater_than";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  BasicObjectPtr ast = CreateAst(args[1], args[2],
                                 args[3]->Get(sym::ast()), sym::greaterThan(),
                                 args[4]->Get(sym::ast()));
  args[3]->Set(sym::ast(), ast);
  return CreateTrue(args[2], ast);
}

DEFINE_SRU_PROC(comp_less_than){ // this, src, pos, comp_begin, bit_sim
  PARSER_CHECK(args.size() >= 5, args[2], "Internal parser error.");
  LOG << "comp_less_than";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  BasicObjectPtr ast = CreateAst(args[1], args[2],
                                 args[3]->Get(sym::ast()), sym::lessThan(),
                                 args[4]->Get(sym::ast()));
  args[3]->Set(sym::ast(), ast);
  return CreateTrue(args[2], ast);
}

DEFINE_SRU_PROC(comp_greater_or_equal){ // this, src, pos, comp_begin, bit_sim
  PARSER_CHECK(args.size() >= 5, args[2], "Internal parser error.");
  LOG << "comp_greater_or_equal";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  BasicObjectPtr ast = CreateAst(args[1], args[2],
                                 args[3]->Get(sym::ast()), sym::greaterOrEqual(),
                                 args[4]->Get(sym::ast()));
  args[3]->Set(sym::ast(), ast);
  return CreateTrue(args[2], ast);
}

DEFINE_SRU_PROC(comp_less_or_equal){ // this, src, pos, comp_begin, bit_sim
  PARSER_CHECK(args.size() >= 5, args[2], "Internal parser error.");
  LOG << "comp_less_or_equal";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  BasicObjectPtr ast = CreateAst(args[1], args[2],
                                 args[3]->Get(sym::ast()), sym::lessOrEqual(),
                                 args[4]->Get(sym::ast()));
  args[3]->Set(sym::ast(), ast);
  return CreateTrue(args[2], ast);
}

DEFINE_SRU_PROC(comp_end){ // this, src, pos, comp_begin
  PARSER_CHECK(args.size() >= 4, args[2], "Internal parser error.");
  LOG << "comp_end";

  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  return CreateTrue(args[2], args[3]->Get(sym::ast()));
}

DEFINE_SRU_PROC(bit_sim_begin){ // this, src, pos, bit_term
  PARSER_CHECK(args.size() >= 4, args[2], "Internal parser error.");
  LOG << "bit_sim_begin";

  return args[3];
}

DEFINE_SRU_PROC(bit_sim_pipe){ // this, src, pos, bit_sim_begin, bit_term
  PARSER_CHECK(args.size() >= 5, args[2], "Internal parser error.");
  LOG << "bit_sim_pipe";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  BasicObjectPtr ast = CreateAst(args[1], args[2],
                                 args[3]->Get(sym::ast()), sym::pipe(),
                                 args[4]->Get(sym::ast()));
  args[3]->Set(sym::ast(), ast);
  return CreateTrue(args[2], ast);
}

DEFINE_SRU_PROC(bit_sim_end){ // this, src, pos, bit_sim_begin
  PARSER_CHECK(args.size() >= 4, args[2], "Internal parser error.");
  LOG << "bit_sim_end";

  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  return CreateTrue(args[2], args[3]->Get(sym::ast()));
}

DEFINE_SRU_PROC(bit_term_begin){ // this, src, pos, bit_shift
  PARSER_CHECK(args.size() >= 4, args[2], "Internal parser error.");
  LOG << "bit_term_begin";

  return args[3];
}

DEFINE_SRU_PROC(bit_term_amp){ // this, src, pos, bit_term_begin, bit_shift
  PARSER_CHECK(args.size() >= 5, args[2], "Internal parser error.");
  LOG << "bit_term_amp";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  BasicObjectPtr ast = CreateAst(args[1], args[2],
                                 args[3]->Get(sym::ast()), sym::amp(),
                                 args[4]->Get(sym::ast()));
  args[3]->Set(sym::ast(), ast);
  return CreateTrue(args[2], ast);
}

DEFINE_SRU_PROC(bit_term_end){ // this, src, pos, bit_term_begin
  PARSER_CHECK(args.size() >= 4, args[2], "Internal parser error.");
  LOG << "bit_term_end";

  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  return CreateTrue(args[2], args[3]->Get(sym::ast()));
}

DEFINE_SRU_PROC(bit_shift_begin){ // this, src, pos, sim
  PARSER_CHECK(args.size() >= 4, args[2], "Internal parser error.");
  LOG << "bit_shift_begin";

  return args[3];
}

DEFINE_SRU_PROC(bit_shift_ltlt){ // this, src, pos, bit_shift_begin, sim
  PARSER_CHECK(args.size() >= 5, args[2], "Internal parser error.");
  LOG << "bit_shift_ltlt";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  BasicObjectPtr ast = CreateAst(args[1], args[2],
                                 args[3]->Get(sym::ast()), sym::ltlt(),
                                 args[4]->Get(sym::ast()));
  args[3]->Set(sym::ast(), ast);
  return CreateTrue(args[2], ast);
}

DEFINE_SRU_PROC(bit_shift_gtgt){ // this, src, pos, bit_shift_begin, sim
  PARSER_CHECK(args.size() >= 5, args[2], "Internal parser error.");
  LOG << "bit_shift_gtgt";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  BasicObjectPtr ast = CreateAst(args[1], args[2],
                                 args[3]->Get(sym::ast()), sym::gtgt(),
                                 args[4]->Get(sym::ast()));
  args[3]->Set(sym::ast(), ast);
  return CreateTrue(args[2], ast);
}

DEFINE_SRU_PROC(bit_shift_end){ // this, src, pos, bit_shift_begin
  PARSER_CHECK(args.size() >= 4, args[2], "Internal parser error.");
  LOG << "bit_shift_end";

  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  return CreateTrue(args[2], args[3]->Get(sym::ast()));
}

DEFINE_SRU_PROC(sim_begin){ // this, src, pos, term
  PARSER_CHECK(args.size() >= 4, args[2], "Internal parser error.");
  LOG << "sim_begin";

  return args[3];
}

DEFINE_SRU_PROC(sim_plus){ // this, src, pos, sim_begin, term
  PARSER_CHECK(args.size() >= 5, args[2], "Internal parser error.");
  LOG << "sim_plus";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  BasicObjectPtr ast = CreateAst(args[1], args[2],
                                 args[3]->Get(sym::ast()), sym::plus(),
                                 args[4]->Get(sym::ast()));
  args[3]->Set(sym::ast(), ast);
  return CreateTrue(args[2], ast);
}

DEFINE_SRU_PROC(sim_minus){ // this, src, pos, sim_begin, term
  PARSER_CHECK(args.size() >= 5, args[2], "Internal parser error.");
  LOG << "sim_minus";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  BasicObjectPtr ast = CreateAst(args[1], args[2],
                                 args[3]->Get(sym::ast()), sym::minus(),
                                 args[4]->Get(sym::ast()));
  args[3]->Set(sym::ast(), ast);
  return CreateTrue(args[2], ast);
}

DEFINE_SRU_PROC(sim_end){ // this, src, pos, sim_begin
  PARSER_CHECK(args.size() >= 4, args[2], "Internal parser error.");
  LOG << "sim_end";

  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  return CreateTrue(args[2], args[3]->Get(sym::ast()));
}

DEFINE_SRU_PROC(term_begin){ // this, src, pos, factor
  PARSER_CHECK(args.size() >= 4, args[2], "Internal parser error.");
  LOG << "term_begin";

  return args[3];
}

DEFINE_SRU_PROC(term_asterisk){ // this, src, pos, term_begin, factor
  PARSER_CHECK(args.size() >= 5, args[2], "Internal parser error.");
  LOG << "term_asterisk";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  BasicObjectPtr ast = CreateAst(args[1], args[2],
                                 args[3]->Get(sym::ast()), sym::asterisk(),
                                 args[4]->Get(sym::ast()));
  args[3]->Set(sym::ast(), ast);
  return CreateTrue(args[2], ast);
}

DEFINE_SRU_PROC(term_slash){ // this, src, pos, term_begin, factor
  PARSER_CHECK(args.size() >= 5, args[2], "Internal parser error.");
  LOG << "term_slash";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  BasicObjectPtr ast = CreateAst(args[1], args[2],
                                 args[3]->Get(sym::ast()), sym::slash(),
                                 args[4]->Get(sym::ast()));
  args[3]->Set(sym::ast(), ast);
  return CreateTrue(args[2], ast);
}

DEFINE_SRU_PROC(term_end){ // this, src, pos, term_begin
  PARSER_CHECK(args.size() >= 4, args[2], "Internal parser error.");
  LOG << "term_end";

  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  return CreateTrue(args[2], args[3]->Get(sym::ast()));
}

DEFINE_SRU_PROC(factor_exclamation){ // this, src, pos, factor
  PARSER_CHECK(args.size() >= 4, args[2], "Internal parser error.");
  LOG << "factor_exclamation";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  BasicObjectPtr ast = CreateAst(args[1], args[2],
                                 args[3]->Get(sym::ast()), sym::exclamation(),
                                 args[4]->Get(sym::ast()));
  args[3]->Set(sym::ast(), ast);
  return CreateTrue(args[2], ast);
}

DEFINE_SRU_PROC(factor_tilde){ // this, src, pos, factor
  PARSER_CHECK(args.size() >= 4, args[2], "Internal parser error.");
  LOG << "factor_tilde";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  BasicObjectPtr ast = CreateAst(args[1], args[2],
                                 args[3]->Get(sym::ast()), sym::tilde(),
                                 args[4]->Get(sym::ast()));
  args[3]->Set(sym::ast(), ast);
  return CreateTrue(args[2], ast);
}


}  // namespace sru_parser
