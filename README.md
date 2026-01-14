# C++ Algebraic Data Types

This repository contains a library-based solution for the lack of pattern matching in C++. It introduces an `Inspect` function, which can be used similarly to a `switch` statement, with the distinction that it is an expression, not a statement (`switch` cannot return a value). A custom implementation of `std::expected` compatible with C++17 has also been added. The `Inspect` function also uses clearer error logging (it explicitly states if a case handler definition was forgotten).

## Example

The `std::variant` is really useful class, but not really ergonomic to work with. It requires usage of `std::visit` with properly defined visitors. With this library, you could use it similarly to Rust's `match` or C's `switch`. A snippet utilizing `adt::Inspect` as pattern matcher is shown below:

```c++
void test_variant() {

  std::cout << "Testing Variant Inspect:" << std::endl;

  std::variant<A, B, C> my_variant;
  my_variant = C{};

  auto article = adt::Inspect<std::string_view>(
      my_variant, [](A value) { return "an"; }, [](auto value) { return "a"; });

  std::cout << "It was " << article << ": "
            << adt::Inspect<char>(
                   my_variant,
                   [](A value) { return 'A'; },
                   [](B value) { return 'B'; },
                   [](C value) { return 'C'; })
            << std::endl;
}
```

### The real reason for using it

The underlying mechanism for this function is the _overloaded visitor_ pattern. It is a really powerful way of dealing with `std::variant`, it however provides really bad error messages, that show all the insides of standard library, in case if one variant is not handled. The `adt::Inspect` solves this issue by checking if all handlers were provided, error is checked prior to calling standard library functions, thus providing a clear error message. Let us "forget" to handle one variant:
```c++
  std::cout << "It was " << article << ": "
            << adt::Inspect<char>(
                   my_variant,
                   [](A value) { return 'A'; },
                  //  [](B value) { return 'B'; },
                   [](C value) { return 'C'; })
            << std::endl;

``` 
This is what we would get should it be only syntactic sugar for _overloaded visitor pattern_:

<details>
  <summary>Click to expand compilation logs without prior checks</summary>

