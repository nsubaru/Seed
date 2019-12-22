#pragma once
#include "Seed.Core.Reflection.Base.h"

namespace Seed
{
	inline namespace Core
	{
		namespace Reflection
		{
			class SeedKernelCall DynamicObject final : IDisposable{
			private:
				class SeedKernelCall IDynamicImpl {
				public:
					virtual String GetTypeName() = 0;
					virtual String GetCxxTypeName() = 0;
					virtual String GetSerilizationMetadata() = 0;

					virtual const TypeInfo& GetImplementationTypeInfo() const = 0;
				public:
					virtual ~IDynamicImpl() =0 {}
				};

				template<typename ... TTypes>
				class SeedKernelCall DynamicImpl final : public IDynamicImpl {
				private:
					using Tuple = Containers::Tuple<TTypes...>;
				private:
					Tuple tuple;
					String typeName;
					String cxxTypeName;
					String serelizableMetaData;
				public:
					DynamicImpl() : 
						tuple(), 
						typeName(NewCTTI::TypeTraits::CTTITypeMetaData<Tuple>::TypeName), 
						cxxTypeName(NewCTTI::TypeTraits::CTTITypeMetaData<Tuple>::CXXTypeName),
						serelizableMetaData(NewSerilization::MetaDataGen<Tuple>())
					{}

					DynamicImpl(const Tuple & tuple) : 
						tuple(tuple),
						typeName(NewCTTI::TypeTraits::CTTITypeMetaData<Tuple>::TypeName), 
						cxxTypeName(NewCTTI::TypeTraits::CTTITypeMetaData<Tuple>::CXXTypeName),
						serelizableMetaData(NewSerilization::MetaDataGen<Tuple>())
					{}

					DynamicImpl(Tuple && tuple) : 
						tuple(MoveRValue(tuple)),
						typeName(NewCTTI::TypeTraits::CTTITypeMetaData<Tuple>::TypeName), 
						cxxTypeName(NewCTTI::TypeTraits::CTTITypeMetaData<Tuple>::CXXTypeName),
						serelizableMetaData(NewSerilization::MetaDataGen<Tuple>())
					{}

					Tuple& GetValue() { return tuple; }
					const Tuple& GetValue() const { return tuple; }
				public:
					virtual String GetTypeName() { return typeName; }
					virtual String GetCxxTypeName() { return cxxTypeName; }
					virtual String GetSerilizationMetadata() { return serelizableMetaData; }

					virtual const TypeInfo& GetImplementationTypeInfo() const { return typeid(Tuple); }
				public:
					virtual ~DynamicImpl() override {}
				};
			private:
				struct Data{
					IDynamicImpl* impl;
					RefCountT RefCount;

					Data(IDynamicImpl*&& impl): impl(impl), RefCount(1) {}
				};
			private:
				Data* data;
			protected:
				virtual void Dispose() override {
					delete this->data->impl;
				}
			public:
				DynamicObject() : data(new Data(new DynamicImpl(Containers::Tuple<>()))) {}
				template<typename ... TTypes>
				DynamicObject(const Containers::Tuple<TTypes...>& tuple) : data(new Data(new DynamicImpl(tuple))) {}
				template<typename ... TTypes>
				DynamicObject(Containers::Tuple<TTypes...>&& tuple) : data(new Data(MoveRValue(new DynamicImpl(MoveRValue(tuple))))) {}

				String GetTypeName() { return data->impl->GetTypeName(); }
				String GetCxxTypeName() { return data->impl->GetCxxTypeName(); }
				String GetSerilizationMetadata() { return data->impl->GetSerilizationMetadata(); }

				const TypeInfo& GetImplementationTypeInfo() const { return data->impl->GetImplementationTypeInfo(); }

				template<typename ... TTypes>
				Containers::Tuple<TTypes...>& GetValue() {
					if(typeid(Containers::Tuple<TTypes...>) == GetImplementationTypeInfo()){
						return dynamic_cast<DynamicImpl<TTypes...>*>(data->impl)->GetValue(); //-V522
					}
					else
					{
						Throw(NullPointException(ExceptionPosition));
					}
				}

				template<typename ... TTypes>
				const Containers::Tuple<TTypes...>& GetValue() const {
					if(typeid(Containers::Tuple<TTypes...>) == GetImplementationTypeInfo()){
						return dynamic_cast<DynamicImpl<TTypes...>*>(data->impl)->GetValue(); //-V522
					}
					else
					{
						Throw(NullPointException(ExceptionPosition));
					}
				}

				RefCountClass(DynamicObject, data);
			};
		}
	}
}