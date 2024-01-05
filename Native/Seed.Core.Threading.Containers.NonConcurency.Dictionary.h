#pragma once
#pragma once
#include "Seed.Core.Threading.Containers.NonConcurency.Base.h"

namespace Seed::Core
{
#pragma region For Each Async
	template<typename THeap, typename TString, typename TData, Containers::TCharacterComparator CharacterComparator>
	template<typename Lambda>
	inline static Threading::BaseTask<void, void> Containers::BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachAsyncImplWithoutIndex(NodeSym* node, Lambda& lambda)
	{
		if (node->left != nullptr) return ForEachAsyncImplWithoutIndex(node->left, lambda);
		if (node->data != nullptr) return ForEachAsyncImplWithoutIndex(node->data, lambda);
		if (node->right != nullptr) return ForEachAsyncImplWithoutIndex(node->right, lambda);
	}

	template<typename THeap, typename TString, typename TData, Containers::TCharacterComparator CharacterComparator>
	template<typename Lambda>
	inline static Threading::BaseTask<void, void> Containers::BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachAsyncImplWithoutIndex(NodeLex* node, Lambda& lambda)
	{
		if (node->data != nullptr)
			co_await lambda(*node->data);

		if (node->node_root != nullptr)
			co_return ForEachAsyncImplWithoutIndex(node->node_root, lambda);
	}

	template<typename THeap, typename TString, typename TData, Containers::TCharacterComparator CharacterComparator>
	template<typename TLambda>
	inline static Threading::BaseTask<void, void> Containers::BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachAsyncImplWithoutIndex(const BaseDictionary* const This, TLambda& lambda)
	{
		if (This != nullptr)
			if (This->node != nullptr)
				return ForEachAsyncImplWithoutIndex(This->node, lambda);
	}

	template<typename THeap, typename TString, typename TData, Containers::TCharacterComparator CharacterComparator>
	template<typename Lambda>
	inline static Threading::BaseTask<void, void> Containers::BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachAsyncImplWithIndex(NodeSym* node, Lambda& lambda, SizeT& Index)
	{
		if (node->left != nullptr) return ForEachAsyncImplWithIndex(node->left, lambda, Index);
		if (node->data != nullptr) return ForEachAsyncImplWithIndex(node->data, lambda, Index);
		if (node->right != nullptr) return ForEachAsyncImplWithIndex(node->right, lambda, Index);
	}

	template<typename THeap, typename TString, typename TData, Containers::TCharacterComparator CharacterComparator>
	template<typename Lambda>
	inline static Threading::BaseTask<void, void> Containers::BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachAsyncImplWithIndex(NodeLex* node, Lambda& lambda, SizeT& Index)
	{
		if (node->data != nullptr)
		{
			co_await lambda(*node->data, Index);
			Index++;
		}

		if (node->node_root != nullptr)
			co_return ForEachAsyncImplWithIndex(node->node_root, lambda, Index);
	}

	template<typename THeap, typename TString, typename TData, Containers::TCharacterComparator CharacterComparator>
	template<typename TLambda>
	inline static Threading::BaseTask<void, void> Containers::BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachAsyncImplWithIndex(const BaseDictionary* const This, TLambda& lambda, SizeT& Index)
	{
		if (This != nullptr)
			if (This->node != nullptr)
				return ForEachAsyncImplWithIndex(This->node, lambda, Index);
	}

	template<typename THeap, typename TString, typename TData, Containers::TCharacterComparator CharacterComparator>
	template<typename LambdaT>
	inline Threading::BaseTask<void, void> Containers::BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachAsyncImpl(LambdaT&& lambda) const
	{
		if constexpr (Concepts::IsAsyncInvocable<LambdaT, const BasePair<THeap, TString, TData>&>)
		{
			if (this->node != nullptr)
				co_await ForEachAsyncImplWithoutIndex(this->node, lambda);

			co_return;
		}
		else if constexpr (Concepts::IsAsyncInvocable<LambdaT, const BasePair<THeap, TString, TData>&, SizeT>)
		{
			SizeT index = 0;
			if (this->node != nullptr)
				co_await ForEachAsyncImplWithIndex(this->node, lambda, index);

			co_return;
		}
		else
		{
			static_assert(Concepts::IsAsyncInvocable<LambdaT, const BasePair<THeap, TString, TData>&> ||
				Concepts::IsAsyncInvocable<LambdaT, const BasePair<THeap, TString, TData>&, SizeT>);
		}
	}

