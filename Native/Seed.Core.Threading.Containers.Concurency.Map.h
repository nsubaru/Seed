#pragma once
#include "Seed.Core.Threading.Containers.Concurency.Base.h"

namespace Seed::Core
{
#pragma region For Each Async Pair
	template<typename THeap, typename TKey, typename TValue>
	template<typename Lambda>
	inline Seed::Core::Threading::BaseTask<void, void> Seed::Core::Containers::BaseConcurencyMap<THeap, TKey, TValue>::ForEachAsyncImplWithIndex(Node* node, SizeT& Index, Lambda& lambda)
	{
		if (node->left != nullptr)
			co_await ForEachAsyncImplWithIndex(node->left, Index, lambda);

		co_await lambda(node->data, Index); Index++;

		if (node->right != nullptr)
			co_await ForEachAsyncImplWithIndex(node->right, Index, lambda);
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename Lambda>
	inline Seed::Core::Threading::BaseTask<void, void> Seed::Core::Containers::BaseConcurencyMap<THeap, TKey, TValue>::ForEachAsyncImplWithoutIndex(Node* node, Lambda& lambda)
	{
		if (node->left != nullptr)
			co_await ForEachAsyncImplWithoutIndex(node->left, lambda);

		co_await lambda(node->data);

		if (node->right != nullptr)
			co_await ForEachAsyncImplWithoutIndex(node->right, lambda);
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename TLambda>
	inline Seed::Core::Threading::Task<void> Seed::Core::Containers::BaseConcurencyMap<THeap, TKey, TValue>::ForEachAsyncImpl(TLambda&& lambda) const
	{
		if constexpr (Concepts::IsAsyncInvocable<TLambda, const BasePair<THeap, TKey, TValue>&>)
		{
			return this->lock.SharedLockAsync([&]() -> Seed::Core::Threading::Task<void> InlineLambda {
				auto node = this->root;

				if (node != nullptr)
				{
					co_await ForEachAsyncImplWithoutIndex(node, lambda);
				}
			});
		}
		else if constexpr (Concepts::IsAsyncInvocable<TLambda, const BasePair<THeap, TKey, TValue>&, SizeT>)
		{
			return this->lock.SharedLockAsync([&]() -> Seed::Core::Threading::Task<void> InlineLambda {
				auto node = this->root;

				if (node != nullptr)
				{
					SizeT index = 0;
					co_await ForEachAsyncImplWithIndex(node, index, lambda);
				}
			});
		}
		else
		{
			static_assert(Concepts::IsAsyncInvocable<TLambda, const BasePair<THeap, TKey, TValue>&> ||
				Concepts::IsAsyncInvocable<TLambda, const BasePair<THeap, TKey, TValue>&, SizeT>);
		}
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename TLambda>
	inline Seed::Core::Threading::Task<void> Seed::Core::Containers::BaseConcurencyMap<THeap, TKey, TValue>::ForEachAsyncImpl(TLambda&& lambda)
	{
		if constexpr (Concepts::IsAsyncInvocable<TLambda, BasePair<THeap, TKey, TValue>&> ||
			Concepts::IsAsyncInvocable<TLambda, const BasePair<THeap, TKey, TValue>&>)
		{ 
			return this->lock.SharedLockAsync([&]() -> Seed::Core::Threading::Task<void> InlineLambda {
				auto node = this->root;
				if (node != nullptr)
				{
					co_await ForEachAsyncImplWithoutIndex(node, lambda);
				}
			});
		}
		else if constexpr (Concepts::IsAsyncInvocable<TLambda, BasePair<THeap, TKey, TValue>&, SizeT> ||
			Concepts::IsAsyncInvocable<TLambda, const BasePair<THeap, TKey, TValue>&, SizeT>)
		{ 
			return this->lock.SharedLockAsync([&]() -> Seed::Core::Threading::Task<void> InlineLambda {
				auto node = this->root;
				if (node != nullptr)
				{
					SizeT index = 0;
					co_await ForEachAsyncImplWithIndex(node, index, lambda);
				}
			});
		}
		else 
		{
			static_assert(Concepts::IsAsyncInvocable<TLambda, BasePair<THeap, TKey, TValue>&> ||
				Concepts::IsAsyncInvocable<TLambda, const BasePair<THeap, TKey, TValue>&> ||
				Concepts::IsAsyncInvocable<TLambda, BasePair<THeap, TKey, TValue>&, SizeT> ||
				Concepts::IsAsyncInvocable<TLambda, const BasePair<THeap, TKey, TValue>&, SizeT>);
		}
	}
#pragma endregion

#pragma region For Each Async Key
	template<typename THeap, typename TKey, typename TValue>
	template<typename Lambda>
	inline Seed::Core::Threading::BaseTask<void, void> Seed::Core::Containers::BaseConcurencyMap<THeap, TKey, TValue>::ForEachKeyAsyncImplWithIndex(Node* node, SizeT& Index, Lambda& lambda)
	{
		if (node->left != nullptr)
			co_await ForEachKeyAsyncImplWithIndex(node->left, Index, lambda);

		co_await lambda(node->data.Key, Index); Index++;

		if (node->right != nullptr)
			co_await ForEachKeyAsyncImplWithIndex(node->right, Index, lambda);
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename Lambda>
	inline Seed::Core::Threading::BaseTask<void, void> Seed::Core::Containers::BaseConcurencyMap<THeap, TKey, TValue>::ForEachKeyAsyncImplWithoutIndex(Node* node, Lambda& lambda)
	{
		if (node->left != nullptr)
			co_await ForEachKeyAsyncImplWithoutIndex(node->left, lambda);

		co_await lambda(node->data.Key);

		if (node->right != nullptr)
			co_await ForEachKeyAsyncImplWithoutIndex(node->right, lambda);
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename TLambda>
	inline Seed::Core::Threading::BaseTask<void, void> Seed::Core::Containers::BaseConcurencyMap<THeap, TKey, TValue>::ForEachKeyAsyncImpl(TLambda&& lambda)
	{
		if constexpr (Concepts::IsAsyncInvocable<TLambda, const TKey&>)
		{
			return this->lock.SharedLockAsync([&]() -> Seed::Core::Threading::BaseTask<void, void> InlineLambda{
				auto node = this->root;

				if (node != nullptr)
				{
					co_await ForEachKeyAsyncImplWithoutIndex(node, lambda);
				}
			});
		}
		else if constexpr (Concepts::IsAsyncInvocable<TLambda, const TKey&, SizeT>)
		{
			return this->lock.SharedLockAsync([&]() -> Seed::Core::Threading::BaseTask<void, void> InlineLambda{
				auto node = this->root;

				if (node != nullptr)
				{
					SizeT index = 0;
					co_await ForEachKeyAsyncImplWithIndex(node, index, lambda);
				}
			});
		}
		else
		{
			static_assert(Concepts::IsAsyncInvocable<TLambda, const TKey&> ||
				Concepts::IsAsyncInvocable<TLambda, const TKey&, SizeT>);
		}
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename TLambda>
	inline Seed::Core::Threading::BaseTask<void, void> Seed::Core::Containers::BaseConcurencyMap<THeap, TKey, TValue>::ForEachKeyAsyncImpl(TLambda&& lambda) const
	{
		if constexpr (Concepts::IsAsyncInvocable<TLambda, const TKey&>)
		{
			return this->lock.SharedLockAsync([&]() -> Seed::Core::Threading::BaseTask<void, void> InlineLambda{
				auto node = this->root;

				if (node != nullptr)
				{
					co_await ForEachKeyAsyncImplWithoutIndex(node, lambda);
				}
			});
		}
		else if constexpr (Concepts::IsAsyncInvocable<TLambda, const TKey&, SizeT>)
		{
			return this->lock.SharedLockAsync([&]() -> Seed::Core::Threading::BaseTask<void, void> InlineLambda{
				auto node = this->root;

				if (node != nullptr)
				{
					SizeT index = 0;
					co_await ForEachKeyAsyncImplWithoutIndex(node, index, lambda);
				}
			});
		}
		else
		{
			static_assert(Concepts::IsAsyncInvocable<TLambda, const TKey&> ||
				Concepts::IsAsyncInvocable<TLambda, const TKey&, SizeT>);
		}
	}
#pragma endregion

#pragma region For Each Async Value
	template<typename THeap, typename TKey, typename TValue>
	template<typename Lambda>
	inline Seed::Core::Threading::BaseTask<void, void> Seed::Core::Containers::BaseConcurencyMap<THeap, TKey, TValue>::ForEachValueAsyncImplWithIndex(Node* node, SizeT& Index, Lambda& lambda)
	{
		if (node->left != nullptr)
			co_await ForEachValueAsyncImplWithIndex(node->left, Index, lambda);

		co_await lambda(node->data.Value, Index); Index++;

		if (node->right != nullptr)
			co_await ForEachValueAsyncImplWithIndex(node->right, Index, lambda);
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename Lambda>
	inline Seed::Core::Threading::BaseTask<void, void> Seed::Core::Containers::BaseConcurencyMap<THeap, TKey, TValue>::ForEachValueAsyncImplWithoutIndex(Node* node, Lambda& lambda)
	{
		if (node->left != nullptr)
			co_await ForEachValueAsyncImplWithoutIndex(node->left, lambda);

		co_await lambda(node->data.Value);

		if (node->right != nullptr)
			co_await ForEachValueAsyncImplWithoutIndex(node->right, lambda);
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename TLambda>
	inline Seed::Core::Threading::BaseTask<void, void> Seed::Core::Containers::BaseConcurencyMap<THeap, TKey, TValue>::ForEachValueAsyncImpl(TLambda&& lambda) const
	{
		if constexpr (Concepts::IsAsyncInvocable<TLambda, const TValue&>)
		{
			return this->lock.SharedLockAsync([&]() -> Seed::Core::Threading::BaseTask<void, void> InlineLambda{
				auto node = this->root;

				if (node != nullptr)
				{
					co_await ForEachValueAsyncImplWithoutIndex(node, lambda);
				}
			});
		}
		else if constexpr (Concepts::IsAsyncInvocable<TLambda, const TValue&, SizeT>)
		{
			return this->lock.SharedLockAsync([&]() -> Seed::Core::Threading::BaseTask<void, void> InlineLambda{
				auto node = this->root;

				if (node != nullptr)
				{
					SizeT index = 0;
					co_await ForEachValueAsyncImplWithIndex(node, index, lambda);
				}
			});
		}
		else
		{
			static_assert(Concepts::IsAsyncInvocable<TLambda, const TValue&> ||
				Concepts::IsAsyncInvocable<TLambda, const TValue&, SizeT>);
		}
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename TLambda>
	inline Seed::Core::Threading::BaseTask<void, void> Seed::Core::Containers::BaseConcurencyMap<THeap, TKey, TValue>::ForEachValueAsyncImpl(TLambda&& lambda)
	{
		if constexpr (Concepts::IsAsyncInvocable<TLambda, TValue&> ||
					Concepts::IsAsyncInvocable<TLambda, const TValue&>)
		{
			return this->lock.SharedLockAsync([&]() -> Seed::Core::Threading::BaseTask<void, void> InlineLambda{
				auto node = this->root;

				if (node != nullptr)
				{
					co_await ForEachValueAsyncImplWithoutIndex(node, lambda);
				}
			});
		}
		else if constexpr (Concepts::IsAsyncInvocable<TLambda, TValue&, SizeT> ||
			Concepts::IsAsyncInvocable<TLambda, const TValue&, SizeT>)
		{
			return this->lock.SharedLockAsync([&]() -> Seed::Core::Threading::BaseTask<void, void> InlineLambda{
				auto node = this->root;

				if (node != nullptr)
				{
					SizeT index = 0;
					co_await ForEachValueAsyncImplWithIndex(node, index, lambda);
				}
			});
		}
		else
		{
			static_assert(Concepts::IsAsyncInvocable<TLambda, TValue&> ||
				Concepts::IsAsyncInvocable<TLambda, const TValue&> ||
				Concepts::IsAsyncInvocable<TLambda, TValue&, SizeT> ||
				Concepts::IsAsyncInvocable<TLambda, const TValue&, SizeT>);
		}
	}
#pragma endregion

#pragma region RemoveAll
	template<typename THeap, typename TKey, typename TValue>
	template<typename Lambda>
	inline Threading::BaseTask<typename Containers::BaseConcurencyMap<THeap, TKey, TValue>::Node*, void> Containers::BaseConcurencyMap<THeap, TKey, TValue>::DestroyTreeWithLambdaPairValuesAsync(Containers::BaseConcurencyMap<THeap, TKey, TValue>::Node* root, Lambda& lambda)
	{
		if (root == nullptr)
			return nullptr;

		if (root->left != nullptr)
		{
			root->left = co_await DestroyTreeWithLambdaPairValuesAsync(root->left, lambda);
		}

		co_await lambda(MoveRValue(root->data));

		if (root->right != nullptr)
		{
			root->right = co_await DestroyTreeWithLambdaPairValuesAsync(root->right, lambda);
		}

		if (!root->RefCount.DecrementAndCheckIsZero())
		{
			root->father = root->left = root->right = nullptr;
			root->This = nullptr;
		}
		else
		{
			delete root;
		}

		co_return nullptr;
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename Lambda>
	inline Threading::BaseTask<void, void> Containers::BaseConcurencyMap<THeap, TKey, TValue>::RemoveAllWithAsyncImpl(Lambda&& lambda)
	{
		return this->lock.SharedLock([&]()->Threading::BaseTask<void, void> InlineLambda{
			this->root = co_await DestroyTreeWithLambdaPairValuesAsync(this->root, lambda);
			this->count = 0;
			co_return;
		});
	}
#pragma endregion
}