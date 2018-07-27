
import Exception

class AuthError(Exception):
    """
    """

    status = 500

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
