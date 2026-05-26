#include <csru/basic_object.h>
#include <csru/library.h>
#include <csru/native_proc.h>
#include <csru/numeric.h>
#include <csru/sru_array.h>
#include <csru/sru_string.h>
#include <iostream>

extern "C" {
#include <dlfcn.h>
#include <ffi.h>
#include <string.h>
#include <stdlib.h>
}

using namespace sru;
using namespace std;

namespace {

class Type {
 public:
  ffi_type* t;
  ffi_type* getFFIType() { return t; }
  Type(ffi_type* t) : t(t) {}
  virtual ~Type() {}
  virtual BasicObjectPtr call(ffi_cif* cif, void* p, void* values[]) = 0;
  virtual void* unboxing(BasicObjectPtr o) = 0;

  Type(const Type& obj);
  Type &operator=(const Type &obj);
};

template <typename T>
class TypeTpl : public Type {
 public:
  TypeTpl(ffi_type* t) : Type(t) {}
  virtual ~TypeTpl() {}
  BasicObjectPtr call(ffi_cif* cif, void* p, void* values[]) {
    T rv;
    if (sizeof(T) < sizeof(ffi_arg)) {
      ffi_arg r;
      ffi_call(cif, FFI_FN(p), &r, values);
      memcpy(&rv, &r, sizeof(T));
    } else {
      ffi_call(cif, FFI_FN(p), &rv, values);
    }
    return boxing(rv);
  }
  virtual BasicObjectPtr boxing(T t) {
    return Library::Instance()->Nil();
  }

  TypeTpl(const TypeTpl& obj);
  TypeTpl &operator=(const TypeTpl &obj);
};

template <typename T>
class IntTypeTpl : public TypeTpl<T> {
 public:
  IntTypeTpl(ffi_type*t) : TypeTpl<T>(t) {}
  virtual ~IntTypeTpl() {}
  BasicObjectPtr boxing(T v) {
    return SRUNumeric::NewInt(v);
  }
  virtual void* unboxing(BasicObjectPtr o) {
    int i;
    DCHECK(SRUNumeric::TryGetIntValue(o, &i))
        << "Mismatch between type and argument: "
        << o->Inspect();
    return new T(i);
  }
  IntTypeTpl(const IntTypeTpl& obj);
  IntTypeTpl &operator=(const IntTypeTpl &obj);
};

template <typename T>
class FloatTypeTpl : public TypeTpl<T> {
 public:
  FloatTypeTpl(ffi_type*t) : TypeTpl<T>(t) {}
  virtual ~FloatTypeTpl() {}
  BasicObjectPtr boxing(T v) {
    return SRUNumeric::NewDouble(v);
  }
  virtual void* unboxing(BasicObjectPtr o) {
    double i;
    DCHECK(SRUNumeric::TryGetDoubleValue(o, &i))
        << "Mismatch between type and argument: "
        << o->Inspect();
    return new T(i);
  }