	template<typename THeap, typename TString, typename TData, Containers::TCharacterComparator CharacterComparator>
	template<typename LambdaT>
	inline Threading::BaseTask<void, void> Containers::BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachAsyncImpl(LambdaT&& lambda)
	{
		if constexpr (Concepts::IsAsyncInvocable<LambdaT, BasePair<THeap, TString, TData>&> ||
			Concepts::IsAsyncInvocable<LambdaT, const BasePair<THeap, TString, TData>&>)
		{
			if (this->node != nullptr)
				co_await ForEachAsyncImplWithoutIndex(this->node, lambda);

			co_return;
		}
		else if constexpr (Concepts::IsAsyncInvocable<LambdaT, BasePair<THeap, TString, TData>&, SizeT> ||
			Concepts::IsAsyncInvocable<LambdaT, const BasePair<THeap, TString, TData>&, SizeT>)
		{
			SizeT index = 0;
			if (this->node != nullptr)
				co_await ForEachAsyncImplWithIndex(this->node, lambda, index);

			co_return;
		}
		else
		{
			static_assert(Concepts::IsAsyncInvocable<LambdaT, BasePair<THeap, TString, TData>&> ||
				Concepts::IsAsyncInvocable<LambdaT, const BasePair<THeap, TString, TData>&> ||
				Concepts::IsAsyncInvocable<LambdaT, BasePair<THeap, TString, TData>&, SizeT> ||
				Concepts::IsAsyncInvocable<LambdaT, const BasePair<THeap, TString, TData>&, SizeT>);
		}
	}
#pragma endregion

#pragma region For Each Key Async
	template<typename THeap, typename TString, typename TData, Containers::TCharacterComparator CharacterComparator>
	template<typename Lambda>
	inline static Threading::BaseTask<void, void> Containers::BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachAsyncKeyImplWithoutIndex(NodeSym* node, Lambda& lambda)
	{
		if (node->left != nullptr) return ForEachAsyncKeyImplWithoutIndex(node->left, lambda);
		if (node->data != nullptr) return ForEachAsyncKeyImplWithoutIndex(node->data, lambda);
		if (node->right != nullptr) return ForEachAsyncKeyImplWithoutIndex(node->right, lambda);
	}

	template<typename THeap, typename TString, typename TData, Containers::TCharacterComparator CharacterComparator>
	template<typename Lambda>
	inline static Threading::BaseTask<void, void> Containers::BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachAsyncKeyImplWithoutIndex(NodeLex* node, Lambda& lambda)
	{
		if (node->data != nullptr)
			co_await lambda(node->data->Key);

		if (node->node_root != nullptr)
			co_return ForEachAsyncKeyImplWithoutIndex(node->node_root, lambda);
	}

	template<typename THeap, typename TString, typename TData, Containers::TCharacterComparator CharacterComparator>
	template<typename TLambda>
	inline static Threading::BaseTask<void, void> Containers::BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachAsyncKeyImplWithoutIndex(const BaseDictionary* const This, TLambda& lambda)
	{
		if (This != nullptr)
			if (This->node != nullptr)
				return ForEachAsyncKeyImplWithoutIndex(This->node, lambda);
	}

	template<typename THeap, typename TString, typename TData, Containers::TCharacterComparator CharacterComparator>
	template<typename Lambda>
	inline static Threading::BaseTask<void, void> Containers::BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachAsyncKeyImplWithIndex(NodeSym* node, Lambda& lambda, SizeT& Index)
	{
		if (node->left != nullptr) return ForEachAsyncKeyImplWithIndex(node->left, lambda, Index);
		if (node->data != nullptr) return ForEachAsyncKeyImplWithIndex(node->data, lambda, Index);
		if (node->right != nullptr) return  ForEachAsyncKeyImplWithIndex(node->right, lambda, Index);
	}

	template<typename THeap, typename TString, typename TData, Containers::TCharacterComparator CharacterComparator>
	template<typename Lambda>
	inline static Threading::BaseTask<void, void> Containers::BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachAsyncKeyImplWithIndex(NodeLex* node, Lambda& lambda, SizeT& Index)
	{
		if (node->data != nullptr)
		{
			co_await lambda(node->data->Key, Index);
			Index++;
		}

		if (node->node_root != nullptr)
			co_return ForEachAsyncKeyImplWithIndex(node->node_root, lambda, Index);
	}

	template<typename THeap, typename TString, typename TData, Containers::TCharacterComparator CharacterComparator>
	template<typename TLambda>
	inline static Threading::BaseTask<void, void> Containers::BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachAsyncKeyImplWithIndex(const BaseDictionary* const This, TLambda& lambda, SizeT& Index)
	{
		if (This != nullptr)
			if (This->node != nullptr)
				return ForEachAsyncKeyImplWithIndex(This->node, lambda, Index);
	}

