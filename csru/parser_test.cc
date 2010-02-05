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
#include "string.h"

using namespace std;
using namespace sru;
using namespace sru_test;

TEST(Parser_ParserObjectTest){
  BasicObjectPtr sru_parser = Library::Instance()->Parser();
  assert(sru_parser.get());
  assert(SRUString::GetValue(sru_parser->Get(fNAME)) == "sru_parser");
  assert(sru_parser->HasSlot("parse"));
}

TEST(Parser_ParseRefTest){
  // "sru_parser"
  string code = "sru_parser";
  BasicObjectPtr r = Interpreter::Instance()->Eval(code);
  assert(r.get());
  assert(r == Library::Instance()->Parser());
}

