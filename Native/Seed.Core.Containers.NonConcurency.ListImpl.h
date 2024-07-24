#pragma once
#include "Seed.Core.Containers.NonConcurency.List.h"

#pragma warning(push)
#pragma warning(disable: 4996)
#pragma warning(disable: 4595)
#pragma warning(disable: 28182)
#pragma warning(disable: 4172)

namespace Seed::Core::Containers
{
#pragma region Internal storage struct
	template<class THeap, typename T>
	BaseList<THeap, T>::~BaseList()
	{
		ExceptionHolderType ex;

		while (this->root != nullptr)
		{
			elem* node = this->root;
			this->root = root->next;
			if (--node->RefCount == 0)
			{
				try
				{
					delete node;
				}
				catch (...)
				{
					ex = Internals::Exceptions::StoreException(ex);
				}

				continue;
			}
			node->next = nullptr;
			node->father = nullptr;
			node->prev = nullptr;
		}

		Internals::Exceptions::ThrowFromExceptionHolder(ex);
	}

	template<class THeap, typename T>
	template<typename TLambda>
	BaseList<THeap, T>::elem* BaseList<THeap, T>::Merge(elem* first, elem* second, TLambda& lambda)
	{
		// If first linked list is empty
		if (!first)
			return second;

		// If second linked list is empty
		if (!second)
			return first;

		// Pick the smaller value
		if (lambda(first->value, second->value))
		{
			first->next = Merge(first->next, second, lambda);
			first->next->prev = first;
			first->prev = nullptr;
			return first;
		}
		else
		{
			second->next = Merge(first, second->next, lambda);
			second->next->prev = second;
			second->prev = nullptr;
			return second;
		}
	}

	// Function to do merge sort
	template<class THeap, typename T>
	template<typename TLambda>
	BaseList<THeap, T>::elem* BaseList<THeap, T>::MergeSort(elem* head, TLambda& lambda)
	{
		if (!head || !head->next)
			return head;
		elem* second = Split(head);

		// Recur for left and right halves
		head = MergeSort(head, lambda);
		second = MergeSort(second, lambda);

		// Merge the two sorted halves
		return Merge(head, second, lambda);
	}

	// Split a doubly linked list (DLL) into 2 DLLs of half sizes
	template<class THeap, typename T>
	inline BaseList<THeap, T>::elem* BaseList<THeap, T>::Split(elem* head)
	{
		elem* fast = head, * slow = head;
		while (fast->next && fast->next->next)
		{
			fast = fast->next->next;
			slow = slow->next;
		}
		elem* temp = slow->next;
		slow->next = nullptr;
		return temp;
	}
#pragma endregion

#pragma region ConstIterator
	template<class THeap, typename T>
	inline BaseList<THeap, T>::ConstIterator::ConstIterator(elem* This):This(This)
	{
		if (This != nullptr) ++This->RefCount;
	}

#pragma region Operators
#pragma region Iterator Ariphmetic
	template<class THeap, typename T>
	const typename BaseList<THeap, T>::ConstIterator BaseList<THeap, T>::ConstIterator::operator++(int) const
	{
		ConstIterator node(*this);
		if (This == nullptr)
			Internals::Exceptions::ThrowEndNodeNotReferenceable(SourcePosition);

		--This->RefCount;
		if (this->This->father == nullptr)
		{
			// If an element does not have a parent, so the container is already destroyed and there are no adjacent elements
			this->This = nullptr;
			return node;
		}
		else
		{
			this->This = this->This->next;
			if (this->This != nullptr) ++This->RefCount;
			return node;
		}
	}

	template<class THeap, typename T>
	const typename BaseList<THeap, T>::ConstIterator BaseList<THeap, T>::ConstIterator::operator--(int) const
	{
		ConstIterator node(*this);
		if (This == nullptr)
			Internals::Exceptions::ThrowEndNodeNotReferenceable(SourcePosition);

		--This->RefCount;
		if (This->father == nullptr)
		{
			// If an element does not have a parent, so the container is already destroyed and there are no adjacent elements
			this->This = nullptr;
			return node;
		}
		else
		{
			this->This = this->This->prev;
			if (this->This != nullptr) ++This->RefCount;
			return node;
		}
	}

	template<class THeap, typename T>
	typename BaseList<THeap, T>::ConstIterator& BaseList<THeap, T>::ConstIterator::operator++()
	{
		if (This == nullptr)
			Internals::Exceptions::ThrowEndNodeNotReferenceable(SourcePosition);

		--This->RefCount;
		if (This->father == nullptr)
		{
			// If an element does not have a parent, so the container is already destroyed and there are no adjacent elements
			if (This->RefCount == 0)
				delete This;
			This = nullptr;
		}
		else
		{
			this->This = this->This->next;
			if (this->This != nullptr) ++This->RefCount;
		}
		return *this;
	}

	template<class THeap, typename T>
	typename BaseList<THeap, T>::ConstIterator& BaseList<THeap, T>::ConstIterator::operator--()
	{
		if (This == nullptr)
			Internals::Exceptions::ThrowEndNodeNotReferenceable(SourcePosition);

		--This->RefCount;
		if (this->This->father == nullptr)
		{
			// If an element does not have a parent, so the container is already destroyed and there are no adjacent elements
			this->This = nullptr;
			return *this;
		}
		else
		{
			this->This = this->This->prev;
			if (this->This != nullptr) ++This->RefCount;
			return *this;
		}
	}
#pragma endregion

#pragma region Comparing
	template<class THeap, typename T>
	inline bool BaseList<THeap, T>::ConstIterator::operator==(const ConstIterator& second) const
	{
		return reinterpret_cast<SizeT>(This) == reinterpret_cast<SizeT>(second.This);
	}

	template<class THeap, typename T>
	inline bool BaseList<THeap, T>::ConstIterator::operator!=(const ConstIterator& second) const
	{
		return reinterpret_cast<SizeT>(This) != reinterpret_cast<SizeT>(second.This);
	}

	template<class THeap, typename T>
	inline bool BaseList<THeap, T>::ConstIterator::operator==(const NullptrT) const
	{
		return reinterpret_cast<SizeT>(This) == reinterpret_cast<SizeT>(nullptr);
	}

	template<class THeap, typename T>
	inline bool BaseList<THeap, T>::ConstIterator::operator!=(const NullptrT) const
	{
		return This != nullptr;
	}
#pragma endregion

#pragma region Data access operators
	template<class THeap, typename T>
	inline const T& BaseList<THeap, T>::ConstIterator::operator*()
	{
		if (This == nullptr)
			Internals::Exceptions::ThrowEndNodeNotReferenceable(SourcePosition);

		return This->value;
	}

	template<class THeap, typename T>
	inline const T& BaseList<THeap, T>::ConstIterator::operator*() const
	{
		if (This == nullptr)
			Internals::Exceptions::ThrowEndNodeNotReferenceable(SourcePosition);

		return This->value;
	}

	template<class THeap, typename T>
	inline const T* const BaseList<THeap, T>::ConstIterator::operator->()
	{
		if (This == nullptr)
			Internals::Exceptions::ThrowEndNodeNotReferenceable(SourcePosition);

		return &This->value;
	}

