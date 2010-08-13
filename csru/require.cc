// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
//

#include "require.h"

#include <dlfcn.h>
#include "native_proc.h"
#include "library.h"
#include "sru_string.h"
#include "interpreter.h"
#include "stack_frame.h"
#include "constants.h"

using namespace sru;
using namespace std;

typedef bool require_proc(const BasicObjectPtr& env);

DEFINE_SRU_PROC(requireNative) {
  if(args.size() < 1)
    return Library::Instance()->False();
  string name = SRUString::GetValue(args[0]).to_str();
  void* handle = dlopen(name.c_str(), RTLD_NOW);
  if(!handle){
    LOG_ERROR << "ERROR: " << dlerror();
    return Library::Instance()->False();
  }
  // TODO: Use C++ style cast.
  require_proc* p = (require_proc*)(
      dlsym(handle, "require"));
  if(!p) {
    LOG_ERROR << "ERROR: " << dlerror();
    return Library::Instance()->False();
  }
  LOG_ERROR << "require ok";
  const BasicObjectPtr& current_enviroment =
    Interpreter::Instance()->
    CurrentStackFrame()->Binding();
  bool result = (*p)(current_enviroment);
  if(result){
    return Library::Instance()->True();
  }else{
    return Library::Instance()->False();
  }
}

namespace sru {

void SetupRequireFunction(const BasicObjectPtr& env){
  env->Set(sym::requireNative(), CREATE_SRU_PROC(requireNative));
}

} // namespace sru
