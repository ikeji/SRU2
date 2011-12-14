// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
//

#ifndef AST_CLASS_H_ 
#define AST_CLASS_H_

namespace sru {

class BasicObjectPtr;
class Library;

class ASTClass {
  public:
    ASTClass(const Library& lib);
    ~ASTClass();
    BasicObjectPtr GetASTNamespace();

    BasicObjectPtr GetLetExpressionClass();
    BasicObjectPtr GetRefExpressionClass();
    BasicObjectPtr GetCallExpressionClass();
    BasicObjectPtr GetProcExpressionClass();
    BasicObjectPtr GetStringExpressionClass();
  private:
    struct Impl;
    Impl* pimpl;
    ASTClass(const ASTClass& obj);
    ASTClass &operator=(const ASTClass& obj);
};

} // namespace sru

#endif  // AST_CLASS_H_

