// Programing Language SRU
// Copyright(C) 2005-2010 IKeJI
// 

#include "parser.h"

#include <cassert>
#include <string>
#include "testing.h"
#include "interpreter.h"
#include "numeric.h"
#include "library.h"
#include "constants.h"
#include "sru_string.h"

using namespace std;
using namespace sru;
using namespace sru_test;

TEST(Parser_ParserObjectTest){
  BasicObjectPtr sru_parser = Library::Instance()->Parser();
  assert(sru_parser.get());
  assert(SRUString::GetValue(sru_parser->Get(sym::__name())).to_str() == "sru_parser");
  assert(sru_parser->HasSlot(sym::parse()));
}

TEST(Parser_ParseRefTest){
  // "sru_parser"
  string code = "sru_parser";
  BasicObjectPtr r = Interpreter::Instance()->Eval(code);
  assert(r.get());
  assert(r == Library::Instance()->Parser());
}

TEST(Parser_ParseStringTest){
  // "\"hoge\""
  string code = "\"hoge\"";
  BasicObjectPtr r = Interpreter::Instance()->Eval(code);
  assert(r.get());
  assert(SRUString::GetValue(r).to_str() == "hoge");
}

TEST(Parser_ParseSpaceTest){
  // "     \"hoge\" "
  string code = "     \"hoge\" ";
  BasicObjectPtr r = Interpreter::Instance()->Eval(code);
  assert(r.get());
  assert(SRUString::GetValue(r).to_str() == "hoge");
}

TEST(Parser_ParseManipulatorTest){
  LOG << "TODO: Enable this test.";
  return;
  string code = "\"hoge\"   \"foo\" \"bar\" ";
  BasicObjectPtr r = Interpreter::Instance()->Eval(code);
  assert(r.get());
  assert(SRUString::GetValue(r).to_str() == "bar");
}
