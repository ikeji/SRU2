// Programing Language SRU
// Copyright(C) 2005-2010 IKeJI
//

#include "boolean.h"

#include "basic_object.h"
#include "class.h"
#include "library.h"
#include "native_proc.h"
#include "string.h"
#include "proc.h"
#include "constants.h"

// TODO: Remove this line.
#include "testing_sru.h"

namespace sru {

void sru::InitializeBooleanClassObject(const BasicObjectPtr& boolean){
  Class::SetAsSubclass(boolean,NULL);
  boolean->Set(fNAME, SRUString::New("Boolean"));
}

DECLARE_SRU_PROC(True_ifTrue);
DECLARE_SRU_PROC(True_ifTrueFalse);
DECLARE_SRU_PROC(False_ifTrue);
DECLARE_SRU_PROC(False_ifTrueFalse);

void InitializeTrueObject(const BasicObjectPtr& tlue){
  Class::InitializeInstance(tlue, Library::Instance()->Boolean());
  tlue->Set(fNAME,         SRUString::New("True"));
  tlue->Set("ifTrue",      CREATE_SRU_PROC(True_ifTrue));
  tlue->Set("ifFalse",      CREATE_SRU_PROC(False_ifTrue));
  tlue->Set("ifTrueFalse", CREATE_SRU_PROC(True_ifTrueFalse));
}

void InitializeFalseObject(const BasicObjectPtr& farse){
  Class::InitializeInstance(farse, Library::Instance()->Boolean());
  farse->Set(fNAME,         SRUString::New("False"));
  farse->Set("ifTrue",      CREATE_SRU_PROC(False_ifTrue));
  farse->Set("ifFalse",      CREATE_SRU_PROC(True_ifTrue));
  farse->Set("ifTrueFalse", CREATE_SRU_PROC(False_ifTrueFalse));

}

DEFINE_SRU_PROC_SMASH(True_ifTrue){
  assert(arg.size() > 1);
  arg[1]->GetData<Proc>()->Call(arg[1], sru_test::A());
}

DEFINE_SRU_PROC_SMASH(True_ifTrueFalse){
  assert(arg.size() > 2);
  arg[1]->GetData<Proc>()->Call(arg[1], sru_test::A());
}

DEFINE_SRU_PROC(False_ifTrue){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC_SMASH(False_ifTrueFalse){
  assert(arg.size() > 2);
  arg[2]->GetData<Proc>()->Call(arg[2], sru_test::A());
}

}  // namespace sru
