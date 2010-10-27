// Programing Language SRU
// Copyright(C) 2005-2009 IKeJI
// 
#include "math.h"

#include <cmath>
#include "native_proc.h"
#include "logging.h"
#include "numeric.h"
#include "constants.h"
#include "sru_string.h"

namespace sru {

DEFINE_SRU_PROC(Sin){
  ARGLEN(2);
  double arg = 0;
  DCHECK(SRUNumeric::TryGetDoubleValue(args[1], &arg)) <<
    "Sin requires numeric.";
  return SRUNumeric::NewDouble(sin(arg));
}

void InitializeMathClass(const BasicObjectPtr& math){
  math->Set(sym::__name(), SRUString::New(sym::Math()));
  math->Set(sym::sin(), CREATE_SRU_PROC(Sin));
}

} // namespace sru
