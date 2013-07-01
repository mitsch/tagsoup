/// @file token.hpp
/// @author Michael Koch
/// @copyright CC BY 3.0

#ifndef __TAGSOUP_TOKEN_HPP__
#define __TAGSOUP_TOKEN_HPP__

#include <tuple>
#include <type_traits>
#include <typeinfo>
#include <cassert>
#include <new>
#include <tagsoup/type_algorithms.hpp>

namespace ts
{

	/// @union _token_values
	/// @tparam T first entry in type list
	/// @tparam Ts rest of type list
	/// @brief holds
	template <typename T, typename ... Ts>
	union _token_values;



	/// @union _token_values<_, _, ...>
	/// @brief specialisation for type list with at least two entries
	/// @tparam T1 first entry in the type list
	/// @tparam T2 second entry in the type list
	/// @tparam Ts rest of type list
	template <typename T1, typename T2, typename ... Ts>
	union _token_values<T1, T2, Ts ...>
	{
		/// instantiation of first type
		T1 value;

		/// rest of type list is instantiated recursively in another _token_values 
		_token_values<T2, Ts ...> values;

		/// @brief constructor with parameter to set one of the inner values
		/// @tparam X type of parameter
		/// @note this constructor is only opted in, if type \a X is contained by the type list (\a T1, \a T2, \a Ts ...)
		/// @param x value to set token
		/// @details This constructor forwards the parameter \a to some auxiliary constructors which helps to find the
		///				correct recursion level to set \a x
		template <typename X, typename = typename std::enable_if<__contains_type<X, T1, T2, Ts ...>::value>::type>
		_token_values(X && x) : _token_values(std::forward<X>(x), std::integral_constant<bool, std::is_same<X, T1>::value>())
		{}

		/// @brief auxiliary constructor for the case that this is not the correct recursion level
		/// @tparam X type of value to set token to
		/// @param x value to set token to
		/// @param right_level indication that we are not on the right level (consider the type!)
		/// @details \a x is forwarded to the next recursion level \a values
		template <typename X>
		_token_values(X && x, const std::false_type right_level) : values(std::forward<X>(x))
		{}

		/// @brief auxiliary constructor for the case that this is the correct recursion level
		/// @tparam X type of value to set token to
		/// @param x value to set token to
		/// @param right_level indication that we are on the right level (consider the type!)
		/// @details \a x is forwarded to \a value
		template <typename X>
		_token_values(X && x, const std::true_type right_level) : value(std::forward<X>(x))
		{}

		/// @brief standard constructor does nothing, but needs implementation since it would be
		///			deleted according to the standard
		_token_values()
		{}

		/// @brief standard destructor does nothing, but needs implementation since it would be
		///			deleted according to the standard
		~_token_values()
		{}

		/// @brief get certain value defined by the type (constant implementation)
		/// @tparam X type of wanted value
		/// @pre type \a X must be contained by type list (\a T1, \a T2, \a Ts ...)
		/// @note there is no guarranty that the wanted value is currently active in this union!
		/// @return constant reference to the wanted value
		template <typename X>
		const X& get() const
		{
			static_assert(contains_type<X, T1, T2, Ts ...>::value, "request type X cannot be found in the type list!");
			return get<X>(std::integral_constant<bool, std::is_same<X, T1>::value>());
		}

		/// @brief auxiliary method to get some specific value defined by its type
		/// @tparam X type of wanted value
		/// @param right_level indicates that this is not the right recursion level (consider the type!)
		/// @return constant reference to the wanted value
		/// @details forward request to next recursion level, that is \a values
		template <typename X>
		const X& get(const std::false_type right_level) const
		{
			return values.template get<X>();
		}

		/// @brief auxiliary method to get some specific value defined by its type
		/// @tparam X type of wanted value
		/// @param right_level indicates that this is the right recursion level (consider the type!)
		/// @return constant reference to the wanted value
		/// @details returns \a value
		template <typename X>
		const X& get(const std::true_type right_level) const	
		{
			return value;
		}

		/// @brief get certain value defined by its type (non-constant implementation)
		/// @tparam X type of wanted value
		/// @pre type \a X must be contained by the type list (\a T1, \a T2, \a Ts ...)
		/// @note there is no guarranty that the wanted value is currently active in this union!
		/// @return reference to the wanted value
		template <typename X>
		X& get()
		{
			static_assert(contains_type<X, T1, T2, Ts ...>::value, "request type X cannot be found in the type list!");
			return get<X>(std::integral_constant<bool, std::is_same<X, T1>::value>());
		}

