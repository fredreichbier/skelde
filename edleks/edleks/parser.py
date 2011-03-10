from dparser import Parser

from . import ast

def make_node(nodes, cls, *args, **kwargs):
    return cls(ast.Coord.from_d(nodes[0].start_loc), *args, **kwargs)

def d_expressions(t, nodes):
    ''' expressions: (NL*) expression (separator expression (NL*))* (NL*) '''
    return make_node(nodes, ast.Block, [t[1]] + map(lambda x: x[1], t[2]))

def d_expression(t):
    ''' expression: message
                  | set_slot
                  | update_slot
    '''
    return t[0]

def d_message(t):
    ''' message: message_send
                  | string
                  | int
    '''
    return t[0]

def d_separator(t):
    ''' separator: "[;\n]*" '''
    return t[0]

def d_whitespace(t):
    r''' whitespace: "[ \r]+" '''
    pass

def d_NL(t):
    r''' NL: "[\n]*" '''
    pass

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

def d_arguments(t):
    ''' arguments: '(' (expressions (',' (NL*) expressions (NL*))*)? ')' '''
    if t[1]:
        return [t[1][0][0]] + map(lambda x: x[2], t[1][0][1:])
    else:
        return []

def d_identifier(t):
    ''' identifier: "[a-zA-Z_][a-zA-Z0-9_]*" '''
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
