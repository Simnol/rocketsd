from enum import Enum

from util import isonow


class Level(Enum):
    FATAL = 0
    ERROR = 1
    WARN = 2
    INFO = 3
    DEBUG = 4

    def __repr__(self):
        return {
            Level.FATAL: 'fatal',
            Level.ERROR: 'error',
            Level.WARN: 'warn',
            Level.INFO: 'info',
            Level.DEBUG: 'debug'
        }[self]


def log(level, component, message, *args, **kwargs):
    date = isonow()
    level = repr(level)
    print(f'[{date}] ({level}) [{component}] {message}')


def fatal(component, message, *args, **kwargs):
    log(Level.FATAL, message, args, kwargs)
    exit(1)


def error(component, message, *args, **kwargs):
    log(Level.ERROR, component, message, args, kwargs)


def warn(component, message, *args, **kwargs):
    log(Level.WARN, component,  message, args, kwargs)


def info(component, message, *args, **kwargs):
    log(Level.INFO, component, message, args, kwargs)


def debug(component, message, *args, **kwargs):
    log(Level.DEBUG, component, message, args, kwargs)
