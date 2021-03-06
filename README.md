# oop-in-c
Object-oriented C

## Why?
I'm doing this to learn and because I'm curious. I'll probably get bored with things eventually.

## Things planned
- [X] basic object system (single inheritance)
- [X] non-instantiable types
- [ ] interfaces/abstract classes
- [ ] generic values
- [ ] parameter specs
- [ ] generic classes? (ex: `HashMap<K,V>`)
- [ ] more stuff (closures?)

## Further comments
It would be nice to have some way of storing type parameters for classes. 
This is something that I don't think GObject does (it does have type parameters for properties, though).
That way, a HashMap would throw exceptions if given the wrong type.
It would also be nice to do this for certain non-instantiable types ("primitives") without boxing.

## Things to look into
1. [dynamic type system](https://en.wikipedia.org/wiki/Type_system#Static_and_dynamic_type_checking_in_practice)
2. [Java reflection with generics](http://stackoverflow.com/questions/3609799/how-to-get-type-parameter-values-using-java-reflection)
3. [GParamSpec](https://developer.gnome.org/gobject/stable/gobject-GParamSpec.html)
4. [C11's \_Generic](http://abissell.com/2014/01/16/c11s-_generic-keyword-macro-applications-and-performance-impacts/)
5. [Object Oriented Programming with ANSI C](https://www.cs.rit.edu/~ats/books/ooc.pdf)
6. [GObject Memory Management](https://developer.gnome.org/gobject/stable/gobject-memory.html)
7. [libffi](https://github.com/atgreen/libffi)
8. [Java super type tokens](http://gafter.blogspot.com/2006/12/super-type-tokens.html)

