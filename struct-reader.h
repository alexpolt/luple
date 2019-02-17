/*

Struct Reader (C++14)

Author: Alexandr Poltavsky, http://alexpolt.github.io

License: Public-domain software

Description: 

  Reads the types of data members of a struct and returns a type list. A dozen lines of code.
  Limited to literal/non-const/non-literal/non-array types (arrays are unfold into separate members).
  All built-in C++ scalar types (bool, int, float, etc.), pointers to them (including to const 
  variants) are supported. To support your custom type add it to type_list_t in the header.
  
  Read more in a blog post: http://alexpolt.github.io/struct-tuple.html

  UPD. Later I've come up with an even more sophisticated hack called The Great Type Loophole.
  check the blog post http://alexpolt.github.io/type-loophole.html

Dependencies: 

  luple.h (a lightweight tuple): luple, luple_t, luple_ns::type_list, luple_ns::tlist_get_n
  utility: std::integer_sequence
  type_traits: std::conditional_t, std::remove_pointer_t, std::remove_const_t, std::is_pointer,
              std::is_const, std::add_const_t, std::add_pointer_t

Usage: 

  #include "struct-reader.h"

  struct data {
    int a;
    char const* b;
    float c;
    //my_class d; //for my_class to be recognized you need to add it to struct_reader::type_list_t
  };

  using data_tlist = struct_reader::as_type_list< data >; //returns luple_ns::type_list<...>

  //luple_t<...> - takes luple_ns::type_list< list of types >, luple<...> takes a list of types directly
  //for more information refer to luple.h

  using data_luple = luple_t< data_tlist >;

  data d{ 1, "test", 1.f };

  auto& l = *reinterpret_cast< data_luple* >( &d );

  //data_luple is not a POD but its layout matches struct data on Clang, GCC and MSVC
  //more details is in the blog post http://alexpolt.github.io/struct-layout.html

  get<2>(l) = 3.f;

  get<int>(l) = 10;

  You can find links to online working examples in the blog post
  http://alexpolt.github.io/struct-tuple.html

*/


namespace struct_reader {

  using namespace luple_ns;

  //this is the main type list, add your own types here
  using type_list_t = type_list<
            void *, bool, char, unsigned char, signed char, short, int, long, long long, 
            unsigned short, unsigned int, unsigned long, unsigned long long, 
            float, double, long double
          >;

  //helper to get type using a templated conversion operator
  template<typename T>
  struct read_type {
    template<typename U>
    constexpr operator U() {
      using noptr = std::remove_pointer_t<U>;
      using nocon = std::remove_const_t<noptr>;
      static_assert( tlist_get_n<T, U>::value != -1 || tlist_get_n<T, nocon>::value != -1, "no such type in type list");
      constexpr int const tid = 0xFFFF, is_ptr = 1 << 16, is_con = 1 << 17;
      data = tlist_get_n<T, U>::value;
      if( data == -1 ) {
        data = tlist_get_n<T, nocon>::value & tid;
        data = data | (std::is_pointer<U>::value ? is_ptr : 0);
        data = data | (std::is_const<noptr>::value ? is_con : 0);
      }
      return {};
    }
    int data;
  };

  using read_type_t = read_type< type_list_t >;

  //here we're using overload resolution to get a data member type
  template<typename T, int... N>
  constexpr auto get_type_id(int n) {
    read_type_t tid[sizeof...(N)]{};
    T{ tid[N]... };
    return tid[n].data;
  }

  //helper to rebuild the type
  template<typename T, int tid, int is_pointer, int is_const>
  constexpr auto get_type() {
    using type = tlist_get_t<T, tid>;
    using ctype = std::conditional_t< (bool)is_const, std::add_const_t<type>, type >;
    using ptype = std::conditional_t< (bool)is_pointer, std::add_pointer_t<ctype>, ctype >;
    return ptype{};
  }

  static constexpr int const tid = 0xFFFF;
  static constexpr int const is_ptr = 1 << 16;
  static constexpr int const is_con = 1 << 17;

  //read struct data member types and put it into a type list
  template<typename T, int... N>
  constexpr auto get_type_list(std::integer_sequence<int, N...>) {
    constexpr int t[] = { get_type_id<T, N...>(N)... };
    (void)t; // maybe unused if N == 0
    return type_list< decltype(get_type<type_list_t, t[N]&tid, t[N]&is_ptr, t[N]&is_con>())...>{};
  }

  //get fields number using expression SFINAE
  template<typename T, int... N>
  constexpr int fields_number(...) { return sizeof...(N)-1; }

  template<typename T, int... N>
  constexpr auto fields_number(int) -> decltype(T{ (N,read_type_t{})... }, sizeof(0)) { return fields_number<T, N..., 0>(0); }

  //and here is our hot and fresh out of kitchen type list (alias template)
  template<typename T>
  using as_type_list = decltype(get_type_list< T >(std::make_integer_sequence< int, fields_number<T>(0) >{}));

}


