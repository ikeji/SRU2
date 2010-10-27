// Programing Language SRU
// Copyright(C) 2005-2010 IKeJI
// 

#include <string>
#ifdef USEOLDGCC
#include <ext/hash_map>
#else
#include <tr1/unordered_map>
#endif
#include "native_proc.h"
#include "sru_string.h"
#include "numeric.h"
#include "library.h"
#include "logging.h"
#include "ast.h"
#include "constants.h"
#include "parser_util.h"

// TODO: remove this dependency
#include "testing_ast.h"

using namespace sru;
using namespace std;
using namespace sru_test;

namespace sru_parser {
namespace memoize {

#ifdef USEOLDGCC
#define HashMap __gnu_cxx::hash_map
#else
#define HashMap std::tr1::unordered_map
#endif

typedef HashMap<int, BasicObjectPtr> result_map;

typedef HashMap<BasicObject*,  // func
            HashMap<BasicObject*, // src
                HashMap<int, // pos
                    BasicObjectPtr> // result
            >
        > memory_map;

memory_map memory;

BasicObjectPtr GetFromMemoize(
  const BasicObjectPtr& func,
  const BasicObjectPtr& src,
  const BasicObjectPtr& pos) {
  int p = SRUNumeric::GetIntValue(pos);
  result_map& rmap = memory[func.get()][src.get()];
  HashMap<int, BasicObjectPtr>::iterator it = rmap.find(p);
  if(it != rmap.end()) return it->second;
  return NULL;
}

BasicObjectPtr SetToMemoize(
  const BasicObjectPtr& func,
  const BasicObjectPtr& src,
  const BasicObjectPtr& pos,
  const BasicObjectPtr& result) {
  int p = SRUNumeric::GetIntValue(pos);
  return memory[func.get()][src.get()][p] = result;
}

void CleanUpMemoize(){
  memory.clear();
}

}  // namespace memoize

DEFINE_SRU_PROC(memoize) { // this, func, src, pos, result
  PARGCHK();
  PARSER_CHECK(args.size() >= 5, args[3], "Internal parser error.");
  return memoize::SetToMemoize(args[1], args[2], args[3], args[4]);
}

DEFINE_SRU_PROC(clearMemoize) { // this, arg
  memoize::CleanUpMemoize();
  if(args.size() < 2) return Library::Instance()->Nil();
  return args[1];
}

}  // namespace sru_parser
