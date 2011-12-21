// Programing Language SRU
// Copyright(C) 2005-2009 IKeJI
// 
#include "numeric.h"

#include <cstdlib>
#include <string>
#include <sstream>
#include <cmath>
#include "basic_object.h"
#include "class.h"
#include "library.h"
#include "native_proc.h"
#include "sru_string.h"
#include "constants.h"
#include "class.h"
#include "utils.h"
#include "interpreter.h"
#include "stack_frame.h"
#include "binding.h"
#include "testing_ast.h"
#include "testing_sru.h"
using namespace sru_test;

using namespace sru;
using namespace std;

struct SRUNumeric::Impl{
  Impl(): is_real(false), int_value(0), double_value(0.0){}
  bool is_real;
  int int_value;
  double double_value;
};

BasicObjectPtr SRUNumeric::NewInt(int value){
  const BasicObjectPtr ret = BasicObject::New(new SRUNumeric(value));
  Class::InitializeInstance(ret, Library::Instance()->Numeric());
  return ret;
}

BasicObjectPtr SRUNumeric::NewDouble(double value){
  const BasicObjectPtr ret = BasicObject::New(new SRUNumeric(value));
  Class::InitializeInstance(ret, Library::Instance()->Numeric());
  return ret;
}

DEFINE_SRU_PROC(NumericParse){
  ARGLEN(2);
  string narg = SRUString::GetValue(args[1]).to_str();
  if(narg.find('.', 0) != string::npos){
    return SRUNumeric::NewDouble(strtod(narg.c_str(), NULL));
  } else {
    return SRUNumeric::NewInt(atoi(narg.c_str()));
  }
}

DEFINE_SRU_PROC(Equal){
  ARGLEN(2);
  double left = 0;
  if(!SRUNumeric::TryGetDoubleValue(args[0], &left))
    return Library::Instance()->False();
  double right = 0;
  if(!SRUNumeric::TryGetDoubleValue(args[1], &right))
    return Library::Instance()->False();

  if(left == right){
    return Library::Instance()->True();
  }else{
    return Library::Instance()->False();
  }
}

DEFINE_SRU_PROC(NotEqual){
  ARGLEN(2);
  double left = 0;
  if(!SRUNumeric::TryGetDoubleValue(args[0], &left))
    return Library::Instance()->True();
  double right = 0;
  if(!SRUNumeric::TryGetDoubleValue(args[1], &right))
    return Library::Instance()->True();

  if(left == right){
    return Library::Instance()->False();
  }else{
    return Library::Instance()->True();
  }
}

DEFINE_SRU_PROC(GreaterThan){
  ARGLEN(2);
  double left = 0;
  DCHECK(SRUNumeric::TryGetDoubleValue(args[0], &left)) <<
    "GreaterThan needs numeric";
  double right = 0;
  DCHECK(SRUNumeric::TryGetDoubleValue(args[1], &right)) <<
    "GreaterThan needs numeric";
  if(left > right){
    return Library::Instance()->True();
  }else{
    return Library::Instance()->False();
  }
}

DEFINE_SRU_PROC(GreaterOrEqual){
  ARGLEN(2);
  double left = 0;
  DCHECK(SRUNumeric::TryGetDoubleValue(args[0], &left)) <<
    "GreaterThan needs numeric";
  double right = 0;
  DCHECK(SRUNumeric::TryGetDoubleValue(args[1], &right)) <<
    "GreaterThan needs numeric";
  if(left >= right){
    return Library::Instance()->True();
  }else{
    return Library::Instance()->False();
  }
}

DEFINE_SRU_PROC(LessThan){
  ARGLEN(2);
  double left = 0;
  DCHECK(SRUNumeric::TryGetDoubleValue(args[0], &left)) <<
    "LessThan needs numeric";
  double right = 0;
  DCHECK(SRUNumeric::TryGetDoubleValue(args[1], &right)) <<
    "LessThan needs numeric";
  if(left < right){
    return Library::Instance()->True();
  }else{
    return Library::Instance()->False();
  }
}

DEFINE_SRU_PROC(LessOrEqual){
  ARGLEN(2);
  double left = 0;
  DCHECK(SRUNumeric::TryGetDoubleValue(args[0], &left)) <<
    "LessThan needs numeric";
  double right = 0;
  DCHECK(SRUNumeric::TryGetDoubleValue(args[1], &right)) <<
    "LessThan needs numeric";
  if(left <= right){
    return Library::Instance()->True();
  }else{
    return Library::Instance()->False();
  }
}

