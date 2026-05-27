# `if expr ... end` desugars to `expr.ifTrue(thenProc)`, and with else
# to `expr.ifTrueFalse(thenProc, elseProc)`. So any object that defines
# those methods can sit in the condition slot like a Boolean.
#
# Note: identifier names cannot start with "then" or "else" because the
# parser treats them as the reserved keywords. We use t/e instead.
class Maybe
  def initialize(present, value)
    self.present = present
    self.value = value
  end
  def ifTrue(t)
    if self.present
      t(self.value)
    end
  end
  def ifTrueFalse(t, e)
    if self.present
      t(self.value)
    else
      e()
    end
  end
end

just7    = Maybe.new(true,  7)
nothing  = Maybe.new(false, nil)

# Custom class instance as the if condition.
if just7
  puts "just7 is present"
end

if nothing
  puts "nothing is present"
else
  puts "nothing is absent"
end

# Confirm ifTrue/ifTrueFalse get the proc args.
just7.ifTrueFalse(
  { |v| puts "got " + v.toS() },
  { puts "no value" }
)
nothing.ifTrueFalse(
  { |v| puts "got " + v.toS() },
  { puts "no value" }
)
