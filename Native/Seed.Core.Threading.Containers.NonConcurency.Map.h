#pragma once
#include "Seed.Core.Threading.Containers.NonConcurency.Base.h"

namespace Seed::Core
{
#pragma region ForEach Pair Async
	template<typename THeap, typename TKey, typename TValue>
	template<typename Lambda>
	inline Threading::BaseTask<void, void> Containers::BaseMap<THeap, TKey, TValue>::ForEachImplWithIndexAsync(Node* node, SizeT& Index, Lambda& lambda)
	{
		if (node->left != nullptr)
			co_await ForEachImplWithIndexAsync(node->left, Index, lambda);

		co_await lambda(node->data, Index); Index++;

		if (node->right != nullptr)
			co_await ForEachImplWithIndexAsync(node->right, Index, lambda);
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename Lambda>
	inline Threading::BaseTask<void, void> Containers::BaseMap<THeap, TKey, TValue>::ForEachImplWithoutIndexAsync(Node* node, Lambda& lambda)
	{
		if (node->left != nullptr)
			co_await ForEachImplWithoutIndexAsync(node->left, lambda);

		co_await lambda(node->data);

		if (node->right != nullptr)
			co_await ForEachImplWithoutIndexAsync(node->right, lambda);
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename TLambda>
	inline Threading::BaseTask<void, void> Containers::BaseMap<THeap, TKey, TValue>::ForEachAsyncImpl(TLambda&& lambda) const
	{
		if constexpr (Concepts::IsAsyncInvocable<TLambda, const BasePair<THeap, TKey, TValue>&>)
		{
			auto node = this->root;
			if (node != nullptr)
			{
				co_await ForEachImplWithoutIndexAsync(node, lambda);
			}
		}
		else if constexpr (Concepts::IsAsyncInvocable<TLambda, const BasePair<THeap, TKey, TValue>&, SizeT>)
		{
			auto node = this->root;
			if (node != nullptr)
			{
				SizeT index = 0;
				co_await ForEachImplWithIndexAsync(node, index, lambda);
			}
		}
		else
		{
			static_assert(Concepts::IsAsyncInvocable<TLambda, const BasePair<THeap, TKey, TValue>&> ||
				Concepts::IsAsyncInvocable<TLambda, const BasePair<THeap, TKey, TValue>&, SizeT>);
		}
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename TLambda>
	inline Threading::BaseTask<void, void> Containers::BaseMap<THeap, TKey, TValue>::ForEachAsyncImpl(TLambda&& lambda)
	{
		if constexpr (Concepts::IsAsyncInvocable<TLambda, BasePair<THeap, TKey, TValue>&> ||
				Concepts::IsAsyncInvocable<TLambda, const BasePair<THeap, TKey, TValue>&>) 
		{
			auto node = this->root;
			if (node != nullptr)
			{
				co_await ForEachImplWithoutIndexAsync(node, lambda);
			}
		}
		else if constexpr (Concepts::IsAsyncInvocable<TLambda, BasePair<THeap, TKey, TValue>&, SizeT> ||
			Concepts::IsAsyncInvocable<TLambda, const BasePair<THeap, TKey, TValue>&, SizeT>) 
		{
			auto node = this->root;
			if (node != nullptr)
			{
				SizeT index = 0;
				co_await ForEachImplWithIndexAsync(node, index, lambda);
			}
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

#pragma region For Each Key Async
	template<typename THeap, typename TKey, typename TValue>
	template<typename Lambda>
	inline Threading::BaseTask<void, void> Containers::BaseMap<THeap, TKey, TValue>::ForEachKeyImplWithIndexAsync(Node* node, SizeT& Index, Lambda& lambda)
	{
		if (node->left != nullptr)
			co_await ForEachKeyImplWithIndexAsync(node->left, Index, lambda);

		co_await lambda(node->data.Key, Index); Index++;

		if (node->right != nullptr)
			co_await ForEachKeyImplWithIndexAsync(node->right, Index, lambda);
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename Lambda>
	inline Threading::BaseTask<void, void> Containers::BaseMap<THeap, TKey, TValue>::ForEachKeyImplWithoutIndexAsync(Node* node, Lambda& lambda)
	{
		if (node->left != nullptr)
			co_await ForEachKeyImplWithoutIndexAsync(node->left, lambda);

		co_await lambda(node->data.Key);

		if (node->right != nullptr)
			co_await ForEachKeyImplWithoutIndexAsync(node->right, lambda);
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename TLambda>
	inline Threading::BaseTask<void, void> Containers::BaseMap<THeap, TKey, TValue>::ForEachKeyAsyncImpl(TLambda&& lambda)
	{
		if constexpr (Concepts::IsAsyncInvocable<TLambda, const TKey&>)
		{
			auto node = this->root;
			if (node != nullptr)
			{
				co_await ForEachKeyImplWithoutIndexAsync(node, lambda);
			}
		}
		else if constexpr (Concepts::IsAsyncInvocable<TLambda, const TKey&, SizeT>)
		{
			auto node = this->root;
			if (node != nullptr)
			{
				SizeT index = 0;
				co_await ForEachKeyImplWithoutIndexAsync(node, index, lambda);
			}
		}
		else
		{
			static_assert(Concepts::IsAsyncInvocable<TLambda, const TKey&> ||
				Concepts::IsAsyncInvocable<TLambda, const TKey&, SizeT>);
		}
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename TLambda>
	inline Threading::BaseTask<void, void> Containers::BaseMap<THeap, TKey, TValue>::ForEachKeyAsyncImpl(TLambda&& lambda) const
	{
		if constexpr (Concepts::IsAsyncInvocable<TLambda, const TKey&>)
		{
			auto node = this->root;
			if (node != nullptr)
			{
				co_await ForEachKeyImplWithoutIndexAsync(node, lambda);
			}
		}
		else if constexpr (Concepts::IsAsyncInvocable<TLambda, const TKey&, SizeT>)
		{
			auto node = this->root;
			if (node != nullptr)
			{
				SizeT index = 0;
				co_await ForEachKeyImplWithIndexAsync(node, index, lambda);
			}
		}
		else
		{
			static_assert(Concepts::IsAsyncInvocable<TLambda, const TKey&> ||
				Concepts::IsAsyncInvocable<TLambda, const TKey&, SizeT>);
		}
	}
#pragma endregion

#pragma region For Each Value Async
	template<typename THeap, typename TKey, typename TValue>
	template<typename Lambda>
	inline Threading::BaseTask<void, void> Containers::BaseMap<THeap, TKey, TValue>::ForEachValueImplWithIndexAsync(Node* node, SizeT& Index, Lambda& lambda)
	{
		if (node->left != nullptr)
			co_await ForEachValueImplWithIndexAsync(node->left, Index, lambda);

		co_await lambda(node->data.Value, Index); Index++;

		if (node->right != nullptr)
			co_await ForEachValueImplWithIndexAsync(node->right, Index, lambda);
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename Lambda>
	inline Threading::BaseTask<void, void> Containers::BaseMap<THeap, TKey, TValue>::ForEachValueImplWithoutIndexAsync(Node* node, Lambda& lambda)
	{
		if (node->left != nullptr)
			co_await ForEachValueImplWithoutIndexAsync(node->left, lambda);

		co_await lambda(node->data.Value);

		if (node->right != nullptr)
			co_await ForEachValueImplWithoutIndexAsync(node->right, lambda);
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename TLambda>
	inline Threading::BaseTask<void, void> Containers::BaseMap<THeap, TKey, TValue>::ForEachValueAsyncImpl(TLambda&& lambda) const
	{
		if constexpr (Concepts::IsAsyncInvocable<TLambda, const TValue&>)
		{
			auto node = this->root;
			if (node != nullptr)
			{
				co_await ForEachValueImplWithoutIndexAsync(node, lambda);
			}
		}
		else if constexpr (Concepts::IsAsyncInvocable<TLambda, const TValue&, SizeT>)
		{
			auto node = this->root;
			if (node != nullptr)
			{
				SizeT index = 0;
				co_await ForEachValueImplWithIndexAsync(node, index, lambda);
			}
		}
		else
		{
			static_assert(Concepts::IsAsyncInvocable<TLambda, const TValue&> ||
				Concepts::IsAsyncInvocable<TLambda, const TValue&, SizeT>);
		}
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename TLambda>
	inline Threading::BaseTask<void, void> Containers::BaseMap<THeap, TKey, TValue>::ForEachValueAsyncImpl(TLambda&& lambda)
	{
		if constexpr (Concepts::IsAsyncInvocable<TLambda, TValue&> ||
				Concepts::IsAsyncInvocable<TLambda, const TValue&>)
		{
			auto node = this->root;
			if (node != nullptr)
			{
				co_await ForEachValueImplWithoutIndexAsync(node, lambda);
			}
		}
		else if constexpr (Concepts::IsAsyncInvocable<TLambda, TValue&, SizeT> ||
			Concepts::IsAsyncInvocable<TLambda, const TValue&, SizeT>)
		{
			auto node = this->root;
			if (node != nullptr)
			{
				SizeT index = 0;
				co_await ForEachValueImplWithIndexAsync(node, index, lambda);
			}
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
	inline Threading::BaseTask<typename Containers::BaseMap<THeap, TKey, TValue>::Node*, void> Containers::BaseMap<THeap, TKey, TValue>::DestroyTreeWithLambdaPairValuesAsync(Containers::BaseMap<THeap, TKey, TValue>::Node* root, Lambda& lambda)
	{
		if (root == nullptr)
			co_return nullptr;

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
	inline Threading::BaseTask<void, void> Containers::BaseMap<THeap, TKey, TValue>::RemoveAllWithAsyncImpl(Lambda&& lambda)
	{
		this->root = co_await DestroyTreeWithLambdaPairValuesAsync(this->root, lambda);
		this->count = 0;
		co_return;
	}
#pragma endregion
}