```
In file included from /usr/include/c++/13/bits/move.h:37,
                 from /usr/include/c++/13/bits/exception_ptr.h:41,
                 from /usr/include/c++/13/exception:164,
                 from /usr/include/c++/13/ios:41,
                 from /usr/include/c++/13/ostream:40,
                 from /usr/include/c++/13/iostream:41,
                 from ../src/main.cpp:21:
/usr/include/c++/13/type_traits: In substitution of ‘template<class _Fn, class ... _Args> using std::invoke_result_t = typename std::invoke_result::type [with _Fn = adt::overloaded<test_variant()::<lambda(A)>, test_variant()::<lambda(C)> >; _Args = {B&}]’:
/usr/include/c++/13/variant:1131:14:   required from ‘constexpr bool std::__detail::__variant::__check_visitor_results(std::index_sequence<_Idx ...>) [with _Visitor = adt::overloaded<test_variant()::<lambda(A)>, test_variant()::<lambda(C)> >; _Variant = std::variant<A, B, C>&; long unsigned int ..._Idxs = {0, 1, 2}; std::index_sequence<_Idx ...> = std::integer_sequence<long unsigned int, 0, 1, 2>]’
/usr/include/c++/13/variant:1868:44:   required from ‘constexpr std::__detail::__variant::__visit_result_t<_Visitor, _Variants ...> std::visit(_Visitor&&, _Variants&& ...) [with _Visitor = adt::overloaded<test_variant()::<lambda(A)>, test_variant()::<lambda(C)> >; _Variants = {variant<A, B, C>&}; __detail::__variant::__visit_result_t<_Visitor, _Variants ...> = char]’
../inc/inspect.hh:233:24:   required from ‘constexpr auto adt::Inspect(Variant&&, Lambdas&& ...) [with R = char; Variant = std::variant<A, B, C>&; Lambdas = {test_variant()::<lambda(A)>, test_variant()::<lambda(C)>}; typename std::enable_if<traits::is_variant<Variant>::value, int>::type <anonymous> = 0]’
../src/main.cpp:65:34:   required from here
/usr/include/c++/13/type_traits:3073:11: error: no type named ‘type’ in ‘struct std::invoke_result<adt::overloaded<test_variant()::<lambda(A)>, test_variant()::<lambda(C)> >, B&>’
 3073 |     using invoke_result_t = typename invoke_result<_Fn, _Args...>::type;
      |           ^~~~~~~~~~~~~~~
In file included from ../inc/inspect.hh:18:
/usr/include/c++/13/variant: In instantiation of ‘constexpr std::__detail::__variant::__visit_result_t<_Visitor, _Variants ...> std::visit(_Visitor&&, _Variants&& ...) [with _Visitor = adt::overloaded<test_variant()::<lambda(A)>, test_variant()::<lambda(C)> >; _Variants = {variant<A, B, C>&}; __detail::__variant::__visit_result_t<_Visitor, _Variants ...> = char]’:
../inc/inspect.hh:233:24:   required from ‘constexpr auto adt::Inspect(Variant&&, Lambdas&& ...) [with R = char; Variant = std::variant<A, B, C>&; Lambdas = {test_variant()::<lambda(A)>, test_variant()::<lambda(C)>}; typename std::enable_if<traits::is_variant<Variant>::value, int>::type <anonymous> = 0]’
../src/main.cpp:65:34:   required from here
/usr/include/c++/13/variant:1868:44:   in ‘constexpr’ expansion of ‘std::__detail::__variant::__check_visitor_results<adt::overloaded<test_variant()::<lambda(A)>, test_variant()::<lambda(C)> >, std::variant<A, B, C>&, 0, 1, 2>((std::make_index_sequence<3>(), std::make_index_sequence<3>()))’
/usr/include/c++/13/variant:1867:26: error: ‘constexpr’ call flows off the end of the function
 1867 |           constexpr bool __visit_rettypes_match = __detail::__variant::
      |                          ^~~~~~~~~~~~~~~~~~~~~~
/usr/include/c++/13/variant:1872:29: error: non-constant condition for static assertion
 1872 |               static_assert(__visit_rettypes_match,
      |                             ^~~~~~~~~~~~~~~~~~~~~~
/usr/include/c++/13/variant:1875:15: error: return-statement with no value, in function returning ‘std::__detail::__variant::__visit_result_t<adt::overloaded<test_variant()::<lambda(A)>, test_variant()::<lambda(C)> >, std::variant<A, B, C>&>’ {aka ‘char’} [-fpermissive]
 1875 |               return;
      |               ^~~~~~
/usr/include/c++/13/variant: In instantiation of ‘static constexpr decltype(auto) std::__detail::__variant::__gen_vtable_impl<std::__detail::__variant::_Multi_array<_Result_type (*)(_Visitor, _Variants ...)>, std::integer_sequence<long unsigned int, __indices ...> >::__visit_invoke(_Visitor&&, _Variants ...) [with _Result_type = std::__detail::__variant::__deduce_visit_result<char>; _Visitor = adt::overloaded<test_variant()::<lambda(A)>, test_variant()::<lambda(C)> >&&; _Variants = {std::variant<A, B, C>&}; long unsigned int ...__indices = {1}]’:
/usr/include/c++/13/variant:1816:5:   required from ‘constexpr decltype(auto) std::__do_visit(_Visitor&&, _Variants&& ...) [with _Result_type = __detail::__variant::__deduce_visit_result<char>; _Visitor = adt::overloaded<test_variant()::<lambda(A)>, test_variant()::<lambda(C)> >; _Variants = {variant<A, B, C>&}]’
/usr/include/c++/13/variant:1878:34:   required from ‘constexpr std::__detail::__variant::__visit_result_t<_Visitor, _Variants ...> std::visit(_Visitor&&, _Variants&& ...) [with _Visitor = adt::overloaded<test_variant()::<lambda(A)>, test_variant()::<lambda(C)> >; _Variants = {variant<A, B, C>&}; __detail::__variant::__visit_result_t<_Visitor, _Variants ...> = char]’
../inc/inspect.hh:233:24:   required from ‘constexpr auto adt::Inspect(Variant&&, Lambdas&& ...) [with R = char; Variant = std::variant<A, B, C>&; Lambdas = {test_variant()::<lambda(A)>, test_variant()::<lambda(C)>}; typename std::enable_if<traits::is_variant<Variant>::value, int>::type <anonymous> = 0]’
../src/main.cpp:65:34:   required from here
/usr/include/c++/13/variant:1060:31: error: no matching function for call to ‘__invoke(adt::overloaded<test_variant()::<lambda(A)>, test_variant()::<lambda(C)> >, B&)’
 1060 |           return std::__invoke(std::forward<_Visitor>(__visitor),
      |                  ~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 1061 |               __element_by_index_or_cookie<__indices>(
      |               ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 1062 |                 std::forward<_Variants>(__vars))...);
      |                 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
In file included from /usr/include/c++/13/bits/refwrap.h:38,
                 from /usr/include/c++/13/string:52,
                 from /usr/include/c++/13/bits/locale_classes.h:40,
                 from /usr/include/c++/13/bits/ios_base.h:41,
                 from /usr/include/c++/13/ios:44:
/usr/include/c++/13/bits/invoke.h:90:5: note: candidate: ‘template<class _Callable, class ... _Args> constexpr typename std::__invoke_result<_Functor, _ArgTypes>::type std::__invoke(_Callable&&, _Args&& ...)’
   90 |     __invoke(_Callable&& __fn, _Args&&... __args)
      |     ^~~~~~~~
/usr/include/c++/13/bits/invoke.h:90:5: note:   template argument deduction/substitution failed:
/usr/include/c++/13/bits/invoke.h: In substitution of ‘template<class _Callable, class ... _Args> constexpr typename std::__invoke_result<_Functor, _ArgTypes>::type std::__invoke(_Callable&&, _Args&& ...) [with _Callable = adt::overloaded<test_variant()::<lambda(A)>, test_variant()::<lambda(C)> >; _Args = {B&}]’:
/usr/include/c++/13/variant:1060:24:   required from ‘static constexpr decltype(auto) std::__detail::__variant::__gen_vtable_impl<std::__detail::__variant::_Multi_array<_Result_type (*)(_Visitor, _Variants ...)>, std::integer_sequence<long unsigned int, __indices ...> >::__visit_invoke(_Visitor&&, _Variants ...) [with _Result_type = std::__detail::__variant::__deduce_visit_result<char>; _Visitor = adt::overloaded<test_variant()::<lambda(A)>, test_variant()::<lambda(C)> >&&; _Variants = {std::variant<A, B, C>&}; long unsigned int ...__indices = {1}]’
/usr/include/c++/13/variant:1816:5:   required from ‘constexpr decltype(auto) std::__do_visit(_Visitor&&, _Variants&& ...) [with _Result_type = __detail::__variant::__deduce_visit_result<char>; _Visitor = adt::overloaded<test_variant()::<lambda(A)>, test_variant()::<lambda(C)> >; _Variants = {variant<A, B, C>&}]’
/usr/include/c++/13/variant:1878:34:   required from ‘constexpr std::__detail::__variant::__visit_result_t<_Visitor, _Variants ...> std::visit(_Visitor&&, _Variants&& ...) [with _Visitor = adt::overloaded<test_variant()::<lambda(A)>, test_variant()::<lambda(C)> >; _Variants = {variant<A, B, C>&}; __detail::__variant::__visit_result_t<_Visitor, _Variants ...> = char]’
../inc/inspect.hh:233:24:   required from ‘constexpr auto adt::Inspect(Variant&&, Lambdas&& ...) [with R = char; Variant = std::variant<A, B, C>&; Lambdas = {test_variant()::<lambda(A)>, test_variant()::<lambda(C)>}; typename std::enable_if<traits::is_variant<Variant>::value, int>::type <anonymous> = 0]’
../src/main.cpp:65:34:   required from here
/usr/include/c++/13/bits/invoke.h:90:5: error: no type named ‘type’ in ‘struct std::__invoke_result<adt::overloaded<test_variant()::<lambda(A)>, test_variant()::<lambda(C)> >, B&>’
ninja: build stopped: subcommand failed.
error: Recipe `build` failed on line 9 with exit code 1
```
</details>


