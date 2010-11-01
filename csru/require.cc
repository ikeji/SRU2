// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
//

#include "require.h"

#include "native_proc.h"
#include "library.h"
#include "sru_string.h"
#include "interpreter.h"
#include "stack_frame.h"
#include "constants.h"

using namespace sru;
using namespace std;

typedef bool require_proc(const BasicObjectPtr& env);

#if defined(WIN32)

#include <windows.h>

void* dlopen(const char* fname, int flag){
  return LoadLibrary(fname);
}
#define RTLD_NOW 1

require_proc* dlsym(void* handle, LPCSTR name){
  return reinterpret_cast<require_proc*>(GetProcAddress(
        reinterpret_cast<HINSTANCE>(handle), name));
};

string dlerror(){
  LPVOID lpMsgBuf;
  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
      FORMAT_MESSAGE_FROM_SYSTEM|
      FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL,
      GetLastError(),
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      (LPTSTR)&lpMsgBuf,
      0,
      NULL);
  string s((LPTSTR)lpMsgBuf);
  return s;
}

#else

#include <dlfcn.h>

#endif // WIN32

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
