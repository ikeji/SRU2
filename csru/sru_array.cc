// Copyright(C) 2005-2008 IKeJI
//

#include "sru_array.h"

#include <sstream>
#include <vector>
#include <algorithm>
#include "basic_object.h"
#include "class.h"
#include "native_proc.h"
#include "constants.h"
#include "sru_string.h"
#include "library.h"
#include "object_container.h"
#include "logging.h"
#include "numeric.h"
#include "interpreter.h"
#include "stack_frame.h"
#include "utils.h"
#include "testing_ast.h"
#include "binding.h"

#include "testing_sru.h"

using namespace sru;
using namespace std;

// TODO: remove this.
using namespace sru_test;

namespace sru {

DEFINE_SRU_PROC(ArrayNew){
  return Array::New();
}

DEFINE_SRU_PROC(ArrayAmp){
  ARGLEN(2);
  CHECK(false) << "Not impliment amp";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayAsterisk){
  ARGLEN(2);
  CHECK(false) << "Not impliment asterisk";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayPlus){
  ARGLEN(2);
  CHECK(false) << "Not impliment plus";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayMinus){
  ARGLEN(2);
  CHECK(false) << "Not impliment minus";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayPipe){
  ARGLEN(2);
  CHECK(false) << "Not impliment pipe";
  return Library::Instance()->Nil();
}

// TODO: Need <->

DEFINE_SRU_PROC(ArrayGet){ // Slice
  ARGLEN(2);
  // TODO: impliment array[1,2]
  // TODO: impliment array[1..3]
  Array* array = args[0]->GetData<Array>();
  DCHECK(array) << "Array not found.";
  int index = 0;
  DCHECK(SRUNumeric::TryGetIntValue(args[1], &index));
  LOG_TRACE << "index: " << index;
  if ((int)array->GetValue()->size() > index){
    if(index < 0) index += array->GetValue()->size();
    if(index >= 0){
      return array->GetValue()->at(index);
    }
  }
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArraySet){
  ARGLEN(3);
  // TODO: impliment array[1,2]=3
  // TODO: impliment array[1..3]=4
  Array* array = args[0]->GetData<Array>();
  DCHECK(array) << "Array not found.";
  int index = 0;
  DCHECK(SRUNumeric::TryGetIntValue(args[1], &index));
  LOG_TRACE << "index: " << index;
  LOG_TRACE << "size: " << array->GetValue()->size();
  if(index < 0) index += array->GetValue()->size();
  LOG_TRACE << "index: " << index;
  DCHECK(0 <= index) << "invalid index";
  // TODO: Customizable default value.
  while(index >= (int)array->GetValue()->size()){
    array->GetValue()->push_back(Library::Instance()->Nil().get());
  }
  array->GetValue()->at(index) = args[2].get();
  return args[2];
}

DEFINE_SRU_PROC(ArrayAssoc){
  CHECK(false) << "Not impliment assoc";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayChoice){
  CHECK(false) << "Not impliment choice";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayClear){
  ARGLEN(1);
  Array* array = args[0]->GetData<Array>();
  DCHECK(array) << "Array not found.";
  array->GetValue()->clear();
  return args[0];
}

DEFINE_SRU_PROC(ArrayCollectEx){
  CHECK(false) << "Not impliment collectex";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayCombination){
  CHECK(false) << "Not impliment combination";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayCompact){
  CHECK(false) << "Not impliment compact";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayCompactEx){
  CHECK(false) << "Not impliment compactex";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayConcat){
  ARGLEN(1);
  Array* array = args[0]->GetData<Array>();
  DCHECK(array) << "Array not found.";
  object_vector* a = array->GetValue();
  for(ptr_vector::const_iterator it = args.begin();
      it != args.end();
      it++){
    if(it == args.begin()) continue;
    Array* array2 = (*it)->GetData<Array>();
    DCHECK(array2) << "Array not found for argument.";
    object_vector* a2 = array2->GetValue();
    for(object_vector::const_iterator i = a2->begin();
        i != a2->end();
        i++){
      a->push_back(*i);
    }
  }
  return args[0];
}

DEFINE_SRU_PROC(ArrayDelete){
  CHECK(false) << "Not impliment delete";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayDeleteIf){
  CHECK(false) << "Not impliment deleteif";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC_SMASH(_array_each_internal){
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
        C(R(sym::_block()),
          C(R(R(sym::_a()),sym::get()),
            R(sym::_a()),
            R(sym::_i())),
          R(sym::_i())),
        C(R(sym::_each_internal()))
      ));
}