DEFINE_SRU_PROC(LtLt){
  ARGLEN(2);
  DCHECK(!SRUNumeric::IsReal(args[0]) && !SRUNumeric::IsReal(args[1]))
    << "Couldn't shift real value";
  int left = 0;
  DCHECK(SRUNumeric::TryGetIntValue(args[0], &left)) <<
    "Plus needs numeric";
  int right = 0;
  DCHECK(SRUNumeric::TryGetIntValue(args[1], &right)) <<
    "Plus needs numeric";
  return SRUNumeric::NewInt(left << right);
}

DEFINE_SRU_PROC(GtGt){
  ARGLEN(2);
  DCHECK(!SRUNumeric::IsReal(args[0]) && !SRUNumeric::IsReal(args[1]))
    << "Couldn't shift real value";
  int left = 0;
  DCHECK(SRUNumeric::TryGetIntValue(args[0], &left)) <<
    "Plus needs numeric";
  int right = 0;
  DCHECK(SRUNumeric::TryGetIntValue(args[1], &right)) <<
    "Plus needs numeric";
  return SRUNumeric::NewInt(left >> right);
}

DEFINE_SRU_PROC(Plus){
  ARGLEN(2);
  if (SRUNumeric::IsReal(args[0]) || SRUNumeric::IsReal(args[1])) {
    double left = 0;
    DCHECK(SRUNumeric::TryGetDoubleValue(args[0], &left)) <<
      "Plus needs numeric";
    double right = 0;
    DCHECK(SRUNumeric::TryGetDoubleValue(args[1], &right)) <<
      "Plus needs numeric";
    return SRUNumeric::NewDouble(left+right);
  } else {
    int left = 0;
    DCHECK(SRUNumeric::TryGetIntValue(args[0], &left)) <<
      "Plus needs numeric";
    int right = 0;
    DCHECK(SRUNumeric::TryGetIntValue(args[1], &right)) <<
      "Plus needs numeric";
    return SRUNumeric::NewInt(left+right);
  }
}

DEFINE_SRU_PROC(Minus){
  ARGLEN(2);
  if (SRUNumeric::IsReal(args[0]) || SRUNumeric::IsReal(args[1])) {
    double left = 0;
    DCHECK(SRUNumeric::TryGetDoubleValue(args[0], &left)) <<
      "Minus needs numeric";
    double right = 0;
    DCHECK(SRUNumeric::TryGetDoubleValue(args[1], &right)) <<
      "Minus needs numeric";
    return SRUNumeric::NewDouble(left-right);
  } else {
    int left = 0;
    DCHECK(SRUNumeric::TryGetIntValue(args[0], &left)) <<
      "Minus needs numeric";
    int right = 0;
    DCHECK(SRUNumeric::TryGetIntValue(args[1], &right)) <<
      "Minus needs numeric";
    return SRUNumeric::NewInt(left-right);
  }
}

DEFINE_SRU_PROC(Asterisk){
  ARGLEN(2);
  if (SRUNumeric::IsReal(args[0]) || SRUNumeric::IsReal(args[1])) {
    double left = 0;
    DCHECK(SRUNumeric::TryGetDoubleValue(args[0], &left)) <<
      "Asterisk needs numeric";
    double right = 0;
    DCHECK(SRUNumeric::TryGetDoubleValue(args[1], &right)) <<
      "Asterisk needs numeric";
    return SRUNumeric::NewDouble(left*right);
  } else {
    int left = 0;
    DCHECK(SRUNumeric::TryGetIntValue(args[0], &left)) <<
      "Asterisk needs numeric";
    int right = 0;
    DCHECK(SRUNumeric::TryGetIntValue(args[1], &right)) <<
      "Asterisk needs numeric";
    return SRUNumeric::NewInt(left*right);
  }
}

DEFINE_SRU_PROC(Slash){
  ARGLEN(2);
  if (SRUNumeric::IsReal(args[0]) || SRUNumeric::IsReal(args[1])) {
    double left = 0;
    DCHECK(SRUNumeric::TryGetDoubleValue(args[0], &left)) <<
      "Slash needs numeric";
    double right = 0;
    DCHECK(SRUNumeric::TryGetDoubleValue(args[1], &right)) <<
      "Slash needs numeric";
    return SRUNumeric::NewDouble(left/right);
  } else {
    int left = 0;
    DCHECK(SRUNumeric::TryGetIntValue(args[0], &left)) <<
      "Slash needs numeric";
    int right = 0;
    DCHECK(SRUNumeric::TryGetIntValue(args[1], &right)) <<
      "Slash needs numeric";
    return SRUNumeric::NewInt(left/right);
  }
}

