// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 
#include "native_proc.h"

#include <string>
#include "interpreter.h"
#include "stack_frame.h"
#include "logging.h"

using namespace sru;
using namespace std;
using namespace sru_logging;

void NativeProc::Call(const BasicObjectPtr& context,
                      const BasicObjectPtr& proc,
                      const ptr_vector& arg){
  // TODO: Refacter this.
  IF_DEBUG_INFO {
    LOGOBJ(log);
    log.ostream() << "Call native proc with: ";
    for(ptr_vector::const_iterator it = arg.begin();
        it != arg.end();
        it++){
      log.ostream() << (*it)->Inspect() << " ";
    }
  }
  const BasicObjectPtr ret = method_body(context, proc, arg);
  PushResult(ret);
}
void NativeProcWithStackSmash::Call(const BasicObjectPtr& context,
                                    const BasicObjectPtr& proc,
                                    const ptr_vector& arg){
  method_body_smash(context, proc, arg);
}

string NativeProc::Inspect(){
  return "Proc({ -- Native Code -- })";
}
  
void NativeProc::PushResult(const BasicObjectPtr& result){
  StackFrame* current_frame = Interpreter::Instance()->CurrentStackFrame();
  current_frame->PushResult(result);
}