		/// @brief auxiliary method to get some specific value defined by its type
		/// @tparam X type of wanted value
		/// @param right_level indicates that is not the right recursion level (consider the type!)
		/// @return reference to the wanted value
		/// @details forward request to the next recursion level, that is \a value
		template <typename X>
		X& get(const std::false_type right_level)
		{
			return values.template get<X>();
		}

		/// @brief auxiliary method to get some specific value defined by its type
		/// @tparam X type of wanted value
		/// @param right_level indicates that is the right recursion level (consider the type!)
		/// @return reference to the wanted value
		/// @details returns \a value
		template <typename X>
		X& get(const std::true_type right_level)
		{
			return value;
		}

		/// @brief calls destructor on value instance whose type matches \a info
		/// @param info information of type whose instance should be destructed
		/// @details If type of current \a value matches \info, we call destructor on \a value.
		///				If not, the request is forwarded to the next recursion level \a values.
		void dtr(const std::type_info & info)
		{
			if (info == typeid(T1))	value.~T1();
			else values.dtr(info);
		}

		/// @brief copy specific value defined by info
		/// @param info specifies type of value and hence the value itself we want to copy
		/// @param t values instance we want to copy from
		/// @details If this one is not the right recursion level, we proceed going to the next recursion level.
		/// @note no destructor will be called here, and depending on previously activated value has to be done before.
		void copy(const std::type_info & info, const _token_values & t)
		{
			if (info == typeid(T1)) new (&value) T1(t.value);
			else values.copy(info, t.values);
		}

		/// @brief move specific value defined by info
		/// @param info specifies type of value and hence the value itself we want to move
		/// @param t values instance we want to move from
		/// @details If this one is not the right recursion level, we proceed going to the next recursion level.
		/// @note no destructor will be called here, and depending on previously activated value has to be done before.
		void move(const std::type_info & info, _token_values && t)
		{
			if (info == typeid(T1)) new (&value) T1(std::move(t.value));
			else values.move(info, std::move(t.values));
		}

	};


	/// @union _token_values<_>
	/// @brief specification for type list of size one
	/// @tparam T single type in the list
	template <typename T>
	union _token_values<T>
	{
		/// instantiation of the remaining type
		T value;

		/// @brief argument constructor setting \a value
		/// @param t rvalue of value
		_token_values(T && t) : value(std::move(t))
		{}

		/// @brief argument constructor setting \a value
		/// @param t lvalue of value
		_token_values(const T & t) : value(t)
		{}

		/// @brief destructor does nothing
		~_token_values() {}

		/// @brief gets value
		/// @pre \a X must equal \a T
		/// @tparam X wanted type
		/// @return constant reference to \a value
		template <typename X> const X& get() const
		{
			static_assert(std::is_same<X, T>::value, "could not find appropriate type in the union!");
			return value;
		}
		
		/// @brief gets value
		/// @pre \a X must equal \a T
		/// @tparam X wanted type
		/// @return reference to \a value
		template <typename X>
		X& get()
		{
			static_assert(std::is_same<X, T>::value, "could not find appropriate type in the union!");
			return value;
		}

		/// @brief calls destructor on \a value
		/// @param info type information
		/// @pre type of \a value, \a T, should match \a info
		void dtr(const std::type_info & info)
		{
			assert(typeid(T) == info);
			value.~T();
		}

		/// @brief copy \a value from another instance
		/// @param info type information of instance to copy
		/// @param t another _token_value instance to copy from
		/// @pre type of \a value, \a T, should match \a info
		void copy(const std::type_info & info, const _token_values & t)
		{
			assert(info == typeid(T));
			new (&value) T(t.value);
		}

		/// @brief move \a value from another instance
		/// @param info type information of instance to move
		/// @param t another _token_value instance to move from
		/// @pre type of \a value, \a T, should match \a info
		void move(const std::type_info & info, _token_values && t)
		{
			assert(info == typeid(T));
			new (&value) T(std::move(t.value));
		}
		
	};

	
	/// @struct token_signature
	/// @tparam Ts list of types which defines the signature
	/// @brief keeps information about all types in a token
	/// @note this struct is needed to simplify the deduction of template parameters
	template <typename T, typename ... Ts>
	struct token_signature{};

