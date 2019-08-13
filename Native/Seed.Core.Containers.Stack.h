#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Memory.h"
#include "Seed.Core.ConstString.h"
#include "Seed.Core.Base.Interlocked.h"
#include "Seed.Core.Containers.Array.h"
#include "Seed.Core.Threading.LockPrimitives.h"

namespace Seed {
	inline namespace Core {
		template<typename THeap, typename T>
		class SeedKernelCall BaseStack final : public Memory::Allocator<THeap>, protected IDisposable {
		private:
			struct Node {
				T Value;
				Node* Prev;

				Node(const T& Value, Node* Prev) :Value(Value), Prev(Prev) {}
				Node(T&& Value, Node* Prev) :Value(MoveRValue(Value)), Prev(Prev) {}
			};
			struct Data {
				Node* Top;
				RefCountT RefCount;
				RefCountT Count;
				Threading::SRWLock Lock;
				Data() : Top(nullptr), RefCount(1), Count(0), Lock() {}
			};
		private:
			Data* data;
		protected:
			virtual void Dispose() {
				Node* tmp;
				while (this->data->Top != nullptr) {
					tmp = this->data->Top;
					this->data->Top = this->data->Top->Prev;
					delete tmp;
				}
			}
		public:
			BaseStack(): data(new Data) {}

			RefCountClass(BaseStack, data);

			void Push(const T & Value) {
				this->data->Lock.ExclusiveLock([&]() {
					this->data->Top = new Node(Value, this->data->Top);
					this->data->Count++;
				});
			}
			void Push(T&& Value) {
				this->data->Lock.ExclusiveLock([&, Value = MoveRValue(Value)] () mutable {
					this->data->Top = new Node(Value, this->data->Top);
					this->data->Count++;
				});
			}
			T Pop() {
				return this->data->Lock.ExclusiveLock([&]() {
					extern __declspec(noreturn) void ThrowNullPointException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line,
						const Seed::SizeT Address,
						const Seed::UInt8 Mode);
					if(this->data->Top == nullptr)
						ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
					auto node = this->data->Top;
					this->data->Top = this->data->Top->Prev;
					this->data->Count--;
					T ret(MoveRValue(node->Value));
					delete node;
					return ret;
				});
			}
			SizeT GetCount() { return this->data->Count; }
			T GetTop() {
				return this->data->Lock.ShareLock([&]() {
					extern __declspec(noreturn) void ThrowNullPointException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line,
						const Seed::SizeT Address,
						const Seed::UInt8 Mode);
					if (this->data->Top == nullptr)
						ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
					return this->data->Top->Value;
				});
			}

			SizeT Property(GetCount) Count;
		};

		namespace {
			template<typename T>
			using Stack = BaseStack<Seed::Core::Memory::Heap, T>;
		}

		namespace {
			template<typename T>
			using SensitiveStack = BaseStack<Seed::Core::Memory::SensitiveDataHeap, T>;
		}
	}
}