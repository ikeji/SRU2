// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
//

#ifndef LOGGING_H_
#define LOGGING_H_

#include <ostream>

namespace sru_logging{

class Logging{
 public:
  Logging(const char* fname, int line, bool cond=true);
  ~Logging();
  std::ostream& ostream();
 private:
  bool cond;
  Logging(const Logging&);
  Logging &operator=(const Logging&);
};

} // namespace sru_logging

#define LOGOBJ(name) \
  sru_logging::Logging name(__FILE__,__LINE__);

#define LOG_ALWAYS \
  sru_logging::Logging(__FILE__,__LINE__).ostream()


#ifdef DEBUG

#define CHECK(assert) \
  if(!(assert)) sru_logging::Logging(__FILE__, __LINE__,assert).ostream()
#define LOG_ERROR \
  if(true) sru_logging::Logging(__FILE__, __LINE__).ostream()

#else

// We don't check 'assert' without DEBUG option.
// TODO: Evaluate 'assert'?
#define CHECK(assert) \
  if(false) sru_logging::Logging(__FILE__, __LINE__).ostream()
#define LOG_ERROR \
  if(false) sru_logging::Logging(__FILE__, __LINE__).ostream()

#endif // DEBUG


#ifdef DEBUG_INFO

#define LOG \
  if(true) sru_logging::Logging(__FILE__, __LINE__).ostream()
#define IF_DEBUG_INFO \
  if(true)

#else

#define LOG \
  if(false) sru_logging::Logging(__FILE__, __LINE__).ostream()
#define IF_DEBUG_INFO \
  if(false)

#endif // DEBUG_INFO


#ifdef DEBUG_TRACE

#define LOG_TRACE \
  if(true) sru_logging::Logging(__FILE__, __LINE__).ostream()
#define IF_DEBUG_TRACE \
  if(true)

#else

#define LOG_TRACE \
  if(false) sru_logging::Logging(__FILE__, __LINE__).ostream()
#define IF_DEBUG_TRACE \
  if(false)

#endif

#endif // LOGGING_H_
