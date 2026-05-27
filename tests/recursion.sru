# Recursion via def. `fact` calls itself by name.
def fact(n)
  if n <= 1
    1
  else
    n * fact(n - 1)
  end
end

puts fact(0).toS()
puts fact(1).toS()
puts fact(5).toS()
puts fact(10).toS()

# Mutual recursion: isEven/isOdd.
def isEven(n)
  if n == 0
    true
  else
    isOdd(n - 1)
  end
end
def isOdd(n)
  if n == 0
    false
  else
    isEven(n - 1)
  end
end

if isEven(10)
  puts "10 even"
end
if isOdd(7)
  puts "7 odd"
end
