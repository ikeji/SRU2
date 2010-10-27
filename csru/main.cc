// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include <iostream>
#include <string>
#include "interpreter.h"
#include "basic_object.h"
#include "logging.h"

#ifdef USE_EDITLINE

extern "C" {
#include <histedit.h>
}

#endif

using namespace std;
using namespace sru;

#ifdef USE_EDITLINE
const char* prompt(EditLine *e){
  return ">> ";
}
#endif

int main(int argc, char* argv[]){
#ifdef USE_EDITLINE
  EditLine *el;
  History *hist;
  HistEvent ev;
  el = el_init(argv[0], stdin, stdout, stderr);
  el_set(el, EL_PROMPT, &prompt);
  hist = history_init();
  DCHECK(hist);
  history(hist,&ev,H_SETSIZE,8000);
  history(hist,&ev,H_LOAD,"./.sru-history");
  el_set(el,EL_HIST,history,hist);
#endif
  cout << endl;
  cout << "C-SRU" << endl;
  while(true){
#ifdef USE_EDITLINE
    int count;
    const char* line = el_gets(el,&count);
    if(line == NULL) break;
    string source(line);
    history(hist,&ev,H_ENTER,line);
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
    if (r != NULL) history(hist,&ev,H_ENTER,line);
#endif
    cout << endl;
  }
#ifdef USE_EDITLINE
  history(hist,&ev,H_SAVE,"./.sru-history");
  history_end(hist);
  el_end(el);
#endif
}
