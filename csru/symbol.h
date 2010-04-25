// Programing Language SRU
// Copyright(C) 2005-2010 IKeJI
//

#ifndef SYMBOL_H_
#define SYMBOL_H_

#include <string>
#ifdef USEOLDGCC
#include <ext/hash_map>
namespace __gnu_cxx {
template <>
struct hash<std::string>{
  size_t operator()(const std::string&x) const {
    return hash<const char*>()(x.c_str());
  }
};
}  // namespace __gnu_cxx;
#else
#include <tr1/unordered_map>
#endif


namespace sru {

class symbol {
 public:
  explicit symbol(const char* str) : str(str), id(0){}
  int getid() const{
    if(id == 0){
      int& i = symbol_table[str];
      if(i==0){
        i = next;
        next++;
      }
      id = i;
    }
    return id;
  }
  const std::string& to_str() const { return str; }
  symbol(const symbol& sym): str(sym.str),id(sym.id) { }
  const symbol& operator=(const symbol& sym){
    this->str = sym.str;
    this->id = sym.id;
    return *this;
  }
  static symbol from_id(int id);
  bool operator<(const symbol& other) const {
    return (str < other.str);
  }
  bool operator==(const symbol& other) const {
    return getid() == other.getid();
  }
 private:
  std::string str;
  mutable int id;
#ifdef USEOLDGCC
  typedef __gnu_cxx::hash_map<std::string,int > symbol_hash;
#else
  typedef std::tr1::unordered_map<std::string,int > symbol_hash;
#endif
  static symbol_hash symbol_table;
  static int next;
};

}  // namespace sym

#endif // SYMBOL_H_
