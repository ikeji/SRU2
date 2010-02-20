
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

// TODO: remove this dependency
#include "testing_ast.h"
#include "testing_sru.h"

using namespace std;
using namespace sru;
using namespace sru_test;

namespace sru_parser {

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
  parser->Set(fNAME, SRUString::New("sru_parser"));
  parser->Set("parse", CREATE_SRU_PROC(Parse));
  parser->Set("trueResult", CREATE_SRU_PROC(TrueResult));

    EOL
    parser.symbols.each do|sym|
      ret += <<-EOL
  parser->Set("#{sym}", CREATE_SRU_PROC(#{sym}));
      EOL
    end
    ret += <<-EOL

    EOL
    parser.terms.each do|term|
      ret += <<-EOL
  parser->Set("term#{term.num}", CREATE_SRU_PROC(term#{term.num}));
      EOL
    end
    ret += <<-EOL

    EOL
    parser.manipulators.each do|mani|
      ret += <<-EOL
  parser->Set("#{mani.name}", CREATE_SRU_PROC(#{mani.name}));
      EOL
    end
    ret += <<-EOL
}

DEFINE_SRU_PROC_SMASH(Parse){
  assert(args.size() > 1);
  // TODO: check args0
  Interpreter::Instance()->DigIntoNewFrame(
      A(R(C(R(R(fTHIS),"program"), R(fTHIS), R("src"), C(R(R("Numeric"),"parse"),R("Numeric"),S("0"))),"ast")),
      Binding::New(Interpreter::Instance()->RootStackFrame()->Binding()));
  // Push args to local
  BasicObjectPtr binding = Interpreter::Instance()->CurrentStackFrame()->Binding();
  binding->Set(fTHIS, args[0]);
  binding->Set("src", args[1]);
}

    EOL
    parser.syntaxes.keys.each do |sym|
      ret += <<-EOL
DEFINE_SRU_PROC_SMASH(#{sym}){
#{pri2(parser,sym)}
  assert(args.size()>2);
  // TODO: Check argument.
  static ptr_vector exps = 
      A(
      EOL
      parser.captures[sym].each do |cap|
        ret += <<-EOL
        L("#{cap}", R("nil")),
        EOL
      end
      ret += <<-EOL
#{spc(8,parser.syntaxes[sym].accept(self, 0))}
        ,
        R("result0")
      );
  Interpreter::Instance()->DigIntoNewFrame(exps,
      Binding::New(Interpreter::Instance()->RootStackFrame()->Binding()));
  // Push args to local
  BasicObjectPtr binding = Interpreter::Instance()->CurrentStackFrame()->Binding();
  binding->Set(fTHIS, args[0]);
  binding->Set("src", args[1]);
  binding->Set("pos0", args[2]);
  // Stack smash!
}
      EOL
    end
    parser.terms.each do |term|
      ret += <<-EOL
DEFINE_SRU_PROC(term#{term.num}){
  string target = "#{term.string}";
  assert(args.size()>2);
  // TODO: Check argument.
  string src = SRUString::GetValue(args[1]);
  int pos = SRUNumeric::GetValue(args[2]);
  // TODO: Define ParserResult type.
  BasicObjectPtr ret = BasicObject::New();
  if(src.compare(pos, target.size(), target) == 0){
    ret->Set("status", Library::Instance()->True());
    ret->Set("pos", SRUNumeric::New(pos + target.size()));
  } else {
    ret->Set("status", Library::Instance()->False());
  }
  return ret;
}
      EOL
    end
    ret += <<-EOL

DEFINE_SRU_PROC(TrueResult){ // this, pos
  LOG << args[1]->Inspect();
  BasicObjectPtr ret = BasicObject::New();
  ret->Set("status", Library::Instance()->True());
  ret->Set("pos", args[1]);
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
L("pos#{n+1}", R("pos#{n}")),
#{peg.left.accept(self, n+1)}
,
L("status#{n}", R(R("result#{n+1}"),"status")),
L("result#{n}",
  C(R(R("status#{n}"), "ifTrueFalse"),
    R("status#{n}"),
    P(R("result#{n+1}")),
    P(
#{spc(6,peg.right.accept(self, n+1))}
      ,
      R("result#{n+1}")
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
L("pos#{n+1}", R("pos#{n}")),
#{peg.left.accept(self, n+1)}
,
L("status#{n}", R(R("result#{n+1}"),"status")),
L("result#{n}",
  C(R(R("status#{n}"), "ifTrueFalse"),
    R("status#{n}"),
    P(
      L("pos#{n+1}", R(R("result#{n+1}"),"pos")),
#{spc(6,peg.right.accept(self, n+1))}
      ,
      R("result#{n+1}")
    ),
    P(R("result#{n+1}"))
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
L("result#{n}",
  C(B("break#{n}",
    L("pos#{n+1}", R("pos#{n}")),
    // TODO: We must return true when empty loop.
    L("last#{n}", C(R(R(fTHIS),"trueResult"),R(fTHIS),R("pos#{n}"))),
    L("block#{n}", P(
#{spc(6, peg.cont.accept(self, n+1))}
      ,
      L("status#{n}", R(R("result#{n+1}"),"status")),
      C(R(R("status#{n}"),"ifFalse"),
        R("status#{n}"),
        P(C(R("break#{n}"),R("last#{n}")))),
      L("pos#{n+1}", R(R("result#{n+1}"),"pos")),
      L("last#{n}", R("result#{n+1}"))
    )),
    C(R(R("block#{n}"),"loop"),R("block#{n}"))
  )))
// end: #{prii(peg)}
// index: #{n}
    EOL
  end
  def visit_Optional(peg, n)
    <<-EOL
// start: #{prii(peg)}
// index: #{n}
L("pos#{n+1}", R("pos#{n}")),
#{peg.cont.accept(self, n+1)}
,
L("status#{n}", R(R("result#{n+1}"),"status")),
L("result#{n}",
  C(R(R("status#{n}"), "ifTrueFalse"),
    R("status#{n}"),
    P(R("result#{n+1}")),
    P(C(R(R(fTHIS),"trueResult"),R(fTHIS),R("pos#{n}")))
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
L("#{peg.capture}",
  L("result#{n}",
    C(R(R(fTHIS),"#{peg.symbol}"),
      R(fTHIS), R("src"), R("pos#{n}")
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
L("result#{n}",
  C(R(R(fTHIS),"term#{peg.num}"),
    R(fTHIS), R("src"), R("pos#{n}")
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
L("#{peg.name}",
  L("result#{n}",
    C(R(R(fTHIS),"#{peg.name}"),
      R(fTHIS),
      R("src"),
      R("pos#{n}"),
    EOL
    peg.varg_list.each do |v|
      r += <<-EOL
      R("#{v}"),
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
