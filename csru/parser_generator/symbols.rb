class SymbolPrinter
  def self.to_s(parser)
    p = self.new
    ret = ""
    parser.symbols.each do|sym|
      ret += sym.to_s + "\n"
    end
    parser.manipulators.each do|mani|
      ret += mani.name.to_s + "\n"
    end
    return ret
  end
end

