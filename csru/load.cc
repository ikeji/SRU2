// Programing Language SRU
// Copyright(C) 2005-2010 IKeJI
//

#include "load.h"

#include <fstream>
#include <string>
#include "native_proc.h"
#include "library.h"
#include "sru_string.h"
#include "interpreter.h"
#include "stack_frame.h"
#include "constants.h"

using namespace sru;
using namespace std;

DEFINE_SRU_PROC(Load) {
  if(args.size() < 1)
    return Library::Instance()->False();
  string name = SRUString::GetValue(args[0]).to_str();
  ifstream ifs(name.c_str());
  string prog = "";
  string buf;
  while(ifs && getline(ifs,buf)){
    prog += buf + "\n";
  }
  // TODO: This code is invalid. Eval is not re-entrant.
  BasicObjectPtr r = Interpreter::Instance()->Eval(prog);
  if(r != NULL){
    return Library::Instance()->True();
  }else{
    return Library::Instance()->False();
  }
}

DEFINE_SRU_PROC_SMASH(Eval) {
  ARGLEN(1);
  ptr_vector pv;
  pv.push_back(args[0]);
  Interpreter::Instance()->DigIntoNewFrame(
      pv,Interpreter::Instance()->CurrentStackFrame()->Binding());
}

namespace sru {

void SetupLoadFunction(const BasicObjectPtr& env){
  env->Set(sym::load(), CREATE_SRU_PROC(Load));
  env->Set(sym::eval(), CREATE_SRU_PROC(Eval));
}

} // namespace sru
