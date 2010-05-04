// Programing Language SRU
// Copyright(C) 2005-2010 IKeJI
// 

#include <string>
#include "basic_object.h"
#include "constants.h"
#include "library.h"
#include "string.h"
#include "numeric.h"
#include "testing_ast.h"

using namespace sru;
using namespace sru_test;

namespace sru_parser {

BasicObjectPtr CreateFalse(const BasicObjectPtr& pos,
                           const BasicObjectPtr& error_message){
  BasicObjectPtr ret = BasicObject::New();
  ret->Set(sym::status(), Library::Instance()->False());
  ret->Set(sym::pos(), pos);
  ret->Set(sym::error(), error_message);
  return ret;
}

BasicObjectPtr CreateFalse(const BasicObjectPtr& pos, const char* error_message){
  return CreateFalse(pos, SRUString::New(symbol(error_message)));
}

BasicObjectPtr CreateFalse(int pos, const char* error_message){
  return CreateFalse(SRUNumeric::New(pos),
      SRUString::New(symbol(error_message)));
}

BasicObjectPtr CreateTrue(const BasicObjectPtr& pos, const BasicObjectPtr& ast){
  BasicObjectPtr ret = BasicObject::New();
  ret->Set(sym::status(), Library::Instance()->True());
  ret->Set(sym::pos(), pos);
  ret->Set(sym::ast(), ast);
  return ret;
}

BasicObjectPtr CreateTrue(int pos, const BasicObjectPtr& ast){
  return CreateTrue(SRUNumeric::New(pos), ast);
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

}  // namespace sru_parser
