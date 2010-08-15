// Copyright(C) 2005-2008 IKeJI
//

#include "sru_array.h"

#include <cassert>
#include "basic_object.h"
#include "class.h"
#include "native_proc.h"
#include "constants.h"
#include "sru_string.h"
#include "library.h"

using namespace sru;

namespace sru {

DEFINE_SRU_PROC(ArrayNew){
  return Array::New();
}

DEFINE_SRU_PROC(ArrayAmp){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayAsterisk){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayPlus){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayMinus){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayPipe){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayLtLt){
  return Library::Instance()->Nil();
}

// TODO: Need <->

DEFINE_SRU_PROC(ArrayGet){ // Slice
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArraySet){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayAssoc){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayChoice){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayClear){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayCollectEx){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayCombination){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayCompact){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayCompactEx){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayConcat){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayDelete){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayDeleteIf){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayEach){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayEachIndex){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayEmptyQ){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayFetch){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayFill){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayFlatten){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayFlattenEx){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayIndex){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayInsert){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayJoin){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayLast){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayLength){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayMapEx){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayNItems){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayPack){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayPermutation){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayPop){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayProduct){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayPush){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayRAssoc){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayRejectEx){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayReplace){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayReverse){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayReverseEx){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayRIndex){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayShift){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayShuffle){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayShuffleEx){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArraySize){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArraySliceEx){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArraySortEx){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayToArray){
  assert(args.size() >= 1);
  return args[0];
}

DEFINE_SRU_PROC(ArrayTranspose){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayUniq){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayUniqEx){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayUnShift){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(ArrayValuesAt){
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
  DEFMETHOD(ltlt, ArrayLtLt);
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
  return BasicObject::New(new Array());
}

struct Array::Impl{
  Impl(){};
};

Array::Array() : pimpl(new Impl){
}

Array::~Array(){
  delete pimpl;
}

void Array::Dispose(){
  delete this;
}

} // namespace sru
