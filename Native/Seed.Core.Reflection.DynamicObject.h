#pragma once
#include "Seed.Core.Reflection.Base.h"

namespace Seed
{
	inline namespace Core
	{
		namespace Reflection
		{
			/// <summary>
			/// Динамічний об'єкт, містить у собі кортеж з типів, доступ отримати можна якщо відомий тип
			/// </summary>
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
				struct TupleGetter {};

				template<typename ... TTypes>
				struct TupleGetter<Containers::Tuple<TTypes...>> {
					constexpr static auto Getter = [] (IDynamicImpl* impl) -> Containers::Tuple<TTypes...>
					{
						auto valueHolder = dynamic_cast<DynamicImpl<TTypes...>*>(impl);
						if (valueHolder == nullptr)
							Throw(NullPointException(__FILE__, "TupleGetter<Containers::Tuple<TTypes...>>::Getter", __LINE__));
						return valueHolder->GetValue();
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
				/// <summary>
				/// Пустий конструктор ініціалізовує пустий кортеж 
				/// </summary>
				DynamicObject() : data(new Data(new DynamicImpl(Containers::Tuple<>()))) {}
				/// <summary>
				/// Конструктор з кортежу
				/// </summary>
				/// <param name="tuple">Кортеж значень</param>
				template<typename ... TTypes>
				DynamicObject(const Containers::Tuple<TTypes...>& tuple) : data(new Data(new DynamicImpl(tuple))) {}
				/// <summary>
				/// Конструктор переміщення з кортежу
				/// </summary>
				/// <param name="tuple">Кортеж значень</param>
				template<typename ... TTypes>
				DynamicObject(Containers::Tuple<TTypes...>&& tuple) : data(new Data(new DynamicImpl(MoveRValue(tuple)))) {}

				/// <summary>
				/// Вертає тип кортежу який містить на даний тип об'єкт
				/// </summary>
				/// <returns>Опис типу</returns>
				RTTI::RTTITypeRecord GetValueType() const { return data->impl->GetValueType(); }

				/// <summary>
				/// Вертає схему типу для серелізатора
				/// </summary>
				/// <returns>Рядкове представлення схеми типів</returns>
				const String& GetSerilizationMetadata() const { return data->impl->GetSerilizationMetadata(); }

				/// <summary>
				/// Вертає cxx rtti опис типу
				/// </summary>
				/// <returns></returns>
				const TypeInfo& GetImplementationTypeInfo() const { return data->impl->GetImplementationTypeInfo(); }

				/// <summary>
				/// Вертає вміст якщо тип коректний, в іншому випадку генерує помилку 
				/// </summary>
				/// <exception cref="Seed::Exceptions::NullPointException">Якщо тип не коректний</exception>
				/// <returns>Посилання на кортеж</returns>
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

				/// <summary>
				/// Вертає вміст якщо тип коректний, в іншому випадку генерує помилку 
				/// </summary>
				/// <exception cref="Seed::Exceptions::NullPointException">Якщо тип не коректний</exception>
				/// <returns>Незмінне посилання на кортеж</returns>
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

				/// <summary>
				/// Вертає вміст якщо тип коректний, в іншому випадку генерує помилку 
				/// </summary>
				/// <exception cref="Seed::Exceptions::NullPointException">Якщо тип не коректний</exception>
				/// <returns>Посилання на кортеж</returns>
				template<typename TTuple> requires Concepts::Tuple<TTuple>
				TTuple GetTuple() {
					
					if(typeid(TTuple) == GetImplementationTypeInfo()){
						return TupleGetter<TTuple>::Getter(data->impl);
					}
					else
					{
						Throw(NullPointException(ExceptionPosition));
					}
				}

				/// <summary>
				/// Вертає вміст якщо тип коректний, в іншому випадку генерує помилку 
				/// </summary>
				/// <exception cref="Seed::Exceptions::NullPointException">Якщо тип не коректний</exception>
				/// <returns>Незмінне посилання на кортеж</returns>
				template<typename TTuple> requires Concepts::Tuple<TTuple>
				const TTuple GetTuple() const {
					
					if(typeid(TTuple) == GetImplementationTypeInfo()){
						return TupleGetter<TTuple>::Getter(data->impl);
					}
					else
					{
						Throw(NullPointException(ExceptionPosition));
					}
				}
			public:
				/// <summary>
				/// Опис типу який містить кортеж
				/// </summary>
				RTTI::RTTITypeRecord Property(GetValueType) ValueType;

				/// <summary>
				/// Схема серелізації
				/// </summary>
				String Property(GetSerilizationMetadata) SerilizationMetadata;

				RefCountClass(DynamicObject, data);
			};
		}
	}
}