#include <iostream>
#include <cassert>
#include "basic_object.h"

using namespace std;
using namespace sru;

void createObjectTest(){
  BasicObject *obj = BasicObject::New();
  assert(obj);
}

void objectPtrTest(){
  BasicObjectPtr p = BasicObject::New();
  BasicObjectPtr p2 = p.get();
  BasicObjectPtr p3;
  p3 = p2;
  assert( p3.get() == p2.get() );

  BasicObjectPtr p4;
  assert( p4.get() == NULL );
}

int main(){
  createObjectTest();
  objectPtrTest();
  cout<< "OK" << endl;
}