DEFINE_SRU_PROC(Percent){
  ARGLEN(2);
  if (SRUNumeric::IsReal(args[0]) || SRUNumeric::IsReal(args[1])) {
    double left = 0;
    DCHECK(SRUNumeric::TryGetDoubleValue(args[0], &left)) <<
      "Percent needs numeric";
    double right = 0;
    DCHECK(SRUNumeric::TryGetDoubleValue(args[1], &right)) <<
      "Percent needs numeric";
    return SRUNumeric::NewDouble(fmod(left, right));
  } else {
    int left = 0;
    DCHECK(SRUNumeric::TryGetIntValue(args[0], &left)) <<
      "Slash needs numeric";
    int right = 0;
    DCHECK(SRUNumeric::TryGetIntValue(args[1], &right)) <<
      "Slash needs numeric";
    return SRUNumeric::NewInt(left % right);
  }
}

DEFINE_SRU_PROC_SMASH(_times_internal){
  const BasicObjectPtr& binding =
    Interpreter::Instance()->CurrentStackFrame()->Binding();
  const BasicObjectPtr& ip = binding->Get(sym::_i());
  if(IsNil(ip)){
    PushResult(Library::Instance()->Nil());
    return;
  }
  const BasicObjectPtr& jp = binding->Get(sym::_j());
  if(IsNil(jp)){
    PushResult(Library::Instance()->Nil());
    return;
  }
  int i = SRUNumeric::GetIntValue(ip) + 1;
  int j = SRUNumeric::GetIntValue(jp);
  LOG_TRACE << "i:" << i;
  LOG_TRACE << "j:" << j;
  if( j <= i ) {
    PushResult(Library::Instance()->Nil());
    return;
  }
  binding->Set(sym::_i(), SRUNumeric::NewInt(i));

  Interpreter::Instance()->CurrentStackFrame()->Setup(
      A(
        C(R(sym::_block()), R(sym::_i())),
        C(R(sym::_times_internal()))
      ));
}

DEFINE_SRU_PROC_SMASH(Times){
  static BasicObjectPtr times_internal;
  ARGLEN(2);
  LOG_TRACE << args[0]->Inspect();
  const int& self = SRUNumeric::GetIntValue(args[0]);
  CHECK(self >= 0) << "Invalid value for time.";
  if(self <= 0){
    PushResult(Library::Instance()->Nil());
    return;
  }

  // TODO: thread safe.
  if(times_internal == NULL)
    times_internal = CREATE_SRU_PROC(_times_internal);

  const BasicObjectPtr binding = Binding::New();
  binding->Set(sym::_i(), SRUNumeric::NewInt(-1));
  binding->Set(sym::_j(), args[0]);
  binding->Set(sym::_times_internal(), times_internal);
  binding->Set(sym::_block(), args[1]);
  Interpreter::Instance()->DigIntoNewFrame(
      A(C(R(sym::_times_internal()))),
      binding);
}

DEFINE_SRU_PROC(Invert){
  ARGLEN(1);
  if (SRUNumeric::IsReal(args[0])) {
    const double& v = SRUNumeric::GetDoubleValue(args[0]);
    return SRUNumeric::NewDouble(-v);
  } else {
    const int& v = SRUNumeric::GetIntValue(args[0]);
    return SRUNumeric::NewInt(-v);
  }
}

DEFINE_SRU_PROC(ToS){
  ARGLEN(1);
  DCHECK(SRUNumeric::IsNumeric(args[0])) <<
    "ToS needs numeric";
  ostringstream s;
  if (SRUNumeric::IsReal(args[0])) {
    const double& v = SRUNumeric::GetDoubleValue(args[0]);
    s << v;
  } else {
    const int& v = SRUNumeric::GetIntValue(args[0]);
    s << v;
  }
  return SRUString::New(symbol(s.str()));
}