<details>
  <summary>Click to expand compilation logs with prior checks</summary>
  
```
In file included from /usr/include/c++/13/bits/move.h:37,
                 from /usr/include/c++/13/bits/exception_ptr.h:41,
                 from /usr/include/c++/13/exception:164,
                 from /usr/include/c++/13/ios:41,
                 from /usr/include/c++/13/ostream:40,
                 from /usr/include/c++/13/iostream:41,
                 from ../src/main.cpp:21:
/usr/include/c++/13/type_traits: In substitution of ‘template<class _Fn, class ... _Args> using std::invoke_result_t = typename std::invoke_result::type [with _Fn = adt::overloaded<test_variant()::<lambda(A)>, test_variant()::<lambda(C)> >; _Args = {B&}]’:
/usr/include/c++/13/variant:1131:14:   required from ‘constexpr bool std::__detail::__variant::__check_visitor_results(std::index_sequence<_Idx ...>) [with _Visitor = adt::overloaded<test_variant()::<lambda(A)>, test_variant()::<lambda(C)> >; _Variant = std::variant<A, B, C>&; long unsigned int ..._Idxs = {0, 1, 2}; std::index_sequence<_Idx ...> = std::integer_sequence<long unsigned int, 0, 1, 2>]’
/usr/include/c++/13/variant:1868:44:   required from ‘constexpr std::__detail::__variant::__visit_result_t<_Visitor, _Variants ...> std::visit(_Visitor&&, _Variants&& ...) [with _Visitor = adt::overloaded<test_variant()::<lambda(A)>, test_variant()::<lambda(C)> >; _Variants = {variant<A, B, C>&}; __detail::__variant::__visit_result_t<_Visitor, _Variants ...> = char]’
../inc/inspect.hh:233:24:   required from ‘constexpr auto adt::Inspect(Variant&&, Lambdas&& ...) [with R = char; Variant = std::variant<A, B, C>&; Lambdas = {test_variant()::<lambda(A)>, test_variant()::<lambda(C)>}; typename std::enable_if<traits::is_variant<Variant>::value, int>::type <anonymous> = 0]’
../src/main.cpp:65:34:   required from here
/usr/include/c++/13/type_traits:3073:11: error: no type named ‘type’ in ‘struct std::invoke_result<adt::overloaded<test_variant()::<lambda(A)>, test_variant()::<lambda(C)> >, B&>’
 3073 |     using invoke_result_t = typename invoke_result<_Fn, _Args...>::type;
      |           ^~~~~~~~~~~~~~~
In file included from ../inc/inspect.hh:18,
                 from ../src/main.cpp:23:
/usr/include/c++/13/variant: In instantiation of ‘constexpr std::__detail::__variant::__visit_result_t<_Visitor, _Variants ...> std::visit(_Visitor&&, _Variants&& ...) [with _Visitor = adt::overloaded<test_variant()::<lambda(A)>, test_variant()::<lambda(C)> >; _Variants = {variant<A, B, C>&}; __detail::__variant::__visit_result_t<_Visitor, _Variants ...> = char]’:
../inc/inspect.hh:233:24:   required from ‘constexpr auto adt::Inspect(Variant&&, Lambdas&& ...) [with R = char; Variant = std::variant<A, B, C>&; Lambdas = {test_variant()::<lambda(A)>, test_variant()::<lambda(C)>}; typename std::enable_if<traits::is_variant<Variant>::value, int>::type <anonymous> = 0]’
../src/main.cpp:65:34:   required from here
/usr/include/c++/13/variant:1868:44:   in ‘constexpr’ expansion of ‘std::__detail::__variant::__check_visitor_results<adt::overloaded<test_variant()::<lambda(A)>, test_variant()::<lambda(C)> >, std::variant<A, B, C>&, 0, 1, 2>((std::make_index_sequence<3>(), std::make_index_sequence<3>()))’
/usr/include/c++/13/variant:1867:26: error: ‘constexpr’ call flows off the end of the function
 1867 |           constexpr bool __visit_rettypes_match = __detail::__variant::
      |                          ^~~~~~~~~~~~~~~~~~~~~~
/usr/include/c++/13/variant:1872:29: error: non-constant condition for static assertion
 1872 |               static_assert(__visit_rettypes_match,
      |                             ^~~~~~~~~~~~~~~~~~~~~~
/usr/include/c++/13/variant:1875:15: error: return-statement with no value, in function returning ‘std::__detail::__variant::__visit_result_t<adt::overloaded<test_variant()::<lambda(A)>, test_variant()::<lambda(C)> >, std::variant<A, B, C>&>’ {aka ‘char’} [-fpermissive]
 1875 |               return;
      |               ^~~~~~
/usr/include/c++/13/variant: In instantiation of ‘static constexpr decltype(auto) std::__detail::__variant::__gen_vtable_impl<std::__detail::__variant::_Multi_array<_Result_type (*)(_Visitor, _Variants ...)>, std::integer_sequence<long unsigned int, __indices ...> >::__visit_invoke(_Visitor&&, _Variants ...) [with _Result_type = std::__detail::__variant::__deduce_visit_result<char>; _Visitor = adt::overloaded<test_variant()::<lambda(A)>, test_variant()::<lambda(C)> >&&; _Variants = {std::variant<A, B, C>&}; long unsigned int ...__indices = {1}]’:
/usr/include/c++/13/variant:1816:5:   required from ‘constexpr decltype(auto) std::__do_visit(_Visitor&&, _Variants&& ...) [with _Result_type = __detail::__variant::__deduce_visit_result<char>; _Visitor = adt::overloaded<test_variant()::<lambda(A)>, test_variant()::<lambda(C)> >; _Variants = {variant<A, B, C>&}]’
/usr/include/c++/13/variant:1878:34:   required from ‘constexpr std::__detail::__variant::__visit_result_t<_Visitor, _Variants ...> std::visit(_Visitor&&, _Variants&& ...) [with _Visitor = adt::overloaded<test_variant()::<lambda(A)>, test_variant()::<lambda(C)> >; _Variants = {variant<A, B, C>&}; __detail::__variant::__visit_result_t<_Visitor, _Variants ...> = char]’
../inc/inspect.hh:233:24:   required from ‘constexpr auto adt::Inspect(Variant&&, Lambdas&& ...) [with R = char; Variant = std::variant<A, B, C>&; Lambdas = {test_variant()::<lambda(A)>, test_variant()::<lambda(C)>}; typename std::enable_if<traits::is_variant<Variant>::value, int>::type <anonymous> = 0]’
../src/main.cpp:65:34:   required from here
/usr/include/c++/13/variant:1060:31: error: no matching function for call to ‘__invoke(adt::overloaded<test_variant()::<lambda(A)>, test_variant()::<lambda(C)> >, B&)’
 1060 |           return std::__invoke(std::forward<_Visitor>(__visitor),
      |                  ~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 1061 |               __element_by_index_or_cookie<__indices>(
      |               ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 1062 |                 std::forward<_Variants>(__vars))...);
      |                 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
In file included from /usr/include/c++/13/bits/refwrap.h:38,
                 from /usr/include/c++/13/string:52,
                 from /usr/include/c++/13/bits/locale_classes.h:40,
                 from /usr/include/c++/13/bits/ios_base.h:41,
                 from /usr/include/c++/13/ios:44:
/usr/include/c++/13/bits/invoke.h:90:5: note: candidate: ‘template<class _Callable, class ... _Args> constexpr typename std::__invoke_result<_Functor, _ArgTypes>::type std::__invoke(_Callable&&, _Args&& ...)’
   90 |     __invoke(_Callable&& __fn, _Args&&... __args)
      |     ^~~~~~~~
/usr/include/c++/13/bits/invoke.h:90:5: note:   template argument deduction/substitution failed:
/usr/include/c++/13/bits/invoke.h: In substitution of ‘template<class _Callable, class ... _Args> constexpr typename std::__invoke_result<_Functor, _ArgTypes>::type std::__invoke(_Callable&&, _Args&& ...) [with _Callable = adt::overloaded<test_variant()::<lambda(A)>, test_variant()::<lambda(C)> >; _Args = {B&}]’:
/usr/include/c++/13/variant:1060:24:   required from ‘static constexpr decltype(auto) std::__detail::__variant::__gen_vtable_impl<std::__detail::__variant::_Multi_array<_Result_type (*)(_Visitor, _Variants ...)>, std::integer_sequence<long unsigned int, __indices ...> >::__visit_invoke(_Visitor&&, _Variants ...) [with _Result_type = std::__detail::__variant::__deduce_visit_result<char>; _Visitor = adt::overloaded<test_variant()::<lambda(A)>, test_variant()::<lambda(C)> >&&; _Variants = {std::variant<A, B, C>&}; long unsigned int ...__indices = {1}]’
/usr/include/c++/13/variant:1816:5:   required from ‘constexpr decltype(auto) std::__do_visit(_Visitor&&, _Variants&& ...) [with _Result_type = __detail::__variant::__deduce_visit_result<char>; _Visitor = adt::overloaded<test_variant()::<lambda(A)>, test_variant()::<lambda(C)> >; _Variants = {variant<A, B, C>&}]’
/usr/include/c++/13/variant:1878:34:   required from ‘constexpr std::__detail::__variant::__visit_result_t<_Visitor, _Variants ...> std::visit(_Visitor&&, _Variants&& ...) [with _Visitor = adt::overloaded<test_variant()::<lambda(A)>, test_variant()::<lambda(C)> >; _Variants = {variant<A, B, C>&}; __detail::__variant::__visit_result_t<_Visitor, _Variants ...> = char]’
../inc/inspect.hh:233:24:   required from ‘constexpr auto adt::Inspect(Variant&&, Lambdas&& ...) [with R = char; Variant = std::variant<A, B, C>&; Lambdas = {test_variant()::<lambda(A)>, test_variant()::<lambda(C)>}; typename std::enable_if<traits::is_variant<Variant>::value, int>::type <anonymous> = 0]’
../src/main.cpp:65:34:   required from here
/usr/include/c++/13/bits/invoke.h:90:5: error: no type named ‘type’ in ‘struct std::__invoke_result<adt::overloaded<test_variant()::<lambda(A)>, test_variant()::<lambda(C)> >, B&>’
../inc/inspect.hh: In instantiation of ‘static constexpr void adt::diagnostic::variant_validator<Visitor, Variant>::validate_alternative() [with long unsigned int I = 1; Visitor = adt::overloaded<test_variant()::<lambda(A)>, test_variant()::<lambda(C)> >; Variant = std::variant<A, B, C>&]’:
../inc/inspect.hh:66:30:   required from ‘static constexpr void adt::diagnostic::variant_validator<Visitor, Variant>::validate_all(std::index_sequence<_Ind ...>) [with long unsigned int ...Is = {0, 1, 2}; Visitor = adt::overloaded<test_variant()::<lambda(A)>, test_variant()::<lambda(C)> >; Variant = std::variant<A, B, C>&; std::index_sequence<_Ind ...> = std::integer_sequence<long unsigned int, 0, 1, 2>]’
../inc/inspect.hh:70:17:   required from ‘static constexpr void adt::diagnostic::variant_validator<Visitor, Variant>::validate() [with Visitor = adt::overloaded<test_variant()::<lambda(A)>, test_variant()::<lambda(C)> >; Variant = std::variant<A, B, C>&]’
../inc/inspect.hh:225:67:   required from ‘constexpr auto adt::Inspect(Variant&&, Lambdas&& ...) [with R = char; Variant = std::variant<A, B, C>&; Lambdas = {test_variant()::<lambda(A)>, test_variant()::<lambda(C)>}; typename std::enable_if<traits::is_variant<Variant>::value, int>::type <anonymous> = 0]’
../src/main.cpp:65:34:   required from here
../inc/inspect.hh:59:60: error: static assertion failed: ❌ INSPECT ERROR: you did not provide a handler for type ->
   59 |           MISSING_HANDLER_FOR_TYPE<std::decay_t<ArgType>>::value,
      |                                                            ^~~~~
../inc/inspect.hh:59:60: note: ‘adt::diagnostic::MISSING_HANDLER_FOR_TYPE<B>::value’ evaluates to false
ninja: build stopped: subcommand failed.
```
</details>

