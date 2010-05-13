
class CppCodeBuilder
  def self.to_s(parser)
    self.new.print(parser)
  end
  def print(parser)
    # TODO: move this to template file.
    ret = <<-EOL
// Programing Language SRU
// Copyright(C) 2005-2010 IKeJI
// 

#include "parser.h"
#include "basic_object.h"
#include "native_proc.h"
#include "constants.h"
#include "binding.h"
#include "string.h"
#include "numeric.h"
#include "library.h"
#include "logging.h"
#include "parser_helper.h"
#include "stack_frame.h"
#include "interpreter.h"

// TODO: remove this dependency
#include "testing_ast.h"
#include "testing_sru.h"

using namespace std;
using namespace sru;
using namespace sru_test;

namespace sru_parser {

DECLARE_SRU_PROC(memoize);
    EOL
    parser.symbols.each do|sym|
      ret += <<-EOL
DECLARE_SRU_PROC(#{sym.to_s});#{pri(parser,sym)}
      EOL
    end
    ret += <<-EOL

    EOL
    ret += <<-EOL

    EOL
    parser.terms.each do|term|
      ret += <<-EOL
DECLARE_SRU_PROC(term#{term.num});// #{term.string}
      EOL
    end
    ret += <<-EOL

    EOL
    parser.manipulators.each do|mani|
      ret += <<-EOL
DECLARE_SRU_PROC(#{mani.name}); // this, src, pos, #{mani.varg_list.map{|m|m.to_s}.join ", "}
      EOL
    end

    ret += <<-EOL

DECLARE_SRU_PROC(Parse);
DECLARE_SRU_PROC(TrueResult);

void InitializeParserObject(BasicObjectPtr& parser){
  parser->Set(sym::__name(), SRUString::New(symbol("sru_parser")));
  parser->Set(sym::parse(), CREATE_SRU_PROC(Parse));
  parser->Set(sym::trueResult(), CREATE_SRU_PROC(TrueResult));

  parser->Set(sym::memoize(), CREATE_SRU_PROC(memoize));
    EOL
    parser.symbols.each do|sym|
      ret += <<-EOL
  parser->Set(sym::#{sym}(), CREATE_SRU_PROC(#{sym}));
      EOL
    end
    ret += <<-EOL

    EOL
    parser.terms.each do|term|
      ret += <<-EOL
  parser->Set(sym::term#{term.num}(), CREATE_SRU_PROC(term#{term.num}));
      EOL
    end
    ret += <<-EOL

    EOL
    parser.manipulators.each do|mani|
      ret += <<-EOL
  parser->Set(sym::#{mani.name}(), CREATE_SRU_PROC(#{mani.name}));
      EOL
    end
    ret += <<-EOL
}

DEFINE_SRU_PROC_SMASH(Parse){
  assert(args.size() > 1);
  // TODO: check args0
  Interpreter::Instance()->DigIntoNewFrame(
      A(
        C(R(R(sym::self()),sym::program()),
          R(sym::self()),
          R(sym::src()),
          C(R(R(sym::Numeric()),sym::parse()),R(sym::Numeric()),S(symbol("0"))))
      ),
      Binding::New(Interpreter::Instance()->RootStackFrame()->Binding()));
  // Push args to local
  BasicObjectPtr binding = Interpreter::Instance()->CurrentStackFrame()->Binding();
  binding->Set(sym::self(), args[0]);
  binding->Set(sym::src(), args[1]);
}

    EOL
    parser.syntaxes.keys.each do |sym|
      ret += <<-EOL
DEFINE_SRU_PROC_SMASH(#{sym}){
#{pri2(parser,sym)}
  assert(args.size()>2);
  // TODO: Check argument.
  
  BasicObjectPtr r = memoize::GetFromMemoize(proc, args[1], args[2]);
  if(r.get() != NULL) {
    StackFrame* current_frame = Interpreter::Instance()->CurrentStackFrame();
    current_frame->PushResult(r);
  }

  static ptr_vector exps = 
      A(
      EOL
      parser.captures[sym].each do |cap|
        ret += <<-EOL
        L(sym::#{cap}(), R(sym::nil())),
        EOL
      end
      ret += <<-EOL
#{spc(8,parser.syntaxes[sym].accept(self, 0))}
        ,
        C(R(R(sym::self()),sym::memoize()),
          R(sym::self()),
          R(R(sym::self()),sym::#{sym}()),
          R(sym::src()),
          R(sym::pos0()),
          R(sym::result0()))
      );
  Interpreter::Instance()->DigIntoNewFrame(exps,
      Binding::New(Interpreter::Instance()->RootStackFrame()->Binding()));
  // Push args to local
  BasicObjectPtr binding = Interpreter::Instance()->CurrentStackFrame()->Binding();
  binding->Set(sym::self(), args[0]);
  binding->Set(sym::src(), args[1]);
  binding->Set(sym::pos0(), args[2]);
  // Stack smash!
}
      EOL
    end
    parser.terms.each do |term|
      ret += <<-EOL
DEFINE_SRU_PROC(term#{term.num}){
  const static string target = "#{term.string}";
  assert(args.size()>2);
  // TODO: Check argument.
  const string& src = SRUString::GetValue(args[1]).to_str();
  int pos = SRUNumeric::GetValue(args[2]);
  // TODO: Define ParserResult type.
  BasicObjectPtr ret = BasicObject::New();
  if(src.compare(pos, target.size(), target) == 0){
    ret->Set(sym::status(), Library::Instance()->True());
    ret->Set(sym::pos(), SRUNumeric::New(pos + target.size()));
  } else {
    ret->Set(sym::status(), Library::Instance()->False());
    ret->Set(sym::pos(), args[2]);
    ret->Set(sym::error(), SRUString::New(symbol("#{term.string} not found.")));
  }
  return ret;
}
      EOL
    end
    ret += <<-EOL

DEFINE_SRU_PROC(TrueResult){ // this, pos
  LOG << args[1]->Inspect();
  BasicObjectPtr ret = BasicObject::New();
  ret->Set(sym::status(), Library::Instance()->True());
  ret->Set(sym::pos(), args[1]);
  return ret;
}

} // namespace sru_parser
    EOL
    return ret
  end
  def spc(n,s)
    s.split("\n").map{|l| " "*n + l}.join("\n")
  end
  def pri(parser, sym)
    return "  // #{parser.syntaxes[sym].accept(Printer.new)}" if(parser.syntaxes[sym])
    ""
  end
  def pri2(parser, sym)
    return "" if(! parser.syntaxes[sym])
    r = parser.syntaxes[sym].accept(Printer.new)
    return <<-EOL
  // #{r}
  LOG << "Enter parser: #{sym} <= #{r.gsub(/"/,'\"')}";
    EOL
    ""
  end
  def prii(p)
    p.accept(Printer.new)
  end
# Each visitor output C++ code for parser.
# each step output parse result as resultN.
# And use other work valiables like hogehogeN ex status2.
  def visit_Or(peg, n)
    <<-EOL
// start: #{prii(peg)}
// index: #{n}
L(sym::pos#{n+1}(), R(sym::pos#{n}())),
#{peg.left.accept(self, n+1)}
,
L(sym::status#{n}(), R(R(sym::result#{n+1}()),sym::status())),
L(sym::result#{n}(),
  C(R(R(sym::status#{n}()), sym::ifTrueFalse()),
    R(sym::status#{n}()),
    P(R(sym::result#{n+1}())),
    P(
      L(sym::error#{n}(), R(sym::result#{n+1}())),
#{spc(6,peg.right.accept(self, n+1))}
      ,
      L(sym::status#{n}(), R(R(sym::result#{n+1}()),sym::status())),
      C(R(R(sym::status#{n}()), sym::ifTrueFalse()),
        R(sym::status#{n}()),
        P(R(sym::result#{n+1}())),
        // If each side was failed, I return longest one.
        P(
          L(sym::left#{n}(), R(R(sym::error#{n}()), sym::pos())),
          L(sym::right#{n}(), R(R(sym::result#{n+1}()), sym::pos())),
          L(sym::status#{n}(),
            C(R(R(sym::left#{n}()),sym::greaterThan()),
              R(sym::left#{n}()), R(sym::right#{n}()))),
          C(R(R(sym::status#{n}()),sym::ifTrueFalse()),
            R(sym::status#{n}()),
            P(R(sym::error#{n}())),
            P(R(sym::result#{n+1}()))
          )
        )
      )
    )
  )
)
// end: #{prii(peg)}
// index: #{n}
    EOL
  end
  def visit_And(peg, n)
    <<-EOL
// start: #{prii(peg)}
// index: #{n}
L(sym::pos#{n+1}(), R(sym::pos#{n}())),
#{peg.left.accept(self, n+1)}
,
L(sym::status#{n}(), R(R(sym::result#{n+1}()),sym::status())),
L(sym::result#{n}(),
  C(R(R(sym::status#{n}()), sym::ifTrueFalse()),
    R(sym::status#{n}()),
    P(
      L(sym::pos#{n+1}(), R(R(sym::result#{n+1}()),sym::pos())),
#{spc(6,peg.right.accept(self, n+1))}
      ,
      R(sym::result#{n+1}())
    ),
    P(R(sym::result#{n+1}()))
  )
)
// end: #{prii(peg)}
// index: #{n}
    EOL
  end
  def visit_Repeater(peg, n)
    <<-EOL
// start: #{prii(peg)}
// index: #{n}
L(sym::result#{n}(),
  C(B(sym::break#{n}(),
    L(sym::pos#{n+1}(), R(sym::pos#{n}())),
    L(sym::last#{n}(),
      C(R(R(sym::self()),sym::trueResult()),
        R(sym::self()),R(sym::pos#{n}()))),
    L(sym::block#{n}(), P(
#{spc(6, peg.cont.accept(self, n+1))}
      ,
      L(sym::status#{n}(), R(R(sym::result#{n+1}()),sym::status())),
      C(R(R(sym::status#{n}()),sym::ifFalse()),
        R(sym::status#{n}()),
        P(C(R(sym::break#{n}()),R(sym::last#{n}())))),
      L(sym::pos#{n+1}(), R(R(sym::result#{n+1}()),sym::pos())),
      L(sym::last#{n}(), R(sym::result#{n+1}()))
    )),
    C(R(R(sym::block#{n}()),sym::loop()),R(sym::block#{n}()))
  )))
// end: #{prii(peg)}
// index: #{n}
    EOL
  end
  def visit_Optional(peg, n)
    <<-EOL
// start: #{prii(peg)}
// index: #{n}
L(sym::pos#{n+1}(), R(sym::pos#{n}())),
#{peg.cont.accept(self, n+1)}
,
L(sym::status#{n}(), R(R(sym::result#{n+1}()),sym::status())),
L(sym::result#{n}(),
  C(R(R(sym::status#{n}()), sym::ifTrueFalse()),
    R(sym::status#{n}()),
    P(R(sym::result#{n+1}())),
    P(C(R(R(sym::self()),sym::trueResult()),R(sym::self()),R(sym::pos#{n}())))
  )
)
// end: #{prii(peg)}
// index: #{n}
    EOL
  end
  def visit_NonTerminalSymbol(peg, n)
    r = <<-EOL
// start: #{prii(peg)}
// index: #{n}
L(sym::#{peg.capture}(),
  L(sym::result#{n}(),
    C(R(R(sym::self()),sym::#{peg.symbol}()),
      R(sym::self()), R(sym::src()), R(sym::pos#{n}())
    )
  )
)
// end: #{prii(peg)}
// index: #{n}
    EOL
  end
  def visit_TerminalSymbol(peg,n)
    <<-EOL
// start: #{prii(peg)}
// index: #{n}
L(sym::result#{n}(),
  C(R(R(sym::self()),sym::term#{peg.num}()),
    R(sym::self()), R(sym::src()), R(sym::pos#{n}())
  )
)
// end: #{prii(peg)}
// index: #{n}
    EOL
  end
  def visit_Manipulator(peg,n)
    r = <<-EOL
// start: #{prii(peg)}
// index: #{n}
L(sym::#{peg.name}(),
  L(sym::result#{n}(),
    C(R(R(sym::self()),sym::#{peg.name}()),
      R(sym::self()),
      R(sym::src()),
      R(sym::pos#{n}()),
    EOL
    peg.varg_list.each do |v|
      r += <<-EOL
      R(sym::#{v}()),
      EOL
    end
    r = r[0..-3]
    r += <<-EOL

    )
  )
)
// end: #{prii(peg)}
// index: #{n}
    EOL
    return r;
  end
end
