from . import ast, skelde

class CompilingVisitor(ast.Visitor):
    def visit_Message(self, node):
        previous = self.visit(node.left) if node.left else None
        arguments = map(self.visit, node.arguments)
        return skelde.Message(node.name, arguments, previous)

    def visit_String(self, node):
        return skelde.Message('"%s"' % node.value.encode('string-escape'))

    def visit_Int(self, node):
        return skelde.Message(node.value)

    def visit_BinaryOp(self, node):
        return self.visit(node.left)(node.op, [self.visit(node.right)])

    def visit_Block(self, node):
        last_sk_message = None
        # connect all messages with ';' messages
        for idx, message in enumerate(node.messages):
            sk_message = self.visit(message)
            p = sk_message
            if idx > 0:
                while True:
                    if p.previous:
                        p = p.previous
                    else:
                        p.previous = skelde.Message(';', None, last_sk_message)
                        break
            last_sk_message = sk_message
        # return rightest message
        return last_sk_message

    def visit_SetSlot(self, node):
        left = self.visit(node.left) if node.left is not None else None
        return skelde.Message('set_slot',
                [skelde.Message('"%s"' % node.name), self.visit(node.value)],
                left)

    def visit_UpdateSlot(self, node):
        left = self.visit(node.left) if node.left is not None else None
        return skelde.Message('update_slot',
                [skelde.Message('"%s"' % node.name), self.visit(node.value)],
                left)
