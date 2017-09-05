
**This repository is home to three separate but related projects.**


## The Great Type Loophole (C++14)

  Header file: [type-loophole.h][]

  Struct Reader is very limited in what it can do. You can't use std::string or a virtual type
  with it, you need to add types to a list before using them. While researching the thing I 
  uncovered a **Great Type Loophole**. Read more in the [blog post][e] with online examples.


## Luple: A Lightweight Tuple (C++14)

  Header file: [luple.h][]

  Luple is a tuple-like data structure. Its primary advantage is that it's very thin and
  also non-recursive (uses multiple inheritance). std::tuple is usually implemented as a single
  inheritance hierarchy and as a result its layout can vary between compilers (see [this post][l]).
  Also it stores data in reverse order. Luple is built on multiple inheritance and its layout is 
  consistent among compilers. It has the same layout as a non-virtual flat data structure with 
  the same data members (formally luple is not a POD and we should keep that in mind when we
  reinterpret\_cast it). Luple can be used in constexpr functions if its data members are of 
  literal type. Check the header for API description.

  See it in action [online at tio.run][c] (or at [Ideone][c]).


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
  [c]: https://goo.gl/ojAhbb "Luple Online Example"
  [d]: https://ideone.com/nK9ttI "Luple Online Example"
  [luple.h]: https://github.com/alexpolt/luple/blob/master/luple.h
  [struct-reader.h]: https://github.com/alexpolt/luple/blob/master/struct-reader.h
  [type-loophole.h]: https://github.com/alexpolt/luple/blob/master/type-loophole.h



