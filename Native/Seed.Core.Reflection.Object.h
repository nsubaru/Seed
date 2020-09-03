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
			class InvlaidDynamicFunctionArgs : public Exception {
			public:
				InvlaidDynamicFunctionArgs(const InvlaidDynamicFunctionArgs& orig) : Exception(orig) {}
				InvlaidDynamicFunctionArgs(InvlaidDynamicFunctionArgs&& from) : Exception(MoveRValue(from)) {}
				InvlaidDynamicFunctionArgs& operator=(const InvlaidDynamicFunctionArgs& from) { this->~InvlaidDynamicFunctionArgs(); this->InvlaidDynamicFunctionArgs::InvlaidDynamicFunctionArgs(from); return *this; }
				InvlaidDynamicFunctionArgs& operator=(InvlaidDynamicFunctionArgs&& from) { this->~InvlaidDynamicFunctionArgs(); this->InvlaidDynamicFunctionArgs::InvlaidDynamicFunctionArgs(MoveRValue(from)); return *this; }
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				InvlaidDynamicFunctionArgs(const ConstAString& File, const ConstAString& FunctName, SizeT Line)
					:Exception(File, FunctName, Line, L"Неможливо викликати динамічну функцію з даним набором аргументів") {}

				///<summary>Текстове представлення інформації про помилки помилки</summary>
				virtual ExceptionString toString() const override final {
					return ExceptionString::Combine({
						ExceptionString(exceptionName), ExceptionString(L". "),
						location.toString(), 
						(innerException == nullptr) ? ExceptionString(L".") : (L". Також є вкладена помилка: " + InnerException->toString())
					});
				}

				/// <summary>
				/// Вертає копію об'єкта помилки
				/// </summary>
				/// <returns>Розумний покажчик на об'єкт помилки</returns>
				virtual ExceptionHolderType Clone() const override {
					return ExceptionHolderType(new InvlaidDynamicFunctionArgs(*this));
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
				struct IsCTTIStringT<CTTI::Details::TypeString<TChars...>> : TypeTraits::LogicTrue {};

				template<typename T>
				constexpr bool IsCTTIStringV = IsCTTIStringT<T>::State;

				template<char ... TChars>
				using CTTIString = CTTI::Details::TypeString<TChars...>;

				template<typename>
				struct MethodCallDescriptor {
					//static_assert(false, "Invalid method type passed");
				};

				template<typename TRet, typename TClass, typename ...TArgs>
				struct MethodCallDescriptor<TRet(TClass::*)(TArgs...)> {
					using MethodType = TRet(TClass::*)(TArgs...);
					using ArgsType = typename Containers::Tuple<typename TypeTraits::RemoveCVRefV<TArgs>...>;

					using Class = TClass;

					static constexpr Boolean IsConst = false;
					static constexpr Boolean IsVolatile = false;

					static constexpr auto ArgsTypeName = CTTI::TypeName<ArgsType>();
					static constexpr auto RetTypeName = CTTI::TypeName<TypeTraits::RemoveCVRefV<TRet>>();

					static String ArgsMetaData() {
						return Serilization::MetaDataGen<ArgsType>();
					}

					static String RetMetaData() {
						if constexpr (TypeTraits::IsSameV<TRet, void>)
						{
							return String(L"None");
						}
						else
						{
							return Serilization::MetaDataGen<Containers::Tuple<TRet>>();
						}
					}

					static constexpr auto MethodCaller = [] (const void* const& pMethod, DynamicObject& This, DynamicObject& dynamic) -> DynamicObject
					{
						if constexpr (IsFunctionWithoutArgs<TArgs...>)
						{
							MethodType method = reinterpret_cast<MethodType const&>(pMethod);
							TClass& pThis = This.GetValue<TClass>().GetByIdx<0>();

							if constexpr (TypeTraits::IsSameV<TRet, void>)
							{

								if (dynamic.GetImplementationTypeInfo() == typeid(Containers::Tuple<>))
								{
									((&pThis)->*method)();
									return DynamicObject();
								}
								else
								{
									Throw(InvlaidDynamicFunctionArgs(__FILE__, "", __LINE__));
								}
							}
							else
							{
								if (dynamic.GetImplementationTypeInfo() == typeid(Containers::Tuple<>))
								{
									return DynamicObject(Containers::Tuple<TRet>(((&pThis)->*method)()));
								}
								else
								{
									Throw(InvlaidDynamicFunctionArgs(__FILE__, "", __LINE__));
								}
							}
						}
						else
						{
							MethodType method = reinterpret_cast<MethodType const&>(pMethod);
							TClass& pThis = This.GetValue<TClass>().GetByIdx<0>();

							if constexpr (TypeTraits::IsSameV<TRet, void>)
							{

								if (dynamic.GetImplementationTypeInfo() == typeid(ArgsType))
								{
									return dynamic.GetValue<TypeTraits::RemoveCVRefV<TArgs>...>().CallWithArgsFromTuple([&] (TArgs&&... args) -> DynamicObject
									{
										((&pThis)->*method)(ForwardReference<TArgs>(args)...);
										return DynamicObject();
									});
								}
								else
								{
									Throw(InvlaidDynamicFunctionArgs(__FILE__, "", __LINE__));
								}
							}
							else
							{
								if (dynamic.GetImplementationTypeInfo() == typeid(ArgsType))
								{
									return dynamic.GetValue<TypeTraits::RemoveCVRefV<TArgs>...>()
										.CallWithArgsFromTuple([&] (TypeTraits::RemoveCVRefV<TArgs>&&... args)
									{
										return DynamicObject(Containers::Tuple<TypeTraits::RemoveCVRefV<TRet>>(((&pThis)->*method)(args...)));
									});
								}
								else
								{
									Throw(InvlaidDynamicFunctionArgs(__FILE__, "", __LINE__));
								}
							}
						}
					};

					static constexpr auto TypeInformationGetter = [] () -> Containers::List<DataTypeRecord>
					{
						if constexpr (TypeTraits::IsSameV<TRet, void>)
						{
							return Containers::List<DataTypeRecord>{DataTypeRecord(TypeTraits::TypeTag<TypeTraits::RemoveCVRefV<TArgs>>{})...};
						}
						else
						{
							return Containers::List<DataTypeRecord>{
								DataTypeRecord(TypeTraits::TypeTag<TypeTraits::RemoveCVRefV<TRet>>{}), 
								DataTypeRecord(TypeTraits::TypeTag<TypeTraits::RemoveCVRefV<TArgs>>{})...
							};
						}
					};

					static constexpr bool IsArgsSerilizable = IsFunctionWithoutArgs<TArgs...> ||
						(CTTI::TypeTraits::CTTITypeMetaData<TypeTraits::RemoveCVRefV<TArgs>>::IsSerilizable && ...);

					static constexpr bool IsRetSerilizable = TypeTraits::IsSameV<TRet, void> ||
						CTTI::TypeTraits::CTTITypeMetaData<TypeTraits::RemoveCVRefV<TRet>>::IsSerilizable;

					using Ret = TypeTraits::RemoveCVRefV<TRet>;
					using Args = ArgsType;
				};

				template<typename TRet, typename TClass, typename ...TArgs>
				struct MethodCallDescriptor<TRet(TClass::*)(TArgs...) const> {
					using MethodType = TRet(TClass::*)(TArgs...);
					using ArgsType = typename Containers::Tuple<typename TypeTraits::RemoveCVRefV<TArgs>...>;

					using Class = TClass;

					static constexpr Boolean IsConst = true;
					static constexpr Boolean IsVolatile = false;

					static constexpr auto ArgsTypeName = CTTI::TypeName<ArgsType>();
					static constexpr auto RetTypeName = CTTI::TypeName<TypeTraits::RemoveCVRefV<TRet>>();

					static String ArgsMetaData() {
						return Serilization::MetaDataGen<ArgsType>();
					}

					static String RetMetaData() {
						if constexpr (TypeTraits::IsSameV<TRet, void>)
						{
							return String(L"None");
						}
						else
						{
							return Serilization::MetaDataGen<Containers::Tuple<TRet>>();
						}
					}

					static constexpr auto MethodCaller = [] (const void* const& pMethod, DynamicObject& This, DynamicObject& dynamic) -> DynamicObject
					{
						if constexpr (IsFunctionWithoutArgs<TArgs...>)
						{
							MethodType method = reinterpret_cast<MethodType const&>(pMethod);
							TClass& pThis = This.GetValue<TClass>().GetByIdx<0>();

							if constexpr (TypeTraits::IsSameV<TRet, void>)
							{

								if (dynamic.GetImplementationTypeInfo() == typeid(Containers::Tuple<>))
								{
									((&pThis)->*method)();
									return DynamicObject();
								}
								else
								{
									Throw(InvlaidDynamicFunctionArgs(__FILE__, "", __LINE__));
								}
							}
							else
							{
								if (dynamic.GetImplementationTypeInfo() == typeid(Containers::Tuple<>))
								{
									return DynamicObject(Containers::Tuple<TRet>(((&pThis)->*method)()));
								}
								else
								{
									Throw(InvlaidDynamicFunctionArgs(__FILE__, "", __LINE__));
								}
							}
						}
						else
						{
							MethodType method = reinterpret_cast<MethodType const&>(pMethod);
							TClass& pThis = This.GetValue<TClass>().GetByIdx<0>();

							if constexpr (TypeTraits::IsSameV<TRet, void>)
							{

								if (dynamic.GetImplementationTypeInfo() == typeid(ArgsType))
								{
									return dynamic.GetValue<TypeTraits::RemoveCVRefV<TArgs>...>().CallWithArgsFromTuple([&] (TArgs&&... args) -> DynamicObject
									{
										((&pThis)->*method)(ForwardReference<TArgs>(args)...);
										return DynamicObject();
									});
								}
								else
								{
									Throw(InvlaidDynamicFunctionArgs(__FILE__, "", __LINE__));
								}
							}
							else
							{
								if (dynamic.GetImplementationTypeInfo() == typeid(ArgsType))
								{
									return dynamic.GetValue<TypeTraits::RemoveCVRefV<TArgs>...>()
										.CallWithArgsFromTuple([&] (TypeTraits::RemoveCVRefV<TArgs>&&... args)
									{
										return DynamicObject(Containers::Tuple<TypeTraits::RemoveCVRefV<TRet>>(((&pThis)->*method)(args...)));
									});
								}
								else
								{
									Throw(InvlaidDynamicFunctionArgs(__FILE__, "", __LINE__));
								}
							}
						}
					};

					static constexpr auto TypeInformationGetter = [] () -> Containers::List<DataTypeRecord>
					{
						if constexpr (TypeTraits::IsSameV<TRet, void>)
						{
							return Containers::List<DataTypeRecord>{DataTypeRecord(TypeTraits::TypeTag<TypeTraits::RemoveCVRefV<TArgs>>{})...};
						}
						else
						{
							return Containers::List<DataTypeRecord>{
								DataTypeRecord(TypeTraits::TypeTag<TypeTraits::RemoveCVRefV<TRet>>{}), 
								DataTypeRecord(TypeTraits::TypeTag<TypeTraits::RemoveCVRefV<TArgs>>{})...
							};
						}
					};

					static constexpr bool IsArgsSerilizable = IsFunctionWithoutArgs<TArgs...> ||
						(CTTI::TypeTraits::CTTITypeMetaData<TypeTraits::RemoveCVRefV<TArgs>>::IsSerilizable && ...);

					static constexpr bool IsRetSerilizable = TypeTraits::IsSameV<TRet, void> ||
						CTTI::TypeTraits::CTTITypeMetaData<TypeTraits::RemoveCVRefV<TRet>>::IsSerilizable;

					using Ret = TypeTraits::RemoveCVRefV<TRet>;
					using Args = ArgsType;
				};

				template<typename TRet, typename TClass, typename ...TArgs>
				struct MethodCallDescriptor<TRet(TClass::*)(TArgs...) volatile> {
					using MethodType = TRet(TClass::*)(TArgs...);
					using ArgsType = typename Containers::Tuple<typename TypeTraits::RemoveCVRefV<TArgs>...>;

					using Class = TClass;

					static constexpr Boolean IsConst = false;
					static constexpr Boolean IsVolatile = true;

					static constexpr auto ArgsTypeName = CTTI::TypeName<ArgsType>();
					static constexpr auto RetTypeName = CTTI::TypeName<TypeTraits::RemoveCVRefV<TRet>>();

					static String ArgsMetaData() {
						return Serilization::MetaDataGen<ArgsType>();
					}

					static String RetMetaData() {
						if constexpr (TypeTraits::IsSameV<TRet, void>)
						{
							return String(L"None");
						}
						else
						{
							return Serilization::MetaDataGen<Containers::Tuple<TRet>>();
						}
					}

					static constexpr auto MethodCaller = [] (const void* const& pMethod, DynamicObject& This, DynamicObject& dynamic) -> DynamicObject
					{
						if constexpr (IsFunctionWithoutArgs<TArgs...>)
						{
							MethodType method = reinterpret_cast<MethodType const&>(pMethod);
							TClass& pThis = This.GetValue<TClass>().GetByIdx<0>();

							if constexpr (TypeTraits::IsSameV<TRet, void>)
							{

								if (dynamic.GetImplementationTypeInfo() == typeid(Containers::Tuple<>))
								{
									((&pThis)->*method)();
									return DynamicObject();
								}
								else
								{
									Throw(InvlaidDynamicFunctionArgs(__FILE__, "", __LINE__));
								}
							}
							else
							{
								if (dynamic.GetImplementationTypeInfo() == typeid(Containers::Tuple<>))
								{
									return DynamicObject(Containers::Tuple<TRet>(((&pThis)->*method)()));
								}
								else
								{
									Throw(InvlaidDynamicFunctionArgs(__FILE__, "", __LINE__));
								}
							}
						}
						else
						{
							MethodType method = reinterpret_cast<MethodType const&>(pMethod);
							TClass& pThis = This.GetValue<TClass>().GetByIdx<0>();

							if constexpr (TypeTraits::IsSameV<TRet, void>)
							{

								if (dynamic.GetImplementationTypeInfo() == typeid(ArgsType))
								{
									return dynamic.GetValue<TypeTraits::RemoveCVRefV<TArgs>...>().CallWithArgsFromTuple([&] (TArgs&&... args) -> DynamicObject
									{
										((&pThis)->*method)(ForwardReference<TArgs>(args)...);
										return DynamicObject();
									});
								}
								else
								{
									Throw(InvlaidDynamicFunctionArgs(__FILE__, "", __LINE__));
								}
							}
							else
							{
								if (dynamic.GetImplementationTypeInfo() == typeid(ArgsType))
								{
									return dynamic.GetValue<TypeTraits::RemoveCVRefV<TArgs>...>()
										.CallWithArgsFromTuple([&] (TypeTraits::RemoveCVRefV<TArgs>&&... args)
									{
										return DynamicObject(Containers::Tuple<TypeTraits::RemoveCVRefV<TRet>>(((&pThis)->*method)(args...)));
									});
								}
								else
								{
									Throw(InvlaidDynamicFunctionArgs(__FILE__, "", __LINE__));
								}
							}
						}
					};

					static constexpr auto TypeInformationGetter = [] () -> Containers::List<DataTypeRecord>
					{
						if constexpr (TypeTraits::IsSameV<TRet, void>)
						{
							return Containers::List<DataTypeRecord>{DataTypeRecord(TypeTraits::TypeTag<TypeTraits::RemoveCVRefV<TArgs>>{})...};
						}
						else
						{
							return Containers::List<DataTypeRecord>{
								DataTypeRecord(TypeTraits::TypeTag<TypeTraits::RemoveCVRefV<TRet>>{}), 
								DataTypeRecord(TypeTraits::TypeTag<TypeTraits::RemoveCVRefV<TArgs>>{})...
							};
						}
					};

					static constexpr bool IsArgsSerilizable = IsFunctionWithoutArgs<TArgs...> ||
						(CTTI::TypeTraits::CTTITypeMetaData<TypeTraits::RemoveCVRefV<TArgs>>::IsSerilizable && ...);

					static constexpr bool IsRetSerilizable = TypeTraits::IsSameV<TRet, void> ||
						CTTI::TypeTraits::CTTITypeMetaData<TypeTraits::RemoveCVRefV<TRet>>::IsSerilizable;

					using Ret = TypeTraits::RemoveCVRefV<TRet>;
					using Args = ArgsType;
				};

				template<typename TRet, typename TClass, typename ...TArgs>
				struct MethodCallDescriptor<TRet(TClass::*)(TArgs...) const volatile> {
					using MethodType = TRet(TClass::*)(TArgs...);
					using ArgsType = typename Containers::Tuple<typename TypeTraits::RemoveCVRefV<TArgs>...>;

					using Class = TClass;

					static constexpr Boolean IsConst = true;
					static constexpr Boolean IsVolatile = true;

					static constexpr auto ArgsTypeName = CTTI::TypeName<ArgsType>();
					static constexpr auto RetTypeName = CTTI::TypeName<TypeTraits::RemoveCVRefV<TRet>>();

					static String ArgsMetaData() {
						return Serilization::MetaDataGen<ArgsType>();
					}

					static String RetMetaData() {
						if constexpr (TypeTraits::IsSameV<TRet, void>)
						{
							return String(L"None");
						}
						else
						{
							return Serilization::MetaDataGen<Containers::Tuple<TRet>>();
						}
					}

					static constexpr auto MethodCaller = [] (const void* const& pMethod, DynamicObject& This, DynamicObject& dynamic) -> DynamicObject
					{
						if constexpr (IsFunctionWithoutArgs<TArgs...>)
						{
							MethodType method = reinterpret_cast<MethodType const&>(pMethod);
							TClass& pThis = This.GetValue<TClass>().GetByIdx<0>();

							if constexpr (TypeTraits::IsSameV<TRet, void>)
							{

								if (dynamic.GetImplementationTypeInfo() == typeid(Containers::Tuple<>))
								{
									((&pThis)->*method)();
									return DynamicObject();
								}
								else
								{
									Throw(InvlaidDynamicFunctionArgs(__FILE__, "", __LINE__));
								}
							}
							else
							{
								if (dynamic.GetImplementationTypeInfo() == typeid(Containers::Tuple<>))
								{
									return DynamicObject(Containers::Tuple<TRet>(((&pThis)->*method)()));
								}
								else
								{
									Throw(InvlaidDynamicFunctionArgs(__FILE__, "", __LINE__));
								}
							}
						}
						else
						{
							MethodType method = reinterpret_cast<MethodType const&>(pMethod);
							TClass& pThis = This.GetValue<TClass>().GetByIdx<0>();

							if constexpr (TypeTraits::IsSameV<TRet, void>)
							{

								if (dynamic.GetImplementationTypeInfo() == typeid(ArgsType))
								{
									return dynamic.GetValue<TypeTraits::RemoveCVRefV<TArgs>...>().CallWithArgsFromTuple([&] (TArgs&&... args) -> DynamicObject
									{
										((&pThis)->*method)(ForwardReference<TArgs>(args)...);
										return DynamicObject();
									});
								}
								else
								{
									Throw(InvlaidDynamicFunctionArgs(__FILE__, "", __LINE__));
								}
							}
							else
							{
								if (dynamic.GetImplementationTypeInfo() == typeid(ArgsType))
								{
									return dynamic.GetValue<TypeTraits::RemoveCVRefV<TArgs>...>()
										.CallWithArgsFromTuple([&] (TypeTraits::RemoveCVRefV<TArgs>&&... args)
									{
										return DynamicObject(Containers::Tuple<TypeTraits::RemoveCVRefV<TRet>>(((&pThis)->*method)(args...)));
									});
								}
								else
								{
									Throw(InvlaidDynamicFunctionArgs(__FILE__, "", __LINE__));
								}
							}
						}
					};

					static constexpr auto TypeInformationGetter = [] () -> Containers::List<DataTypeRecord>
					{
						if constexpr (TypeTraits::IsSameV<TRet, void>)
						{
							return Containers::List<DataTypeRecord>{DataTypeRecord(TypeTraits::TypeTag<TypeTraits::RemoveCVRefV<TArgs>>{})...};
						}
						else
						{
							return Containers::List<DataTypeRecord>{
								DataTypeRecord(TypeTraits::TypeTag<TypeTraits::RemoveCVRefV<TRet>>{}), 
								DataTypeRecord(TypeTraits::TypeTag<TypeTraits::RemoveCVRefV<TArgs>>{})...
							};
						}
					};

					static constexpr bool IsArgsSerilizable = IsFunctionWithoutArgs<TArgs...> ||
						(CTTI::TypeTraits::CTTITypeMetaData<TypeTraits::RemoveCVRefV<TArgs>>::IsSerilizable && ...);

					static constexpr bool IsRetSerilizable = TypeTraits::IsSameV<TRet, void> ||
						CTTI::TypeTraits::CTTITypeMetaData<TypeTraits::RemoveCVRefV<TRet>>::IsSerilizable;

					using Ret = TypeTraits::RemoveCVRefV<TRet>;
					using Args = ArgsType;
				};

				template<typename T, T pMethodType, typename TName>
				struct MethodDescriptor {
					static_assert(IsCTTIStringV<TName>, "TName must be CTTIString");

					static constexpr auto Name = Seed::Core::CTTI::Details::TypeStringToArray(TName());
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

					static constexpr auto TypeInformationGetter = MethodCallDescriptor<T>::TypeInformationGetter;

					static constexpr bool IsArgsSerilizable = MethodCallDescriptor<T>::IsArgsSerilizable;

					static constexpr bool IsRetSerilizable = MethodCallDescriptor<T>::IsRetSerilizable;

					using Ret = typename MethodCallDescriptor<T>::Ret;
					using Args = typename MethodCallDescriptor<T>::Args;
				};

				template<typename T, typename ... TArgs>
				struct ObjectCtorAndDtor {
					static_assert(Core::TypeTraits::IsConstructible<T, TArgs...>, "Can not construct class with this args");

					static constexpr auto UnifiedCtor = [] (DynamicObject&& args) -> DynamicObject
					{
						if constexpr (IsFunctionWithoutArgs<TArgs...>)
						{
							if (args.GetImplementationTypeInfo() == typeid(Containers::Tuple<>))
							{
								return DynamicObject(Containers::Tuple<T>());
							}
							else
							{
								Throw(InvlaidDynamicFunctionArgs(__FILE__, "", __LINE__));
							}
						}
						else
						{
							if (args.GetImplementationTypeInfo() == typeid(Containers::Tuple<TArgs...>))
							{
								return args.GetValue<TArgs...>().CallWithArgsFromTuple([] (TArgs&&... args)
								{
									return DynamicObject(Containers::Tuple<T>(T(args...)));
								});
							}
							else
							{
								Throw(InvlaidDynamicFunctionArgs(__FILE__, "", __LINE__));
							}
						}
					};

					static constexpr auto UnifiedDtor = [] (DynamicObject&& object)
					{
					};

					static constexpr auto CtorArgsTypeName = CTTI::TypeName<Containers::Tuple<TArgs...>>();

					static String ArgsMetaData() {
						return Serilization::MetaDataGen<Containers::Tuple<TArgs...>>();
					}
				};

				template<typename T>
				struct ObjectNullCtorAndDtor {
					static constexpr auto UnifiedCtor = [] (DynamicObject&& args) -> DynamicObject
					{
						Throw(InvlaidDynamicFunctionArgs(__FILE__, "", __LINE__));
					};

					static constexpr auto UnifiedDtor = [] (DynamicObject&& object)
					{
					};

					static constexpr auto CtorArgsTypeName = Seed::Core::CTTI::Details::TypeStringToArray(GenNGTypeString("`invalid`")());

					static String ArgsMetaData() {
						return Seed::Core::CTTI::Details::TypeStringToArray(GenNGTypeString("`Can not construct`")());
					}
				};

				template<typename TName, typename TGet, TGet GetT, typename TSetterMethods>
				struct PropertyDescriptor {
				private:
					template<SizeT I = 0>
					static constexpr bool CheckSetterMethods() {
						if constexpr (TSetterMethods::Count == I)
						{
							return true;
						}
						else
						{
							return (
								TypeTraits::IsSameT<typename TSetterMethods::ItemType<I>::Ret, void>::State &&
									TSetterMethods::ItemType<0>::Args::Count == 1 &&
									TypeTraits::IsSameV<
										typename TypeTraits::RemoveCV<
											TypeTraits::RemoveRefV<
												typename TSetterMethods::ItemType<0>::Args::ItemType<0>
											>
										>, 
										typename MethodCallDescriptor<TGet>::Ret
									>
								) &&
								CheckSetterMethods<I + 1>();
						}
					}
				public:
					using PropType = typename MethodCallDescriptor<TGet>::Ret;
					using PropClass = typename MethodCallDescriptor<TGet>::Class;
					static constexpr auto Name = Seed::Core::CTTI::Details::TypeStringToArray(TName());
					static constexpr auto PropTypeName = CTTI::TypeName<PropType>();
					static constexpr auto GetCaller = MethodCallDescriptor<TGet>::MethodCaller;
					static constexpr auto pGet = GetT;
					static constexpr auto PropTypeInformationGetter = MethodCallDescriptor<TGet>::TypeInformationGetter;
					static constexpr bool IsRw = (TSetterMethods::Count != 0);
					static constexpr bool IsValueSerilizable = MethodCallDescriptor<TGet>::IsRetSerilizable;
					static constexpr auto SerilizeFunction = [] (const DynamicObject& data) -> Containers::Blob
					{
						if constexpr (IsValueSerilizable)
						{
							//TODO: temp solution for compiling. Think how pass TypeRegistry
							auto registry = Core::Serilization::TypesRegistry();

							auto pObjGet = pGet;
							auto emptyArgs = DynamicObject(Containers::Tuple<>());
							//HACK: we know that getter use const object but for unification method caller lambda use object without 
							//HACK: const modificator and we should remove it to get property value
							auto propValue = GetCaller(reinterpret_cast<void*&>(pObjGet), const_cast<DynamicObject&>(data), emptyArgs)
								.GetValue<PropType>()
								.GetByIdx<0>();
							return Serilization::Serilize(propValue, registry);
						}
						else
						{
							Throw(NonSerelizableType(__FILE__, "", __LINE__, PropTypeName));
						}
					};
					static constexpr auto SerilizeTypeMetaData = [] () -> String
					{
						if constexpr (IsValueSerilizable)
						{
							return Serilization::MetaDataGen<PropType>();
						}
						else
						{
							return String();
						}
					};

					using SetterMethods = TSetterMethods;

					static_assert(MethodCallDescriptor<TGet>::IsConst, "Property getter should be const method");
					static_assert(CheckSetterMethods(), "Invalid setters for property");
				};

				template<typename TObj, typename TObjectCtorAndDtor, typename TMethodsTuple, typename TPropertiesTuple>
				struct ObjectDescriptor {
					static constexpr auto Name = CTTI::TypeName<TObj>();

					static constexpr auto RttiTypeInfo = [] () 
					{
						return TypeRecord<TObj>();
					};
						
					static constexpr auto UnifiedCtor = TObjectCtorAndDtor::UnifiedCtor;
					static constexpr auto UnifiedDtor = TObjectCtorAndDtor::UnifiedDtor;

					using Methods = TMethodsTuple;
					using Properties = TPropertiesTuple;

					static constexpr auto CtorArgsTypeName = TObjectCtorAndDtor::CtorArgsTypeName;

					static String ArgsMetaData() {
						return TObjectCtorAndDtor::ArgsMetaData();
					}

				private:
					template<SizeT TId = 0>
					static constexpr bool IsAllMethodsHaveCorrectClassImpl() {
						if constexpr (TId == TMethodsTuple::Count)
						{
							return true;
						}
						else
						{
							return
								TypeTraits::IsSameT<
								typename Methods::ItemType<TId>::MethodClass,
								TObj>::State
								&&
								IsAllMethodsHaveCorrectClassImpl<TId + 1>();
						}
					}
				public:
					static_assert(IsAllMethodsHaveCorrectClassImpl(), "Invalid method passed");
				};
			}

			class ObjectRTTIRecord final : IDisposable, public Serilization::ISerilizable<ObjectRTTIRecord> {
			public:
				using CtorCaller = DynamicObject (*)(DynamicObject&&);
				using DtorCaller = void (*)(DynamicObject&&);
				using MethodCaller = DynamicObject(*)(const void* const&, DynamicObject&, DynamicObject&);
				using TypeInformationGetterType = Containers::List<DataTypeRecord>(*)();
			public:
				class MethodRTTIRecord final : IDisposable, public Serilization::ISerilizable<MethodRTTIRecord> {
				private:
					struct Data {
						String MethodName;
						MethodCaller methodCaller;
						const void* const& pMethod;
						String MethodArgs;
						String MethodRet;
						String MethodArgsMetaData;
						String MethodRetMetaData;

						TypeInformationGetterType typeInformationGetter;
						Boolean IsRetSerilizable;
						Boolean IsArgsSerilizable;

						Boolean IsConstMethod;
						Boolean IsVolatileMethod;

						RefCountT RefCount;

						ForceInline Data(
							String&& methodName,
							MethodCaller methodCaller,
							const void* const& pMethod,
							String&& methodArgs,
							String&& methodRet,
							String&& methodArgsMetaData,
							String&& methodRetMetaData,
							TypeInformationGetterType typeInformationGetter,
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
							typeInformationGetter(typeInformationGetter),
							IsRetSerilizable(IsRetSerilizable),
							IsArgsSerilizable(IsArgsSerilizable),
							IsConstMethod(isConstMethod),
							IsVolatileMethod(isVolatileMethod),
							RefCount(1) {}
					};
				private:
					Data* data;
				protected:
					ForceInline virtual void Dispose() override {}
					ForceInline virtual Byte* GetDataPtr() const override {
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
					ForceInline MethodRTTIRecord(TypeTraits::TypeTag<TMethodDescriptor>)
						: data(new Data(
							TMethodDescriptor::Name,
							TMethodDescriptor::MethodCaller,
							reinterpret_cast<const void* const&>(TMethodDescriptor::pMethod),
							TMethodDescriptor::ArgsTypeName,
							TMethodDescriptor::RetTypeName,
							TMethodDescriptor::ArgsMetaData(),
							TMethodDescriptor::RetMetaData(),
							TMethodDescriptor::TypeInformationGetter,
							TMethodDescriptor::IsRetSerilizable,
							TMethodDescriptor::IsArgsSerilizable,
							TMethodDescriptor::IsConst,
							TMethodDescriptor::IsVolatile
						)) {}

					ForceInline const String& GetName() const {
						return this->data->MethodName;
					}

					ForceInline MethodCaller GetMethodCaller() const {
						return this->data->methodCaller;
					}

					ForceInline const String& GetArgsTypeName() const {
						return this->data->MethodArgs;
					}

					ForceInline const String& GetRetTypeName() const {
						return this->data->MethodRet;
					}

					ForceInline const String& GetArgsMetaData() const {
						return this->data->MethodArgsMetaData;
					}

					ForceInline const String& GetRetMetaData() const {
						return this->data->MethodRetMetaData;
					}

					ForceInline TypeInformationGetterType GetTypeInformationGetter() const {
						return this->data->typeInformationGetter;
					}

					ForceInline bool GetIsRetSerilizable() const {
						return this->data->IsRetSerilizable;
					}

					ForceInline bool GetIsArgsSerilizable() const {
						return this->data->IsArgsSerilizable;
					}

					ForceInline bool GetIsConst() const {
						return this->data->IsConstMethod;
					}

					ForceInline bool GetIsVolatile() const {
						return this->data->IsVolatileMethod;
					}

					ForceInline const void* const& GetMethodPointer() const {
						return this->data->pMethod;
					}

					String Property(GetName) Name;
					MethodCaller Property(GetMethodCaller) MethodCallerFunction;
					String Property(GetArgsTypeName) ArgsTypeName;
					String Property(GetRetTypeName) RetTypeName;
					String Property(GetArgsMetaData) ArgsMetaData;
					String Property(GetRetMetaData) RetMetaData;
					TypeInformationGetterType Property(GetTypeInformationGetter) TypeInformationGetterFunction;
					bool Property(GetIsRetSerilizable) IsRetSerilizable;
					bool Property(GetIsArgsSerilizable) IsArgsSerilizable;
					bool Property(GetIsConst) IsConst;
					bool Property(GetIsVolatile) IsVolatile;


					RefCountClass(MethodRTTIRecord, data);
				};

				class PropertyRTTIRecord final : IDisposable, public Serilization::ISerilizable<MethodRTTIRecord> {
				public:
					using TSerilizationFunction = Containers::Blob(*)(const DynamicObject&);
					using TSerilizationMetaDataGetter = String(*)();

					struct CallerData {
						MethodCaller Setter;
						const void* const& pSetter;

						ForceInline CallerData(MethodCaller Setter, const void* const& pSetter) : Setter(Setter), pSetter(pSetter) {}
						ForceInline CallerData(const CallerData& callerData) : Setter(callerData.Setter), pSetter(callerData.pSetter) {}
						ForceInline CallerData(CallerData&& callerData) : Setter(callerData.Setter), pSetter(callerData.pSetter) {}
						ForceInline CallerData& operator=(const CallerData& callerData) {
							this->~CallerData();
							this->CallerData::CallerData(callerData);
							return *this;
						}
						ForceInline CallerData& operator=(CallerData&& callerData) {
							this->~CallerData();
							this->CallerData::CallerData(MoveRValue(callerData));
							return *this;
						}
						ForceInline ~CallerData() {}
					};
				private:
					struct Data {
						String Name;
						String PropTypeName;
						MethodCaller Getter;
						Containers::List<CallerData> Setters;
						Boolean IsRW;
						const void* const& pGetter;
						TypeInformationGetterType typeInformationGetter;
						Boolean IsValueSerilizable;
						TSerilizationFunction serilizationFunction;
						TSerilizationMetaDataGetter serilizationMetaDataGetter;

						RefCountT RefCount;

						ForceInline Data(
							String Name,
							String PropTypeName,
							MethodCaller Getter,
							Containers::List<CallerData> Setters,
							Boolean IsRW,
							const void* const& pGetter,
							TypeInformationGetterType typeInformationGetter,
							Boolean IsValueSerilizable,
							TSerilizationFunction serilizationFunction,
							TSerilizationMetaDataGetter serilizationMetaDataGetter
						) :
							Name(Name),
							PropTypeName(PropTypeName),
							Getter(Getter),
							Setters(Setters),
							IsRW(IsRW),
							pGetter(pGetter),
							typeInformationGetter(typeInformationGetter),
							IsValueSerilizable(IsValueSerilizable),
							serilizationFunction(serilizationFunction),
							serilizationMetaDataGetter(serilizationMetaDataGetter),
							RefCount(1) {}
					};
				private:
					template<typename TMethods, SizeT I = 0>
					ForceInline static Containers::List<CallerData> GenSettersData(TypeTraits::TypeTag<TMethods> tag) {
						if constexpr (TMethods::Count == I)
						{
							return {};
						}
						else
						{
							return MoveRValue(
								Containers::List<CallerData>{
									CallerData{
										TMethods::ItemType<I>::MethodCaller, reinterpret_cast<const void* const&>(TMethods::ItemType<I>::pMethod)
									}
								} 
								+= GenSettersData<TMethods, I + 1>(tag)
							);
						}
					}
				private:
					Data* data;
				protected:
					ForceInline virtual void Dispose() override {}
					ForceInline virtual Byte* GetDataPtr() const override {
						return reinterpret_cast<Byte*>(data);
					}
				public:
					DefStructMetaData(Data,
						DefStructFieldData(Data, Name),
						DefStructFieldData(Data, IsRW),
						DefStructFieldData(Data, PropTypeName),
						DefStructFieldData(Data, IsValueSerilizable)
					);
				public:
					template<typename TPropertyDescriptor>
					ForceInline PropertyRTTIRecord(TypeTraits::TypeTag<TPropertyDescriptor>) :
						data(new Data(
							TPropertyDescriptor::Name,
							TPropertyDescriptor::PropTypeName,
							TPropertyDescriptor::GetCaller,
							GenSettersData<TPropertyDescriptor::SetterMethods, 0>(TypeTraits::TypeTag<TPropertyDescriptor::SetterMethods>{}),
							TPropertyDescriptor::IsRw,
							reinterpret_cast<const void* const&>(TPropertyDescriptor::pGet),
							TPropertyDescriptor::PropTypeInformationGetter,
							TPropertyDescriptor::IsValueSerilizable,
							TPropertyDescriptor::SerilizeFunction,
							TPropertyDescriptor::SerilizeTypeMetaData
						)) {}
				public:
					ForceInline String GetName() const {
						return data->Name;
					}

					ForceInline String GetPropTypeName() const {
						return data->PropTypeName;
					}

					ForceInline MethodCaller GetGetter() const {
						return data->Getter;
					}

					ForceInline const Containers::List<CallerData>& GetSetters() const {
						return data->Setters;
					}

					ForceInline Boolean GetIsRW() const {
						return data->IsRW;
					}

					ForceInline const void* const& GetGetterAddr() const {
						return data->pGetter;
					}

					ForceInline TypeInformationGetterType GetTypeInformationGetter() const {
						return data->typeInformationGetter;
					}

					ForceInline DynamicObject CallGetterForObject(DynamicObject& obj) const {
						auto args = DynamicObject(Containers::Tuple<>());
						return data->Getter(data->pGetter, obj, args);
					}

					ForceInline Boolean GetIsSerilizable() const {
						return data->IsValueSerilizable;
					}

					ForceInline TSerilizationFunction GetSerilizableFunction() const {
						return data->serilizationFunction;
					}

					ForceInline TSerilizationMetaDataGetter GetSerilizableMetaDataGetter() const {
						return data->serilizationMetaDataGetter;
					}

					String Property(GetName) Name;
					String Property(GetPropTypeName) PropertieTypeName;
					MethodCaller Property(GetGetter) Getter;
					Containers::List<CallerData> Property(GetSetters) Setters;
					Boolean Property(GetIsRW) IsRW;
					TypeInformationGetterType Property(GetTypeInformationGetter) TypeInformationGetter;
					Boolean Property(GetIsSerilizable) IsSerilizable;
					TSerilizationFunction Property(GetSerilizableFunction) SerilizationFunction;
					TSerilizationMetaDataGetter Property(GetSerilizableMetaDataGetter) SerilizableMetaDataGetter;
				public:
					RefCountClass(PropertyRTTIRecord, data);
				};
			private:
				struct Data {
					String ClassName;
					String CtorArgs;
					String CtorMetaDate;
					CtorCaller ctorCaller;
					DtorCaller dtorCaller;
					RTTI::RTTITypeRecord RttiTypeInfo;
					Containers::List<MethodRTTIRecord> Methods;
					Containers::List<PropertyRTTIRecord> Properties;
					RefCountT RefCount;

					ForceInline Data(
						String&& className,
						String&& ctorArgs,
						String&& ctorMetaDate,
						CtorCaller ctorCaller,
						DtorCaller dtorCaller,
						RTTI::RTTITypeRecord RttiTypeInfo,
						Containers::List<MethodRTTIRecord>&& methods,
						Containers::List<PropertyRTTIRecord>&& properties
					) :
						ClassName(MoveRValue(className)),
						CtorArgs(MoveRValue(ctorArgs)),
						CtorMetaDate(MoveRValue(ctorMetaDate)),
						ctorCaller(ctorCaller),
						dtorCaller(dtorCaller),
						RttiTypeInfo(RttiTypeInfo),
						Methods(MoveRValue(methods)),
						Properties(MoveRValue(properties)),
						RefCount(1) {}

				};
			private:
				Data* data;
			private:
				template<SizeT TId, typename ... TMethod>
				ForceInline static Containers::List<MethodRTTIRecord> GenerateRTTIMethodInfo(TypeTraits::TypeTag<Containers::Tuple<TMethod...>>) {
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

				template<SizeT TId, typename ... TProperties>
				ForceInline static Containers::List<PropertyRTTIRecord> GenerateRTTIPropertieInfo(TypeTraits::TypeTag<Containers::Tuple<TProperties...>>) {
					if constexpr (TId == sizeof...(TProperties))
					{
						return Containers::List<PropertyRTTIRecord>();
					}
					else
					{
						return Containers::List<PropertyRTTIRecord>(MoveRValue(
							(
								Containers::List<PropertyRTTIRecord>({
									PropertyRTTIRecord(TypeTraits::TypeTag<typename Containers::Tuple<TProperties...>::ItemType<TId>>{})
								}) += 
									MoveRValue(GenerateRTTIPropertieInfo<TId + 1>(TypeTraits::TypeTag<Containers::Tuple<TProperties...>>{})
								)
							)
						));
					}
				}
			protected:
				ForceInline virtual void Dispose() override {}
				ForceInline virtual Byte* GetDataPtr() const override {
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
				ForceInline ObjectRTTIRecord(TypeTraits::TypeTag<TObjectDescriptor>) :
					data(new Data(
						TObjectDescriptor::Name,
						TObjectDescriptor::CtorArgsTypeName,
						TObjectDescriptor::ArgsMetaData(),
						TObjectDescriptor::UnifiedCtor,
						TObjectDescriptor::UnifiedDtor,
						TObjectDescriptor::RttiTypeInfo(),
						GenerateRTTIMethodInfo<0>(TypeTraits::TypeTag<TObjectDescriptor::Methods>{}),
						GenerateRTTIPropertieInfo<0>(TypeTraits::TypeTag<TObjectDescriptor::Properties>{})
					)) {};

				ForceInline const String& GetName() const {
					return this->data->ClassName;
				}

				ForceInline const String& GetCtorArgs() const {
					return this->data->CtorArgs;
				}

				ForceInline const String& GetArgsMetaData() const {
					return this->data->CtorMetaDate;
				}

				ForceInline CtorCaller GetCtorCaller() const {
					return this->data->ctorCaller;
				}

				ForceInline DtorCaller GetDtorCaller() const {
					return this->data->dtorCaller;
				}

				ForceInline const Containers::List<MethodRTTIRecord>& GetAllMethods() const {
					return this->data->Methods;
				}

				ForceInline const Containers::List<PropertyRTTIRecord>& GetAllProperties() const {
					return this->data->Properties;
				}

				ForceInline RTTI::RTTITypeRecord GetRTTITypeInfo() const {
					return this->data->RttiTypeInfo;
				}

				String Property(GetName) ClassName;
				String Property(GetCtorArgs) CtorArgs;
				String Property(GetArgsMetaData) ArgsMetaData;
				CtorCaller Property(GetCtorCaller) CtorCallerFunction;
				DtorCaller Property(GetDtorCaller) DtorCallerFunction;
				Containers::List<MethodRTTIRecord> Property(GetAllMethods) Methods;
				Containers::List<PropertyRTTIRecord> Property(GetAllProperties) Properties;

				RefCountClass(ObjectRTTIRecord, data);
			};

		}
	}
}
#define ObjectMethodsDefs(...) Seed::Core::Containers::Tuple<__VA_ARGS__>
#define ObjectPropsDefs(...) Seed::Core::Containers::Tuple<__VA_ARGS__>
#define MethodDescription(Class, Method) Seed::Core::Reflection::Details::MethodDescriptor<decltype(&Class::Method), &Class::Method, GenNGTypeString(#Method)>
#define MethodDescriptionWithArgs(Class, Method, RetType, ...) Seed::Core::Reflection::Details::MethodDescriptor<RetType(Class::*)(__VA_ARGS__), &Class::Method, GenNGTypeString(#Method)>
#define	ObjectProperty(Name, Getter, ...) Seed::Core::Reflection::Details::PropertyDescriptor<GenNGTypeString(#Name), decltype(&Getter), &Getter, Seed::Core::Containers::Tuple<__VA_ARGS__>>
#define CtorDescription(Class, ...) Seed::Core::Reflection::Details::ObjectCtorAndDtor<Class, __VA_ARGS__>
#define NullCtorDescription(Class) Seed::Core::Reflection::Details::ObjectNullCtorAndDtor<Class>
#define ObjectDefinition(Class, CtorDesc, Methods, Props) Seed::Core::Reflection::Details::ObjectDescriptor<Class, CtorDesc, Methods, Props>
#define GenObjectRTTIRecord(ObjectTypeRecord) Seed::Core::Reflection::ObjectRTTIRecord(Seed::Core::TypeTraits::TypeTag<ObjectTypeRecord>{})