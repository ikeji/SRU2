// Programing Language SRU
// Copyright(C) 2005-2010 IKeJI
//

#include "utils.h"

#include <string>
#include "logging.h"
#include "library.h"

using namespace std;

namespace sru {

namespace {

bool IsLf(char c){
  return c == '\n' || c == '\r';
}

void ShowLastLine(const string& src, size_t pos){
  size_t cur = pos;
  if(cur == 0) return;
  // If cursor is in end of line, move back.
  if(IsLf(src[cur])) cur--;

  // Move cursor to last line.
  while(true){
    // If this is first line, just return.
    if(cur == 0) return;
    if(IsLf(src[cur])) break;
    cur--;
  }
  cur--;
  size_t end_of_last_line = cur;

  // Move cursor to start of last line.
  while(true){
    if(IsLf(src[cur])) break;
    if(cur == 0) break;
    cur--;
  }
  if(IsLf(src[cur])) cur++;
  size_t start_of_last_line = cur;

  LOG_ALWAYS << src.substr(start_of_last_line,
      end_of_last_line-start_of_last_line + 1);
}

void ShowNextLine(const string& src, size_t pos){
  size_t cur = pos;
  // If cursor is in end of line, move back.
  if(IsLf(src[cur])) cur--;

  // Move cursor to next line.
  while(true){
    if(IsLf(src[cur])) break;
    // If this is last line, just return.
    if(cur >= src.size()) return;
    cur++;
  }
  cur++;
  size_t start_of_next_line = cur;

  // Move cursor to end of next line.
  while(true){
    if(IsLf(src[cur])) break;
    if(cur >= src.size()) break;
    cur++;
  }
  cur--;
  size_t end_of_next_line = cur;

  LOG_ALWAYS << src.substr(start_of_next_line,
      end_of_next_line-start_of_next_line + 1);
}

}


void PrintErrorPosition(const string& src, size_t pos){
  // Unmatchd src.
  if (src.size() < pos) return;

  ShowLastLine(src,pos);

  size_t start = pos;
  // If pos is end of line, we use previous line.
  if(IsLf(src[start])) start--;

  // Find start of this line.
  while(true){
    if(IsLf(src[start])){
      start++;
      break;
    }
    if(start == 0) break;
    start--;
  }
  size_t end = pos;
  string post = "";
  while(true){
    if(end >= src.size()) break;
    if(IsLf(src[end])){
      end--;
      break;
    }
    if(end-start > 50){
      post = " ... ";
      break;
    }
    end++;
  }
  LOG_ALWAYS << src.substr(start, end-start + 1) << post;
  string spc = "";
  for(size_t i=0;i<pos-start;i++) spc+=" ";
  LOG_ALWAYS << spc + "^";
  
  ShowNextLine(src,pos);
}

bool IsNil(const BasicObjectPtr& ptr){
  return IsNil(ptr.get());
}

bool IsNil(const BasicObject* ptr){
  return ptr == NULL ||
    ptr == Library::Instance()->Nil().get();
}

}  // namespace sru
