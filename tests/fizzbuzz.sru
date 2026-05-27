# FizzBuzz 1..20
i = 1
while (i <= 20)
  if (i % 15) == 0
    puts "FizzBuzz"
  elsif (i % 3) == 0
    puts "Fizz"
  elsif (i % 5) == 0
    puts "Buzz"
  else
    puts i.toS()
  end
  i = i + 1
end
