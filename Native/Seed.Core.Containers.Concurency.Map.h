#pragma once
#include "Seed.Core.Containers.TypeTraits.h"
#include "Seed.Core.Containers.Pair.h"
#include "Seed.Core.Containers.API.LINQ.h"
#include "Seed.Core.ConstString.h"

namespace Seed {
	inline namespace Core {
		namespace Containers {
			template<typename THeap, typename TKey, typename TValue>
			class SeedKernelCall BaseConcurencyMap :
				public LINQ::IAssociableCollection<BaseConcurencyMap, THeap, TKey, TValue, BasePair<THeap, TKey, TValue>, true>,
				protected IDisposable,
				public IClonable<BaseConcurencyMap<THeap, TKey, TValue>>
			{
				struct Tree;

				///<summary>Елемент дерева і елемент в асоціативному масиві</summary>
				struct SeedKernelCall Node : public Memory::Allocator<THeap>
				{
				private:
					///<summary>Ключ і значення елемента</summary>
					BasePair<THeap, TKey, TValue> data;

					///<summary>Гетер ключа</summary>
					const TKey& GetKey() const { return data.Key; }
					///<summary>Гетер значення</summary>
					TValue& GetValue() { return data.Value; }
					///<summary>Гетер значення</summary>
					const TValue& GetValue() const { return data.Value; }
					///<summary>Сеттер значення</summary>
					void SetValue(const TValue& newValue) { data.Value = newValue; }
					///<summary>Оптимізований сеттер значення</summary>
					void SetValue(TValue&& newValue) { data.Value = MoveRValue(newValue); }

					///<summary>Ключ</summary>
					TKey Property(GetKey) key;
					///<summary>Значення</summary>
					TValue Property(GetValue, SetValue) value;

					///<summary>Віддаленість елемента від корення дерева(висота)</summary>
					UInt16 height;

					///<summary>Покажчик на батьківський елемент</summary>
					Node* father;
					///<summary>Покажчик на лівий елемент</summary>
					Node* left;
					///<summary>Покажчик на правий елемент</summary>
					Node* right;

					Tree* This;

					///<summary>Кількіст силок на даний елемент</summary>
					RefCountT RefCount;

					///<summary>Конструктор копіювання ключа і значення</summary>
					Node(const TKey& key, const TValue& value, Node* father, Tree* This) : data(BasePair<THeap, TKey, TValue>(key, value)), height(1), father(father), This(This), right(nullptr), left(nullptr), RefCount(1)
					{};

					///<summary>Конструктор переміщення значення і копіювання значення</summary>
					Node(TKey&& key, const TValue& value, Node* father, Tree* This) : data(BasePair<THeap, TKey, TValue>((MoveRValue(key)), value)), height(1), father(father), This(This), right(nullptr), left(nullptr), RefCount(1)
					{}

					///<summary>Конструктор копіювання значення і переміщення значення</summary>
					Node(const TKey& key, TValue&& value, Node* father, Tree* This) : data(BasePair<THeap, TKey, TValue>(key, MoveRValue(value))), height(1), father(father), This(This), right(nullptr), left(nullptr), RefCount(1)
					{};

					///<summary>Конструктор переміщення і значення і ключа</summary>
					Node(TKey&& key, TValue&& value, Node* father, Tree* This) : data(BasePair<THeap, TKey, TValue>(MoveRValue(key), MoveRValue(value))), height(1), father(father), This(This), right(nullptr), left(nullptr), RefCount(1)
					{};

					///<summary>Конструктор копіювання пари ключ і значення</summary>
					Node(const BasePair<THeap, TKey, TValue>& data, Node* father, Tree* This) : data(data), height(1), father(father), This(This), right(nullptr), left(nullptr), RefCount(1)
					{};

					///<summary>Конструктор переміщення пари ключ і значення</summary>
					Node(BasePair<THeap, TKey, TValue>&& data, Node* father, Tree* This) : data(MoveRValue(data)), height(1), father(father), This(This), right(nullptr), left(nullptr), RefCount(1)
					{};

					friend BaseConcurencyMap;
					friend BaseConcurencyMap::Iterator;
				};

				///<summary>Дані дерева</summary>
				struct Tree {
					///<summary>Покажчик на кореневий елемент дерева</summary>
					Node* root;
					///<summary>Кількість елементва в дереві</summary>
					RefCountT count;
					///<summary>RWLock - для синхронізації читання запису
					///<para>Він захищає структуру дерева, а не значення елемента</para>
					///</summary>
					Threading::SRWLock lock;

					RefCountT RefCount;

					Tree() : root(nullptr), count(0), lock(), RefCount(1) {}
				};
			private:
				///<summary>Дані об'єкта</summary>
				Tree* This;
			protected:
				virtual void Dispose() final override {
					This->lock.ExclusiveLock([&]() {
						DestroyTree(This->root);
						});
				}
			private:
				///<summary>Рекурсивний пошук елемента по ключу в дереві</summary>
				///<param name = "node">Кореневий в якому шукає</param>
				///<param name = "key">Ключ по якому шукає</param>
				///<returns>Елемент дерева або <c>nullptr<c></returns>
				Node* Find(Node* node, const TKey& key) const {
					if (node == nullptr)
						return nullptr;
					if (node->key == key)
						return node;
					else
						if (node->key > key)
							return Find(node->right, key);
						else
							return Find(node->left, key);
				}

				///<summary>Обчислення висоти дерева</summary>
				///<param name = "node">Елемент для якого відбувається обчилення</summary>
				///<returns>Значення висоти</summary>
				UInt16 Height(Node* node)
				{
					return node ? node->height : 0;
				}
				///<summary>Обчислює різницю висот правого і лівого під елемента</summary>
				///<param name = "node">Батьківський елемент</param>
				///<returns>Відємне значення вказує що у правого вища висота, додатнє - у лівого, 0 рівні по висоті</returns>
				Int16 BFactor(Node* node)
				{
					return  Height(node->right) - Height(node->left);
				}

				///<summary>Переобчислює висоту даного елемента</summary>
				///<param name = "node">Елемент для якого оновлюється значення висоти</param>
				void FixHeight(Node* node)
				{
					UInt16 HLeft = Height(node->left);
					UInt16 HRight = Height(node->right);
					node->height = Max(HLeft, HRight) + 1;
				}

				///<summary>Правий малий поворот дерева</summary>
				///<param name = "node">Елемент відносно якого відбувається поворот</param>
				///<returns>Новий батьківський елемент</summary>
				Node* RotateRight(Node* node)
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

				///<summary>Лвий малий поворот дерева</summary>
				///<param name = "node">Елемент відносно якого відбувається поворот</param>
				///<returns>Новий батьківський елемент</summary>
				Node* RotateLeft(Node* node)
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

				///<summary>Балансування дерева
				///<para>Зводиться до переобчилення висоти і виклику відповідних поворотів
				///Великі повороти реалізовуються через пару малих
				///</summary>
				///<param name = "node">Елемент відносно якого відбувається балансування</param>
				///<returns>Новий батьківський елемент</summary>
				Node* Balance(Node* node)
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

				///<summary>Вставка нового елемента</summary>
				///<param name = "node">Батьківський елемент</param>
				///<param name = "key">Новий ключ</param>
				///<param name = "value">Нове значення</param>
				///<returns>Новий батьківський елемент</returns>
				///<exception cref = "Seed::KeyAllredyExistException">Ключ існує в асоціативному масиві</exception>
				Node* Insert(Node* node, const TKey& key, const TValue& value, Node* father)
				{
					if (!node) return new Node(key, value, father, this->This);
					if (key < node->key)
						node->left = Insert(node->left, key, value, node);
					else
						if (key != node->key)
							node->right = Insert(node->right, key, value, node);
						else
							Internals::Exceptions::ThrowKeyAlredyExist(__FILE__, __FUNCSIG__, __LINE__);
					return Balance(node);
				}

				///<summary>Вставка нового елемента</summary>
				///<param name = "node">Батьківський елемент</param>
				///<param name = "key">Новий ключ</param>
				///<param name = "value">Нове значення</param>
				///<returns>Новий батьківський елемент</returns>
				///<exception cref = "Seed::KeyAllredyExistException">Ключ існує в асоціативному масиві</exception>
				Node* Insert(Node* node, TKey&& key, const TValue& value, Node* father)
				{
					if (!node) return new Node(MoveRValue(key), value, father, this->This);
					if (key < node->key)
						node->left = Insert(node->left, MoveRValue(key), value, node);
					else
						if (key != node->key)
							node->right = Insert(node->right, MoveRValue(key), value, node);
						else
							Internals::Exceptions::ThrowKeyAlredyExist(__FILE__, __FUNCSIG__, __LINE__);
					return Balance(node);
				}

				///<summary>Вставка нового елемента</summary>
				///<param name = "node">Батьківський елемент</param>
				///<param name = "key">Новий ключ</param>
				///<param name = "value">Нове значення</param>
				///<returns>Новий батьківський елемент</returns>
				///<exception cref = "Seed::KeyAllredyExistException">Ключ існує в асоціативному масиві</exception>
				Node* Insert(Node* node, const TKey& key, TValue&& value, Node* father)
				{
					if (!node) return new Node(key, MoveRValue(value), father, this->This);
					if (key < node->key)
						node->left = Insert(node->left, key, MoveRValue(value), node);
					else
						if (key != node->key)
							node->right = Insert(node->right, key, MoveRValue(value), node);
						else
							Internals::Exceptions::ThrowKeyAlredyExist(__FILE__, __FUNCSIG__, __LINE__);
					return Balance(node);
				}

				///<summary>Вставка нового елемента</summary>
				///<param name = "node">Батьківський елемент</param>
				///<param name = "key">Новий ключ</param>
				///<param name = "value">Нове значення</param>
				///<returns>Новий батьківський елемент</returns>
				///<exception cref = "Seed::KeyAllredyExistException">Ключ існує в асоціативному масиві</exception>
				Node* Insert(Node* node, TKey&& key, TValue&& value, Node* father)
				{
					if (!node) return new Node(MoveRValue(key), MoveRValue(value), father, this->This);
					if (key < node->key)
						node->left = Insert(node->left, MoveRValue(key), MoveRValue(value), node);
					else
						if (key != node->key)
							node->right = Insert(node->right, MoveRValue(key), MoveRValue(value), node);
						else
							Internals::Exceptions::ThrowKeyAlredyExist(__FILE__, __FUNCSIG__, __LINE__);
					return Balance(node);
				}

				///<summary>Вставка нового елемента</summary>
				///<param name = "data">Значення ключа і значення</param>
				///<param name = "value">Нове значення</param>
				///<returns>Новий батьківський елемент</returns>
				///<exception cref = "Seed::KeyAllredyExistException">Ключ існує в асоціативному масиві</exception>
				Node* Insert(Node* node, const BasePair<THeap, TKey, TValue>& pair, Node* father)
				{
					if (!node) return new Node(pair, father, this->This);
					if (pair.Key < node->key)
						node->left = Insert(node->left, pair, node);
					else
						if (pair.Key != node->key)
							node->right = Insert(node->right, pair, node);
						else
							Internals::Exceptions::ThrowKeyAlredyExist(__FILE__, __FUNCSIG__, __LINE__);
					return Balance(node);
				}

				///<summary>Вставка нового елемента</summary>
				///<param name = "data">Значення ключа і значення</param>
				///<param name = "value">Нове значення</param>
				///<returns>Новий батьківський елемент</returns>
				///<exception cref = "Seed::KeyAllredyExistException">Ключ існує в асоціативному масиві</exception>
				Node* Insert(Node* node, BasePair<THeap, TKey, TValue>&& pair, Node* father)
				{
					if (!node) return new Node(MoveRValue(pair), father, this->This);
					if (pair.Key < node->key)
						node->left = Insert(node->left, MoveRValue(pair), node);
					else
						if (pair.Key != node->key)
							node->right = Insert(node->right, MoveRValue(pair), node);
						else
							Internals::Exceptions::ThrowKeyAlredyExist(__FILE__, __FUNCSIG__, __LINE__);
					return Balance(node);
				}

				///<summary>Рекурсивний пошук мінімального елемента в дереві</summary>
				///<param name = "node">Кореневий елемент</summary>
				///<returns>Мінімальний елемент</returns>
				static Node* FindMin(Node* node)
				{
					return node->left ? FindMin(node->left) : node;
				}

				///<summary>Рекурсивний пошук максимального елемента в дереві</summary>
				///<param name = "node">Кореневий елемент</summary>
				///<returns>Максимальний елемент</returns>
				static Node* FindMax(Node* node)
				{
					return node->right ? FindMax(node->right) : node;
				}

				///<summary>Рекурсивне видалення мінімального елемента</summary>
				///<param name = "node">Батьківський елемент</param>
				///<returns>Батьківський елемент</returns>
				Node* RemoveMin(Node* node)
				{
					if (node->left == nullptr)
						return node->right;
					node->left = RemoveMin(node->left);
					return Balance(node);
				}

				///<summary>Рекурсивне видалення елемента по ключу</summary>
				///<param name = "node">Батьківський елемент</summary>
				///<param name = "key">Ключ</param>
				///<returns>Батьківський елемент</returns>
				Node* Remove(Node* node, const TKey& key)
				{
					if (!node) return nullptr;
					if (key < node->key)
						node->left = Remove(node->left, key);
					else if (key > node->key)
						node->right = Remove(node->right, key);
					else //  key == Node->key
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

				///<summary>Рекурсивне знищеання дерева</summary>
				///<param name = "root">Батьківський елемент</param>
				Node* DestroyTree(Node* root) //-V688
				{
					if (root != nullptr)
					{
						if (root->left != nullptr)
						{
							root->left = DestroyTree(root->left);
						}
						if (root->right != nullptr)
						{
							root->right = DestroyTree(root->right);
						}
						if (--root->RefCount == 0)
							delete root;
						else {
							root->father = root->left = root->right = nullptr;
							root->This = nullptr;
						}
					}
					return nullptr;
				}

				///<summary>Рекурсивне копіювання дерева</summary>
				///<param name = "node">Елемент старого дерева</param>
				///<param name = "parrent">Батьківський елемент в новому дереві</param>
				///<param name = "owner">Поінтеер на нове дерево</param>
				///<returns>Поінтер на новий елемент з під елементами</returns>
				Node* copyTree(Node* node, Node * parrent, Tree * owner) const {
					Node* newNode = new Node(node->key, node->value, parrent, owner);
					newNode->height = node->height;
					if (node->left != nullptr)
						newNode->left = copyTree(node->left, newNode, owner);
					if (node->right != nullptr)
						newNode->right = copyTree(node->right, newNode, owner);
					return newNode;
				}
			private:
				///<summary>Наступний елемент</summary>
				///<param name = "node">Поточний елемент</param>
				///<returns>Наступний елемент або <c>nullptr</c></returns>
				///<exception cref = "Seed::NullPointException">Якщо <c>node == nullptr</c></exception>
				static Node* Next(Node* node)
				{
					if (node == nullptr)
						Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__);
					if (node->This != nullptr) {
						return node->This->lock.ShareLock([&]() {
							--node->RefCount;
							if (node->right != nullptr) {
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
							});
					}
					else {
						if (--node->RefCount == 0) {
							delete node;
						}
						return nullptr;
					}
				}

				///<summary>Попередній елемент</summary>
				///<param name = "node">Поточний елемент</param>
				///<returns>Попередній елемент або <c>nullptr</c></returns>
				///<exception cref = "Seed::NullPointException">Якщо <c>node == nullptr</c></exception>
				static Node* Prev(Node* node)
				{
					if (node == nullptr)
						Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__);
					if (node->This != nullptr) {
						return node->This->lock.ShareLock([&]() {
							--node->RefCount;
							if (node->left != nullptr) {
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
							});
					}
					else {
						if (--node->RefCount == 0) {
							delete node;
						}
						return nullptr;
					}
				}
			public:
				///<summary>Ітератор на елемент в асоціативному масиві</summary>
				class SeedKernelCall Iterator final : protected IDisposable
				{
				private:
					friend BaseConcurencyMap;
					///<summary>Покажчик на елемент</summary>
					Node* item;
					///<summary>Конструктор з елемента</summary>
					Iterator(Node* item) : item(item) { if (item != nullptr) ++item->RefCount; };
				protected:
					virtual void Dispose() {}
				public:
					///<summary>Конструктор</summary>
					Iterator() : item(nullptr) {};

					RefCountClass(Iterator, item);

					///<summary>Переходить до наступного елемента</summary>
					///<exception cref = "Seed::NullPointException">Якщо це нуль-ітератор</c></exception>
					///<returns>Силку на цей ітератор</returns>
					Iterator& operator++()
					{
						this->item = Next(item);
						return *this;
					}

					///<summary>Переходить до попереднього елемента</summary>
					///<exception cref = "Seed::NullPointException">Якщо це нуль-ітератор</c></exception>
					///<returns>Силка на цей ітератор</returns>
					Iterator& operator--()
					{
						this->item = Prev(item);
						return *this;
					}

					///<summary>Порівняння ітераторів</summary>
					///<param name = "other">Лівий операнд</param>
					///<returns><c>true</c>Якщо не рівні</returns>
					bool operator!=(const Iterator& other)
					{
						if (this->item == nullptr && other.item == nullptr)
							return false;
						else if (this->item == nullptr || other.item == nullptr)
							return true;
						else
							return this->item->key != other.item->key;
					}

					///<summary>Порівняння з нуль-ітератором</summary>
					///<param name = "other"><c>nullptr</c></param>
					///<returns><c>true</c>Якщо це нуль-ітератор</returns>
					bool operator!=(const NullptrT&)
					{
						return this->item != nullptr;
					}

					///<summary>Операція доступу до елемента</summary>
					///<returns>Вертає поінтер силку на елемент</returns>
					auto* operator->()
					{
						return &this->item->data;
					};

					///<summary>Операція доступу до елемента</summary>
					///<returns>Вертає поінтер силку на елемент</returns>
					auto& operator*()
					{
						return this->item->data;
					}

					///<summary>Константна операція доступу до елемента</summary>
					///<returns>Вертає поінтер силку на елемент</returns>
					const auto* operator->() const
					{
						return &this->item->data;
					};

					///<summary>Константна операція доступу до елемента</summary>
					///<returns>Вертає поінтер силку на елемент</returns>
					const auto& operator*() const
					{
						return this->item->data;
					}

					template<typename U>
					friend void Seed::Core::Swap(U& A, U& B);
				};

				friend Iterator;

				///<summary>Вертає ітератор на пергий елемент</summary>
				///<returns>Ітервтор на перший елемент</summary>
				Iterator First()
				{
					return this->This->lock.ShareLock([&]() {
						return Iterator(FindMin(This->root));
						});
				}
				///<summary>Вертає ітератор на останій елемент</summary>
				///<returns>Ітервтор на останій елемент</summary>
				Iterator Last()
				{
					return this->This->lock.ShareLock([&]() {
						return Iterator(FindMax(This->root));
						});
				}
				///<summary>Вертає ітератор на перший елемент</summary>
				///<returns>Ітервтор на перший елемент</summary>
				Iterator begin()
				{
					return this->This->lock.ShareLock([&]() {
						return First();
						});
				}
				///<summary>Вертає ітератор на останій елемент</summary>
				///<returns>Ітервтор на останій елемент</summary>
				const Iterator end()
				{
					return Iterator(nullptr);
				}

			public:
				///<summary>Конструктор</summary>
				BaseConcurencyMap() :This(new Tree)
				{}

				///<summary>Конструктор</summary>
				BaseConcurencyMap(InitilizerList<BasePair<THeap, TKey, TValue>>&& pairs) :This(new Tree)
				{
					for (auto&& it : pairs) {
						Insert(MoveRValue(it));
					}
				}

				template<typename U>
				friend void Seed::Core::Swap(U& A, U& B);

				///<summary>Оператори підтримки підрахунку силок об'єкта</summary>
				RefCountClass(BaseConcurencyMap, This);

				///<summary>Вертає кількість елементів в асоціативному масиві</summary>
				SizeT getCount() const { return This->count; }
				///<value>Кількість елементів в асоціативному масиві</value>
				SizeT Property(getCount) Count;

				///<summary>Вставка нового елемента</summary>
				///<param name = "pair">Пара ключ і значення</param>
				///<exception cref = "Seed::KeyAllredyExistException">Ключ існує в асоціативному масиві</exception>
				void Insert(const BasePair<THeap, TKey, TValue>& pair)
				{
					This->lock.ExclusiveLock([&]() {
						This->root = Insert(This->root, pair, nullptr);
						++This->count;
						});
				}

				///<summary>Вставка нового елемента</summary>
				///<param name = "pair">Пара ключ і значення</param>
				///<exception cref = "Seed::KeyAllredyExistException">Ключ існує в асоціативному масиві</exception>
				void Insert(BasePair<THeap, TKey, TValue>&& pair)
				{
					This->lock.ExclusiveLock([&]() {
						This->root = Insert(This->root, pair, nullptr);
						++This->count;
						});
				}

				///<summary>Вставка нового елемента</summary>
				///<param name = "key">Ключ нового елемента</param>
				///<param name = "value">Значення нового елемента</param>
				///<exception cref = "Seed::KeyAllredyExistException">Ключ існує в асоціативному масиві</exception>
				void Insert(const TKey& key, const TValue& value)
				{
					This->lock.ExclusiveLock([&]() {
						This->root = Insert(This->root, key, value, nullptr);
						++This->count;
						});
				}

				///<summary>Вставка нового елемента</summary>
				///<param name = "key">Ключ нового елемента</param>
				///<param name = "value">Значення нового елемента</param>
				///<exception cref = "Seed::KeyAllredyExistException">Ключ існує в асоціативному масиві</exception>
				void Insert(TKey&& key, const TValue& value)
				{
					This->lock.ExclusiveLock([&, key = MoveRValue(key)]() mutable {
						This->root = Insert(This->root, MoveRValue(key), value, nullptr);
						++This->count;
					});
				}

				///<summary>Вставка нового елемента</summary>
				///<param name = "key">Ключ нового елемента</param>
				///<param name = "value">Значення нового елемента</param>
				///<exception cref = "Seed::KeyAllredyExistException">Ключ існує в асоціативному масиві</exception>
				void Insert(const TKey& key, TValue&& value)
				{
					This->lock.ExclusiveLock([&, value = MoveRValue(value)]() mutable {
						This->root = Insert(This->root, key, MoveRValue(value), nullptr);
						++This->count;
					});
				}

				///<summary>Вставка нового елемента</summary>
				///<param name = "key">Ключ нового елемента</param>
				///<param name = "value">Значення нового елемента</param>
				///<exception cref = "Seed::KeyAllredyExistException">Ключ існує в асоціативному масиві</exception>
				void Insert(TKey&& key, TValue&& value)
				{
					This->lock.ExclusiveLock([&, key = MoveRValue(key), value = MoveRValue(value)]() mutable {
						This->root = Insert(This->root, MoveRValue(key), MoveRValue(value), nullptr);
						++This->count;
					});
				}

				///<summary>Видалення елемента по значенню</summary>
				///<param name = "key">Ключ по якому відбуватиметься видалення</param>
				void Delete(const TKey& key)
				{
					This->lock.ExclusiveLock([&]() {
						--This->count;
						This->root = Remove(This->root, key);
						});
				}

				///<summary>Отриання значення по ключу</summary>
				///<param name = "key">Ключ по якому шукатиме значення</param>
				///<returns>Силка на значення</returns>
				///<exception cref = "Seed::NullPointException">За заданим ключом немає значення в масиві</exception>
				TValue& operator[](const TKey& key) {
					Node* node = This->lock.ShareLock([&]() {
						return Find(This->root, key); });

					if (node == nullptr)
						Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__);
					return node->value;
				}
				///<summary>Отриання значення по ключу</summary>
				///<param name = "key">Ключ по якому шукатиме значення</param>
				///<returns>Константна силка на значення</returns>
				///<exception cref = "Seed::NullPointException">За заданим ключом немає значення в масиві</exception>
				const TValue& operator[](const TKey& key) const { //-V659
					Node* node = This->lock.ShareLock([&]() {
						return Find(This->root, key); });

					if (node == nullptr)
						Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__);
					return node->value;
				}

				bool IsExist(const TKey& key) const {
					return This->lock.ShareLock([&]() {
						return Find(This->root, key); }) != nullptr;
				}

				bool IsExist(TKey&& key) const {
					return This->lock.ShareLock([&]() {
						return Find(This->root, key); }) != nullptr;
				}

				virtual BaseConcurencyMap Clone() const override final {
					return This->lock.ShareLock([&]() {
						BaseConcurencyMap result;
						result.This->count = This->count;
						result.This->root = copyTree(This->root, nullptr, result.This);
						return result;
						});
				}
			protected:
				template<template<typename THeap, typename TKey, typename TValue> typename TCollection, typename THeap, typename TKey, typename TValue, typename TPair, bool IsConcurencySuportT>
				friend class LINQ::IAssociableCollection;
				template<template<typename THeap, typename TKey, typename TValue> typename TCollection, typename THeap, typename TKey, typename TValue, typename TPair, bool IsConcurencySuportT>
				friend class LINQ::IMap;
#pragma region ForEach Pair
				template<typename Lambda>
				__forceinline static void ForEachImplWithIndex(Node* node, SizeT& Index, Lambda& lambda) {
					if (node->left != nullptr)
						ForEachImplWithIndex(node->left, Index, lambda);
					lambda(node->data, Index); Index++;
					if (node->right != nullptr)
						ForEachImplWithIndex(node->left, Index, lambda);
				}

				template<typename Lambda>
				__forceinline static void ForEachImplWithoutIndex(Node* node, Lambda& lambda) {
					if (node->left != nullptr)
						ForEachImplWithoutIndex(node->left, lambda);
					lambda(node->data);
					if (node->right != nullptr)
						ForEachImplWithoutIndex(node->left, lambda);
				}

				template<typename LambdaT>
				__forceinline const BaseConcurencyMap& ForEachImpl(LambdaT&& lambda) const {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const BasePair<THeap, TKey, TValue>&>) {
						return This->lock.ShareLock([&]() -> const BaseConcurencyMap& {
							auto node = this->This->root;
							if (node == nullptr)
							{
								ForEachImplWithoutIndex(node, lambda);
							}
							return *this;
							});
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const BasePair<THeap, TKey, TValue>&, SizeT>) {
						return This->lock.ShareLock([&]() -> const BaseConcurencyMap& {
							auto node = this->This->root;
							if (node != nullptr)
							{
								SizeT index = 0;
								ForEachImplWithIndex(node, index, lambda);
							}
							return *this;
							});
					}
					else {
						static_assert(false);
					}
				}

				template<typename LambdaT>
				__forceinline BaseConcurencyMap& ForEachImpl(LambdaT&& lambda) {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, BasePair<THeap, TKey, TValue>&> ||
						TypeTraits::IsLambdaConvertibleV<LambdaT, void, const BasePair<THeap, TKey, TValue>&>) {
						return This->lock.ShareLock([&]() -> BaseConcurencyMap& {
							auto node = this->This->root;
							if (node == nullptr)
							{
								ForEachImplWithoutIndex(node, lambda);
							}
							return *this;
							});
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, BasePair<THeap, TKey, TValue>&, SizeT> ||
						TypeTraits::IsLambdaConvertibleV<LambdaT, void, const BasePair<THeap, TKey, TValue>&, SizeT>) {
						return This->lock.ShareLock([&]() -> BaseConcurencyMap& {
							auto node = this->This->root;
							if (node != nullptr)
							{
								SizeT index = 0;
								ForEachImplWithIndex(node, index, lambda);
							}
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
				__forceinline static void ForEachKeyImplWithIndex(Node* node, SizeT& Index, Lambda& lambda) {
					if (node->left != nullptr)
						ForEachKeyImplWithIndex(node->left, Index, lambda);
					lambda(node->data.Key, Index); Index++;
					if (node->right != nullptr)
						ForEachKeyImplWithIndex(node->left, Index, lambda);
				}

				template<typename Lambda>
				__forceinline static void ForEachKeyImplWithoutIndex(Node* node, Lambda& lambda) {
					if (node->left != nullptr)
						ForEachKeyImplWithoutIndex(node->left, lambda);
					lambda(node->data.Key);
					if (node->right != nullptr)
						ForEachKeyImplWithoutIndex(node->left, lambda);
				}

				template<typename LambdaT>
				__forceinline BaseConcurencyMap& ForEachKeyImpl(LambdaT&& lambda) {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const TKey&>) {
						return This->lock.ShareLock([&]() -> BaseConcurencyMap& {
							auto node = this->This->root;
							if (node == nullptr)
							{
								ForEachKeyImplWithoutIndex(node, lambda);
							}
							return *this;
							});
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const TKey&, SizeT>) {
						return This->lock.ShareLock([&]() -> BaseConcurencyMap& {
							auto node = this->This->root;
							if (node != nullptr)
							{
								SizeT index = 0;
								ForEachKeyImplWithIndex(node, index, lambda);
							}
							return *this;
							});
					}
					else {
						static_assert(false);
					}
				}

				template<typename LambdaT>
				__forceinline const BaseConcurencyMap& ForEachKeyImpl(LambdaT&& lambda) const {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const TKey&>) {
						return This->lock.ShareLock([&]() -> const BaseConcurencyMap& {
							auto node = this->This->root;
							if (node == nullptr)
							{
								ForEachKeyImplWithoutIndex(node, lambda);
							}
							return *this;
							});
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const TKey&, SizeT>) {
						return This->lock.ShareLock([&]() -> const BaseConcurencyMap& {
							auto node = this->This->root;
							if (node != nullptr)
							{
								SizeT index = 0;
								ForEachKeyImplWithIndex(node, index, lambda);
							}
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
				__forceinline static void ForEachValueImplWithIndex(Node* node, SizeT& Index, Lambda& lambda) {
					if (node->left != nullptr)
						ForEachValueImplWithIndex(node->left, Index, lambda);
					lambda(node->data.Value, Index); Index++;
					if (node->right != nullptr)
						ForEachValueImplWithIndex(node->left, Index, lambda);
				}

				template<typename Lambda>
				__forceinline static void ForEachValueImplWithoutIndex(Node* node, Lambda& lambda) {
					if (node->left != nullptr)
						ForEachValueImplWithoutIndex(node->left, lambda);
					lambda(node->data.Value);
					if (node->right != nullptr)
						ForEachValueImplWithoutIndex(node->left, lambda);
				}

				template<typename LambdaT>
				__forceinline const BaseConcurencyMap& ForEachValueImpl(LambdaT&& lambda) const {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const TValue>& > ) {
						return This->lock.ShareLock([&]() -> const BaseConcurencyMap& {
							auto node = this->This->root;
							if (node == nullptr)
							{
								ForEachValueImplWithoutIndex(node, lambda);
							}
							return *this;
							});
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const TValue&, SizeT>) {
						return This->lock.ShareLock([&]() -> const BaseConcurencyMap& {
							auto node = this->This->root;
							if (node != nullptr)
							{
								SizeT index = 0;
								ForEachValueImplWithIndex(node, index, lambda);
							}
							return *this;
							});
					}
					else {
						static_assert(false);
					}
				}

				template<typename LambdaT>
				__forceinline BaseConcurencyMap& ForEachValueImpl(LambdaT&& lambda) {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, TValue&> ||
						TypeTraits::IsLambdaConvertibleV<LambdaT, void, const TValue&>) {
						return This->lock.ShareLock([&]() -> BaseConcurencyMap& {
							auto node = this->This->root;
							if (node == nullptr)
							{
								ForEachValueImplWithoutIndex(node, lambda);
							}
							return *this;
							});
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, TValue&, SizeT> ||
						TypeTraits::IsLambdaConvertibleV<LambdaT, void, const TValue&, SizeT>) {
						return This->lock.ShareLock([&]() -> BaseConcurencyMap& {
							auto node = this->This->root;
							if (node != nullptr)
							{
								SizeT index = 0;
								ForEachValueImplWithIndex(node, index, lambda);
							}
							return *this;
							});
					}
					else {
						static_assert(false);
					}
				}
#pragma endregion
			protected:
				///<summary>Рекурсивне знищеання дерева</summary>
				///<param name = "root">Батьківський елемент</param>
				template<typename Lambda>
				Node* DestroyTreeWithLambdaPairValues(Node* root, Lambda& lambda) //-V688
				{
					if (root != nullptr)
					{
						if (root->left != nullptr)
						{
							root->left = DestroyTreeWithLambdaPairValues(root->left, lambda);
						}
						if (root->right != nullptr)
						{
							root->right = DestroyTreeWithLambdaPairValues(root->right, lambda);
						}
						lambda(MoveRValue(root->data));
						if (--root->RefCount == 0)
							delete root;
						else {
							root->father = root->left = root->right = nullptr;
							root->This = nullptr;
						}
					}
					return nullptr;
				}

				__forceinline BaseConcurencyMap& RemoveAllImpl() {
					this->Dispose();
					return *this;
				}

				template<typename Lambda>
				__forceinline BaseConcurencyMap& RemoveAllWithImpl(Lambda&& lambda) {
					return This->lock.ShareLock([&]() -> BaseConcurencyMap& {
						This->root = DestroyTreeWithLambdaPairValues(This->root, lambda);
						This->count = 0;
						return *this;
						});
				}
			};

			///<summary>Шаблон потоко захищеного асоціативного масиву з обнулючим алокатором
			///<para>
			/// Внутрішньо реалізований як червоно-чорне дерево.
			/// Для кожного елемента відбувається підрахунок силок,
			/// що дозволяє корректно його знищити навіть
			/// якщо десь використовується ітератор на елемент
			///</para>
			///</summary>
			///<param name = "TKey">Тип ключа</param>
			///<param name = "TValue">Тип значення</param>
			namespace {
				template<typename TKey, typename TVal>
				using ConcurencySensitiveMap = BaseConcurencyMap<Seed::Core::Memory::SensitiveDataHeap, TKey, TVal>;
			}
			///<summary>Шаблон потоко захищеного асоціативного масиву
			///<para>
			/// Внутрішньо реалізований як червоно-чорне дерево.
			/// Для кожного елемента відбувається підрахунок силок,
			/// що дозволяє корректно його знищити навіть
			/// якщо десь використовується ітератор на елемент
			///</para>
			///</summary>
			///<param name = "TKey">Тип ключа</param>
			///<param name = "TValue">Тип значення</param>
			namespace {
				template<typename TKey, typename TVal>
				using ConcurencyMap = BaseConcurencyMap<Seed::Core::Memory::Heap, TKey, TVal>;
			}
		}

		namespace TypeTraits {
			namespace Collection {
				template<typename THeap, typename TKey, typename TVal>
				struct IsMapT<Seed::Core::Containers::BaseConcurencyMap<THeap, TKey, TVal>> : LogicTrue {};
			}
		}

		template<typename THeap, typename TKey, typename TVal>
		__forceinline void Swap(Containers::BaseConcurencyMap<THeap, TKey, TVal>& A, Containers::BaseConcurencyMap<THeap, TKey, TVal>& B) {
			//TODO: Interlocked Exchange
			Swap(A.This, B.This);
		}

		template<typename THeap, typename TKey, typename TVal>
		__forceinline void Swap(typename Containers::BaseConcurencyMap<THeap, TKey, TVal>::Iterators& A, typename Containers::BaseConcurencyMap<THeap, TKey, TVal>::Iterators& B) {
			//TODO: Interlocked Exchange
			Swap(A.This, B.This);
		}
	}
}