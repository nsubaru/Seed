#pragma once
#include "Seed.Core.Containers.Lazy.h"

namespace Seed::Core
{
	template<typename THeap, typename T>
	ForceInline void BaseLazy<THeap, T>::CheckIsExistOrGetNew() const
	{
		if (This->value == nullptr)
		{
			This->value = new T(This->getter());
		}
	}

	template<typename THeap, typename T>
	void BaseLazy<THeap, T>::Dispose()
	{
		delete This->value;
	}

	template<typename THeap, typename T>
	ForceInline BaseLazy<THeap, T>::BaseLazy(LazyGetter getter): This(new Data(getter))
	{
		if (getter == nullptr)
		{
			Throw(NullPointException(SourcePosition));
		}
	}

	template<typename THeap, typename T>
	ForceInline BaseLazy<THeap, T>::BaseLazy(const T& val): This(new Data(new T(val)))
	{
	}

	template<typename THeap, typename T>
	ForceInline BaseLazy<THeap, T>::BaseLazy(T&& val): This(new Data(new T(MoveRValue(val))))
	{
	}

	template<typename THeap, typename T>
	template<typename> requires Concepts::IsDefaultConstructible<T>
	ForceInline BaseLazy<THeap, T>::BaseLazy()  //-V730
		: BaseLazy(ArrowFunct(T())) {}

	template<typename THeap, typename T>
	ForceInline T& BaseLazy<THeap, T>::operator*()&
	{
		CheckIsExistOrGetNew();
		return *This->value;
	}

	template<typename THeap, typename T>
	ForceInline const T& BaseLazy<THeap, T>::operator*() const&
	{
		CheckIsExistOrGetNew();
		return *This->value;
	}

	template<typename THeap, typename T>
	ForceInline T* BaseLazy<THeap, T>::operator->()&
	{
		CheckIsExistOrGetNew();
		return This->value;
	}

	template<typename THeap, typename T>
	ForceInline const T* BaseLazy<THeap, T>::operator->() const&
	{
		CheckIsExistOrGetNew();
		return This->value;
	}

	template<typename THeap, typename T>
	ForceInline T&& BaseLazy<THeap, T>::operator*()&&
	{
		CheckIsExistOrGetNew();
		T value = MoveRValue(*This->value);
		delete This->value;
		This->value = nullptr;
		return MoveRValue(value);
	}

	template<typename THeap, typename T>
	ForceInline T*&& BaseLazy<THeap, T>::operator->()&&  //-V659
	{
		CheckIsExistOrGetNew();
		T* value = MoveRValue(This->value);
		This->value = nullptr;
		return MoveRValue(value);
	}
























}