// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "testing.h"
#include "proc.h"

#include <iostream>
#include <cassert>
#include <vector>

using namespace std;
using namespace sru;

TEST(Proc_NewTest){
  vector<string> varg;
  string retval;
  ptr_vector expressions;
  assert(Proc::New(varg,retval,expressions).get());
}

