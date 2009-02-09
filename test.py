from struct import pack

def number(value):
    return pack('ii', 2, value)

def string(value):
    return pack('ii%dc' % len(value), 3, len(value), *value)

def block(content, count):
    return pack('ii', 1, count) + content

def message(name, receiver, args):
    s = pack('i', 4)
    s += name + receiver
    s += pack('i', len(args))
    s += ''.join(args)
    return s

def nil():
    return pack('i', 5)

with open('test.sk', 'w') as sk:
    # write prelude
    sk.write('sk\x00\x01')
    # write number
    sk.write(message(string("print"), string("huhu"), []))

