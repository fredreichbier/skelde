from struct import pack

BYTECODE_VERSION = '\x00\x02'

OP_MESSAGE = 1
OP_EOA = 2

class Message(object):
    def __init__(self, name, arguments=None, previous=None):
        self.name = name
        if arguments is None:
            arguments = []
        self.arguments = arguments
        self.previous = previous

    def generate_bytecode(self):
        bytecode = pack('B', OP_MESSAGE)
        bytecode += pack_string(self.name)
        bytecode += pack('B', len(self.arguments))
        for argument in self.arguments:
            bytecode += argument.generate_avalanche()
        return bytecode

    def generate_avalanche(self):
        chain = [self]
        while chain[0].previous:
            chain.insert(0, chain[0].previous)
        bytecode = ''
        for obj in chain:
            bytecode += obj.generate_bytecode()
        bytecode += pack('B', OP_EOA)
        return bytecode

    def generate_full(self):
        return 'sk' + BYTECODE_VERSION + self.generate_avalanche()

    def __call__(self, name, arguments=None):
        msg = Message(name, arguments, self)
        return msg

    def print_me(self, indent=0):
        if self.previous:
            self.previous.print_me(indent)
        print '    ' * indent + self.name
        for idx, arg in enumerate(self.arguments):
            print '    ' * indent + '[%d] =>' % idx
            arg.print_me(indent + 1)

def pack_string(s):
    return pack('%dsB' % len(s), s, 0)

