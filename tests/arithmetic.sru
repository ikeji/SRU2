# Basic arithmetic. Use intermediate variables because `puts x.toS()` parses
# as `puts(x).toS()` when x is parenthesized.
a = (1 + 2).toS();           puts a
b = (10 - 4).toS();          puts b
c = (3 * 7).toS();           puts c
d = (20 / 4).toS();          puts d
e = (10 % 3).toS();          puts e

# Precedence
f = (1 + 2 * 3).toS();       puts f
g = ((1 + 2) * 3).toS();     puts g

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
h = (1 / 2).toS();           puts h
i = (1.0 / 2.0).toS();       puts i