	template<class THeap, typename T>
	inline const T* const BaseList<THeap, T>::ConstIterator::operator->() const
	{
		if (This == nullptr)
			Internals::Exceptions::ThrowEndNodeNotReferenceable(SourcePosition);

		return &This->value;
	}
#pragma endregion
#pragma endregion

#pragma region Iterator move
	template<class THeap, typename T>
	inline const BaseList<THeap, T>::ConstIterator BaseList<THeap, T>::ConstIterator::Next() const
	{
		if (This == nullptr)
			Internals::Exceptions::ThrowEndNodeNotReferenceable(SourcePosition);

		if (This->father == nullptr)
			// If an element does not have a parent, so the container is already destroyed and there are no adjacent elements
			return ConstIterator(nullptr);
		else
			return ConstIterator(This->next);
	}

	template<class THeap, typename T>
	inline const BaseList<THeap, T>::ConstIterator BaseList<THeap, T>::ConstIterator::Prev() const
	{
		if (This == nullptr)
			Internals::Exceptions::ThrowEndNodeNotReferenceable(SourcePosition);

		if (This->father == nullptr)
			// If an element does not have a parent, so the container is already destroyed and there are no adjacent elements
			return ConstIterator(nullptr);
		else
			return ConstIterator(This->prev);
	}
#pragma endregion

#pragma region Ctor, Dtor, copy operators
	template<class THeap, typename T>
	inline BaseList<THeap, T>::ConstIterator::ConstIterator(const ConstIterator& iterator): This(iterator.This)
	{
		if (This != nullptr)
			++This->RefCount;
	}

	template<class THeap, typename T>
	inline BaseList<THeap, T>::ConstIterator::ConstIterator(ConstIterator&& iterator): This(iterator.This)
	{
		iterator.This = nullptr;
	}

	template<class THeap, typename T>
	inline BaseList<THeap, T>::ConstIterator& BaseList<THeap, T>::ConstIterator::operator=(const ConstIterator& iterator)
	{
		this->~ConstIterator();
		this->ConstIterator::ConstIterator(iterator);
		return *this;
	}

	template<class THeap, typename T>
	inline BaseList<THeap, T>::ConstIterator& BaseList<THeap, T>::ConstIterator::operator=(ConstIterator&& iterator)
	{
		this->~ConstIterator();
		this->ConstIterator::ConstIterator(MoveRValue(iterator));
		return *this;
	}

	template<class THeap, typename T>
	inline BaseList<THeap, T>::ConstIterator::~ConstIterator()
	{
		if (This != nullptr)
		{
			if (--This->RefCount == 0)
			{
				delete This;
			}
			This = nullptr;
		}
	}
#pragma endregion
#pragma endregion

#pragma region Iterator
	template<class THeap, typename T>
	inline BaseList<THeap, T>::Iterator::Iterator(elem* This):This(This)
	{
		if (This != nullptr) ++This->RefCount;
	}

#pragma region Operators
#pragma region Iterator Ariphmetic
	template<class THeap, typename T>
	const typename BaseList<THeap, T>::Iterator BaseList<THeap, T>::Iterator::operator++(int) const
	{
		Iterator node(*this);
		if (This == nullptr)
			Internals::Exceptions::ThrowEndNodeNotReferenceable(SourcePosition);

		--This->RefCount;
		if (this->This->father == nullptr)
		{
			// If an element does not have a parent, so the container is already destroyed and there are no adjacent elements
			this->This = nullptr;
			return node;
		}
		else
		{
			this->This = this->This->next;
			if (this->This != nullptr) ++This->RefCount;
			return node;
		}
	}

	template<class THeap, typename T>
	const typename BaseList<THeap, T>::Iterator BaseList<THeap, T>::Iterator::operator--(int) const
	{
		Iterator node(*this);
		if (This == nullptr)
			Internals::Exceptions::ThrowEndNodeNotReferenceable(SourcePosition);

		--This->RefCount;
		if (This->father == nullptr)
		{
			// If an element does not have a parent, so the container is already destroyed and there are no adjacent elements
			this->This = nullptr;
			return node;
		}
		else
		{
			this->This = this->This->prev;
			if (this->This != nullptr) ++This->RefCount;
			return node;
		}
	}

	template<class THeap, typename T>
	typename BaseList<THeap, T>::Iterator& BaseList<THeap, T>::Iterator::operator++()
	{
		if (This == nullptr)
			Internals::Exceptions::ThrowEndNodeNotReferenceable(SourcePosition);

		--This->RefCount;
		if (This->father == nullptr)
		{
			// If an element does not have a parent, so the container is already destroyed and there are no adjacent elements
			if (This->RefCount == 0)
				delete This;
			This = nullptr;
		}
		else
		{
			this->This = this->This->next;
			if (this->This != nullptr) ++This->RefCount;
		}
		return *this;
	}

	template<class THeap, typename T>
	typename BaseList<THeap, T>::Iterator& BaseList<THeap, T>::Iterator::operator--()
	{
		if (This == nullptr)
			Internals::Exceptions::ThrowEndNodeNotReferenceable(SourcePosition);

		--This->RefCount;
		if (this->This->father == nullptr)
		{
			// If an element does not have a parent, so the container is already destroyed and there are no adjacent elements
			this->This = nullptr;
			return *this;
		}
		else
		{
			this->This = this->This->prev;
			if (this->This != nullptr) ++This->RefCount;
			return *this;
		}
	}
#pragma endregion

#pragma region Comparing
	template<class THeap, typename T>
	inline bool BaseList<THeap, T>::Iterator::operator==(const Iterator& second) const
	{
		return reinterpret_cast<SizeT>(This) == reinterpret_cast<SizeT>(second.This);
	}

	template<class THeap, typename T>
	inline bool BaseList<THeap, T>::Iterator::operator==(const Seed::Core::NullptrT) const
	{
		return reinterpret_cast<SizeT>(This) == reinterpret_cast<SizeT>(nullptr);
	}

	template<class THeap, typename T>
	BaseList<THeap, T>::Iterator::operator BaseList<THeap, T>::ConstIterator() const
	{
		return ConstIterator(This);
	}
#pragma endregion

#pragma region Data access operators
	template<class THeap, typename T>
	inline T& BaseList<THeap, T>::Iterator::operator*()
	{
		if (This == nullptr)
			Internals::Exceptions::ThrowEndNodeNotReferenceable(SourcePosition);

		return This->value;
	}

	template<class THeap, typename T>
	inline const T& BaseList<THeap, T>::Iterator::operator*() const
	{
		if (This == nullptr)
			Internals::Exceptions::ThrowEndNodeNotReferenceable(SourcePosition);

		return This->value;
	}

	template<class THeap, typename T>
	inline T* BaseList<THeap, T>::Iterator::operator->()
	{
		if (This == nullptr)
			Internals::Exceptions::ThrowEndNodeNotReferenceable(SourcePosition);

		return &This->value;
	}

	template<class THeap, typename T>
	inline const T* BaseList<THeap, T>::Iterator::operator->() const
	{
		if (This == nullptr)
			Internals::Exceptions::ThrowEndNodeNotReferenceable(SourcePosition);

		return &This->value;
	}
#pragma endregion
#pragma endregion

#pragma region Iterator move
	template<class THeap, typename T>
	inline const BaseList<THeap, T>::Iterator BaseList<THeap, T>::Iterator::Next() const
	{
		if (This == nullptr)
			Internals::Exceptions::ThrowEndNodeNotReferenceable(SourcePosition);

		if (This->father == nullptr)
			// If an element does not have a parent, so the container is already destroyed and there are no adjacent elements
			return Iterator(nullptr);
		else
			return Iterator(This->next);
	}

