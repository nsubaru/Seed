#pragma once
#include "Seed.Core.Memory.h"
#include "Seed.Core.Exceptions.h"

namespace Seed {
	inline namespace Core {
		///<summary>Ключ вжк існує</summary>
		class KeyAllredyExistException : Exception {
		public:
			///<summary>Конструктор помилки</summary>
			///<param name = 'File'>Файл в якому згенерована помилка</param>
			///<param name = 'FunctName'>Ім'я функції</param>
			///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
			KeyAllredyExistException(const ConstAString& File, const ConstAString& FunctName, SizeT Line)
				:Exception(File, FunctName, Line, L"Ключ вже існує")
			{}

			///<summary>Текстове представлення інформації про помилки помилки</summary>
			virtual ExceptionString toString() const override final {
				return ExceptionString(exeptionName)
					+ ExceptionString(L". ")
					+ location.toString()
					+ ExceptionString(L". ");
			}

			virtual ~KeyAllredyExistException(){}
		};

		///<summary>Інтерфейс пари значеннь і ключа</summary>
		///<param name = "TKey">Тип ключа</param>
		///<param name = "TValue">Тип значення</param>
		template<typename TKey, typename TValue>
		class IPair {
		public:
			///<summary>Вертає значення ключа</summary>
			virtual TKey GetKey() = 0;
			///<summary>Вертаж поточне значення</summary>
			virtual TValue GetValue() = 0;
			///<summary>Встановлює нове поточне значення</summary>
			///<param name = "NewValue">Нове значення</param>
			virtual void SetValue(const TValue& NewValue) = 0;
			///<summary>Встановлює нове поточне значення</summary>
			///<param name = "NewValue">Нове значення</param>
			virtual void SetValue(TValue&& NewValue) = 0;

			///<value>Ключ</value>
			TKey Property(GetKey) Key;
			///<value>Значення</value>
			TValue Property(GetValue, SetValue) Value;

			virtual ~IPair() = 0 {}
		};

		///<summary>Шаблон потоко захищеного асоціативного масиву
		///<para>
		/// Внутрішньо реалізований як червоно-чорне дерево.
		/// Для кожного елемента відбувається підрахунок силок,
		/// що дозволяє корректно його знищити навіть
		/// якщо десь використовується ітератор на елемент
		///</para>
		///</summary>
		///<param name = "Heap">Куча для алокатора</param>
		///<param name = "TKey">Тип ключа</param>
		///<param name = "TValue">Тип значення</param>
		template<typename Heap, typename TKey, typename TValue>
		class BaseMap : virtual public Memory::Allocator<Heap>
		{
		private:
			struct Tree;

			///<summary>Елемент дерева і елемент в асоціативному масиві</summary>
			struct Node : virtual public Memory::Allocator<Heap>, public IPair<TKey, TValue>
			{
				///<summary>Ключ елемента</summary>
				TKey key;
				///<summary>Значення елемента</summary>
				TValue value;
			private:
				///<summary>Віддаленість елемента від корення дерева(висота)</summary>
				UInt16 height;

				///<summary>Покажчик на батьківський елемент</summary>
				Node* father;
				///<summary>Покажчик на лівий елемент</summary>
				Node* left;
				///<summary>Покажчик на правий елемент</summary>
				Node* right;

				Tree* tree;

				///<summary>Кількіст силок на даний елемент</summary>
				RefCountT RefCount;

				///<summary>Конструктор копіювання ключа і значення</summary>
				Node(const TKey& key, const TValue& value, Node* father, Tree* tree) : key(key), value(value), height(1), father(father), tree(tree), right(nullptr), left(nullptr), RefCount(1)
				{};

				///<summary>Конструктор переміщення значення і копіювання значення</summary>
				Node(TKey&& key, const TValue& value, Node* father, Tree* tree) : key(MoveRValue(key)), value(value), height(1), father(father), tree(tree), right(nullptr), left(nullptr), RefCount(1)
				{};

				///<summary>Конструктор копіювання значення і переміщення значення</summary>
				Node(const TKey& key, TValue&& value, Node* father, Tree* tree) : key(key), value(MoveRValue(value)), height(1), father(father), tree(tree), right(nullptr), left(nullptr), RefCount(1)
				{};

				///<summary>Конструктор</summary>
				Node(TKey&& key, TValue&& value, Node* father, Tree* tree) : key(MoveRValue(key)), value(MoveRValue(value)), height(1), father(father), tree(tree), right(nullptr), left(nullptr), RefCount(1)
				{};
			public:
				///<summary>Вертає значення ключа</summary>
				virtual TKey GetKey() { return key; }
				///<summary>Вертаж поточне значення</summary>
				virtual TValue GetValue() { return value; }
				///<summary>Встановлює нове поточне значення</summary>
				///<param name = "NewValue">Нове значення</param>
				virtual void SetValue(const TValue& value) { this->value = value; }
				///<summary>Встановлює нове поточне значення</summary>
				///<param name = "NewValue">Нове значення</param>
				virtual void SetValue(TValue&& value) { this->value = MoveRValue(value); }

				friend BaseMap;
				friend BaseMap::Iterator;
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

				Tree() : root(nullptr), count(0), lock() {}
			};
		private:
			Tree* tree;
		private:
			///<summary>Рекурсивний пошук елемента по ключу в дереві</summary>
			///<param name = "node">Кореневий в якому шукає</param>
			///<param name = "key">Ключ по якому шукає</param>
			///<returns>Елемент дерева або <c>nullptr<c></returns>
			Node* Find(Node * node, const TKey& key) const {
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
			Node * RotateLeft(Node * node)
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
			Node * Balance(Node * node)
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
			Node* Insert(Node * node, const TKey & key, const TValue & value, Node * father)
			{
				if (!node) return new Node(key, value, father, this->tree);
				if (key < node->key)
					node->left = Insert(node->left, key, value, node);
				else
					if (key != node->key)
						node->right = Insert(node->right, key, value, node);
					else
						Throw(KeyAllredyExistException(ExceptionPosition));
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
				if (!node) return new Node(MoveRValue(key), value, father, this->tree);
				if (key < node->key)
					node->left = Insert(node->left, MoveRValue(key), value, node);
				else
					if (key != node->key)
						node->right = Insert(node->right, MoveRValue(key), value, node);
					else
						Throw(KeyAllredyExistException(ExceptionPosition));
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
				if (!node) return new Node(key, MoveRValue(value), father, this->tree);
				if (key < node->key)
					node->left = Insert(node->left, key, MoveRValue(value), node);
				else
					if (key != node->key)
						node->right = Insert(node->right, key, MoveRValue(value), node);
					else
						Throw(KeyAllredyExistException(ExceptionPosition));
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
				if (!node) return new Node(MoveRValue(key), MoveRValue(value), father, this->tree);
				if (key < node->key)
					node->left = Insert(node->left, MoveRValue(key), MoveRValue(value), node);
				else
					if (key != node->key)
						node->right = Insert(node->right, MoveRValue(key), MoveRValue(value), node);
					else
						Throw(KeyAllredyExistException(ExceptionPosition));
				return Balance(node);
			}

			///<summary>Рекурсивний пошук мінімального елемента в дереві</summary>
			///<param name = "node">Кореневий елемент</summary>
			///<returns>Мінімальний елемент</returns>
			static Node* FindMin(Node * node)
			{
				return node->left ? FindMin(node->left) : node;
			}

			///<summary>Рекурсивний пошук максимального елемента в дереві</summary>
			///<param name = "node">Кореневий елемент</summary>
			///<returns>Максимальний елемент</returns>
			static Node* FindMax(Node * node)
			{
				return node->right ? FindMax(node->right) : node;
			}

			///<summary>Рекурсивне видалення мінімального елемента</summary>
			///<param name = "node">Батьківський елемент</param>
			///<returns>Батьківський елемент</returns>
			Node* RemoveMin(Node * node)
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
			Node* Remove(Node * node, TKey key)
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
			///<param name = "root">Батьківський елемент</summary>
			Node* DestroyTree(Node * root) //-V688
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
					if(--root->RefCount == 0)
						delete root;
					else {
						root->father = root->left = root->right = nullptr;
						root->tree = nullptr;
					}
				}
				return nullptr;
			}
		private:
			///<summary>Наступний елемент</summary>
			///<param name = "node">Поточний елемент</param>
			///<returns>Наступний елемент або <c>nullptr</c></returns>
			///<exception cref = "Seed::NullPointException">Якщо <c>node == nullptr</c></exception>
			static Node* Next(Node * node)
			{
				if (node == nullptr)
					Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(node), NullPointException::Types::Read));
				if (node->tree != nullptr) {
					return node->tree->lock.ShareLock([&]() {
						node->RefCount--;
						if (node->right != nullptr)
							return FindMin(node->right);

						Node* father = node->father;
						while (father != nullptr && node == father->right)
						{
							node = father;
							father = father->father;
						}
						if(father != nullptr)
							father->RefCount++;
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
			static Node* Prev(Node * node)
			{
				if (node == nullptr)
					Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(node), NullPointException::Types::Read));
				if (node->tree != nullptr) {
					return node->tree->lock.ShareLock([&]() {
						node->RefCount--;
						if (node->left != nullptr)
							return FindMax(node->left);

						Node* father = node->father;
						while (father != nullptr && node == father->left)
						{
							node = father;
							father = father->father;
						}
						father->RefCount++;
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
			class Iterator : IDisposable
			{
			private:
				friend BaseMap;
				///<summary>Покажчик на елемент</summary>
				Node* item;
				///<summary>Конструктор з елемента</summary>
				Iterator(Node* item) : item(item) { if(item != nullptr ) item->RefCount++; };
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
				Node* operator->()
				{
					return static_cast<IPair<TKey, TValue>*>(item);
				};

				///<summary>Операція доступу до елемента</summary>
				///<returns>Вертає поінтер силку на елемент</returns>
				Node& operator*()
				{
					return *item;
				}

				/////<summary>Деструктор</summary>
				//~Iterator(){
				//	if (this->item != nullptr) {
				//		if (--this->item->RefCount == 0) {
				//			delete this->item;
				//		}
				//		this->item = nullptr;
				//	}
				//}
			};

			friend Iterator;

			///<summary>Вертає ітератор на пергий елемент</summary>
			///<returns>Ітервтор на перший елемент</summary>
			Iterator First()
			{
				return this->tree->lock.ShareLock([&]() {
					return Iterator(FindMin(tree->root));
					});
			}
			///<summary>Вертає ітератор на останій елемент</summary>
			///<returns>Ітервтор на останій елемент</summary>
			Iterator Last()
			{
				return this->tree->lock.ShareLock([&]() {
					return Iterator(FindMax(tree->root));
					});
			}
			///<summary>Вертає ітератор на перший елемент</summary>
			///<returns>Ітервтор на перший елемент</summary>
			Iterator begin()
			{
				return this->tree->lock.ShareLock([&]() {
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
			BaseMap() :tree(new Tree)
			{}

			///<summary>Вертає кількість елементів в асоціативному масиві</summary>
			SizeT getCount() { return tree->count; }
			///<value>Кількість елементів в асоціативному масиві</value>
			SizeT Property(getCount) Count;

			///<summary>Вставка нового елемента</summary>
			///<param name = "key">Ключ нового елемента</param>
			///<param name = "value">Значення нового елемента</param>
			///<exception cref = "Seed::KeyAllredyExistException">Ключ існує в асоціативному масиві</exception>
			void Insert(const TKey& key, const TValue& value)
			{
				tree->lock.ExclusiveLock([&]() {
					tree->root = Insert(tree->root, key, value, nullptr);
					tree->count++;
					});
			}

			///<summary>Вставка нового елемента</summary>
			///<param name = "key">Ключ нового елемента</param>
			///<param name = "value">Значення нового елемента</param>
			///<exception cref = "Seed::KeyAllredyExistException">Ключ існує в асоціативному масиві</exception>
			void Insert(TKey&& key, const TValue& value)
			{
				tree->lock.ExclusiveLock([&, key = MoveRValue(key)]() mutable {
					tree->root = Insert(tree->root, MoveRValue(key), value, nullptr);
					tree->count++;
					});
			}

			///<summary>Вставка нового елемента</summary>
			///<param name = "key">Ключ нового елемента</param>
			///<param name = "value">Значення нового елемента</param>
			///<exception cref = "Seed::KeyAllredyExistException">Ключ існує в асоціативному масиві</exception>
			void Insert(const TKey& key, TValue&& value)
			{
				tree->lock.ExclusiveLock([&, value = MoveRValue(value)]() mutable {
					tree->root = Insert(tree->root, key, MoveRValue(value), nullptr);
					tree->count++;
					});
			}

			///<summary>Вставка нового елемента</summary>
			///<param name = "key">Ключ нового елемента</param>
			///<param name = "value">Значення нового елемента</param>
			///<exception cref = "Seed::KeyAllredyExistException">Ключ існує в асоціативному масиві</exception>
			void Insert(TKey&& key, TValue&& value)
			{
				tree->lock.ExclusiveLock([&, key = MoveRValue(key), value = MoveRValue(value)]() mutable {
					tree->root = Insert(tree->root, MoveRValue(key), MoveRValue(value), nullptr);
					tree->count++;
					});
			}
			
			///<summary>Видалення елемента по значенню</summary>
			///<param name = "key">Ключ по якому відбуватиметься видалення</param>
			void Delete(TKey key)
			{
				tree->lock.ExclusiveLock([&]() {
					tree->count--;
					tree->root = Remove(tree->root, key);
					});
			}

			///<summary>Отриання значення по ключу</summary>
			///<param name = "key">Ключ по якому шукатиме значення</param>
			///<returns>Силка на значення</returns>
			///<exception cref = "Seed::NullPointException">За заданим ключом немає значення в масиві</exception>
			TValue& operator[](const TKey& key) {
				Node* node = tree->lock.ShareLock([&]() {
					return Find(tree->root, key); });

				if (node == nullptr)
					Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(nullptr), NullPointException::Types::Read));
				return node->value;
			}
			///<summary>Отриання значення по ключу</summary>
			///<param name = "key">Ключ по якому шукатиме значення</param>
			///<returns>Константна силка на значення</returns>
			///<exception cref = "Seed::NullPointException">За заданим ключом немає значення в масиві</exception>
			const TValue& operator[](const TKey& key) const {
				Node* node = tree->lock.ShareLock([&]() mutable {
					return Find(tree->root, key); });

				if (node == nullptr)
					Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(nullptr), NullPointException::Types::Read));
				return node->value;
			}

			///<summary>Деструктор</summary>
			~BaseMap()
			{
				tree->lock.ExclusiveLock([&]() {
					DestroyTree(tree->root);
					});
				delete tree;
				tree = nullptr;
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
			using SecuredMap = BaseMap<Seed::Core::Memory::SensitiveDataHeap, TKey, TVal>;
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
			using Map = BaseMap<Seed::Core::Memory::Heap, TKey, TVal>;
		}
	}
}