"""Validation visitor for Protocol

    Checks and validates what can not be check by DTD:
        - Ensures Node IDs are unique
        - Ensures Message IDs are unique within Node scope
        - Ensures Node and Message IDs fit CAN frame (5 and 6 bit respectively)
        - Ensures Message FQNs are unique
"""
from log import error


def _dict_add_or_set(data, key, default=1, increment=1):
    """Add to dict value, set key not yet in dict"""
    if key in data:
        data[key] += increment
    else:
        data[key] = default


class ValidationVisitor:
    """Validates XML"""
    def __init__(self):
        super().__init__()

        self.errors = False
        self.full_names = {}
        self.current_node = 0
        self.message_ids = {}
        self.node_ids = {}

    def visitMessage(self, message):
        _dict_add_or_set(self.full_names, message.fqn())
        _dict_add_or_set(self.message_ids[self.current_node], message.id)

    def visitMessageGroup(self, group):
        for child in group.childs:
            child.accept(self)

    def visitNode(self, node):
        self.current_node = node.id
        self.message_ids[node.id] = {}

        _dict_add_or_set(self.node_ids, node.id)

        for child in node.childs:
            child.accept(self)

    def visitProtocol(self, protocol):
        for node in protocol.childs:
            node.accept(self)

        return self.validate()

    def validate(self):
        for name in self.full_names:
            if self.full_names[name] != 1:
                self.errors = True
                error('validation', f'Multiple messages named "{name}"')

        for node in self.node_ids:
            if node >= 32:
                self.errors = True
                error('validation', f'Node ID {node} exceeds 5-bit limit (31)')

            if self.node_ids[node] != 1:
                self.errors = True
                error("validation", f'Multiple nodes with ID {node}')

        for node in self.message_ids:
            for message in self.message_ids[node]:
                if message >= 64:
                    self.errors = True
                    error('validation', f'Message ID {message} exceeds 6-bit limit (63) (node {node})')  # noqa

                if self.message_ids[node][message] != 1:
                    self.errors = True
                    error("validation", f'Multiple messages with ID {message} for node {node}')  # noqa


def validate(protocol):
    """Validate protocol, returns true if there are no errors"""
    validator = ValidationVisitor()
    protocol.accept(validator)
    return not validator.errors
