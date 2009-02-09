import pyskelde
from pyskelde.ast import * # uh. evil.

ast = Message(
        'set_slot',
        NIL,
        [
            String('foo'), 
            String('bar')
        ]
    )

with open('test.sk', 'w') as sk:
    # write prelude
    sk.write('sk\x00\x01')
    # build the ast
    sk.write(ast.build())

