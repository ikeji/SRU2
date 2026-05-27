# Basic class declaration: fields via self.x, methods, instantiation.
class Point
  def initialize(x, y)
    self.x = x
    self.y = y
  end
  def show()
    puts "(" + self.x.toS() + ", " + self.y.toS() + ")"
  end
  def add(other)
    Point.new(self.x + other.x, self.y + other.y)
  end
end

p1 = Point.new(1, 2)
p2 = Point.new(10, 20)
p1.show()
p2.show()
p1.add(p2).show()

# External slot read.
s = p1.x.toS() + "/" + p1.y.toS()
puts s
