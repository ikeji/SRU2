// Programing Language SRU
// Copyright(C) 2005-2010 IKeJI
//

#include "symbol.h"
#include "logging.h"

using namespace sru;

symbol::symbol_hash symbol::symbol_table;
int symbol::next = 1;

symbol symbol::from_id(int id){
  for(symbol_hash::iterator it = symbol_table.begin();
      it != symbol_table.end();
      it++){
    if(it->second == id)
      return symbol(it->first.c_str());
  }
  CHECK(false) << "Couldn't found symbol";
  return symbol("");
}
