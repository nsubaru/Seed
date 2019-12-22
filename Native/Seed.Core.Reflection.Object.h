#pragma once
#include "Seed.Core.Reflection.Base.h"
#include "Seed.Core.Reflection.DynamicObject.h"
#include "Seed.Core.Reflection.DataTypeInformation.h"

namespace Seed
{
	inline namespace Core
	{
		namespace Reflection
		{
			///<summary>Неможливо викликати динамічну функцію з даним набором аргументів</summary>
			class SeedKernelCall InvlaidDynamicFunctionArgs : public Exception {
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				InvlaidDynamicFunctionArgs(const ConstAString& File, const ConstAString& FunctName, SizeT Line)
					:Exception(File, FunctName, Line, L"Неможливо викликати динамічну функцію з даним набором аргументів") {}

				///<summary>Текстове представлення інформації про помилки помилки</summary>
				virtual ExceptionString toString() const override final {
					return ExceptionString(exeptionName)
						+ ExceptionString(L". ")
						+ location.toString()
						+ ExceptionString(L". ");
				}

				virtual ~InvlaidDynamicFunctionArgs() {}
			};

			namespace Details
			{
				template<typename ... TArgs>
				constexpr bool IsFunctionWithoutArgs =
					sizeof...(TArgs) == 0
					;//||
					//(sizeof...(TArgs) == 1 && TypeTraits::IsSameV<TypeTraits::Pack<void>, TypeTraits::Pack<TArgs...>>);

				template<typename T>
				struct IsCTTIStringT : TypeTraits::LogicFalse {};

				template<char ... TChars>
				struct IsCTTIStringT<NewCTTI::Details::TypeString<TChars...>> : TypeTraits::LogicTrue {};

				template<typename T>
				constexpr bool IsCTTIStringV = IsCTTIStringT<T>::State;

				template<char ... TChars>
				using CTTIString = NewCTTI::Details::TypeString<TChars...>;

				template<typename>
				struct MethodCallDescriptor {
					//static_assert(false, "Invalid method type passed");
				};

				template<typename TRet, typename TClass, typename ...TArgs>
				struct MethodCallDescriptor<TRet(TClass::*)(TArgs...)> {
					using MethodType = TRet(TClass::*)(TArgs...);

					using Class = TClass;

					static constexpr Boolean IsConst = false;
					static constexpr Boolean IsVolatile = false;

					static constexpr auto ArgsTypeName = NewCTTI::TypeName<Containers::Tuple<TArgs...>>();
					static constexpr auto RetTypeName = NewCTTI::TypeName<Containers::Tuple<TRet>>();

					static String ArgsMetaData() {
						return NewSerilization::MetaDataGen<Containers::Tuple<TArgs...>>();
					}

					static String RetMetaData() {
						if constexpr (TypeTraits::IsSameV<TRet, void>)
						{
							return String(L"None");
						}
						else
						{
							return NewSerilization::MetaDataGen<Containers::Tuple<TRet>>();
						}
					}

					static constexpr auto MethodCaller = [] (const void* const& pMethod, void* thisPtr, DynamicObject& dynamic) -> DynamicObject
					{
						if constexpr (IsFunctionWithoutArgs<TArgs...>)
						{
							MethodType method = reinterpret_cast<MethodType const&>(pMethod);
							TClass* pThis = reinterpret_cast<TClass*>(thisPtr);

							if constexpr (TypeTraits::IsSameV<TRet, void>)
							{

								if (dynamic.GetImplementationTypeInfo() == typeid(Containers::Tuple<>))
								{
									(pThis->*method)();
									return DynamicObject();
								}
								else
								{
									Throw(InvlaidDynamicFunctionArgs(ExceptionPosition));
								}
							}
							else
							{
								if (dynamic.GetImplementationTypeInfo() == typeid(Containers::Tuple<>))
								{
									return DynamicObject(Containers::Tuple<TRet>((pThis->*method)()));
								}
								else
								{
									Throw(InvlaidDynamicFunctionArgs(ExceptionPosition));
								}
							}
						}
						else
						{
							MethodType method = reinterpret_cast<MethodType const&>(pMethod);
							TClass* pThis = reinterpret_cast<TClass*>(thisPtr);

							if constexpr (TypeTraits::IsSameV<TRet, void>)
							{

								if (dynamic.GetImplementationTypeInfo() == typeid(Containers::Tuple<TArgs...>))
								{
									return dynamic.GetValue<TArgs...>().CallWithArgsFromTuple([&] (TArgs&&... args) -> DynamicObject
									{
										(pThis->*method)(args...);
										return DynamicObject();
									});
								}
								else
								{
									Throw(InvlaidDynamicFunctionArgs(ExceptionPosition));
								}
							}
							else
							{
								if (dynamic.GetImplementationTypeInfo() == typeid(Containers::Tuple<TArgs...>))
								{
									return dynamic.GetValue<TArgs...>().CallWithArgsFromTuple([&] (TArgs&&... args)
									{
										return DynamicObject(Containers::Tuple<TRet>((pThis->*method)(args...)));
									});
								}
								else
								{
									Throw(InvlaidDynamicFunctionArgs(ExceptionPosition));
								}
							}
						}
					};

					static constexpr auto ArgsTypeInformationGetter = [] () -> Containers::List<DataTypeRecord>
					{
						return Containers::List<DataTypeRecord>{DataTypeRecord(TypeTraits::TypeTag<TArgs>{})...};
					};

					static constexpr bool IsArgsSerilizable = IsFunctionWithoutArgs<TArgs...> ||
						(NewCTTI::TypeTraits::CTTITypeMetaData<TArgs>::IsSerilizable && ...);

					static constexpr bool IsRetSerilizable = TypeTraits::IsSameV<TRet, void> ||
						NewCTTI::TypeTraits::CTTITypeMetaData<TRet>::IsSerilizable;
				};

				template<typename TRet, typename TClass, typename ...TArgs>
				struct MethodCallDescriptor<TRet(TClass::*)(TArgs...) const> {
					using MethodType = TRet(TClass::*)(TArgs...) const;

					using Class = TClass;

					static constexpr Boolean IsConst = true;
					static constexpr Boolean IsVolatile = false;

					static constexpr auto ArgsTypeName = NewCTTI::TypeName<Containers::Tuple<TArgs...>>();
					static constexpr auto RetTypeName = NewCTTI::TypeName<Containers::Tuple<TRet>>();

					static String ArgsMetaData() {
						return NewSerilization::MetaDataGen<Containers::Tuple<TArgs...>>();
					}

					static String RetMetaData() {
						if constexpr (TypeTraits::IsSameV<TRet, void>)
						{
							return String(L"None");
						}
						else
						{
							return NewSerilization::MetaDataGen<Containers::Tuple<TRet>>();
						}
					}

					static constexpr auto MethodCaller = [] (const void* const& pMethod, void* thisPtr, DynamicObject& dynamic) -> DynamicObject
					{
						if constexpr (IsFunctionWithoutArgs<TArgs...>)
						{
							MethodType method = reinterpret_cast<MethodType const&>(pMethod);
							TClass* pThis = reinterpret_cast<TClass*>(thisPtr);

							if constexpr (TypeTraits::IsSameV<TRet, void>)
							{

								if (dynamic.GetImplementationTypeInfo() == typeid(Containers::Tuple<>))
								{
									pThis->*method(args...);
									return DynamicObject();
								}
								else
								{
									Throw(InvlaidDynamicFunctionArgs(ExceptionPosition));
								}
							}
							else
							{
								if (dynamic.GetImplementationTypeInfo() == typeid(Containers::Tuple<>))
								{
									return DynamicObject(Containers::Tuple<TRet>((pThis->*method)()));
								}
								else
								{
									Throw(InvlaidDynamicFunctionArgs(ExceptionPosition));
								}
							}
						}
						else
						{
							MethodType method = reinterpret_cast<MethodType const&>(pMethod);
							TClass* pThis = reinterpret_cast<TClass*>(thisPtr);

							if constexpr (TypeTraits::IsSameV<TRet, void>)
							{

								if (dynamic.GetImplementationTypeInfo() == typeid(Containers::Tuple<TArgs...>))
								{
									return dynamic.GetValue<TArgs...>().CallWithArgsFromTuple([&] (TArgs&&... args) -> DynamicObject
									{
										(pThis->*method)(args...);
										return DynamicObject();
									});
								}
								else
								{
									Throw(InvlaidDynamicFunctionArgs(ExceptionPosition));
								}
							}
							else
							{
								if (dynamic.GetImplementationTypeInfo() == typeid(Containers::Tuple<TArgs...>))
								{
									return dynamic.GetValue<TArgs...>().CallWithArgsFromTuple([&] (TArgs&&... args)
									{
										return DynamicObject(Containers::Tuple<TRet>(pThis->*method(args...)));
									});
								}
								else
								{
									Throw(InvlaidDynamicFunctionArgs(ExceptionPosition));
								}
							}
						}
					};

					static constexpr auto ArgsTypeInformationGetter = [] () -> Containers::List<DataTypeRecord>
					{
						return Containers::List<DataTypeRecord>{DataTypeRecord(TypeTraits::TypeTag<TArgs>{})...};
					};

					static constexpr bool IsArgsSerilizable = IsFunctionWithoutArgs<TArgs...> ||
						(NewCTTI::TypeTraits::CTTITypeMetaData<TArgs>::IsSerilizable && ...);

					static constexpr bool IsRetSerilizable = TypeTraits::IsSameV<TRet, void> ||
						NewCTTI::TypeTraits::CTTITypeMetaData<TRet>::IsSerilizable;
				};

				template<typename TRet, typename TClass, typename ...TArgs>
				struct MethodCallDescriptor<TRet(TClass::*)(TArgs...) volatile> {
					using MethodType = TRet(TClass::*)(TArgs...) volatile;

					using Class = TClass;

					static constexpr Boolean IsConst = false;
					static constexpr Boolean IsVolatile = true;

					static constexpr auto ArgsTypeName = NewCTTI::TypeName<Containers::Tuple<TArgs...>>();
					static constexpr auto RetTypeName = NewCTTI::TypeName<Containers::Tuple<TRet>>();

					static String ArgsMetaData() {
						return NewSerilization::MetaDataGen<Containers::Tuple<TArgs...>>();
					}

					static String RetMetaData() {
						if constexpr (TypeTraits::IsSameV<TRet, void>)
						{
							return String(L"None");
						}
						else
						{
							return NewSerilization::MetaDataGen<Containers::Tuple<TRet>>();
						}
					}

					static constexpr auto MethodCaller = [] (const void* const& pMethod, void* thisPtr, DynamicObject& dynamic) -> DynamicObject
					{
						if constexpr (IsFunctionWithoutArgs<TArgs...>)
						{
							MethodType method = reinterpret_cast<MethodType const&>(pMethod);
							TClass* pThis = reinterpret_cast<TClass*>(thisPtr);

							if constexpr (TypeTraits::IsSameV<TRet, void>)
							{

								if (dynamic.GetImplementationTypeInfo() == typeid(Containers::Tuple<>))
								{
									pThis->*method();
									return DynamicObject();
								}
								else
								{
									Throw(InvlaidDynamicFunctionArgs(ExceptionPosition));
								}
							}
							else
							{
								if (dynamic.GetImplementationTypeInfo() == typeid(Containers::Tuple<>))
								{
									return DynamicObject(Containers::Tuple<TRet>((pThis->*method)()));
								}
								else
								{
									Throw(InvlaidDynamicFunctionArgs(ExceptionPosition));
								}
							}
						}
						else
						{
							MethodType method = reinterpret_cast<MethodType const&>(pMethod);
							TClass* pThis = reinterpret_cast<TClass*>(thisPtr);

							if constexpr (TypeTraits::IsSameV<TRet, void>)
							{

								if (dynamic.GetImplementationTypeInfo() == typeid(Containers::Tuple<TArgs...>))
								{
									return dynamic.GetValue<TArgs...>().CallWithArgsFromTuple([&] (TArgs&&... args) -> DynamicObject
									{
										(pThis->*method)(args...);
										return DynamicObject();
									});
								}
								else
								{
									Throw(InvlaidDynamicFunctionArgs(ExceptionPosition));
								}
							}
							else
							{
								if (dynamic.GetImplementationTypeInfo() == typeid(Containers::Tuple<TArgs...>))
								{
									return dynamic.GetValue<TArgs...>().CallWithArgsFromTuple([&] (TArgs&&... args)
									{
										return DynamicObject(Containers::Tuple<TRet>(pThis->*method(args...)));
									});
								}
								else
								{
									Throw(InvlaidDynamicFunctionArgs(ExceptionPosition));
								}
							}
						}
					};

					static constexpr auto ArgsTypeInformationGetter = [] () -> Containers::List<DataTypeRecord>
					{
						return Containers::List<DataTypeRecord>{DataTypeRecord(TypeTraits::TypeTag<TArgs>{})...};
					};

					static constexpr bool IsArgsSerilizable = IsFunctionWithoutArgs<TArgs...> ||
						(NewCTTI::TypeTraits::CTTITypeMetaData<TArgs>::IsSerilizable && ...);

					static constexpr bool IsRetSerilizable = TypeTraits::IsSameV<TRet, void> ||
						NewCTTI::TypeTraits::CTTITypeMetaData<TRet>::IsSerilizable;
				};

				template<typename TRet, typename TClass, typename ...TArgs>
				struct MethodCallDescriptor<TRet(TClass::*)(TArgs...) const volatile> {
					using MethodType = TRet(TClass::*)(TArgs...) const volatile;

					using Class = TClass;

					static constexpr Boolean IsConst = true;
					static constexpr Boolean IsVolatile = true;

					static constexpr auto ArgsTypeName = NewCTTI::TypeName<Containers::Tuple<TArgs...>>();
					static constexpr auto RetTypeName = NewCTTI::TypeName<Containers::Tuple<TRet>>();

					static String ArgsMetaData() {
						return NewSerilization::MetaDataGen<Containers::Tuple<TArgs...>>();
					}

					static String RetMetaData() {
						if constexpr (TypeTraits::IsSameV<TRet, void>)
						{
							return String(L"None");
						}
						else
						{
							return NewSerilization::MetaDataGen<Containers::Tuple<TRet>>();
						}
					}

					static constexpr auto MethodCaller = [] (const void* const& pMethod, void* thisPtr, DynamicObject& dynamic) -> DynamicObject
					{
						if constexpr (IsFunctionWithoutArgs<TArgs...>)
						{
							MethodType method = reinterpret_cast<MethodType const&>(pMethod);
							TClass* pThis = reinterpret_cast<TClass*>(thisPtr);

							if constexpr (TypeTraits::IsSameV<TRet, void>)
							{

								if (dynamic.GetImplementationTypeInfo() == typeid(Containers::Tuple<>))
								{
									pThis->*method();
									return DynamicObject();
								}
								else
								{
									Throw(InvlaidDynamicFunctionArgs(ExceptionPosition));
								}
							}
							else
							{
								if (dynamic.GetImplementationTypeInfo() == typeid(Containers::Tuple<>))
								{
									return DynamicObject(Containers::Tuple<TRet>((pThis->*method)()));
								}
								else
								{
									Throw(InvlaidDynamicFunctionArgs(ExceptionPosition));
								}
							}
						}
						else
						{
							MethodType method = reinterpret_cast<MethodType const&>(pMethod);
							TClass* pThis = reinterpret_cast<TClass*>(thisPtr);

							if constexpr (TypeTraits::IsSameV<TRet, void>)
							{

								if (dynamic.GetImplementationTypeInfo() == typeid(Containers::Tuple<TArgs...>))
								{
									return dynamic.GetValue<TArgs...>().CallWithArgsFromTuple([&] (TArgs&&... args) -> DynamicObject
									{
										(pThis->*method)(args...);
										return DynamicObject();
									});
								}
								else
								{
									Throw(InvlaidDynamicFunctionArgs(ExceptionPosition));
								}
							}
							else
							{
								if (dynamic.GetImplementationTypeInfo() == typeid(Containers::Tuple<TArgs...>))
								{
									return dynamic.GetValue<TArgs...>().CallWithArgsFromTuple([&] (TArgs&&... args)
									{
										return DynamicObject(Containers::Tuple<TRet>(pThis->*method(args...)));
									});
								}
								else
								{
									Throw(InvlaidDynamicFunctionArgs(ExceptionPosition));
								}
							}
						}
					};

					static constexpr auto ArgsTypeInformationGetter = [] () -> Containers::List<DataTypeRecord>
					{
						return Containers::List<DataTypeRecord>{DataTypeRecord(TypeTraits::TypeTag<TArgs>{})...};
					};

					static constexpr bool IsArgsSerilizable = IsFunctionWithoutArgs<TArgs...> ||
						(NewCTTI::TypeTraits::CTTITypeMetaData<TArgs>::IsSerilizable && ...);

					static constexpr bool IsRetSerilizable = TypeTraits::IsSameV<TRet, void> ||
						NewCTTI::TypeTraits::CTTITypeMetaData<TRet>::IsSerilizable;
				};

				template<typename T, T pMethodType, typename TName>
				struct MethodDescriptor {
					static_assert(IsCTTIStringV<TName>, "TName must be CTTIString");

					static constexpr auto Name = Seed::Core::NewCTTI::Details::TypeStringToArray(TName());
					using MethodCallType = typename MethodCallDescriptor<T>::MethodType;

					using MethodClass = typename MethodCallDescriptor<T>::Class;

					static constexpr Boolean IsConst = MethodCallDescriptor<T>::IsConst;
					static constexpr Boolean IsVolatile = MethodCallDescriptor<T>::IsVolatile;

					static constexpr auto ArgsTypeName = MethodCallDescriptor<T>::ArgsTypeName;
					static constexpr auto RetTypeName = MethodCallDescriptor<T>::RetTypeName;

					static constexpr auto pMethod = pMethodType;

					static String ArgsMetaData() {
						return MethodCallDescriptor<T>::ArgsMetaData();
					}

					static String RetMetaData() {
						return MethodCallDescriptor<T>::RetMetaData();
					}

					static constexpr auto MethodCaller = MethodCallDescriptor<T>::MethodCaller;

					static constexpr auto ArgsTypeInformationGetter = MethodCallDescriptor<T>::ArgsTypeInformationGetter;

					static constexpr bool IsArgsSerilizable = MethodCallDescriptor<T>::IsArgsSerilizable;

					static constexpr bool IsRetSerilizable = MethodCallDescriptor<T>::IsRetSerilizable;
				};

				template<typename T, typename ... TArgs>
				struct ObjectCtorAndDtor {
					static_assert(Core::TypeTraits::IsConstructible<T, TArgs...>, "Can not construct class with this args");

					static constexpr auto UnifiedCtor = [] (DynamicObject&& args) -> void*
					{
						if constexpr (IsFunctionWithoutArgs<TArgs...>)
						{
							if (args.GetImplementationTypeInfo() == typeid(Containers::Tuple<>))
							{
								return new T();
							}
							else
							{
								Throw(InvlaidDynamicFunctionArgs(ExceptionPosition));
							}
						}
						else
						{
							if (args.GetImplementationTypeInfo() == typeid(Containers::Tuple<TArgs...>))
							{
								return args.GetValue<TArgs...>().CallWithArgsFromTuple([] (TArgs&&... args)
								{
									return new T(args...);
								});
							}
							else
							{
								Throw(InvlaidDynamicFunctionArgs(ExceptionPosition));
							}
						}
					};

					static constexpr auto UnifiedDtor = [] (void*& pObject)
					{
						delete static_cast<T*>(pObject);
						pObject = nullptr;
					};

					static constexpr auto CtorArgsTypeName = NewCTTI::TypeName<Containers::Tuple<TArgs...>>();

					static String ArgsMetaData() {
						return NewSerilization::MetaDataGen<Containers::Tuple<TArgs...>>();
					}
				};

				template<typename TObj, typename TObjectCtorAndDtor, typename ... TMethods>
				struct ObjectDescriptor {
					static constexpr auto Name = NewCTTI::TypeName<TObj>();

					static constexpr auto RttiTypeInfo = [] () 
					{
						return &typeid(TObj);
					};
						
					static constexpr auto UnifiedCtor = TObjectCtorAndDtor::UnifiedCtor;
					static constexpr auto UnifiedDtor = TObjectCtorAndDtor::UnifiedDtor;

					using Methods = Containers::Tuple<TMethods...>;

					static constexpr auto CtorArgsTypeName = TObjectCtorAndDtor::CtorArgsTypeName;

					static String ArgsMetaData() {
						return TObjectCtorAndDtor::ArgsMetaData();
					}

				private:
					template<SizeT TId = 0>
					static constexpr bool IsAllMethodsHaveCorrectClassImpl() {
						if constexpr (TId == sizeof...(TMethods))
						{
							return true;
						}
						else
						{
							return
								TypeTraits::IsSameV<
								Methods::ItemType<TId>::MethodClass,
								TObj>
								&&
								IsAllMethodsHaveCorrectClassImpl<TId + 1>();
						}
					}
				public:
					static_assert(IsAllMethodsHaveCorrectClassImpl(), "Invalid method passed");
				};
			}

			class ObjectRTTIRecord final : IDisposable, NewSerilization::ISerilizable<ObjectRTTIRecord> {
			public:
				using CtorCaller = void* (*)(DynamicObject&&);
				using DtorCaller = void (*)(void*&);
				using MethodCaller = DynamicObject(*)(const void* const&, void*, DynamicObject&);
				using ArgsTypeInformationGetter = Containers::List<DataTypeRecord>(*)();
			public:
				class MethodRTTIRecord : IDisposable, NewSerilization::ISerilizable<MethodRTTIRecord> {
				private:
					struct Data {
						String MethodName;
						MethodCaller methodCaller;
						const void* const& pMethod;
						String MethodArgs;
						String MethodRet;
						String MethodArgsMetaData;
						String MethodRetMetaData;

						ArgsTypeInformationGetter argsTypeInformationGetter;
						Boolean IsRetSerilizable;
						Boolean IsArgsSerilizable;

						Boolean IsConstMethod;
						Boolean IsVolatileMethod;

						RefCountT RefCount;

						Data(
							String&& methodName,
							MethodCaller methodCaller,
							const void* const& pMethod,
							String&& methodArgs,
							String&& methodRet,
							String&& methodArgsMetaData,
							String&& methodRetMetaData,
							ArgsTypeInformationGetter argsTypeInformationGetter,
							Boolean IsRetSerilizable,
							Boolean IsArgsSerilizable,
							Boolean isConstMethod,
							Boolean isVolatileMethod
						)
							:
							MethodName(MoveRValue(methodName)),
							methodCaller(methodCaller),
							pMethod(pMethod),
							MethodArgs(MoveRValue(methodArgs)),
							MethodRet(MoveRValue(methodRet)),
							MethodArgsMetaData(MoveRValue(methodArgsMetaData)),
							MethodRetMetaData(MoveRValue(methodRetMetaData)),
							argsTypeInformationGetter(argsTypeInformationGetter),
							IsRetSerilizable(IsRetSerilizable),
							IsArgsSerilizable(IsArgsSerilizable),
							IsConstMethod(isConstMethod),
							IsVolatileMethod(isVolatileMethod),
							RefCount(1) {}
					};
				private:
					Data* data;
				protected:
					virtual void Dispose() override {}
					virtual Byte* GetDataPtr() const override {
						return reinterpret_cast<Byte*>(data);
					}
				public:
					DefStructMetaData(Data,
						DefStructFieldData(Data, MethodName),
						DefStructFieldData(Data, MethodArgs),
						DefStructFieldData(Data, MethodRet),
						DefStructFieldData(Data, MethodArgsMetaData),
						DefStructFieldData(Data, MethodRetMetaData),
						DefStructFieldData(Data, IsRetSerilizable),
						DefStructFieldData(Data, IsArgsSerilizable),
						DefStructFieldData(Data, IsConstMethod),
						DefStructFieldData(Data, IsVolatileMethod)
					);
				public:
					template<typename TMethodDescriptor>
					forceinline MethodRTTIRecord(TypeTraits::TypeTag<TMethodDescriptor>)
						: data(new Data(
							TMethodDescriptor::Name,
							TMethodDescriptor::MethodCaller,
							reinterpret_cast<const void* const&>(TMethodDescriptor::pMethod),
							TMethodDescriptor::ArgsTypeName,
							TMethodDescriptor::RetTypeName,
							TMethodDescriptor::ArgsMetaData(),
							TMethodDescriptor::RetMetaData(),
							TMethodDescriptor::ArgsTypeInformationGetter,
							TMethodDescriptor::IsRetSerilizable,
							TMethodDescriptor::IsArgsSerilizable,
							TMethodDescriptor::IsConst,
							TMethodDescriptor::IsVolatile
						)) {}

					const String& GetName() const {
						return this->data->MethodName;
					}

					MethodCaller GetMethodCaller() const {
						return this->data->methodCaller;
					}

					const String& GetArgsTypeName() const {
						return this->data->MethodArgs;
					}

					const String& GetRetTypeName() const {
						return this->data->MethodRet;
					}

					const String& GetArgsMetaData() const {
						return this->data->MethodArgsMetaData;
					}

					const String& GetRetMetaData() const {
						return this->data->MethodRetMetaData;
					}

					ArgsTypeInformationGetter GetArgsTypeInformationGetter() const {
						return this->data->argsTypeInformationGetter;
					}

					bool GetIsRetSerilizable() const {
						return this->data->IsRetSerilizable;
					}

					bool GetIsArgsSerilizable() const {
						return this->data->IsRetSerilizable;
					}

					bool GetIsConst() const {
						return this->data->IsConstMethod;
					}

					bool GetIsVolatile() const {
						return this->data->IsVolatileMethod;
					}

					const void* const& GetMethodPointer() const {
						return this->data->pMethod;
					}

					String Property(GetName) Name;
					MethodCaller Property(GetMethodCaller) MethodCallerFunction;
					String Property(GetArgsTypeName) ArgsTypeName;
					String Property(GetRetTypeName) RetTypeName;
					String Property(GetArgsMetaData) ArgsMetaData;
					String Property(GetRetMetaData) RetMetaData;
					ArgsTypeInformationGetter Property(GetArgsTypeInformationGetter) ArgsTypeInformationGetterFunction;
					bool Property(GetIsRetSerilizable) IsRetSerilizable;
					bool Property(GetIsArgsSerilizable) IsArgsSerilizable;
					bool Property(GetIsConst) IsConst;
					bool Property(GetIsVolatile) IsVolatile;


					RefCountClass(MethodRTTIRecord, data);
				};
			private:
				struct Data {
					String ClassName;
					String CtorArgs;
					String CtorMetaDate;
					CtorCaller ctorCaller;
					DtorCaller dtorCaller;
					const TypeInfo* RttiTypeInfo;
					Containers::List<MethodRTTIRecord> Methods;
					RefCountT RefCount;

					Data(
						String&& className,
						String&& ctorArgs,
						String&& ctorMetaDate,
						CtorCaller ctorCaller,
						DtorCaller dtorCaller,
						const TypeInfo* RttiTypeInfo,
						Containers::List<MethodRTTIRecord>&& methods
					) :
						ClassName(MoveRValue(className)),
						CtorArgs(MoveRValue(ctorArgs)),
						CtorMetaDate(MoveRValue(ctorMetaDate)),
						ctorCaller(ctorCaller),
						dtorCaller(dtorCaller),
						RttiTypeInfo(RttiTypeInfo),
						Methods(MoveRValue(methods)),
						RefCount(1) {}

				};
			private:
				Data* data;
			private:
				template<SizeT TId, typename ... TMethod>
				static Containers::List<MethodRTTIRecord> GenerateRTTIMethodInfo(TypeTraits::TypeTag<Containers::Tuple<TMethod...>>) {
					if constexpr (TId == sizeof...(TMethod))
					{
						return Containers::List<MethodRTTIRecord>();
					}
					else
					{
						return Containers::List<MethodRTTIRecord>(MoveRValue(
							(Containers::List<MethodRTTIRecord>({
							MethodRTTIRecord(TypeTraits::TypeTag<typename Containers::Tuple<TMethod...>::ItemType<TId>>{})
								})
								+=
								MoveRValue(GenerateRTTIMethodInfo<TId + 1>(TypeTraits::TypeTag<Containers::Tuple<TMethod...>>{})))
						));
					}
				}
			protected:
				virtual void Dispose() override {}
				virtual Byte* GetDataPtr() const override {
					return reinterpret_cast<Byte*>(data);
				}
			public:
				DefStructMetaData(Data,
					DefStructFieldData(Data, ClassName),
					DefStructFieldData(Data, CtorArgs),
					DefStructFieldData(Data, CtorMetaDate),
					DefStructFieldData(Data, Methods)
				);
			public:
				template<typename TObjectDescriptor>
				ObjectRTTIRecord(TypeTraits::TypeTag<TObjectDescriptor>) :
					data(new Data(
						TObjectDescriptor::Name,
						TObjectDescriptor::CtorArgsTypeName,
						TObjectDescriptor::ArgsMetaData(),
						TObjectDescriptor::UnifiedCtor,
						TObjectDescriptor::UnifiedDtor,
						TObjectDescriptor::RttiTypeInfo(),
						GenerateRTTIMethodInfo<0>(TypeTraits::TypeTag<TObjectDescriptor::Methods>{})
					)) {};

				const String& GetName() const {
					return this->data->ClassName;
				}

				const String& GetCtorArgs() const {
					return this->data->CtorArgs;
				}

				const String& GetArgsMetaData() const {
					return this->data->CtorMetaDate;
				}

				CtorCaller GetCtorCaller() const {
					return this->data->ctorCaller;
				}

				DtorCaller GetDtorCaller() const {
					return this->data->dtorCaller;
				}

				const Containers::List<MethodRTTIRecord>& GetAllMethods() const {
					return this->data->Methods;
				}

				const TypeInfo& GetRTTITypeInfo() const {
					return *this->data->RttiTypeInfo;
				}

				String Property(GetName) ClassName;
				String Property(GetCtorArgs) CtorArgs;
				String Property(GetArgsMetaData) ArgsMetaData;
				CtorCaller Property(GetCtorCaller) CtorCallerFunction;
				DtorCaller Property(GetDtorCaller) DtorCallerFunction;
				Containers::List<MethodRTTIRecord> Property(GetAllMethods) Methods;

				RefCountClass(ObjectRTTIRecord, data);
			};

		}
	}
}

#define MethodDescription(Class, Method) Seed::Core::Reflection::Details::MethodDescriptor<decltype(&Class::Method), &Class::Method, GenNGTypeString(#Method)>
#define CtorDescription(Class, ...) Seed::Core::Reflection::Details::ObjectCtorAndDtor<Class, __VA_ARGS__>
#define ObjectDefinition(Class, CtorDesc, ...) Seed::Core::Reflection::Details::ObjectDescriptor<Class, CtorDesc, __VA_ARGS__>