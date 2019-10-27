#pragma once
#include "Seed.Core.Base.h"

namespace Seed {
	inline namespace Core {
		namespace TypeTraits {
			namespace Collection {
				template<typename T>
				struct IsArrayT : LogicFalse {};

				template<typename T>
				constexpr bool IsArray = IsArrayT<T>::State;

				template<typename T>
				struct IsVListT : LogicFalse {};

				template<typename T>
				constexpr bool IsVList = IsVListT<T>::State;

				template<typename T>
				struct IsDictionaryT : LogicFalse {};

				template<typename T>
				constexpr bool IsDictionary = IsDictionaryT<T>::State;

				template<typename T>
				struct IsDynamicQueueT : LogicFalse {};

				template<typename T>
				constexpr bool IsDynamicQueue = IsDynamicQueueT<T>::State;

				template<typename T>
				struct IsDynamicStackT : LogicFalse {};

				template<typename T>
				constexpr bool IsDynamicStack = IsDynamicStackT<T>::State;

				template<typename T>
				struct IsFixedArrayT : LogicFalse {};
				
				template<typename T>
				constexpr bool IsFixedArray = IsFixedArrayT<T>::State;

				template<typename T>
				struct IsFixedQueueT : LogicFalse {};

				template<typename T>
				constexpr bool IsFixedQueue = IsFixedQueueT<T>::State;

				template<typename T>
				struct IsFixedStackT : LogicFalse {};

				template<typename T>
				constexpr bool IsFixedStack = IsFixedStackT<T>::State;

				template<typename T>
				struct IsListT : LogicFalse {};

				template<typename T>
				constexpr bool IsList = IsListT<T>::State;

				template<typename T>
				struct IsMapT : LogicFalse {};

				template<typename T>
				constexpr bool IsMap = IsMapT<T>::State;

				template<typename T>
				struct IsPriorityQueueT : LogicFalse {};

				template<typename T>
				constexpr bool IsPriorityQueue = IsPriorityQueueT<T>::State;

				template<typename T>
				struct IsPairT : LogicFalse {};

				template<typename T>
				constexpr bool IsPair = IsPairT<T>::State;

				template<typename T>
				struct IsStringT : LogicFalse {};

				template<typename T>
				constexpr bool IsString = IsStringT<T>::State;
			}

			template<typename T>
			struct IsCollectionT : LogicConst<
				Collection::IsArray<T> ||
				Collection::IsVList<T> ||
				Collection::IsDictionary<T> ||
				Collection::IsDynamicQueue<T> ||
				Collection::IsDynamicStack<T> ||
				Collection::IsFixedArray<T> ||
				Collection::IsFixedQueue<T> ||
				Collection::IsFixedStack<T> ||
				Collection::IsList<T> ||
				Collection::IsMap<T> ||
				Collection::IsPriorityQueue<T>
			> {};

			template<typename T>
			constexpr Boolean IsCollection = IsCollectionT<T>::State;
		}
	}
}