  FloatTypeTpl(const FloatTypeTpl& obj);
  FloatTypeTpl &operator=(const FloatTypeTpl &obj);
};

template<>
class TypeTpl<void> : public Type {
 public:
  TypeTpl(ffi_type* t) : Type(t) {}
  BasicObjectPtr call(ffi_cif* cif, void* p, void* values[]) {
    ffi_call(cif, FFI_FN(p), NULL, values);
    return Library::Instance()->Nil();
  }
  virtual void* unboxing(BasicObjectPtr o) {
    DCHECK(false) << "No void argument allowed.";
    return NULL;
  }
};

template<>
class TypeTpl<void*> : public Type {
 public:
  TypeTpl(ffi_type* t) : Type(t) {}
  BasicObjectPtr call(ffi_cif* cif, void* p, void* values[]) {
    void* rv;
    ffi_call(cif, FFI_FN(p), &rv, values);
    return Library::Instance()->Nil();
  }
  BasicObjectPtr boxing(void* v) {
    return SRUString::New(symbol((char*)v));
  }
  virtual void* unboxing(BasicObjectPtr o) {
    string val = SRUString::GetValue(o).to_str();
    void* r = malloc(val.size() + 1);
    memcpy(r, val.c_str(), val.size());
    ((char*)r)[val.size()] = '\0';
    return new (void*)(r);
  }
};

Type* getTypeByString(const string& typeName) {

#define TYPE(name, type) \
  if (typeName == #name) \
  return new TypeTpl<type>(&ffi_type_##name)
#define INT_TYPE(name, type) \
  if (typeName == #name) \
    return new IntTypeTpl<type>(&ffi_type_##name)
#define FLOAT_TYPE(name, type) \
  if (typeName == #name) \
    return new FloatTypeTpl<type>(&ffi_type_##name)

  INT_TYPE(uchar, unsigned char);
  INT_TYPE(schar, char);
  INT_TYPE(ushort, unsigned short);
  INT_TYPE(sshort, short);
  INT_TYPE(uint, unsigned int);
  INT_TYPE(sint, int);
  INT_TYPE(ulong, unsigned long);
  INT_TYPE(slong, long);
  // INT_TYPE(uint8);
  // INT_TYPE(sint8);
  // INT_TYPE(uint16);
  // INT_TYPE(sint16);
  // INT_TYPE(uint32);
  // INT_TYPE(sint32);
  // INT_TYPE(uint64);
  // INT_TYPE(sint64);
  FLOAT_TYPE(float, float);
  FLOAT_TYPE(double, double);

  TYPE(void, void);
  TYPE(pointer, void*);

#undef TYPE
  return NULL;
}

}  // namespace


DEFINE_SRU_PROC(ffi_call){ // (filename, funcname, rettype, arg_types, arg_list)
  ARGLEN(5);
  string fname = SRUString::GetValue(args[0]).to_str();
  string funcname = SRUString::GetValue(args[1]).to_str();
  string rettype = SRUString::GetValue(args[2]).to_str();
  Array* arg_types = args[3]->GetData<Array>();
  DCHECK(arg_types) << "4th argument should be array: " << args[3]->Inspect();
  Array* arg_list = args[4]->GetData<Array>();
  DCHECK(arg_types) << "5th argument should be array: " << args[4]->Inspect();

  void* h = dlopen(fname.c_str(), RTLD_NOW);
  DCHECK(h) << "Couldn't load " << fname;
  void* p = dlsym(h, funcname.c_str());
  DCHECK(p) << "Couldn't find " << funcname << " on " << fname;

  Type* rt = getTypeByString(rettype);
  DCHECK(rt) << "Return type error " << rettype;
  ffi_cif cif;
  const unsigned int num_args = arg_types->GetValue()->size();
  ffi_type* ffi_args[num_args+1];
  void* values[num_args+1];
  for (unsigned int i=0;i < num_args;i++) {
    string t = SRUString::GetValue(arg_types->GetValue()->at(i)).to_str();
    Type* at = getTypeByString(t);
    DCHECK(at) << i+1 << "th argument(" << t << ") is unknown";
    ffi_args[i] = at->getFFIType();
    if (arg_list->GetValue()->size() > i) {
      values[i] = at->unboxing(arg_list->GetValue()->at(i));
    } else {
      values[i] = new (void*)(NULL);
    }
  }
  ffi_type* ffi_rettype = rt->getFFIType();
  DCHECK(
      ffi_prep_cif(
          &cif, FFI_DEFAULT_ABI, num_args, ffi_rettype, ffi_args)
      == FFI_OK)
      << "Couldn't prepare " << funcname << " on " << fname;

  BasicObjectPtr r = rt->call(&cif, p, values);
  for (unsigned int i=0;i < num_args;i++) {
    //delete values[i];
  }
  delete rt;
  return r;
}

extern "C" bool require(const BasicObjectPtr& env){
  env->Set(symbol("ffi_call"), CREATE_SRU_PROC(ffi_call));
  return true;
};
