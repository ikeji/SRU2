// Programing Language SRU
// Copyright(C) 2005-2010 IKeJI
// 

namespace sru {
class BasicObjectPtr;
}

namespace sru_parser {
namespace memoize {

sru::BasicObjectPtr GetFromMemoize(
  const sru::BasicObjectPtr& func,
  const sru::BasicObjectPtr& src,
  const sru::BasicObjectPtr& pos);

sru::BasicObjectPtr SetToMemoize(
  const sru::BasicObjectPtr& func,
  const sru::BasicObjectPtr& src,
  const sru::BasicObjectPtr& pos,
  const sru::BasicObjectPtr& result);

}  // namespace memoize
}  // namespace sru_parser
