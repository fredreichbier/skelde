from dparser import Parser

from . import ast

def make_node(nodes, cls, *args, **kwargs):
    return cls(ast.Coord.from_d(nodes[0].start_loc), *args, **kwargs)

def d_expressions(t, nodes):
    ''' expressions: (NL*) expression (separator expression (NL*))* (NL*) '''
    return make_node(nodes, ast.Block, [t[1]] + map(lambda x: x[1], t[2]))

def d_expression(t):
    ''' expression: set_slot
                  | update_slot
                  | paren
                  | if
                  | message
    '''
    return t[0]

def d_message(t):
    ''' message: message_send
                  | string
                  | int
                  | binary_op
    '''
    return t[0]

def d_separator(t):
    ''' separator: "[;\n]+" '''
    return t[0]

def d_whitespace(t):
    r''' whitespace: "[ \r]+" '''
    pass

def d_NL(t):
    r''' NL: "[\n]*" '''
    pass

def d_if(t, nodes):
    r''' if: '%if' expression arguments '''
    print t, nodes

def d_set_slot(t, nodes):
    r''' set_slot: expression? identifier ':=' expression '''
    left = t[0][0] if t[0] else None
    name = t[1]
    value = t[3]
    return make_node(nodes, ast.SetSlot, left, name, value)

def d_update_slot(t, nodes):
    r''' update_slot: expression? identifier '=' expression '''
    left = t[0][0] if t[0] else None
    name = t[1]
    value = t[3]
    return make_node(nodes, ast.UpdateSlot, left, name, value)

#def d_message(t, nodes):
#    ''' message: message_send
#               | string
#               | int '''
#    return t[0]

def d_message_send(t, nodes):
    ''' message_send: message? identifier arguments? '''
    left = t[0][0] if t[0] else None
    name = t[1]
    arguments = t[2][0] if t[2] else []
    return make_node(nodes, ast.Message, left, name, arguments)

def d_paren(t):
    ''' paren: '(' expression ')' '''
    return t[1]

def d_arguments(t):
    ''' arguments: '(' (expressions (',' expressions)*)? ')' '''
    if t[1]:
        return [t[1][0][0]] + map(lambda x: x[1], t[1][0][1:])
    else:
        return []

def d_binary_op(t, nodes):
    ''' binary_op: expression bop expression '''
    return make_node(nodes, ast.BinaryOp, t[1], t[0], t[2])

def d_bop(t):
    ''' bop: '*' $binary_op_left 2
           | '+' $binary_op_left 1
           | '==' $binary_op_left 1
    '''
    return t[0]

def d_identifier(t):
#    r''' identifier: "[a-zA-Z_][a-zA-Z0-9_]*" ''' # kind of stupid!
    r''' identifier: "[^0-9 ;()\":=\r\n,][^ ;()\":=\r\n,]*" ''' # kind of complicated!
    return t[0]

def d_string(t, nodes):
    r''' string: "\"[^\"]*\"" '''
    return make_node(nodes, ast.String, t[0].decode('string-escape')[1:-1])

def d_int(t, nodes): # TODO: float
    r''' int: "[0-9_]+" '''
    return make_node(nodes, ast.Int, t[0])

def parse(s):
    parser = Parser()
    return parser.parse(s).getStructure()
