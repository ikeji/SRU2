// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
//

#include <iostream>
#include <fstream>
#include <string>
#include "interpreter.h"
#include "basic_object.h"
#include "logging.h"

#ifdef USE_EDITLINE

extern "C" {
#include <histedit.h>
}
#include <cctype>
#include "symbol.h"

#endif

using namespace std;
using namespace sru;

#ifdef USE_EDITLINE
const char* prompt(EditLine *e){
  return ">> ";
}

bool isAlNumUndEx(char c){
  return
    ('a' <= c && c <= 'z') ||
    ('A' <= c && c <= 'Z') ||
    ('0' <= c && c <= '9') ||
    ('_' == c) ||
    ('!' == c) ||
    false;
}

unsigned char complete(EditLine *e, int ch){
  const char* ptr;
  const LineInfo* lf = el_line(e);
  int res = CC_ERROR;

  for(ptr = lf->cursor - 1;
      isAlNumUndEx(*ptr) && ptr >= lf->buffer;
      ptr--)
    ;
  ptr++;
  size_t len = lf->cursor - ptr;
  string base(ptr, len);

  vector<string> candiate;
  for(symbol::symbol_hash::const_iterator it = symbol::begin();
      it != symbol::end();
      it++){
    const string& key = it->first;
    if(key.find(base) == 0){
      candiate.push_back(string(key.c_str()+len));
    }
  }

  if(candiate.empty()){ // not found
    return res;
  } else if(candiate.size() == 1){ // only one candidate.
    if(el_insertstr(e, candiate[0].c_str()) == -1){
      return CC_ERROR;
    }
    return CC_REFRESH;
  } else {
    // find prefix.
    string s = candiate[0];
    for(vector<string>::const_iterator it = candiate.begin();
        it != candiate.end();
        it++){
      while(true){
        if(s.empty()) break;
        if(it->find(s) == 0) break;
        s = s.substr(0,s.size()-1);
      }
    }
    if(s.empty()) return CC_ERROR;
    if(el_insertstr(e, s.c_str()) == -1){
      return CC_ERROR;
    }
    return CC_REFRESH;
  }
}
#endif

void repl(int argc, char* argv[]){
#ifdef USE_EDITLINE
  EditLine *el;
  History *hist;
  HistEvent ev;
  el = el_init(argv[0], stdin, stdout, stderr);
  el_source(el, NULL);
  el_set(el, EL_PROMPT, &prompt);
  hist = history_init();
  DCHECK(hist);
  history(hist,&ev,H_SETSIZE,10000);
  history(hist,&ev,H_LOAD,"./.sru-history");
  el_set(el,EL_HIST,history,hist);
  el_set(el, EL_ADDFN, "ed-complete", "Complete argument", complete);
  el_set(el, EL_BIND, "^I", "ed-complete", NULL);

  // For create table for complete.
  Interpreter::Instance();
#endif
  cout << endl;
  cout << "C-SRU" << endl;
  while(true){
#ifdef USE_EDITLINE
    int count;
    const char* line = el_gets(el,&count);
    if(line == NULL) break;
    string source(line);
    //history(hist,&ev,H_ENTER,line);
#else
    cout << ">> ";
    string source;
    getline(cin, source);
#endif
    if(source[source.size()-1] == '\n')
      source = source.substr(0,source.size()-1); //chomp
    if(source.empty()) break;
    BasicObjectPtr r = Interpreter::Instance()->Eval(source);
    if (r != NULL) cout << r->Inspect() << endl;
#ifdef USE_EDITLINE
    if (r != NULL){
      history(hist,&ev,H_ENTER,line);
      history(hist,&ev,H_SAVE,"./.sru-history");
    }
#endif
    cout << endl;
  }
#ifdef USE_EDITLINE
  history(hist,&ev,H_SAVE,"./.sru-history");
  history_end(hist);
  el_end(el);
#endif
}

int main(int argc, char* argv[]){
  if(argc==1){
    repl(argc, argv);
  }else if(argc==2){
    ifstream ifs(argv[1]);
    string prog = "";
    string buf;
    while(ifs && getline(ifs,buf)){
      prog += buf + "\n";
    }
    BasicObjectPtr r = Interpreter::Instance()->Eval(prog);
    if (r != NULL) LOG << r->Inspect();
  }
}
