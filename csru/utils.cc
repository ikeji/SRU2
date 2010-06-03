// Programing Language SRU
// Copyright(C) 2005-2010 IKeJI
//

#include "utils.h"

#include <string>
#include "logging.h"
#include "library.h"

using namespace std;

namespace sru {

void PrintErrorPosition(const string& src, size_t pos){
  // Unmatchd src.
  if (src.size() < pos) return;
  size_t start = pos;
  while(true){
    if(src[start] == '\n' || src[start] == '\r') break;
    if(start == 0) break;
    start--;
  }
  size_t end = pos;
  string post = "";
  while(true){
    if(end >= src.size()) break;
    if(src[end] == '\n' || src[end] == '\r') break;
    if(end-start > 50){
      post = " ... ";
      break;
    }
    end++;
  }
  LOG_ALWAYS << src.substr(start, end-start) << post;
  string spc = "";
  for(size_t i=0;i<pos-start;i++) spc+=" ";
  LOG_ALWAYS << spc + "^";
}

bool IsNil(const BasicObjectPtr& ptr){
  return IsNil(ptr.get());
}

bool IsNil(const BasicObject* ptr){
  return ptr == NULL ||
    ptr == Library::Instance()->Nil().get();
}

}  // namespace sru
