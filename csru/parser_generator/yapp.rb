class ParserBuilder
  attr_accessor :syntaxes, :symbols, :terms
  def initialize
    @symbols = []
    @terms = []
    @syntaxes = {}
  end

  def self.build(&proc)
    p = self.new
    p.instance_eval &proc
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
    @symbols.each{|s| @syntaxes[s].accept(r)}
  end

  def symbol(*symbols)
    parser  = self
    symbols.each do|sym|
      self.class.module_eval do
        define_method(sym){
          s = NonTerminalSymbol.new(sym)
          s.parser = parser
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
  attr_accessor :parser, :symbol
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

// TODO: remove this dependency
#include "testing_ast.h"
#include "testing_sru.h"

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

void sru_parser::InitializeParserObject(BasicObjectPtr& parser){
  parser->Set(fNAME, SRUString::New("sru_parser"));

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


    EOL
    parser.syntaxes.keys.each do |sym|
      ret += <<-EOL
DEFINE_SRU_PROC_SMASH(#{sym}){
#{pri(parser,sym)}
  assert(arg.size()>2);
  // TODO: Check argument.
  Interpreter::Instance()->DigIntoNewFrame(
      A(
#{spc(8,parser.syntaxes[sym].accept(self, 0))}
        ,
        R("result0")
      ),
      Binding::New(Interpreter::Instance()->RootStackFrame()->Binding()));
  // Push args to local
  BasicObjectPtr binding = Interpreter::Instance()->CurrentStackFrame()->Binding();
  binding->Set(fTHIS, arg[0]);
  binding->Set("src", arg[1]);
  binding->Set("pos0", arg[2]);
  // Stack smash!
}
      EOL
    end
    parser.terms.each do |term|
      ret += <<-EOL
DEFINE_SRU_PROC(term#{term.num}){
  string target = "#{term.string}";
  assert(arg.size()>2);
  // TODO: Check argument.
  string src = SRUString::GetValue(arg[1]);
  int pos = SRUNumeric::GetValue(arg[2]);
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
#{spc(6,peg.left.accept(self, n+1))}
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
#{spc(6,peg.left.accept(self, n+1))}
    ),
    P(R("result#{n+1}"))
  )
)
// end: #{prii(peg)}
// index: #{n}
    EOL
  end
  def visit_NonTerminalSymbol(peg, n)
    <<-EOL
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
    L("block#{n}", P(
      L("pos#{n+1}", R("pos#{n}")),
#{spc(6, peg.cont.accept(self, n+1))}
      ,
      L("status#{n}", R(R("result#{n+1}"),"status")),
      C(R(R("status#{n}"),"ifFalse"),
        R("status#{n}"),
        P(C(R("break#{n}"),R("last#{n}")))),
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
