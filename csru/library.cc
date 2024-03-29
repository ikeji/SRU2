// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "library.h"

#include "basic_object.h"
#include "numeric.h"
#include "parser.h"
#include "object.h"
#include "sru_string.h"
#include "constants.h"
#include "binding.h"
#include "boolean.h"
#include "proc.h"
#include "class.h"
#include "sru_array.h"
#include "sru_math.h"
#include "sru_sys.h"
#include "nil.h"
#include "ast_class.h"

using namespace sru;

struct Library::Impl {
  Impl():
      Nil(),
      Binding(),
      Class(),
      Object(),
      Proc(),
      Array(),
      Hash(),
      String(),
      Numeric(),
      Boolean(),
      True(),
      False(),
      Parser(),
      Math(),
      Sys(),
      Ast() {}
     
  BasicObjectPtr Nil;
  BasicObjectPtr Binding;
  BasicObjectPtr Class;
  BasicObjectPtr Object;
  BasicObjectPtr Proc;
  BasicObjectPtr Array;
  BasicObjectPtr Hash;
  BasicObjectPtr String;
  BasicObjectPtr Numeric;
  BasicObjectPtr Boolean;
  BasicObjectPtr True;
  BasicObjectPtr False;
  BasicObjectPtr Parser;
  BasicObjectPtr Math;
  BasicObjectPtr Sys;
  ASTClass* Ast;
  
  void initialiseInteralClasses();
 private:
  Impl(const Impl& obj);
  Impl &operator=(const Impl& obj);
};

BasicObjectPtr Library::Nil() const { return pimpl->Nil; }

BasicObjectPtr Library::Binding() const { return pimpl->Binding; }

BasicObjectPtr Library::Class() const { return pimpl->Class; }
BasicObjectPtr Library::Object() const { return pimpl->Object; }
BasicObjectPtr Library::Proc() const { return pimpl->Proc; }

BasicObjectPtr Library::Array() const { return pimpl->Array; }
BasicObjectPtr Library::Hash() const { return pimpl->Hash; }
BasicObjectPtr Library::String() const { return pimpl->String; }
BasicObjectPtr Library::Numeric() const { return pimpl->Numeric; }
BasicObjectPtr Library::Boolean() const { return pimpl->Boolean; }
BasicObjectPtr Library::True() const { return pimpl->True; }
BasicObjectPtr Library::False() const { return pimpl->False; }

BasicObjectPtr Library::Parser() const { return pimpl->Parser; }

BasicObjectPtr Library::Math() const { return pimpl->Math; }
BasicObjectPtr Library::Sys() const { return pimpl->Sys; }

ASTClass* Library::Ast() const { return pimpl->Ast; }

Library* Library::Instance(){
  static Library inst;
  static bool initialized = false;
  if(!initialized){
    initialized = true;
    inst.pimpl->initialiseInteralClasses();
    inst.pimpl->Ast = new ASTClass(inst);
  }
  return &inst;
}

Library::Library():pimpl(new Impl()){
}

Library::~Library(){
  delete pimpl;
}

void Library::Impl::initialiseInteralClasses(){
  LOG << "Start initializing library.";
  // Set new objects
  Nil = BasicObject::New();
  Binding = BasicObject::New();
  Class = BasicObject::New();
  Object = BasicObject::New();
  Proc = BasicObject::New();
  Array = BasicObject::New();
  Hash = BasicObject::New();
  String = BasicObject::New();
  Numeric = BasicObject::New();
  Boolean = BasicObject::New();
  True = BasicObject::New();
  False = BasicObject::New();
  Parser = BasicObject::New();
  Math = BasicObject::New();
  Sys = BasicObject::New();

  // Define class path.
  Class->Set(sym::klass(), Class);
  Object->Set(sym::klass(), Class);
  Proc->Set(sym::klass(), Class);
  Array->Set(sym::klass(), Class);
  Hash->Set(sym::klass(), Class);
  String->Set(sym::klass(), Class);
  Numeric->Set(sym::klass(), Class);
  Boolean->Set(sym::klass(), Class);
  True->Set(sym::klass(), Boolean);
  False->Set(sym::klass(), Boolean);
  Math->Set(sym::klass(), Class);
  Sys->Set(sym::klass(), Class);

  // Define inheritance path.
  Class->Set(sym::superclass(), Object);
  Object->Set(sym::superclass(), Nil);
  Proc->Set(sym::superclass(), Object);
  Array->Set(sym::superclass(), Object);
  Hash->Set(sym::superclass(), Object);
  String->Set(sym::superclass(), Object);
  Numeric->Set(sym::superclass(), Object);
  Boolean->Set(sym::superclass(), Object);
  Math->Set(sym::superclass(), Object);
  Sys->Set(sym::superclass(), Object);

  // Initialize each Objects
  // TODO: Impliment Object Initialize.
  Class::InitializeClassClassFirst(Class);

  InitializeObjectClass(Object);
  Binding::InitializeClassObject(Binding);
  SRUNumeric::InitializeClassObject(Numeric);
  sru_parser::InitializeParserObject(Parser);
  InitializeBooleanClassObject(Boolean);
  InitializeTrueObject(True);
  InitializeFalseObject(False);
  Proc::InitializeClassObject(Proc);
  Class::InitializeClassClassLast(Class);
  Array::InitializeClass(Array);
  InitializeMathClass(Math);
  InitializeSysClass(Sys);
  SRUString::InitializeStringClass(String);
  SetupNilObject(Nil);

  // TODO: Move this to hash.cc
  Hash->Set(sym::__name(), SRUString::New(sym::Hash()));
}

void Library::BindPrimitiveObjects(const BasicObjectPtr& frame){
  frame->Set(sym::nil(),Instance()->Nil());
  frame->Set(sym::Binding(),Instance()->Binding());
  frame->Set(sym::Class(),Instance()->Class());
  frame->Set(sym::Object(),Instance()->Object());
  frame->Set(sym::Proc(),Instance()->Proc());
  frame->Set(sym::Array(),Instance()->Array());
  frame->Set(sym::Hash(),Instance()->Hash());
  frame->Set(sym::String(),Instance()->String());
  frame->Set(sym::Numeric(),Instance()->Numeric());
  frame->Set(sym::Boolean(),Instance()->Boolean());
  frame->Set(sym::tlue(),Instance()->True());
  frame->Set(sym::farse(),Instance()->False());
  frame->Set(sym::__parser(), Instance()->Parser());
  frame->Set(sym::Math(), Instance()->Math());
  frame->Set(sym::Sys(), Instance()->Sys());
  frame->Set(sym::AST(), Instance()->Ast()->GetASTNamespace());
}
