// Programing Language SRU
// Copyright(C) 2005-2010 IKeJI
// 

#include "parser.h"
#include "string.h"
#include "basic_object.h"
#include "constants.h"

using namespace sru;
using namespace sru_parser;

void sru_parser::InitializeParserObject(BasicObjectPtr& parser){
  parser->Set(fNAME, SRUString::New("parser"));
  //TODO: register parser.
}

