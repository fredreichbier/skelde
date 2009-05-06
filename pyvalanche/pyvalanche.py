from __future__ import with_statement
from skelde import Message

END = 0
IDENTIFIER = 1
LPAREN = 2
RPAREN = 3
COMMA = 4

def tokenize(string):
    pos = 0
    while pos < len(string):
        if string[pos] == '(':
            yield (LPAREN, string[pos])
            pos += 1
        elif string[pos] == ')':
            yield (RPAREN, string[pos])
            pos += 1
        elif string[pos] == ',':
            yield (COMMA, string[pos])
            pos += 1
        elif string[pos] == ';':
            yield (IDENTIFIER, string[pos])
            pos += 1
        elif string[pos] in ' \t\r\n':
            pos += 1
        elif string[pos] == '#':
            while pos < len(string) and string[pos] != '\n':
                pos += 1
        elif string[pos] == '"':
            first = string[pos]
            value = first
            pos += 1
            while True:
                if string[pos] == '\\':
                    value += string[pos:pos+1].decode('string-escape')
                    pos += 2
                elif string[pos] == first:
                    value += first
                    pos += 1
                    break
                else:
                    value += string[pos]
                    pos += 1
            yield (IDENTIFIER, value)
        else:
            value = ''
            while (pos < len(string) and string[pos] not in '(), \n\t\r;'):
                value += string[pos]
                pos += 1
            yield (IDENTIFIER, value)
    yield (END, None)

class ParseError(Exception):
    pass

class Parser(object):
    def __init__(self, iterator):
        self.iterator = iterator
        self.token = None
        self.next()

    def next(self):
        self.token = self.iterator.next()

    def parse(self, end=(END,)):
        tail = None
        while self.token[0] not in end:
            shift = True
            if self.token[0] == IDENTIFIER:
                name = self.token[1]
                args = []
                self.next()
                if self.token[0] == LPAREN:
                    # arguments ...
                    self.next()
                    args.append(self.parse((COMMA, RPAREN)))
                    while self.token[0] == COMMA:
                        self.next()
                        args.append(self.parse((COMMA, RPAREN)))
                    if self.token[0] != RPAREN:
                        raise ParseError(
                                "arguments not terminated by a rparen")
                else:
                    shift = False
                if not tail:
                    tail = Message(name, args)
                else:
                    tail = tail(name, args)
            if shift:
                self.next()
        return tail

if __name__ == '__main__':
    import sys
    if len(sys.argv) < 2:
        print 'Usage: python pyvalanche.py INPUT [OUTPUT]'
        print '(if no OUTPUT is given, the skelde bytecode is printed to stdout)'
        sys.exit(1)

    with open(sys.argv[1], 'r') as f:
        text = f.read()

    msg = Parser(tokenize(text)).parse()
    full = msg.generate_full()
    if len(sys.argv) == 2:
        msg.print_me()
    else:
        with open(sys.argv[2], 'w') as f:
            f.write(full)

