// Programing Language SRU
// Copyright(C) 2005-2010 IKeJI
// 

#include <string>

namespace sru {
class BasicObjectPtr;
} // namespace sru

namespace sru_parser {

sru::BasicObjectPtr CreateFalse(
    sru::BasicObjectPtr pos,
    sru::BasicObjectPtr error_message);
sru::BasicObjectPtr CreateFalse(
    sru::BasicObjectPtr pos,
    const char* error_message);
sru::BasicObjectPtr CreateFalse(
    int pos,
    const char* error_message);

sru::BasicObjectPtr CreateTrue(
    sru::BasicObjectPtr pos,
    sru::BasicObjectPtr ast);
sru::BasicObjectPtr CreateTrue(
    int pos,
    sru::BasicObjectPtr ast);

}  // namespace sru_parser
