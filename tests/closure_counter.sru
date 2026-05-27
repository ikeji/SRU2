# Closures capture outer bindings. Inner `n = n + 1` assigns to the outer
# slot because the parent chain already has `n`.
def make_counter()
  n = 0
  { n = n + 1; n }
end

c1 = make_counter()
c2 = make_counter()
puts c1().toS()
puts c1().toS()
puts c1().toS()
puts c2().toS()
puts c2().toS()
puts c1().toS()
