/*

luple: a Lightweight Tuple (C++14)

Author: Alexandr Poltavsky, http://alexpolt.github.io

License: Public-domain software

Description:

  Luple is a tuple-like data structure. Its primary advantage is that it's very thin and
  also non-recursive: uses multiple inheritance. Using multiple inheritance has the advantage 
  of having the same layout as a non-virtual flat data structure with the same data members.
  With single inheritance compilers pack data in different ways.

  Still, luple is not a POD, so you're on your own if you reinterpret_cast it. Luple can be 
  used in constexpr functions. 
  
  Initially it was created as part of a structure data member types reading experiment.
  Check the struct-reader.h file for more details.

Dependencies: 

  utility: std::integer_sequence, std::forward, std::move
  type_traits: std::conditional_t, std::is_same, std::enable_if

Usage:

  #include "luple.h"

  Construction:

    //luple<...>

    using person_t = luple< char const*, int id >;

    person_t p[] = { { "alex", 0 }, { "ivan", 1 } };

    luple< std::string, int > l0{ "test", 10 };
    luple< std::string, int > l0{ std::string{"test"}, 20 };

    luple< std::string, int > l1{ l0 };
    luple< std::string, int > l1{ std::move( l0 ) };


    //luple_t< type_list<...> >

    //it's sometimes more useful to deal with a single T parameter 
    //rather dealing with a parameter pack TT...

    using tlist = luple_ns::type_list< int, int, int >;

    luple_t< tlist > obj{ 1, 2, 3 };

    template<typename T>
    void print( luple_t< T > const& ) { ... }

  Assigment:

    l1 = l0;
    l1 = std::move( l0 );

  Accesing data:

    using foo_t = luple< int, float, std::string >;

    foo_t f0{ 10, 10.f, "hello world" };

    auto& n = get< 0 >( f0 );
    auto& str = get< std::string >( f0 );
    get< float >( f0 ) = 20.f;

    auto sz = size( f0 );

    int int_index = index< std::string >( f0 );

    static_assert( int_index != -1 , "not found" );

    using element1_t = luple_ns::element_t< foo_t, 2 >;

    element1_t str{ "hello" };

    luple_do( f0, []( auto& value ) { std::cout << value << ", "; } );

  Comparison:

    using person_t = luple< char const*, int id >;

    person_t p[] = { { "alex", 0 }, { "ivan", 1 } };

    bool less = p[0] < p[1];
    bool equal = p[0] == p[1];

    //etc.

*/


#include <utility>
#include <type_traits>

namespace luple_ns {

  //type list
  template<typename... TT> struct type_list { static const int size = sizeof...(TT); };


  //get element type by index
  template<typename T, int N, int M = 0> struct tlist_get;

  template<int N, int M, typename T, typename... TT> struct tlist_get< type_list<T, TT...>, N, M > {
    static_assert(N < (int) sizeof...(TT)+1 + M, "type index out of bounds");
    using type = std::conditional_t< N == M, T, typename tlist_get< type_list<TT...>, N, M + 1 >::type >;
  };

  template<int N, int M> struct tlist_get< type_list<>, N, M > { using type = void; };

  template<int N> struct tlist_get< type_list<>, N, 0 > {};


  template<typename T, int N>
  using tlist_get_t = typename tlist_get<T, N>::type;

  //get element index by type
  template<typename T, typename U, int N = 0> struct tlist_get_n;

  template<typename U, int N, typename T, typename... TT> struct tlist_get_n< type_list<T, TT...>, U, N > {
    static const int value = std::is_same< T, U >::value ? N : tlist_get_n< type_list<TT...>, U, N + 1 >::value;
  };
  template<typename U, int N> struct tlist_get_n< type_list<>, U, N > {
    static const int value = -1;
  };


  //a building block that is used in multiple inheritane
  template<typename T, int N> struct tuple_element {
    tlist_get_t<T, N> value;
  };


  //base of luple and also parent of tuple_element's
  template<typename T, typename U> struct tuple_base;

  template<typename... TT, int... NN>
  struct tuple_base< type_list<TT...>, std::integer_sequence<int, NN...> > : tuple_element< type_list<TT...>, NN >... {

    using tlist = type_list<TT...>;

    template<typename... UU, typename U = std::enable_if_t< sizeof...(UU) == sizeof...(NN) >>
    constexpr tuple_base ( UU&&... args ) : tuple_element< tlist, NN >{ std::forward<UU>(args) }... {}

    template<typename... UU, typename = std::enable_if_t< sizeof...(UU) != sizeof...(NN) >>
    constexpr tuple_base ( UU const&... ) {
      static_assert( sizeof...(UU) == sizeof...(NN), "wrong number of arguments" );
    }

    constexpr tuple_base () {}
  };


