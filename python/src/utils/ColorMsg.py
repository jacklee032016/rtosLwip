
from termcolor import cprint

def test_msg(msg):
    cprint(msg, None, attrs=['bold'])

def error_msg(msg):
    cprint(msg,  'red', attrs=['bold'])

def success_msg(msg):
    cprint(msg,  'blue', attrs=['bold'])

def debug_msg(msg, debug=False):
    if debug == True:
        cprint(msg,  'green', attrs=['bold'])
