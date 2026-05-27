# Array basics — what csru actually implements: Array.new(), push, at, size,
# array literal `[a, b, c]`. Many other Array methods are stubs (pop, shift,
# join, each, ...) and are intentionally not exercised here.

# Empty array from Array.new() — note the () is required.
xs = Array.new()
xs.push(10)
xs.push(20)
xs.push(30)

puts xs.size().toS()
puts xs.at(0).toS()
puts xs.at(2).toS()

# Array literal.
ys = [1, 2, 3, 4, 5]
puts ys.size().toS()

# Hand-rolled iteration; Array#each is a stub.
i = 0
sum = 0
while (i < ys.size())
  sum = sum + ys.at(i)
  i = i + 1
end
puts sum.toS()

# Nested arrays via explicit push (array literal flattens otherwise).
inner = [3, 4]
zs = Array.new()
zs.push(1)
zs.push("two")
zs.push(inner)
puts zs.at(1)
puts zs.at(2).at(1).toS()