  //tuple=luple
  //T: type_list< ... user types ... >
  template<typename T> struct tuple : tuple_base< T, std::make_integer_sequence<int, T::size> > {

    using type_list = T;
    using base = tuple_base< T, std::make_integer_sequence<int, T::size> >;

    static const int size = T::size;

    template<int N> constexpr auto& get() {
      static_assert(N < T::size, "tuple::get -> out of bounds access");
      return tuple_element< T, N >::value;
    }

    template<typename U> constexpr auto& get() {
      static_assert(tlist_get_n<T, U>::value != -1, "no such type in type list");
      return tuple_element< T, tlist_get_n<T, U>::value >::value;
    }

    template<int N> constexpr auto& get() const {
      static_assert(N < T::size, "tuple::get -> out of bounds access");
      return tuple_element< T, N >::value;
    }

    template<typename U> constexpr auto& get() const {
      static_assert(tlist_get_n<T, U>::value != -1, "no such type in type list");
      return tuple_element< T, tlist_get_n<T, U>::value >::value;
    }

    using base::base;
  };

  //template alias to wrap types into type_list
  template<typename... TT>
  using luple = tuple< type_list< TT... > >;

  //template alias that takes type_list directly
  template<typename T>
  using luple_t = tuple< T >;

  //get function helpers
  template<int N, typename T> constexpr auto& get ( tuple<T>& t ) { return t.template get<N>(); }
  template<typename U, typename T> constexpr auto& get ( tuple<T>& t ) { return t.template get<U>(); }

  template<int N, typename T> constexpr auto& get ( tuple<T> const& t ) { return t.template get<N>(); }
  template<typename U, typename T> constexpr auto& get ( tuple<T> const& t ) { return t.template get<U>(); }

  //tuple size
  template<typename T> constexpr auto size ( tuple<T> const& ) { return T::size; }

  //member index from type
  template<typename U, typename T> constexpr auto index ( tuple<T> const& ) { return tlist_get_n< T, U >::value; }

  //type for index
  template<typename T, int N>
  using element_t = tlist_get_t< typename T::type_list, N >;

  //helper to run code for every member of luple
  template<int... N, typename T0, typename T1>
  void luple_do_impl ( std::integer_sequence<int, N...>, T0& t, T1 fn ) {

    //in C++17 we got folding expressions

    char dummy[] = { ( fn( get<N>(t) ), char{} )... };

    (void)dummy;
  }

  //helper to run code for every member of tuple
  template<typename T0, typename T1>
  void luple_do ( T0& t, T1 fn ) {

    luple_do_impl( std::make_integer_sequence< int, T0::type_list::size >{}, t, fn );
  }
  
}

//import into global namespace
using luple_ns::luple;
using luple_ns::luple_t;
using luple_ns::get;
using luple_ns::index;


//relational operators helpers

template<int N, typename T, typename U, typename = std::enable_if_t< N == T::size >>
bool luple_cmp_less ( T&, U& ) { return false; }

template<int N, typename T, typename U, typename = std::enable_if_t< (N < T::size) >>
bool luple_cmp_less ( luple_t< T > const& a, luple_t< U > const& b ) {

  bool less = get< N >( a ) < get< N >( b );
  bool equal = get< N >( a ) == get< N >( b );

  return less ? true : ( equal ? luple_cmp_less< N+1 >( a, b ) : false );
}

template<int N, typename T, typename U, typename = std::enable_if_t< N == T::size >>
bool luple_cmp_equal ( T&, U& ) { return true; }

template<int N, typename T, typename U, typename = std::enable_if_t< (N < T::size) >>
bool luple_cmp_equal ( luple_t< T > const& a, luple_t< U > const& b ) {

  bool equal = get< N >( a ) == get< N >( b );

  return equal ? luple_cmp_equal< N+1 >( a, b ) : false;
}

//relational operators

template<typename T, typename U, typename = std::enable_if_t< (T::size > 0) && T::size == U::size >>
bool operator< ( luple_t< T > const& a, luple_t< U > const& b) {

  return luple_cmp_less< 0 >( a, b );
}

template<typename T, typename U, typename = std::enable_if_t< (T::size > 0) && T::size == U::size >>
bool operator== ( luple_t< T > const& a, luple_t< U > const& b) {

  return luple_cmp_equal< 0 >( a, b );
}

//the rest are easy

template<typename T, typename U>
bool operator!= ( luple_t< T > const& a, luple_t< U > const& b) { return !( a == b ); }

template<typename T, typename U>
bool operator> ( luple_t< T > const& a, luple_t< U > const& b) { return b < a; }

template<typename T, typename U>
bool operator<= ( luple_t< T > const& a, luple_t< U > const& b) { return !( a > b ); }

template<typename T, typename U>
bool operator>= ( luple_t< T > const& a, luple_t< U > const& b) { return !( a < b ); }



