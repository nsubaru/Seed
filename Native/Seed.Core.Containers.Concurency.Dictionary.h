#pragma once
#include "Seed.Core.Containers.TypeTraits.h"
#include "Seed.Core.Containers.API.LINQ.h"
#include "Seed.Core.Containers.Pair.h"
#include "Seed.Core.Containers.Concurency.Map.h"
#include "Seed.Core.String.h"
#include "Seed.Core.Exceptions.h"

namespace Seed {
	inline namespace Core {
		namespace Containers {
			//Шаблон словника слів який виконує швидкісний пошук слова
			template<typename THeap, typename TString, typename TData>
			class SeedKernelCall BaseConcurencyDictionary :
				public Memory::Allocator<THeap>,
				public LINQ::IDictionary<BaseConcurencyDictionary, THeap, TString, TData, BasePair<THeap, TString, TData>, true>,
				public IDisposable,
				public IClonable<BaseConcurencyDictionary<THeap, TString, TData>>
			{
				static_assert(TypeTraits::IsSameV<TString, BaseString<THeap>>, "BaseConcurencyDictionary key must be BaseString<THeap>");
			private:
				using Pair = BasePair<THeap, TString, TData>;
				//Структура букви в складі AVL-дерева
				struct NodeSym;

				//Структура піддервева
				struct NodeLex : public Memory::Allocator<THeap> {
					RefCountT count;
					NodeSym* node_root;
					BasePair<THeap, TString, TData>* data;
					NodeLex(SizeT count = 0) :node_root(nullptr), count(count), data(nullptr) {};
				};

				//Структура букви в складі AVL-дерева
				struct NodeSym : public Memory::Allocator<THeap> {
					UInt8 height;
					Char key;
					NodeLex* data;
					NodeSym* left;
					NodeSym* right;
					NodeSym(Char key) :key(key), height(1), left(nullptr), right(nullptr), data(nullptr) {}
				};

				//Висота дерева
				__forceinline UInt8 height(NodeSym* node) {
					return node ? node->height : 0;
				}

				//Різниця висот лівої і правої підгілки
				__forceinline Int8 bfactor(NodeSym* node) {
					return height(node->right) - height(node->left);
				}

				//Переобчислення висоти
				__forceinline void fixheight(NodeSym* node) {
					auto h_left = height(node->left);
					auto h_right = height(node->right);
					node->height = ((h_left < h_right) ? h_right : h_left) + 1;
				}

				//Правий поворо піддерева
				NodeSym* rotateright(NodeSym* p) {
					NodeSym* q = p->left;
					p->left = q->right;
					q->right = p;
					fixheight(p);
					fixheight(q);
					return q;
				}

				//Лівий поворот піддерева
				NodeSym* rotateleft(NodeSym* q) {
					NodeSym* p = q->right;
					q->right = p->left;
					p->left = q;
					fixheight(q);
					fixheight(p);
					return p;
				}

				//Балансування піддерева
				NodeSym* balance(NodeSym* p) {
					fixheight(p);
					if (bfactor(p) == 2)
					{
						if (bfactor(p->right) < 0)
							p->right = rotateright(p->right);
						return rotateleft(p);
					}
					if (bfactor(p) == -2)
					{
						if (bfactor(p->left) > 0)
							p->left = rotateleft(p->left);
						return rotateright(p);
					}
					return p;
				}

				//Вставка елемента в дерево
				NodeSym* insert(NodeSym* p, wchar_t key) {
					if (!p) return new NodeSym(key);
					if (key == p->key) return p;
					if (key < p->key)
						p->left = insert(p->left, key);
					else
						p->right = insert(p->right, key);
					return balance(p);
				}

				//Пошук мінімального елемента в дереві
				__forceinline NodeSym* findmin(NodeSym* p) {
					return p->left ? findmin(p->left) : p;
				}

				//Видалення мінімального елемента в дереві
				__forceinline NodeSym* removemin(NodeSym* p) {
					if (p->left == 0)
						return p->right;
					p->left = removemin(p->left);
					return balance(p);
				}

				//Видалення елемента по ключу в піддереві
				NodeSym* remove(NodeSym* p, Char key) {
					if (!p) return nullptr;
					if (key < p->key)
						p->left = remove(p->left, key);
					else if (key > p->key)
						p->right = remove(p->right, key);
					else
					{
						NodeSym* q = p->left;
						NodeSym* r = p->right;
						delete p;
						if (!r) return q;
						NodeSym* min = findmin(r);
						min->right = removemin(r);
						min->left = q;
						return balance(min);
					}
					return balance(p);
				}

				//Пошук елемента в дереві
				__forceinline NodeSym* find(NodeSym* root, Char key) const { //-V688
					if (!root) return nullptr;
					while (root->key != key) {
						if (root->key > key)
							root = root->left;
						else
							root = root->right;
						if (!root)
							return nullptr;
					}
					return root;
				}

				//Вставка елемента в систему дерев
				NodeLex* insert(
					NodeLex* rootLex, //-V688
					Pair&& data,
					size_t index = 1
				) {
					if (!rootLex) {
						rootLex = new NodeLex;
					}
					rootLex->node_root = insert(rootLex->node_root, data.Key[index - 1]);
					auto subnode = find(rootLex->node_root, data.Key[index - 1]);
					if (data.Key.Size != index) {
						subnode->data = insert(subnode->data, MoveRValue(data), index + 1);
					}
					else {
						if (subnode->data == nullptr) subnode->data = new NodeLex(1);
						if (subnode->data->data != nullptr)
							Throw(NullPointException(ExceptionPosition));
						subnode->data->data = new Pair(MoveRValue(data));
					}
					++rootLex->count;
					return rootLex;
				}

				//Пошук елемента в системі дерев
				NodeLex* find(
					NodeLex* This, //-V688
					const TString& data,
					size_t index = 0
				) const {
					if (!This) return nullptr;
					auto subnode = find(This->node_root, data[index]);
					if (!subnode) return nullptr;
					if (data.Size - 1 != index) {
						return find(subnode->data, data, index + 1);
					}
					else {
						return subnode->data;
					}
				}

				//Видалення елемента з системи дерев
				NodeLex* remove_node(NodeLex* This, //-V688
					const TString& key,
					bool& removed,
					size_t index = 1
				) {
					if (!This) return nullptr;
					if (key.Size != index - 1) {
						auto subnode = find(This->node_root, key[index - 1]);
						if (!subnode) return This;
						subnode->data = remove_node(subnode->data, key, removed, index + 1);
						if (!subnode->data) return This;
						if (removed) {
							--subnode->data->count;
							if (subnode->data->count == 0) {
								delete subnode->data;
								This->node_root = remove(This->node_root, key[index - 1]);
							}
						}
					}
					else {
						if (This->data == nullptr) {}
						else {
							delete This->data;
							removed = true;
						}
					}
					return This;
				}

				//Деструкція елемента дерева
				NodeLex* destroy_node(NodeLex* This) { //-V688
					if (This != nullptr) {
						if (This->data != nullptr) { //-V809
							delete This->data;
						}
						if (This->node_root != nullptr)
							This->node_root = destroy_tree(This->node_root);
					}
					delete This;
					return nullptr;
				}

				//Деструкція дерева
				NodeSym* destroy_tree(NodeSym* This) { //-V688
					if (This != nullptr) {
						if (This->left != nullptr) {
							if (This->left->data != nullptr) {
								This->left->data = destroy_node(This->left->data);
							}
							This->left = destroy_tree(This->left);
							delete This->left;
						}
						if (This->right != nullptr) {
							if (This->right->data != nullptr) {
								This->right->data = destroy_node(This->right->data);
							}
							This->right = destroy_tree(This->right);
							delete This->right;
						}
						if (This->data != nullptr) {
							This->data = destroy_node(This->data);
						}
						delete This;
					}
					return nullptr;
				}
			private:
				//Кореневий елемент
				struct RootNode : public Memory::Allocator<THeap> {
					NodeLex* node;
					Threading::SRWLock lock;
					RefCountT count;
					RefCountT RefCount;
					__forceinline RootNode() noexcept :node(nullptr), lock(), count(0), RefCount(1) {};
				};
			private:
				//Кореневий елемент
				RootNode* This;
			private:
				//Копіювання елементів піддерева
				NodeSym* create_lex_copy(NodeSym*& orig) const {
					NodeSym* copy = new NodeSym(orig->key);
					if (orig->data != nullptr) copy->data = create_lex_copy(orig->data);
					if (orig->left != nullptr) copy->left = create_lex_copy(orig->left);
					if (orig->right != nullptr) copy->right = create_lex_copy(orig->right);
					copy->height = orig->height;
					return copy;
				}
				//Копіювання піддерева
				NodeLex* create_lex_copy(NodeLex*& orig) const {
					NodeLex* copy = new NodeLex(orig->count);
					if (orig->node_root != nullptr) copy->node_root = create_lex_copy(orig->node_root);
					if (orig->data != nullptr) {
						copy->data = new BasePair<THeap, TString, TData>(*orig->data);
					}
					return copy;
				}
				//Копіювання системи дерев
				RootNode* create_lex_copy(RootNode* This) const { //-V688
					if (This == nullptr) return nullptr;
					RootNode* copy = new RootNode();
					if (This->node != nullptr) copy->node = create_lex_copy(This->node);
					copy->RefCount = This->RefCount;
					return copy;
				}

				void toMap(
					NodeSym* node,
					BaseConcurencyMap<THeap, TString, TData>& list)
				{
					if (node->left != nullptr) toMap(node->left, list);
					if (node->data != nullptr) toMap(node->data, list);
					if (node->right != nullptr) toMap(node->right, list);
				}

				void toMap(
					NodeLex* node,
					BaseConcurencyMap<THeap, TString, TData>& list)
				{
					if (node->data != nullptr)
						list.Insert(*node->data);
					if (node->node_root != nullptr)
						toMap(node->node_root, list);
				}

				void toMap(
					RootNode* This, //-V688
					BaseConcurencyMap<THeap, TString, TData>& list)
				{
					if (This != nullptr)
						if (This->node != nullptr)
							toMap(This->node, list);
				}
			protected:
				virtual void Dispose() final {
					This->lock.ExclusiveLock([&]() {
						This->node = destroy_node(This->node);
						This->count = 0;
						});
				}
			public:
				//Конструктор
				BaseConcurencyDictionary() :This(new RootNode()) {};
				///<summary>Конструктор</summary>
				BaseConcurencyDictionary(InitilizerList<BasePair<THeap, TString, TData>>&& pairs) : This(new RootNode())
				{
					for (auto&& it : pairs) {
						Insert(MoveRValue(it));
					}
				}

				//Вставка елемента в систему по назві і значенню
				BaseConcurencyDictionary& Insert(const TString& key, const TData& value) {
					BasePair<THeap, TString, TData> data{ key, value };
					return This->lock.ExclusiveLock([&]() -> BaseConcurencyDictionary& {
						This->node = insert(This->node, MoveRValue(data));
						++This->count;
						return *this;
						});
				}
				//Вставка елемента в систему по назві і значенню
				BaseConcurencyDictionary& Insert(TString&& key, const TData& value) {
					BasePair<THeap, TString, TData> data{ MoveRValue(key), value };
					return This->lock.ExclusiveLock([&]() -> BaseConcurencyDictionary& {
						This->node = insert(This->node, MoveRValue(data));
						++This->count;
						return *this;
						});
				}
				//Вставка елемента в систему по назві і значенню
				BaseConcurencyDictionary& Insert(const TString& key, TData&& value) {
					BasePair<THeap, TString, TData> data{ key, MoveRValue(value) };
					return This->lock.ExclusiveLock([&]() -> BaseConcurencyDictionary& {
						This->node = insert(This->node, MoveRValue(data));
						++This->count;
						return *this;
						});
				}
				//Вставка елемента в систему по назві і значенню
				BaseConcurencyDictionary& Insert(TString&& key, TData&& value) {
					BasePair<THeap, TString, TData> data{ MoveRValue(key), MoveRValue(value) };
					return This->lock.ExclusiveLock([&]() -> BaseConcurencyDictionary& {
						This->node = insert(This->node, MoveRValue(data));
						++This->count;
						return *this;
						});
				}
				//Вставка елемента в систему по назві і значенню
				BaseConcurencyDictionary& Insert(Pair&& data) {
					return This->lock.ExclusiveLock([&, data = MoveRValue(data)]() mutable->BaseConcurencyDictionary& {
						This->node = insert(This->node, MoveRValue(data));
						++This->count;
						return *this;
					});
				}
				//Вставка елемента в систему по назві і значенню
				BaseConcurencyDictionary& Insert(const Pair& data) {
					return This->lock.ExclusiveLock([&]() mutable -> BaseConcurencyDictionary& {
						This->node = insert(This->node, MoveRValue(Pair(data)));
						++This->count;
						return *this;
						});
				}

				//Пошук елемента за ключом
				__forceinline const TData& Find(const TString& Key) const {
					return This->lock.ShareLock([&]() -> const TData& {
						if (Key.Size == 0)
							Throw(NullPointException(ExceptionPosition));
						if (This == nullptr)
							Throw(NullPointException(ExceptionPosition));
						NodeLex* node = find(This->node, Key);
						if (node == nullptr)
							Throw(NullPointException(ExceptionPosition));
						return node->data->Value;
						});
				}

				//Пошук елемента за ключом
				__forceinline TData& Find(const TString& Key) {
					return This->lock.ShareLock([&]() -> TData& {
						if (Key.Size == 0)
							Throw(NullPointException(ExceptionPosition));
						if (This == nullptr)
							Throw(NullPointException(ExceptionPosition));
						NodeLex* node = find(This->node, Key);
						if (node == nullptr)
							Throw(NullPointException(ExceptionPosition));
						return node->data->Value;
						});
				}

				__forceinline TData& operator[](const TString& key) {
					return Find(key);
				}

				__forceinline TData& operator[](TString&& key) {
					return Find(key);
				}

				__forceinline const TData& operator[](const TString& key) const {
					return Find(key);
				}

				__forceinline const TData& operator[](TString&& key) const {
					return Find(key);
				}

				//Провірка чи дерево пусте
				__forceinline Boolean IsEmpty() const noexcept { return This->RefCount == 0 ? false : true; }

				//Провірки чи є елемент з заданим ключем
				__forceinline Boolean IsExist(const TString& Key) const {
					return This->lock.ShareLock([&]() -> bool {
						if (This == nullptr) return false;
						NodeLex* node = find(This->node, Key);
						return (node == nullptr || node->data == nullptr) ? false : true;
						});
				}

				__forceinline Boolean IsExist(TString&& Key) const {
					return This->lock.ShareLock([&]() -> bool {
						if (This == nullptr) return false;
						NodeLex* node = find(This->node, Key);
						return (node == nullptr || node->data == nullptr) ? false : true;
						});
				}

				//Видалення елемента з системи дерев
				BaseConcurencyDictionary& Remove(const TString& Key) {
					return This->lock.ExclusiveLock([&]() mutable ->BaseConcurencyDictionary& {
						bool flag = false;
						This->node = remove_node(This->node, Key, flag);
						if (flag) --This->node->count; //-V522
						return *this;
						});
				}

				//Кількість елемент в системі
				SizeT GetCount() const noexcept {
					return This->count;
				}

				SizeT Property(GetCount) Count;

				//Повертає ассоціативний масив з лексикона
				BaseConcurencyMap<THeap, TString, TData> toMap() {
					return This->lock.ShareLock([&]() -> BaseConcurencyMap<THeap, TString, TData> {
						BaseConcurencyMap<THeap, TString, TData> ret_val;
						toMap(This, ret_val);
						return ret_val;
						});
				}

				template<typename U>
				friend void Seed::Core::Swap(U& A, U& B);

				virtual BaseConcurencyDictionary Clone() const override {
					return This->lock.ShareLock([&]() {
						BaseConcurencyDictionary result;
						result.This->node = create_lex_copy(This->node);
						result.This->count = This->count;
						return result;
						});
				}

				RefCountClass(BaseConcurencyDictionary, This)
			protected:
#pragma region LINQ
				template<template<typename THeap, typename TKey, typename TValue> typename TCollection, typename THeap, typename TKey, typename TValue, typename TPair, bool IsConcurencySuportT>
				friend class LINQ::IAssociableCollection;
				template<template<typename THeap, typename TKey, typename TValue> typename TCollection, typename THeap, typename TKey, typename TValue, typename TPair, bool IsConcurencySuportT>
				friend class LINQ::IDictionary;
#pragma region For Each
				template<typename Lambda>
				__forceinline static void ForEachImplWithoutIndex(
					NodeSym* node,
					Lambda& lambda)
				{
					if (node->left != nullptr) ForEachImplWithoutIndex(node->left, lambda);
					if (node->data != nullptr) ForEachImplWithoutIndex(node->data, lambda);
					if (node->right != nullptr) ForEachImplWithoutIndex(node->right, lambda);
				}

				template<typename Lambda>
				__forceinline static void ForEachImplWithoutIndex(
					NodeLex* node,
					Lambda& lambda)
				{
					if (node->data != nullptr)
						lambda(*node->data);
					if (node->node_root != nullptr)
						ForEachImplWithoutIndex(node->node_root, lambda);
				}

				template<typename Lambda>
				__forceinline static void ForEachImplWithoutIndex(
					RootNode* This, //-V688
					Lambda& lambda)
				{
					if (This != nullptr)
						if (This->node != nullptr)
							ForEachImplWithoutIndex(This->node, lambda);
				}

				template<typename Lambda>
				__forceinline static void ForEachImplWithIndex(
					NodeSym* node,
					Lambda& lambda,
					SizeT& Index)
				{
					if (node->left != nullptr) ForEachImplWithIndex(node->left, lambda, Index);
					if (node->data != nullptr) ForEachImplWithIndex(node->data, lambda, Index);
					if (node->right != nullptr) ForEachImplWithIndex(node->right, lambda, Index);
				}

				template<typename Lambda>
				__forceinline static void ForEachImplWithIndex(
					NodeLex* node,
					Lambda& lambda,
					SizeT& Index)
				{
					if (node->data != nullptr) {
						lambda(*node->data, Index);
						Index++;
					}
					if (node->node_root != nullptr)
						ForEachImplWithIndex(node->node_root, lambda, Index);
				}

				template<typename Lambda>
				__forceinline static void ForEachImplWithIndex(
					RootNode* This, //-V688
					Lambda& lambda,
					SizeT& Index)
				{
					if (This != nullptr)
						if (This->node != nullptr)
							ForEachImplWithIndex(This->node, lambda, Index);
				}

				template<typename LambdaT>
				__forceinline const BaseConcurencyDictionary& ForEachImpl(LambdaT&& lambda) const {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const BasePair<THeap, TString, TData>&>) {
						return This->lock.ShareLock([&]() -> const BaseConcurencyDictionary& {
							ForEachImplWithoutIndex(This, lambda);
							return *this;
							});
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const BasePair<THeap, TString, TData>&, SizeT>) {
						return This->lock.ShareLock([&]() -> const BaseConcurencyDictionary& {
							SizeT index = 0;
							ForEachImplWithIndex(This, index, lambda);
							return *this;
							});
					}
					else {
						static_assert(false);
					}
				}

				template<typename LambdaT>
				__forceinline BaseConcurencyDictionary& ForEachImpl(LambdaT&& lambda) {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, BasePair<THeap, TString, TData>&> ||
						TypeTraits::IsLambdaConvertibleV<LambdaT, void, const BasePair<THeap, TString, TData>&>) {
						return This->lock.ShareLock([&]() -> BaseConcurencyDictionary& {
							ForEachImplWithoutIndex(This, lambda);
							return *this;
							});
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, BasePair<THeap, TString, TData>&, SizeT> ||
						TypeTraits::IsLambdaConvertibleV<LambdaT, void, const BasePair<THeap, TString, TData>&, SizeT>) {
						return This->lock.ShareLock([&]() -> BaseConcurencyDictionary& {
							SizeT index = 0;
							ForEachImplWithIndex(node, index, lambda);
							return *this;
							});
					}
					else {
						static_assert(false);
					}
				}
#pragma endregion
#pragma region For Each Key
				template<typename Lambda>
				__forceinline static void ForEachKeyImplWithoutIndex(
					NodeSym* node,
					Lambda& lambda)
				{
					if (node->left != nullptr) ForEachKeyImplWithoutIndex(node->left, lambda);
					if (node->data != nullptr) ForEachKeyImplWithoutIndex(node->data, lambda);
					if (node->right != nullptr) ForEachKeyImplWithoutIndex(node->right, lambda);
				}

				template<typename Lambda>
				__forceinline static void ForEachKeyImplWithoutIndex(
					NodeLex* node,
					Lambda& lambda)
				{
					if (node->data != nullptr)
						lambda(node->data->Key);
					if (node->node_root != nullptr)
						ForEachKeyImplWithoutIndex(node->node_root, lambda);
				}

				template<typename Lambda>
				__forceinline static void ForEachKeyImplWithoutIndex(
					RootNode* This, //-V688
					Lambda& lambda)
				{
					if (This != nullptr)
						if (This->node != nullptr)
							ForEachKeyImplWithoutIndex(This->node, lambda);
				}

				template<typename Lambda>
				__forceinline static void ForEachKeyImplWithIndex(
					NodeSym* node,
					Lambda& lambda,
					SizeT& Index)
				{
					if (node->left != nullptr) ForEachKeyImplWithIndex(node->left, lambda, Index);
					if (node->data != nullptr) ForEachKeyImplWithIndex(node->data, lambda, Index);
					if (node->right != nullptr) ForEachKeyImplWithIndex(node->right, lambda, Index);
				}

				template<typename Lambda>
				__forceinline static void ForEachKeyImplWithIndex(
					NodeLex* node,
					Lambda& lambda,
					SizeT& Index)
				{
					if (node->data != nullptr) {
						lambda(node->data->Key, Index);
						Index++;
					}
					if (node->node_root != nullptr)
						ForEachKeyImplWithIndex(node->node_root, lambda, Index);
				}

				template<typename Lambda>
				__forceinline static void ForEachKeyImplWithIndex(
					RootNode* This, //-V688
					Lambda& lambda,
					SizeT& Index)
				{
					if (This != nullptr)
						if (This->node != nullptr)
							ForEachImplWithIndex(This->node, lambda, Index);
				}

				template<typename LambdaT>
				__forceinline const BaseConcurencyDictionary& ForEachKeyImpl(LambdaT&& lambda) const {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const TString&>) {
						return This->lock.ShareLock([&]() -> const BaseConcurencyDictionary& {
							ForEachKeyImplWithoutIndex(This, lambda);
							return *this;
							});
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const TString&, SizeT>) {
						return This->lock.ShareLock([&]() -> const BaseConcurencyDictionary& {
							SizeT index = 0;
							ForEachKeyImplWithIndex(This, index, lambda);
							return *this;
							});
					}
					else {
						static_assert(false);
					}
				}

