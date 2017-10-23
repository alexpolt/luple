
This repository is home to separate but related projects: 
C++ Type Loophole, luple, nuple, C++ String Interning, Struct Reader.



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

  See it in action [online at tio.run][l-tio] (also [Coliru][l-col] or [Wandbox][l-wan]).

  Read the header for API documentation.


## nuple: a Named Tuple (C++14)

  Header file: [nuple.h][]

  nuple extends luple and allows giving names to data members. It works by using C++ string
  interning (intern.h) which allows for a neat syntax. Check this [blog post][n].

  See it in action [online at tio.run][n-tio] (also [Coliru][n-col] or [Wandbox][n-wan]).
  
  Also a nuple-to-json [example at tio.run][j-tio] (also [Coliru][j-col] or [Wandbox][j-wan]).

  Read the header for API documentation.


## C++ String Interning (C++14)

  Header file: [intern.h][]

  By turning a string literal into a type we can create an interned string in C++. This has
  a number of useful properties, nuple is one of them. Read more [here][i].

  There is a limit on string length of 10 characters (sort of arbitrary), you can increase it by 
  editing the $(...) macro in the intern.h header. Or #define N3599 to use [N3599][]
  proposal (adds string literal template to the language) which GCC and Clang implement as an 
  extension, hopefully MSVC will support it too (Update: N3599 enabled by default).

  Also you can check an online example [here at tio.run][i-tio] (or at [Ideone][i-col]).

  Read the header for API documentation.


## Struct Reader (C++14)

  Header file: [struct-reader.h][]

  Struct Reader can detect and create a type list of the data member types of a flat data 
  structure if they are literal, non-const/non-reference types. So, it's a form of primitive RTTI.
  We can turn the type list into a luple, so that means we can type cast our data to luple. 
  Check the [blog post][b] for more details about the technique and online examples.
  The API description is in the header.

  After discovery of the Great Type Loophole there is not much value to Struct Reader, I guess.

  Read the header for API documentation.


## The C++ Type Loophole (C++14)

  Header file: [type-loophole.h][]

  Struct Reader is very limited in what it can do. You can't use std::string or a virtual type
  with it, you need to add types to a list before using them. While researching the thing I 
  uncovered a **C++ Type Loophole**. Read more in the [blog post][e] with online examples.

  Read the header for API documentation.

---

## License

  The code in this repository is Public-domain software.

  ![Pubic domain software](http://alexpolt.github.io/images/public_domain_mark.png)


  [l]: http://alexpolt.github.io/struct-layout.html "Visual C++ Struct Layout Reminder"
  [b]: http://alexpolt.github.io/struct-tuple.html "Structure Data Members as a Type List Using Pure C++ (C++14)"
  [e]: http://alexpolt.github.io/type-loophole.html "The Great Type Loophole (C++14)"
  [n]: http://alexpolt.github.io/named-tuple.html "nuple: a Named Tuple"
  [i]: http://alexpolt.github.io/intern.html "Useful Properties of String Interning in C++"

  [l-tio]: https://goo.gl/pdvYee "luple Online Example at tio.run"
  [l-col]: http://coliru.stacked-crooked.com/a/f40d4c6f63cdf166 "luple Online Example at Coliru"
  [l-wan]: https://wandbox.org/permlink/Awss0a3gxcaFKN8i "luple Online Example at Wandbox"

  [n-tio]: https://goo.gl/zvo26z "nuple Online Example at tio.run"
  [n-col]: http://coliru.stacked-crooked.com/a/52984bf7d0b4db19 "nuple Online Example at Coliru"
  [n-wan]: https://wandbox.org/permlink/ghbTSf5LwztoyCta "nuple Online Example at Wandbox"

  [j-tio]: https://goo.gl/c8ofW5 "nuple Online Example at tio.run"
  [j-col]: http://coliru.stacked-crooked.com/a/8f2f84adae0cb751 "nuple Online Example at Coliru"
  [j-wan]: https://wandbox.org/permlink/NBHrlq8UJ9kDf0KS "nuple Online Example at Wandbox"

  [i-tio]: https://goo.gl/sBtBKn "C++ String Interning Online Example at tio.run"
  [i-col]: http://coliru.stacked-crooked.com/a/ad43084765b89d9c "C++ String Interning Online Example at Coliru"


  [luple.h]: https://github.com/alexpolt/luple/blob/master/luple.h
  [nuple.h]: https://github.com/alexpolt/luple/blob/master/nuple.h
  [intern.h]: https://github.com/alexpolt/luple/blob/master/intern.h

  [struct-reader.h]: https://github.com/alexpolt/luple/blob/master/struct-reader.h
  [type-loophole.h]: https://github.com/alexpolt/luple/blob/master/type-loophole.h

  [N3599]: http://open-std.org/JTC1/SC22/WG21/docs/papers/2013/n3599.html "Literal operator templates for strings"


