
class ParserBuilder
  attr_accessor :syntaxes, :symbols, :terms, :captures, :manipulators
  def initialize
    @symbols = []
    @terms = []
    @syntaxes = {}
    @captures = Hash.new{|h,k| h[k] = []}
    @manipulators = []
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
      peg.cont.accept(self)
    end
    def visit_Optional(peg)
      peg.cont.accept(self)
    end
    def visit_Manipulator(peg)
    end
  end

  def regester_terminal_symbols
    r = RegesterTerminalSymbolsInternal.new(self)
    @symbols.each{|s| @syntaxes.has_key?(s) && @syntaxes[s].accept(r)}
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
      peg.cont.accept(self, sym)
    end
    def visit_Optional(peg, sym)
      peg.cont.accept(self, sym)
    end
    def visit_Manipulator(peg, sym)
      @parser.captures[sym] << peg.capture if peg.capture
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
        define_method(sym) do |*cap|
          raise Exception.new "syntax error" if(cap.size > 1)
          s = NonTerminalSymbol.new(sym)
          s.parser = parser
          if cap.size() == 1
            raise Exception.new "syntax error" if !cap[0].instance_of?(Symbol)
            s.capture = cap[0]
          else
            s.capture = sym
          end
          return s
        end
      end
    end
    @symbols.push *symbols
  end

  def manipulator(*manipulators)
    parser = self
    manipulators.each do|mani|
      self.class.module_eval do
        define_method(mani) do |*arg|
          found = nil
          @manipulators.each do|m|
            next if(m.name != mani)
            raise Exception.new("duplicated manipulator name") if m.varg_list.size != arg.size
            found = m
            break
          end
          m = Manipulator.new(mani, arg)
          return m if found
          @manipulators << m
          return m
        end
      end
    end
  end

  def r(arg)
    return Repeater.new(arg)
  end
  def o(arg)
    return Optional.new(arg)
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
  def ~@
    Optional.new(self)
  end
end

class PEG
  def self.convert(arg)
    return arg if arg.is_a?(PEG)
    return TerminalSymbol.new(arg) if arg.is_a?(String)
    return RegexpSymbol.new(arg) if arg.is_a?(Regexp)
    raise Exception.new("Don't support " + arg.class.to_s)
  end
  def accept(visitor, *args)
    args.unshift self
    visitor.send("visit_#{self.class}",*args)
  end
  def *(right)
    And.new(self,PEG.convert(right))
  end
  def |(right)
    Or.new(self,PEG.convert(right))
  end
  def ~@
    Optional.new(self)
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
  attr_accessor :cont
  def initialize(cont)
    @cont = PEG.convert cont
  end
end

class Repeater < PEG
  attr_accessor :cont
  def initialize(cont)
    @cont = PEG.convert cont
  end
end

class Optional < PEG
  attr_accessor :cont
  def initialize(cont)
    @cont = PEG.convert cont
  end
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
    @parser.syntaxes[@symbol] = PEG.convert(right)
  end
  def initialize(symbol)
    @symbol = symbol
  end
end

class RegexpSymbol < PEG
  attr_accessor :regexp
  def initialize(regexp)
    @regexp = regexp
  end
end

class Manipulator < PEG
  attr_accessor :name, :varg_list, :capture
  def initialize(name, varg_list)
    @name = name
    @capture = name
    @varg_list = varg_list
  end
end
