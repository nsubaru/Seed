#pragma once
#include "Seed.Core.Threading.Containers.Concurency.Base.h"

template<class THeap, typename T>
template<typename TLambda>
inline Seed::Core::Threading::BaseTask<Seed::Core::Containers::BaseConcurencyList<THeap, T>, void> Seed::Core::Containers::BaseConcurencyList<THeap, T>::ForEachAsyncImpl(TLambda&& lambda)
{
    if constexpr (TypeTraits::IsInvocable<TLambda, const T&> ||
                            TypeTraits::IsInvocable<TLambda, T&>) 
	{
		return this->lock.SharedLockAsync([&]() -> Threading::BaseTask<BaseConcurencyList<THeap, T>, void> InlineLambda {
			auto node = this->root;
			while (node != nullptr) {
				co_await lambda(node->value);
				node = node->next;
			}
			co_return *this;
		});
	}
	else if constexpr (TypeTraits::IsInvocable<TLambda, const T&, SizeT> ||
                        TypeTraits::IsInvocable<TLambda, T&, SizeT>) 
	{
		return this->lock.SharedLockAsync([&]() -> Threading::BaseTask<BaseConcurencyList<THeap, T>, void> InlineLambda {
			auto node = this->root;
			SizeT index = 0;
			while (node != nullptr) {
				co_await lambda(node->value, index);
				node = node->next;
				index++;
			}
			co_return *this;
		});
	}
	else 
	{
		static_assert(TypeTraits::IsInvocable<TLambda, const T&> ||
			TypeTraits::IsInvocable<TLambda, T&> ||
			TypeTraits::IsInvocable<TLambda, const T&, SizeT> ||
			TypeTraits::IsInvocable<TLambda, T&, SizeT>);
		__assume(0);
	}
}

template<class THeap, typename T>
template<typename TLambda>
inline Seed::Core::Threading::BaseTask<Seed::Core::Containers::BaseConcurencyList<THeap, T>, void> Seed::Core::Containers::BaseConcurencyList<THeap, T>::ForEachAsyncImpl(TLambda&& lambda) const
{
	if constexpr (TypeTraits::IsInvocable<TLambda, const T&>) 
	{
		return this->lock.SharedLock([&]() -> Threading::BaseTask<BaseConcurencyList<THeap, T>, void> InlineLambda {
			auto node = this->root;
			while (node != nullptr) {
				co_await lambda(node->value);
				node = node->next;
			}
			co_return *this;
		});
	}
	else if constexpr (TypeTraits::IsInvocable<TLambda, const T&, SizeT>) 
	{
		return this->lock.SharedLock([&]() -> Threading::BaseTask<BaseConcurencyList<THeap, T>, void> InlineLambda {
			auto node = this->root;
			SizeT index = 0;
			while (node != nullptr) {
				co_await lambda(node->value, index);
				node = node->next;
				index++;
			}
			co_return *this;
		});
	}
	else 
	{
		static_assert(TypeTraits::IsInvocable<TLambda, const T&> ||
			TypeTraits::IsInvocable<TLambda, const T&, SizeT>);
		__assume(0);
	}
}