The difference is this simple log, that greatly reduces effort of searching for the cause, a short, explicit message:
```
../inc/inspect.hh:225:67:   required from ‘constexpr auto adt::Inspect(Variant&&, Lambdas&& ...) [with R = char; Variant = std::variant<A, B, C>&; Lambdas = {test_variant()::<lambda(A)>, test_variant()::<lambda(C)>}; typename std::enable_if<traits::is_variant<Variant>::value, int>::type <anonymous> = 0]’
../src/main.cpp:65:34:   required from here
../inc/inspect.hh:59:60: error: static assertion failed: ❌ INSPECT ERROR: you did not provide a handler for type ->
   59 |           MISSING_HANDLER_FOR_TYPE<std::decay_t<ArgType>>::value,
      |                                                            ^~~~~
../inc/inspect.hh:59:60: note: ‘adt::diagnostic::MISSING_HANDLER_FOR_TYPE<B>::value’ evaluates to false
ninja: build stopped: subcommand failed.
```

### Further examples

Another important class that might need to be analysed in similar fashion is `std::optional`. Here the task is simpler -- you only need to handle two cases. An example below:
```c++
void test_optional() {

  std::cout << "Testing Optional Inspect:" << std::endl;
  std::optional<int> my_opt = 42;

  adt::Inspect(
      my_opt, [](int value) { std::cout << "Value: " << value << std::endl; },
      []() { std::cout << "No Value" << std::endl; });

  my_opt = std::nullopt;
  std::cout << "Optional contains: "
            << adt::Inspect<std::string>(
                   my_opt,
                   [](int value) { return "Value: " + std::to_string(value); },
                   []() { return "No Value"; })
            << std::endl;
}
```