DEFINE_SRU_PROC_SMASH(ArrayEach){
  static BasicObjectPtr array_each_internal;
  ARGLEN(2);
  LOG_TRACE << args[0]->Inspect();
  Array* array = args[0]->GetData<Array>();
  DCHECK(array) << "Invalid value for each.";
  int size = (int)array->GetValue()->size();
  if(size == 0){
    PushResult(Library::Instance()->Nil());
    return;
  }

  // TODO: thread safe.
  if(array_each_internal == NULL)
    array_each_internal = CREATE_SRU_PROC(_array_each_internal);

  const BasicObjectPtr binding = Binding::New();
  binding->Set(sym::_a(), args[0]);
  binding->Set(sym::_i(), SRUNumeric::NewInt(-1));
  binding->Set(sym::_j(), SRUNumeric::NewInt(size));
  binding->Set(sym::_each_internal(), array_each_internal);
  binding->Set(sym::_block(), args[1]);
  Interpreter::Instance()->DigIntoNewFrame(
      A(C(R(sym::_each_internal()))),
      binding);
}

DEFINE_SRU_PROC(ArrayEmptyQ){
  CHECK(false) << "Not impliment empty";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayFetch){
  CHECK(false) << "Not impliment fetch";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayFill){
  CHECK(false) << "Not impliment fill";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayFlatten){
  CHECK(false) << "Not impliment flatten";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayFlattenEx){
  CHECK(false) << "Not impliment flattenex";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayIndex){
  CHECK(false) << "Not impliment index";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayInsert){
  CHECK(false) << "Not implimenti insert";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayJoin){
  CHECK(false) << "Not impliment join";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayLast){
  CHECK(false) << "Not impliment last";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayLength){
  CHECK(false) << "Not impliment length";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayMapEx){
  CHECK(false) << "Not impliment mapex";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayNItems){
  CHECK(false) << "Not impliment nitems";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayPack){
  CHECK(false) << "Not impliment pack";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayPermutation){
  CHECK(false) << "Not impliment permutation";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayPop){
  CHECK(false) << "Not impliment pop";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayProduct){
  CHECK(false) << "Not impliment product";
  return Library::Instance()->Nil();
}

// TODO: Test this.
DEFINE_SRU_PROC(ArrayPush){
  ARGLEN(2);
  Array* array = args[0]->GetData<Array>();
  DCHECK(array) << "Array not found.";
  for(size_t i=1;i<args.size();i++){
    array->GetValue()->push_back(args[i].get());
  }
  return args[0];
}

DEFINE_SRU_PROC(ArrayRAssoc){
  CHECK(false) << "Not impliment rassoc";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayRejectEx){
  CHECK(false) << "Not impliment rejectex";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayReplace){
  ARGLEN(2);
  Array* self = args[0]->GetData<Array>();
  DCHECK(self) << "Array not found.";
  Array* array = args[1]->GetData<Array>();
  DCHECK(array) << "Array not found.";
  object_vector* that = array->GetValue();
  self->GetValue()->assign(that->begin(), that->end());
  return args[0];
}

DEFINE_SRU_PROC(ArrayReverse){
  ARGLEN(1);
  Array* self = args[0]->GetData<Array>();
  DCHECK(self) << "Array not found.";
  BasicObjectPtr ret = Array::New();
  Array* array = ret->GetData<Array>();
  object_vector* that = self->GetValue();
  array->GetValue()->assign(that->rbegin(), that->rend());
  return ret;
}

DEFINE_SRU_PROC(ArrayReverseEx){
  ARGLEN(1);
  Array* self = args[0]->GetData<Array>();
  DCHECK(self) << "Array not found.";
  object_vector* that = self->GetValue();
  reverse(that->begin(), that->end());
  return args[0];
}

DEFINE_SRU_PROC(ArrayRIndex){
  CHECK(false) << "Not impliment rindex";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayShift){
  CHECK(false) << "Not impliment shift";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayShuffle){
  CHECK(false) << "Not impliment shuffle";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayShuffleEx){
  CHECK(false) << "Not impliment suffleex";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArraySize){
  ARGLEN(1);
  Array* self = args[0]->GetData<Array>();
  DCHECK(self) << "Array not found.";
  return SRUNumeric::NewInt(self->GetValue()->size());
}

DEFINE_SRU_PROC(ArraySliceEx){
  CHECK(false) << "Not impliment sliceex";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArraySortEx){
  CHECK(false) << "Not impliment sortex";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayToArray){
  ARGLEN(1);
  return args[0];
}

DEFINE_SRU_PROC(ArrayTranspose){
  CHECK(false) << "Not impliment transpose";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayUniq){
  CHECK(false) << "Not impliment uniq";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayUniqEx){
  CHECK(false) << "Not impliment uniqex";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayUnShift){
  CHECK(false) << "Not impliment unshift";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayValuesAt){
  CHECK(false) << "Not impliment valuesat";
  return Library::Instance()->Nil();
}

