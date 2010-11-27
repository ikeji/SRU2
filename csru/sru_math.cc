// Programing Language SRU
// Copyright(C) 2005-2009 IKeJI
// 
#include "sru_math.h"

#include <cmath>
#include <cstdlib>
#include "native_proc.h"
#include "logging.h"
#include "numeric.h"
#include "constants.h"
#include "sru_string.h"
#include "library.h"

namespace sru {

DEFINE_SRU_PROC(Sin){
  ARGLEN(2);
  double arg = 0;
  DCHECK(SRUNumeric::TryGetDoubleValue(args[1], &arg)) <<
    "sin requires numeric.";
  return SRUNumeric::NewDouble(sin(arg));
}

DEFINE_SRU_PROC(Cos){
  ARGLEN(2);
  double arg = 0;
  DCHECK(SRUNumeric::TryGetDoubleValue(args[1], &arg)) <<
    "Cos requires numeric.";
  return SRUNumeric::NewDouble(cos(arg));
}

bool IsSrandEd = false;

// NOTE: Thread unsafe.
DEFINE_SRU_PROC(Srand){
  int seed = static_cast<unsigned int>(time(NULL));
  if(args.size() >= 2){
    DCHECK(SRUNumeric::TryGetIntValue(args[1], &seed)) <<
      "srand requires numeric.";
  }
  srand(seed);
  IsSrandEd = true;
  return Library::Instance()->Nil();
}

// NOTE: Thread unsafe.
DEFINE_SRU_PROC(Rand){
  if(!IsSrandEd){
    IsSrandEd = true;
    srand(time(NULL));
  }
  if(args.size() >= 2){
    int max = 0;
    DCHECK(SRUNumeric::TryGetIntValue(args[1], &max)) <<
      "rand requires numeric.";
    // NOTE: I found new rand function could use with %.
    return SRUNumeric::NewInt(rand() % max);
  } else {
    return SRUNumeric::NewDouble(static_cast<double>(rand()) / RAND_MAX);
  }
}

void InitializeMathClass(const BasicObjectPtr& math){
  math->Set(sym::__name(), SRUString::New(sym::Math()));
  math->Set(sym::sin(), CREATE_SRU_PROC(Sin));
  math->Set(sym::cos(), CREATE_SRU_PROC(Cos));
  math->Set(sym::srand(), CREATE_SRU_PROC(Srand));
  math->Set(sym::rand(), CREATE_SRU_PROC(Rand));
}

} // namespace sru
