#pragma once
#include "Seed.Core.Containers.NonConcurency.Dictionary.h"

namespace Seed::Core::Containers
{
	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	ForceInline UInt8 BaseDictionary<THeap, TString, TData, CharacterComparator>::height(NodeSym* targetNode)
	{
		return targetNode ? targetNode->height : 0;
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	ForceInline Int8 BaseDictionary<THeap, TString, TData, CharacterComparator>::bFactor(NodeSym* targetNode)
	{
		return height(targetNode->right) - height(targetNode->left);
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	ForceInline void BaseDictionary<THeap, TString, TData, CharacterComparator>::fixHeight(NodeSym* targetNode)
	{
		auto h_left = height(targetNode->left);
		auto h_right = height(targetNode->right);
		targetNode->height = ((h_left < h_right) ? h_right : h_left) + 1;
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	ForceInline typename BaseDictionary<THeap, TString, TData, CharacterComparator>::NodeSym* BaseDictionary<THeap, TString, TData, CharacterComparator>::rotateRight(NodeSym* p)
	{
		NodeSym* q = p->left;
		p->left = q->right;
		q->right = p;
		fixHeight(p);
		fixHeight(q);
		return q;
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	ForceInline typename BaseDictionary<THeap, TString, TData, CharacterComparator>::NodeSym* BaseDictionary<THeap, TString, TData, CharacterComparator>::rotateLeft(NodeSym* q)
	{
		NodeSym* p = q->right;
		q->right = p->left;
		p->left = q;
		fixHeight(q);
		fixHeight(p);
		return p;
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	typename BaseDictionary<THeap, TString, TData, CharacterComparator>::NodeSym* BaseDictionary<THeap, TString, TData, CharacterComparator>::balance(NodeSym* p)
	{
		fixHeight(p);

		if (bFactor(p) == 2)
		{
			if (bFactor(p->right) < 0)
				p->right = rotateRight(p->right);

			return rotateLeft(p);
		}
		if (bFactor(p) == -2)
		{
			if (bFactor(p->left) > 0)
				p->left = rotateLeft(p->left);

			return rotateRight(p);
		}
		return p;
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	typename BaseDictionary<THeap, TString, TData, CharacterComparator>::NodeSym* BaseDictionary<THeap, TString, TData, CharacterComparator>::insert(NodeSym* p, wchar_t key)
	{
		if (!p) return new NodeSym(key);

		if (CharacterComparator(key, p->key) == 0) return p;

		if (CharacterComparator(key, p->key) < 0)
			p->left = insert(p->left, key);
		else
			p->right = insert(p->right, key);
		return balance(p);
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	typename BaseDictionary<THeap, TString, TData, CharacterComparator>::NodeSym* BaseDictionary<THeap, TString, TData, CharacterComparator>::findMin(NodeSym* p)
	{
		return p->left ? findMin(p->left) : p;
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	typename BaseDictionary<THeap, TString, TData, CharacterComparator>::NodeSym* BaseDictionary<THeap, TString, TData, CharacterComparator>::removeMin(NodeSym* p)
	{
		if (p->left == 0)
			return p->right;

		p->left = removeMin(p->left);
		return balance(p);
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	typename BaseDictionary<THeap, TString, TData, CharacterComparator>::NodeSym* BaseDictionary<THeap, TString, TData, CharacterComparator>::remove(NodeSym* p, Char key)
	{
		if (!p) return nullptr;

		if (CharacterComparator(key, p->key) < 0)
			p->left = remove(p->left, key);
		else if (CharacterComparator(key, p->key) > 0)
			p->right = remove(p->right, key);
		else
		{
			NodeSym* q = p->left;
			NodeSym* r = p->right;
			delete p;
			if (!r) return q;
			NodeSym* min = findMin(r);
			min->right = removeMin(r);
			min->left = q;
			return balance(min);
		}
		return balance(p);
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	typename BaseDictionary<THeap, TString, TData, CharacterComparator>::NodeSym* BaseDictionary<THeap, TString, TData, CharacterComparator>::find(NodeSym* root, Char key) const
	{ //-V688
		if (!root) return nullptr;

		while (CharacterComparator(root->key, key) != 0)
		{
			if (CharacterComparator(root->key, key) > 0)
				root = root->left;
			else
				root = root->right;

			if (!root)
				return nullptr;
		}

		return root;
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	typename BaseDictionary<THeap, TString, TData, CharacterComparator>::NodeLex* BaseDictionary<THeap, TString, TData, CharacterComparator>::insert(
		NodeLex* rootLex, //-V688
		Pair&& data,
		size_t index
	)
	{
		if (!rootLex)
		{
			rootLex = new NodeLex;
		}

		rootLex->node_root = insert(rootLex->node_root, data.Key[index - 1]);
		auto subnode = find(rootLex->node_root, data.Key[index - 1]);

		if (data.Key.Length != index)
		{
			subnode->data = insert(subnode->data, MoveRValue(data), index + 1); //-V522
		}
		else
		{
			if (subnode->data == nullptr) subnode->data = new NodeLex(1);

			if (subnode->data->data != nullptr)
				Throw(NullPointException(SourcePosition));

			subnode->data->data = new Pair(MoveRValue(data));
		}

		++rootLex->count;
		return rootLex;
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<typename UString>
	typename BaseDictionary<THeap, TString, TData, CharacterComparator>::NodeLex* BaseDictionary<THeap, TString, TData, CharacterComparator>::find(
		NodeLex* This, //-V688
		const UString& data,
		size_t index
	) const
	{
		if (!This)
			return nullptr;
		auto subnode = find(This->node_root, data[index]);

		if (!subnode)
			return nullptr;

		if (data.Length - 1 != index)
		{
			return find(subnode->data, data, index + 1);
		}
		else
		{
			return subnode->data;
		}
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	typename BaseDictionary<THeap, TString, TData, CharacterComparator>::NodeLex* BaseDictionary<THeap, TString, TData, CharacterComparator>::remove_node(NodeLex* This, //-V688
		const TString& key,
		bool& removed,
		size_t index
	)
	{
		if (!This) return nullptr;

		if (key.Length != index - 1)
		{
			auto subnode = find(This->node_root, key[index - 1]);
			if (!subnode)
				return This;

			subnode->data = remove_node(subnode->data, key, removed, index + 1);
			if (!subnode->data)
				return This;

			if (removed)
			{
				--subnode->data->count;
				if (subnode->data->count == 0)
				{
					delete subnode->data;
					subnode->data = nullptr;
					This->node_root = remove(This->node_root, key[index - 1]);
				}
			}
		}
		else
		{
			if (This->data != nullptr)
			{
				delete This->data;
				removed = true;
			}
		}
		return This;
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	typename BaseDictionary<THeap, TString, TData, CharacterComparator>::NodeLex* BaseDictionary<THeap, TString, TData, CharacterComparator>::destroy_node(NodeLex* This)
	{
		if (This != nullptr)
		{
			delete This->data;

			if (This->node_root != nullptr)
				This->node_root = destroy_tree(This->node_root);
		}

		delete This;
		return nullptr;
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	typename BaseDictionary<THeap, TString, TData, CharacterComparator>::NodeSym* BaseDictionary<THeap, TString, TData, CharacterComparator>::destroy_tree(NodeSym* This)
	{
		if (This != nullptr)
		{
			if (This->left != nullptr)
			{
				if (This->left->data != nullptr)
				{
					This->left->data = destroy_node(This->left->data);
				}

				This->left = destroy_tree(This->left);
				delete This->left;
			}

			if (This->data != nullptr)
			{
				This->data = destroy_node(This->data);
			}

			if (This->right != nullptr)
			{
				if (This->right->data != nullptr)
				{
					This->right->data = destroy_node(This->right->data);
				}

				This->right = destroy_tree(This->right);
				delete This->right;
			}

			delete This;
		}
		return nullptr;
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<typename TLambda>
	typename BaseDictionary<THeap, TString, TData, CharacterComparator>::NodeLex* BaseDictionary<THeap, TString, TData, CharacterComparator>::destroy_node(NodeLex* This, TLambda& lambda)
	{
		if (This == nullptr)
			return nullptr;

		if (This->node_root != nullptr)
			This->node_root = destroy_tree(This->node_root, lambda);

		if (This->data != nullptr)
		{
			lambda(MoveRValue(*This->data));
			delete This->data;
		}

		delete This;
		return nullptr;
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<typename TLambda>
	typename BaseDictionary<THeap, TString, TData, CharacterComparator>::NodeSym* BaseDictionary<THeap, TString, TData, CharacterComparator>::destroy_tree(NodeSym* This, TLambda& lambda)
	{
		if (This == nullptr)
			return nullptr;

		if (This->left != nullptr)
		{
			This->left = destroy_tree(This->left, lambda);
		}

		if (This->data != nullptr)
		{
			This->data = destroy_node(This->data, lambda);
		}

		if (This->right != nullptr)
		{
			This->right = destroy_tree(This->right, lambda);
		}

		delete This;
		return nullptr;
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	typename BaseDictionary<THeap, TString, TData, CharacterComparator>::NodeSym* BaseDictionary<THeap, TString, TData, CharacterComparator>::create_lex_copy(NodeSym* orig) const
	{
		NodeSym* copy = new NodeSym(orig->key);

		if (orig->data != nullptr) copy->data = create_lex_copy(orig->data);
		if (orig->left != nullptr) copy->left = create_lex_copy(orig->left);
		if (orig->right != nullptr) copy->right = create_lex_copy(orig->right);

		copy->height = orig->height;
		return copy;
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	typename BaseDictionary<THeap, TString, TData, CharacterComparator>::NodeLex* BaseDictionary<THeap, TString, TData, CharacterComparator>::create_lex_copy(NodeLex* orig) const
	{
		NodeLex* copy = new NodeLex(orig->count);
		if (orig->node_root != nullptr) copy->node_root = create_lex_copy(orig->node_root);

		if (orig->data != nullptr)
		{
			if constexpr (TypeTraits::IsStandardLayout<TData>)
			{
				copy->data = new BasePair<THeap, TString, TData>(*orig->data);
			}
			else if constexpr (Concepts::HasCloneMethod<TData>)
			{
				copy->data = new BasePair<THeap, TString, TData>(orig->data->Clone());
			}
			else
			{
				copy->data = new BasePair<THeap, TString, TData>(*orig->data);
			}
		}

		return copy;
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	BaseDictionary<THeap, TString, TData, CharacterComparator>* BaseDictionary<THeap, TString, TData, CharacterComparator>::create_lex_copy(BaseDictionary* This) const
	{
		if (This == nullptr)
			return nullptr;

		BaseDictionary* copy = new BaseDictionary();

		if (This->node != nullptr)
			copy->node = create_lex_copy(This->node);

		return copy;
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<typename> requires Concepts::IsClonable<TString>&& Concepts::IsClonable<TData>
	typename BaseDictionary<THeap, TString, TData, CharacterComparator>::NodeSym* BaseDictionary<THeap, TString, TData, CharacterComparator>::create_lex_clone_copy(NodeSym* orig) const
	{
		NodeSym* copy = new NodeSym(orig->key);

		if (orig->data != nullptr) copy->data = create_lex_clone_copy(orig->data);
		if (orig->left != nullptr) copy->left = create_lex_clone_copy(orig->left);
		if (orig->right != nullptr) copy->right = create_lex_clone_copy(orig->right);

		copy->height = orig->height;
		return copy;
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<typename> requires Concepts::IsClonable<TString>&& Concepts::IsClonable<TData>
	typename BaseDictionary<THeap, TString, TData, CharacterComparator>::NodeLex* BaseDictionary<THeap, TString, TData, CharacterComparator>::create_lex_clone_copy(NodeLex* orig) const
	{
		NodeLex* copy = new NodeLex(orig->count);
		if (orig->node_root != nullptr) copy->node_root = create_lex_clone_copy(orig->node_root);

		if (orig->data != nullptr)
		{
			copy->data = new BasePair<THeap, TString, TData>(Core::Clone(*orig->data));
		}
		return copy;
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<typename TLambda>
	typename BaseDictionary<THeap, TString, TData, CharacterComparator>::NodeSym* BaseDictionary<THeap, TString, TData, CharacterComparator>::create_lex_clone_copy(NodeSym* orig, TLambda& lambda) const
	{
		NodeSym* copy = new NodeSym(orig->key);

		if (orig->data != nullptr) copy->data = create_lex_clone_copy(orig->data, lambda);
		if (orig->left != nullptr) copy->left = create_lex_clone_copy(orig->left, lambda);
		if (orig->right != nullptr) copy->right = create_lex_clone_copy(orig->right, lambda);

		copy->height = orig->height;
		return copy;
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<typename TLambda>
	typename BaseDictionary<THeap, TString, TData, CharacterComparator>::NodeLex* BaseDictionary<THeap, TString, TData, CharacterComparator>::create_lex_clone_copy(NodeLex* orig, TLambda& lambda) const
	{
		NodeLex* copy = new NodeLex(orig->count);
		if (orig->node_root != nullptr) copy->node_root = create_lex_clone_copy(orig->node_root);

		if (orig->data != nullptr)
		{
			copy->data = new BasePair<THeap, TString, TData>(lambda(orig->data));
		}

		return copy;
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	void BaseDictionary<THeap, TString, TData, CharacterComparator>::ToMap(
			NodeSym* targetNode,
			BaseMap<THeap, TString, TData>& list)
	{
		if (targetNode->left != nullptr) ToMap(targetNode->left, list);
		if (targetNode->data != nullptr) ToMap(targetNode->data, list);
		if (targetNode->right != nullptr) ToMap(targetNode->right, list);
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	void BaseDictionary<THeap, TString, TData, CharacterComparator>::ToMap(
		NodeLex* targetNode,
		BaseMap<THeap, TString, TData>& list)
	{
		if (targetNode->data != nullptr)
			list.Insert(*targetNode->data);
		if (targetNode->node_root != nullptr)
			ToMap(targetNode->node_root, list);
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	void BaseDictionary<THeap, TString, TData, CharacterComparator>::ToMap(
		BaseDictionary* This,
		BaseMap<THeap, TString, TData>& list)
	{
		if (This != nullptr)
			if (This->node != nullptr)
				ToMap(This->node, list);
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	BaseDictionary<THeap, TString, TData, CharacterComparator>::~BaseDictionary()
	{
		this->node = destroy_node(this->node);
		this->count = 0;
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	BaseDictionary<THeap, TString, TData, CharacterComparator>::BaseDictionary(InitilizerList<BasePair<THeap, TString, TData>>&& pairs): node(nullptr), count(0)
	{
		for (auto&& it : pairs)
		{
			Insert(MoveRValue(it));
		}
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	BaseDictionary<THeap, TString, TData, CharacterComparator>::BaseDictionary(const BaseDictionary& original): node(nullptr), count(original.count)
	{
		if (count != 0)
		{
			this->node = create_lex_copy(original.node);
		}
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	BaseDictionary<THeap, TString, TData, CharacterComparator>::BaseDictionary(BaseDictionary&& original): node(original.node), count(original.count)
	{
		original.node = nullptr;
		original.count = 0;
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	BaseDictionary<THeap, TString, TData, CharacterComparator>& BaseDictionary<THeap, TString, TData, CharacterComparator>::operator=(const BaseDictionary& original)
	{
		this->~BaseDictionary();
		this->BaseDictionary::BaseDictionary(original);
		return *this;
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	BaseDictionary<THeap, TString, TData, CharacterComparator>& BaseDictionary<THeap, TString, TData, CharacterComparator>::operator=(BaseDictionary&& original)
	{
		this->~BaseDictionary();
		this->BaseDictionary::BaseDictionary(MoveRValue(original));
		return *this;
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	ForceInline BaseDictionary<THeap, TString, TData, CharacterComparator>& BaseDictionary<THeap, TString, TData, CharacterComparator>::Insert(const TString& key, const TData& value)
	{
		BasePair<THeap, TString, TData> data{ key, value };
		this->node = insert(this->node, MoveRValue(data));
		++this->count;
		return *this;
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	ForceInline BaseDictionary<THeap, TString, TData, CharacterComparator>& BaseDictionary<THeap, TString, TData, CharacterComparator>::Insert(TString&& key, const TData& value)
	{
		BasePair<THeap, TString, TData> data{ MoveRValue(key), value };
		this->node = insert(this->node, MoveRValue(data));
		++this->count;
		return *this;
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	ForceInline BaseDictionary<THeap, TString, TData, CharacterComparator>& BaseDictionary<THeap, TString, TData, CharacterComparator>::Insert(const TString& key, TData&& value)
	{
		BasePair<THeap, TString, TData> data{ key, MoveRValue(value) };
		this->node = insert(this->node, MoveRValue(data));
		++this->count;
		return *this;
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	ForceInline BaseDictionary<THeap, TString, TData, CharacterComparator>& BaseDictionary<THeap, TString, TData, CharacterComparator>::Insert(TString&& key, TData&& value)
	{
		BasePair<THeap, TString, TData> data{ MoveRValue(key), MoveRValue(value) };
		this->node = insert(this->node, MoveRValue(data));
		++this->count;
		return *this;
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	ForceInline BaseDictionary<THeap, TString, TData, CharacterComparator>& BaseDictionary<THeap, TString, TData, CharacterComparator>::Insert(Pair&& data)
	{
		this->node = insert(this->node, MoveRValue(data));
		++this->count;
		return *this;
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	ForceInline BaseDictionary<THeap, TString, TData, CharacterComparator>& BaseDictionary<THeap, TString, TData, CharacterComparator>::Insert(const Pair& data)
	{
		this->node = insert(this->node, MoveRValue(Pair(data)));
		++this->count;
		return *this;
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<typename UString> requires (requires(const UString& str)
	{
		{
			str[0]
		}->Concepts::IsConvertible<Char>;
	})
	const TData& BaseDictionary<THeap, TString, TData, CharacterComparator>::Find(const UString& Key) const
	{
		if (Key.Length == 0)
			Throw(NullPointException(SourcePosition));
		if (node == nullptr)
			Throw(NotFoundException(SourcePosition, L"Invalid Key"));

		NodeLex* targetNode = find(this->node, Key);

		if (targetNode == nullptr || targetNode->data == nullptr)
			Throw(NotFoundException(SourcePosition, L"Invalid Key"));

		return targetNode->data->Value;
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<typename UString> requires (Concepts::IsComparableWith<TString, UString>&& requires(const UString& str)
	{
		{
			str[0]
		}->Concepts::IsConvertible<Char>;
	})
	TData& BaseDictionary<THeap, TString, TData, CharacterComparator>::Find(const UString& Key)
	{
		if (Key.Length == 0)
			Throw(NullPointException(SourcePosition));
		if (this->node == nullptr)
			Throw(NotFoundException(SourcePosition, L"Invalid Key"));

		NodeLex* targetNode = find(this->node, Key);

		if (targetNode == nullptr || targetNode->data == nullptr)
			Throw(NotFoundException(SourcePosition, L"Invalid Key"));

		return targetNode->data->Value;
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<SizeT TN>
	TData& BaseDictionary<THeap, TString, TData, CharacterComparator>::Find(StaticTextRef<TN> Key)
	{
		return Find(ConstString(Key));
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<SizeT TN>
	const TData& BaseDictionary<THeap, TString, TData, CharacterComparator>::Find(StaticTextRef<TN> Key) const
	{
		return Find(ConstString(Key));
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<typename UString> requires (requires(const UString& str)
	{
		{
			str[0]
		}->Concepts::IsConvertible<Char>;
	})
	ForceInline TData& BaseDictionary<THeap, TString, TData, CharacterComparator>::operator[](const UString& key)
	{
		return Find(key);
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<typename UString> requires (requires(const UString& str)
	{
		{
			str[0]
		}->Concepts::IsConvertible<Char>;
	})
	ForceInline TData& BaseDictionary<THeap, TString, TData, CharacterComparator>::operator[](UString&& key)
	{
		return Find(key);
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<typename UString> requires (requires(const UString& str)
	{
		{
			str[0]
		}->Concepts::IsConvertible<Char>;
	})
	ForceInline const TData& BaseDictionary<THeap, TString, TData, CharacterComparator>::operator[](const UString& key) const
	{
		return Find(key);
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<typename UString> requires (requires(const UString& str)
	{
		{
			str[0]
		}->Concepts::IsConvertible<Char>;
	})
	ForceInline const TData& BaseDictionary<THeap, TString, TData, CharacterComparator>::operator[](UString&& key) const
	{
		return Find(key);
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<SizeT TN>
	TData& BaseDictionary<THeap, TString, TData, CharacterComparator>::operator[](StaticTextRef<TN> Key)
	{
		return Find(ConstString(Key));
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<SizeT TN>
	const TData& BaseDictionary<THeap, TString, TData, CharacterComparator>::operator[](StaticTextRef<TN> Key) const
	{
		return Find(ConstString(Key));
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	ForceInline Boolean BaseDictionary<THeap, TString, TData, CharacterComparator>::IsEmpty() const
	{
		return this->count == 0;
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<typename UString> requires (requires(const UString& str)
	{
		{
			str[0]
		}->Concepts::IsConvertible<Char>;
	})
	ForceInline Boolean BaseDictionary<THeap, TString, TData, CharacterComparator>::IsExist(const UString& Key) const
	{
		if (node == nullptr)
			return false;

		NodeLex* targetNode = find(this->node, Key);
		return (targetNode == nullptr || targetNode->data == nullptr) ? false : true;
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<typename UString> requires (requires(const UString& str)
	{
		{
			str[0]
		}->Concepts::IsConvertible<Char>;
	})
	ForceInline Boolean BaseDictionary<THeap, TString, TData, CharacterComparator>::IsExist(UString&& Key) const
	{
		if (node == nullptr)
			return false;

		NodeLex* targetNode = find(this->node, Key);
		return (targetNode == nullptr || targetNode->data == nullptr) ? false : true;
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<SizeT TN>
	ForceInline Boolean BaseDictionary<THeap, TString, TData, CharacterComparator>::IsExist(StaticTextRef<TN> Key) const
	{
		return IsExist(ConstString(Key));
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	BaseDictionary<THeap, TString, TData, CharacterComparator>& BaseDictionary<THeap, TString, TData, CharacterComparator>::Remove(const TString& Key)
	{
		bool flag = false;
		this->node = remove_node(this->node, Key, flag);
		if (flag)
		{
			--this->count;
		}

		return *this;
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	ForceInline SizeT BaseDictionary<THeap, TString, TData, CharacterComparator>::GetCount() const
	{
		return this->count;
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	ForceInline BaseMap<THeap, TString, TData> BaseDictionary<THeap, TString, TData, CharacterComparator>::ToMap()
	{
		BaseMap<THeap, TString, TData> retVal;
		ToMap(this, retVal);
		return retVal;
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<typename> requires (Concepts::IsClonable<TString>&& Concepts::IsClonable<TData>)
	BaseDictionary<THeap, TString, TData, CharacterComparator> BaseDictionary<THeap, TString, TData, CharacterComparator>::Clone() const
	{
		BaseDictionary result;
		result.node = create_lex_clone_copy(this->node);
		result.count = this->count;
		return result;
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<typename TLambda> requires (Concepts::IsInvocableWithResult<TLambda, TData, const TData&>)
	BaseDictionary<THeap, TString, TData, CharacterComparator> BaseDictionary<THeap, TString, TData, CharacterComparator>::Clone(TLambda&& lambda) const
	{
		BaseDictionary result;
		result.node = create_lex_clone_copy(this->node, lambda);
		result.count = this->count;
		return result;
	}

#pragma region LINQ

#pragma region For Each
	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<typename Lambda>
	static void BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachImplWithoutIndex(
		NodeSym* node,
		Lambda& lambda)
	{
		if (node->left != nullptr) ForEachImplWithoutIndex(node->left, lambda);
		if (node->data != nullptr) ForEachImplWithoutIndex(node->data, lambda);
		if (node->right != nullptr) ForEachImplWithoutIndex(node->right, lambda);
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<typename Lambda>
	static void BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachImplWithoutIndex(
		NodeLex* node,
		Lambda& lambda)
	{
		if (node->data != nullptr)
			lambda(*node->data);

		if (node->node_root != nullptr)
			ForEachImplWithoutIndex(node->node_root, lambda);
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<typename TLambda>
	ForceInline static void BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachImplWithoutIndex(
		const BaseDictionary* const This,
		TLambda& lambda)
	{
		if (This != nullptr)
			if (This->node != nullptr)
				ForEachImplWithoutIndex(This->node, lambda);
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<typename Lambda>
	static void BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachImplWithIndex(
		NodeSym* node,
		Lambda& lambda,
		SizeT& Index)
	{
		if (node->left != nullptr) ForEachImplWithIndex(node->left, lambda, Index);
		if (node->data != nullptr) ForEachImplWithIndex(node->data, lambda, Index);
		if (node->right != nullptr) ForEachImplWithIndex(node->right, lambda, Index);
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<typename Lambda>
	static void BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachImplWithIndex(
		NodeLex* node,
		Lambda& lambda,
		SizeT& Index)
	{
		if (node->data != nullptr)
		{
			lambda(*node->data, Index);
			Index++;
		}

		if (node->node_root != nullptr)
			ForEachImplWithIndex(node->node_root, lambda, Index);
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<typename TLambda>
	ForceInline static void BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachImplWithIndex(
		const BaseDictionary* const This,
		TLambda& lambda,
		SizeT& Index)
	{
		if (This != nullptr)
			if (This->node != nullptr)
				ForEachImplWithIndex(This->node, lambda, Index);
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<typename LambdaT>
	const BaseDictionary<THeap, TString, TData, CharacterComparator>& BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachImpl(LambdaT&& lambda) const
	{
		if constexpr (TypeTraits::IsInvocable<LambdaT, const BasePair<THeap, TString, TData>&>)
		{
			if (this->node != nullptr)
				ForEachImplWithoutIndex(this->node, lambda);
			return *this;
		}
		else if constexpr (TypeTraits::IsInvocable<LambdaT, const BasePair<THeap, TString, TData>&, SizeT>)
		{
			SizeT index = 0;
			if (this->node != nullptr)
				ForEachImplWithIndex(this->node, lambda, index);
			return *this;
		}
		else
		{
			static_assert(TypeTraits::IsInvocable<LambdaT, const BasePair<THeap, TString, TData>&> ||
				TypeTraits::IsInvocable<LambdaT, const BasePair<THeap, TString, TData>&, SizeT>);
		}
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<typename LambdaT>
	BaseDictionary<THeap, TString, TData, CharacterComparator>& BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachImpl(LambdaT&& lambda)
	{
		if constexpr (TypeTraits::IsInvocable<LambdaT, BasePair<THeap, TString, TData>&> ||
			TypeTraits::IsInvocable<LambdaT, const BasePair<THeap, TString, TData>&>)
		{
			if (this->node != nullptr)
				ForEachImplWithoutIndex(this->node, lambda);
			return *this;
		}
		else if constexpr (TypeTraits::IsInvocable<LambdaT, BasePair<THeap, TString, TData>&, SizeT> ||
			TypeTraits::IsInvocable<LambdaT, const BasePair<THeap, TString, TData>&, SizeT>)
		{
			SizeT index = 0;
			if (this->node != nullptr)
				ForEachImplWithIndex(this->node, lambda, index);
			return *this;
		}
		else
		{
			static_assert(TypeTraits::IsInvocable<LambdaT, BasePair<THeap, TString, TData>&> ||
				TypeTraits::IsInvocable<LambdaT, const BasePair<THeap, TString, TData>&> ||
				TypeTraits::IsInvocable<LambdaT, BasePair<THeap, TString, TData>&, SizeT> ||
				TypeTraits::IsInvocable<LambdaT, const BasePair<THeap, TString, TData>&, SizeT>);
		}
	}
#pragma endregion

#pragma region For Each Key
	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<typename Lambda>
	static void BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachKeyImplWithoutIndex(
		NodeSym* node,
		Lambda& lambda)
	{
		if (node->left != nullptr) ForEachKeyImplWithoutIndex(node->left, lambda);
		if (node->data != nullptr) ForEachKeyImplWithoutIndex(node->data, lambda);
		if (node->right != nullptr) ForEachKeyImplWithoutIndex(node->right, lambda);
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<typename Lambda>
	static void BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachKeyImplWithoutIndex(
		NodeLex* node,
		Lambda& lambda)
	{
		if (node->data != nullptr)
			lambda(node->data->Key);

		if (node->node_root != nullptr)
			ForEachKeyImplWithoutIndex(node->node_root, lambda);
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<typename TLambda>
	ForceInline static void BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachKeyImplWithoutIndex(
		const BaseDictionary* const This,
		TLambda& lambda)
	{
		if (This != nullptr)
			if (This->node != nullptr)
				ForEachKeyImplWithoutIndex(This->node, lambda);
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<typename Lambda>
	static void BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachKeyImplWithIndex(
		NodeSym* node,
		Lambda& lambda,
		SizeT& Index)
	{
		if (node->left != nullptr) ForEachKeyImplWithIndex(node->left, lambda, Index);
		if (node->data != nullptr) ForEachKeyImplWithIndex(node->data, lambda, Index);
		if (node->right != nullptr) ForEachKeyImplWithIndex(node->right, lambda, Index);
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<typename Lambda>
	static void BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachKeyImplWithIndex(
		NodeLex* node,
		Lambda& lambda,
		SizeT& Index)
	{
		if (node->data != nullptr)
		{
			lambda(node->data->Key, Index);
			Index++;
		}

		if (node->node_root != nullptr)
			ForEachKeyImplWithIndex(node->node_root, lambda, Index);
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<typename TLambda>
	ForceInline static void BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachKeyImplWithIndex(
		const BaseDictionary* const This,
		TLambda& lambda,
		SizeT& Index)
	{
		if (This != nullptr)
			if (This->node != nullptr)
				ForEachKeyImplWithIndex(This->node, lambda, Index);
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<typename LambdaT>
	const BaseDictionary<THeap, TString, TData, CharacterComparator>& BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachKeyImpl(LambdaT&& lambda) const
	{
		if constexpr (TypeTraits::IsInvocable<LambdaT, const TString&>)
		{
			if (this->node != nullptr)
				ForEachKeyImplWithoutIndex(this->node, lambda);
			return *this;
		}
		else if constexpr (TypeTraits::IsInvocable<LambdaT, const TString&, SizeT>)
		{
			SizeT index = 0;
			if (this->node != nullptr)
				ForEachKeyImplWithIndex(this->node, lambda, index);
			return *this;
		}
		else
		{
			static_assert(TypeTraits::IsInvocable<LambdaT, const TString&> ||
				TypeTraits::IsInvocable<LambdaT, const TString&, SizeT>);
		}
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<typename LambdaT>
	BaseDictionary<THeap, TString, TData, CharacterComparator>& BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachKeyImpl(LambdaT&& lambda)
	{
		if constexpr (TypeTraits::IsInvocable<LambdaT, const TString&>)
		{
			if (this->node != nullptr)
				ForEachKeyImplWithoutIndex(this->node, lambda);
			return *this;
		}
		else if constexpr (TypeTraits::IsInvocable<LambdaT, const TString&, SizeT>)
		{
			SizeT index = 0;
			if (this->node != nullptr)
				ForEachKeyImplWithIndex(this->node, lambda, index);
			return *this;
		}
		else
		{
			static_assert(TypeTraits::IsInvocable<LambdaT, const TString&> ||
				TypeTraits::IsInvocable<LambdaT, const TString&, SizeT>);
		}
	}
#pragma endregion

#pragma region For Each Value
	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<typename Lambda>
	static void BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachValueImplWithoutIndex(
		NodeSym* node,
		Lambda& lambda)
	{
		if (node->left != nullptr) ForEachValueImplWithoutIndex(node->left, lambda);
		if (node->data != nullptr) ForEachValueImplWithoutIndex(node->data, lambda);
		if (node->right != nullptr) ForEachValueImplWithoutIndex(node->right, lambda);
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<typename Lambda>
	static void BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachValueImplWithoutIndex(
		NodeLex* node,
		Lambda& lambda)
	{
		if (node->data != nullptr)
			lambda(node->data->Value);

		if (node->node_root != nullptr)
			ForEachValueImplWithoutIndex(node->node_root, lambda);
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<typename TLambda>
	ForceInline static void BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachValueImplWithoutIndex(
		const BaseDictionary* const This,
		TLambda& lambda)
	{
		if (This != nullptr)
			if (This->node != nullptr)
				ForEachValueImplWithoutIndex(This->node, lambda);
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<typename Lambda>
	static void BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachValueImplWithIndex(
		NodeSym* node,
		Lambda& lambda,
		SizeT& Index)
	{
		if (node->left != nullptr) ForEachValueImplWithIndex(node->left, lambda, Index);
		if (node->data != nullptr) ForEachValueImplWithIndex(node->data, lambda, Index);
		if (node->right != nullptr) ForEachValueImplWithIndex(node->right, lambda, Index);
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<typename Lambda>
	static void BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachValueImplWithIndex(
		NodeLex* node,
		Lambda& lambda,
		SizeT& Index)
	{
		if (node->data != nullptr)
		{
			lambda(node->data->Value, Index);
			Index++;
		}
		if (node->node_root != nullptr)
			ForEachValueImplWithIndex(node->node_root, lambda, Index);
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<typename TLambda>
	ForceInline static void BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachValueImplWithIndex(
		const BaseDictionary* const This,
		TLambda& lambda,
		SizeT& index)
	{
		if (This != nullptr)
			if (This->node != nullptr)
				ForEachValueImplWithIndex(This->node, lambda, index);
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<typename LambdaT>
	const BaseDictionary<THeap, TString, TData, CharacterComparator>& BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachValueImpl(LambdaT&& lambda) const
	{
		if constexpr (TypeTraits::IsInvocable<LambdaT, const TData&>)
		{
			if (this->node != nullptr)
				ForEachValueImplWithoutIndex(this->node, lambda);
			return *this;
		}
		else if constexpr (TypeTraits::IsInvocable<LambdaT, const TData&, SizeT>)
		{
			SizeT index = 0;
			if (this->node != nullptr)
				ForEachValueImplWithIndex(this->node, lambda, index);
			return *this;
		}
		else
		{
			static_assert(TypeTraits::IsInvocable<LambdaT, const TData&> ||
				TypeTraits::IsInvocable<LambdaT, const TData&, SizeT>);
		}
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<typename LambdaT>
	BaseDictionary<THeap, TString, TData, CharacterComparator>& BaseDictionary<THeap, TString, TData, CharacterComparator>::ForEachValueImpl(LambdaT&& lambda)
	{
		if constexpr (TypeTraits::IsInvocable<LambdaT, TData&> ||
			TypeTraits::IsInvocable<LambdaT, const TData&>)
		{
			if (this->node != nullptr)
				ForEachValueImplWithoutIndex(this->node, lambda);
			return *this;
		}
		else if constexpr (TypeTraits::IsInvocable<LambdaT, TData&, SizeT> ||
			TypeTraits::IsInvocable<LambdaT, const TData&, SizeT>)
		{
			SizeT index = 0;
			if (this->node != nullptr)
				ForEachValueImplWithIndex(this->node, lambda, index);
			return *this;
		}
		else
		{
			static_assert(TypeTraits::IsInvocable<LambdaT, TData&> ||
				TypeTraits::IsInvocable<LambdaT, const TData&> ||
				TypeTraits::IsInvocable<LambdaT, TData&, SizeT> ||
				TypeTraits::IsInvocable<LambdaT, const TData&, SizeT>);
		}
	}
#pragma endregion

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	BaseDictionary<THeap, TString, TData, CharacterComparator>& BaseDictionary<THeap, TString, TData, CharacterComparator>::RemoveAllImpl()
	{
		this->node = destroy_node(this->node);
		this->count = 0;
		return *this;
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	template<typename TLambda>
	BaseDictionary<THeap, TString, TData, CharacterComparator>& BaseDictionary<THeap, TString, TData, CharacterComparator>::RemoveAllWithImpl(TLambda&& lambda)
	{
		this->node = destroy_node(this->node, lambda);
		this->count = 0;
		return *this;
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	ForceInline void BaseDictionary<THeap, TString, TData, CharacterComparator>::UnsafeAdd(Pair&& pair)
	{
		Insert(MoveRValue(pair));
	}

	template<typename THeap, typename TString, typename TData, TCharacterComparator CharacterComparator>
	ForceInline void BaseDictionary<THeap, TString, TData, CharacterComparator>::UnsafeAdd(const Pair& pair)
	{
		Insert(pair);
	}
#pragma endregion
}