	template<typename THeap, typename TString, typename TData, Containers::TCharacterComparator CharacterComparator>
	template<typename LambdaT>
	inline Threading::BaseTask<void, void> Containers::BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachAsyncKeyImpl(LambdaT&& lambda) const
	{
		if constexpr (Concepts::IsAsyncInvocable<LambdaT, const TString&>)
		{
			if (this->node != nullptr)
				co_await ForEachAsyncKeyImplWithoutIndex(this->node, lambda);
			
			co_return;
		}
		else if constexpr (Concepts::IsAsyncInvocable<LambdaT, const TString&, SizeT>)
		{
			SizeT index = 0;
			if (this->node != nullptr)
				co_await ForEachAsyncKeyImplWithIndex(this->node, lambda, index);

			co_return;
		}
		else
		{
			static_assert(Concepts::IsAsyncInvocable<LambdaT, const TString&> ||
				Concepts::IsAsyncInvocable<LambdaT, const TString&, SizeT>);
		}
	}

	template<typename THeap, typename TString, typename TData, Containers::TCharacterComparator CharacterComparator>
	template<typename LambdaT>
	inline Threading::BaseTask<void, void> Containers::BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachAsyncKeyImpl(LambdaT&& lambda)
	{
		if constexpr (Concepts::IsAsyncInvocable<LambdaT, const TString&>)
		{
			if (this->node != nullptr)
				co_await ForEachAsyncKeyImplWithoutIndex(this->node, lambda);

			co_return;
		}
		else if constexpr (Concepts::IsAsyncInvocable<LambdaT, const TString&, SizeT>)
		{
			SizeT index = 0;
			if (this->node != nullptr)
				co_await ForEachAsyncKeyImplWithIndex(this->node, lambda, index);
			
			co_return;
		}
		else
		{
			static_assert(Concepts::IsAsyncInvocable<LambdaT, const TString&> ||
				Concepts::IsAsyncInvocable<LambdaT, const TString&, SizeT>);
		}
	}
#pragma endregion

#pragma region For Each Value Async
	template<typename THeap, typename TString, typename TData, Containers::TCharacterComparator CharacterComparator>
	template<typename Lambda>
	inline static Threading::BaseTask<void, void> Containers::BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachAsyncValueImplWithoutIndex(NodeSym* node, Lambda& lambda)
	{
		if (node->left != nullptr) return ForEachAsyncValueImplWithoutIndex(node->left, lambda);
		if (node->data != nullptr) return ForEachAsyncValueImplWithoutIndex(node->data, lambda);
		if (node->right != nullptr) return ForEachAsyncValueImplWithoutIndex(node->right, lambda);
	}

	template<typename THeap, typename TString, typename TData, Containers::TCharacterComparator CharacterComparator>
	template<typename Lambda>
	inline static Threading::BaseTask<void, void> Containers::BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachAsyncValueImplWithoutIndex(NodeLex* node, Lambda& lambda)
	{
		if (node->data != nullptr)
			co_await lambda(node->data->Value);

		if (node->node_root != nullptr)
			co_return ForEachAsyncValueImplWithoutIndex(node->node_root, lambda);
	}

	template<typename THeap, typename TString, typename TData, Containers::TCharacterComparator CharacterComparator>
	template<typename TLambda>
	inline static Threading::BaseTask<void, void> Containers::BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachAsyncValueImplWithoutIndex(const BaseDictionary* const This, TLambda& lambda)
	{
		if (This != nullptr)
			if (This->node != nullptr)
				return ForEachAsyncValueImplWithoutIndex(This->node, lambda);
	}

	template<typename THeap, typename TString, typename TData, Containers::TCharacterComparator CharacterComparator>
	template<typename Lambda>
	inline static Threading::BaseTask<void, void> Containers::BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachAsyncValueImplWithIndex(NodeSym* node, Lambda& lambda, SizeT& Index)
	{
		if (node->left != nullptr) return ForEachAsyncValueImplWithIndex(node->left, lambda, Index);
		if (node->data != nullptr) return ForEachAsyncValueImplWithIndex(node->data, lambda, Index);
		if (node->right != nullptr) return ForEachAsyncValueImplWithIndex(node->right, lambda, Index);
	}

	template<typename THeap, typename TString, typename TData, Containers::TCharacterComparator CharacterComparator>
	template<typename Lambda>
	inline static Threading::BaseTask<void, void> Containers::BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachAsyncValueImplWithIndex(NodeLex* node, Lambda& lambda, SizeT& Index)
	{
		if (node->data != nullptr)
		{
			co_await lambda(node->data->Value, Index);
			Index++;
		}
		if (node->node_root != nullptr)
			co_return ForEachAsynchValueImplWithIndex(node->node_root, lambda, Index);
	}

