requireNative "lib/ffi.so"

def initscr()
  ffi_call("libncursesw.so.5", "initscr", "pointer", [], [])
end

def cbreak()
  ffi_call("libncursesw.so.5", "cbreak", "sint", [], [])
end

def noecho()
  ffi_call("libncursesw.so.5", "noecho", "sint", [], [])
end

def endwin()
  ffi_call("libncursesw.so.5", "endwin", "sint", [], [])
end

def move(x, y)
  ffi_call("libncursesw.so.5", "move", "sint", ["sint", "sint"], [y, x])
end

def addstr(str)
  ffi_call("libncursesw.so.5", "addstr", "sint", ["pointer"], [str])
end

def refresh()
  ffi_call("libncursesw.so.5", "refresh", "sint", [], [])
end

def sleep(t)
  ffi_call("libc.so.6", "sleep", "uint", ["uint"], [t])
end

initscr(); cbreak(); noecho();

move(1,1)
addstr("hello")
refresh()
sleep(1)
move(1,1)
addstr("world")
refresh()
sleep(1)

endwin();
