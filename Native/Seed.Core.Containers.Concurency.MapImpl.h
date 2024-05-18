#pragma once
#include "Seed.Core.Containers.Concurency.Map.h"

#pragma warning(push)
// This internal implementation file which can use internal API functions
#pragma warning(disable: 4996)

namespace Seed::Core::Containers
{
	template<typename THeap, typename TKey, typename TValue>
	template<typename UKey>
	typename BaseConcurencyMap<THeap, TKey, TValue>::Node* BaseConcurencyMap<THeap, TKey, TValue>::Find(Node* node, const UKey& key)
	{
		if (node == nullptr)
			return nullptr;

		if (node->key == key)
			return node;
		else
			if (node->key < key)
				return Find(node->right, key);
			else
				return Find(node->left, key);
	}

	template<typename THeap, typename TKey, typename TValue>
	inline UInt16 BaseConcurencyMap<THeap, TKey, TValue>::Height(Node* node)
	{
		return node ? node->height : 0;
	}

	template<typename THeap, typename TKey, typename TValue>
	inline Int16 BaseConcurencyMap<THeap, TKey, TValue>::BFactor(Node* node)
	{
		return  Height(node->right) - Height(node->left);
	}

	template<typename THeap, typename TKey, typename TValue>
	inline void BaseConcurencyMap<THeap, TKey, TValue>::FixHeight(Node* node)
	{
		const UInt16 hLeft = Height(node->left);
		const UInt16 hRight = Height(node->right);
		node->height = Core::Max(hLeft, hRight) + 1;
	}

	template<typename THeap, typename TKey, typename TValue>
	inline typename BaseConcurencyMap<THeap, TKey, TValue>::Node* BaseConcurencyMap<THeap, TKey, TValue>::RotateRight(Node* node)
	{
		Node* left = node->left;
		Node* nodeFather = node->father;

		node->left = left->right;
		if (node->left != nullptr)
			node->left->father = node;

		left->right = node;
		if (left->right != nullptr)
			left->right->father = left;

		left->father = nodeFather;

		FixHeight(node);
		FixHeight(left);
		return left;
	}

	template<typename THeap, typename TKey, typename TValue>
	inline typename BaseConcurencyMap<THeap, TKey, TValue>::Node* BaseConcurencyMap<THeap, TKey, TValue>::RotateLeft(Node* node)
	{
		Node* right = node->right;
		Node* nodeFather = node->father;

		node->right = right->left;
		if (node->right != nullptr)
			node->right->father = node;

		right->left = node;
		if (right->left != nullptr)
			right->left->father = right;

		right->father = nodeFather;

		FixHeight(node);
		FixHeight(right);
		return right;
	}

	template<typename THeap, typename TKey, typename TValue>
	typename BaseConcurencyMap<THeap, TKey, TValue>::Node* BaseConcurencyMap<THeap, TKey, TValue>::Balance(Node* node)
	{
		FixHeight(node);
		if (BFactor(node) == 2)
		{
			if (BFactor(node->right) < 0)
				node->right = RotateRight(node->right);

			return RotateLeft(node);
		}

		if (BFactor(node) == -2)
		{
			if (BFactor(node->left) > 0)
				node->left = RotateLeft(node->left);

			return RotateRight(node);
		}
		return node;
	}

	template<typename THeap, typename TKey, typename TValue>
	typename BaseConcurencyMap<THeap, TKey, TValue>::Node* BaseConcurencyMap<THeap, TKey, TValue>::Insert(Node* node, const TKey& key, const TValue& value, Node* father)
	{
		if (!node) return new Node(key, value, father, this);

		if (key < node->key)
			node->left = Insert(node->left, key, value, node);
		else
			if (key != node->key)
				node->right = Insert(node->right, key, value, node);
			else
				Internals::Exceptions::ThrowKeyAlreadyExist(SourcePosition);

		return Balance(node);
	}

	template<typename THeap, typename TKey, typename TValue>
	typename BaseConcurencyMap<THeap, TKey, TValue>::Node* BaseConcurencyMap<THeap, TKey, TValue>::Insert(Node* node, TKey&& key, const TValue& value, Node* father)
	{
		if (!node) return new Node(MoveRValue(key), value, father, this);

		if (key < node->key)
			node->left = Insert(node->left, MoveRValue(key), value, node);
		else
			if (key != node->key)
				node->right = Insert(node->right, MoveRValue(key), value, node);
			else
				Internals::Exceptions::ThrowKeyAlreadyExist(SourcePosition);

		return Balance(node);
	}

	template<typename THeap, typename TKey, typename TValue>
	typename BaseConcurencyMap<THeap, TKey, TValue>::Node* BaseConcurencyMap<THeap, TKey, TValue>::Insert(Node* node, const TKey& key, TValue&& value, Node* father)
	{
		if (!node) return new Node(key, MoveRValue(value), father, this);

		if (key < node->key)
			node->left = Insert(node->left, key, MoveRValue(value), node);
		else
			if (key != node->key)
				node->right = Insert(node->right, key, MoveRValue(value), node);
			else
				Internals::Exceptions::ThrowKeyAlreadyExist(SourcePosition);

		return Balance(node);
	}

	template<typename THeap, typename TKey, typename TValue>
	typename BaseConcurencyMap<THeap, TKey, TValue>::Node* BaseConcurencyMap<THeap, TKey, TValue>::Insert(Node* node, TKey&& key, TValue&& value, Node* father)
	{
		if (!node) return new Node(MoveRValue(key), MoveRValue(value), father, this);

		if (key < node->key)
			node->left = Insert(node->left, MoveRValue(key), MoveRValue(value), node);
		else
			if (key != node->key)
				node->right = Insert(node->right, MoveRValue(key), MoveRValue(value), node);
			else
				Internals::Exceptions::ThrowKeyAlreadyExist(SourcePosition);

		return Balance(node);
	}

