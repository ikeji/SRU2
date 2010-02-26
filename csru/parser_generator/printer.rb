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
  def visit_Optional(peg)
    "[" + peg.cont.accept(self) + "]"
  end
  def visit_Manipulator(peg)
    " { " + peg.name.to_s + " } "
  end
end

