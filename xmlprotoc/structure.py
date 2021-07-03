from enum import Enum

from log import error


def _mandatory(xml, *elements):
    return all([key in xml.attrib for key in elements])


class MessageType(Enum):
    BOOL = 'bool'
    INT = 'int'
    FLOAT = 'float'
    STRING = 'string'

    def __str__(self):
        return self.value


class _NamedElement:
    def __init__(self, name, parent):
        self.name = name
        self.parent = parent


class _NamedComposedElement(_NamedElement):
    def __init__(self, name, parent):
        super().__init__(name, parent)
        self.childs = []

    def add(self, child):
        self.childs.append(child)


class Message(_NamedElement):
    """Leaf element of protocol, an individual message that can be sent.

        Each Message has at least 3 elements:
            - An ID (local to the it's Node)
            - A name
            - A type (oneof Bool, Int, Float, String)

        Additionnaly, a Message can be tagged as a command (command="yes"), but
        this is optional. By default this is false ("no")
    """

    def __init__(self, id, name, type, command, parent):
        super().__init__(name, parent)
        self.id = id
        self.type = type
        self.command = command

    def accept(self, visitor):
        visitor.visitMessage(self)

    def __repr__(self):
        return f'<Message id={self.id} name={self.name} type={self.type}{" command" if self.command else ""}>'  # noqa

    def fqn(self):
        names = []
        parent = self.parent
        while parent:
            names.append(parent.name)
            parent = parent.parent

        return '.'.join(reversed(names)) + '.' + self.name

    @staticmethod
    def parse(xml, parent):
        if _mandatory(xml, 'id', 'name', 'type'):
            if not xml.attrib['id'].isdigit():
                error("syntax", f'Invalid ID for message: {id} is not a number')  # noqa
                return None

            type = MessageType(xml.attrib['type'])
            if not type:
                return None

            command = True if 'command' in xml.attrib and xml.attrib['command'] == 'yes' else False  # noqa
            return Message(int(xml.attrib['id']), xml.attrib['name'], type, command, parent)  # noqa
        else:
            error("syntax", 'Missing mandatory attributes for Message')
            return None


class MessageGroup(_NamedComposedElement):
    """A group of messages that share a name token"""
    def __init__(self, name, parent):
        super().__init__(name, parent)

    def accept(self, visitor):
        visitor.visitMessageGroup(self)

    def __repr__(self):
        return f'<MessageGroup name={self.name}>'

    @staticmethod
    def parse(xml, parent):
        if _mandatory(xml, 'name'):
            group = MessageGroup(xml.attrib['name'], parent)

            for subxml in xml:
                child = None
                if subxml.tag == 'message':
                    child = Message.parse(subxml, group)

                if subxml.tag == 'message_group':
                    child = MessageGroup.parse(subxml, group)

                if child:
                    group.add(child)

            return group
        else:
            error("syntax", 'Missing mandatory attributes for MessageGroup')
            return None


class Node(_NamedComposedElement):
    """An object that can emit messages"""
    def __init__(self, id, name, parent):
        super().__init__(name, parent)
        self.id = id

    def accept(self, visitor):
        visitor.visitNode(self)

    def __repr__(self):
        return f'<Node id={self.id} name={self.name}>'

    @staticmethod
    def parse(xml, parent):
        if _mandatory(xml, 'id', 'name'):
            if not xml.attrib['id'].isdigit():
                error("syntax", f'Invalid ID for node: {id} is not a number')  # noqa
                return None

            node = Node(int(xml.attrib['id']), xml.attrib['name'], parent)

            for subxml in xml:
                child = None
                if subxml.tag == 'message':
                    child = Message.parse(subxml, node)

                if subxml.tag == 'message_group':
                    child = MessageGroup.parse(subxml, node)

                if child:
                    node.add(child)

            return node
        else:
            error("syntax", 'Missing mandatory attributes for Node')
            return None


class Protocol(_NamedComposedElement):
    """Root element for a Protocol definition"""
    def __init__(self, name):
        super().__init__(name, None)

    def accept(self, visitor):
        return visitor.visitProtocol(self)

    def __repr__(self):
        return f'<Protocol name={self.name}>'

    @staticmethod
    def parse(xml):
        if _mandatory(xml, 'name'):
            protocol = Protocol(xml.attrib['name'])
        else:
            return

        if 'c_prefix' in xml.attrib:
            protocol.c_prefix = xml.attrib['c_prefix']

        for subxml in xml:
            if subxml.tag == 'node':
                node = Node.parse(subxml, protocol)
                if node:
                    protocol.add(node)

        return protocol