	/// @class token
	/// @brief can hold values of different types, but only one at a time
	/// @tparam T first possible type
	/// @tparam Ts rest of possible types
	template <typename T, typename ... Ts>
	struct token
	{
		static_assert(!__contains_duplicate<T, Ts ...>::value, "list of template parameter is not free of duplicates!");

		using signature = token_signature<T, Ts ...>;

		/// information about which type is active
		const std::type_info & binded_type;

		/// union of specified types
		_token_values<T, Ts ...> values;

		/// @brief initialise if type is one of \a T or \a Ts
		/// @tparam X type of instance to initialise with
		/// @param x instance to initialise
		template <typename X, typename = typename std::enable_if<contains_type<X, T, Ts ...>::value>::type>
		token(X && x) : binded_type(typeid(X)), values(std::forward<X>(x))
		{}

		/// @brief move constructor
		/// @param t instance to move from
		token(token && t) : binded_type(t.binded_type)
		{
			values.move(t.binded_type, std::move(t.values));
		}

		/// @brief copy constructor
		/// @param t instance to copy from
		token(const token & t) : binded_type(t.binded_type)	
		{
			values.copy(t.binded_type, t.values);
		}
		
		/// @brief destructor destructs active value
		~token()
		{
			values.dtr(binded_type);
		}

		/// @brief copy assignment
		/// @return this reference
		/// @param t instance to copy from
		/// @details currently active value will be destructed and active value of \a t will be copied
		token& operator = (const token & t)
		{
			values.dtr(binded_type);
			binded_type = t.binded_type;
			values.copy(binded_type, t.values);
			return *this;
		}

		/// @brief move assignment
		/// @return this reference
		/// @param t instance to move from
		/// @details currently active value will be destructed and active value of \a t will be moved
		token& operator = (token && t)
		{
			values.dtr(binded_type);
			binded_type = t.binded_type;
			values.move(binded_type, std::move(t.values));
			return *this;
		}

		/// @brief test whether specific \a X is active right now
		/// @tparam X wanted type
		/// @pre \a X must be contained by type list (\a T, \a Ts ...)
		/// @return boolean value whether currently active instance matches type \a X
		template <typename X> bool is_type() const
		{
			static_assert(contains_type<X, T, Ts ...>::value, "type X must be part of the class template type list!");
			return typeid(X) == binded_type;
		}

		/// @brief getter for value of type \a X
		/// @tparam X wanted type
		/// @pre X must be contained by type list (\a T, \a Ts ...)
		/// @pre requested value must be currently active
		/// @return constant reference to wanted value
		template <typename X>
		const X& get() const
		{
			static_assert(contains_type<X, T, Ts ...>::value, "type X must be part of the class template type list!");
			assert(typeid(X) == binded_type);
			return values.template get<X>();
		}

		/// @brief getter for value of type \a X
		/// @tparam X wanted type
		/// @pre X must be contained by type list (\a T, \a Ts ...)
		/// @pre requested value must be currently active
		/// @return reference to wanted value
		template <typename X>
		X& get()
		{
			static_assert(contains_type<X, T, Ts ...>::value, "type X must be part of the class template type list!");
			assert(typeid(X) == binded_type);
			return values.template get<X>();
		}
	};

	/// @brief makes a token
	/// @tparam X specific type to initialise the token with one of its instances
	/// @tparam T first possible type of token
	/// @tparam Ts rest of possible types the token can hold
	/// @param x value to initialise token with
	/// @param signature helps compiler to figure out which types the token can hold
	/// @pre \a X must be contained by type list (\a T, \a Ts ...)
	/// @pre type list (\a T, \a Ts ...) may not have any duplicate
	/// @return token initialised with \a x
	template <typename X, typename T, typename ... Ts>
	token<T, Ts ...> make_token(X && x, const token_signature<T, Ts ...> signature)
	{
		static_assert(contains_type<X, T, Ts ...>::value, "type X is not contained by type list (T, Ts ...)");
		static_assert(!contains_duplicate<T, Ts ...>::value, "type list (T, Ts ...) contains duplicates!");
		return token<T, Ts ...>(std::forward<X>(x));
	}




}

#endif