template<class THeap, typename T>
template<typename TLambda>
inline Seed::Core::Threading::BaseTask<Seed::Core::Containers::BaseConcurencyList<THeap, T>, void> Seed::Core::Containers::BaseConcurencyList<THeap, T>::ForEachAsyncImpl(TLambda&& lambda, SizeT count)
{
	if constexpr (TypeTraits::IsInvocable<TLambda, const T&> ||
                            TypeTraits::IsInvocable<TLambda, T&>) 
	{
		return this->lock.SharedLockAsync([&]() -> Threading::BaseTask<BaseConcurencyList<THeap, T>, void> InlineLambda {
			if (count > this->size)
			{
				auto node = this->root;
				while (node != nullptr) {
					co_await lambda(node->value);
					node = node->next;
				}
				co_return *this;
			}
			else {
				SizeT i = 0;
				auto node = this->root;
				while (node != nullptr) {
					co_await lambda(node->value);
					node = node->next;
					i++;
					if (i == count)
						co_return *this;
				}
				co_return *this;
			}
		});
	}
	else if constexpr (TypeTraits::IsInvocable<TLambda, const T&, SizeT> ||
                        TypeTraits::IsInvocable<TLambda, T&, SizeT>) 
	{
		return this->lock.SharedLockAsync([&]() -> Threading::BaseTask<BaseConcurencyList<THeap, T>, void> InlineLambda {
			if (count > this->size)
			{
				auto node = this->root;
				SizeT index = 0;
				while (node != nullptr) {
					co_await lambda(node->value, index);
					node = node->next;
					index++;
				}
				co_return *this;
			}
			else {
				SizeT index = 0;
				auto node = this->root;
				while (node != nullptr) {
					co_await lambda(node->value, index);
					node = node->next;
					index++;
					if (index == count)
						co_return *this;
				}
				co_return *this;
			}
		});
	}
	else 
	{
		static_assert(TypeTraits::IsInvocable<TLambda, const T&> ||
			TypeTraits::IsInvocable<TLambda, T&> ||
			TypeTraits::IsInvocable<TLambda, const T&, SizeT> ||
			TypeTraits::IsInvocable<TLambda, T&, SizeT>);
		__assume(0);
	}
}

template<class THeap, typename T>
template<typename TLambda>
inline Seed::Core::Threading::BaseTask<Seed::Core::Containers::BaseConcurencyList<THeap, T>, void> Seed::Core::Containers::BaseConcurencyList<THeap, T>::ForEachAsyncImpl(TLambda&& lambda, SizeT count) const
{
	if constexpr (TypeTraits::IsInvocable<TLambda, const T&>) 
	{
		return this->lock.SharedLockAsync([&]() -> const BaseConcurencyList& InlineLambda {
			if (count > this->size)
			{
				auto node = this->root;
				while (node != nullptr) {
					co_await lambda(node->value);
					node = node->next;
				}
				co_return *this;
			}
			else {
				SizeT i = 0;
				auto node = this->root;
				while (node != nullptr) {
					co_await lambda(node->value);
					node = node->next;
					i++;
					if (i == count)
						co_return *this;
				}
				co_return *this;
			}
		});
	}
	else if constexpr (TypeTraits::IsInvocable<TLambda, const T&, SizeT>) 
	{
		return this->lock.SharedLockAsync([&]() -> const BaseConcurencyList& InlineLambda {
			if (count > this->size)
			{
				auto node = this->root;
				SizeT index = 0;
				while (node != nullptr) {
					co_await lambda(node->value, index);
					node = node->next;
					index++;
				}
				co_return *this;
			}
			else {
				SizeT index = 0;
				auto node = this->root;
				while (node != nullptr) {
					co_await lambda(node->value, index);
					node = node->next;
					index++;
					if (index == count)
						co_return *this;
				}
				co_return *this;
			}
		});
	}
	else 
	{
		static_assert(TypeTraits::IsInvocable<TLambda, const T&> ||
			TypeTraits::IsInvocable<TLambda, const T&, SizeT>);
		__assume(0);
	}
}

template<class THeap, typename T>
template<typename TLambda>
inline Seed::Core::Threading::BaseTask<void, void> Seed::Core::Containers::BaseConcurencyList<THeap, T>::RemoveAllAsyncImpl(TLambda&& lambda)
{
	return this->lock.ExclusiveLockAsync([&]() -> Threading::BaseTask<void, void> InlineLambda
	{
		SizeT idx = 0;
		while (this->root != nullptr)
		{
			elem* node = this->root;
			this->root = this->root->next;
			node->next = nullptr;
			node->father = nullptr;
			node->prev = nullptr;

			if (Concepts::IsAsyncInvocable<TLambda, T&&, SizeT>)
			{
				co_await lambda(MoveRValue(node->value), idx);
			}
			else
			{
				co_await lambda(MoveRValue(node->value));
			}

			if (--node->RefCount == 0)
			{
				delete node;
				continue;
			}

			idx++;
		}

		this->root = this->last = nullptr;
		this->size = 0;
		return *this;
	});
}