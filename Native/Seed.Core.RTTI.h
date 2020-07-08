#pragma once
#include "Seed.Core.CTTI.h"
#include "Seed.Core.Serilization.h"

namespace Seed
{
	inline namespace Core
	{
		namespace RTTI
		{
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

				template<typename T>
				forceinline RTTITypeRecord(TypeTraits::TypeTag<T>) : data(new Data(
					Lazy<String>(ArrowFunct(String(CTTI::TypeTraits::CTTITypeMetaData<T>::TypeName))),
					Lazy<String>(ArrowFunct(String(CTTI::TypeTraits::CTTITypeMetaData<T>::CXXTypeName))),
					CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass,
					CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable,
					&typeid(T)
				))
				{}

				forceinline const String& GetTypeName() const {
					return *data->TypeName;
				}

				forceinline const String& GetCxxTypeName() const {
					return *data->CxxTypeName;
				}

				forceinline CTTI::TypeTraits::EType GetCxxTypeClass() const {
					return data->TypeClass;
				}

				forceinline Boolean GetIsSerilizeable() const {
					return data->IsSerilizeable;
				}

				forceinline const auto& TypeId() const {
					return *data->rttiTypeInfo;
				}

				forceinline ID GetHashCode() const {
					return data->rttiTypeInfo->GetHashCode();
				}
			public:
				String					Property(GetTypeName)		TypeName;
				String					Property(GetCxxTypeName)	CxxTypeName;
				CTTI::TypeTraits::EType Property(GetCxxTypeClass)	CxxTypeClass;
				Boolean					Property(GetIsSerilizeable) IsSerilizeable;
				ID						Property(GetHashCode)		HashCode;
			public:
				forceinline auto operator==(const RTTITypeRecord& other) const {
					return *this->data->rttiTypeInfo == *other.data->rttiTypeInfo;
				}

				forceinline auto operator!=(const RTTITypeRecord& other) const {
					return *this->data->rttiTypeInfo != *other.data->rttiTypeInfo;
				}

				forceinline auto operator<=>(const RTTITypeRecord& other) const {
					return this->data->rttiTypeInfo->HashCode <=> other.data->rttiTypeInfo->HashCode;
				}

				forceinline auto operator==(const TypeInfo& other) const {
					return *this->data->rttiTypeInfo == other;
				}

				forceinline auto operator!=(const TypeInfo& other) const {
					return *this->data->rttiTypeInfo != other;
				}

				forceinline friend auto operator==(const TypeInfo& other, const RTTITypeRecord& item) {
					return other == *item.data->rttiTypeInfo;
				}

				forceinline friend auto operator!=(const TypeInfo& other, const RTTITypeRecord& item) {
					return other == *item.data->rttiTypeInfo;
				}

				forceinline friend auto operator<=>(const TypeInfo& other, const RTTITypeRecord& item) {
					return other.HashCode <=> item.data->rttiTypeInfo->HashCode;
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
forceinline Seed::Core::RTTI::RTTITypeRecord TypeRecord() {
	return Seed::Core::RTTI::RTTITypeRecord(Seed::Core::TypeTraits::TypeTag<T>{});
}