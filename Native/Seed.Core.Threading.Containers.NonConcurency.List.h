#pragma once
#include "Seed.Core.Threading.Containers.NonConcurency.Base.h"

namespace Seed::Core
{
#pragma region ForEachAsyncImpl
	template<class THeap, typename T>
	template<typename TLambda>
	inline Threading::Task<void> Containers::BaseList<THeap, T>::ForEachAsyncImpl(TLambda&& lambda)
	{
		if constexpr (Concepts::IsAsyncInvocable<TLambda, const T&> ||
				Concepts::IsAsyncInvocable<TLambda, T&>)
		{
			auto node = this->root;
			while (node != nullptr)
			{
				co_await lambda(node->value);
				node = node->next;
			}
			co_return;
		}
		else if constexpr (Concepts::IsAsyncInvocable<TLambda, const T&, SizeT> ||
							Concepts::IsAsyncInvocable<TLambda, T&, SizeT>)
		{
			auto node = this->root;
			SizeT index = 0;
			while (node != nullptr)
			{
				co_await lambda(node->value, index);
				node = node->next;
				index++;
			}
			co_return;
		}
		else
		{
			static_assert(
				Concepts::IsAsyncInvocable<TLambda, const T&> ||
				Concepts::IsAsyncInvocable<TLambda, T&> ||
				Concepts::IsAsyncInvocable<TLambda, const T&, SizeT> ||
				Concepts::IsAsyncInvocable<TLambda, T&, SizeT>);
		}
	}

	template<class THeap, typename T>
	template<typename TLambda>
	inline Threading::Task<void> Containers::BaseList<THeap, T>::ForEachAsyncImpl(TLambda&& lambda) const
	{
		if constexpr (Concepts::IsAsyncInvocable<TLambda, const T&>)
		{
			auto node = this->root;
			while (node != nullptr)
			{
				co_await lambda(node->value);
				node = node->next;
			}
			co_return;
		}
		else if constexpr (Concepts::IsAsyncInvocable<TLambda, const T&, SizeT>)
		{
			auto node = this->root;
			SizeT index = 0;
			while (node != nullptr)
			{
				co_await lambda(node->value, index);
				node = node->next;
				index++;
			}
			co_return;
		}
		else
		{
			static_assert(
				Concepts::IsAsyncInvocable<TLambda, const T&> ||
				Concepts::IsAsyncInvocable<TLambda, const T&, SizeT>
				);
		}
	}

	template<class THeap, typename T>
	template<typename TLambda>
	inline Threading::Task<void> Containers::BaseList<THeap, T>::ForEachAsyncImpl(TLambda&& lambda, SizeT count)
	{
		if constexpr (Concepts::IsAsyncInvocable<TLambda, const T&> ||
									Concepts::IsAsyncInvocable<TLambda, T&>)
		{
			if (count > this->size)
			{
				auto node = this->root;
				while (node != nullptr)
				{
					co_await lambda(node->value);
					node = node->next;
				}
				co_return;
			}
			else
			{
				SizeT i = 0;
				auto node = this->root;
				while (node != nullptr)
				{
					co_await lambda(node->value);
					node = node->next;
					i++;
					if (i == count)
						co_return;
				}
				co_return;
			}
		}
		else if constexpr (Concepts::IsAsyncInvocable<TLambda, const T&, SizeT> ||
							Concepts::IsAsyncInvocable<TLambda, T&, SizeT>)
		{
			if (count > this->size)
			{
				auto node = this->root;
				SizeT index = 0;
				while (node != nullptr)
				{
					co_await lambda(node->value, index);
					node = node->next;
					index++;
				}
				co_return;
			}
			else
			{
				SizeT index = 0;
				auto node = this->root;
				while (node != nullptr)
				{
					co_await lambda(node->value, index);
					node = node->next;
					index++;
					if (index == count)
						co_return;
				}
				co_return;
			}
		}
		else
		{
			static_assert(
				Concepts::IsAsyncInvocable<TLambda, const T&> ||
				Concepts::IsAsyncInvocable<TLambda, T&> ||
				Concepts::IsAsyncInvocable<TLambda, const T&, SizeT> ||
				Concepts::IsAsyncInvocable<TLambda, T&, SizeT>);
		}
	}

	template<class THeap, typename T>
	template<typename TLambda>
	inline Threading::Task<void> Containers::BaseList<THeap, T>::ForEachAsyncImpl(TLambda&& lambda, SizeT count) const
	{
		if constexpr (Concepts::IsAsyncInvocable<TLambda, const T&>)
		{
			if (count > this->size)
			{
				auto node = this->root;
				while (node != nullptr)
				{
					co_await lambda(node->value);
					node = node->next;
				}
				co_return;
			}
			else
			{
				SizeT i = 0;
				auto node = this->root;
				while (node != nullptr)
				{
					co_await lambda(node->value);
					node = node->next;
					i++;
					if (i == count)
						co_return;
				}
				co_return;
			}
		}
		else if constexpr (Concepts::IsAsyncInvocable<TLambda, const T&, SizeT>)
		{
			if (count > this->size)
			{
				auto node = this->root;
				SizeT index = 0;
				while (node != nullptr)
				{
					co_await lambda(node->value, index);
					node = node->next;
					index++;
				}
				co_return;
			}
			else
			{
				SizeT index = 0;
				auto node = this->root;
				while (node != nullptr)
				{
					co_await lambda(node->value, index);
					node = node->next;
					index++;
					if (index == count)
						co_return;
				}
				co_return;
			}
		}
		else
		{
			static_assert(
				Concepts::IsAsyncInvocable<TLambda, const T&> ||
				Concepts::IsAsyncInvocable<TLambda, const T&, SizeT>
				);
		}
	}
#pragma endregion

#pragma region RemoveAll
	template<class THeap, typename T>
	template<typename TLambda>
	inline Threading::BaseTask<void, void> Containers::BaseList<THeap, T>::RemoveAllAsyncImpl(TLambda&& lambda)
	{
		SizeT idx = 0;

		elem* node;
		while (this->root != nullptr)
		{
			node = this->root;
			this->root = root->next;
			node->next = nullptr;
			node->father = nullptr;
			node->prev = nullptr;

			if constexpr (Concepts::IsAsyncInvocable<TLambda, T&&, SizeT>)
			{
				co_await lambda(MoveRValue(node->value), idx); idx++;
			}
			else
			{
				co_await lambda(MoveRValue(node->value));
			}

			if (--node->RefCount == 0)
			{
				delete node;
			}

			--this->size;
		}

		this->root = this->last = nullptr;
		this->size = 0;
	}
#pragma endregion
}