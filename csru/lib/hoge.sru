p 0
p requireNative("./ffi.so")
p 1
p ffi_call
p 2
p ffi_call("libc.so.6", "getpid", "sint", [], [])
p 3
p ffi_call("libc.so.6", "getgid", "sint", [], [])
p 4

a = ffi_call("libc.so.6", "clock", "sint", [], [])
p 5
p ffi_call("libc.so.6", "puts", "sint", ["pointer"], ["hogehgoe"])
p 6
b = ffi_call("libc.so.6", "clock", "sint", [], [])
p 7
p ((b - a) / 1000000.0)
p 8

