#New-style classes inherit from object, or from another new-style class
class Dog(object):

    name = ''
    moves = []

    def __init__(self, name):
        self.name = name

    def moves_setup(self):
        print("%s is called" % (self.__class__.__name__))
        self.moves.append('walk')
        self.moves.append('run')

    def get_moves(self):
        return self.moves

class Superdog(Dog):

    #Let's try to append new fly ability to our Superdog
    def moves_setup(self):
        print("%s is called" % (self.__class__.__name__))
        #Set default moves by calling method of parent class

        # following get same results
        #super(Superdog, self).moves_setup()
        super().moves_setup()

        # following is wrong
        #super(Dog, self).moves_setup()
        self.moves.append('fly')

dog = Superdog('Freddy')
print( dog.name )# Freddy
dog.moves_setup()
print(dog.get_moves()) # ['walk', 'run', 'fly'].
#As you can see our Superdog has all moves defined in the base Dog class