				template<typename LambdaT>
				__forceinline BaseConcurencyDictionary& ForEachKeyImpl(LambdaT&& lambda) {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const TString&>) {
						return This->lock.ShareLock([&]() -> BaseConcurencyDictionary& {
							ForEachKeyImplWithoutIndex(This, lambda);
							return *this;
							});
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const TString&, SizeT>) {
						return This->lock.ShareLock([&]() -> BaseConcurencyDictionary& {
							SizeT index = 0;
							ForEachKeyImplWithIndex(This, index, lambda);
							return *this;
							});
					}
					else {
						static_assert(false);
					}
				}
#pragma endregion
#pragma region For Each Value
				template<typename Lambda>
				__forceinline static void ForEachValueImplWithoutIndex(
					NodeSym* node,
					Lambda& lambda)
				{
					if (node->left != nullptr) ForEachValueImplWithoutIndex(node->left, lambda);
					if (node->data != nullptr) ForEachValueImplWithoutIndex(node->data, lambda);
					if (node->right != nullptr) ForEachValueImplWithoutIndex(node->right, lambda);
				}

				template<typename Lambda>
				__forceinline static void ForEachValueImplWithoutIndex(
					NodeLex* node,
					Lambda& lambda)
				{
					if (node->data != nullptr)
						lambda(node->data->Value);
					if (node->node_root != nullptr)
						ForEachValueImplWithoutIndex(node->node_root, lambda);
				}

				template<typename Lambda>
				__forceinline static void ForEachValueImplWithoutIndex(
					RootNode* This, //-V688
					Lambda& lambda)
				{
					if (This != nullptr)
						if (This->node != nullptr)
							ForEachValueImplWithoutIndex(This->node, lambda);
				}

				template<typename Lambda>
				__forceinline static void ForEachValueImplWithIndex(
					NodeSym* node,
					Lambda& lambda,
					SizeT& Index)
				{
					if (node->left != nullptr) ForEachValueImplWithIndex(node->left, lambda, Index);
					if (node->data != nullptr) ForEachValueImplWithIndex(node->data, lambda, Index);
					if (node->right != nullptr) ForEachValueImplWithIndex(node->right, lambda, Index);
				}

				template<typename Lambda>
				__forceinline static void ForEachValueImplWithIndex(
					NodeLex* node,
					Lambda& lambda,
					SizeT& Index)
				{
					if (node->data != nullptr) {
						lambda(node->data->Value, Index);
						Index++;
					}
					if (node->node_root != nullptr)
						ForEachValueImplWithIndex(node->node_root, lambda, Index);
				}

				template<typename Lambda>
				__forceinline static void ForEachValueImplWithIndex(
					RootNode* This, //-V688
					Lambda& lambda,
					SizeT& Index)
				{
					if (This != nullptr)
						if (This->node != nullptr)
							ForEachValueImplWithIndex(This->node, lambda, Index);
				}

				template<typename LambdaT>
				__forceinline const BaseConcurencyDictionary& ForEachValueImpl(LambdaT&& lambda) const {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const TData&>) {
						return This->lock.ShareLock([&]() -> const BaseConcurencyDictionary& {
							ForEachValueImplWithoutIndex(This, lambda);
							return *this;
							});
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const TData&, SizeT>) {
						return tree->lock.ShareLock([&]() -> const BaseConcurencyDictionary& {
							SizeT index = 0;
							ForEachValueImplWithIndex(This, index, lambda);
							return *this;
							});
					}
					else {
						static_assert(false);
					}
				}

				template<typename LambdaT>
				__forceinline BaseConcurencyDictionary& ForEachValueImpl(LambdaT&& lambda) {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, TData&> ||
						TypeTraits::IsLambdaConvertibleV<LambdaT, void, const TData&>) {
						return This->lock.ShareLock([&]() -> BaseConcurencyDictionary& {
							ForEachValueImplWithoutIndex(This, lambda);
							return *this;
							});
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, TData&, SizeT> ||
						TypeTraits::IsLambdaConvertibleV<LambdaT, void, const TData&, SizeT>) {
						return tree->lock.ShareLock([&]() -> BaseConcurencyDictionary& {
							SizeT index = 0;
							ForEachValueImplWithIndex(This, index, lambda);
							return *this;
							});
					}
					else {
						static_assert(false);
					}
				}
