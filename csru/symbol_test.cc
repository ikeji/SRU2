// Programing Language SRU
// Copyright(C) 2005-2010 IKeJI
//

#include "symbol.h"

#include "testing.h"
#include <cassert>

using namespace sru;

TEST(Symbol_CreateTest){
  symbol hoge("hoge");
  assert(hoge.to_str() == "hoge");
  assert(hoge.getid() != 0);
}

TEST(Symbol_DifferentIdTest){
  symbol hoge("hoge");
  symbol fuga("fuga");
  assert(hoge.getid() != fuga.getid());
}

TEST(Symbol_SamfeIdTest){
  symbol hoge("hoge");
  symbol hoge2("hoge");
  assert(hoge.getid() == hoge2.getid());
}

