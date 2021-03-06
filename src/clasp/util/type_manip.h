// 
// Copyright (c) 2010, Benjamin Kaufmann
// 
// This file is part of Clasp. See http://www.cs.uni-potsdam.de/clasp/ 
// 
// Clasp is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// Clasp is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Clasp; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//
#ifndef BK_LIB_TYPE_MANIP_H_INCLUDED
#define BK_LIB_TYPE_MANIP_H_INCLUDED
namespace bk_lib { namespace detail {
#if (_MSC_VER >= 1300)
#define ALIGNOF(PARAM) (__alignof(PARAM))
#elif defined(__GNUC__)
#define ALIGNOF(PARAM) (__alignof__(PARAM))
#else
template <class T>
struct align_helper { char x; T  y; };
#define ALIGNOF(T) (sizeof(align_helper<T>)-sizeof(T))
#endif


// if b then if_type else else_type
template <bool b, class if_type, class else_type>
struct if_then_else;

template <class if_type, class else_type>
struct if_then_else<true, if_type, else_type>  {  typedef if_type type;  };
template <class if_type, class else_type>
struct if_then_else<false, if_type, else_type> { typedef else_type type; };

// 1 if T == U, else 0
template <class T, class U>  struct same_type        { enum { value = 0 }; };
template <class T>           struct same_type<T,T>   { enum { value = 1 }; };

template <bool>              struct disable_if       { typedef bool type; };
template <>                  struct disable_if<true> {  };

// not in list - marks end of type list
struct nil_type {};

// list of types - terminated by nil_type
template <class head, class tail>
struct type_list {
	typedef head head_type;
	typedef tail tail_type;
};

// generates a type lits with up to 18 elements
template <
	typename T1  = nil_type, typename T2  = nil_type, typename T3  = nil_type,
	typename T4  = nil_type, typename T5  = nil_type, typename T6  = nil_type,
	typename T7  = nil_type, typename T8  = nil_type, typename T9  = nil_type,
	typename T10 = nil_type, typename T11 = nil_type, typename T12 = nil_type,
	typename T13 = nil_type, typename T14 = nil_type, typename T15 = nil_type,
	typename T16 = nil_type, typename T17 = nil_type, typename T18 = nil_type
> 
struct generate_type_list {
	typedef typename generate_type_list<T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18>::type tail_type;
	typedef type_list<T1, tail_type> type;
};

template <> 
struct generate_type_list<> { typedef nil_type type; };

// maps an integer constant to a type
template <int i>
struct int2type { enum { value = i }; };
typedef int2type<0> false_type;
typedef int2type<1> true_type;

// declared but not defined
struct unknown_type;

// finds the element in the type list TList that 
// has the same alignment as X or X if no such element exists
template <class X, class TList>
struct max_align;

// IF ALIGNOF(X) == ALIGNOF(H) then H 
// ELSE max_align<X, Tail>
template <bool, class X, class H, class Tail>
struct max_align_aux;

// Base case: ALIGNOF(X) == ALIGNOF(H)
template <class X, class H, class T>
struct max_align_aux<true, X, H, T> {
	typedef H type;
};

// Recursive case
template <bool, class X, class H, class Tail>
struct max_align_aux {
	typedef typename max_align<X, Tail>::type type;
};

template <class X>
struct max_align<X, nil_type> {
	typedef X type;
};

template <class X, class H, class T>
struct max_align<X, type_list<H, T> > {
private:
	enum { x_align = ALIGNOF(X) };
	enum { h_align = ALIGNOF(H) };
public:
	typedef typename max_align_aux<static_cast<int>(x_align) == static_cast<int>(h_align), X, H, T>::type type;
	enum { value = sizeof(type) };
};

// computes alignment size (::value) and type (::type)  of T
template <class T>
struct align_of {
	typedef generate_type_list<bool, char, short, int, long, float, double, long double, void*,
		unknown_type(*)(unknown_type), unknown_type* unknown_type::*,
		unknown_type (unknown_type::*)(unknown_type)>::type align_list;
	typedef typename max_align<T, align_list>::type type;
	enum { value = max_align<T, align_list>::value };
};

#undef ALIGNOF

}}
#endif

