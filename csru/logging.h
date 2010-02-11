// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
//

#ifndef LOGGING_H_
#define LOGGING_H_

#include <string>
#include <iostream>
#include <ostream>

namespace sru_logging{

class Logging{
 public:
  Logging(const char* fname, int line){
    std::cout << "[" << fname << ":" << line << "] ";
  }
  ~Logging(){
    std::cout << std::endl;
  }
  std::ostream& cout(){
    return std::cout;
  }
 private:
  Logging(const Logging&);
  Logging &operator=(const Logging&);
};

} // namespace sru_logging

#define LOGOBJ(name) \
  sru_logging::Logging name(__FILE__,__LINE__);

#ifdef DEBUG_INFO

#define LOG \
  if(true) sru_logging::Logging(__FILE__, __LINE__).cout()
#define IF_DEBUG \
  if(true)

#else

#define LOG \
  if(false) sru_logging::Logging(__FILE__, __LINE__).cout()
#define IF_DEBUG \
  if(false)

#endif // DEBUG

#ifdef DEBUG

#define CHECK(assert) \
  if(!(assert)) sru_logging::Logging(__FILE__, __LINE__).cout()
#define LOG_ERROR \
  if(true) sru_logging::Logging(__FILE__, __LINE__).cout()

#else

// We don't check 'assert' without DEBUG option.
// TODO: Evaluate 'assert'?
#define CHECK(assert) \
  if(false) sru_logging::Logging(__FILE__, __LINE__).cout()
#define LOG_ERROR \
  if(false) sru_logging::Logging(__FILE__, __LINE__).cout()

#endif // DEBUG

#endif // LOGGING_H_
