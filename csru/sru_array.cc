// Copyright(C) 2005-2008 IKeJI
//

#include "sru_array.h"

#include <sstream>
#include <vector>
#include "basic_object.h"
#include "class.h"
#include "native_proc.h"
#include "constants.h"
#include "sru_string.h"
#include "library.h"
#include "object_container.h"
#include "logging.h"
#include "numeric.h"

using namespace sru;
using namespace std;

namespace sru {

DEFINE_SRU_PROC(ArrayNew){
  return Array::New();
}

DEFINE_SRU_PROC(ArrayAmp){
  ARGLEN(2);
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayAsterisk){
  ARGLEN(2);
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayPlus){
  ARGLEN(2);
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayMinus){
  ARGLEN(2);
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayPipe){
  ARGLEN(2);
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

// TODO: Need <->

DEFINE_SRU_PROC(ArrayGet){ // Slice
  ARGLEN(2);
  // TODO: impliment array[1,2]
  // TODO: impliment array[1..3]
  Array* array = args[0]->GetData<Array>();
  CHECK(array) << "Array not found.";
  int index = 0;
  CHECK(SRUNumeric::TryGetValue(args[1], &index));
  LOG_ALWAYS << "index: " << index;
  if ((int)array->GetValue()->size() > index){
    if(index < 0) index += array->GetValue()->size();
    if(index >= 0){
      return array->GetValue()->at(index);
    }
  }
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArraySet){
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayAssoc){
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayChoice){
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayClear){
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayCollectEx){
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayCombination){
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayCompact){
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayCompactEx){
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayConcat){
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayDelete){
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayDeleteIf){
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayEach){
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayEachIndex){
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayEmptyQ){
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayFetch){
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayFill){
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayFlatten){
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayFlattenEx){
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayIndex){
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayInsert){
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayJoin){
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayLast){
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayLength){
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayMapEx){
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayNItems){
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayPack){
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayPermutation){
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayPop){
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayProduct){
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

// TODO: Test this.
DEFINE_SRU_PROC(ArrayPush){
  ARGLEN(2);
  Array* array = args[0]->GetData<Array>();
  CHECK(array) << "Array not found.";
  for(size_t i=1;i<args.size();i++){
    array->GetValue()->push_back(args[i].get());
  }
  return args[0];
}

DEFINE_SRU_PROC(ArrayRAssoc){
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayRejectEx){
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayReplace){
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayReverse){
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayReverseEx){
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayRIndex){
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayShift){
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayShuffle){
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayShuffleEx){
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArraySize){
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArraySliceEx){
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArraySortEx){
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayToArray){
  ARGLEN(1);
  return args[0];
}

DEFINE_SRU_PROC(ArrayTranspose){
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayUniq){
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayUniqEx){
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayUnShift){
  CHECK(false) << "Not impliment";
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayValuesAt){
  CHECK(false) << "Not impliment";
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
  DEFMETHOD(eachIndex, ArrayEachIndex);
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
