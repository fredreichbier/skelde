class Coord(object):
    def __init__(self, line, col):
        self.line = line
        self.col = col

    @classmethod
    def from_d(cls, loc):
        return cls(loc.line, loc.col)

    def __repr__(self):
        return '<%s line=%d col=%d>' % (type(self).__name__, self.line, self.col)

class Node(object):
    def __init__(self, coord):
        self.coord = coord

    def __repr__(self):
        return '<%s at 0x%x, coord=%r>' % (type(self).__name__, id(self), self.coord)

class Block(Node):
    def __init__(self, coord, messages):
        Node.__init__(self, coord)
        self.messages = messages

class Message(Node):
    def __init__(self, coord, left, name, arguments):
        Node.__init__(self, coord)
        self.left = left
        self.name = name
        self.arguments = [] if arguments is None else arguments

class String(Node):
    def __init__(self, coord, value):
        Node.__init__(self, coord)
        self.value = value

class Int(Node):
    def __init__(self, coord, value):
        Node.__init__(self, coord)
        self.value = value

class SetSlot(Node):
    def __init__(self, coord, left, name, value):
        Node.__init__(self, coord)
        self.left = left
        self.name = name
        self.value = value

class Visitor(object):
    def visit(self, node):
        return self.dispatch(node)

    def dispatch(self, node, *args):
        clsname = node.__class__.__name__
        meth = getattr(self, 'visit_' + clsname, self.default)
        return meth(node, *args)

    def default(self, node, *args):
        print 'No visitor for Node %s, args=%s' % (node, args)
        return NotImplemented