	template<typename THeap, typename TString, typename TData, Containers::TCharacterComparator CharacterComparator>
	template<typename TLambda>
	inline static Threading::BaseTask<void, void> Containers::BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachAsyncValueImplWithIndex(const BaseDictionary* const This, TLambda& lambda, SizeT& index)
	{
		if (This != nullptr)
			if (This->node != nullptr)
				return ForEachAsyncValueImplWithIndex(This->node, lambda, index);
	}

	template<typename THeap, typename TString, typename TData, Containers::TCharacterComparator CharacterComparator>
	template<typename LambdaT>
	inline Threading::BaseTask<void, void> Containers::BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachAsyncValueImpl(LambdaT&& lambda) const
	{
		if constexpr (Concepts::IsAsyncInvocable<LambdaT, const TData&>)
		{
			if (this->node != nullptr)
				co_await ForEachAsyncValueImplWithoutIndex(this->node, lambda);

			co_return;
		}
		else if constexpr (Concepts::IsAsyncInvocable<LambdaT, const TData&, SizeT>)
		{
			SizeT index = 0;
			if (this->node != nullptr)
				co_await ForEachAsyncValueImplWithIndex(this->node, lambda, index);
			
			co_return;
		}
		else
		{
			static_assert(Concepts::IsAsyncInvocable<LambdaT, const TData&> ||
				Concepts::IsAsyncInvocable<LambdaT, const TData&, SizeT>);
		}
	}

	template<typename THeap, typename TString, typename TData, Containers::TCharacterComparator CharacterComparator>
	template<typename LambdaT>
	inline Threading::BaseTask<void, void> Containers::BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachAsyncValueImpl(LambdaT&& lambda)
	{
		if constexpr (Concepts::IsAsyncInvocable<LambdaT, TData&> ||
			Concepts::IsAsyncInvocable<LambdaT, const TData&>)
		{
			if (this->node != nullptr)
				co_await ForEachAsyncValueImplWithoutIndex(this->node, lambda);

			co_return;
		}
		else if constexpr (Concepts::IsAsyncInvocable<LambdaT, TData&, SizeT> ||
			Concepts::IsAsyncInvocable<LambdaT, const TData&, SizeT>)
		{
			SizeT index = 0;
			if (this->node != nullptr)
				co_await ForEachAsyncValueImplWithIndex(this->node, lambda, index);

			co_return;
		}
		else
		{
			static_assert(Concepts::IsAsyncInvocable<LambdaT, TData&> ||
				Concepts::IsAsyncInvocable<LambdaT, const TData&> ||
				Concepts::IsAsyncInvocable<LambdaT, TData&, SizeT> ||
				Concepts::IsAsyncInvocable<LambdaT, const TData&, SizeT>);
		}
	}
#pragma endregion

#pragma region RemoveAll
	template<typename THeap, typename TString, typename TData, Containers::TCharacterComparator CharacterComparator>
	template<typename TLambda>
	inline Threading::BaseTask<void, void> Containers::BaseDictionary<THeap, TString, TData, CharacterComparator>::destroy_treeAsync(NodeSym* This, TLambda& lambda)
	{
		if (This == nullptr)
			co_return;

		if (This->left != nullptr)
		{
			co_await destroy_treeAsync(This->left, lambda);
			This->left = nullptr;
		}

		if (This->data != nullptr)
		{
			co_await destroy_nodeAsync(This->data, lambda);
			This->data = nullptr;
		}

		if (This->right != nullptr)
		{
			co_await destroy_treeAsync(This->right, lambda);
			This->right = nullptr;
		}

		delete This;
		co_return;
	}

	template<typename THeap, typename TString, typename TData, Containers::TCharacterComparator CharacterComparator>
	template<typename TLambda>
	inline Threading::BaseTask<void, void> Containers::BaseDictionary<THeap, TString, TData, CharacterComparator>::destroy_nodeAsync(NodeLex* This, TLambda& lambda)
	{
		if (This == nullptr)
			co_return;

		if (This->node_root != nullptr) {
			 co_await destroy_treeAsync(This->node_root, lambda);
			 This->node_root = nullptr;
		}

		if (This->data != nullptr)
		{
			co_await lambda(MoveRValue(*This->data));
			delete This->data;
		}

		delete This;
		co_return;
	}

	template<typename THeap, typename TString, typename TData, Containers::TCharacterComparator CharacterComparator>
	template<typename TLambda>
	inline Threading::BaseTask<void, void> Containers::BaseDictionary<THeap, TString, TData, CharacterComparator>::RemoveAllAsyncWithImpl(TLambda&& lambda)
	{
		co_await destroy_nodeAsync(this->node, lambda);
		this->node = nullptr;
		this->count = 0;

		co_return;
	}
#pragma endregion
}