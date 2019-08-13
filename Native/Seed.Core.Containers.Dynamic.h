#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Exceptions.h"
#include "Seed.Core.Memory.h"

namespace Seed {
	inline namespace Core {
		class IDynamicImpl {
		protected:
			virtual const TypeInfo& GetVariablesPackTypeInfo() = 0;
			virtual void* GetVariablePackRaw() = 0;
		public:
			template<typename ... Types>
			Tuple<Types...>& GetVariables() {
				using pack = Tuple<Types...>;
				if (typeid(pack).hash_code() == GetVariablesPackTypeInfo().hash_code()) {
					return *reinterpret_cast<pack*>(GetVariablePackRaw());
				}
				else
					Throw(InvalidTypeException(ExceptionPosition, typeid(pack), GetVariablesPackTypeInfo()));
			}

			const TypeInfo& GetType() { return GetVariablesPackTypeInfo(); }

			virtual ~IDynamicImpl() = 0 {};
		};
		template<typename ... Types>
		class DynamicImp : public IDynamicImpl {
		private:
			Tuple<Types...> data;
		private:
			virtual const TypeInfo& GetVariablesPackTypeInfo() final {
				return typeid(decltype(data));
			}
			virtual void* GetVariablePackRaw() {
				return reinterpret_cast<void*>(&data);
			}
		public:
			DynamicImp(Types... types): data(types...) {}
			virtual ~DynamicImp() override final {};
		};

		class SeedKernelCall Dynamic final : protected IDisposable {
		private:
			struct data {
				IDynamicImpl* pack;
				RefCountT RefCount;
				data(IDynamicImpl* pack) : pack(pack), RefCount(1) {}
			};
		private:
			data* This;
		private:
			virtual void Dispose() override final {
				delete This->pack;
			}
		public:
			template<typename ... Types>
			Dynamic(Types ... types) : This(new data(new DynamicImp<Types...>(types...))) {}

			template<typename ... Types>
			Tuple<Types...>& GetVariables() {
				return This->pack->GetVariables<Types...>();
			}

			const TypeInfo& GetVariableType() {
				return This->pack->GetType();
			}

			TypeInfo Property(GetVariableType) Type;

			RefCountClass(Dynamic, This);
		};
	}
}