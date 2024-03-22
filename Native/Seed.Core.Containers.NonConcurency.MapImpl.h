#pragma once
#include "Seed.Core.Containers.NonConcurency.Map.h"

#pragma warning(push)
// This internal implementation file which can use internal API functions
#pragma warning(disable: 4996)

namespace Seed::Core::Containers
{
	template<typename THeap, typename TKey, typename TValue>
	BaseMap<THeap, TKey, TValue>::~BaseMap()
	{
		DestroyTree(this->root);
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename UKey>
	typename BaseMap<THeap, TKey, TValue>::Node* BaseMap<THeap, TKey, TValue>::Find(Node* node, const UKey& key)
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
	UInt16 BaseMap<THeap, TKey, TValue>::Height(Node* node)
	{
		return node ? node->height : 0;
	}

	template<typename THeap, typename TKey, typename TValue>
	Int16 BaseMap<THeap, TKey, TValue>::BFactor(Node* node)
	{
		return  Height(node->right) - Height(node->left);
	}

	template<typename THeap, typename TKey, typename TValue>
	void BaseMap<THeap, TKey, TValue>::FixHeight(Node* node)
	{
		const UInt16 HLeft = Height(node->left);
		const UInt16 HRight = Height(node->right);

		node->height = Core::Max(HLeft, HRight) + 1;
	}

	template<typename THeap, typename TKey, typename TValue>
	typename BaseMap<THeap, TKey, TValue>::Node* BaseMap<THeap, TKey, TValue>::RotateRight(Node* node)
	{
		Node* left = node->left;
		Node* node_father = node->father;

		node->left = left->right;
		if (node->left != nullptr)
			node->left->father = node;

		left->right = node;
		if (left->right != nullptr)
			left->right->father = left;

		left->father = node_father;

		FixHeight(node);
		FixHeight(left);
		return left;
	}

	template<typename THeap, typename TKey, typename TValue>
	typename BaseMap<THeap, TKey, TValue>::Node* BaseMap<THeap, TKey, TValue>::RotateLeft(Node* node)
	{
		Node* right = node->right;
		Node* node_father = node->father;

		node->right = right->left;
		if (node->right != nullptr)
			node->right->father = node;

		right->left = node;
		if (right->left != nullptr)
			right->left->father = right;

		right->father = node_father;

		FixHeight(node);
		FixHeight(right);
		return right;
	}

	template<typename THeap, typename TKey, typename TValue>
	typename BaseMap<THeap, TKey, TValue>::Node* BaseMap<THeap, TKey, TValue>::Balance(Node* node)
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
	typename BaseMap<THeap, TKey, TValue>::Node* BaseMap<THeap, TKey, TValue>::Insert(Node* node, const TKey& key, const TValue& value, Node* father)
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
	typename BaseMap<THeap, TKey, TValue>::Node* BaseMap<THeap, TKey, TValue>::Insert(Node* node, TKey&& key, const TValue& value, Node* father)
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
	typename BaseMap<THeap, TKey, TValue>::Node* BaseMap<THeap, TKey, TValue>::Insert(Node* node, const TKey& key, TValue&& value, Node* father)
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
	typename BaseMap<THeap, TKey, TValue>::Node* BaseMap<THeap, TKey, TValue>::Insert(Node* node, TKey&& key, TValue&& value, Node* father)
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
	typename BaseMap<THeap, TKey, TValue>::Node* BaseMap<THeap, TKey, TValue>::Insert(Node* node, const BasePair<THeap, TKey, TValue>& pair, Node* father)
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
	typename BaseMap<THeap, TKey, TValue>::Node* BaseMap<THeap, TKey, TValue>::Insert(Node* node, BasePair<THeap, TKey, TValue>&& pair, Node* father)
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
	typename BaseMap<THeap, TKey, TValue>::Node* BaseMap<THeap, TKey, TValue>::FindMin(Node* node)
	{
		return node->left ? FindMin(node->left) : node;
	}

	template<typename THeap, typename TKey, typename TValue>
	typename BaseMap<THeap, TKey, TValue>::Node* BaseMap<THeap, TKey, TValue>::FindMax(Node* node)
	{
		return node->right ? FindMax(node->right) : node;
	}

	template<typename THeap, typename TKey, typename TValue>
	typename BaseMap<THeap, TKey, TValue>::Node* BaseMap<THeap, TKey, TValue>::RemoveMin(Node* node)
	{
		if (node->left == nullptr)
			return node->right;

		node->left = RemoveMin(node->left);
		return Balance(node);
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename UKey>
	typename BaseMap<THeap, TKey, TValue>::Node* BaseMap<THeap, TKey, TValue>::Remove(Node* node, const UKey& key)
	{
		if (!node) return nullptr;

		if (node->key > key)
			node->left = Remove(node->left, key);
		else if (node->key < key)
			node->right = Remove(node->right, key);
		else
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
	typename BaseMap<THeap, TKey, TValue>::Node* BaseMap<THeap, TKey, TValue>::DestroyTree(Node* root) //-V688
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
	typename BaseMap<THeap, TKey, TValue>::Node* BaseMap<THeap, TKey, TValue>::copyTree(Node* node, Node* parent, BaseMap* owner) const
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
	typename BaseMap<THeap, TKey, TValue>::Node* BaseMap<THeap, TKey, TValue>::cloneTree(Node* node, Node* parent, BaseMap* owner) const
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
	typename BaseMap<THeap, TKey, TValue>::Node* BaseMap<THeap, TKey, TValue>::cloneTree(Node* node, Node* parent, BaseMap* owner, TLambda&& lambda) const
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
	typename BaseMap<THeap, TKey, TValue>::Node* BaseMap<THeap, TKey, TValue>::Next(Node* node)
	{
		if (node == nullptr)
			Internals::Exceptions::ThrowNullPointException(SourcePosition);

		if (node->This != nullptr)
		{
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
	typename BaseMap<THeap, TKey, TValue>::Node* BaseMap<THeap, TKey, TValue>::Prev(Node* node)
	{
		if (node == nullptr)
			Internals::Exceptions::ThrowNullPointException(SourcePosition);

		if (node->This != nullptr)
		{
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
			if (--node->RefCount == 0)
			{
				delete node;
			}

			return nullptr;
		}
	}

	template<typename THeap, typename TKey, typename TValue>
	typename BaseMap<THeap, TKey, TValue>::Node* BaseMap<THeap, TKey, TValue>::moveTree(Node* node, BaseMap* owner)
	{
		if (node->left != nullptr)
			node->left = moveTree(node->left, owner);

		if (node->right != nullptr)
			node->right = moveTree(node->right, owner);

		node->This = owner;
		return node;
	}

	template<typename THeap, typename TKey, typename TValue>
	ForceInline BaseMap<THeap, TKey, TValue>::ConstIterator::ConstIterator(Node* item) : item(item)
	{
		if (item != nullptr) ++item->RefCount;
	};

	template<typename THeap, typename TKey, typename TValue>
	ForceInline BaseMap<THeap, TKey, TValue>::ConstIterator& BaseMap<THeap, TKey, TValue>::ConstIterator::operator++()
	{
		this->item = Next(item);
		return *this;
	}

	template<typename THeap, typename TKey, typename TValue>
	ForceInline BaseMap<THeap, TKey, TValue>::ConstIterator& BaseMap<THeap, TKey, TValue>::ConstIterator::operator--()
	{
		this->item = Prev(item);
		return *this;
	}

	template<typename THeap, typename TKey, typename TValue>
	ForceInline BaseMap<THeap, TKey, TValue>::ConstIterator& BaseMap<THeap, TKey, TValue>::ConstIterator::operator++(int)
	{
		return ConstIterator(Next(item));
	}

	template<typename THeap, typename TKey, typename TValue>
	ForceInline BaseMap<THeap, TKey, TValue>::ConstIterator& BaseMap<THeap, TKey, TValue>::ConstIterator::operator--(int)
	{
		return ConstIterator(Prev(item));
	}

	template<typename THeap, typename TKey, typename TValue>
	ForceInline bool BaseMap<THeap, TKey, TValue>::ConstIterator::operator!=(const ConstIterator& other)
	{
		if (this->item == nullptr && other.item == nullptr)
			return false;
		else if (this->item == nullptr || other.item == nullptr)
			return true;
		else
			return this->item->key != other.item->key;
	}

	template<typename THeap, typename TKey, typename TValue>
	ForceInline bool BaseMap<THeap, TKey, TValue>::ConstIterator::operator!=(NullptrT)
	{
		return this->item != nullptr;
	}

	template<typename THeap, typename TKey, typename TValue>
	ForceInline const auto* const BaseMap<THeap, TKey, TValue>::ConstIterator::operator->()
	{
		return &this->item->data;
	};

	template<typename THeap, typename TKey, typename TValue>
	ForceInline const auto& BaseMap<THeap, TKey, TValue>::ConstIterator::operator*()
	{
		return this->item->data;
	}

	template<typename THeap, typename TKey, typename TValue>
	ForceInline const BasePair<THeap, TKey, TValue>* const BaseMap<THeap, TKey, TValue>::ConstIterator::operator->() const
	{
		return &this->item->data;
	};

	template<typename THeap, typename TKey, typename TValue>
	ForceInline const auto& BaseMap<THeap, TKey, TValue>::ConstIterator::operator*() const
	{
		return this->item->data;
	}

#pragma region Ctor, Dtor, copy operators
	template<typename THeap, typename TKey, typename TValue>
	ForceInline BaseMap<THeap, TKey, TValue>::ConstIterator::ConstIterator(const ConstIterator& iterator): item(iterator.item)
	{
		if (item != nullptr)
			++item->RefCount;
	}

	template<typename THeap, typename TKey, typename TValue>
	ForceInline BaseMap<THeap, TKey, TValue>::ConstIterator::ConstIterator(ConstIterator&& iterator): item(iterator.item)
	{
		iterator.item = nullptr;
	}

	template<typename THeap, typename TKey, typename TValue>
	ForceInline BaseMap<THeap, TKey, TValue>::ConstIterator& BaseMap<THeap, TKey, TValue>::ConstIterator::operator=(const ConstIterator& iterator)
	{
		this->~ConstIterator();
		this->ConstIterator::ConstIterator(iterator);
		return *this;
	}

	template<typename THeap, typename TKey, typename TValue>
	ForceInline BaseMap<THeap, TKey, TValue>::ConstIterator& BaseMap<THeap, TKey, TValue>::ConstIterator::operator=(ConstIterator&& iterator)
	{
		this->~ConstIterator();
		this->ConstIterator::ConstIterator(MoveRValue(iterator));
		return *this;
	}

	template<typename THeap, typename TKey, typename TValue>
	ForceInline BaseMap<THeap, TKey, TValue>::ConstIterator::~ConstIterator()
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

	template<typename THeap, typename TKey, typename TValue>
	ForceInline BaseMap<THeap, TKey, TValue>::ConstIterator BaseMap<THeap, TKey, TValue>::GetRoot() const
	{
		return ConstIterator(FindMin(this->root));
	}
	
	template<typename THeap, typename TKey, typename TValue>
	ForceInline BaseMap<THeap, TKey, TValue>::ConstIterator BaseMap<THeap, TKey, TValue>::GetLast() const
	{
		return ConstIterator(FindMax(this->root));
	}

	template<typename THeap, typename TKey, typename TValue>
	ForceInline BaseMap<THeap, TKey, TValue>::ConstIterator BaseMap<THeap, TKey, TValue>::begin() const
	{
		return GetRoot();
	}

	template<typename THeap, typename TKey, typename TValue>
	ForceInline const BaseMap<THeap, TKey, TValue>::ConstIterator BaseMap<THeap, TKey, TValue>::end() const
	{
		return ConstIterator(nullptr);
	}

	template<typename THeap, typename TKey, typename TValue>
	ForceInline BaseMap<THeap, TKey, TValue>::Iterator::Iterator(Node* item): item(item)
	{
		if (item != nullptr) ++item->RefCount;
	};

	template<typename THeap, typename TKey, typename TValue>
	ForceInline BaseMap<THeap, TKey, TValue>::Iterator& BaseMap<THeap, TKey, TValue>::Iterator::operator++()
	{
		this->item = Next(item);
		return *this;
	}

	template<typename THeap, typename TKey, typename TValue>
	ForceInline BaseMap<THeap, TKey, TValue>::Iterator& BaseMap<THeap, TKey, TValue>::Iterator::operator--()
	{
		this->item = Prev(item);
		return *this;
	}

	template<typename THeap, typename TKey, typename TValue>
	ForceInline BaseMap<THeap, TKey, TValue>::Iterator& BaseMap<THeap, TKey, TValue>::Iterator::operator++(int)
	{
		return Iterator(Next(item));
	}

	template<typename THeap, typename TKey, typename TValue>
	ForceInline BaseMap<THeap, TKey, TValue>::Iterator& BaseMap<THeap, TKey, TValue>::Iterator::operator--(int)
	{
		return Iterator(Prev(item));
	}

	template<typename THeap, typename TKey, typename TValue>
	ForceInline bool BaseMap<THeap, TKey, TValue>::Iterator::operator==(const Iterator& other)
	{
		if (this->item == nullptr && other.item == nullptr)
			return true;
		else if (this->item == nullptr || other.item == nullptr)
			return false;
		else
			return this->item->key == other.item->key;
	}

	template<typename THeap, typename TKey, typename TValue>
	ForceInline bool BaseMap<THeap, TKey, TValue>::Iterator::operator==(NullptrT)
	{
		return this->item == nullptr;
	}

	template<typename THeap, typename TKey, typename TValue>
	ForceInline bool BaseMap<THeap, TKey, TValue>::Iterator::operator!=(const Iterator& other)
	{
		if (this->item == nullptr && other.item == nullptr)
			return false;
		else if (this->item == nullptr || other.item == nullptr)
			return true;
		else
			return this->item->key != other.item->key;
	}

	template<typename THeap, typename TKey, typename TValue>
	ForceInline bool BaseMap<THeap, TKey, TValue>::Iterator::operator!=(NullptrT)
	{
		return this->item != nullptr;
	}

	template<typename THeap, typename TKey, typename TValue>
	ForceInline auto* BaseMap<THeap, TKey, TValue>::Iterator::operator->()
	{
		return &this->item->data;
	};

	template<typename THeap, typename TKey, typename TValue>
	ForceInline auto& BaseMap<THeap, TKey, TValue>::Iterator::operator*()
	{
		return this->item->data;
	}

	template<typename THeap, typename TKey, typename TValue>
	ForceInline const BasePair<THeap, TKey, TValue>* const BaseMap<THeap, TKey, TValue>::Iterator::operator->() const
	{
		return &this->item->data;
	};

	template<typename THeap, typename TKey, typename TValue>
	ForceInline const auto& BaseMap<THeap, TKey, TValue>::Iterator::operator*() const
	{
		return this->item->data;
	}

	template<typename THeap, typename TKey, typename TValue>
	ForceInline BaseMap<THeap, TKey, TValue>::Iterator::operator typename BaseMap<THeap, TKey, TValue>::ConstIterator() const
	{
		return ConstIterator(item);
	}

#pragma region Ctor, Dtor, copy operators
	template<typename THeap, typename TKey, typename TValue>
	ForceInline BaseMap<THeap, TKey, TValue>::Iterator::Iterator(const Iterator& iterator): item(iterator.item)
	{
		if (item != nullptr)
			++item->RefCount;
	}

	template<typename THeap, typename TKey, typename TValue>
	ForceInline BaseMap<THeap, TKey, TValue>::Iterator::Iterator(Iterator&& iterator): item(iterator.item)
	{
		iterator.item = nullptr;
	}

	template<typename THeap, typename TKey, typename TValue>
	ForceInline BaseMap<THeap, TKey, TValue>::Iterator& BaseMap<THeap, TKey, TValue>::Iterator::operator=(const Iterator& iterator)
	{
		this->~Iterator();
		this->Iterator::Iterator(iterator);
		return *this;
	}

	template<typename THeap, typename TKey, typename TValue>
	ForceInline BaseMap<THeap, TKey, TValue>::Iterator& BaseMap<THeap, TKey, TValue>::Iterator::operator=(Iterator&& iterator)
	{
		this->~Iterator();
		this->Iterator::Iterator(MoveRValue(iterator));
		return *this;
	}

	template<typename THeap, typename TKey, typename TValue>
	ForceInline BaseMap<THeap, TKey, TValue>::Iterator::~Iterator()
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

	template<typename THeap, typename TKey, typename TValue>
	ForceInline BaseMap<THeap, TKey, TValue>::Iterator BaseMap<THeap, TKey, TValue>::GetRoot()
	{
		return Iterator(FindMin(this->root));
	}

	template<typename THeap, typename TKey, typename TValue>
	ForceInline BaseMap<THeap, TKey, TValue>::Iterator BaseMap<THeap, TKey, TValue>::GetLast()
	{
		return Iterator(FindMax(this->root));
	}

	template<typename THeap, typename TKey, typename TValue>
	ForceInline BaseMap<THeap, TKey, TValue>::Iterator BaseMap<THeap, TKey, TValue>::begin()
	{
		return GetRoot();
	}

	template<typename THeap, typename TKey, typename TValue>
	ForceInline BaseMap<THeap, TKey, TValue>::Iterator BaseMap<THeap, TKey, TValue>::end()
	{
		return Iterator(nullptr);
	}

	template<typename THeap, typename TKey, typename TValue>
	BaseMap<THeap, TKey, TValue>::BaseMap(InitilizerList<BasePair<THeap, TKey, TValue>>&& pairs): root(nullptr), count(0)
	{
		for (auto&& it : pairs)
		{
			Insert(MoveRValue(it));
		}
	}

	template<typename THeap, typename TKey, typename TValue>
	BaseMap<THeap, TKey, TValue>::BaseMap(const BaseMap& original): root(nullptr), count(original.count)
	{
		if (count != 0)
		{
			root = copyTree(original.root, nullptr, this);
		}
	}

	template<typename THeap, typename TKey, typename TValue>
	BaseMap<THeap, TKey, TValue>::BaseMap(BaseMap&& original): root(original.root), count(original.count)
	{
		original.root = nullptr;
		original.count = 0;

		if (root != nullptr)
			root = moveTree(root, this);
	}

	template<typename THeap, typename TKey, typename TValue>
	BaseMap<THeap, TKey, TValue>& BaseMap<THeap, TKey, TValue>::operator=(const BaseMap& original)
	{
		this->~BaseMap();
		this->BaseMap::BaseMap(original);
		return *this;
	}

	template<typename THeap, typename TKey, typename TValue>
	BaseMap<THeap, TKey, TValue>& BaseMap<THeap, TKey, TValue>::operator=(BaseMap&& original)
	{
		this->~BaseMap();
		this->BaseMap::BaseMap(MoveRValue(original));
		return *this;
	}

	template<typename THeap, typename TKey, typename TValue>
	ForceInline SizeT BaseMap<THeap, TKey, TValue>::GetCount() const
	{
		return this->count;
	}

	template<typename THeap, typename TKey, typename TValue>
	ForceInline void BaseMap<THeap, TKey, TValue>::Insert(const BasePair<THeap, TKey, TValue>& pair)
	{
		this->root = Insert(this->root, pair, nullptr);
		++this->count;
	}

	template<typename THeap, typename TKey, typename TValue>
	ForceInline void BaseMap<THeap, TKey, TValue>::Insert(BasePair<THeap, TKey, TValue>&& pair)
	{
		this->root = Insert(this->root, pair, nullptr);
		++this->count;
	}

	template<typename THeap, typename TKey, typename TValue>
	ForceInline void BaseMap<THeap, TKey, TValue>::Insert(const TKey& key, const TValue& value)
	{
		this->root = Insert(this->root, key, value, nullptr);
		++this->count;
	}

	template<typename THeap, typename TKey, typename TValue>
	ForceInline void BaseMap<THeap, TKey, TValue>::Insert(TKey&& key, const TValue& value)
	{
		this->root = Insert(this->root, MoveRValue(key), value, nullptr);
		++this->count;
	}

	template<typename THeap, typename TKey, typename TValue>
	ForceInline void BaseMap<THeap, TKey, TValue>::Insert(const TKey& key, TValue&& value)
	{
		this->root = Insert(this->root, key, MoveRValue(value), nullptr);
		++this->count;
	}

	template<typename THeap, typename TKey, typename TValue>
	ForceInline void BaseMap<THeap, TKey, TValue>::Insert(TKey&& key, TValue&& value)
	{
		this->root = Insert(this->root, MoveRValue(key), MoveRValue(value), nullptr);
		++this->count;
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename UKey> requires Concepts::HaveEqual<TKey, UKey>
	ForceInline void BaseMap<THeap, TKey, TValue>::Delete(const UKey& key)
	{
		--this->count;
		this->root = Remove(this->root, key);
	}

	template<typename THeap, typename TKey, typename TValue>
	BaseMap<THeap, TKey, TValue>& BaseMap<THeap, TKey, TValue>::operator+=(BasePair<THeap, TKey, TValue>&& pair)
	{
		auto [key, value] = MoveRValue(pair);
		Insert(MoveRValue(key), MoveRValue(value));
		return *this;
	}

	template<typename THeap, typename TKey, typename TValue>
	BaseMap<THeap, TKey, TValue>& BaseMap<THeap, TKey, TValue>::operator+=(const BasePair<THeap, TKey, TValue>& pair)
	{
		return *this += BasePair<THeap, TKey, TValue>(pair);
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename UKey> requires Concepts::HaveEqual<TKey, UKey>
	ForceInline TValue& BaseMap<THeap, TKey, TValue>::operator[](const UKey& key)
	{
		Node* node = Find(this->root, key);

		if (node == nullptr)
			Internals::Exceptions::ThrowNotFound(SourcePosition);
		else
			return node->value;
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename UKey> requires Concepts::HaveEqual<TKey, UKey>
	ForceInline const TValue& BaseMap<THeap, TKey, TValue>::operator[](const UKey& key) const
	{
		Node* node = Find(this->root, key);

		if (node == nullptr)
			Internals::Exceptions::ThrowNotFound(SourcePosition);
		else
			return node->value;
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename UKey> requires TypeTraits::IsComparableV<TKey, UKey>
	inline BaseMap<THeap, TKey, TValue>::Iterator BaseMap<THeap, TKey, TValue>::FindAsIterator(const UKey& key)
	{
		return Iterator(Find(this->root, key));
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename UKey> requires TypeTraits::IsComparableV<TKey, UKey>
	inline BaseMap<THeap, TKey, TValue>::ConstIterator BaseMap<THeap, TKey, TValue>::FindAsIterator(const UKey& key) const
	{
		return ConstIterator(Find(this->root, key));
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename UKey> requires Concepts::HaveEqual<TKey, UKey>
	ForceInline bool BaseMap<THeap, TKey, TValue>::IsExist(const UKey& key) const
	{
		return Find(this->root, key) != nullptr;
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename UKey> requires Concepts::HaveEqual<TKey, UKey>
	ForceInline bool BaseMap<THeap, TKey, TValue>::IsExist(UKey&& key) const
	{
		return Find(this->root, key) != nullptr;
	}

	template<typename THeap, typename TKey, typename TValue>
	ForceInline BaseMap<THeap, TKey, TValue> BaseMap<THeap, TKey, TValue>::Clone() const
	requires (Concepts::IsClonable<TKey>&& Concepts::IsClonable<TValue>)
	{
		BaseMap result;
		result.count = this->count;
		result.root = cloneTree(this->root, nullptr, &result);
		return result;
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename TLambda>
	requires (Concepts::IsInvocableWithResult<TLambda, BasePair<THeap, TKey, TValue>, const BasePair<THeap, TKey, TValue>&>)
	ForceInline BaseMap<THeap, TKey, TValue> BaseMap<THeap, TKey, TValue>::Clone() const
	{
		BaseMap result;
		result.count = this->count;
		result.root = cloneTree(this->root, nullptr, &result);
		return result;
	}

#pragma region ForEach Pair
	template<typename THeap, typename TKey, typename TValue>
	template<typename Lambda>
	void BaseMap<THeap, TKey, TValue>::ForEachImplWithIndex(Node* node, SizeT& Index, Lambda& lambda)
	{
		if (node->left != nullptr)
			ForEachImplWithIndex(node->left, Index, lambda);

		lambda(node->data, Index); Index++;

		if (node->right != nullptr)
			ForEachImplWithIndex(node->right, Index, lambda);
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename Lambda>
	void BaseMap<THeap, TKey, TValue>::ForEachImplWithoutIndex(Node* node, Lambda& lambda)
	{
		if (node->left != nullptr)
			ForEachImplWithoutIndex(node->left, lambda);

		lambda(node->data);

		if (node->right != nullptr)
			ForEachImplWithoutIndex(node->right, lambda);
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename TLambda>
	const BaseMap<THeap, TKey, TValue>& BaseMap<THeap, TKey, TValue>::ForEachImpl(TLambda&& lambda) const
	{
		if constexpr (TypeTraits::IsInvocable<TLambda, const BasePair<THeap, TKey, TValue>&>)
		{
			auto node = this->root;
			if (node != nullptr)
			{
				ForEachImplWithoutIndex(node, lambda);
			}
			return *this;
		}
		else if constexpr (TypeTraits::IsInvocable<TLambda, const BasePair<THeap, TKey, TValue>&, SizeT>)
		{
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
	BaseMap<THeap, TKey, TValue>& BaseMap<THeap, TKey, TValue>::ForEachImpl(TLambda&& lambda)
	{
		if constexpr (TypeTraits::IsInvocable<TLambda, BasePair<THeap, TKey, TValue>&> ||
			TypeTraits::IsInvocable<TLambda, const BasePair<THeap, TKey, TValue>&>)
		{
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
	void BaseMap<THeap, TKey, TValue>::ForEachKeyImplWithIndex(Node* node, SizeT& Index, Lambda& lambda)
	{
		if (node->left != nullptr)
			ForEachKeyImplWithIndex(node->left, Index, lambda);

		lambda(node->data.Key, Index); Index++;

		if (node->right != nullptr)
			ForEachKeyImplWithIndex(node->right, Index, lambda);
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename Lambda>
	void BaseMap<THeap, TKey, TValue>::ForEachKeyImplWithoutIndex(Node* node, Lambda& lambda)
	{
		if (node->left != nullptr)
			ForEachKeyImplWithoutIndex(node->left, lambda);

		lambda(node->data.Key);

		if (node->right != nullptr)
			ForEachKeyImplWithoutIndex(node->right, lambda);
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename TLambda>
	BaseMap<THeap, TKey, TValue>& BaseMap<THeap, TKey, TValue>::ForEachKeyImpl(TLambda&& lambda)
	{
		if constexpr (TypeTraits::IsInvocable<TLambda, const TKey&>)
		{
			auto node = this->root;
			if (node != nullptr)
			{
				ForEachKeyImplWithoutIndex(node, lambda);
			}
			return *this;
		}
		else if constexpr (TypeTraits::IsInvocable<TLambda, const TKey&, SizeT>)
		{
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
	const BaseMap<THeap, TKey, TValue>& BaseMap<THeap, TKey, TValue>::ForEachKeyImpl(TLambda&& lambda) const
	{
		if constexpr (TypeTraits::IsInvocable<TLambda, const TKey&>)
		{
			auto node = this->root;
			if (node != nullptr)
			{
				ForEachKeyImplWithoutIndex(node, lambda);
			}
			return *this;
		}
		else if constexpr (TypeTraits::IsInvocable<TLambda, const TKey&, SizeT>)
		{
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
	void BaseMap<THeap, TKey, TValue>::ForEachValueImplWithIndex(Node* node, SizeT& Index, Lambda& lambda)
	{
		if (node->left != nullptr)
			ForEachValueImplWithIndex(node->left, Index, lambda);

		lambda(node->data.Value, Index); Index++;

		if (node->right != nullptr)
			ForEachValueImplWithIndex(node->right, Index, lambda);
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename Lambda>
	void BaseMap<THeap, TKey, TValue>::ForEachValueImplWithoutIndex(Node* node, Lambda& lambda)
	{
		if (node->left != nullptr)
			ForEachValueImplWithoutIndex(node->left, lambda);

		lambda(node->data.Value);

		if (node->right != nullptr)
			ForEachValueImplWithoutIndex(node->right, lambda);
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename TLambda>
	const BaseMap<THeap, TKey, TValue>& BaseMap<THeap, TKey, TValue>::ForEachValueImpl(TLambda&& lambda) const
	{
		if constexpr (TypeTraits::IsInvocable<TLambda, const TValue&>)
		{
			auto node = this->root;
			if (node != nullptr)
			{
				ForEachValueImplWithoutIndex(node, lambda);
			}
			return *this;
		}
		else if constexpr (TypeTraits::IsInvocable<TLambda, const TValue&, SizeT>)
		{
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
	BaseMap<THeap, TKey, TValue>& BaseMap<THeap, TKey, TValue>::ForEachValueImpl(TLambda&& lambda)
	{
		if constexpr (TypeTraits::IsInvocable<TLambda, TValue&> ||
			TypeTraits::IsInvocable<TLambda, const TValue&>)
		{
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

	template<typename THeap, typename TKey, typename TValue>
	ForceInline void BaseMap<THeap, TKey, TValue>::UnsafeAdd(BasePair<THeap, TKey, TValue>&& pair)
	{
		Insert(MoveRValue(pair));
	}

	template<typename THeap, typename TKey, typename TValue>
	ForceInline void BaseMap<THeap, TKey, TValue>::UnsafeAdd(const BasePair<THeap, TKey, TValue>& pair)
	{
		Insert(pair);
	}
#pragma endregion

	template<typename THeap, typename TKey, typename TValue>
	template<typename Lambda>
	typename BaseMap<THeap, TKey, TValue>::Node* BaseMap<THeap, TKey, TValue>::DestroyTreeWithLambdaPairValues(Node* root, Lambda& lambda) //-V688
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
	BaseMap<THeap, TKey, TValue>& BaseMap<THeap, TKey, TValue>::RemoveAllImpl()
	{
		this->root = DestroyTree(this->root);
		this->count = 0;
		return *this;
	}

	template<typename THeap, typename TKey, typename TValue>
	template<typename Lambda>
	BaseMap<THeap, TKey, TValue>& BaseMap<THeap, TKey, TValue>::RemoveAllWithImpl(Lambda&& lambda)
	{
		this->root = DestroyTreeWithLambdaPairValues(this->root, lambda);
		this->count = 0;
		return *this;
	}
}

#pragma warning(pop)