The C++17 standard library does not have more algebraic datatypes in standard. C++23 added `std::expected`, but it is relatively new feature that will not be used by many contemporary codebases as it will take few more years to adopt the newer standard. So this library provides conceptually similar solution inspired by Rust's `Result`, it is called `adt::Result`. It is slightly more verbose when it comes to specifying if a argument is an error or correct value than `std::expected`. A usage example was provided below:

```c++
void test_result() {
  std::cout << "Testing Result Inspect:" << std::endl;

  adt::Result<int, ErrorCode> my_result = adt::Ok(100);
  std::cout << "Result contains: "
            << adt::Inspect<std::string>(
                   my_result,
                   [](int value) { return "Value: " + std::to_string(value); },
                   [](ErrorCode err) {
                     return "Error: " + std::to_string(static_cast<int>(err));
                   })
            << std::endl;

  my_result = adt::Error(ErrorCode::ERROR_TWO);
  std::cout << "Result contains: ";
  adt::Inspect(
      my_result,
      [](int value) { std::cout << "Value: " << value << std::endl; },
      [](ErrorCode err) {
        std::cout << "Error: " << static_cast<int>(err) << std::endl;
      });

  // Test with same type for Value and Error, now handling Ok<T> and Error<E>
  // directly in order to avoid ambiguity
  adt::Result<int, int> another_result = adt::Ok(55);
  std::cout << "Another Result contains: "
            << adt::Inspect<std::string>(
                   another_result,
                   [](adt::Ok<int> value) {
                     return "Value: " + std::to_string(value.get());
                   },
                   [](adt::Error<int> err) {
                     return "Error: " + std::to_string(err.get());
                   })
            << std::endl;
}

```

## Building and Running the example

Two build methods are provided for this example.

### _Meson_ Configuration

You must have _Meson_ and _just_ (for the Justfile) installed. To run, execute:
```bash
just
```

That's it.

### _Makefile_ Configuration

This project is simple, so creating a _makefile_ was not much trouble. An _LLVM_ compiler configuration was chosen because it is slightly more portable than _GCC_. Therefore, you need _make_ and _clang++_ for the compilation itself.

**NOTE**: By modifying the `Makefile` and uncommenting `# CXX=g++`, you can change the compiler to GCC.

To run the program, execute the command:
```bash
make
```
That's it.