	template<typename THeap, typename TKey, typename TValue>
	typename BaseConcurencyMap<THeap, TKey, TValue>::Node* BaseConcurencyMap<THeap, TKey, TValue>::Insert(Node* node, const BasePair<THeap, TKey, TValue>& pair, Node* father)
	{
		if (!node) return new Node(pair, father, this);

		if (pair.Key < node->key)
			node->left = Insert(node->left, pair, node);
		else
			if (pair.Key != node->key)
				node->right = Insert(node->right, pair, node);
			else
				Internals::Exceptions::ThrowKeyAlreadyExist(SourcePosition);

		return Balance(node);
	}

	template<typename THeap, typename TKey, typename TValue>
	typename BaseConcurencyMap<THeap, TKey, TValue>::Node* BaseConcurencyMap<THeap, TKey, TValue>::Insert(Node* node, BasePair<THeap, TKey, TValue>&& pair, Node* father)
	{
		if (!node) return new Node(MoveRValue(pair), father, this);

		if (pair.Key < node->key)
			node->left = Insert(node->left, MoveRValue(pair), node);
		else
			if (pair.Key != node->key)
				node->right = Insert(node->right, MoveRValue(pair), node);
			else
				Internals::Exceptions::ThrowKeyAlreadyExist(SourcePosition);

		return Balance(node);
	}

	template<typename THeap, typename TKey, typename TValue>
	typename BaseConcurencyMap<THeap, TKey, TValue>::Node* BaseConcurencyMap<THeap, TKey, TValue>::FindMin(Node* node)
	{
		return node->left ? FindMin(node->left) : node;
	}

	template<typename THeap, typename TKey, typename TValue>
	typename BaseConcurencyMap<THeap, TKey, TValue>::Node* BaseConcurencyMap<THeap, TKey, TValue>::FindMax(Node* node)
	{
		return node->right ? FindMax(node->right) : node;
	}

