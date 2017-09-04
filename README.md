
**This repository is home to three separate but related projects.**

# Luple: A Lightweight Tuple (C++14)

  Luple is a tuple-like data structure. Its primary advantage is that it's very thin and
  also non-recursive (uses multiple inheritance). std::tuple is usually implemented as a single
  inheritance hierarchy and as a result its layout can vary between compilers (see [this post][l]).
  Luple is built on multiple inheritance and its layout is much more consistent among compilers.
  That means it has the same layout as a non-virtual flat data structure with the same data members.
  Still, luple is not a POD, so you're on your own if you reinterpret\_cast it. Luple can be used 
  in constexpr functions if its data members are of literal type.

  See it in action [online at tio.run][c].
  
# Struct Reader (C++14)

  Struct Reader can detect and create a type list of the data member types of a flat data 
  structure if they are literal, non-const/non-reference types. So, it's a form of primitive RTTI.
  We can turn the type list into a luple, so that means we can type cast our data to luple. 
  Check the [blog post][b] for more details about the technique and some online examples.

# The Great Type Loophole

  [l]: http://alexpolt.github.io/struct-layout.html "Visual C++ Struct Layout Reminder"
  [b]: http://alexpolt.github.io/struct-tuple.html
  [c]: https://goo.gl/bWwE9B "Luple Online Example"