void SRUNumeric::InitializeClassObject(const BasicObjectPtr& numeric){
  Class::SetAsSubclass(numeric, NULL);
  numeric->Set(sym::parse(),CREATE_SRU_PROC(NumericParse));
  numeric->Set(sym::__name(), SRUString::New(symbol("Numeric")));
  Class::SetAsInstanceMethod(numeric, sym::equal(), CREATE_SRU_PROC(Equal));
  Class::SetAsInstanceMethod(numeric, sym::notEqual(), CREATE_SRU_PROC(NotEqual));
  Class::SetAsInstanceMethod(numeric, sym::greaterThan(), CREATE_SRU_PROC(GreaterThan));
  Class::SetAsInstanceMethod(numeric, sym::greaterOrEqual(), CREATE_SRU_PROC(GreaterOrEqual));
  Class::SetAsInstanceMethod(numeric, sym::lessThan(), CREATE_SRU_PROC(LessThan));
  Class::SetAsInstanceMethod(numeric, sym::lessOrEqual(), CREATE_SRU_PROC(LessOrEqual));
  Class::SetAsInstanceMethod(numeric, sym::ltlt(), CREATE_SRU_PROC(LtLt));
  Class::SetAsInstanceMethod(numeric, sym::gtgt(), CREATE_SRU_PROC(GtGt));
  Class::SetAsInstanceMethod(numeric, sym::plus(), CREATE_SRU_PROC(Plus));
  Class::SetAsInstanceMethod(numeric, sym::minus(), CREATE_SRU_PROC(Minus));
  Class::SetAsInstanceMethod(numeric, sym::asterisk(), CREATE_SRU_PROC(Asterisk));
  Class::SetAsInstanceMethod(numeric, sym::slash(), CREATE_SRU_PROC(Slash));
  Class::SetAsInstanceMethod(numeric, sym::percent(), CREATE_SRU_PROC(Percent));
  Class::SetAsInstanceMethod(numeric, sym::invert(), CREATE_SRU_PROC(Invert));

  Class::SetAsInstanceMethod(numeric, sym::times(), CREATE_SRU_PROC(Times));
  Class::SetAsInstanceMethod(numeric, sym::toS(), CREATE_SRU_PROC(ToS));
}

int SRUNumeric::GetIntValue(const BasicObjectPtr& obj){
  SRUNumeric* n = obj->GetData<SRUNumeric>();
  if(!n) return 0;
  if(n->pimpl->is_real){
    return static_cast<int>(n->pimpl->double_value);
  }
  return n->pimpl->int_value;
}

double SRUNumeric::GetDoubleValue(const BasicObjectPtr& obj){
  SRUNumeric* n = obj->GetData<SRUNumeric>();
  if(!n) return 0;
  if(n->pimpl->is_real){
    return n->pimpl->double_value;
  }
  return n->pimpl->int_value;
}

bool SRUNumeric::TryGetIntValue(const BasicObjectPtr& obj, int* val){
  SRUNumeric* n = obj->GetData<SRUNumeric>();
  if(!n) return false;
  if(n->pimpl->is_real){
    *val = static_cast<int>(n->pimpl->double_value);
  } else {
    *val = n->pimpl->int_value;
  }
  return true;
}

bool SRUNumeric::TryGetDoubleValue(const BasicObjectPtr& obj, double* val){
  SRUNumeric* n = obj->GetData<SRUNumeric>();
  if(!n) return false;
  if(n->pimpl->is_real){
    *val = n->pimpl->double_value;
  } else {
    *val = n->pimpl->int_value;
  }
  return true;
}

bool SRUNumeric::IsNumeric(const BasicObjectPtr& obj){
  SRUNumeric* n = obj->GetData<SRUNumeric>();
  return !!n;
}

bool SRUNumeric::IsReal(const BasicObjectPtr& obj){
  SRUNumeric* n = obj->GetData<SRUNumeric>();
  if(!n) return false;
  return n->pimpl->is_real;
}

string SRUNumeric::Inspect(){
  ostringstream o;
  if(pimpl->is_real){
    o << "Numeric(" << pimpl->double_value << ")";
  } else {
    o << "Numeric(" << pimpl->int_value << ")";
  }
  return o.str();
}

SRUNumeric::SRUNumeric(int n):
  pimpl(new Impl()){
  pimpl->is_real = false;
  pimpl->int_value = n;
}

SRUNumeric::SRUNumeric(double n):
  pimpl(new Impl()){
  pimpl->is_real = true;
  pimpl->double_value = n;
}

SRUNumeric::~SRUNumeric(){
  delete pimpl;
}

void SRUNumeric::Dispose(){
  delete this;
}
