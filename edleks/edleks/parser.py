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
                  | arrow
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

def d_arrow(t, nodes):
    r''' arrow: '{' arguments_really '->' (NL*) arguments_really '}' '''
    return make_node(nodes, ast.Arrow, t[1] + t[4])

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
    ''' paren: '(' expressions ')'
             | '(' expression ')'
    '''
    return t[1]

def d_arguments(t):
    ''' arguments: '(' arguments_really? ')' '''
    if t[1]:
        return t[1][0]
    else:
        return []

def d_arguments_really(t):
    ''' arguments_really: expressions (',' expressions)* '''
    return [t[0]] + map(lambda x: x[1], t[1])

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
    r''' identifier: "[^0-9 ;()\":=\r{}\n,][^ ;()\":=\r\n{},]*" ''' # kind of complicated!
    return t[0]

def d_string(t, nodes):
    r''' string: "\"[^\"]*\"" '''
    return make_node(nodes, ast.String, t[0].decode('string-escape')[1:-1])

def d_int(t, nodes): # TODO: float
    r''' int: "[0-9_]+" '''
    return make_node(nodes, ast.Int, t[0])

def parse(s):
    parser = Parser()
    return parser.parse(s, print_debug_info=0).getStructure()
