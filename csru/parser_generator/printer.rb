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
  def visit_And(peg)
    l = peg.left.accept(self)
    l = "( " + l + " )" if peg.left.is_a? Or
    r = peg.right.accept(self)
    r = "( " + r + " )" if peg.right.is_a? Or
    l + " " + r
  end
  def visit_Or(peg)
    peg.left.accept(self) + " | " + peg.right.accept(self)
  end
  def visit_Not(peg)
    "!(" + peg.cont.accept(self) + ")"
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
  def visit_Optional(peg)
    "[" + peg.cont.accept(self) + "]"
  end
  def visit_Manipulator(peg)
    " {{ " + peg.name.to_s + " }} "
  end
end

