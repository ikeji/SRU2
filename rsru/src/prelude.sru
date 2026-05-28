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

Array.instanceMethods.first = { |self|
  if self.size() == 0
    nil
  else
    self.at(0)
  end
}

Array.instanceMethods.last = { |self|
  n = self.size()
  if n == 0
    nil
  else
    self.at(n - 1)
  end
}

Array.instanceMethods.reverse = { |self|
  out = Array.new()
  i = self.size() - 1
  while (i >= 0)
    out.push(self.at(i))
    i = i - 1
  end
  out
}

# Returns the index of the first element equal to value, or -1.
Array.instanceMethods.indexOf = { |self, value|
  i = 0
  n = self.size()
  found = -1
  while (i < n)
    if self.at(i) == value
      found = i
      i = n
    end
    i = i + 1
  end
  found
}

Array.instanceMethods.contains = { |self, value|
  self.indexOf(value) >= 0
}

# Returns the first element where block(x) is true, or nil.
Array.instanceMethods.find = { |self, block|
  i = 0
  n = self.size()
  result = nil
  while (i < n)
    v = self.at(i)
    if block(v)
      result = v
      i = n
    end
    i = i + 1
  end
  result
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

Numeric.instanceMethods.min = { |self, other|
  if self < other
    self
  else
    other
  end
}

Numeric.instanceMethods.max = { |self, other|
  if self > other
    self
  else
    other
  end
}

# ---- String ----

String.instanceMethods.contains = { |self, needle|
  self.indexOf(needle) >= 0
}

String.instanceMethods.startsWith = { |self, prefix|
  pn = prefix.size()
  if self.size() < pn
    false
  else
    self.substr(0, pn) == prefix
  end
}

String.instanceMethods.endsWith = { |self, suffix|
  sn = suffix.size()
  n = self.size()
  if n < sn
    false
  else
    self.substr(n - sn, sn) == suffix
  end
}

# ---- Hash ----

Hash.instanceMethods.each = { |self, block|
  ks = self.keys()
  i = 0
  while (i < ks.size())
    k = ks.at(i)
    block(k, self.get(k))
    i = i + 1
  end
  self
}

Hash.instanceMethods.eachKey = { |self, block|
  ks = self.keys()
  i = 0
  while (i < ks.size())
    block(ks.at(i))
    i = i + 1
  end
  self
}

Hash.instanceMethods.eachValue = { |self, block|
  vs = self.values()
  i = 0
  while (i < vs.size())
    block(vs.at(i))
    i = i + 1
  end
  self
}
