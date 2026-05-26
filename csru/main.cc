// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
//

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include "interpreter.h"
#include "basic_object.h"
#include "logging.h"
#include "stack_frame.h"
#include "constants.h"
#include "symbol.h"
#include "unistd.h"

#ifdef USE_EDITLINE

extern "C" {
#include <histedit.h>
}
#include <cctype>
#include <cstdlib>
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

string histfilename() {
  const char* home = getenv("HOME");
  if (home == NULL) home = ".";
  return string(home) + "/.sru-history";
}
#endif

void repl(int argc, char* argv[]){
#ifdef USE_EDITLINE
  EditLine *el;
  History *hist;
  HistEvent ev;
  string histfile = histfilename();
  el = el_init(argv[0], stdin, stdout, stderr);
  el_source(el, NULL);
  el_set(el, EL_PROMPT, &prompt);
  hist = history_init();
  DCHECK(hist);
  history(hist,&ev,H_SETSIZE,10000);
  history(hist,&ev,H_LOAD, histfile.c_str());
  el_set(el,EL_HIST,history,hist);
  el_set(el, EL_ADDFN, "ed-complete", "Complete argument", complete);
  el_set(el, EL_BIND, "^I", "ed-complete", NULL);

  // For create table for complete.
  Interpreter::Instance();
#endif
  cout << endl;
  cout << "C-SRU" << endl;
  int i = 1;
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
    if (r != NULL) {
      cout << "_" << i << " = " << r->Inspect() << endl;
      BasicObjectPtr g = Interpreter::Instance()->RootStackFrame()->Binding();
      g->Set(sym::_(), r);
      ostringstream s;
      s << "_" << i;
      g->Set(symbol(s.str()), r);
      i++;
    }
#ifdef USE_EDITLINE
    if (r != NULL){
      history(hist,&ev,H_ENTER,line);
      history(hist,&ev,H_SAVE,histfile.c_str());
    }
#endif
    cout << endl;
  }
#ifdef USE_EDITLINE
  history(hist,&ev,H_SAVE,histfile.c_str());
  history_end(hist);
  el_end(el);
#endif
}

BasicObjectPtr evalStream(istream& is) {
  string prog = "";
  string buf;
  while(is && getline(is,buf)){
    prog += buf + "\n";
  }
  BasicObjectPtr r = Interpreter::Instance()->Eval(prog);
  return r;
}

int main(int argc, char* argv[]){
  if(argc==1){
    if (isatty(STDIN_FILENO)) {
      repl(argc, argv);
    } else {
      BasicObjectPtr r = evalStream(cin);
      if (r != NULL) LOG << r->Inspect();
    }
  }else{
    ifstream ifs(argv[1]);
    BasicObjectPtr r = evalStream(ifs);
    if (r != NULL) LOG << r->Inspect();
  }
}
