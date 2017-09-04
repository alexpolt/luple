/*

Luple: a Lightweight Tuple

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
  Check the struct_reader.h file for more details.

Dependencies: 
  utility: std::integer_sequence, std::forward, std::move
  type_traits: std::conditional_t, std::is_same, std::enable_if

Usage:

  Construction:

    luple< int, char ... > l0;
    luple< std::string > l0{ "test" };
    luple< std::string > l0{ std::string{"test"} };
    luple< std::string > l1{ l0 };
    luple< std::string > l1{ std::move( l0 ) };

  Assigment:

    l1 = l0;
    l1 = std::move( l0 );

  Accesing data:

    using my_luple_t = luple< int, std::string >;

    my_luple_t l2{ 10, "test" };

    auto& n = get<0>( l2 );
    auto& str = get<std::string>( l2 );

    auto sz = size( l2 );
    auto sz = my_luple_t::type_list::size;

    using element0_t = luple_ns::tlist_get_t< my_luple_t::type_list, 0 >;

    const int int_index = luple_ns::tlist_get_n< my_luple_t::type_list, int >::value;
    const int int_index = index<int>( l2 );
    static_assert( int_index != -1 , "not found" );

*/


#include <utility>
#include <type_traits>

namespace luple_ns {

  //type list
  template<typename... TT> struct type_list { static const int size = sizeof...(TT); };

  //get element by index
  template<typename T, int N, int M = 0> struct tlist_get;

  template<int N, int M, typename T, typename... TT> struct tlist_get< type_list<T, TT...>, N, M > {
    static_assert(N < (int) sizeof...(TT)+1 + M, "type index out of bounds");
    using type = std::conditional_t< N == M, T, typename tlist_get< type_list<TT...>, N, M + 1 >::type >;
  };

  template<int N, int M> struct tlist_get< type_list<>, N, M > {
    using type = void;
  };

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
    constexpr tuple_base ( TT const&... args ) : tuple_element< tlist, NN >{ args }... {}
    constexpr tuple_base ( TT&&... args ) : tuple_element< tlist, NN >{ std::move(args) }... {}
    constexpr tuple_base () {}
  };

  //tuple=luple
  //T: type_list< ... user types ... >
  template<typename T> struct tuple : tuple_base< T, std::make_integer_sequence<int, T::size> > {

    using type_list = T;
    using base = tuple_base< T, std::make_integer_sequence<int, T::size> >;

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

  template<int N, typename T> constexpr auto& get ( tuple<T>& t ) { return t.template get<N>(); }
  template<typename U, typename T> constexpr auto& get ( tuple<T>& t ) { return t.template get<U>(); }

  template<int N, typename T> constexpr auto& get ( tuple<T> const& t ) { return t.template get<N>(); }
  template<typename U, typename T> constexpr auto& get ( tuple<T> const& t ) { return t.template get<U>(); }

  template<typename T> constexpr auto size ( tuple<T> const& t ) { return T::size; }
  template<typename U, typename T> constexpr auto index ( tuple<T> const& t ) { return tlist_get_n< T, U >::value; }

  //template alias to wrap types into type_list
  template<typename... TT>
  using luple = tuple< type_list<TT...> >;
}

//import into global namespace
using luple_ns::luple;
using luple_ns::get;
using luple_ns::index;


