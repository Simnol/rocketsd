#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""XML to #defines compiler"""


import argparse

from lxml import etree

from pretty import pretty
from compile import compile
from log import error
from util import joinstr
from structure import Protocol
from validation import validate
from version import __version__


def main(args):
    try:
        parser = etree.XMLParser(dtd_validation=True)
        root = etree.parse(args.xml, parser).getroot()
    except etree.XMLSyntaxError as e:
        error('syntax', repr(e))
        return 1

    protocol = Protocol.parse(root)

    if not validate(protocol):
        return 1

    if args.out:
        compile(protocol, args)

    if args.print:
        pretty(protocol)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--version', action='version', version=joinstr('.', *__version__))  # noqa
    parser.add_argument('xml', action='store', type=str, help='Input protocol XML file')  # noqa
    parser.add_argument('--out', action='store', required=False, type=str, help='C-language Output file')  # noqa
    parser.add_argument('-B', '--prepend', action='store', required=False, type=str, help='File to preprend to output')  # noqa
    parser.add_argument('-A', '--append', action='store', required=False, type=str, help='File to apprend to output')  # noqa
    parser.add_argument('--print', action='store_true', required=False, help='Debug print')  # noqa
    exit(main(parser.parse_args()))
