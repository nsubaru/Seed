#pragma once
#include "Seed.Core.Memory.h"
#include "Seed.Core.Exceptions.h"

namespace Seed {
	inline namespace Core {
		template<typename Heap, typename TKey, typename TValue>
		class BaseMap : virtual public Memory::Allocator<Heap>
		{
		private:
			struct Node : virtual public Memory::Allocator<Heap>
			{
				TKey key;
				TValue value;

				UInt16 height;

				Node* father;
				Node* left;
				Node* right;

				Node(const TKey& key, const TValue& value, Node* father) : key(key), value(value), height(1), father(father), right(nullptr), left(nullptr)
				{};
			};
		private:
			Node* root;
		private:
			Node* Find(Node * node, const TKey& key) {
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

			UInt16 Height(Node* node)
			{
				return node ? node->height : 0;
			}
			Int16 BFactor(Node* node)
			{
				return  Height(node->right) - Height(node->left);
			}
			void FixHeight(Node* node)
			{
				UInt16 HLeft = Height(node->left);
				UInt16 HRight = Height(node->right);
				node->height = Max(HLeft, HRight) + 1;
			}

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

			Node* Insert(Node * node, const TKey & key, const TValue & value, Node * father)
			{
				if (!node) return new Node(key, value, father);
				if (key < node->key)
					node->left = Insert(node->left, key, value, node);
				else
					node->right = Insert(node->right, key, value, node);
				return Balance(node);
			}

			static Node* FindMin(Node * node)
			{
				return node->left ? FindMin(node->left) : node;
			}

			static Node* FindMax(Node * node)
			{
				return node->right ? FindMax(node->right) : node;
			}

			Node* RemoveMin(Node * node)
			{
				if (node->left == nullptr)
					return node->right;
				node->left = RemoveMin(node->left);
				return Balance(node);
			}

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

			Node* DestroyTree(Node * root) //-V688
			{
				if (root != nullptr)
				{
					if (root->left != nullptr)
					{
						root->left = DestroyTree(root->left);
						delete root->left;
					}
					if (root->right != nullptr)
					{
						root->right = DestroyTree(root->right);
						delete root->right;
					}
					delete root;
				}
				return nullptr;
			}
		private:
			static Node* Next(Node * node)
			{
				if (node->right != nullptr)
					return FindMin(node->right);

				Node * father = node->father;
				while (father != nullptr && node == father->right)
				{
					node = father;
					father = father->father;
				}
				return father;
			}

			static Node* Prev(Node * node)
			{
				if (node->left != nullptr)
					return FindMax(node->left);

				Node * father = node->father;
				while (father != nullptr && node == father->left)
				{
					node = father;
					father = father->father;
				}
				return father;
			}
		public:
			class Iterator
			{
			private:
				friend BaseMap;

				Node* item;
				Iterator(Node* item) : item(item)
				{};
			public:
				Iterator() : item(nullptr)
				{};
				Iterator operator++()
				{
					this->item = Next(item);
					return *this;
				}
				Iterator operator--()
				{
					this->item = Prev(item);
					return *this;
				}
				bool operator!=(const Iterator& other)
				{
					if (this->item == nullptr && other.item == nullptr)
						return false;
					else if (this->item == nullptr || other.item == nullptr)
						return true;
					else
						return this->item->key != other.item->key;
				}

				bool operator!=(const NullptrT&)
				{
					return this->item != nullptr;
				}

				Node*& operator->()
				{
					return item;
				};
				Node*& operator*()
				{
					return item;
				}
				~Iterator()
				{}
			};

			friend Iterator;

			Iterator First()
			{
				return Iterator(FindMin(root));
			}
			Iterator Last()
			{
				return Iterator(FindMax(root));
			}

			Iterator begin()
			{
				return First();
			}

			Iterator end()
			{
				return Iterator(nullptr);
			}

		public:
			BaseMap() :root(nullptr)
			{}

			void Insert(const TKey& key, const TValue& value)
			{
				root = Insert(root, key, value, nullptr);
			}

			void Delete(TKey key)
			{
				root = Remove(root, key);
			}

			TValue& operator[](const TKey& key) {
				Node* node = Find(root, key);
				if (node == nullptr)
					Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(nullptr), NullPointException::Types::Read));
				return node->value;
			}

			const TValue& operator[](const TKey& key) const {
				Node* node = Find(root, key);
				if (node == nullptr)
					Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(nullptr), NullPointException::Types::Read));
				return node->value;
			}

			~BaseMap()
			{
				DestroyTree(root);
			}
		};

		template<typename TKey, typename TVal>
		using SecuredMap = BaseMap<Seed::Core::Memory::SensitiveDataHeap, TKey, TVal>;

		template<typename TKey, typename TVal>
		using Map = BaseMap<Seed::Core::Memory::Heap, TKey, TVal>;
	}
}