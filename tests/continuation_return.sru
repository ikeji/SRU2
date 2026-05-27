# `return` is automatically bound inside `def` bodies as the continuation
# that escapes the function. Using it like an early return is the simplest
# form of continuation use.

def first_even(arr)
  i = 0
  while (i < arr.size())
    v = arr.at(i)
    if (v % 2) == 0
      return(v)
    end
    i = i + 1
  end
  return(nil)
end

p first_even([1, 3, 5, 4, 7])
p first_even([1, 3, 5, 7])

# Bare `return` (no parens, no arg) does NOT escape — it's the continuation
# object itself; calling it returns nil. So `return(nil)` and `return()` are
# the explicit "escape with nil" forms.
def doneOrNot(flag)
  if flag
    return("done")
  end
  "fallthrough"
end

puts doneOrNot(true)
puts doneOrNot(false)
