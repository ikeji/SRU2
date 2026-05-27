# Single-level inheritance: subclass overrides, parent slot still accessible.
class Animal
  def initialize(name)
    self.name = name
  end
  def speak()
    puts self.name + " makes a generic sound"
  end
  def describe()
    puts "I am " + self.name
  end
end

class Dog < Animal
  def speak()
    puts self.name + " barks"
  end
end

class Cat < Animal
  def speak()
    puts self.name + " meows"
  end
end

a = Animal.new("Generic")
d = Dog.new("Rex")
c = Cat.new("Mittens")

a.speak()
d.speak()
c.speak()

# describe() is inherited from Animal
d.describe()
c.describe()
