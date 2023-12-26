#pragma once
#include "Seed.Core.Threading.Containers.NonConcurency.Base.h"

template<class THeap, typename T>
template<typename TLambda>
inline Seed::Core::Threading::BaseTask<void, void> Seed::Core::Containers::BaseList<THeap, T>::RemoveAllAsyncImpl(TLambda&& lambda)
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