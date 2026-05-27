# SRU-side standard library, loaded once at VM boot.
# These are convenience methods that don't need to touch native data and so
# are simpler to express here than in Rust.

# ---- Array ----

Array.instanceMethods.each = { |self, block|
  i = 0
  while (i < self.size())
    block(self.at(i))
    i = i + 1
  end
  self
}

Array.instanceMethods.map = { |self, block|
  out = Array.new()
  i = 0
  while (i < self.size())
    out.push(block(self.at(i)))
    i = i + 1
  end
  out
}

Array.instanceMethods.select = { |self, block|
  out = Array.new()
  i = 0
  while (i < self.size())
    v = self.at(i)
    if block(v)
      out.push(v)
    end
    i = i + 1
  end
  out
}

Array.instanceMethods.inject = { |self, acc, block|
  i = 0
  while (i < self.size())
    acc = block(acc, self.at(i))
    i = i + 1
  end
  acc
}

Array.instanceMethods.join = { |self, sep|
  out = ""
  i = 0
  n = self.size()
  while (i < n)
    if i > 0
      out = out + sep
    end
    v = self.at(i)
    out = out + v.toS()
    i = i + 1
  end
  out
}

# ---- Numeric ----

Numeric.instanceMethods.abs = { |self|
  if self < 0
    self.invert()
  else
    self
  end
}

Numeric.instanceMethods.negate = { |self|
  self.invert()
}

Numeric.instanceMethods.times = { |self, block|
  i = 0
  while (i < self)
    block(i)
    i = i + 1
  end
  self
}
