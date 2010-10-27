// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include <iostream>
#include <string>
#include "interpreter.h"
#include "basic_object.h"

using namespace std;
using namespace sru;

int main(){
  cout << endl;
  cout << "C-SRU" << endl;
  while(true){
    cout << ">> ";
    string source;
    getline(cin, source);
    if(source.empty()) break;
    BasicObjectPtr r = Interpreter::Instance()->Eval(source);
    if (r != NULL) cout << r->Inspect() << endl;
    cout << endl;
  }
}
