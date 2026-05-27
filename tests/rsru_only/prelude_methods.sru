# Exercise the SRU-side prelude: Array#each/map/select/inject/join and
# Numeric#abs/negate/times. These are written in SRU (rsru/src/prelude.sru)
# so this test is rsru-only — csru doesn't ship the same prelude.

a = [1, 2, 3, 4, 5]

a.each({ |x| puts x.toS() })

squared = a.map({ |x| x * x })
puts squared.join(", ")

evens = a.select({ |x| (x % 2) == 0 })
puts evens.join(", ")

sum = a.inject(0, { |acc, x| acc + x })
puts sum.toS()

n = -7
puts n.abs().toS()
puts n.negate().toS()

total = 0
(4).times({ |i| total = total + i })
puts total.toS()