	template<class THeap, typename T>
	inline const BaseList<THeap, T>::Iterator BaseList<THeap, T>::Iterator::Prev() const
	{
		if (This == nullptr)
			Internals::Exceptions::ThrowEndNodeNotReferenceable(SourcePosition);

		if (This->father == nullptr)
			// If an element does not have a parent, so the container is already destroyed and there are no adjacent elements
			return Iterator(nullptr);
		else
			return Iterator(This->prev);
	}
#pragma endregion

#pragma region List element manipulations
	template<class THeap, typename T>
	BaseList<THeap, T>::Iterator BaseList<THeap, T>::Iterator::Remove(bool isReciveNextItInstead)
	{
		if (this->This == nullptr)
			Internals::Exceptions::ThrowEndNodeNotReferenceable(SourcePosition);

		if (this->This->father == nullptr)
		{
			if (--This->RefCount == 0)
				delete This;
			This = nullptr;
			return *this;
		}
		else
		{
			__assume(this->This->father != nullptr); //-V547

			if (this->This->next == nullptr && this->This->prev == nullptr)
			{
				this->This->father->root = this->This->father->last = nullptr;
			}
			else if (this->This->next != nullptr && this->This->prev != nullptr)
			{
				this->This->prev->next = this->This->next;
				this->This->next->prev = this->This->prev;
			}
			else if (this->This->next == nullptr)
			{
				if (this->This->prev != nullptr)
					this->This->prev->next = nullptr;
				this->This->father->last = this->This->prev;
			}
			else if (this->This->prev == nullptr) //-V2516
			{
				this->This->next->prev = nullptr;
				this->This->father->root = this->This->next;
			}

			--((This)->father->size);


			if (--this->This->RefCount == 1)
			{
				auto newThis = this->This->next;
				delete this->This;
				if (isReciveNextItInstead)
				{
					this->This = newThis;
					if (this->This != nullptr)
						++this->This->RefCount;
				}
				else
				{
					this->This = nullptr;
				}
			}
			else
			{
				auto newThis = this->This->next;
				this->This->next = nullptr;
				this->This->prev = nullptr;
				if (isReciveNextItInstead)
				{
					this->This = newThis;
					if (this->This != nullptr)
						++this->This->RefCount;
				}
				else
				{
					this->This = nullptr;
				}
			}
		}
		return *this;
	}

#pragma region Adding element to list by iterator
	template<class THeap, typename T>
	inline BaseList<THeap, T>::Iterator BaseList<THeap, T>::Iterator::Insert(const T& Value)
	{
		return AddAfter(Value);
	}

	template<class THeap, typename T>
	inline BaseList<THeap, T>::Iterator BaseList<THeap, T>::Iterator::Insert(T&& Value)
	{
		return AddAfter(MoveRValue(Value));
	}

	template<class THeap, typename T>
	BaseList<THeap, T>::Iterator BaseList<THeap, T>::Iterator::AddAfter(const T& value)
	{

		if (This == nullptr)
			Internals::Exceptions::ThrowNullPointException(SourcePosition);

		if (This->father == nullptr)
			Internals::Exceptions::ThrowNullPointException(SourcePosition, reinterpret_cast<SizeT>(This->father), 0);

		// New element creation
		auto newAfter = new elem(value, This->father, This, This->next);
		if (This->next != nullptr)
		{
			This->next->prev = newAfter;
		}
		else
		{
			This->father->last = newAfter;
		}
		This->next = newAfter;
		This->father->size++;
		return newAfter;
	}

	template<class THeap, typename T>
	BaseList<THeap, T>::Iterator BaseList<THeap, T>::Iterator::AddAfter(T&& value)
	{
		if (This == nullptr)
			Internals::Exceptions::ThrowNullPointException(SourcePosition);

		if (This->father == nullptr)
			Internals::Exceptions::ThrowNullPointException(SourcePosition, reinterpret_cast<SizeT>(This->father), 0);

		// New element creation
		auto newAfter = new elem(MoveRValue(value), This->father, This, This->next);
		if (This->next != nullptr)
		{
			This->next->prev = newAfter;
		}
		else
		{
			This->father->last = newAfter;
		}
		This->next = newAfter;
		This->father->size++;
		return newAfter;
	}

	template<class THeap, typename T>
	BaseList<THeap, T>::Iterator BaseList<THeap, T>::Iterator::AddBefore(const T& value)
	{
		if (This == nullptr)
			Internals::Exceptions::ThrowNullPointException(SourcePosition);

		if (This->father == nullptr)
			Internals::Exceptions::ThrowNullPointException(SourcePosition, reinterpret_cast<SizeT>(This->father), 0);

		// New element creation
		auto newBefore = new elem(value, This->father, This->prev, This);
		if (This->prev != nullptr)
		{
			This->prev->next = newBefore;
		}
		else
		{
			This->father->root = newBefore;
		}
		This->prev = newBefore;
		This->father->size++;
		return newBefore;
	}

	template<class THeap, typename T>
	BaseList<THeap, T>::Iterator BaseList<THeap, T>::Iterator::AddBefore(T&& value)
	{
		if (This == nullptr)
			Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);

		if (This->father == nullptr)
			Internals::Exceptions::ThrowNullPointException(SourcePosition, reinterpret_cast<SizeT>(This->father), 0);

		// New element creation
		auto newBefore = new elem(MoveRValue(value), This->father, This->prev, This);
		if (This->prev != nullptr)
		{
			This->prev->next = newBefore;
		}
		else
		{
			This->father->root = newBefore;
		}
		This->prev = newBefore;
		This->father->size++;
		return newBefore;
	}
#pragma endregion
#pragma endregion

#pragma region Ctor, Dtor, copy operators
	template<class THeap, typename T>
	inline BaseList<THeap, T>::Iterator::Iterator(const Iterator& iterator): This(iterator.This)
	{
		if (This != nullptr)
			++This->RefCount;
	}

	template<class THeap, typename T>
	inline BaseList<THeap, T>::Iterator::Iterator(Iterator&& iterator): This(iterator.This)
	{
		iterator.This = nullptr;
	}

	template<class THeap, typename T>
	inline BaseList<THeap, T>::Iterator& BaseList<THeap, T>::Iterator::operator=(const Iterator& iterator)
	{
		this->~Iterator();
		this->Iterator::Iterator(iterator);
		return *this;
	}

	template<class THeap, typename T>
	inline BaseList<THeap, T>::Iterator& BaseList<THeap, T>::Iterator::operator=(Iterator&& iterator)
	{
		this->~Iterator();
		this->Iterator::Iterator(MoveRValue(iterator));
		return *this;
	}

	template<class THeap, typename T>
	inline BaseList<THeap, T>::Iterator::~Iterator()
	{
		if (This != nullptr)
		{
			if (--This->RefCount == 0)
			{
				delete This;
			}
			This = nullptr;
		}
	}
#pragma endregion
#pragma endregion

