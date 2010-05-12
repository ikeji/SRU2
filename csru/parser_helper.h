// Programing Language SRU
// Copyright(C) 2005-2010 IKeJI
// 

namespace sru {
class BasicObjectPtr;
}

namespace sru_parser {
namespace memoize {

sru::BasicObjectPtr GetFromMemoize(
  sru::BasicObjectPtr func,
  sru::BasicObjectPtr src,
  sru::BasicObjectPtr pos);

sru::BasicObjectPtr SetToMemoize(
  sru::BasicObjectPtr func,
  sru::BasicObjectPtr src,
  sru::BasicObjectPtr pos,
  sru::BasicObjectPtr result);

}  // namespace memoize
}  // namespace sru_parser
