# Church-encoded booleans via closures. Exercises higher-order procs and the
# call-by-name discipline of `{ |x, y| ... }`.
T = { |x, y| x }
F = { |x, y| y }
AND = { |p, q| p(q, p) }
OR = { |p, q| p(p, q) }
NOT = { |p| p(F, T) }

def label(x)
  x("T", "F")
end

puts label(T)
puts label(F)
puts label(AND(T, T))
puts label(AND(T, F))
puts label(OR(T, F))
puts label(OR(F, F))
puts label(NOT(T))
puts label(NOT(F))
