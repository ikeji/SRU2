# Basic arithmetic. Pass the full expression to puts in one parenthesised
# group — `puts (e).toS()` (with the space) would call `puts(e).toS()`.
puts((1 + 2).toS())
puts((10 - 4).toS())
puts((3 * 7).toS())
puts((20 / 4).toS())
puts((10 % 3).toS())

# Precedence
puts((1 + 2 * 3).toS())
puts(((1 + 2) * 3).toS())

# Comparison returns Boolean and dispatches if
if 5 > 3
  puts "5>3 ok"
end
if 5 < 3
  puts "should not"
else
  puts "5<3 false"
end

# String concat
puts "foo" + "bar"

# Integer vs real division
puts((1 / 2).toS())
puts((1.0 / 2.0).toS())
