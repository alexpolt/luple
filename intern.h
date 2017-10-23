/*

C++ String Interning

Author: Alexandr Poltavsky, http://alexpolt.github.io

License: Public-domain software

Description:

  Turns a string literal into a type. 
  
  Read more in the blog post http://alexpolt.github.io/intern.html

  The macro $(...) has a limit of 10 characters. Edit it for longer strings.

  Would've been easier if MSVC implemented the proposal N3599 by Richard Smith
  "Literal operator templates for strings". 
  
  GCC and Clang implement it as an extension. This allows strings of any length. 

  To use this extension: #define N3599

  Update: N3599 is now enabled by default, waiting for MSVC to implement it

Dependencies: 

Usage:

  1. interning

    std::cout << $("an interned string")::value;

  2. as a template parameter

    some_template< $("tag") >

  3. overload resolution

    void method( $("apple") );
    void method( $("orange") );

    method( $("apple"){} );

  4. nuple - a named tuple (nuple.h)

    nuple< $("first"), int, $("second"), float > p;

    get< int >(p) = 5;

    get< $("second") >(p) = 5.f;

  and other things like linking by name, string processing, etc. 
  (see the proposal and read my blog post).

*/


//Use N3599 proposal by default on GCC and Clang

#define N3599


namespace intern {

  template<char... NN> struct string {

    static constexpr char const value[ sizeof...(NN) ]{NN...};

    static_assert( value[ sizeof...(NN) - 1 ] == '\0', "interned string was too long, see $(...) macro" );

    static constexpr auto data() { return value; }
  };

  template<char... N> constexpr char const string<N...>::value[];
 
  template<int N>
  constexpr char ch ( char const(&s)[N], int i ) { return i < N ? s[i] : '\0'; }

  template<typename T> struct is_string {
    static const bool value = false;
  };

  template<char... NN> struct is_string< string<NN...> > {
    static const bool value = true;
  };

}


#if !defined( _MSC_VER ) && defined( N3599 )

  template<typename T, T... C>
  auto operator ""_intern() {
    return intern::string<C..., T{}>{};
  }

  #define $( s ) decltype( s ## _intern )

#else

  //prefixing macros with a $ helps with readability
  #define $c( a, b ) intern::ch( a, b )

  //10 characters + '\0', add $c(...) for bigger strings
  #define $( s ) intern::string< $c(s,0),$c(s,1),$c(s,2),$c(s,3),$c(s,4),$c(s,5),$c(s,6),$c(s,7),$c(s,8),$c(s,9),$c(s,10) >

#endif


