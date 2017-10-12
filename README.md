
This repository is home to separate but related projects: 
C++ Type Loophole, luple, nuple, C++ String Interning, Struct Reader.


## The C++ Type Loophole (C++14)

  Header file: [type-loophole.h][]

  Struct Reader is very limited in what it can do. You can't use std::string or a virtual type
  with it, you need to add types to a list before using them. While researching the thing I 
  uncovered a **C++ Type Loophole**. Read more in the [blog post][e] with online examples.


## luple: A Lightweight (in terms of source code) Tuple (C++14)

  Header file: [luple.h][]

  Luple is a tuple-like data structure. Its primary advantage is that it's very thin and
  also non-recursive (uses multiple inheritance). std::tuple is usually implemented as a single
  inheritance hierarchy and as a result its layout can vary between compilers (see [this post][l]).
  Also it stores data in reverse order. Luple is built on multiple inheritance and its layout is 
  consistent among compilers. It has the same layout as a non-virtual flat data structure with 
  the same data members (formally luple is not a POD and we should keep that in mind when we
  reinterpret\_cast it). Luple can be used in constexpr functions if its data members are of 
  literal type. Check the header for API description.

  See it in action [online at tio.run][c] (or at [Ideone][d]).

## nuple: a Named Tuple (C++14)

  Header file: [nuple.h][]

  nuple extends luple and allows giving names to data members. It works by using C++ string
  interning (intern.h) which allows for a neat syntax. Check this [blog post][n].

  Online example is [here at tio.run][n-tio] (or at [Ideone][n-ide]).

## C++ String Interning (C++14)

  Header file: [intern.h][]

  By turning a string literal into a type we can create an interned string in C++. This has
  a number of useful properties, nuple is one of them. Read more [here][i].

  There is a limit on string length of 10 characters (sort of arbitrary), you can increase it by 
  editing the $(...) macro in the intern.h header. Or #define N3599 to use [N3599][]
  proposal (adds string literal template to the language) which GCC and Clang implement as an 
  extension, hopefully MSVC will support it too.

  Also you can check an online example [here at tio.run][i-tio] (or at [Ideone][i-ide]).


## Struct Reader (C++14)

  Header file: [struct-reader.h][]

  Struct Reader can detect and create a type list of the data member types of a flat data 
  structure if they are literal, non-const/non-reference types. So, it's a form of primitive RTTI.
  We can turn the type list into a luple, so that means we can type cast our data to luple. 
  Check the [blog post][b] for more details about the technique and online examples.
  The API description is in the header.

  After discovery of the Great Type Loophole there is not much value to Struct Reader, I guess.

---

## License

  The code in this repository is Public-domain software.

  ![Pubic domain software](http://alexpolt.github.io/images/public_domain_mark.png)


  [l]: http://alexpolt.github.io/struct-layout.html "Visual C++ Struct Layout Reminder"
  [b]: http://alexpolt.github.io/struct-tuple.html "Structure Data Members as a Type List Using Pure C++ (C++14)"
  [e]: http://alexpolt.github.io/type-loophole.html "The Great Type Loophole (C++14)"
  [n]: http://alexpolt.github.io/named-tuple.html "nuple: a Named Tuple"
  [i]: http://alexpolt.github.io/intern.html "Useful Properties of String Interning in C++"

  [c]: https://goo.gl/ojAhbb "luple Online Example"
  [d]: https://ideone.com/nK9ttI "luple Online Example"
  [n-tio]: https://goo.gl/EMWgBG "nuple Online Example"
  [n-ide]: https://ideone.com/uvZ3uZ "nuple Online Example"
  [i-tio]: https://goo.gl/LfDTZ3 "C++ String Interning Online Example"
  [i-ide]: https://ideone.com/Dh6az7 "C++ String Interning Online Example"


  [luple.h]: https://github.com/alexpolt/luple/blob/master/luple.h
  [nuple.h]: https://github.com/alexpolt/luple/blob/master/nuple.h
  [intern.h]: https://github.com/alexpolt/luple/blob/master/intern.h

  [struct-reader.h]: https://github.com/alexpolt/luple/blob/master/struct-reader.h
  [type-loophole.h]: https://github.com/alexpolt/luple/blob/master/type-loophole.h

  [N3599]: http://open-std.org/JTC1/SC22/WG21/docs/papers/2013/n3599.html "Literal operator templates for strings"


