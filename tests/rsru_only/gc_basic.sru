# Sys.gc forces a mark-and-sweep and returns the live count afterwards.
# Sys.liveCount returns the count without forcing GC.

# Allocate lots of garbage in a loop. GC kicks in periodically; without
# it this would push the working set up indefinitely.
i = 0
while (i < 5000)
  x = Array.new()
  x.push(i)
  i = i + 1
end

# After a forced GC, the only objects left should be the bootstrap heap
# (well under 1000) — none of the per-iteration `x` arrays survive.
n = Sys.gc()
if n < 1000
  puts "ok"
else
  puts "leak: " + n.toS()
end
