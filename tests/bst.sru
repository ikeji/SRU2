# Binary search tree — class with self-referential nodes, recursive methods,
# and method chaining via `return self`.
class Node
  def initialize(value)
    self.value = value
    self.left = nil
    self.right = nil
  end
end

class BST
  def initialize()
    self.root = nil
  end

  def insert(v)
    if self.root == nil
      self.root = Node.new(v)
    else
      self.insertAt(self.root, v)
    end
    self
  end

  def insertAt(n, v)
    if v < n.value
      if n.left == nil
        n.left = Node.new(v)
      else
        self.insertAt(n.left, v)
      end
    else
      if n.right == nil
        n.right = Node.new(v)
      else
        self.insertAt(n.right, v)
      end
    end
  end

  def inorder()
    self.inorderAt(self.root)
  end

  def inorderAt(n)
    if n != nil
      self.inorderAt(n.left)
      puts n.value.toS()
      self.inorderAt(n.right)
    end
  end
end

t = BST.new()
t.insert(5).insert(3).insert(7).insert(1).insert(4).insert(9).insert(2)
t.inorder()
