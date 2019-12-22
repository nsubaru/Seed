#pragma once
#include "Seed.Core.NewCTTI.h"
#include "Seed.Core.NewSerilization.h"

namespace Seed
{
	inline namespace Core
	{
		namespace RTTI
		{
			class SeedKernelCall RTTITypeRecord : IDisposable, NewSerilization::ISerilizable<RTTITypeRecord>
			{
				struct Data {
					String TypeName;
					String CxxTypeName;
					NewCTTI::TypeTraits::EType TypeClass;
					Boolean IsSerilizeable;
					const TypeInfo* rttiTypeInfo;
					RefCountT RefCount;

					Data(
						String && TypeName, 
						String && CxxTypeName, 
						NewCTTI::TypeTraits::EType TypeClass, 
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
				virtual Byte* GetDataPtr() const { return reinterpret_cast<Byte*>(data);}
			private:
				Data * data;
			public:
				DefStructMetaData(Data, 
					DefStructFieldData(Data, TypeName),
					DefStructFieldData(Data, CxxTypeName),
					DefStructFieldData(Data, TypeClass),
					DefStructFieldData(Data, IsSerilizeable)
					);

				RefCountClass(RTTITypeRecord, data);

				template<typename T>
				RTTITypeRecord(TypeTraits::TypeTag<T>) : data(new Data(
					NewCTTI::TypeTraits::CTTITypeMetaData<T>::TypeName,
					NewCTTI::TypeTraits::CTTITypeMetaData<T>::CXXTypeName,
					NewCTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass,
					NewCTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable,
					&typeid(T)
				))
				{}

				const String& GetTypeName() { return data->TypeName; }
				const String& GetCxxTypeName() { return data->CxxTypeName; }
				NewCTTI::TypeTraits::EType GetCxxTypeClass() { return data->TypeClass; }
				Boolean GetIsSerilizeable() { return data->IsSerilizeable; }
				const auto& TypeId() {return *data->rttiTypeInfo; }
			public:
				bool operator==(const RTTITypeRecord& other) const {
					return this->data->rttiTypeInfo == other.data->rttiTypeInfo;
				}

				bool operator!=(const RTTITypeRecord& other) const {
					return this->data->rttiTypeInfo != other.data->rttiTypeInfo;
				}

				bool operator<(const RTTITypeRecord& other) const {
					return this->data->rttiTypeInfo < other.data->rttiTypeInfo;
				}

				bool operator>(const RTTITypeRecord& other) const {
					return this->data->rttiTypeInfo > other.data->rttiTypeInfo;
				}

				bool operator<=(const RTTITypeRecord& other) const {
					return this->data->rttiTypeInfo <= other.data->rttiTypeInfo;
				}

				bool operator>=(const RTTITypeRecord& other) const {
					return this->data->rttiTypeInfo >= other.data->rttiTypeInfo;
				}
			};

			class SeedKernelCall RTTIFunction : IDisposable, NewSerilization::ISerilizable<RTTITypeRecord> {
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

			class SeedKernelCall RTTIMethod : IDisposable, NewSerilization::ISerilizable<RTTITypeRecord> {
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