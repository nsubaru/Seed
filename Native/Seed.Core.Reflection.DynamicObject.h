#pragma once
#include "Seed.Core.Reflection.Base.h"

namespace Seed
{
	inline namespace Core
	{
		namespace Reflection
		{
			class DynamicObject final : IDisposable{
			private:
				class IDynamicImpl {
				public:
					virtual String GetSerilizationMetadata() const = 0;
					virtual RTTI::RTTITypeRecord GetValueType() const = 0;

					virtual const TypeInfo& GetImplementationTypeInfo() const = 0;
				public:
					virtual ~IDynamicImpl() =0 {}
				};

				template<typename ... TTypes>
				class DynamicImpl final : public IDynamicImpl {
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
						typeName(CTTI::TypeTraits::CTTITypeMetaData<Tuple>::TypeName), 
						cxxTypeName(CTTI::TypeTraits::CTTITypeMetaData<Tuple>::CXXTypeName),
						serelizableMetaData(Serilization::MetaDataGen<Tuple>())
					{}

					DynamicImpl(const Tuple & tuple) : 
						tuple(tuple),
						typeName(CTTI::TypeTraits::CTTITypeMetaData<Tuple>::TypeName), 
						cxxTypeName(CTTI::TypeTraits::CTTITypeMetaData<Tuple>::CXXTypeName),
						serelizableMetaData(Serilization::MetaDataGen<Tuple>())
					{}

					DynamicImpl(Tuple && tuple) : 
						tuple(MoveRValue(tuple)),
						typeName(CTTI::TypeTraits::CTTITypeMetaData<Tuple>::TypeName), 
						cxxTypeName(CTTI::TypeTraits::CTTITypeMetaData<Tuple>::CXXTypeName),
						serelizableMetaData(Serilization::MetaDataGen<Tuple>())
					{}

					Tuple& GetValue() { return tuple; }
					const Tuple& GetValue() const { return tuple; }
				public:
					virtual RTTI::RTTITypeRecord GetValueType() const {
						static auto type = TypeRecord<Tuple>();
						return type;
					}

					virtual String GetSerilizationMetadata() const { return serelizableMetaData; }

					virtual const TypeInfo& GetImplementationTypeInfo() const { return typeid(Tuple); }
				public:
					virtual ~DynamicImpl() override {}
				};
			private:
				template<typename T>
				struct _ {};

				template<typename ... TTypes>
				struct _<Containers::Tuple<TTypes...>> {
					constexpr static auto __ = [] (IDynamicImpl* impl) -> Containers::Tuple<TTypes...>
					{
						return dynamic_cast<DynamicImpl<TTypes...>*>(impl)->GetValue();
					};
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
				DynamicObject(Containers::Tuple<TTypes...>&& tuple) : data(new Data(new DynamicImpl(MoveRValue(tuple)))) {}

				const RTTI::RTTITypeRecord& GetValueType() const { return data->impl->GetValueType(); }
				const String& GetSerilizationMetadata() const { return data->impl->GetSerilizationMetadata(); }

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

				template<typename TTuple> requires Concepts::Tuple<TTuple>
				TTuple GetTuple() {
					
					if(typeid(TTuple) == GetImplementationTypeInfo()){
						return _<TTuple>::__(data->impl);
					}
					else
					{
						Throw(NullPointException(ExceptionPosition));
					}
				}

				template<typename TTuple> requires Concepts::Tuple<TTuple>
				const TTuple GetTuple() const {
					
					if(typeid(TTuple) == GetImplementationTypeInfo()){
						return _<TTuple>::__(data->impl);
					}
					else
					{
						Throw(NullPointException(ExceptionPosition));
					}
				}

				RTTI::RTTITypeRecord Property(GetValueType) ValueType;
				String Property(GetSerilizationMetadata) SerilizationMetadata;

				RefCountClass(DynamicObject, data);
			};
		}
	}
}