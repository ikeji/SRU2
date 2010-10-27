// Programing Language SRU
// Copyright(C) 2005-2009 IKeJI
// 
#include "sru_sys.h"

#include <ctime>
#include "native_proc.h"
#include "numeric.h"
#include "constants.h"
#include "sru_string.h"

namespace sru {

DEFINE_SRU_PROC(Clock){
  ARGLEN(2);
  return SRUNumeric::NewDouble(static_cast<double>(clock())/CLOCKS_PER_SEC);
}

void InitializeSysClass(const BasicObjectPtr& sys){
  sys->Set(sym::__name(), SRUString::New(sym::Sys()));
  sys->Set(sym::clock(), CREATE_SRU_PROC(Clock));
}

} // namespace sru
