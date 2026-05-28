# Standard-library methods that rsru ships beyond what csru implements.
# Mostly SRU-side prelude entries plus the native String#split/indexOf and
# Boolean#toS additions.

# Array — prelude
a = [1, 2, 3, 4, 5]
puts(a.first().toS())
puts(a.last().toS())
puts(a.reverse().join(","))
puts(a.indexOf(3).toS())
puts(a.contains(99).toS())
puts(a.contains(2).toS())
puts(a.find({ |x| (x % 2) == 0 }).toS())

# String — native + prelude
s = "hello world"
puts(s.indexOf("world").toS())
puts(s.indexOf("xyz").toS())
puts(s.contains("hello").toS())
puts(s.startsWith("hello").toS())
puts(s.endsWith("world").toS())
puts(s.endsWith("hello").toS())
parts = "a,b,c,d".split(",")
puts(parts.size().toS())
puts(parts.at(2))

# Numeric — prelude
puts((3).min(5).toS())
puts((3).max(5).toS())

# Boolean — native
puts(true.toS())
puts(false.toS())
puts((1 == 1).toS())

# Array mutation + sort
m = [3, 1, 4, 1, 5, 9, 2, 6]
puts(m.sort().join(","))
puts(m.pop().toS())
puts(m.shift().toS())
puts(m.size().toS())

m2 = [1, 2]
m2.concat([3, 4])
puts(m2.join(","))

# Hash merge
ha = Hash.new()
ha.set("a", 1)
hb = Hash.new()
hb.set("b", 2)
ha.merge(hb)
puts(ha.size().toS())
