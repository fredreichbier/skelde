from struct import pack

from . import opcodes as op

class Node(object):
    def __init__(self):
        pass

    def build(self):
        raise NotImplementedError()

class String(Node):
    def __init__(self, value):
        Node.__init__(self)
        self.value = value

    def build(self):
        return pack('ii%dc' % len(self.value), op.STRING, len(self.value), *self.value)

class Number(Node):
    def __init__(self, value):
        Node.__init__(self)
        self.value = value

    def build(self):
        return pack('ii', op.NUMBER, self.value)

class Block(Node):
    def __init__(self, children=None):
        Node.__init__(self)
        if children is None:
            children = []
        self.children = []

    def build(self):
        code = pack('ii', op.BLOCK, len(self.children))
        for child in self.children:
            code += child.build()
        return code

class Nil(Node):
    def build(self):
        return pack('i', op.NIL)

NIL = Nil()

class Message(Node):
    def __init__(self, name, receiver=NIL, arguments=None):
        """
            :Parameters:
                `name` : str or String
                    Name of the message
        """
        Node.__init__(self)
        self.name = name
        self.receiver = receiver
        if arguments is None:
            arguments = []
        self.arguments = arguments

    def build(self):
        code = pack('i', op.MESSAGE)
        name = self.name
        if isinstance(name, basestring):
            name = String(name)
        # name
        code += name.build()
        # receiver
        code += self.receiver.build()
        # arguments
        code += pack('i', len(self.arguments))
        for argument in self.arguments:
            code += argument.build()
        print repr(code)
        return code

