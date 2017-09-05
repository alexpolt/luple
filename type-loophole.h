/*

The Great Type Loophole (C++14)

Author: Alexandr Poltavsky, http://alexpolt.github.io

License: Public-domain software

Description: 

  The Great Type Loophole is a technique that allows to exchange type information with template
  instantiations. Basically you can assign and read type information during compile time.
  Here it is used to detect data members of a data type. I described it for the first time in
  this blog post http://alexpolt.github.io/type-loophole.html .
  
  Before this, I've created a Struct Reader to get data member types from a struct,but it can only 
  work with literal types from a fixed list. This can detect any types in a struct (except for 
  references and cv-qualified types) with nothing but C++ (C++14). It doesn't require a pre-built 
  type list. It uses list initialization for this and because of that it is limited to aggregate 
  types. You can try it on something with a constructor but it will most likely fail (especially 
  if there are constructors with an std::initializer_list).

Dependencies: 

  luple.h (a lightweight tuple): luple_ns::type_list (bare template with a parameter pack)
  utility: std::integer_sequence

Usage: 

    #include "type-loophole.h"

    struct test { 

      test() {}
    };

    struct data { 
      test t0;  
      std::string t1; 
      std::vector<int> t2; 
    };

    using data_tlist = loophole_ns::as_type_list< data >; // type_list< test, std::string, std::vector >
    using data_luple = luple< data_tlist >; //check luple.h for API

    data d{ {}, "Hello World!", {1, 2 ,3} };
    auto& l = reinterpret_cast< data_luple& >

    auto sz = size( data_luple );
    
    printf("%s\n", get<std::string>(l).data());

    for (auto i : get<2>(l)) printf("%d, ",i);

  You can find links to online working examples in the blog post
  http://alexpolt.github.io/type-loophole.html

*/


#include "luple.h"


namespace loophole_ns {

  /*
    tag<T,N> generates friend declarations and helps with overload resolution.
    There are two types: one with the auto return type, which is the way we read types later.
    The second one is used in the detection of instantiations without which we'd get multiple
    definitions.
  */

  template<typename T, int N>
  struct tag {
    friend auto loophole(tag<T,N>);
    constexpr friend int cloophole(tag<T,N>);
  };


  /*
    The definitions of friend functions.
  */
  template<typename T, typename U, int N, bool B>
  struct fn_def {
    friend auto loophole(tag<T,N>) { return U{}; }
    constexpr friend int cloophole(tag<T,N>) { return 0; }
  };

  /*
    This specialization is to avoid multiple definition errors.
  */
  template<typename T, typename U, int N>
  struct fn_def<T, U, N, true> {};

  /*
    This has a templated conversion operator which in turn triggers instantiations.
    Important point, using sizeof seems to be more reliable. Also default template
    arguments are "cached" (I think). To fix that I provide a U template parameter to
    the ins functions which do the detection using constexpr friend functions and SFINAE.
  */
  template<typename T, int N>
  struct c_op {
    template<typename U, int M> static auto ins(...) -> int;
    template<typename U, int M, int = cloophole(tag<T,M>{}) > static auto ins(int) -> char;

    template<typename U, int = sizeof(fn_def<T, U, N, sizeof(ins<U, N>(0)) == sizeof(char)>)>
    operator U();
  };

  /*
    Here we detect the data type field number. The byproduct is instantiations.
    Uses list initialization. Won't work for types with user provided constructors.
    In C++17 there is std::is_aggregate which can be added later.
  */

  template<typename T, int... NN>
  constexpr int fields_number(...) { return sizeof...(NN)-1; }

  template<typename T, int... NN>
  constexpr auto fields_number(int) -> decltype(T{ c_op<T,NN>{}... }, 0) {
    return fields_number<T, NN..., sizeof...(NN)>(0);
  }

  /*
    This is a helper to turn a data structure into a type list.
    Usage is: loophole_ns::as_type_list< data_t >
    I keep dependency on luple (a lightweight tuple of my design) because it's handy
    to turn a structure into luple (tuple). luple has the advantage of more stable layout
    across compilers and we can reinterpret_cast between the data structure and luple.
    More details are in the luple.h header.
  */

  template<typename T, typename U>
  struct loophole_type_list;

  template<typename T, int... NN>
  struct loophole_type_list< T, std::integer_sequence<int, NN...> > {
    using type = luple_ns::type_list< decltype(loophole(tag<T, NN>{}))... >;
  };

  template<typename T>
  using as_type_list =
    typename loophole_type_list<T, std::make_integer_sequence<int, fields_number<T>(0)>>::type;

}


