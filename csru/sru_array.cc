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

  Class::SetAsInstanceMethod(array, sym::amp(), CREATE_SRU_PROC(ArrayAmp));
  Class::SetAsInstanceMethod(array, sym::asterisk(), CREATE_SRU_PROC(ArrayAsterisk));
  Class::SetAsInstanceMethod(array, sym::plus(), CREATE_SRU_PROC(ArrayPlus));
  Class::SetAsInstanceMethod(array, sym::minus(), CREATE_SRU_PROC(ArrayMinus));
  Class::SetAsInstanceMethod(array, sym::pipe(), CREATE_SRU_PROC(ArrayPipe));
  Class::SetAsInstanceMethod(array, sym::ltlt(), CREATE_SRU_PROC(ArrayLtLt));
  Class::SetAsInstanceMethod(array, sym::get(), CREATE_SRU_PROC(ArrayGet));
  Class::SetAsInstanceMethod(array, sym::at(), CREATE_SRU_PROC(ArrayGet));
  Class::SetAsInstanceMethod(array, sym::slice(), CREATE_SRU_PROC(ArrayGet));
  Class::SetAsInstanceMethod(array, sym::set(), CREATE_SRU_PROC(ArraySet));

  Class::SetAsInstanceMethod(array, sym::assoc(), CREATE_SRU_PROC(ArrayAssoc));
  Class::SetAsInstanceMethod(array, sym::choice(), CREATE_SRU_PROC(ArrayChoice));
  Class::SetAsInstanceMethod(array, sym::clear(), CREATE_SRU_PROC(ArrayClear));
  Class::SetAsInstanceMethod(array, sym::collectEx(), CREATE_SRU_PROC(ArrayCollectEx));
  Class::SetAsInstanceMethod(array, sym::combination(), CREATE_SRU_PROC(ArrayCombination));
  Class::SetAsInstanceMethod(array, sym::compact(), CREATE_SRU_PROC(ArrayCompact));
  Class::SetAsInstanceMethod(array, sym::compactEx(), CREATE_SRU_PROC(ArrayCompactEx));
  Class::SetAsInstanceMethod(array, sym::concat(), CREATE_SRU_PROC(ArrayConcat));
  Class::SetAsInstanceMethod(array, sym::derete(), CREATE_SRU_PROC(ArrayDelete));
  Class::SetAsInstanceMethod(array, sym::derete_if(), CREATE_SRU_PROC(ArrayDeleteIf));
  Class::SetAsInstanceMethod(array, sym::each(), CREATE_SRU_PROC(ArrayEach));
  Class::SetAsInstanceMethod(array, sym::each_index(), CREATE_SRU_PROC(ArrayEachIndex));
  Class::SetAsInstanceMethod(array, sym::emptyQ(), CREATE_SRU_PROC(ArrayEmptyQ));
  Class::SetAsInstanceMethod(array, sym::fetch(), CREATE_SRU_PROC(ArrayFetch));
  Class::SetAsInstanceMethod(array, sym::fill(), CREATE_SRU_PROC(ArrayFill));
  Class::SetAsInstanceMethod(array, sym::flatten(), CREATE_SRU_PROC(ArrayFlatten));
  Class::SetAsInstanceMethod(array, sym::flattenEx(), CREATE_SRU_PROC(ArrayFlattenEx));
  Class::SetAsInstanceMethod(array, sym::index(), CREATE_SRU_PROC(ArrayIndex));
  Class::SetAsInstanceMethod(array, sym::insert(), CREATE_SRU_PROC(ArrayInsert));
  Class::SetAsInstanceMethod(array, sym::join(), CREATE_SRU_PROC(ArrayJoin));
  Class::SetAsInstanceMethod(array, sym::last(), CREATE_SRU_PROC(ArrayLast));
  Class::SetAsInstanceMethod(array, sym::length(), CREATE_SRU_PROC(ArrayLength));
  Class::SetAsInstanceMethod(array, sym::mapEx(), CREATE_SRU_PROC(ArrayMapEx));
  Class::SetAsInstanceMethod(array, sym::nitems(), CREATE_SRU_PROC(ArrayNItems));
  Class::SetAsInstanceMethod(array, sym::pack(), CREATE_SRU_PROC(ArrayPack));
  Class::SetAsInstanceMethod(array, sym::permutation(), CREATE_SRU_PROC(ArrayPermutation));
  Class::SetAsInstanceMethod(array, sym::pop(), CREATE_SRU_PROC(ArrayPop));
  Class::SetAsInstanceMethod(array, sym::product(), CREATE_SRU_PROC(ArrayProduct));
  Class::SetAsInstanceMethod(array, sym::push(), CREATE_SRU_PROC(ArrayPush));
  Class::SetAsInstanceMethod(array, sym::rassoc(), CREATE_SRU_PROC(ArrayRAssoc));
  Class::SetAsInstanceMethod(array, sym::rejectEx(), CREATE_SRU_PROC(ArrayRejectEx));
  Class::SetAsInstanceMethod(array, sym::replace(), CREATE_SRU_PROC(ArrayReplace));
  Class::SetAsInstanceMethod(array, sym::reverse(), CREATE_SRU_PROC(ArrayReverse));
  Class::SetAsInstanceMethod(array, sym::reverseEx(), CREATE_SRU_PROC(ArrayReverseEx));
  Class::SetAsInstanceMethod(array, sym::rindex(), CREATE_SRU_PROC(ArrayRIndex));
  Class::SetAsInstanceMethod(array, sym::shift(), CREATE_SRU_PROC(ArrayShift));
  Class::SetAsInstanceMethod(array, sym::shuffle(), CREATE_SRU_PROC(ArrayShuffle));
  Class::SetAsInstanceMethod(array, sym::shuffleEx(), CREATE_SRU_PROC(ArrayShuffleEx));
  Class::SetAsInstanceMethod(array, sym::size(), CREATE_SRU_PROC(ArraySize));
  Class::SetAsInstanceMethod(array, sym::sliceEx(), CREATE_SRU_PROC(ArraySliceEx));
  Class::SetAsInstanceMethod(array, sym::sortEx(), CREATE_SRU_PROC(ArraySortEx));
  Class::SetAsInstanceMethod(array, sym::to_a(), CREATE_SRU_PROC(ArrayToArray));
  Class::SetAsInstanceMethod(array, sym::transpose(), CREATE_SRU_PROC(ArrayTranspose));
  Class::SetAsInstanceMethod(array, sym::uniq(), CREATE_SRU_PROC(ArrayUniq));
  Class::SetAsInstanceMethod(array, sym::uniqEx(), CREATE_SRU_PROC(ArrayUniqEx));
  Class::SetAsInstanceMethod(array, sym::unshift(), CREATE_SRU_PROC(ArrayUnShift));
  Class::SetAsInstanceMethod(array, sym::values_at(), CREATE_SRU_PROC(ArrayValuesAt));
  //Class::SetAsInstanceMethod(array, sym::(), CREATE_SRU_PROC());
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