	template<typename THeap, typename TKey, typename TValue>
	typename BaseConcurencyMap<THeap, TKey, TValue>::Node* BaseConcurencyMap<THeap, TKey, TValue>::RemoveMin(Node* node)
	{
		if (node->left == nullptr)
			return node->right;
		node->left = RemoveMin(node->left);
		return Balance(node);
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename UKey>
	typename BaseConcurencyMap<THeap, TKey, TValue>::Node* BaseConcurencyMap<THeap, TKey, TValue>::Remove(Node* node, const UKey& key)
	{
		if (!node) return nullptr;

		if (node->key > key)
			node->left = Remove(node->left, key);
		else if (node->key < key)
			node->right = Remove(node->right, key);
		else //key == Node->key
		{
			Node* left = node->left;
			Node* right = node->right;
			delete node;
			if (!right) return left;
			Node* min = FindMin(right);
			min->right = RemoveMin(right);
			min->left = left;
			return Balance(min);
		}

		return Balance(node);
	}

	template<typename THeap, typename TKey, typename TValue>
	typename BaseConcurencyMap<THeap, TKey, TValue>::Node* BaseConcurencyMap<THeap, TKey, TValue>::DestroyTree(Node* root) //-V688
	{
		if (root == nullptr)
			return nullptr;

		if (root->left != nullptr)
		{
			root->left = DestroyTree(root->left);
		}

		if (root->right != nullptr)
		{
			root->right = DestroyTree(root->right);
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

		return nullptr;
	}

	template<typename THeap, typename TKey, typename TValue>
	typename BaseConcurencyMap<THeap, TKey, TValue>::Node* BaseConcurencyMap<THeap, TKey, TValue>::copyTree(Node* node, Node* parent, BaseConcurencyMap* owner)
	{
		Node* newNode = new Node(node->key, node->value, parent, owner);
		newNode->height = node->height;

		if (node->left != nullptr)
			newNode->left = copyTree(node->left, newNode, owner);
		if (node->right != nullptr)
			newNode->right = copyTree(node->right, newNode, owner);

		return newNode;
	}

	template<typename THeap, typename TKey, typename TValue>
	typename BaseConcurencyMap<THeap, TKey, TValue>::Node* BaseConcurencyMap<THeap, TKey, TValue>::cloneTree(Node* node, Node* parent, BaseConcurencyMap* owner) const
	{
		Node* newNode = new Node(Core::Clone(node->key), Core::Clone(node->value), parent, owner);
		newNode->height = node->height;

		if (node->left != nullptr)
			newNode->left = cloneTree(node->left, newNode, owner);
		if (node->right != nullptr)
			newNode->right = cloneTree(node->right, newNode, owner);

		return newNode;
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename TLambda>
	typename BaseConcurencyMap<THeap, TKey, TValue>::Node* BaseConcurencyMap<THeap, TKey, TValue>::cloneTree(Node* node, Node* parent, BaseConcurencyMap* owner, TLambda&& lambda) const
	{
		Node* newNode = new Node(lambda(node->data), parent, owner);
		newNode->height = node->height;

		if (node->left != nullptr)
			newNode->left = cloneTree(node->left, newNode, owner, lambda);
		if (node->right != nullptr)
			newNode->right = cloneTree(node->right, newNode, owner, lambda);

		return newNode;
	}

	template<typename THeap, typename TKey, typename TValue>
	typename BaseConcurencyMap<THeap, TKey, TValue>::Node* BaseConcurencyMap<THeap, TKey, TValue>::Next(Node* node)
	{
		if (node == nullptr)
			Internals::Exceptions::ThrowNullPointException(SourcePosition);

		if (node->This != nullptr) 
		{
			auto _ = Threading::SharedScoppedLock(node->This->lock);

			--node->RefCount;
			if (node->right != nullptr)
			{
				Node* ret = FindMin(node->right);
				++ret->RefCount++;
				return ret;
			}

			Node* father = node->father;
			while (father != nullptr && node == father->right)
			{
				node = father;
				father = father->father;
			}

			if (father != nullptr)
				++father->RefCount;

			return father;
		}
		else 
		{
			if (--node->RefCount == 0)
			{
				delete node;
			}
			return nullptr;
		}
	}

	template<typename THeap, typename TKey, typename TValue>
	typename BaseConcurencyMap<THeap, TKey, TValue>::Node* BaseConcurencyMap<THeap, TKey, TValue>::Prev(Node* node)
	{
		if (node == nullptr)
			Internals::Exceptions::ThrowNullPointException(SourcePosition);

		if (node->This != nullptr) 
		{
			auto _ = Threading::SharedScoppedLock(node->This->lock);

			--node->RefCount;
			if (node->left != nullptr)
			{
				Node* ret = FindMax(node->left);
				++ret->RefCount;
				return ret;
			}

			Node* father = node->father;
			while (father != nullptr && node == father->left)
			{
				node = father;
				father = father->father;
			}

			if (father != nullptr)
				++father->RefCount;

			return father;
		}
		else 
		{
			if (--node->RefCount == 0) {
				delete node;
			}
			return nullptr;
		}
	}

	template<typename THeap, typename TKey, typename TValue>
	typename BaseConcurencyMap<THeap, TKey, TValue>::Node* BaseConcurencyMap<THeap, TKey, TValue>::moveTree(Node* node, BaseConcurencyMap* owner) {
		if (node->left != nullptr)
			node->left = moveTree(node->left, owner);

		if (node->right != nullptr)
			node->right = moveTree(node->right, owner);

		node->This = owner;
		return node;
	}

#pragma region ConstIterator
	template<typename THeap, typename TKey, typename TValue>
	inline BaseConcurencyMap<THeap, TKey, TValue>::ConstIterator::ConstIterator(Node* item): item(item)
	{
		if (item != nullptr) ++item->RefCount;
	};

	template<typename THeap, typename TKey, typename TValue>
	inline BaseConcurencyMap<THeap, TKey, TValue>::ConstIterator& BaseConcurencyMap<THeap, TKey, TValue>::ConstIterator::operator++()
	{
		this->item = Next(item);
		return *this;
	}

	template<typename THeap, typename TKey, typename TValue>
	inline BaseConcurencyMap<THeap, TKey, TValue>::ConstIterator& BaseConcurencyMap<THeap, TKey, TValue>::ConstIterator::operator--()
	{
		this->item = Prev(item);
		return *this;
	}

	template<typename THeap, typename TKey, typename TValue>
	inline BaseConcurencyMap<THeap, TKey, TValue>::ConstIterator& BaseConcurencyMap<THeap, TKey, TValue>::ConstIterator::operator++(int)
	{
		return ConstIterator(Next(item));
	}

	template<typename THeap, typename TKey, typename TValue>
	inline BaseConcurencyMap<THeap, TKey, TValue>::ConstIterator& BaseConcurencyMap<THeap, TKey, TValue>::ConstIterator::operator--(int)
	{
		return ConstIterator(Prev(item));
	}

	template<typename THeap, typename TKey, typename TValue>
	inline bool BaseConcurencyMap<THeap, TKey, TValue>::ConstIterator::operator!=(const ConstIterator& other)
	{
		if (this->item == nullptr && other.item == nullptr)
			return false;
		else if (this->item == nullptr || other.item == nullptr)
			return true;
		else
			return this->item->key != other.item->key;
	}

	template<typename THeap, typename TKey, typename TValue>
	inline bool BaseConcurencyMap<THeap, TKey, TValue>::ConstIterator::operator!=(NullptrT)
	{
		return this->item != nullptr;
	}

	template<typename THeap, typename TKey, typename TValue>
	inline const auto* const BaseConcurencyMap<THeap, TKey, TValue>::ConstIterator::operator->()
	{
		return &this->item->data;
	};

	template<typename THeap, typename TKey, typename TValue>
	inline const auto& BaseConcurencyMap<THeap, TKey, TValue>::ConstIterator::operator*()
	{
		return this->item->data;
	}

	template<typename THeap, typename TKey, typename TValue>
	inline const auto* const BaseConcurencyMap<THeap, TKey, TValue>::ConstIterator::operator->() const
	{
		return &this->item->data;
	};

	template<typename THeap, typename TKey, typename TValue>
	inline const auto& BaseConcurencyMap<THeap, TKey, TValue>::ConstIterator::operator*() const
	{
		return this->item->data;
	}

#pragma region Ctor, Dtor, copy operators
	template<typename THeap, typename TKey, typename TValue>
	inline BaseConcurencyMap<THeap, TKey, TValue>::ConstIterator::ConstIterator(const ConstIterator& iterator): item(iterator.item)
	{
		if (item != nullptr)
			++item->RefCount;
	}

	template<typename THeap, typename TKey, typename TValue>
	inline BaseConcurencyMap<THeap, TKey, TValue>::ConstIterator::ConstIterator(ConstIterator&& iterator): item(iterator.item)
	{
		iterator.item = nullptr;
	}

	template<typename THeap, typename TKey, typename TValue>
	inline BaseConcurencyMap<THeap, TKey, TValue>::ConstIterator& BaseConcurencyMap<THeap, TKey, TValue>::ConstIterator::operator=(const ConstIterator& iterator)
	{
		this->~ConstIterator();
		this->ConstIterator::ConstIterator(iterator);
		return *this;
	}

	template<typename THeap, typename TKey, typename TValue>
	inline BaseConcurencyMap<THeap, TKey, TValue>::ConstIterator& BaseConcurencyMap<THeap, TKey, TValue>::ConstIterator::operator=(ConstIterator&& iterator)
	{
		this->~ConstIterator();
		this->ConstIterator::ConstIterator(MoveRValue(iterator));
		return *this;
	}

	template<typename THeap, typename TKey, typename TValue>
	inline BaseConcurencyMap<THeap, TKey, TValue>::ConstIterator::~ConstIterator()
	{
		if (item != nullptr)
		{
			if (--item->RefCount == 0)
			{
				delete item;
			}
			item = nullptr;
		}
	}
#pragma endregion
#pragma endregion

	template<typename THeap, typename TKey, typename TValue>
	typename BaseConcurencyMap<THeap, TKey, TValue>::ConstIterator BaseConcurencyMap<THeap, TKey, TValue>::GetRoot() const
	{
		auto _ = Threading::SharedScoppedLock(this->lock);

		return ConstIterator(FindMin(this->root));
	}

	template<typename THeap, typename TKey, typename TValue>
	typename BaseConcurencyMap<THeap, TKey, TValue>::ConstIterator BaseConcurencyMap<THeap, TKey, TValue>::GetLast() const
	{
		auto _ = Threading::SharedScoppedLock(this->lock);

		return ConstIterator(FindMax(this->root));
	}

	template<typename THeap, typename TKey, typename TValue>
	typename BaseConcurencyMap<THeap, TKey, TValue>::ConstIterator BaseConcurencyMap<THeap, TKey, TValue>::begin() const
	{
		auto _ = Threading::SharedScoppedLock(this->lock);

		return GetRoot();
	}

	template<typename THeap, typename TKey, typename TValue>
	typename BaseConcurencyMap<THeap, TKey, TValue>::ConstIterator BaseConcurencyMap<THeap, TKey, TValue>::end() const
	{
		return ConstIterator(nullptr);
	}

#pragma region Iterator
	template<typename THeap, typename TKey, typename TValue>
	inline BaseConcurencyMap<THeap, TKey, TValue>::Iterator::Iterator(Node* item) : item(item)
	{
		if (item != nullptr) ++item->RefCount;
	};

	template<typename THeap, typename TKey, typename TValue>
	inline BaseConcurencyMap<THeap, TKey, TValue>::Iterator& BaseConcurencyMap<THeap, TKey, TValue>::Iterator::operator++()
	{
		this->item = Next(item);
		return *this;
	}

	template<typename THeap, typename TKey, typename TValue>
	inline BaseConcurencyMap<THeap, TKey, TValue>::Iterator& BaseConcurencyMap<THeap, TKey, TValue>::Iterator::operator--()
	{
		this->item = Prev(item);
		return *this;
	}

	template<typename THeap, typename TKey, typename TValue>
	inline BaseConcurencyMap<THeap, TKey, TValue>::Iterator& BaseConcurencyMap<THeap, TKey, TValue>::Iterator::operator++(int)
	{
		return Iterator(Next(item));
	}

	template<typename THeap, typename TKey, typename TValue>
	inline BaseConcurencyMap<THeap, TKey, TValue>::Iterator& BaseConcurencyMap<THeap, TKey, TValue>::Iterator::operator--(int)
	{
		return Iterator(Prev(item));
	}

	template<typename THeap, typename TKey, typename TValue>
	inline BaseConcurencyMap<THeap, TKey, TValue>::Iterator::operator typename BaseConcurencyMap<THeap, TKey, TValue>::ConstIterator() const
	{
		return ConstIterator(item);
	}

	template<typename THeap, typename TKey, typename TValue>
	inline bool BaseConcurencyMap<THeap, TKey, TValue>::Iterator::operator!=(const Iterator& other)
	{
		if (this->item == nullptr && other.item == nullptr)
			return false;
		else if (this->item == nullptr || other.item == nullptr)
			return true;
		else
			return this->item->key != other.item->key;
	}

	template<typename THeap, typename TKey, typename TValue>
	inline bool BaseConcurencyMap<THeap, TKey, TValue>::Iterator::operator!=(NullptrT)
	{
		return this->item != nullptr;
	}

	template<typename THeap, typename TKey, typename TValue>
	inline auto* BaseConcurencyMap<THeap, TKey, TValue>::Iterator::operator->()
	{
		return &this->item->data;
	};

	template<typename THeap, typename TKey, typename TValue>
	inline auto& BaseConcurencyMap<THeap, TKey, TValue>::Iterator::operator*()
	{
		return this->item->data;
	}

	template<typename THeap, typename TKey, typename TValue>
	inline const auto* const BaseConcurencyMap<THeap, TKey, TValue>::Iterator::operator->() const
	{
		return &this->item->data;
	};

	template<typename THeap, typename TKey, typename TValue>
	inline const auto& BaseConcurencyMap<THeap, TKey, TValue>::Iterator::operator*() const
	{
		return this->item->data;
	}

#pragma region Ctor, Dtor, copy operators
	template<typename THeap, typename TKey, typename TValue>
	inline BaseConcurencyMap<THeap, TKey, TValue>::Iterator::Iterator(const Iterator& iterator): item(iterator.item)
	{
		if (item != nullptr)
			++item->RefCount;
	}

	template<typename THeap, typename TKey, typename TValue>
	inline BaseConcurencyMap<THeap, TKey, TValue>::Iterator::Iterator(Iterator&& iterator): item(iterator.item)
	{
		iterator.item = nullptr;
	}

	template<typename THeap, typename TKey, typename TValue>
	inline BaseConcurencyMap<THeap, TKey, TValue>::Iterator& BaseConcurencyMap<THeap, TKey, TValue>::Iterator::operator=(const Iterator& iterator)
	{
		this->~Iterator();
		this->Iterator::Iterator(iterator);
		return *this;
	}

	template<typename THeap, typename TKey, typename TValue>
	inline BaseConcurencyMap<THeap, TKey, TValue>::Iterator& BaseConcurencyMap<THeap, TKey, TValue>::Iterator::operator=(Iterator&& iterator)
	{
		this->~Iterator();
		this->Iterator::Iterator(MoveRValue(iterator));
		return *this;
	}

	template<typename THeap, typename TKey, typename TValue>
	inline BaseConcurencyMap<THeap, TKey, TValue>::Iterator::~Iterator()
	{
		if (item != nullptr)
		{
			if (--item->RefCount == 0)
			{
				delete item;
			}
			item = nullptr;
		}
	}
#pragma endregion
#pragma endregion

	template<typename THeap, typename TKey, typename TValue>
	BaseConcurencyMap<THeap, TKey, TValue>::Iterator BaseConcurencyMap<THeap, TKey, TValue>::GetRoot()
	{
		auto _ = Threading::SharedScoppedLock(this->lock);

		return Iterator(FindMin(this->root));
	}

	template<typename THeap, typename TKey, typename TValue>
	BaseConcurencyMap<THeap, TKey, TValue>::Iterator BaseConcurencyMap<THeap, TKey, TValue>::GetLast()
	{
		auto _ = Threading::SharedScoppedLock(this->lock);

		return Iterator(FindMax(this->root));
	}

	template<typename THeap, typename TKey, typename TValue>
	BaseConcurencyMap<THeap, TKey, TValue>::Iterator BaseConcurencyMap<THeap, TKey, TValue>::begin()
	{
		return GetRoot();
	}

	template<typename THeap, typename TKey, typename TValue>
	BaseConcurencyMap<THeap, TKey, TValue>::Iterator BaseConcurencyMap<THeap, TKey, TValue>::end()
	{
		return Iterator(nullptr);
	}

	template<typename THeap, typename TKey, typename TValue>
	BaseConcurencyMap<THeap, TKey, TValue>::~BaseConcurencyMap()
	{
		auto _ = Threading::ExclusiveScoppedLock(this->lock);

		DestroyTree(this->root);
		this->root = nullptr;
	}

	template<typename THeap, typename TKey, typename TValue>
	BaseConcurencyMap<THeap, TKey, TValue>::BaseConcurencyMap(InitilizerList<BasePair<THeap, TKey, TValue>>&& pairs): root(nullptr), count(), lock()
	{
		for (auto&& it : pairs)
		{
			Insert(MoveRValue(it));
		}
	}

	template<typename THeap, typename TKey, typename TValue>
	BaseConcurencyMap<THeap, TKey, TValue>::BaseConcurencyMap(const BaseConcurencyMap& map): lock(), root(nullptr)
	{
		auto _ = Threading::SharedScoppedLock(map.lock);

		this->count = map.count;
		this->root = copyTree(map.root, nullptr, this);
	}

	template<typename THeap, typename TKey, typename TValue>
	BaseConcurencyMap<THeap, TKey, TValue>::BaseConcurencyMap(BaseConcurencyMap&& map): lock(), root(nullptr)
	{
		auto _ = Threading::ExclusiveScoppedLock(map.lock);

		this->count = map.count;
		this->root = map.root;
		map.root = nullptr;
		map.count = 0;
		map.moveTree(this->root, this);
	}

	template<typename THeap, typename TKey, typename TValue>
	BaseConcurencyMap<THeap, TKey, TValue>& BaseConcurencyMap<THeap, TKey, TValue>::operator=(const BaseConcurencyMap& original)
	{
		this->~BaseConcurencyMap();
		this->BaseConcurencyMap::BaseConcurencyMap(original);
		return *this;
	}

	template<typename THeap, typename TKey, typename TValue>
	BaseConcurencyMap<THeap, TKey, TValue>& BaseConcurencyMap<THeap, TKey, TValue>::operator=(BaseConcurencyMap&& original)
	{
		this->~BaseConcurencyMap();
		this->BaseConcurencyMap::BaseConcurencyMap(MoveRValue(original));
		return *this;
	}

	template<typename THeap, typename TKey, typename TValue>
	inline SizeT BaseConcurencyMap<THeap, TKey, TValue>::GetCount() const
	{
		return this->count.Value;
	}

	template<typename THeap, typename TKey, typename TValue>
	void BaseConcurencyMap<THeap, TKey, TValue>::Insert(const BasePair<THeap, TKey, TValue>& pair)
	{
		auto _ = Threading::ExclusiveScoppedLock(this->lock);
		
		this->root = Insert(this->root, pair, nullptr);
		++this->count;
	}

	template<typename THeap, typename TKey, typename TValue>
	void BaseConcurencyMap<THeap, TKey, TValue>::Insert(BasePair<THeap, TKey, TValue>&& pair)
	{
		auto _ = Threading::ExclusiveScoppedLock(this->lock);

		this->root = Insert(this->root, pair, nullptr);
		++this->count;
	}

	template<typename THeap, typename TKey, typename TValue>
	void BaseConcurencyMap<THeap, TKey, TValue>::Insert(const TKey& key, const TValue& value)
	{
		auto _ = Threading::ExclusiveScoppedLock(this->lock);

		this->root = Insert(this->root, key, value, nullptr);
		++this->count;
	}

	template<typename THeap, typename TKey, typename TValue>
	void BaseConcurencyMap<THeap, TKey, TValue>::Insert(TKey&& key, const TValue& value)
	{
		auto _ = Threading::ExclusiveScoppedLock(this->lock);

		this->root = Insert(this->root, MoveRValue(key), value, nullptr);
		++this->count;
	}

	template<typename THeap, typename TKey, typename TValue>
	void BaseConcurencyMap<THeap, TKey, TValue>::Insert(const TKey& key, TValue&& value)
	{
		auto _ = Threading::ExclusiveScoppedLock(this->lock);

		this->root = Insert(this->root, key, MoveRValue(value), nullptr);
		++this->count;
	}

	template<typename THeap, typename TKey, typename TValue>
	void BaseConcurencyMap<THeap, TKey, TValue>::Insert(TKey&& key, TValue&& value)
	{
		auto _ = Threading::ExclusiveScoppedLock(this->lock);

		this->root = Insert(this->root, MoveRValue(key), MoveRValue(value), nullptr);
		++this->count;
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename UKey, typename V>
	void BaseConcurencyMap<THeap, TKey, TValue>::Delete(const UKey& key)
	{
		auto _ = Threading::ExclusiveScoppedLock(this->lock);

		--this->count;
		this->root = Remove(this->root, key);
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename UKey, typename V>
	TValue& BaseConcurencyMap<THeap, TKey, TValue>::operator[](const UKey& key)
	{
		auto _ = Threading::SharedScoppedLock(this->lock);

		auto node = Find(this->root, key);

		if (node == nullptr)
			Internals::Exceptions::ThrowNotFound(SourcePosition);

		return node->value;
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename UKey> requires TypeTraits::IsComparableV<TKey, UKey>
	const TValue& BaseConcurencyMap<THeap, TKey, TValue>::operator[](const UKey& key) const
	{
		auto _ = Threading::SharedScoppedLock(this->lock);

		auto node = Find(this->root, key);

		if (node == nullptr)
			Internals::Exceptions::ThrowNotFound(SourcePosition);

		return node->value;
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename UKey> requires TypeTraits::IsComparableV<TKey, UKey>
	inline BaseConcurencyMap<THeap, TKey, TValue>::Iterator BaseConcurencyMap<THeap, TKey, TValue>::FindAsIterator(const UKey& key)
	{
		auto _ = Threading::SharedScoppedLock(this->lock);

		return Iterator(Find(this->root, key));
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename UKey> requires TypeTraits::IsComparableV<TKey, UKey>
	inline BaseConcurencyMap<THeap, TKey, TValue>::ConstIterator BaseConcurencyMap<THeap, TKey, TValue>::FindAsIterator(const UKey& key) const
	{
		auto _ = Threading::SharedScoppedLock(this->lock);

		return ConstIterator(Find(this->root, key));
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename UKey, typename V>
	bool BaseConcurencyMap<THeap, TKey, TValue>::IsExist(const UKey& key) const
	{
		auto _ = Threading::SharedScoppedLock(this->lock);

		return Find(this->root, key) != nullptr;
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename UKey, typename V>
	bool BaseConcurencyMap<THeap, TKey, TValue>::IsExist(UKey&& key) const
	{
		auto _ = Threading::SharedScoppedLock(this->lock);
		return Find(this->root, key) != nullptr;
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename> requires (Concepts::IsClonable<TKey>&& Concepts::IsClonable<TValue>)
	inline BaseConcurencyMap<THeap, TKey, TValue> BaseConcurencyMap<THeap, TKey, TValue>::Clone() const
	{
		BaseConcurencyMap result;
		result.count = this->count;
		result.root = cloneTree(this->root, nullptr, &result);
		return result;
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename TLambda>
	requires (Concepts::IsInvocableWithResult<TLambda, BasePair<THeap, TKey, TValue>, const BasePair<THeap, TKey, TValue>&>)
	inline BaseConcurencyMap<THeap, TKey, TValue> BaseConcurencyMap<THeap, TKey, TValue>::Clone() const
	{
		BaseConcurencyMap result;
		result.count = this->count;
		result.root = cloneTree(this->root, nullptr, &result);
		return result;
	}

#pragma region For Each Pair
	template<typename THeap, typename TKey, typename TValue>
	template<typename Lambda>
	void BaseConcurencyMap<THeap, TKey, TValue>::ForEachImplWithIndex(Node* node, SizeT& Index, Lambda& lambda)
	{
		if (node->left != nullptr)
			ForEachImplWithIndex(node->left, Index, lambda);

		lambda(node->data, Index); Index++;

		if (node->right != nullptr)
			ForEachImplWithIndex(node->right, Index, lambda);
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename Lambda>
	void BaseConcurencyMap<THeap, TKey, TValue>::ForEachImplWithoutIndex(Node* node, Lambda& lambda)
	{
		if (node->left != nullptr)
			ForEachImplWithoutIndex(node->left, lambda);

		lambda(node->data);

		if (node->right != nullptr)
			ForEachImplWithoutIndex(node->right, lambda);
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename TLambda>
	const BaseConcurencyMap<THeap, TKey, TValue>& BaseConcurencyMap<THeap, TKey, TValue>::ForEachImpl(TLambda&& lambda) const
	{
		if constexpr (TypeTraits::IsInvocable<TLambda, const BasePair<THeap, TKey, TValue>&>)
		{
			auto _ = Threading::SharedScoppedLock(this->lock);

			auto node = this->root;
			if (node != nullptr)
			{
				ForEachImplWithoutIndex(node, lambda);
			}

			return *this;
		}
		else if constexpr (TypeTraits::IsInvocable<TLambda, const BasePair<THeap, TKey, TValue>&, SizeT>)
		{
			auto _ = Threading::SharedScoppedLock(this->lock);

			auto node = this->root;
			if (node != nullptr)
			{
				SizeT index = 0;
				ForEachImplWithIndex(node, index, lambda);
			}
			return *this;
		}
		else
		{
			static_assert(TypeTraits::IsInvocable<TLambda, const BasePair<THeap, TKey, TValue>&> ||
				TypeTraits::IsInvocable<TLambda, const BasePair<THeap, TKey, TValue>&, SizeT>);
		}
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename TLambda>
	BaseConcurencyMap<THeap, TKey, TValue>& BaseConcurencyMap<THeap, TKey, TValue>::ForEachImpl(TLambda&& lambda)
	{
		if constexpr (TypeTraits::IsInvocable<TLambda, BasePair<THeap, TKey, TValue>&> ||
			TypeTraits::IsInvocable<TLambda, const BasePair<THeap, TKey, TValue>&>)
		{
			auto _ = Threading::SharedScoppedLock(this->lock);

			auto node = this->root;
			if (node != nullptr)
			{
				ForEachImplWithoutIndex(node, lambda);
			}

			return *this;
		}
		else if constexpr (TypeTraits::IsInvocable<TLambda, BasePair<THeap, TKey, TValue>&, SizeT> ||
			TypeTraits::IsInvocable<TLambda, const BasePair<THeap, TKey, TValue>&, SizeT>)
		{
			auto _ = Threading::SharedScoppedLock(this->lock);

			auto node = this->root;
			if (node != nullptr)
			{
				SizeT index = 0;
				ForEachImplWithIndex(node, index, lambda);
			}

			return *this;
		}
		else
		{
			static_assert(TypeTraits::IsInvocable<TLambda, BasePair<THeap, TKey, TValue>&> ||
				TypeTraits::IsInvocable<TLambda, const BasePair<THeap, TKey, TValue>&> ||
				TypeTraits::IsInvocable<TLambda, BasePair<THeap, TKey, TValue>&, SizeT> ||
				TypeTraits::IsInvocable<TLambda, const BasePair<THeap, TKey, TValue>&, SizeT>);
		}
	}
#pragma endregion

#pragma region For Each Key
	template<typename THeap, typename TKey, typename TValue>
	template<typename Lambda>
	void BaseConcurencyMap<THeap, TKey, TValue>::ForEachKeyImplWithIndex(Node* node, SizeT& Index, Lambda& lambda)
	{
		if (node->left != nullptr)
			ForEachKeyImplWithIndex(node->left, Index, lambda);

		lambda(node->data.Key, Index); Index++;

		if (node->right != nullptr)
			ForEachKeyImplWithIndex(node->right, Index, lambda);
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename Lambda>
	void BaseConcurencyMap<THeap, TKey, TValue>::ForEachKeyImplWithoutIndex(Node* node, Lambda& lambda)
	{
		if (node->left != nullptr)
			ForEachKeyImplWithoutIndex(node->left, lambda);

		lambda(node->data.Key);

		if (node->right != nullptr)
			ForEachKeyImplWithoutIndex(node->right, lambda);
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename TLambda>
	BaseConcurencyMap<THeap, TKey, TValue>& BaseConcurencyMap<THeap, TKey, TValue>::ForEachKeyImpl(TLambda&& lambda)
	{
		if constexpr (TypeTraits::IsInvocable<TLambda, const TKey&>)
		{
			auto _ = Threading::SharedScoppedLock(this->lock);

			auto node = this->root;
			if (node != nullptr)
			{
				ForEachKeyImplWithoutIndex(node, lambda);
			}

			return *this;
		}
		else if constexpr (TypeTraits::IsInvocable<TLambda, const TKey&, SizeT>)
		{
			auto _ = Threading::SharedScoppedLock(this->lock);

			auto node = this->root;
			if (node != nullptr)
			{
				SizeT index = 0;
				ForEachKeyImplWithIndex(node, index, lambda);
			}

			return *this;
		}
		else
		{
			static_assert(TypeTraits::IsInvocable<TLambda, const TKey&> ||
				TypeTraits::IsInvocable<TLambda, const TKey&, SizeT>);
		}
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename TLambda>
	const BaseConcurencyMap<THeap, TKey, TValue>& BaseConcurencyMap<THeap, TKey, TValue>::ForEachKeyImpl(TLambda&& lambda) const
	{
		if constexpr (TypeTraits::IsInvocable<TLambda, const TKey&>)
		{
			auto _ = Threading::SharedScoppedLock(this->lock);

			auto node = this->root;
			if (node != nullptr)
			{
				ForEachKeyImplWithoutIndex(node, lambda);
			}

			return *this;
		}
		else if constexpr (TypeTraits::IsInvocable<TLambda, const TKey&, SizeT>)
		{
			auto _ = Threading::SharedScoppedLock(this->lock);

			auto node = this->root;
			if (node != nullptr)
			{
				SizeT index = 0;
				ForEachKeyImplWithIndex(node, index, lambda);
			}

			return *this;
		}
		else
		{
			static_assert(TypeTraits::IsInvocable<TLambda, const TKey&> ||
				TypeTraits::IsInvocable<TLambda, const TKey&, SizeT>);
		}
	}
#pragma endregion

#pragma region For Each Value
	template<typename THeap, typename TKey, typename TValue>
	template<typename Lambda>
	void BaseConcurencyMap<THeap, TKey, TValue>::ForEachValueImplWithIndex(Node* node, SizeT& Index, Lambda& lambda)
	{
		if (node->left != nullptr)
			ForEachValueImplWithIndex(node->left, Index, lambda);

		lambda(node->data.Value, Index); Index++;

		if (node->right != nullptr)
			ForEachValueImplWithIndex(node->right, Index, lambda);
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename Lambda>
	void BaseConcurencyMap<THeap, TKey, TValue>::ForEachValueImplWithoutIndex(Node* node, Lambda& lambda)
	{
		if (node->left != nullptr)
			ForEachValueImplWithoutIndex(node->left, lambda);

		lambda(node->data.Value);

		if (node->right != nullptr)
			ForEachValueImplWithoutIndex(node->right, lambda);
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename TLambda>
	const BaseConcurencyMap<THeap, TKey, TValue>& BaseConcurencyMap<THeap, TKey, TValue>::ForEachValueImpl(TLambda&& lambda) const
	{
		if constexpr (TypeTraits::IsInvocable<TLambda, const TValue&>)
		{
			auto _ = Threading::SharedScoppedLock(this->lock);

			auto node = this->root;
			if (node != nullptr)
			{
				ForEachValueImplWithoutIndex(node, lambda);
			}

			return *this;
		}
		else if constexpr (TypeTraits::IsInvocable<TLambda, const TValue&, SizeT>)
		{
			auto _ = Threading::SharedScoppedLock(this->lock);

			auto node = this->root;
			if (node != nullptr)
			{
				SizeT index = 0;
				ForEachValueImplWithIndex(node, index, lambda);
			}

			return *this;
		}
		else
		{
			static_assert(TypeTraits::IsInvocable<TLambda, const TValue&> ||
				TypeTraits::IsInvocable<TLambda, const TValue&, SizeT>);
		}
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename TLambda>
	BaseConcurencyMap<THeap, TKey, TValue>& BaseConcurencyMap<THeap, TKey, TValue>::ForEachValueImpl(TLambda&& lambda)
	{
		if constexpr (TypeTraits::IsInvocable<TLambda, TValue&> ||
			TypeTraits::IsInvocable<TLambda, const TValue&>)
		{
			auto _ = Threading::SharedScoppedLock(this->lock);

			auto node = this->root;
			if (node != nullptr)
			{
				ForEachValueImplWithoutIndex(node, lambda);
			}

			return *this;
		}
		else if constexpr (TypeTraits::IsInvocable<TLambda, TValue&, SizeT> ||
			TypeTraits::IsInvocable<TLambda, const TValue&, SizeT>)
		{
			auto _ = Threading::SharedScoppedLock(this->lock);

			auto node = this->root;
			if (node != nullptr)
			{
				SizeT index = 0;
				ForEachValueImplWithIndex(node, index, lambda);
			}

			return *this;
		}
		else
		{
			static_assert(TypeTraits::IsInvocable<TLambda, TValue&> ||
				TypeTraits::IsInvocable<TLambda, const TValue&> ||
				TypeTraits::IsInvocable<TLambda, TValue&, SizeT> ||
				TypeTraits::IsInvocable<TLambda, const TValue&, SizeT>);
		}
	}
#pragma endregion

	template<typename THeap, typename TKey, typename TValue>
	template<typename Lambda>
	typename BaseConcurencyMap<THeap, TKey, TValue>::Node* BaseConcurencyMap<THeap, TKey, TValue>::DestroyTreeWithLambdaPairValues(Node* root, Lambda& lambda) //-V688
	{
		if (root == nullptr)
			return nullptr;

		if (root->left != nullptr)
		{
			root->left = DestroyTreeWithLambdaPairValues(root->left, lambda);
		}

		lambda(MoveRValue(root->data));

		if (root->right != nullptr)
		{
			root->right = DestroyTreeWithLambdaPairValues(root->right, lambda);
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

		return nullptr;
	}

	template<typename THeap, typename TKey, typename TValue>
	BaseConcurencyMap<THeap, TKey, TValue>& BaseConcurencyMap<THeap, TKey, TValue>::RemoveAllImpl()
	{
		auto _ = Threading::ExclusiveScoppedLock(this->lock);

		this->root = DestroyTree(this->root);
		this->count = 0;
		return *this;
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename Lambda>
	BaseConcurencyMap<THeap, TKey, TValue>& BaseConcurencyMap<THeap, TKey, TValue>::RemoveAllWithImpl(Lambda&& lambda)
	{
		auto _ = Threading::SharedScoppedLock(this->lock);

		this->root = DestroyTreeWithLambdaPairValues(this->root, lambda);
		this->count = 0;

		return *this;
	}

	template<typename THeap, typename TKey, typename TValue>
	inline void BaseConcurencyMap<THeap, TKey, TValue>::UnsafeAdd(BasePair<THeap, TKey, TValue>&& pair)
	{
		Insert(MoveRValue(pair));
	}

	template<typename THeap, typename TKey, typename TValue>
	inline void BaseConcurencyMap<THeap, TKey, TValue>::UnsafeAdd(const BasePair<THeap, TKey, TValue>& pair)
	{
		Insert(pair);
	}
}

#pragma warning(pop)