#pragma region Operators
	template<class THeap, typename T>
	BaseList<THeap, T>::BaseList(const BaseList& original): size(original.size)
	{
		elem* node = original.root;
		if (node != nullptr)
		{
			this->root = this->last = new elem(node->value, this);
			node = node->next;
			while (node != nullptr)
			{
				this->last = this->last->next = new elem(node->value, this, this->last);
				node = node->next;
			}
		}
		else
		{
			this->root = this->last = nullptr;
		}
	}

	template<class THeap, typename T>
	BaseList<THeap, T>::BaseList(BaseList&& original):
		size(original.size),
		root(original.root),
		last(original.last)
	{
		if (original.root != nullptr)
		{
			original.root = nullptr;
			original.last = nullptr;
			original.size = 0;

			auto node = this->root;
			while (node != nullptr)
			{
				node->father = this;
				node = node->next;
			}
		}

		original.size = 0;
	}

	template<class THeap, typename T>
	BaseList<THeap, T>& BaseList<THeap, T>::operator=(const BaseList& original)
	{
		this->~BaseList();
		this->BaseList::BaseList(original);
		return *this;
	}

	template<class THeap, typename T>
	BaseList<THeap, T>& BaseList<THeap, T>::operator=(BaseList&& original)
	{
		this->~BaseList();
		this->BaseList::BaseList(MoveRValue(original));
		return *this;
	}

	template<class THeap, typename T>
	BaseList<THeap, T>::BaseList(InitilizerList<T>&& list): size(list.size())
	{
		if (list.size() == 0) return;
		auto it = list.begin();
		this->root = new elem(MoveRValue(*it), this);
		this->root->value = *it;
		this->last = this->root;
		++it;

		if (this->size != 1)
		{
			for (SizeT i = 1; i < this->size; i++)
			{
				this->last->next = new elem(MoveRValue(*it), this, this->last);
				this->last = this->last->next;
				++it;
			}
		}
	}

	template<class THeap, typename T>
	inline BaseList<THeap, T>& BaseList<THeap, T>::operator+=(const T& Value)
	{
		this->AddLast(Value); return *this;
	}

	template<class THeap, typename T>
	inline BaseList<THeap, T>& BaseList<THeap, T>::operator+=(T&& Value)
	{
		this->AddLast(MoveRValue(Value)); return *this;
	}

	template<class THeap, typename T>
	BaseList<THeap, T>& BaseList<THeap, T>::operator+=(const BaseList& otherList)
	{
		if (otherList.root == this->root)
			return *this;

		if (otherList.size == 0)
			return *this;

		auto node = otherList.root;
		while (node != nullptr)
		{
			if (this->size == 0)
			{
				this->last = this->root = new elem(node->value, this, this->last);
			}
			else
			{
				this->last->next = new elem(node->value, this, this->last);
			}

			this->last = this->last->next;
			node = node->next;
		}
		this->size = this->size + otherList.size;
		return *this;
	}

	template<class THeap, typename T>
	BaseList<THeap, T>& BaseList<THeap, T>::operator+=(BaseList&& otherList)
	{
		if (otherList.root == this->root)
			return *this;

		if (otherList.size == 0)
			return *this;

		if (this->root == nullptr)
		{
			*this = MoveRValue(otherList);
			return *this;
		}

		//Ignore otherList.This->root == null as if it true than otherList.This->size == 0
		__assume(otherList.root != nullptr);
		auto item = otherList.root;
		while (item != nullptr)
		{
			item->father = this;
			item = item->next;
		}
		otherList.root->prev = this->last;
		this->last->next = otherList.root;
		this->last = otherList.last;
		this->size = this->size + otherList.size;
		otherList.root = otherList.last = nullptr;
		otherList.size = 0;
		return *this;
	}

	template<class THeap, typename T>
	Boolean BaseList<THeap, T>::operator==(const BaseList& second) const requires Concepts::HaveEqual<T, T>
	{
		if (&this->size == &second.size)
			return true;

		if (this->Count == second.Count)
		{
			auto it1 = this->root;
			auto it2 = second.root;
			while (it1 != nullptr)
			{
				if (!(it1->value == it2->value))
					return false;
				it1 = it1->next;
				it2 = it2->next;
			}
			return true;
		}
		return false;
	}

	template<class THeap, typename T>
	Boolean BaseList<THeap, T>::operator!=(const BaseList& second) const requires Concepts::HaveNotEqual<T, T>
	{
		if (&this->size == &second.size)
			return true;

		if (this->Count == second.Count)
		{
			auto it1 = this->root;
			auto it2 = second.root;
			while (it1 != nullptr)
			{
				if (!(it1->value != it2->value))
					return true;
				it1 = it1->next;
				it2 = it2->next;
			}
			return false;
		}
		return true;
	}
#pragma endregion

#pragma region Item count methods
	template<class THeap, typename T>
	inline Boolean BaseList<THeap, T>::IsEmpty() const
	{
		return this->size == 0;
	}

	template<class THeap, typename T>
	inline SizeT BaseList<THeap, T>::GetCount() const
	{
		return this->size;
	}
#pragma endregion

#pragma region Iterators
#pragma region Gettter
	template<class THeap, typename T>
	inline BaseList<THeap, T>::Iterator BaseList<THeap, T>::GetRoot()
	{
		if (this->root == nullptr) return GetEnd();
		return Iterator(this->root);
	}

	template<class THeap, typename T>
	inline BaseList<THeap, T>::Iterator BaseList<THeap, T>::GetLast()
	{
		if (this->last == nullptr) return GetEnd();
		return Iterator(this->last);
	}

	template<class THeap, typename T>
	inline BaseList<THeap, T>::ConstIterator BaseList<THeap, T>::GetRoot() const
	{
		if (this->root == nullptr) return GetEnd();
		return ConstIterator(this->root);
	}

	template<class THeap, typename T>
	inline BaseList<THeap, T>::ConstIterator BaseList<THeap, T>::GetLast() const
	{
		if (this->last == nullptr) return GetEnd();
		return ConstIterator(this->last);
	}

	template<class THeap, typename T>
	inline BaseList<THeap, T>::Iterator BaseList<THeap, T>::GetEnd()
	{
		return Iterator(nullptr);
	}

	template<class THeap, typename T>
	inline BaseList<THeap, T>::ConstIterator BaseList<THeap, T>::GetEnd() const
	{
		return ConstIterator(nullptr);
	}
#pragma endregion

#pragma region Add new value by iterator
	template<class THeap, typename T>
	BaseList<THeap, T>::Iterator BaseList<THeap, T>::AddFirst(const T& Value) {
		if (this->root == nullptr)
		{
			this->last = this->root = new elem(Value, this);
			this->size = 1;
			return this->last;
		}

		this->root = this->root->prev = new elem(Value, this, nullptr, this->root);
		++this->size;
		return this->last;
	}

	template<class THeap, typename T>
	BaseList<THeap, T>::Iterator BaseList<THeap, T>::AddFirst(T&& Value) {
		if (this->root == nullptr)
		{
			this->last = this->root = new elem(MoveRValue(Value), this);
			this->size = 1;
			return this->last;
		}

		this->root = this->root->prev = new elem(MoveRValue(Value), this, nullptr, this->root);
		++this->size;
		return this->last;
	}

	template<class THeap, typename T>
	BaseList<THeap, T>::Iterator BaseList<THeap, T>::AddLast(const T& Value)
	{
		if (this->root == nullptr)
		{
			this->last = this->root = new elem(Value, this);
			this->size = 1;
			return this->last;
		}
		this->last = this->last->next = new elem(Value, this, this->last);
		++this->size;
		return this->last;
	}

	template<class THeap, typename T>
	BaseList<THeap, T>::Iterator BaseList<THeap, T>::AddLast(T&& Value)
	{
		if (this->root == nullptr)
		{
			this->last = this->root = new elem(MoveRValue(Value), this);
			this->size = 1;
			return this->last;
		}

		this->last = this->last->next = new elem(MoveRValue(Value), this, this->last);
		++this->size;
		return this->last;
	}

	template<class THeap, typename T>
	inline BaseList<THeap, T>::Iterator BaseList<THeap, T>::AddAfter(Iterator Item, const T& Value)
	{
		return Item.AddAfter(Value);
	}

	template<class THeap, typename T>
	inline BaseList<THeap, T>::Iterator BaseList<THeap, T>::AddAfter(Iterator Item, T&& Value)
	{
		return Item.AddAfter(MoveRValue(Value));
	}

	template<class THeap, typename T>
	inline BaseList<THeap, T>::Iterator BaseList<THeap, T>::AddBefore(Iterator Item, const T& Value)
	{
		return Item.AddBefore(Value);
	}

	template<class THeap, typename T>
	inline BaseList<THeap, T>::Iterator BaseList<THeap, T>::AddBefore(Iterator Item, T&& Value)
	{
		return Item.AddBefore(MoveRValue(Value));
	}
