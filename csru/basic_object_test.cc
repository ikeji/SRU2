#include <iostream>
#include <cassert>
#include "basic_object.h"

using namespace std;
using namespace sru;
int main(){
  // TODO: need test
  BasicObject *obj = BasicObject::New();
  assert(obj);
  cout<< "OK" << endl;
}
