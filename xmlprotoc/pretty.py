def _depth(element):
    depth = 0
    parent = element.parent
    while parent:
        depth += 1
        parent = parent.parent
    return depth


class PrintVisitor:
    """Debug visitor to pretty-print() a protocol and all subelements"""
    def visitMessage(self, message):
        print('\t' * _depth(message), message, sep='')

    def visitMessageGroup(self, group):
        print('\t' * _depth(group), group, sep='')
        for child in group.childs:
            child.accept(self)

    def visitNode(self, node):
        print('\t' * _depth(node), node, sep='')
        for child in node.childs:
            child.accept(self)

    def visitProtocol(self, protocol):
        print(protocol)
        for node in protocol.childs:
            node.accept(self)


def pretty(protocol):
    protocol.accept(PrintVisitor())
