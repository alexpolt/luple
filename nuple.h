/*

nuple: a named tuple (C++14)

Author: Alexandr Poltavsky, http://alexpolt.github.io

License: Public-domain software

Description:

  nuple is a named tuple implementation. String interning (intern.h) makes for a neat
  interface. There is a blog post http://alexpolt.github.io/named-tuple.html

Dependencies: 

  luple.h: luple (lightweight tuple)
  intern.h: C++ string interning
  type_traits: std::enable_if

Usage:

  #include "nuple.h"

  using nameid_t = nuple< $("name"), char const*, $("id"), int >; //for $(...) - see intern.h header


  nameid_t n[] = { {"alex", 1}, {"ivan", 2} };

  for( auto const& v : n )

    printf( "name: %s, id: %d\n", get<$("name")>(v), get<$("id")>(v) );


  auto get_person ( int i ) { return nameid_t{ "john", i }; }

  //or - return nuple<$("name"), char const*, $("id"), int>{ "john", i };

  //or - return as_nuple( $name("name"), "john", $name("id"), i ); 


  auto p = get_person( 3 );

  printf( "name: %s, id: %d\n", get<$("name")>( p ), get<$("id")>( p ) );


  //as_nuple(...), similar to make_tuple, macro is $name(...) instead of $(...)

  auto get_person( int id ) {

    return as_nuple( $name("name"), std::string{ "john" }, $name("id"), id ); 
  }


  //nuple extends luple, so luple methods work as usual (as in tuple)

  printf( "luple size: %d\n", size( p ) );

  get<0>( p ) = "irene"; //or get<$("name")>( p ) = "irene";
  
  get<int>( p ) = 4; //or get<$("id")>( p ) = 4;

  bool is_equal = p == p;

  //see luple.h for more examples


  // nuple_ns::nuple_t - return type for tag name by index

  using field0_t = nuple_ns::name_t< nameid_t, 0 >; //the same as $("name")

*/


#include "luple.h"
#include "intern.h"


namespace nuple_ns {


  //filter template sorts nuple parameters into two lists: 
  //types (passed to luple) and names (used for looking up member index)

  template<typename TL, typename NL, int is_str, typename... TT> struct filter_;

  template<typename TL, typename NL, typename T, typename... TT> 
  struct filter_< TL, NL, -1, T, TT... > : 
    filter_< TL, NL, intern::is_string< T >::value, T, TT... > {};

  template<typename TL, typename NL, typename T, typename... TT> 
  struct filter_< TL, NL, true, T, TT... > : 
    filter_< TL, typename NL::template add<T>::type, -1, TT... > {};

  template<typename TL, typename NL, typename T, typename... TT> 
  struct filter_< TL, NL, false, T, TT... > : 
    filter_< typename TL::template add<T>::type, NL, -1, TT... > {};

  template<typename TL, typename NL, int is_str> struct filter_< TL, NL, is_str > {
    using nlist = NL;
    using tlist = TL;
  };

  template<typename... TT>
  using filter = filter_< luple_ns::type_list<>, luple_ns::type_list<>, -1, TT... >;


  //nuple is just a thin layer on top of luple

  template<typename... TT>
  struct nuple : luple_t< typename filter<TT...>::tlist > {

    using name_list = typename filter<TT...>::nlist;
    using base = luple_t< typename filter<TT...>::tlist >;

    static_assert( name_list::size == base::type_list::size, "name and type list sizes don't match" );

    using base::base;
  };

  //turn a nuple member name into an index and call get< index >( ... )

  template<typename T, typename = std::enable_if_t< intern::is_string<T>::value >, typename... TT> 
  constexpr auto & get ( nuple<TT...> & t ) {

    static_assert( luple_ns::tlist_get_n< typename nuple<TT...>::name_list, T>::value != -1, "no such nuple name" );

    return get< luple_ns::tlist_get_n< typename nuple<TT...>::name_list, T >::value >( t ); 
  }

  template<typename T, typename = std::enable_if_t< intern::is_string<T>::value >, typename... TT> 
  constexpr auto & get ( nuple<TT...> const & t ) {

    static_assert( luple_ns::tlist_get_n< typename nuple<TT...>::name_list, T>::value != -1, "no such nuple name" );
    
    return get< luple_ns::tlist_get_n< typename nuple<TT...>::name_list, T >::value >( t ); 
  }


  //nuple member name (interned string) for index
  template<typename T, int N>
  using name_t = luple_ns::tlist_get_t< typename T::name_list, N >;


  //as_nuple( ... )

  #define $name(s) $(s){}

  template<typename T, int N = 0, typename = void> 
  struct check_args {

    static const bool value = false;
  };

  template<typename T, typename... TT, int N>
  struct check_args< luple_ns::type_list<T, TT...>, N,

          std::enable_if_t< ( N%2 > 0 ) || ( N%2 == 0 && intern::is_string<T>::value ) >> {

    static const bool value = true && check_args< luple_ns::type_list<TT...>, N + 1 >::value;
  };

  template<int N> 
  struct check_args< luple_ns::type_list<>, N, void> {  

    static const bool value = true;
  };


  template<typename... TT, int... NN> 
  auto as_nuple_( luple<TT...> l, std::integer_sequence< int, NN... > ) {

    using param_list = luple_ns::type_list<TT...>;

    static_assert( check_args<param_list>::value, "order of arguments should be name, value..." );

    return nuple< TT... >{ std::move( get<NN*2+1>( l ) )... };
  }
  
  //as_nuple( $name("..."), value, ... ) -> nuple< $(...), decltype( value ), ... >
  //note the difference between $name(..) and $(...)

  template<typename... TT>
  auto as_nuple( TT... args ) {

    static_assert( sizeof...(TT) % 2 == 0, "wrong number of arguments");

    return as_nuple_( luple< TT... >{ std::move( args )... }, std::make_integer_sequence< int, sizeof...(TT)/2 >{} );
  }

}

//import into global namespace

using nuple_ns::nuple;
using nuple_ns::get;
using nuple_ns::as_nuple;


