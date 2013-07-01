/// @file type_algorithms.hpp
/// @author Michael Koch
/// @copyright CC BY 3.0

#ifndef __TAGSOUP_TYPE_ALGORITHMS_HPP__
#define __TAGSOUP_TYPE_ALGORITHMS_HPP__

namespace ts
{

	/// @class _contains_type
	/// @brief auxiliary class contains previous success or failure of finding \a X
	/// @tparam X type to search for
	/// @tparam found indicates whether type has been found so far
	/// @tparam Ts list of type to search
	/// @details branching in template meta programming has to be done via template parameters;
	/// 			here it is the boolean flag which indicates if type \X has been found.
	template <typename X, bool found, typename ... Ts>
	struct _contains_type : std::integral_constant<bool, found>
	{};

	/// @class _contains_type<_, false, _, _ ...>
	/// @brief specialisatin of _contains_type when type has not been found and there is at least one list entry left
	/// @tparam X type to search for
	/// @tparam T first entry in the list
	/// @tparam Ts rest of list (can be empty)
	template <typename X, typename T, typename ... Ts>
	struct _contains_type<X, false, T, Ts ...> : _contains_type<X, std::is_same<X, T>::value, Ts ...>
	{};

	/// @class contains_type
	/// @brief search list of types for one specific type
	/// @tparam X type to search for
	/// @tparam Ts list of types to search
	template <typename X, typename ... Ts>
	struct contains_type;

	/// @class contains_type<_>
	/// @brief specialisiation for empty list of types
	/// @tparam X type to search for
	template <typename X>
	struct contains_type<X> : std::false_type
	{};

	/// @class contains-type<_, _, _ ...>
	/// @brief specialisation for non empty list of types
	/// @tparam X type to search for
	/// @tparam T first entry in list
	/// @tparam Ts rest of list
	template <typename X, typename T, typename ... Ts>
	struct contains_type<X, T, Ts ...> : _contains_type<X, std::is_same<X, T>::value, Ts ...>
	{};




	/// @class _contains_duplicate
	/// @brief 
	template <bool match, typename ... Ts>
	struct _contains_duplicate : std::integral_constant<bool, match>
	{};

	/// @class _contains_duplicate<false, _, _ ...>
	/// @brief specialisation for non empty lists where no duplicate has been found so far
	/// @tparam T first entry in list
	/// @tparam Ts rest of list
	template <typename T, typename ... Ts>
	struct _contains_duplicate<false, T, Ts ...> : _contains_duplicate<contains_type<T, Ts ...>::value, Ts ...>
	{};

	/// @class contains_duplicate
	/// @brief tests whether any type duplicates appear in the given type list
	/// @tparam Ts list of types
	template <typename ... Ts>
	struct contains_duplicate;

	/// @class contains_duplicate<>
	/// @brief specialisation for empty type list
	template <>
	struct contains_duplicate<> : std::false_type
	{};
	
	/// @class contains_duplicate<_, _ ...>
	/// @brief specialisation for non empty list
	/// @tparam T first entry in the type list
	/// @tparam Ts rest of type list
	template <typename T, typename ... Ts>
	struct contains_duplicate<T, Ts ...> : _contains_duplicate<contains_type<T, Ts ...>::value, Ts ...>
	{};

}

#endif

