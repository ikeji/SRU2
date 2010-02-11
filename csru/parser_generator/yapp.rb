class ParserBuilder
  attr_accessor :syntaxes, :symbols, :terms, :captures
  def initialize
    @symbols = []
    @terms = []
    @syntaxes = {}
    @captures = Hash.new{|h,k| h[k] = []}
  end

  def self.build(&proc)
    p = self.new
    p.instance_eval &proc
    p.regester_terminal_symbols
    p.regester_captures
    p
  end

  class RegesterTerminalSymbolsInternal
    @@term_num = 0
    def initialize(parser)
      @parser = parser
    end
    def visit_TerminalSymbol(peg)
      peg.num = @@term_num
      @@term_num += 1
      @parser.terms.push peg
    end
    def visit_Or(peg)
      peg.left.accept(self)
      peg.right.accept(self)
    end
    def visit_And(peg)
      peg.left.accept(self)
      peg.right.accept(self)
    end
    def visit_NonTerminalSymbol(peg)
    end
    def visit_RegexpSymbol(peg)
    end
    def visit_Repeater(peg)
    end
  end

  def regester_terminal_symbols
    r = RegesterTerminalSymbolsInternal.new(self)
    @symbols.each{|s| @syntaxes[s] && @syntaxes[s].accept(r)}
  end

  class RegesterCapturesInternal
    def initialize(parser)
      @parser = parser
    end
    def visit_NonTerminalSymbol(peg, sym)
      @parser.captures[sym] << peg.capture if peg.capture
    end
    def visit_Or(peg, sym)
      peg.left.accept(self, sym)
      peg.right.accept(self, sym)
    end
    def visit_And(peg, sym)
      peg.left.accept(self, sym)
      peg.right.accept(self, sym)
    end
    def visit_TerminalSymbol(peg, sym)
    end
    def visit_RegexpSymbol(peg, sym)
    end
    def visit_Repeater(peg, sym)
    end
  end
  
  def regester_captures
    r = RegesterCapturesInternal.new(self)
    @symbols.each do|s|
      @syntaxes[s].accept(r,s) if @syntaxes[s]
      @captures[s].uniq!
    end
  end

  def symbol(*symbols)
    parser  = self
    symbols.each do|sym|
      self.class.module_eval do
        define_method(sym){|*cap|
          s = NonTerminalSymbol.new(sym)
          s.parser = parser
          if cap.size() > 0
            s.capture = cap[0]
          else
            s.capture = sym
          end
          return s
        }
      end
    end
    @symbols.push *symbols
  end

  def r(arg)
    return Repeater.new(arg)
  end
end

class String
  alias oldand * 
  def *(arg)
    return And.new(TerminalSymbol.new(self),arg) if arg.is_a?(PEG)
    oldand(arg)
  end
  def |(arg)
    Or.new(TerminalSymbol.new(self),arg)
  end
end

class PEG
  def convert(arg)
    return arg if arg.is_a?(PEG)
    return TerminalSymbol.new(arg) if arg.is_a?(String)
    return RegexpSymbol.new(arg) if arg.is_a?(Regexp)
    raise Exception.new
  end
  def accept(visitor, *args)
    args.unshift self
    visitor.send("visit_#{self.class}",*args)
  end
  def *(right)
    And.new(self,convert(right))
  end
  def |(right)
    Or.new(self,convert(right))
  end
end

class And < PEG
  attr_accessor :left, :right
  def initialize(left,right)
    @left = left
    @right = right
  end
end

class Or < PEG
  attr_accessor :left, :right
  def initialize(left,right)
    @left = left
    @right = right
  end
end

class Not < PEG
end

class TerminalSymbol < PEG
  attr_accessor :string, :num
  def initialize(string)
    @string = string
  end
end

class NonTerminalSymbol < PEG
  attr_accessor :parser, :symbol, :capture
  def <=(right)
    raise Exception.new if @parser == nil
    @parser.syntaxes[@symbol] = convert(right)
  end
  def initialize(symbol)
    @symbol = symbol
  end
end

class Repeater < PEG
  attr_accessor :cont
  def initialize(cont)
    @cont = cont
  end
end

class RegexpSymbol < PEG
  attr_accessor :regexp
  def initialize(regexp)
    @regexp = regexp
  end
end

class Printer
  def self.to_s(parser)
    p = self.new
    ret = ""
    ret += "symbol: " + parser.symbols.inspect + "\n"
    parser.syntaxes.keys.each do |syn|
      ret += "#{syn} := #{parser.syntaxes[syn].accept(p)}\n"
    end
    return ret
  end
  def visit_Or(peg)
    peg.left.accept(self) + " | " + peg.right.accept(self)
  end
  def visit_And(peg)
    peg.left.accept(self) + " " + peg.right.accept(self)
  end
  def visit_NonTerminalSymbol(peg)
    peg.symbol.to_s
  end
  def visit_RegexpSymbol(peg)
    peg.regexp.inspect
  end
  def visit_TerminalSymbol(peg)
    "\"" + peg.string + "\""
  end
  def visit_Repeater(peg)
    "{" + peg.cont.accept(self) + "}"
  end
end

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
    parser.terms.each do|term|
      ret += <<-EOL
DECLARE_SRU_PROC(term#{term.num});// #{term.string}
      EOL
    end
    ret += <<-EOL

DECLARE_SRU_PROC(Parse);

void InitializeParserObject(BasicObjectPtr& parser){
  parser->Set(fNAME, SRUString::New("sru_parser"));
  parser->Set("parse", CREATE_SRU_PROC(Parse));

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
  Interpreter::Instance()->DigIntoNewFrame(
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
      ),
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
  def visit_NonTerminalSymbol(peg, n)
    r = <<-EOL
// start: #{prii(peg)}
// index: #{n}
L("result#{n}",
  C(R(R(fTHIS),"#{peg.symbol}"),
    R(fTHIS), R("src"), R("pos#{n}")
  )
)
// end: #{prii(peg)}
// index: #{n}
    EOL
    if peg.capture
      return "L(\"#{peg.capture}\",\n" + r + ")"
    else
      return r
    end 
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
  def visit_Repeater(peg, n)
    <<-EOL
// start: #{prii(peg)}
// index: #{n}
L("result#{n}",
  C(B("break#{n}",
    L("pos#{n+1}", R("pos#{n}")),
    L("last#{n}", R("nil")),
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
end

# vim:sw=2:ts=1000
