
import copy
import re

# decorate function to remove odd parameters from kwargs
def remove_args(fx):
    def wrap(*args, **kwargs):
        kwargs2 = copy.copy(kwargs)

        ret = None
        done = False

        while not done:
            try:
                ret = fx(*args, **kwargs2)
                done = True
            except TypeError as err:  # for: TypeError: request() got an unexpected keyword argument 'uri'
                print("TypeError:{0}".format(err))
                # re.findall() return a list, so get first item in list
                key = re.findall("\'(\w+)\'", format(err))[0]
                key = re.findall("\'(\w+)\'", format(err))
                print("Method \"%s\" removing \"%s\"" % (fx.__name__, key) )
                del kwargs2[key]  # Remove offending key
        return ret

    return wrap

