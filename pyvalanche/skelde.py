from struct import pack

BYTECODE_VERSION = '\x00\x02'

OP_MESSAGE = 1
OP_EOA = 2

class Message(object):
    def __init__(self, name, arguments=None, previous=None, next=None):
        self.name = name
        if arguments is None:
            arguments = []
        self.arguments = arguments
        self.previous = previous
        self.next = next

    def generate_bytecode(self):
        bytecode = pack('B', OP_MESSAGE)
        bytecode += pack_string(self.name)
        bytecode += pack('B', len(self.arguments))
        for argument in self.arguments:
            bytecode += argument.generate_avalanche()
        return bytecode

    def generate_avalanche(self):
        obj = self
        while obj.previous:
            obj = obj.previous
        bytecode = ''
        while obj:
            bytecode += obj.generate_bytecode()
            obj = obj.next
        bytecode += pack('B', OP_EOA)
        return bytecode

    def generate_full(self):
        return 'sk' + BYTECODE_VERSION + self.generate_avalanche()

    def __call__(self, name, arguments=None):
        msg = self.next = Message(name, arguments, self)
        return msg

def pack_string(s):
    return pack('%dsB' % len(s), s, 0)

