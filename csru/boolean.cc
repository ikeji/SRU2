// Programing Language SRU
// Copyright(C) 2005-2010 IKeJI
//

#include "boolean.h"

#include "basic_object.h"
#include "class.h"
#include "library.h"
#include "native_proc.h"
#include "sru_string.h"
#include "proc.h"
#include "constants.h"

// TODO: Remove this line.
#include "testing_sru.h"

namespace sru {

void InitializeBooleanClassObject(const BasicObjectPtr& boolean){
  Class::SetAsSubclass(boolean,NULL);
  boolean->Set(sym::__name(), SRUString::New(sym::Boolean()));
}

DECLARE_SRU_PROC(True_ifTrue);
DECLARE_SRU_PROC(True_ifTrueFalse);
DECLARE_SRU_PROC(False_ifTrue);
DECLARE_SRU_PROC(False_ifTrueFalse);
DECLARE_SRU_PROC(ReturnSelf);
DECLARE_SRU_PROC(ReturnOther);
DECLARE_SRU_PROC(ReturnTrue);
DECLARE_SRU_PROC(ReturnFalse);

void InitializeTrueObject(const BasicObjectPtr& tlue){
  Class::InitializeInstance(tlue, Library::Instance()->Boolean());
  tlue->Set(sym::__name(),      SRUString::New(sym::True()));
  tlue->Set(sym::ifTrue(),      CREATE_SRU_PROC(True_ifTrue));
  tlue->Set(sym::ifFalse(),     CREATE_SRU_PROC(False_ifTrue));
  tlue->Set(sym::ifTrueFalse(), CREATE_SRU_PROC(True_ifTrueFalse));
  tlue->Set(sym::ampamp(),      CREATE_SRU_PROC(ReturnOther));
  tlue->Set(sym::pipepipe(),    CREATE_SRU_PROC(ReturnSelf));
  tlue->Set(sym::exclamation(), CREATE_SRU_PROC(ReturnFalse));
}

void InitializeFalseObject(const BasicObjectPtr& farse){
  Class::InitializeInstance(farse, Library::Instance()->Boolean());
  farse->Set(sym::__name(),      SRUString::New(sym::False()));
  farse->Set(sym::ifTrue(),      CREATE_SRU_PROC(False_ifTrue));
  farse->Set(sym::ifFalse(),     CREATE_SRU_PROC(True_ifTrue));
  farse->Set(sym::ifTrueFalse(), CREATE_SRU_PROC(False_ifTrueFalse));
  farse->Set(sym::ampamp(),      CREATE_SRU_PROC(ReturnSelf));
  farse->Set(sym::pipepipe(),    CREATE_SRU_PROC(ReturnOther));
  farse->Set(sym::exclamation(), CREATE_SRU_PROC(ReturnTrue));
}

DEFINE_SRU_PROC_SMASH(True_ifTrue){
  ARGLEN(2);
  Proc* p = args[1]->GetData<Proc>();
  CHECK(p) << "ifTrue requires Proc";
  p->Call(context, args[1], sru_test::A());
}

DEFINE_SRU_PROC_SMASH(True_ifTrueFalse){
  ARGLEN(3);
  Proc* p = args[1]->GetData<Proc>();
  CHECK(p) << "ifTrueFalse requires Proc";
  p->Call(context, args[1], sru_test::A());
}

DEFINE_SRU_PROC(False_ifTrue){
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC_SMASH(False_ifTrueFalse){
  ARGLEN(3);
  Proc* p = args[2]->GetData<Proc>();
  CHECK(p) << "ifTrueFalse requires Proc";
  p->Call(context, args[2], sru_test::A());
}

DEFINE_SRU_PROC(ReturnSelf){
  ARGLEN(1);
  return args[0];
}

DEFINE_SRU_PROC(ReturnOther){
  ARGLEN(2);
  return args[1];
}

DEFINE_SRU_PROC(ReturnTrue){
  ARGLEN(1);
  return Library::Instance()->True();
}

DEFINE_SRU_PROC(ReturnFalse){
  ARGLEN(1);
  return Library::Instance()->False();
}

}  // namespace sru
