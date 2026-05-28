# Hash class — csru ships only a placeholder, rsru implements the full
# string-keyed map with set/get/has/delete/keys/values plus an SRU-side
# each / eachKey / eachValue from the prelude.

h = Hash.new()
h.set("name", "Alice")
h.set("age", 30)
h.set("city", "Tokyo")

puts h.get("name")
puts h.get("age").toS()
puts h.size().toS()

# Boolean queries
if h.hasKey("city")
  puts "has city"
end
if h.hasKey("country")
  puts "would be wrong"
else
  puts "no country"
end

# Integer keys are first-class too.
h2 = Hash.new()
h2.set(1, "one")
h2.set(42, "answer")
puts h2.get(42)
puts h2.get(99)

# Delete returns the removed value.
puts h.delete("age").toS()
puts h.size().toS()

# Iterate sum via SRU-side prelude each.
nums = Hash.new()
nums.set("a", 1)
nums.set("b", 2)
nums.set("c", 3)
sum = 0
nums.eachValue({ |v| sum = sum + v })
puts sum.toS()
