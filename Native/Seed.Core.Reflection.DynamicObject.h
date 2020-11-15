#pragma once
#include "Seed.Core.Reflection.Base.h"

#pragma warning(push)
//This internal implementation file which can use internal API functions
#pragma warning(disable: 4996)

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
			public:
				template<typename THeap, typename T, typename TEnable>
				friend struct Serilization::Details::BaseSerlizationHandler;
			private:
				class IDynamicImpl {
				public:
					template<typename THeap, typename T, typename TEnable>
					friend struct Serilization::Details::BaseSerlizationHandler;
				public:
					virtual const String& GetTypeName() const = 0;
					virtual const String& GetCxxTypeName() const = 0;
					virtual const String& GetSerilizationMetadata() const = 0;
					virtual RTTI::RTTITypeRecord GetValueType() const = 0;

					virtual const TypeInfo& GetImplementationTypeInfo() const = 0;
					virtual const TypeInfo& GetImplementationItem1TypeInfo() const = 0;
					virtual SizeT GetItemsCount() const = 0;

					virtual const Serilization::TypeMetaDataRecord GetTypeShema() const = 0;
				public:
					virtual ~IDynamicImpl() =0 {}
				};

				template<typename ... TTypes>
				class DynamicImpl final : public IDynamicImpl {
				public:
					template<typename THeap, typename T, typename TEnable>
					friend struct Serilization::Details::BaseSerlizationHandler;
				private:
					using Tuple = Containers::Tuple<TTypes...>;
				private:
					Tuple tuple;
					Lazy<String> typeName;
					Lazy<String> cxxTypeName;
					Lazy<String> serelizableMetaData;
					Lazy<Serilization::TypeMetaDataRecord> typeShema;
				public:
					DynamicImpl() : 
						tuple(), 
						typeName(ArrowFunct(CTTI::TypeTraits::CTTITypeMetaData<Tuple>::TypeName)),
						cxxTypeName(ArrowFunct(CTTI::TypeTraits::CTTITypeMetaData<Tuple>::CXXTypeName)),
						serelizableMetaData(ArrowFunct(Serilization::MetaDataGen<Tuple>())),
						typeShema(ArrowFunct(Serilization::GetTypeRecordForType<Tuple>()))
					{}

					DynamicImpl(const Tuple & tuple) : 
						tuple(tuple),
						typeName(CTTI::TypeTraits::CTTITypeMetaData<Tuple>::TypeName), 
						cxxTypeName(CTTI::TypeTraits::CTTITypeMetaData<Tuple>::CXXTypeName),
						serelizableMetaData(Serilization::MetaDataGen<Tuple>()),
						typeShema(ArrowFunct(Serilization::GetTypeRecordForType<Tuple>()))
					{}

					DynamicImpl(Tuple && tuple) : 
						tuple(MoveRValue(tuple)),
						typeName(CTTI::TypeTraits::CTTITypeMetaData<Tuple>::TypeName), 
						cxxTypeName(CTTI::TypeTraits::CTTITypeMetaData<Tuple>::CXXTypeName),
						serelizableMetaData(Serilization::MetaDataGen<Tuple>()),
						typeShema(ArrowFunct(Serilization::GetTypeRecordForType<Tuple>()))
					{}

					Tuple& GetValue() { return tuple; }
					const Tuple& GetValue() const { return tuple; }
				public:
					virtual RTTI::RTTITypeRecord GetValueType() const {
						if constexpr (Tuple::Count == 1) {
							static auto type = ::TypeRecord<typename Tuple::ItemType<0>>();
							return type;
						}
						else {
							static auto type = ::TypeRecord<Tuple>();
							return type;
						}
					}

					virtual const String& GetTypeName() const { return *typeName; }
					virtual const String& GetCxxTypeName() const { return *cxxTypeName; }

					virtual const String& GetSerilizationMetadata() const { return *serelizableMetaData; }

					virtual const TypeInfo& GetImplementationTypeInfo() const { 
						return typeid(Tuple);
					}

					virtual const TypeInfo& GetImplementationItem1TypeInfo() const { 
						if constexpr (Tuple::Count == 0) {
							Throw(NotImplementedException(ExceptionPosition));
						}
						else {
							return typeid(typename Tuple::ItemType<0>);
						}
					}

					virtual const Serilization::TypeMetaDataRecord GetTypeShema() const { return *typeShema; }

					virtual SizeT GetItemsCount() const { return Tuple::Count; }
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
				/// Конструктор з даних
				/// </summary>
				/// <param name="data">Кортеж даних</param>
				template<typename ... TTypes> requires (!(sizeof...(TTypes) == 1 && (Concepts::IsSame<DynamicObject, TypeTraits::RemoveCVRefV<TTypes>> && ...)))
				DynamicObject(const TTypes&... data) : data(new Data(new DynamicImpl(Containers::Tuple<TTypes...>(data)))) {}
				/// <summary>
				/// Конструктор з даних
				/// </summary>
				/// <param name="data">Кортеж даних</param>
				template<typename ... TTypes> requires (!(sizeof...(TTypes) == 1 && (Concepts::IsSame<DynamicObject, TypeTraits::RemoveCVRefV<TTypes>> && ...)))
				DynamicObject(TTypes&&... data) : data(new Data(new DynamicImpl(Containers::Tuple<TTypes...>(ForwardReference<TTypes>(data)...)))){}
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
				/// Вертає рядкове представлення імені типу
				/// </summary>
				/// <returns>Рядкове представлення імені типу</returns>
				const String& GetTypeName() const { return data->impl->GetTypeName(); }

				/// <summary>
				/// Вертає рядкове представлення cxx імені типу
				/// </summary>
				/// <returns>Рядкове представлення cxx імені типу</returns>
				const String& GetCxxTypeName() const { return data->impl->GetCxxTypeName(); }

				/// <summary>
				/// Вертає рядкове представлення схеми типу для серелізатора
				/// </summary>
				/// <returns>Рядкове представлення схеми типів</returns>
				const String& GetSerilizationMetadata() const { return data->impl->GetSerilizationMetadata(); }

				/// <summary>
				/// Вертає cxx rtti опис типу кортежа
				/// </summary>
				/// <returns></returns>
				const TypeInfo& GetImplementationTypeInfo() const { return data->impl->GetImplementationTypeInfo(); }

				/// <summary>
				/// Вертає cxx rtti опис типу першого елементам кортежу при умові що кортеж містить лише один елемент
				/// </summary>
				/// <returns></returns>
				const TypeInfo& GetImplementationItem1TypeInfo() const { return data->impl->GetImplementationItem1TypeInfo(); }

				/// <summary>
				/// Вертає кільксть елементва в кортежі
				/// </summary>
				/// <returns></returns>
				SizeT GetItemsCount() const { return data->impl->GetItemsCount(); }

				/// <summary>
				/// Вертає схему типу для серелізатора
				/// </summary>
				/// <returns>Об'єкт схеми типу</returns>
				const Serilization::TypeMetaDataRecord GetTypeShema() const { return data->impl->GetTypeShema(); }

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
				/// Ім'я типу який містить кортеж
				/// </summary>
				String Property(GetTypeName) TypeName;

				/// <summary>
				/// Ім'я cxx типу який містить кортеж
				/// </summary>
				String Property(GetCxxTypeName) CxxTypeName;

				/// <summary>
				/// Схема серелізації
				/// </summary>
				Serilization::TypeMetaDataRecord Property(GetTypeShema) TypeShema;

				/// <summary>
				/// Рядкове представлення схеми серелізації
				/// </summary>
				String Property(GetSerilizationMetadata) SerilizationMetadata;

				/// <summary>
				/// Кільксть елементва в кортежі
				/// </summary>
				SizeT Property(GetItemsCount) ItemsCount;

				RefCountClass(DynamicObject, data);
			};
		}
	}
}

#pragma warning(pop)