#pragma endregion
#pragma endregion

#pragma region For each support methods
	template<class THeap, typename T>
	inline BaseList<THeap, T>::ConstIterator BaseList<THeap, T>::begin() const
	{
		return GetRoot();
	}

	template<class THeap, typename T>
	inline BaseList<THeap, T>::ConstIterator BaseList<THeap, T>::end() const
	{
		return nullptr;
	}

	template<class THeap, typename T>
	inline BaseList<THeap, T>::Iterator BaseList<THeap, T>::begin()
	{
		return GetRoot();
	}

	template<class THeap, typename T>
	inline BaseList<THeap, T>::Iterator BaseList<THeap, T>::end()
	{
		return nullptr;
	}
#pragma endregion

	template<class THeap, typename T>
	void BaseList<THeap, T>::Remove(Iterator& node)
	{
		if (node.This == nullptr)
			return;

		if (node.This->father == nullptr)
		{
			if (--node.This->RefCount == 0)
				delete node.This;
			node.This = nullptr;
		}
		else
		{
			if (node.This->next == nullptr && node.This->prev == nullptr)
			{
				node.This->father->root = node.This->father->last = nullptr;
			}
			else if (node.This->next != nullptr && node.This->prev != nullptr)
			{
				node.This->prev->next = node.This->next;
				node.This->next->prev = node.This->prev;
			}
			else if (node.This->next == nullptr)
			{
				if (node.This->prev != nullptr)
					node.This->prev->next = nullptr;
				node.This->father->last = node.This->prev;
			}
			else if (node.This->prev == nullptr)
			{
				node.This->next->prev = nullptr;
				node.This->father->root = node.This->next;
			}

			--((node.This)->father->size);

			if (--node.This->RefCount == 1)
			{
				delete node.This;
				node.This = nullptr;
			}
			else
			{
				node.This->next = nullptr;
				node.This->prev = nullptr;
				node.This = nullptr;
			}
		}
	}

	template<class THeap, typename T>
	void BaseList<THeap, T>::Remove(Iterator&& node)
	{
		Remove(node);
	}

	template<class THeap, typename T>
	template<typename> requires (Concepts::IsClonable<T>)
	BaseList<THeap, T> BaseList<THeap, T>::Clone() const
	{
		BaseList result;
		result.size = this->size;
		elem* node = this->root;
		if (node != nullptr)
		{
			result.root = result.last = new elem(Core::Clone(node->value), &result);
			node = node->next;
			while (node != nullptr)
			{
				result.last = result.last->next = new elem(Core::Clone(node->value), &result, result.last);
				node = node->next;
			}
		}
		return result;
	}

	template<class THeap, typename T>
	template<typename TLambda> requires (Concepts::IsInvocableWithResult<TLambda, T, const T&>)
	BaseList<THeap, T> BaseList<THeap, T>::Clone(TLambda&& lambda) const
	{
		BaseList result;
		result.size = this->size;
		elem* node = this->root;
		if (node != nullptr)
		{
			result.root = result.last = new elem(lambda(node->value), result);
			node = node->next;
			while (node != nullptr)
			{
				result.last = result.last->next = new elem(lambda(node->value), result, result.last);
				node = node->next;
			}
		}
		return result;
	}

	template<class THeap, typename T>
	template<typename TLambda>
	BaseList<THeap, T>& BaseList<THeap, T>::ForEachImpl(TLambda&& lambda)
	{
		if constexpr (TypeTraits::IsInvocable<TLambda, const T&> ||
						TypeTraits::IsInvocable<TLambda, T&>)
		{
			auto node = this->root;
			while (node != nullptr)
			{
				lambda(node->value);
				node = node->next;
			}
			return *this;
		}
		else if constexpr (TypeTraits::IsInvocable<TLambda, const T&, SizeT> ||
							TypeTraits::IsInvocable<TLambda, T&, SizeT>)
		{
			auto node = this->root;
			SizeT index = 0;
			while (node != nullptr)
			{
				lambda(node->value, index);
				node = node->next;
				index++;
			}
			return *this;
		}
		else
		{
			static_assert(
				TypeTraits::IsInvocable<TLambda, const T&> ||
				TypeTraits::IsInvocable<TLambda, T&> ||
				TypeTraits::IsInvocable<TLambda, const T&, SizeT> ||
				TypeTraits::IsInvocable<TLambda, T&, SizeT>);
		}
	}

	template<class THeap, typename T>
	template<typename TLambda>
	const BaseList<THeap, T>& BaseList<THeap, T>::ForEachImpl(TLambda&& lambda) const
	{
		if constexpr (TypeTraits::IsInvocable<TLambda, const T&>)
		{
			auto node = this->root;
			while (node != nullptr)
			{
				lambda(node->value);
				node = node->next;
			}
			return *this;
		}
		else if constexpr (TypeTraits::IsInvocable<TLambda, const T&, SizeT>)
		{
			auto node = this->root;
			SizeT index = 0;
			while (node != nullptr)
			{
				lambda(node->value, index);
				node = node->next;
				index++;
			}
			return *this;
		}
		else
		{
			static_assert(
				TypeTraits::IsInvocable<TLambda, const T&> ||
				TypeTraits::IsInvocable<TLambda, const T&, SizeT>
				);
		}
	}

	template<class THeap, typename T>
	template<typename TLambda>
	BaseList<THeap, T>& BaseList<THeap, T>::ForEachImpl(TLambda&& lambda, SizeT count)
	{
		if constexpr (TypeTraits::IsInvocable<TLambda, const T&> ||
						TypeTraits::IsInvocable<TLambda, T&>)
		{
			if (count > this->size)
			{
				auto node = this->root;
				while (node != nullptr)
				{
					lambda(node->value);
					node = node->next;
				}
				return *this;
			}
			else
			{
				SizeT i = 0;
				auto node = this->root;
				while (node != nullptr)
				{
					lambda(node->value);
					node = node->next;
					i++;
					if (i == count)
						return *this;
				}
				return *this;
			}
		}
		else if constexpr (TypeTraits::IsInvocable<TLambda, const T&, SizeT> ||
							TypeTraits::IsInvocable<TLambda, T&, SizeT>)
		{
			if (count > this->size)
			{
				auto node = this->root;
				SizeT index = 0;
				while (node != nullptr)
				{
					lambda(node->value, index);
					node = node->next;
					index++;
				}
				return *this;
			}
			else
			{
				SizeT index = 0;
				auto node = this->root;
				while (node != nullptr)
				{
					lambda(node->value, index);
					node = node->next;
					index++;
					if (index == count)
						return *this;
				}
				return *this;
			}
		}
		else
		{
			static_assert(
				TypeTraits::IsInvocable<TLambda, const T&> ||
				TypeTraits::IsInvocable<TLambda, T&> ||
				TypeTraits::IsInvocable<TLambda, const T&, SizeT> ||
				TypeTraits::IsInvocable<TLambda, T&, SizeT>);
		}
	}

	template<class THeap, typename T>
	template<typename TLambda>
	const BaseList<THeap, T>& BaseList<THeap, T>::ForEachImpl(TLambda&& lambda, SizeT count) const
	{
		if constexpr (TypeTraits::IsInvocable<TLambda, const T&>)
		{
			if (count > this->size)
			{
				auto node = this->root;
				while (node != nullptr)
				{
					lambda(node->value);
					node = node->next;
				}
				return *this;
			}
			else
			{
				SizeT i = 0;
				auto node = this->root;
				while (node != nullptr)
				{
					lambda(node->value);
					node = node->next;
					i++;
					if (i == count)
						return *this;
				}
				return *this;
			}
		}
		else if constexpr (TypeTraits::IsInvocable<TLambda, const T&, SizeT>)
		{
			if (count > this->size)
			{
				auto node = this->root;
				SizeT index = 0;
				while (node != nullptr)
				{
					lambda(node->value, index);
					node = node->next;
					index++;
				}
				return *this;
			}
			else
			{
				SizeT index = 0;
				auto node = this->root;
				while (node != nullptr)
				{
					lambda(node->value, index);
					node = node->next;
					index++;
					if (index == count)
						return *this;
				}
				return *this;
			}
		}
		else
		{
			static_assert(
				TypeTraits::IsInvocable<TLambda, const T&> ||
				TypeTraits::IsInvocable<TLambda, const T&, SizeT>
				);
		}
	}

	template<class THeap, typename T>
	template<typename U, typename TLambda>
	U BaseList<THeap, T>::AggregateImpl(TLambda&& lambda, const U& value) const
	{
		auto node = this->root;
		U result = value;
		U& data = result;
		SizeT index = 0;

		while (node != nullptr)
		{
			if constexpr (
				Concepts::IsInvocableWithResult<TLambda, U&, U&, const T&> ||
				Concepts::IsInvocableWithResult<TLambda, U, U&, const T&>)
			{
				data = lambda(data, node->value);
			}
			else
			{
				lambda(data, node->value);
			}

			node = node->next;
			index++;
		}
		return MoveRValue(result);
	}

	template<class THeap, typename T>
	template<typename U, typename TLambda>
	U BaseList<THeap, T>::AggregateImpl(TLambda&& lambda, U&& value) const
	{
		auto node = this->root;
		SizeT index = 0;

		while (node != nullptr)
		{
			if constexpr (
				Concepts::IsInvocableWithResult<TLambda, U&, U&, const T&> ||
				Concepts::IsInvocableWithResult<TLambda, U, U&, const T&>)
			{
				value = lambda(value, node->value);
			}
			else
			{
				lambda(value, node->value);
			}

			node = node->next;
			index++;
		}
		return MoveRValue(value);
	}

	template<class THeap, typename T>
	inline bool BaseList<THeap, T>::AnyImpl() const
	{
		return this->size > 0;
	}

	template<class THeap, typename T>
	template<typename TLambda>
	bool BaseList<THeap, T>::AnyImpl(TLambda lambda) const
	{
		auto node = this->root;
		SizeT index = 0;
		while (node != nullptr)
		{
			if (lambda(node->value))
				return true;
			node = node->next;
			index++;
		}
		return false;
	}

	template<class THeap, typename T>
	template<typename TLambda>
	bool BaseList<THeap, T>::AllImpl(TLambda lambda) const
	{
		auto node = this->root;
		SizeT index = 0;
		while (node != nullptr)
		{
			if (!lambda(node->value))
				return false;
			node = node->next;
			index++;
		}
		return true;
	}

	template<class THeap, typename T>
	BaseList<THeap, T>& BaseList<THeap, T>::SortImpl(bool IsDescending) requires Concepts::IsComparableWith<T, T>
	{
		if (!IsDescending)
		{
			auto l = [](const T& V1, const T& V2) InlineLambda{ return V1 < V2; };
			this->root = MergeSort(this->root, l);
		}
		else
		{
			auto l = [](const T& V1, const T& V2) InlineLambda{ return V1 > V2; };
			this->root = MergeSort(this->root, l);
		}

		this->last = [&] () {
			elem* node = this->root;
			while (node->next != nullptr)
				node = node->next;
			return node;
		}();

		return *this;
	}

	template<class THeap, typename T>
	template<typename TLambda>
	BaseList<THeap, T>& BaseList<THeap, T>::SortImpl(TLambda&& lambda)
	{
		if (this->size == 0)
			return *this;

		this->root = MergeSort(this->root, lambda);

		this->last = [&] () {
			elem* node = this->root;
			while (node->next != nullptr)
				node = node->next;
			return node;
		}();

		return *this;
	}

	template<class THeap, typename T>
	template<typename TLambda>
	BaseList<THeap, T>& BaseList<THeap, T>::SortByLambdaImpl(TLambda&& lambda, bool IsDescending)
	{
		if (this->size == 0)
			return *this;

		if (IsDescending)
		{
			auto l = [lambda = MoveRValue(lambda)](const T& l, const T& r) mutable InlineLambda{ return !(lambda(l) < lambda(r)); };
			this->root = MergeSort(this->root, l);
		}
		else
		{
			auto l = [lambda = MoveRValue(lambda)](const T& l, const T& r) mutable InlineLambda{ return lambda(l) < lambda(r); };
			this->root = MergeSort(this->root, l);
		}

		this->last = [&] () {
			elem* node = this->root;
			while (node->next != nullptr)
				node = node->next;
			return node;
		}();

		return *this;
	}

	template<class THeap, typename T>
	template<typename TLambda>
	T& BaseList<THeap, T>::FirstImpl(TLambda&& lambda)
	{
		elem* node = this->root;
		while (node != nullptr)
			if (lambda(node->value))
				return node->value;
			else
				node = node->next;

		Internals::Exceptions::ThrowNotFound(SourcePosition);
	}

	template<class THeap, typename T>
	template<typename TLambda>
	T& BaseList<THeap, T>::LastImpl(TLambda&& lambda)
	{
		elem* node = this->root;
		elem* selected = nullptr;
		while (node != nullptr)
		{
			if (lambda(node->value))
				selected = node;
			node = node->next;
		}

		if (selected != nullptr)
			return selected->value;

		Internals::Exceptions::ThrowNotFound(SourcePosition);
	}

	template<class THeap, typename T>
	template<typename TLambda>
	const T& BaseList<THeap, T>::FirstImpl(TLambda&& lambda) const
	{
		elem* node = this->root;
		while (node != nullptr)
			if (lambda(node->value))
				return node->value;
			else
				node = node->next;

		Internals::Exceptions::ThrowNotFound(SourcePosition);
	}

	template<class THeap, typename T>
	template<typename TLambda>
	const T& BaseList<THeap, T>::LastImpl(TLambda&& lambda) const
	{
		elem* node = this->root;
		elem* selected = nullptr;
		while (node != nullptr)
		{
			if (lambda(node->value))
				selected = node;
			node = node->next;
		}

		if (selected != nullptr)
			return selected->value;

		Internals::Exceptions::ThrowNotFound(SourcePosition);
	}

	template<class THeap, typename T>
	template<typename TLambda>
	BaseList<THeap, T>::Iterator BaseList<THeap, T>::FirstItImpl(TLambda&& lambda)
	{
		elem* node = this->root;
		while (node != nullptr)
			if (lambda(node->value))
				return Iterator(node);
			else
				node = node->next;

		return Iterator(nullptr);
	}

	template<class THeap, typename T>
	template<typename TLambda>
	BaseList<THeap, T>::Iterator BaseList<THeap, T>::LastItImpl(TLambda&& lambda)
	{
		elem* node = this->root;
		elem* selected = nullptr;
		while (node != nullptr)
		{
			if (lambda(node->value))
				selected = node;
			node = node->next;
		}

		if (selected != nullptr)
			return Iterator(selected);

		return Iterator(nullptr);
	}

	template<class THeap, typename T>
	template<typename TLambda>
	BaseList<THeap, T>::ConstIterator BaseList<THeap, T>::FirstItImpl(TLambda&& lambda) const
	{
		elem* node = this->root;
		while (node != nullptr)
			if (lambda(node->value))
				return ConstIterator(node);
			else
				node = node->next;

		return ConstIterator(nullptr);
	}

	template<class THeap, typename T>
	template<typename TLambda>
	BaseList<THeap, T>::ConstIterator BaseList<THeap, T>::LastItImpl(TLambda&& lambda) const
	{
		elem* node = this->root;
		elem* selected = nullptr;
		while (node != nullptr)
		{
			if (lambda(node->value))
				selected = node;
			node = node->next;
		}

		if (selected != nullptr)
			return ConstIterator(selected);

		return ConstIterator(nullptr);
	}

	template<class THeap, typename T>
	template<typename TLambda>
	inline T BaseList<THeap, T>::FirstOrImpl(TLambda&& lambda, T&& item) const
	{
		if (size == 0)
			return MoveRValue(item);

		elem* node = this->root;
		while (node != nullptr)
			if (lambda(node->value))
				return node->value;
			else
				node = node->next;

		return MoveRValue(item);
	}

	template<class THeap, typename T>
	template<typename TLambda>
	inline T BaseList<THeap, T>::LastOrImpl(TLambda&& lambda, T&& item) const
	{
		if (size == 0)
			return MoveRValue(item);

		elem* node = this->root;
		elem* selected = nullptr;
		while (node != nullptr)
		{
			if (lambda(node->value))
				selected = node;
			node = node->next;
		}

		if (selected != nullptr)
			return selected->value;

		return MoveRValue(item);
	}

	template<class THeap, typename T>
	template<typename TLambda>
	inline T BaseList<THeap, T>::FirstOrImpl(TLambda&& lambda, const T& item) const
	{
		if (size == 0)
			return item;

		elem* node = this->root;
		while (node != nullptr)
			if (lambda(node->value))
				return node->value;
			else
				node = node->next;

		return item;
	}

	template<class THeap, typename T>
	template<typename TLambda>
	inline T BaseList<THeap, T>::LastOrImpl(TLambda&& lambda, const T& item) const
	{
		if (size == 0)
			return item;

		elem* node = this->root;
		elem* selected = nullptr;
		while (node != nullptr)
		{
			if (lambda(node->value))
				selected = node;
			node = node->next;
		}

		if (selected != nullptr)
			return selected->value;

		return item;
	}

	template<class THeap, typename T>
	template<typename TLambda, typename TGetterLambda>
	inline T BaseList<THeap, T>::FirstOrImpl(TLambda&& lambda, TGetterLambda&& otherLambda) const
	{
		if (size == 0)
			return otherLambda();

		elem* node = this->root;
		while (node != nullptr)
			if (lambda(node->value))
				return node->value;
			else
				node = node->next;

		return otherLambda();
	}

	template<class THeap, typename T>
	template<typename TLambda, typename TGetterLambda>
	inline T BaseList<THeap, T>::LastOrImpl(TLambda&& lambda, TGetterLambda&& otherLambda) const
	{
		if (size == 0)
			return otherLambda();

		elem* node = this->root;
		elem* selected = nullptr;
		while (node != nullptr)
		{
			if (lambda(node->value))
				selected = node;
			node = node->next;
		}

		if (selected != nullptr)
			return selected->value;

		return otherLambda();
	}

	template<class THeap, typename T>
	BaseList<THeap, T>& BaseList<THeap, T>::ReverseImpl()&
	{
		elem* temp = nullptr;
		elem* current = this->root;

		while (current != nullptr)
		{
			temp = current->prev;
			current->prev = current->next;
			current->next = temp;
			current = current->prev;
		}

		Seed::Core::Swap(this->root, this->last);

		return *this;
	}

	template<class THeap, typename T>
	BaseList<THeap, T> BaseList<THeap, T>::ReverseImpl()&&
	{
		elem* temp = nullptr;
		elem* current = this->root;

		while (current != nullptr)
		{
			temp = current->prev;
			current->prev = current->next;
			current->next = temp;
			current = current->prev;
		}

		Seed::Core::Swap(this->root, this->last);

		return MoveRValue(*this);
	}

	template<class THeap, typename T>
	inline void BaseList<THeap, T>::UnsafeAdd(const T& value)
	{
		if (this->root == nullptr)
		{
			this->root = this->last = new elem(value, this);
		}
		else
		{
			this->last->prev->next = this->last = new elem(value, this, this->last);
		}

		++this->size;
	}

	template<class THeap, typename T>
	inline void BaseList<THeap, T>::UnsafeAdd(T&& value)
	{
		if (this->root == nullptr)
		{
			this->root = this->last = new elem(MoveRValue(value), this);
		}
		else
		{
			this->last->prev->next = this->last = new elem(MoveRValue(value), this, this->last);
		}

		++this->size;
	}

	template<class THeap, typename T>
	BaseList<THeap, T>& BaseList<THeap, T>::RemoveAllImpl()
	{
		while (this->root != nullptr)
		{
			elem* node = this->root;
			this->root = this->root->next;
			node->next = nullptr;
			node->father = nullptr;
			node->prev = nullptr;
			if (--node->RefCount == 0)
			{
				delete node;
				continue;
			}
		}

		this->root = this->last = nullptr;
		this->size = 0;
		return *this;
	}

	template<class THeap, typename T>
	template<typename TLambda>
	BaseList<THeap, T>& BaseList<THeap, T>::RemoveAllImpl(TLambda&& lambda)
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

			if constexpr (Concepts::IsInvocable<TLambda, T&&, SizeT>)
			{
				lambda(MoveRValue(node->value), idx); idx++;
			}
			else
			{
				lambda(MoveRValue(node->value));
			}

			if (--node->RefCount == 0)
			{
				delete node;
			}

			--this->size;
		}

		this->root = this->last = nullptr;
		this->size = 0;

		return *this;
	}

	template<class THeap, typename T>
	template<typename TFilterLambda, typename TLambda>
	BaseList<THeap, T>& BaseList<THeap, T>::RemoveIfFilterAndLambdaWithIndex(TFilterLambda&& filter, TLambda&& lambda)
	{
		if (this->root == nullptr)
			return *this;

		elem* it = this->root;
		elem* tmp;
		SizeT Index = 0;
		while (it != nullptr)
		{
			if (filter(it->value, Index))
			{
				lambda(it->value, Index);

				if (it->next == nullptr && it->prev == nullptr)
				{
					this->root = nullptr;
					this->last = nullptr;
					tmp = nullptr;
				}
				else if (it->next == nullptr)
				{
					tmp = nullptr;
					if (it->prev != nullptr)
						it->prev->next = nullptr;
					this->last = it->prev;
				}
				else if (it->prev == nullptr)
				{
					tmp = it->next;
					it->next->prev = nullptr;
					this->root = it->next;
				}
				else
				{
					tmp = it->next;
					tmp->prev = it->prev;
					it->prev->next = tmp;
				}
				--it->RefCount;
				if (it->RefCount == 0)
				{
					delete it;
				}
				--this->size;
				it = tmp;

			}
			else  it = it->next;
		}
		return *this;
	}

	template<class THeap, typename T>
	template<typename TFilterLambda, typename TLambda>
	BaseList<THeap, T>& BaseList<THeap, T>::RemoveIfFilterWithIndex(TFilterLambda&& filter, TLambda&& lambda)
	{
		if (this->root == nullptr)
			return *this;

		elem* it = this->root;
		elem* tmp;
		SizeT Index = 0;
		while (it != nullptr)
		{
			if (filter(it->value, Index))
			{
				lambda(it->value);

				if (it->next == nullptr && it->prev == nullptr)
				{
					this->root = nullptr;
					this->last = nullptr;
					tmp = nullptr;
				}
				else if (it->next == nullptr)
				{
					tmp = nullptr;
					if (it->prev != nullptr)
						it->prev->next = nullptr;
					this->last = it->prev;
				}
				else if (it->prev == nullptr)
				{
					tmp = it->next;
					it->next->prev = nullptr;
					this->root = it->next;
				}
				else
				{
					tmp = it->next;
					tmp->prev = it->prev;
					it->prev->next = tmp;
				}
				--it->RefCount;
				if (it->RefCount == 0)
				{
					delete it;
				}
				--this->size;
				it = tmp;

			}
			else  it = it->next;
		}
		return *this;
	}

	template<class THeap, typename T>
	template<typename TFilterLambda, typename TLambda>
	BaseList<THeap, T>& BaseList<THeap, T>::RemoveIfLambdaWithIndex(TFilterLambda&& filter, TLambda&& lambda)
	{
		if (this->root == nullptr)
			return *this;

		elem* it = this->root;
		elem* tmp;
		SizeT Index = 0;
		while (it != nullptr)
		{
			if (filter(it->value))
			{
				lambda(it->value, Index);

				if (it->next == nullptr && it->prev == nullptr)
				{
					this->root = nullptr;
					this->last = nullptr;
					tmp = nullptr;
				}
				else if (it->next == nullptr)
				{
					tmp = nullptr;
					if (it->prev != nullptr)
						it->prev->next = nullptr;
					this->last = it->prev;
				}
				else if (it->prev == nullptr)
				{
					tmp = it->next;
					it->next->prev = nullptr;
					this->root = it->next;
				}
				else
				{
					tmp = it->next;
					tmp->prev = it->prev;
					it->prev->next = tmp;
				}
				--it->RefCount;
				if (it->RefCount == 0)
				{
					delete it;
				}
				--this->size;
				it = tmp;

			}
			else  it = it->next;
		}
		return *this;
	}

	template<class THeap, typename T>
	template<typename TFilterLambda, typename TLambda>
	BaseList<THeap, T>& BaseList<THeap, T>::RemoveIfFilterAndLambdaWithoutIndex(TFilterLambda&& filter, TLambda&& lambda)
	{
		if (this->root == nullptr)
			return *this;

		elem* it = this->root;
		elem* tmp;
		SizeT Index = 0;
		while (it != nullptr)
		{
			if (filter(it->value))
			{
				lambda(MoveRValue(it->value));

				if (it->next == nullptr && it->prev == nullptr)
				{
					this->root = nullptr;
					this->last = nullptr;
					tmp = nullptr;
				}
				else if (it->next == nullptr)
				{
					tmp = nullptr;
					if (it->prev != nullptr)
						it->prev->next = nullptr;
					this->last = it->prev;
				}
				else if (it->prev == nullptr)
				{
					tmp = it->next;
					it->next->prev = nullptr;
					this->root = it->next;
				}
				else
				{
					tmp = it->next;
					tmp->prev = it->prev;
					it->prev->next = tmp;
				}
				--it->RefCount;
				if (it->RefCount == 0)
				{
					delete it;
				}
				--this->size;
				it = tmp;

			}
			else  it = it->next;
		}
		return *this;
	}

	template<class THeap, typename T>
	template<typename TFilterLambda, typename TLambda>
	inline BaseList<THeap, T>& BaseList<THeap, T>::RemoveIfImpl(TFilterLambda&& filter, TLambda&& lambda)
	{
		if constexpr (TypeTraits::IsInvocable<TLambda, T&&> &&
			TypeTraits::IsInvocableWithResult<TFilterLambda, Boolean, const T&>)
		{
			return RemoveIfFilterAndLambdaWithoutIndex(MoveRValue(filter), MoveRValue(lambda));
		}
		else if constexpr (TypeTraits::IsInvocable<TLambda, void, T&&> &&
			TypeTraits::IsInvocableWithResult<TFilterLambda, Boolean, const T&, SizeT>)
		{
			return RemoveIfFilterWithIndex(MoveRValue(filter), MoveRValue(lambda));
		}
		else if constexpr (TypeTraits::IsInvocable<TLambda, void, T&&, SizeT> &&
			TypeTraits::IsInvocableWithResult<TFilterLambda, Boolean, const T&>)
		{
			return RemoveIfLambdaWithIndex(MoveRValue(filter), MoveRValue(lambda));
		}
		else if constexpr (TypeTraits::IsInvocable<TLambda, void, T&&, SizeT> &&
			TypeTraits::IsInvocableWithResult<TFilterLambda, Boolean, const T&, SizeT>)
		{
			return RemoveIfFilterAndLambdaWithIndex(MoveRValue(filter), MoveRValue(lambda));
		}
		else
			static_assert((TypeTraits::IsInvocable<TLambda, T&&> &&
						   TypeTraits::IsInvocableWithResult<TFilterLambda, Boolean, const T&>) ||
				(TypeTraits::IsInvocable<TLambda, void, T&&> &&
				 TypeTraits::IsInvocableWithResult<TFilterLambda, Boolean, const T&, SizeT>) ||
				(TypeTraits::IsInvocable<TLambda, void, T&&, SizeT> &&
				 TypeTraits::IsInvocableWithResult<TFilterLambda, Boolean, const T&>) ||
				(TypeTraits::IsInvocable<TLambda, void, T&&, SizeT> &&
				 TypeTraits::IsInvocableWithResult<TFilterLambda, Boolean, const T&, SizeT>));
	}

	template<class THeap, typename T>
	template<typename TLambda>
	SizeT BaseList<THeap, T>::GetCountIfImpl(TLambda&& lambda) const
	{
		SizeT selectedCount = 0;

		auto node = this->root;
		while (node != nullptr)
		{
			if (lambda(node->value))
				selectedCount++;
			node = node->next;
		}

		return selectedCount;
	}
#pragma endregion
}
#pragma warning(pop)