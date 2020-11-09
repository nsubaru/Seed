#pragma once
#include "Seed.Core.Containers.TypeTraits.h"
#include "Seed.Core.Containers.Pair.h"
#include "Seed.Core.Containers.API.LINQ.h"
#include "Seed.Core.Containers.NonConcurency.Map.h"
#include "Seed.Core.String.h"
#include "Seed.Core.Exceptions.h"

namespace Seed {
	inline namespace Core {
		namespace Containers {
			/// <summary>
			/// Шаблон словника слів який виконує швидкісний пошук слова
			/// </summary>
			template<typename THeap, typename TString, typename TData>
			class BaseDictionary :
				public Memory::Allocator<THeap>,
				public LINQ::IDictionary<BaseDictionary, THeap, TString, TData, BasePair<THeap, TString, TData>, true>,
				public IDisposable,
				public IClonable<BaseDictionary<THeap, TString, TData>>
			{
				static_assert(TypeTraits::IsSameV<TString, BaseString<THeap>>, "BaseDictionary key must be BaseString<THeap>");
			private:
				using Pair = BasePair<THeap, TString, TData>;
				//Структура букви в складі AVL-дерева
				struct NodeSym;

				//Структура піддервева
				struct NodeLex : public Memory::Allocator<THeap> {
					RefCountT count;
					NodeSym* node_root;
					BasePair<THeap, TString, TData>* data;

					ForceInline NodeLex(SizeT count = 0) :node_root(nullptr), count(count), data(nullptr) {};
				};

				//Структура букви в складі AVL-дерева
				struct NodeSym : public Memory::Allocator<THeap> {
					UInt8 height;
					Char key;
					NodeLex* data;
					NodeSym* left;
					NodeSym* right;

					ForceInline NodeSym(Char key) :key(key), height(1), left(nullptr), right(nullptr), data(nullptr) {}
				};

				//Висота дерева
				ForceInline UInt8 height(NodeSym* node) {
					return node ? node->height : 0;
				}

				//Різниця висот лівої і правої підгілки
				ForceInline Int8 bfactor(NodeSym* node) {
					return height(node->right) - height(node->left);
				}

				//Переобчислення висоти
				ForceInline void fixheight(NodeSym* node) {
					auto h_left = height(node->left);
					auto h_right = height(node->right);
					node->height = ((h_left < h_right) ? h_right : h_left) + 1;
				}

				//Правий поворо піддерева
				ForceInline NodeSym* rotateright(NodeSym* p) {
					NodeSym* q = p->left;
					p->left = q->right;
					q->right = p;
					fixheight(p);
					fixheight(q);
					return q;
				}

				//Лівий поворот піддерева
				ForceInline NodeSym* rotateleft(NodeSym* q) {
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
				ForceInline NodeSym* insert(NodeSym* p, wchar_t key) {
					if (!p) return new NodeSym(key);
					if (key == p->key) return p;
					if (key < p->key)
						p->left = insert(p->left, key);
					else
						p->right = insert(p->right, key);
					return balance(p);
				}

				//Пошук мінімального елемента в дереві
				ForceInline NodeSym* findmin(NodeSym* p) {
					return p->left ? findmin(p->left) : p;
				}

				//Видалення мінімального елемента в дереві
				ForceInline NodeSym* removemin(NodeSym* p) {
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
				ForceInline NodeSym* find(NodeSym* root, Char key) const { //-V688
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
					if (data.Key.Lenght != index) {
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
					if (data.Lenght - 1 != index) {
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
					if (key.Lenght != index - 1) {
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
					RefCountT count;
					RefCountT RefCount;

					ForceInline RootNode() :node(nullptr), count(0), RefCount(1) {};
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

				void ToMap(
					NodeSym* node,
					BaseMap<THeap, TString, TData>& list)
				{
					if (node->left != nullptr) ToMap(node->left, list);
					if (node->data != nullptr) ToMap(node->data, list);
					if (node->right != nullptr) ToMap(node->right, list);
				}

				void ToMap(
					NodeLex* node,
					BaseMap<THeap, TString, TData>& list)
				{
					if (node->data != nullptr)
						list.Insert(*node->data);
					if (node->node_root != nullptr)
						ToMap(node->node_root, list);
				}

				void ToMap(
					RootNode* This, //-V688
					BaseMap<THeap, TString, TData>& list)
				{
					if (This != nullptr)
						if (This->node != nullptr)
							ToMap(This->node, list);
				}
			protected:
				ForceInline virtual void Dispose() final {
					This->node = destroy_node(This->node);
					This->count = 0;
				}
			public:
				/// <summary>
				/// Пустий конструктор який ініціалізовує пустий словник
				/// </summary>
				ForceInline BaseDictionary() :This(new RootNode()) {};

				/// <summary>
				/// Конструктор з списку пар і ключів
				/// </summary>
				/// <param name="pairs">Список пар і ключів</param>
				BaseDictionary(InitilizerList<BasePair<THeap, TString, TData>>&& pairs) : This(new RootNode())
				{
					for (auto&& it : pairs) {
						Insert(MoveRValue(it));
					}
				}

				
				/// <summary>
				/// Вставка елемента в словник по ключу і значенню
				/// </summary>
				/// <param name="key">Ключ</param>
				/// <param name="value">Значення</param>
				/// <returns>Силка на поточний об'єкт</returns>
				ForceInline BaseDictionary& Insert(const TString& key, const TData& value) {
					BasePair<THeap, TString, TData> data{ key, value };
					This->node = insert(This->node, MoveRValue(data));
					++This->count;
					return *this;
				}

				
				/// <summary>
				/// Вставка елемента в словник по ключу і значенню
				/// </summary>
				/// <param name="key">Ключ</param>
				/// <param name="value">Значення</param>
				/// <returns>Силка на поточний об'єкт</returns>
				ForceInline BaseDictionary& Insert(TString&& key, const TData& value) {
					BasePair<THeap, TString, TData> data{ MoveRValue(key), value };
					This->node = insert(This->node, MoveRValue(data));
					++This->count;
					return *this;
				}
				
				/// <summary>
				/// Вставка елемента в словник по ключу і значенню
				/// </summary>
				/// <param name="key">Ключ</param>
				/// <param name="value">Значення</param>
				/// <returns>Силка на поточний об'єкт</returns>
				ForceInline BaseDictionary& Insert(const TString& key, TData&& value) {
					BasePair<THeap, TString, TData> data{ key, MoveRValue(value) };
					This->node = insert(This->node, MoveRValue(data));
					++This->count;
					return *this;
				}
				
				/// <summary>
				/// Вставка елемента в словник по ключу і значенню
				/// </summary>
				/// <param name="key">Ключ</param>
				/// <param name="value">Значення</param>
				/// <returns>Силка на поточний об'єкт</returns>
				ForceInline BaseDictionary& Insert(TString&& key, TData&& value) {
					BasePair<THeap, TString, TData> data{ MoveRValue(key), MoveRValue(value) };
					This->node = insert(This->node, MoveRValue(data));
					++This->count;
					return *this;
				}
				
				/// <summary>
				/// Вставка елемента в словник по ключу і значенню
				/// </summary>
				/// <param name="data">Пара ключ і значення</param>
				/// <returns>Силка на поточний об'єкт</returns>
				ForceInline BaseDictionary& Insert(Pair&& data) {
					This->node = insert(This->node, MoveRValue(data));
					++This->count;
					return *this;
				}
				
				/// <summary>
				/// Вставка елемента в словник по ключу і значенню
				/// </summary>
				/// <param name="data">Пара ключ і значення</param>
				/// <returns>Силка на поточний об'єкт</returns>
				ForceInline BaseDictionary& Insert(const Pair& data) {
					This->node = insert(This->node, MoveRValue(Pair(data)));
					++This->count;
					return *this;
				}

				/// <summary>
				/// Пошук значення по ключу
				/// </summary>
				/// <param name="Key">Ключ</param>
				/// <returns>Константна силка на значення</returns>
				ForceInline const TData& Find(const TString& Key) const {
					if (Key.Lenght == 0)
						Throw(NullPointException(ExceptionPosition));
					if (This == nullptr)
						Throw(NotFoundException(ExceptionPosition, L"Invalid Key"));
					NodeLex* node = find(This->node, Key);
					if (node == nullptr)
						Throw(NotFoundException(ExceptionPosition, L"Invalid Key"));
					return node->data->Value;
				}

				/// <summary>
				/// Пошук значення по ключу
				/// </summary>
				/// <param name="Key">Ключ</param>
				/// <returns>Силка на значення</returns>
				ForceInline TData& Find(const TString& Key) {
					if (Key.Lenght == 0)
						Throw(NullPointException(ExceptionPosition));
					if (This == nullptr)
						Throw(NotFoundException(ExceptionPosition, L"Invalid Key"));
					NodeLex* node = find(This->node, Key);
					if (node == nullptr)
						Throw(NotFoundException(ExceptionPosition, L"Invalid Key"));
					return node->data->Value;
				}

				/// <summary>
				/// Оператор вибірки елемента по ключу
				/// </summary>
				/// <param name="key">Ключ</param>
				/// <returns>Силка на значення</returns>
				ForceInline TData& operator[](const TString& key) {
					return Find(key);
				}

				/// <summary>
				/// Оператор вибірки елемента по ключу
				/// </summary>
				/// <param name="key">Ключ</param>
				/// <returns>Силка на значення</returns>
				ForceInline TData& operator[](TString&& key) {
					return Find(key);
				}

				/// <summary>
				/// Константний оператор вибірки елемента по ключу
				/// </summary>
				/// <param name="key">Ключ</param>
				/// <returns>Константна силка на значення</returns>
				ForceInline const TData& operator[](const TString& key) const {
					return Find(key);
				}

				/// <summary>
				/// Константний оператор вибірки елемента по ключу
				/// </summary>
				/// <param name="key">Ключ</param>
				/// <returns>Константна силка на значення</returns>
				ForceInline const TData& operator[](TString&& key) const {
					return Find(key);
				}

				/// <summary>
				/// Провірка чи словник пустий
				/// </summary>
				/// <returns>true якщо словник пустий</returns>
				ForceInline Boolean IsEmpty() const { return This->RefCount == 0 ? false : true; }

				/// <summary>
				/// Перевірка чи є елемент з заданим ключом в словнику
				/// </summary>
				/// <param name="Key">Ключ по якому шукають елемент</param>
				/// <returns>true якщо елемент існує</returns>
				ForceInline Boolean IsExist(const TString& Key) const {
					if (This == nullptr) return false;
					NodeLex* node = find(This->node, Key);
					return (node == nullptr || node->data == nullptr) ? false : true;
				}

				/// <summary>
				/// Перевірка чи є елемент з заданим ключом в словнику
				/// </summary>
				/// <param name="Key">Ключ по якому шукають елемент</param>
				/// <returns>true якщо елемент існує</returns>
				ForceInline Boolean IsExist(TString&& Key) const {
					if (This == nullptr) return false;
					NodeLex* node = find(This->node, Key);
					return (node == nullptr || node->data == nullptr) ? false : true;
				}

				/// <summary>
				/// Видалення елемента по ключу
				/// </summary>
				/// <param name="Key">Ключ по якому елемент шукається</param>
				/// <returns>Силка на поточний об'єкт</returns>
				BaseDictionary& Remove(const TString& Key) {
					bool flag = false;
					This->node = remove_node(This->node, Key, flag);
					if (flag) --This->node->count; //-V522
					return *this;
				}

				/// <summary>
				/// Кількість елементів в словнику
				/// </summary>
				/// <returns>Поточна кількість елементів в словнику</returns>
				ForceInline SizeT GetCount() const {
					return This->count;
				}

				/// <summary>
				/// Поточна кількість елементів в словнику
				/// </summary>
				SizeT Property(GetCount) Count;

				/// <summary>
				/// Копіювання елементів в ассоціативний масив
				/// </summary>
				/// <returns>Асоціативний масив з елементами з словника</returns>
				ForceInline BaseMap<THeap, TString, TData> ToMap() {
					BaseMap<THeap, TString, TData> ret_val;
					ToMap(This, ret_val);
					return ret_val;
				}

				/// <summary>
				/// Клонування об'єкта
				/// </summary>
				/// <returns>Вертає новий словник з копією елементів</returns>
				ForceInline virtual BaseDictionary Clone() const override {
					BaseDictionary result;
					result.This->node = create_lex_copy(This->node);
					result.This->count = This->count;
					return result;
				}

				template<typename U>
				friend void Seed::Core::Swap(U& A, U& B);

				RefCountClass(BaseDictionary, This)
			protected:
#pragma region LINQ
				template<template<typename THeap, typename TKey, typename TValue> typename TCollection, typename THeap, typename TKey, typename TValue, typename TPair, bool IsConcurencySuportT>
				friend class LINQ::IAssociableCollection;
				template<template<typename THeap, typename TKey, typename TValue> typename TCollection, typename THeap, typename TKey, typename TValue, typename TPair, bool IsConcurencySuportT>
				friend class LINQ::IDictionary;
                template<typename THeap, typename T, typename Enable>
                friend struct Seed::Core::Serilization::Details::BaseSerlizationHandler;
#pragma region For Each
				template<typename Lambda>
				ForceInline static void ForEachImplWithoutIndex(
					NodeSym* node,
					Lambda& lambda)
				{
					if (node->left != nullptr) ForEachImplWithoutIndex(node->left, lambda);
					if (node->data != nullptr) ForEachImplWithoutIndex(node->data, lambda);
					if (node->right != nullptr) ForEachImplWithoutIndex(node->right, lambda);
				}

				template<typename Lambda>
				static void ForEachImplWithoutIndex(
					NodeLex* node,
					Lambda& lambda)
				{
					if (node->data != nullptr)
						lambda(*node->data);
					if (node->node_root != nullptr)
						ForEachImplWithoutIndex(node->node_root, lambda);
				}

				template<typename Lambda>
				ForceInline static void ForEachImplWithoutIndex(
					RootNode* This, //-V688
					Lambda& lambda)
				{
					if (This != nullptr)
						if (This->node != nullptr)
							ForEachImplWithoutIndex(This->node, lambda);
				}

				template<typename Lambda>
				ForceInline static void ForEachImplWithIndex(
					NodeSym* node,
					Lambda& lambda,
					SizeT& Index)
				{
					if (node->left != nullptr) ForEachImplWithIndex(node->left, lambda, Index);
					if (node->data != nullptr) ForEachImplWithIndex(node->data, lambda, Index);
					if (node->right != nullptr) ForEachImplWithIndex(node->right, lambda, Index);
				}

				template<typename Lambda>
				static void ForEachImplWithIndex(
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
				ForceInline static void ForEachImplWithIndex(
					RootNode* This, //-V688
					Lambda& lambda,
					SizeT& Index)
				{
					if (This != nullptr)
						if (This->node != nullptr)
							ForEachImplWithIndex(This->node, lambda, Index);
				}

				template<typename LambdaT>
				ForceInline const BaseDictionary& ForEachImpl(LambdaT&& lambda) const {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const BasePair<THeap, TString, TData>&>) {
						ForEachImplWithoutIndex(This, lambda);
						return *this;
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const BasePair<THeap, TString, TData>&, SizeT>) {
						SizeT index = 0;
						ForEachImplWithIndex(This, index, lambda);
						return *this;
					}
					else {
						static_assert(false);
					}
				}

				template<typename LambdaT>
				ForceInline BaseDictionary& ForEachImpl(LambdaT&& lambda) {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, BasePair<THeap, TString, TData>&> ||
						TypeTraits::IsLambdaConvertibleV<LambdaT, void, const BasePair<THeap, TString, TData>&>) {
						ForEachImplWithoutIndex(This, lambda);
						return *this;
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, BasePair<THeap, TString, TData>&, SizeT> ||
						TypeTraits::IsLambdaConvertibleV<LambdaT, void, const BasePair<THeap, TString, TData>&, SizeT>) {
						SizeT index = 0;
						ForEachImplWithIndex(This, index, lambda);
						return *this;
					}
					else {
						static_assert(false);
					}
				}
#pragma endregion
#pragma region For Each Key
				template<typename Lambda>
				ForceInline static void ForEachKeyImplWithoutIndex(
					NodeSym* node,
					Lambda& lambda)
				{
					if (node->left != nullptr) ForEachKeyImplWithoutIndex(node->left, lambda);
					if (node->data != nullptr) ForEachKeyImplWithoutIndex(node->data, lambda);
					if (node->right != nullptr) ForEachKeyImplWithoutIndex(node->right, lambda);
				}

				template<typename Lambda>
				static void ForEachKeyImplWithoutIndex(
					NodeLex* node,
					Lambda& lambda)
				{
					if (node->data != nullptr)
						lambda(node->data->Key);
					if (node->node_root != nullptr)
						ForEachKeyImplWithoutIndex(node->node_root, lambda);
				}

				template<typename Lambda>
				ForceInline static void ForEachKeyImplWithoutIndex(
					RootNode* This, //-V688
					Lambda& lambda)
				{
					if (This != nullptr)
						if (This->node != nullptr)
							ForEachKeyImplWithoutIndex(This->node, lambda);
				}

				template<typename Lambda>
				ForceInline static void ForEachKeyImplWithIndex(
					NodeSym* node,
					Lambda& lambda,
					SizeT& Index)
				{
					if (node->left != nullptr) ForEachKeyImplWithIndex(node->left, lambda, Index);
					if (node->data != nullptr) ForEachKeyImplWithIndex(node->data, lambda, Index);
					if (node->right != nullptr) ForEachKeyImplWithIndex(node->right, lambda, Index);
				}

				template<typename Lambda>
				static void ForEachKeyImplWithIndex(
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
				ForceInline static void ForEachKeyImplWithIndex(
					RootNode* This, //-V688
					Lambda& lambda,
					SizeT& Index)
				{
					if (This != nullptr)
						if (This->node != nullptr)
							ForEachImplWithIndex(This->node, lambda, Index);
				}

				template<typename LambdaT>
				ForceInline const BaseDictionary& ForEachKeyImpl(LambdaT&& lambda) const {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const TString&>) {
						ForEachKeyImplWithoutIndex(This, lambda);
						return *this;
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const TString&, SizeT>) {
						SizeT index = 0;
						ForEachKeyImplWithIndex(This, index, lambda);
						return *this;
					}
					else {
						static_assert(false);
					}
				}

				template<typename LambdaT>
				ForceInline BaseDictionary& ForEachKeyImpl(LambdaT&& lambda) {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const TString&>) {
						ForEachKeyImplWithoutIndex(This, lambda);
						return *this;
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const TString&, SizeT>) {
						SizeT index = 0;
						ForEachKeyImplWithIndex(This, index, lambda);
						return *this;
					}
					else {
						static_assert(false);
					}
				}
#pragma endregion
#pragma region For Each Value
				template<typename Lambda>
				ForceInline static void ForEachValueImplWithoutIndex(
					NodeSym* node,
					Lambda& lambda)
				{
					if (node->left != nullptr) ForEachValueImplWithoutIndex(node->left, lambda);
					if (node->data != nullptr) ForEachValueImplWithoutIndex(node->data, lambda);
					if (node->right != nullptr) ForEachValueImplWithoutIndex(node->right, lambda);
				}

				template<typename Lambda>
				static void ForEachValueImplWithoutIndex(
					NodeLex* node,
					Lambda& lambda)
				{
					if (node->data != nullptr)
						lambda(node->data->Value);
					if (node->node_root != nullptr)
						ForEachValueImplWithoutIndex(node->node_root, lambda);
				}

				template<typename Lambda>
				ForceInline static void ForEachValueImplWithoutIndex(
					RootNode* This, //-V688
					Lambda& lambda)
				{
					if (This != nullptr)
						if (This->node != nullptr)
							ForEachValueImplWithoutIndex(This->node, lambda);
				}

				template<typename Lambda>
				ForceInline static void ForEachValueImplWithIndex(
					NodeSym* node,
					Lambda& lambda,
					SizeT& Index)
				{
					if (node->left != nullptr) ForEachValueImplWithIndex(node->left, lambda, Index);
					if (node->data != nullptr) ForEachValueImplWithIndex(node->data, lambda, Index);
					if (node->right != nullptr) ForEachValueImplWithIndex(node->right, lambda, Index);
				}

				template<typename Lambda>
				static void ForEachValueImplWithIndex(
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
				ForceInline static void ForEachValueImplWithIndex(
					RootNode* This, //-V688
					Lambda& lambda,
					SizeT& Index)
				{
					if (This != nullptr)
						if (This->node != nullptr)
							ForEachValueImplWithIndex(This->node, lambda, Index);
				}

				template<typename LambdaT>
				ForceInline const BaseDictionary& ForEachValueImpl(LambdaT&& lambda) const {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const TData&>) {
						ForEachValueImplWithoutIndex(This, lambda);
						return *this;
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const TData&, SizeT>) {
						SizeT index = 0;
						ForEachValueImplWithIndex(This, index, lambda);
						return *this;
					}
					else {
						static_assert(false);
					}
				}

				template<typename LambdaT>
				ForceInline BaseDictionary& ForEachValueImpl(LambdaT&& lambda) {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, TData&> ||
						TypeTraits::IsLambdaConvertibleV<LambdaT, void, const TData&>) {
						ForEachValueImplWithoutIndex(This, lambda);
						return *this;
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, TData&, SizeT> ||
						TypeTraits::IsLambdaConvertibleV<LambdaT, void, const TData&, SizeT>) {
						SizeT index = 0;
						ForEachValueImplWithIndex(This, index, lambda);
						return *this;
					}
					else {
						static_assert(false);
					}
				}
#pragma endregion
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

				ForceInline BaseDictionary& RemoveAllImpl() {
					This->node = destroy_node(This->node);
					This->count = 0;
					return *this;
				}

				template<typename Lambda>
				BaseDictionary& RemoveAllWithImpl(Lambda&& lambda) {
					return This->lock.ShareLock([&]() -> BaseDictionary& {
						This->root = destroy_tree(This->root, lambda);
						This->count = 0;
						return *this;
						});
				}

				ForceInline void UnsafeAdd(Pair&& pair){
					Insert(MoveRValue(pair));
				}

				ForceInline void UnsafeAdd(const Pair& pair){
					Insert(pair);
				}
#pragma endregion
			};

			/// <summary>
			/// Словник з швидкісним пошуком елементів по ключу
			/// </summary>
			namespace
			{
				template<typename T>
				using Dictionary = BaseDictionary<Memory::Heap, BaseString<Memory::Heap>, T>;
			}

			/// <summary>
			/// Словник з швидкісним пошуком елементів по ключу та захищеним алокатором пам'яті
			/// </summary>
			namespace
			{
				template<typename T>
				using SensitiveDictionary = BaseDictionary<Memory::SensitiveDataHeap, BaseString<Memory::Heap>, T>;
			}
		}

		namespace TypeTraits {
			namespace Collection {
				template<typename THeap, typename TString, typename T>
				struct IsDictionaryT<Seed::Core::Containers::BaseDictionary<THeap, TString, T>> : LogicTrue {};
			}
		}

		template<typename THeap, typename TString, typename T>
		ForceInline void Swap(Containers::BaseDictionary<THeap, TString, T>& A, Containers::BaseDictionary<THeap, TString, T>& B) {
			Swap(A.This, B.This);
		}
	}
}