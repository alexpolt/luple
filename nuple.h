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

  using nameid_t = nuple< $("name"), char const*, $("id"), int >; //$(...) - see intern.h header

  nameid_t n[] = { {"alex", 1}, {"ivan", 2} };

  for( auto const& v : n )
    printf( "name: %s, id: %d\n", get<$("name")>(v), get<$("id")>(v) );


  auto get_person ( int i ) { return nameid_t{ "john", i }; }

  auto p = get_person( 3 );

  printf( "name: %s, id: %d\n", get<$("name")>( p ), get<$("id")>( p ) );


  //nuple extends luple, so luple methods work as usual (as in tuple)

  printf( "luple size: %d\n", size( p ) );

  get<0>( p ) = "irene"; //or get<$("name")>( p ) = "irene";
  
  get<int>( p ) = 4; //or get<$("id")>( p ) = 4;

  bool is_equal = p == p;

  //see luple.h for more examples

*/


#include "luple.h"
#include "intern.h"


namespace nuple_ns {

  //filter template sorts nuple parameters into two lists: 
  //types (passed to luple) and names (used for looking up member index)

  template<typename TL, typename NL, typename... TT> struct filter_ {
    using nlist = NL;
    using tlist = TL;
  };

  template<typename... TL, typename... NL, typename... TT, char... NN>
  struct filter_< luple_ns::type_list<TL...>, luple_ns::type_list<NL...>, intern::string<NN...>, TT... > {
    using tl = luple_ns::type_list<TL...>;
    using nl = luple_ns::type_list<NL..., intern::string<NN...>>;
    using nlist = typename filter_< tl, nl, TT... >::nlist;
    using tlist = typename filter_< tl, nl, TT... >::tlist;
  };

  template<typename... TL, typename... NL, typename T, typename... TT>
  struct filter_< luple_ns::type_list<TL...>, luple_ns::type_list<NL...>, T, TT... > {
    using tl = luple_ns::type_list<TL..., T>;
    using nl = luple_ns::type_list<NL...>;
    using nlist = typename filter_< tl, nl, TT... >::nlist;
    using tlist = typename filter_< tl, nl, TT... >::tlist;
  };

  template<typename... TT>
  using filter = filter_< luple_ns::type_list<>, luple_ns::type_list<>, TT... >;


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
  constexpr auto& get ( nuple<TT...>& t ) {

    static_assert( luple_ns::tlist_get_n< typename nuple<TT...>::name_list, T>::value != -1, "no such nuple name" );

    return get< luple_ns::tlist_get_n< typename nuple<TT...>::name_list, T >::value >( t ); 
  }

  template<typename T, typename = std::enable_if_t< intern::is_string<T>::value >, typename... TT> 
  constexpr auto& get ( nuple<TT...> const& t ) {

    static_assert( luple_ns::tlist_get_n< typename nuple<TT...>::name_list, T>::value != -1, "no such nuple name" );
    
    return get< luple_ns::tlist_get_n< typename nuple<TT...>::name_list, T >::value >( t ); 
  }

  //nuple member name (interned string) for index
  template<typename T, int N>
  using name_t = luple_ns::tlist_get_t< typename T::name_list, N >;

}


using nuple_ns::nuple;


