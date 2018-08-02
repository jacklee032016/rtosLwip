
import functools

def singleton(cls):
    instances = dict()
    @functools.wraps(cls)
    def _singletone(*args, **kwargs):
        if cls not in instances:
            instances[cls] = cls

        return instances[cls](*args, **kwargs)

    return _singletone
