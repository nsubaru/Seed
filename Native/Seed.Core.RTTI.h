#pragma once
#include "Seed.Core.CTTI.h"
#include "Seed.Core.Serilization.h"

namespace Seed
{
	inline namespace Core
	{
		namespace RTTI
		{
			/// <summary>
			/// Запис про тип
			/// </summary>
			class RTTITypeRecord : IDisposable
			{
				using Getter = String(*)();

				struct Data {
					Lazy<String> TypeName;
					Lazy<String> CxxTypeName;


					CTTI::TypeTraits::EType TypeClass;
					Boolean IsSerilizeable;
					const TypeInfo* rttiTypeInfo;
					RefCountT RefCount;

					Data(
						Lazy<String> && TypeName, 
						Lazy<String> && CxxTypeName, 
						CTTI::TypeTraits::EType TypeClass, 
						Boolean IsSerilizeable, 
						const TypeInfo* rttiTypeInfo
					) : TypeName(MoveRValue(TypeName)), 
						CxxTypeName(MoveRValue(CxxTypeName)), 
						TypeClass(TypeClass), 
						IsSerilizeable(IsSerilizeable), 
						rttiTypeInfo(rttiTypeInfo), 
						RefCount(1)
					{}
				};
			protected:
				virtual void Dispose() override {}
			private:
				Data * data;
			public:
				RefCountClass(RTTITypeRecord, data);
			public:
				/// <summary>
				/// Конструктор запису типу
				/// </summary>
				template<typename T>
				ForceInline RTTITypeRecord(TypeTraits::TypeTag<T>) : data(new Data(
					Lazy<String>(ArrowFunct(String(CTTI::TypeTraits::CTTITypeMetaData<T>::TypeName))),
					Lazy<String>(ArrowFunct(String(CTTI::TypeTraits::CTTITypeMetaData<T>::CXXTypeName))),
					CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass,
					CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable,
					&typeid(T)
				))
				{}

				/// <summary>
				/// Вертає ім'я типу
				/// </summary>
				/// <returns>Рядкове представлення імені типу</returns>
				ForceInline const String& GetTypeName() const {
					return *data->TypeName;
				}

				/// <summary>
				/// Вертає Cxx ім'я типу
				/// </summary>
				/// <returns>Рядкове представлення Cxx імені</returns>
				ForceInline const String& GetCxxTypeName() const {
					return *data->CxxTypeName;
				}

				/// <summary>
				/// Вертає CTTI клас типу
				/// </summary>
				/// <returns></returns>
				ForceInline CTTI::TypeTraits::EType GetCxxTypeClass() const {
					return data->TypeClass;
				}

				/// <summary>
				/// Вказує чи тип підтримує серелізацію
				/// </summary>
				/// <returns>true якщо тип серелізується</returns>
				ForceInline Boolean GetIsSerilizeable() const {
					return data->IsSerilizeable;
				}

				/// <summary>
				/// Вертає Cxx опис типу
				/// </summary>
				/// <returns>Cxx опис типу</returns>
				ForceInline const auto& TypeId() const {
					return *data->rttiTypeInfo;
				}

				/// <summary>
				/// Вертає хеш типу
				/// </summary>
				/// <returns>Хеш типу</returns>
				ForceInline ID GetHashCode() const {
					return data->rttiTypeInfo->GetHashCode();
				}
			public:
				/// <summary>
				/// Ім'я типу
				/// </summary>
				String					Property(GetTypeName)		TypeName;
				/// <summary>
				/// Cxx ім'я типу
				/// </summary>
				String					Property(GetCxxTypeName)	CxxTypeName;
				/// <summary>
				/// CTTI клас типу
				/// </summary>
				CTTI::TypeTraits::EType Property(GetCxxTypeClass)	CxxTypeClass;
				/// <summary>
				/// Вказує чи тип підтримує серелізацію
				/// </summary>
				Boolean					Property(GetIsSerilizeable) IsSerilizeable;
				/// <summary>
				/// Хеш типу
				/// </summary>
				ID						Property(GetHashCode)		HashCode;
			public:
				ForceInline auto operator==(const RTTITypeRecord& other) const {
					return *this->data->rttiTypeInfo == *other.data->rttiTypeInfo;
				}

				ForceInline auto operator!=(const RTTITypeRecord& other) const {
					return *this->data->rttiTypeInfo != *other.data->rttiTypeInfo;
				}

				ForceInline auto operator<(const RTTITypeRecord& other) const {
					return this->data->rttiTypeInfo < other.data->rttiTypeInfo;
				}

				ForceInline auto operator>(const RTTITypeRecord& other) const {
					return this->data->rttiTypeInfo > other.data->rttiTypeInfo;
				}

				ForceInline auto operator<=(const RTTITypeRecord& other) const {
					return *this == other || *this < other;
				}

				ForceInline auto operator>=(const RTTITypeRecord& other) const {
					return *this == other || *this > other;
				}

				ForceInline auto operator<=>(const RTTITypeRecord& other) const {
					if (*this == other)
					{
						return Comparing::StrongOrdering::equal;
					}
					else if (*this > other)
					{
						return Comparing::StrongOrdering::greater;
					}
					else
					{
						return Comparing::StrongOrdering::less;
					}
				}

				ForceInline auto operator==(const TypeInfo& other) const {
					return *this->data->rttiTypeInfo == other;
				}

				ForceInline auto operator!=(const TypeInfo& other) const {
					return *this->data->rttiTypeInfo != other;
				}

				ForceInline friend auto operator==(const TypeInfo& other, const RTTITypeRecord& item) {
					return other == *item.data->rttiTypeInfo;
				}

				ForceInline friend auto operator!=(const TypeInfo& other, const RTTITypeRecord& item) {
					return other != *item.data->rttiTypeInfo;
				}

				ForceInline friend auto operator<(const TypeInfo& other, const RTTITypeRecord& item) {
					return &other < item.data->rttiTypeInfo;
				}

				ForceInline friend auto operator>(const TypeInfo& other, const RTTITypeRecord& item) {
					return &other > item.data->rttiTypeInfo;
				}

				ForceInline friend auto operator<=(const TypeInfo& other, const RTTITypeRecord& item) {
					return (other == item) || (other < item);
				}

				ForceInline friend auto operator>=(const TypeInfo& other, const RTTITypeRecord& item) {
					return (other == item) || (other > item);
				}

				ForceInline friend auto operator<=>(const TypeInfo& other, const RTTITypeRecord& item) {
					if (other == item)
					{
						return Comparing::StrongOrdering::equal;
					}
					else if (other > item)
					{
						return Comparing::StrongOrdering::greater;
					}
					else
					{
						return Comparing::StrongOrdering::less;
					}
				}
			};

			class RTTIFunction : IDisposable, public Serilization::ISerilizable<RTTIFunction> {
				struct Data{
					RTTITypeRecord FunctionRecord;
					RTTITypeRecord FunctionRetRecord;
					Containers::List<RTTITypeRecord> FunctionArgsRecord;
					RefCountT RefCount;

					Data(
						RTTITypeRecord && FunctionRecord, 
						RTTITypeRecord && FunctionRetRecord, 
						Containers::List<RTTITypeRecord> && FunctionArgsRecord
					) :
						FunctionRecord(MoveRValue(FunctionRecord)),
						FunctionRetRecord(MoveRValue(FunctionRetRecord)),
						FunctionArgsRecord(MoveRValue(FunctionArgsRecord)),
						RefCount(1)
					{}
				};
			protected:
				virtual void Dispose() override {}
				virtual Byte* GetDataPtr() const { return reinterpret_cast<Byte*>(data);}
			private:
				Data * data;
			public:
				DefStructMetaData(Data, 
					DefStructFieldData(Data, FunctionRecord),
					DefStructFieldData(Data, FunctionRetRecord),
					DefStructFieldData(Data, FunctionArgsRecord)
					);

				RefCountClass(RTTIFunction, data);
			public:
				template<typename Ret, typename ... Args>
				RTTIFunction(Ret(*pFunction)(Args...))
					: data(new Data(
						RTTITypeRecord(TypeTraits::TypeTag<decltype(pFunction)>{}),
						RTTITypeRecord(TypeTraits::TypeTag<Ret>{}),
						{ RTTITypeRecord(TypeTraits::TypeTag<Args>{})... }
					)){}
			public:
				const RTTITypeRecord & GetFunctionRecord() { return data->FunctionRecord; }
				const RTTITypeRecord & GetFunctionRetRecord() { return data->FunctionRetRecord; }
				const Containers::List<RTTITypeRecord> & GetFunctionArgsRecord() { return data->FunctionArgsRecord; }

				const String & GetFunctionTypeName() { return data->FunctionRecord.GetTypeName(); }

				const auto& FunctionTypeId() { return data->FunctionRecord.TypeId(); }
			};

			class RTTIMethod : IDisposable, Serilization::ISerilizable<RTTIMethod> {
				struct Data{
					RTTITypeRecord MethodRecord;
					RTTITypeRecord MethodClassRecord;
					RTTITypeRecord MethodRetRecord;
					Containers::List<RTTITypeRecord> MethodArgsRecord;
					RefCountT RefCount;

					Data(
						RTTITypeRecord && MethodRecord, 
						RTTITypeRecord && MethodClassRecord,
						RTTITypeRecord && MethodRetRecord, 
						Containers::List<RTTITypeRecord> && MethodArgsRecord
					) :
						MethodRecord(MoveRValue(MethodRecord)),
						MethodClassRecord(MoveRValue(MethodClassRecord)),
						MethodRetRecord(MoveRValue(MethodRetRecord)),
						MethodArgsRecord(MoveRValue(MethodArgsRecord)),
						RefCount(1)
					{}
				};
			protected:
				virtual void Dispose() override {}
				virtual Byte* GetDataPtr() const { return reinterpret_cast<Byte*>(data);}
			private:
				Data * data;
			public:
				DefStructMetaData(Data, 
					DefStructFieldData(Data, MethodRecord),
					DefStructFieldData(Data, MethodClassRecord),
					DefStructFieldData(Data, MethodRetRecord),
					DefStructFieldData(Data, MethodArgsRecord)
					);

				RefCountClass(RTTIMethod, data);
			public:
				template<typename Ret, typename TClass, typename ... Args>
				RTTIMethod(Ret(TClass::*pMethod)(Args...))
					: data(new Data(
						RTTITypeRecord(TypeTraits::TypeTag<decltype(pMethod)>{}),
						RTTITypeRecord(TypeTraits::TypeTag<Ret>{}),
						RTTITypeRecord(TypeTraits::TypeTag<TClass>{}),
						{ RTTITypeRecord(TypeTraits::TypeTag<Args>{})... }
					)){}

				template<typename Ret, typename TClass, typename ... Args>
				RTTIMethod(Ret(TClass::*pMethod)(Args...) const)
					: data(new Data(
						RTTITypeRecord(TypeTraits::TypeTag<decltype(pMethod)>{}),
						RTTITypeRecord(TypeTraits::TypeTag<Ret>{}),
						RTTITypeRecord(TypeTraits::TypeTag<TClass>{}),
						{ RTTITypeRecord(TypeTraits::TypeTag<Args>{})... }
					)){}

				template<typename Ret, typename TClass, typename ... Args>
				RTTIMethod(Ret(TClass::*pMethod)(Args...) volatile)
					: data(new Data(
						RTTITypeRecord(TypeTraits::TypeTag<decltype(pMethod)>{}),
						RTTITypeRecord(TypeTraits::TypeTag<Ret>{}),
						RTTITypeRecord(TypeTraits::TypeTag<TClass>{}),
						{ RTTITypeRecord(TypeTraits::TypeTag<Args>{})... }
					)){}

				template<typename Ret, typename TClass, typename ... Args>
				RTTIMethod(Ret(TClass::*pMethod)(Args...) const volatile)
					: data(new Data(
						RTTITypeRecord(TypeTraits::TypeTag<decltype(pMethod)>{}),
						RTTITypeRecord(TypeTraits::TypeTag<Ret>{}),
						RTTITypeRecord(TypeTraits::TypeTag<TClass>{}),
						{ RTTITypeRecord(TypeTraits::TypeTag<Args>{})... }
					)){}
			public:
				const RTTITypeRecord & GetMethodRecord() { return data->MethodRecord; }
				const RTTITypeRecord & GetMethodClassRecord() { return data->MethodClassRecord; }
				const RTTITypeRecord & GetMethodRetRecord() { return data->MethodRetRecord; }
				const Containers::List<RTTITypeRecord> & GetMethodArgsRecord() { return data->MethodArgsRecord; }

				const String & GetMethodTypeName() { return data->MethodRecord.GetTypeName(); }


				const auto& MethodTypeId() { return data->MethodRecord.TypeId(); }
			};
		}
	}
}

template<typename T>
ForceInline Seed::Core::RTTI::RTTITypeRecord TypeRecord() {
	return Seed::Core::RTTI::RTTITypeRecord(Seed::Core::TypeTraits::TypeTag<T>{});
}