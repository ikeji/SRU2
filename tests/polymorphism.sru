# Polymorphism: subclasses override methods, parent's method dispatches via
# self to the subclass impl.
class Shape
  def area()
    0
  end
  def describe()
    puts self.name() + " has area " + self.area().toS()
  end
end

class Circle < Shape
  def initialize(r)
    self.r = r
  end
  def name()
    "circle"
  end
  def area()
    # Integer-only approximation; csru's Numeric.parse doesn't carry pi.
    self.r * self.r * 3
  end
end

class Square < Shape
  def initialize(s)
    self.s = s
  end
  def name()
    "square"
  end
  def area()
    self.s * self.s
  end
end

shapes = [Circle.new(5), Square.new(4)]
i = 0
while (i < shapes.size())
  shapes.at(i).describe()
  i = i + 1
end
