// Programing Language SRU
// Copyright(C) 2005-2010 IKeJI
// 

#include <string>
#include "basic_object.h"
#include "constants.h"
#include "library.h"
#include "sru_string.h"
#include "numeric.h"
#include "testing_ast.h"

using namespace sru;
using namespace sru_test;

namespace sru_parser {

BasicObjectPtr CreateFalse(const BasicObjectPtr& pos,
                           const BasicObjectPtr& error_message){
  const BasicObjectPtr ret = BasicObject::New();
  ret->Set(sym::status(), Library::Instance()->False());
  ret->Set(sym::pos(), pos);
  ret->Set(sym::error(), error_message);
  LOG << error_message->Inspect();
  return ret;
}

BasicObjectPtr CreateFalse(const BasicObjectPtr& pos, const char* error_message){
  return CreateFalse(pos, SRUString::New(symbol(error_message)));
}

BasicObjectPtr CreateFalse(int pos, const char* error_message){
  return CreateFalse(SRUNumeric::NewInt(pos),
      SRUString::New(symbol(error_message)));
}

BasicObjectPtr CreateTrue(const BasicObjectPtr& pos, const BasicObjectPtr& ast){
  const BasicObjectPtr ret = BasicObject::New();
  ret->Set(sym::status(), Library::Instance()->True());
  ret->Set(sym::pos(), pos);
  ret->Set(sym::ast(), ast);
  return ret;
}

BasicObjectPtr CreateTrue(int pos, const BasicObjectPtr& ast){
  return CreateTrue(SRUNumeric::NewInt(pos), ast);
}

sru::BasicObjectPtr CreateAst(
    const sru::BasicObjectPtr& src,
    const sru::BasicObjectPtr& pos,
    const sru::BasicObjectPtr& self,
    const symbol& method){
  return E(src, pos,
           C(
             P(sym::doldol(),
               E(src,pos,
                 C(R(R(sym::doldol()), method),
                   R(sym::doldol())
                 )
               )
             ),
             self
           )
         );
}

sru::BasicObjectPtr CreateAst(
    const sru::BasicObjectPtr& src,
    const sru::BasicObjectPtr& pos,
    const sru::BasicObjectPtr& self,
    const symbol& method,
    const sru::BasicObjectPtr& arg1){
  return E(src, pos,
           C(
             P(sym::doldol(),
               E(src,pos,
                 C(R(R(sym::doldol()), method),
                   R(sym::doldol()),
                   arg1
                 )
               )
             ),
             self
           )
         );
}

sru::BasicObjectPtr CreateAst(
    const sru::BasicObjectPtr& src,
    const sru::BasicObjectPtr& pos,
    const sru::BasicObjectPtr& self,
    const symbol& method,
    const sru::BasicObjectPtr& arg1,
    const sru::BasicObjectPtr& arg2){
  return E(src, pos,
           C(
             P(sym::doldol(),
               E(src,pos,
                 C(R(R(sym::doldol()), method),
                   R(sym::doldol()),
                   arg1,
                   arg2
                 )
               )
             ),
             self
           )
         );
}

}  // namespace sru_parser