void Array::InitializeClass(const BasicObjectPtr& array){
  Class::SetAsSubclass(array,NULL);
  array->Set(sym::__name(), SRUString::New(sym::Array()));
  array->Set(sym::mew(), CREATE_SRU_PROC(ArrayNew));

#define DEFMETHOD(name, proc) \
  Class::SetAsInstanceMethod(array, sym::name(), CREATE_SRU_PROC(proc))

  DEFMETHOD(amp, ArrayAmp);
  DEFMETHOD(asterisk, ArrayAsterisk);
  DEFMETHOD(plus, ArrayPlus);
  DEFMETHOD(minus, ArrayMinus);
  DEFMETHOD(pipe, ArrayPipe);
  DEFMETHOD(ltlt, ArrayPush);
  DEFMETHOD(get, ArrayGet);
  DEFMETHOD(at, ArrayGet);
  DEFMETHOD(slice, ArrayGet);
  DEFMETHOD(set, ArraySet);

  DEFMETHOD(assoc, ArrayAssoc);
  DEFMETHOD(choice, ArrayChoice);
  DEFMETHOD(clear, ArrayClear);
  DEFMETHOD(collectEx, ArrayCollectEx);
  DEFMETHOD(combination, ArrayCombination);
  DEFMETHOD(compact, ArrayCompact);
  DEFMETHOD(compactEx, ArrayCompactEx);
  DEFMETHOD(concat, ArrayConcat);
  DEFMETHOD(derete, ArrayDelete);
  DEFMETHOD(dereteIf, ArrayDeleteIf);
  DEFMETHOD(each, ArrayEach);
  DEFMETHOD(eachIndex, ArrayEach);
  DEFMETHOD(emptyQ, ArrayEmptyQ);
  DEFMETHOD(fetch, ArrayFetch);
  DEFMETHOD(fill, ArrayFill);
  DEFMETHOD(flatten, ArrayFlatten);
  DEFMETHOD(flattenEx, ArrayFlattenEx);
  DEFMETHOD(index, ArrayIndex);
  DEFMETHOD(insert, ArrayInsert);
  DEFMETHOD(join, ArrayJoin);
  DEFMETHOD(last, ArrayLast);
  DEFMETHOD(length, ArrayLength);
  DEFMETHOD(mapEx, ArrayMapEx);
  DEFMETHOD(nitems, ArrayNItems);
  DEFMETHOD(pack, ArrayPack);
  DEFMETHOD(permutation, ArrayPermutation);
  DEFMETHOD(pop, ArrayPop);
  DEFMETHOD(product, ArrayProduct);
  DEFMETHOD(push, ArrayPush);
  DEFMETHOD(rassoc, ArrayRAssoc);
  DEFMETHOD(rejectEx, ArrayRejectEx);
  DEFMETHOD(replace, ArrayReplace);
  DEFMETHOD(reverse, ArrayReverse);
  DEFMETHOD(reverseEx, ArrayReverseEx);
  DEFMETHOD(rindex, ArrayRIndex);
  DEFMETHOD(shift, ArrayShift);
  DEFMETHOD(shuffle, ArrayShuffle);
  DEFMETHOD(shuffleEx, ArrayShuffleEx);
  DEFMETHOD(size, ArraySize);
  DEFMETHOD(sliceEx, ArraySliceEx);
  DEFMETHOD(sortEx, ArraySortEx);
  DEFMETHOD(toA, ArrayToArray);
  DEFMETHOD(transpose, ArrayTranspose);
  DEFMETHOD(uniq, ArrayUniq);
  DEFMETHOD(uniqEx, ArrayUniqEx);
  DEFMETHOD(unshift, ArrayUnShift);
  DEFMETHOD(valuesAt, ArrayValuesAt);

#undef DEFMETHOD
}

BasicObjectPtr Array::New(){
  const BasicObjectPtr ret = BasicObject::New(new Array());
  Class::InitializeInstance(ret, Library::Instance()->Array());
  return ret;
}

BasicObjectPtr Array::New(const ptr_vector& array){
  Array* a = new Array();
  a->value = Conv(array);
  const BasicObjectPtr ret = BasicObject::New(a);
  Class::InitializeInstance(ret, Library::Instance()->Array());
  return ret;
}

Array::Array():value() {
}

Array::~Array() {
}

void Array::Dispose(){
  delete this;
}

void Array::Mark(){
  for(object_vector::iterator it = value.begin();
      it != value.end();
      it++){
    (*it)->Mark();
  }
}

std::string Array::Inspect(){
  ostringstream os;
  os << "Array ";
  os << "size=" << value.size() << " ";
  os << "[";
  for(object_vector::iterator it = value.begin();
      it != value.end();
      it++){
    if(it != value.begin()) os << ", ";
    os << (*it)->Inspect();
  }
  os << "]";
  return os.str();
}

} // namespace sru
