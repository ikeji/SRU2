# `return(return)` returns the continuation itself as the function's value.
# Calling that continuation later jumps the program back to just after
# the function call.
#
# Note: the canonical SRU continuation pattern only works reliably inside
# a closure block — at the top-level, the captured frame isn't the right
# one. So we wrap the body in `{ ... }()`.

{
  count = 0
  saved = nil

  def capture()
    return(return)
  end

  k = capture()
  count = count + 1
  if count == 1
    puts "first time, count=" + count.toS()
    saved = k
    k(nil)
  end
  if count == 2
    puts "second time via saved, count=" + count.toS()
  end
  puts "done"
}()
