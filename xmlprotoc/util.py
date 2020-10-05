import datetime


def isonow():
    return datetime.datetime.now().replace(microsecond=0).isoformat(sep=' ')


def joinstr(sep, *items):
    return sep.join([str(item) for item in items])