#pragma endregion
			protected:
				//Деструкція елемента дерева
				template<typename Lambda>
				NodeLex* destroy_node(NodeLex* This, Lambda& lambda) { //-V688
					if (This != nullptr) {
						if (This->data != nullptr) { //-V809
							lambda(MoveRValue(*This->data));
							delete This->data;
						}
						if (This->node_root != nullptr)
							This->node_root = destroy_tree(This->node_root, lambda);
					}
					delete This;
					return nullptr;
				}

				//Деструкція дерева
				template<typename Lambda>
				NodeSym* destroy_tree(NodeSym* This, Lambda& lambda) { //-V688
					if (This != nullptr) {
						if (This->left != nullptr) {
							if (This->left->data != nullptr) {
								This->left->data = destroy_node(This->left->data, lambda);
							}
							This->left = destroy_tree(This->left);
							delete This->left;
						}
						if (This->right != nullptr) {
							if (This->right->data != nullptr) {
								This->right->data = destroy_node(This->right->data, lambda);
							}
							This->right = destroy_tree(This->right);
							delete This->right;
						}
						if (This->data != nullptr) {
							This->data = destroy_node(This->data, lambda);
						}
						delete This;
					}
					return nullptr;
				}

				__forceinline BaseConcurencyDictionary& RemoveAllImpl() {
					this->Dispose();
					return *this;
				}

				template<typename Lambda>
				__forceinline BaseConcurencyDictionary& RemoveAllWithImpl(Lambda&& lambda) {
					return This->lock.ShareLock([&]() -> BaseConcurencyDictionary& {
						This->root = destroy_tree(This->root, lambda);
						This->count = 0;
						return *this;
						});
				}
#pragma endregion
			};

			//Швидкісний словник з стандартним алокатором пам'яті
			template<typename T>
			using ConcurencyDictionary = BaseConcurencyDictionary<Memory::Heap, BaseString<Memory::Heap>, T>;

			//Швидкісний словник з захищеним алокатором пам'яті
			template<typename T>
			using ConcurencySensitiveDictionary = BaseConcurencyDictionary<Memory::SensitiveDataHeap, BaseString<Memory::Heap>, T>;
		}

		namespace TypeTraits {
			namespace Collection {
				template<typename THeap, typename TString, typename T>
				struct IsDictionaryT<Seed::Core::Containers::BaseConcurencyDictionary<THeap, TString, T>> : LogicTrue {};
			}
		}

		template<typename THeap, typename TString, typename T>
		__forceinline void Swap(Containers::BaseConcurencyDictionary<THeap, TString, T>& A, Containers::BaseConcurencyDictionary<THeap, TString, T>& B) {
			Swap(A.This, B.This);
		}
	}
}