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
				constexpr bool IsFunctionWithoutArgs = sizeof...(TArgs) == 0;

				template<typename, typename>
				struct MethodCallDescriptor {
				};

				template<typename TTargetClass, typename TRet, typename TClass, typename ...TArgs>
				struct MethodCallDescriptor<TTargetClass, TRet(TClass::*)(TArgs...)> {
					using MethodType = TRet(TClass::*)(TArgs...);
					using ArgsType = typename Containers::Tuple<typename TypeTraits::RemoveCVRefV<TArgs>...>;

					using Class = TTargetClass;

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
				private:
					template<SizeT TIdx>
					ForceInline static auto GetArgByIdxFromDelegate(const Containers::Array<DynamicObject>& args) {
						return args[(ArgsType::Count - 1) - TIdx].GetValue<typename ArgsType::ItemType<(ArgsType::Count - 1) - TIdx>>().GetByIdx<0>();
					}

					template<typename T>
					struct GetArgsFromArrayOfDynamicsImpl;

					template<SizeT... TIdxs>
					struct GetArgsFromArrayOfDynamicsImpl<TypeTraits::Indexator<TIdxs...>> {
						ForceInline static auto GetArgsFromArrayOfDynamics(const Containers::Array<DynamicObject>& args) {
							return Containers::Tuple<decltype(GetArgByIdxFromDelegate<TIdxs>(args))...>(GetArgByIdxFromDelegate<TIdxs>(args) ... );
						}
					};

					ForceInline static auto GetArgsFromArrayOfDynamics(const Containers::Array<DynamicObject>& args) {
						return GetArgsFromArrayOfDynamicsImpl<typename TypeTraits::Indexer<ArgsType::Count>::Type>::GetArgsFromArrayOfDynamics(args);
					}
				public:
					static constexpr auto MethodCaller = [] (const void* const& pMethod, DynamicObject& This, const Containers::Array<DynamicObject>& args) -> DynamicObject
					{
						if constexpr (IsFunctionWithoutArgs<TArgs...>)
						{
							MethodType method = reinterpret_cast<MethodType const&>(const_cast<void*&>(pMethod));
							TTargetClass& pThis = This.GetValue<TTargetClass>().GetByIdx<0>();

							if constexpr (TypeTraits::IsSameV<TRet, void>)
							{

								if (args.Count == 0)
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
								if (args.Count == 0)
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
							TTargetClass& pThis = This.GetValue<TTargetClass>().GetByIdx<0>();

							if constexpr (TypeTraits::IsSameV<TRet, void>)
							{
								if (args.Count == ArgsType::Count)
								{
									return GetArgsFromArrayOfDynamics(args).CallWithArgsFromTuple([&] (TypeTraits::RemoveCVRefV<TArgs>&&... args) -> DynamicObject
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
								if (args.Count == ArgsType::Count)
								{
									return GetArgsFromArrayOfDynamics(args).CallWithArgsFromTuple([&](TypeTraits::RemoveCVRefV<TArgs>&&... args) -> DynamicObject
									{
										return DynamicObject(Containers::Tuple<TypeTraits::RemoveCVRefV<TRet>>(((&pThis)->*method)(ForwardReference<TArgs>(args)...)));
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
							return Containers::List<DataTypeRecord>{ DataTypeRecord(TypeTraits::TypeTag<TypeTraits::RemoveCVRefV<TArgs>>{}) ... };
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

				template<typename TTargetClass, typename TRet, typename TClass, typename ...TArgs>
				struct MethodCallDescriptor<TTargetClass, TRet(TClass::*)(TArgs...) const> {
					using MethodType = TRet(TClass::*)(TArgs...) const;
					using ArgsType = typename Containers::Tuple<typename TypeTraits::RemoveCVRefV<TArgs>...>;

					using Class = TTargetClass;

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

				private:
					template<SizeT TIdx>
					ForceInline static auto GetArgByIdxFromDelegate(const Containers::Array<DynamicObject>& args) {
						return args[(ArgsType::Count - 1) - TIdx].GetValue<typename ArgsType::ItemType<(ArgsType::Count - 1) - TIdx>>().GetByIdx<0>();
					}

					template<typename T>
					struct GetArgsFromArrayOfDynamicsImpl;

					template<SizeT... TIdxs>
					struct GetArgsFromArrayOfDynamicsImpl<TypeTraits::Indexator<TIdxs...>> {
						ForceInline static auto GetArgsFromArrayOfDynamics(const Containers::Array<DynamicObject>& args) {
							return Containers::Tuple<decltype(GetArgByIdxFromDelegate<TIdxs>(args))...>(GetArgByIdxFromDelegate<TIdxs>(args) ...);
						}
					};

					ForceInline static auto GetArgsFromArrayOfDynamics(const Containers::Array<DynamicObject>& args) {
						return GetArgsFromArrayOfDynamicsImpl<typename TypeTraits::Indexer<ArgsType::Count>::Type>::GetArgsFromArrayOfDynamics(args);
					}
				public:
					static constexpr auto MethodCaller = [](const void* const& pMethod, DynamicObject& This, const Containers::Array<DynamicObject>& args) -> DynamicObject
					{
						if constexpr (IsFunctionWithoutArgs<TArgs...>)
						{
							MethodType method = reinterpret_cast<MethodType const&>(pMethod);
							TTargetClass& pThis = This.GetValue<TTargetClass>().GetByIdx<0>();

							if constexpr (TypeTraits::IsSameV<TRet, void>)
							{

								if (args.Count == 0)
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
								if (args.Count == 0)
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
							TTargetClass& pThis = This.GetValue<TTargetClass>().GetByIdx<0>();

							if constexpr (TypeTraits::IsSameV<TRet, void>)
							{
								if (args.Count == ArgsType::Count)
								{
									return GetArgsFromArrayOfDynamics(args).CallWithArgsFromTuple([&](TypeTraits::RemoveCVRefV<TArgs>&&... args) -> DynamicObject
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
								if (args.Count == ArgsType::Count)
								{
									return GetArgsFromArrayOfDynamics(args).CallWithArgsFromTuple([&](TypeTraits::RemoveCVRefV<TArgs>&&... args) -> DynamicObject
										{
											return DynamicObject(Containers::Tuple<TypeTraits::RemoveCVRefV<TRet>>(((&pThis)->*method)(ForwardReference<TArgs>(args)...)));
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
								DataTypeRecord(TypeTraits::TypeTag<TypeTraits::RemoveCVRefV<TArgs>>{}) ...
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

				template<typename TTargetClass, typename TRet, typename TClass, typename ...TArgs>
				struct MethodCallDescriptor<TTargetClass, TRet(TClass::*)(TArgs...) volatile> {
					using MethodType = TRet(TClass::*)(TArgs...) volatile;
					using ArgsType = typename Containers::Tuple<typename TypeTraits::RemoveCVRefV<TArgs>...>;

					using Class = TTargetClass;

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

				private:
					template<SizeT TIdx>
					ForceInline static auto GetArgByIdxFromDelegate(const Containers::Array<DynamicObject>& args) {
						return args[(ArgsType::Count - 1) - TIdx].GetValue<typename ArgsType::ItemType<(ArgsType::Count - 1) - TIdx>>().GetByIdx<0>();
					}

					template<typename T>
					struct GetArgsFromArrayOfDynamicsImpl;

					template<SizeT... TIdxs>
					struct GetArgsFromArrayOfDynamicsImpl<TypeTraits::Indexator<TIdxs...>> {
						ForceInline static auto GetArgsFromArrayOfDynamics(const Containers::Array<DynamicObject>& args) {
							return Containers::Tuple<decltype(GetArgByIdxFromDelegate<TIdxs>(args))...>(GetArgByIdxFromDelegate<TIdxs>(args) ...);
						}
					};

					ForceInline static auto GetArgsFromArrayOfDynamics(const Containers::Array<DynamicObject>& args) {
						return GetArgsFromArrayOfDynamicsImpl<typename TypeTraits::Indexer<ArgsType::Count>::Type>::GetArgsFromArrayOfDynamics(args);
					}
				public:
					static constexpr auto MethodCaller = [](const void* const& pMethod, DynamicObject& This, const Containers::Array<DynamicObject>& args) -> DynamicObject
					{
						if constexpr (IsFunctionWithoutArgs<TArgs...>)
						{
							MethodType method = reinterpret_cast<MethodType const&>(pMethod);
							TTargetClass& pThis = This.GetValue<TTargetClass>().GetByIdx<0>();

							if constexpr (TypeTraits::IsSameV<TRet, void>)
							{

								if (args.Count == 0)
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
								if (args.Count == 0)
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
							TTargetClass& pThis = This.GetValue<TTargetClass>().GetByIdx<0>();

							if constexpr (TypeTraits::IsSameV<TRet, void>)
							{
								if (args.Count == ArgsType::Count)
								{
									return GetArgsFromArrayOfDynamics(args).CallWithArgsFromTuple([&](TypeTraits::RemoveCVRefV<TArgs>&&... args) -> DynamicObject
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
								if (args.Count == ArgsType::Count)
								{
									return GetArgsFromArrayOfDynamics(args).CallWithArgsFromTuple([&](TypeTraits::RemoveCVRefV<TArgs>&&... args) -> DynamicObject
										{
											return DynamicObject(Containers::Tuple<TypeTraits::RemoveCVRefV<TRet>>(((&pThis)->*method)(ForwardReference<TArgs>(args)...)));
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

				template<typename TTargetClass, typename TRet, typename TClass, typename ...TArgs>
				struct MethodCallDescriptor<TTargetClass, TRet(TClass::*)(TArgs...) const volatile> {
					using MethodType = TRet(TClass::*)(TArgs...) const volatile;
					using ArgsType = typename Containers::Tuple<typename TypeTraits::RemoveCVRefV<TArgs>...>;

					using Class = TTargetClass;

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

				private:
					template<SizeT TIdx>
					ForceInline static auto GetArgByIdxFromDelegate(const Containers::Array<DynamicObject>& args) {
						return args[(ArgsType::Count - 1) - TIdx].GetValue<typename ArgsType::ItemType<(ArgsType::Count - 1) - TIdx>>().GetByIdx<0>();
					}

					template<typename T>
					struct GetArgsFromArrayOfDynamicsImpl;

					template<SizeT... TIdxs>
					struct GetArgsFromArrayOfDynamicsImpl<TypeTraits::Indexator<TIdxs...>> {
						ForceInline static auto GetArgsFromArrayOfDynamics(const Containers::Array<DynamicObject>& args) {
							return Containers::Tuple<decltype(GetArgByIdxFromDelegate<TIdxs>(args))...>(GetArgByIdxFromDelegate<TIdxs>(args) ...);
						}
					};

					ForceInline static auto GetArgsFromArrayOfDynamics(const Containers::Array<DynamicObject>& args) {
						return GetArgsFromArrayOfDynamicsImpl<typename TypeTraits::Indexer<ArgsType::Count>::Type>::GetArgsFromArrayOfDynamics(args);
					}
				public:
					static constexpr auto MethodCaller = [](const void* const& pMethod, DynamicObject& This, const Containers::Array<DynamicObject>& args) -> DynamicObject
					{
						if constexpr (IsFunctionWithoutArgs<TArgs...>)
						{
							MethodType method = reinterpret_cast<MethodType const&>(pMethod);
							TTargetClass& pThis = This.GetValue<TTargetClass>().GetByIdx<0>();

							if constexpr (TypeTraits::IsSameV<TRet, void>)
							{

								if (args.Count == 0)
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
								if (args.Count == 0)
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
							TTargetClass& pThis = This.GetValue<TTargetClass>().GetByIdx<0>();

							if constexpr (TypeTraits::IsSameV<TRet, void>)
							{
								if (args.Count == ArgsType::Count)
								{
									return GetArgsFromArrayOfDynamics(args).CallWithArgsFromTuple([&](TypeTraits::RemoveCVRefV<TArgs>&&... args) -> DynamicObject
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
								if (args.Count == ArgsType::Count)
								{
									return GetArgsFromArrayOfDynamics(args).CallWithArgsFromTuple([&](TypeTraits::RemoveCVRefV<TArgs>&&... args) -> DynamicObject
										{
											return DynamicObject(Containers::Tuple<TypeTraits::RemoveCVRefV<TRet>>(((&pThis)->*method)(ForwardReference<TArgs>(args)...)));
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

				template<typename T, typename ... TArgs>
				struct ObjectCtorAndDtor {
					static_assert(Core::TypeTraits::IsConstructible<T, TArgs...>, "Can not construct class with this args");

				private:
					template<SizeT TIdx>
					ForceInline static auto GetArgByIdxFromDelegate(const Containers::Array<DynamicObject>& args) {
						return args[(Containers::Tuple<TArgs...>::Count - 1) - TIdx].GetValue<typename Containers::Tuple<TArgs...>::ItemType<(Containers::Tuple<TArgs...>::Count - 1) - TIdx>>().GetByIdx<0>();
					}

					template<typename T>
					struct GetArgsFromArrayOfDynamicsImpl;

					template<SizeT... TIdxs>
					struct GetArgsFromArrayOfDynamicsImpl<TypeTraits::Indexator<TIdxs...>> {
						ForceInline static auto GetArgsFromArrayOfDynamics(const Containers::Array<DynamicObject>& args) {
							return Containers::Tuple<decltype(GetArgByIdxFromDelegate<TIdxs>(args))...>(GetArgByIdxFromDelegate<TIdxs>(args) ...);
						}
					};

					ForceInline static auto GetArgsFromArrayOfDynamics(const Containers::Array<DynamicObject>& args) {
						return GetArgsFromArrayOfDynamicsImpl<typename TypeTraits::Indexer<Containers::Tuple<TArgs...>::Count>::Type>::GetArgsFromArrayOfDynamics(args);
					}
				public:

					static constexpr auto UnifiedCtor = [] (const Containers::Array<DynamicObject>& args) -> DynamicObject
					{
						if constexpr (IsFunctionWithoutArgs<TArgs...>)
						{
							if (args.Count == 0)
							{
								return DynamicObject(Containers::Tuple<T>());
							}
							else
							{
								Throw(InvlaidDynamicFunctionArgs(__FILE__, "UnifiedCtor", __LINE__));
							}
						}
						else
						{
							if (args.Count == Containers::Tuple<TArgs...>::Count)
							{
								return GetArgsFromArrayOfDynamics(args).CallWithArgsFromTuple([] (TArgs&&... args)
								{
									return DynamicObject(Containers::Tuple<T>(T(args...)));
								});
							}
							else
							{
								Throw(InvlaidDynamicFunctionArgs(__FILE__, "UnifiedCtor", __LINE__));
							}
						}
					};

					static constexpr auto UnifiedDtor = [] (DynamicObject&& object)
					{
						(void) object;
					};

					static constexpr auto CtorArgsTypeName = CTTI::TypeName<Containers::Tuple<TArgs...>>();

					static String ArgsMetaData() {
						return Serilization::MetaDataGen<Containers::Tuple<TArgs...>>();
					}
				};

				template<typename T>
				struct ObjectNullCtorAndDtor {
					static constexpr auto UnifiedCtor = [] (const Containers::Array<DynamicObject>&) -> DynamicObject
					{
						Throw(InvlaidDynamicFunctionArgs(__FILE__, "ObjectNullCtorAndDtor", __LINE__));
					};

					static constexpr auto UnifiedDtor = [] (DynamicObject&& object)
					{
						(void) object;
					};

					static constexpr auto CtorArgsTypeName = Seed::Core::CTTI::Details::CreateTypeStr(L"`invalid`");

					static String ArgsMetaData() {
						return Seed::Core::CTTI::Details::CreateTypeStr(L"`Can not construct`");
					}
				};

				template<typename TClass, typename T, T pMethodType, typename TName>
				struct MethodDescriptor {
					TName Name;

					using MethodCallType = typename MethodCallDescriptor<TClass, T>::MethodType;

					using MethodClass = TClass;

					static constexpr Boolean IsConst = MethodCallDescriptor<TClass, T>::IsConst;
					static constexpr Boolean IsVolatile = MethodCallDescriptor<TClass, T>::IsVolatile;

					static constexpr auto ArgsTypeName = MethodCallDescriptor<TClass, T>::ArgsTypeName;
					static constexpr auto RetTypeName = MethodCallDescriptor<TClass, T>::RetTypeName;

					static constexpr auto pMethod = pMethodType;

					static String ArgsMetaData() {
						return MethodCallDescriptor<TClass, T>::ArgsMetaData();
					}

					static String RetMetaData() {
						return MethodCallDescriptor<TClass, T>::RetMetaData();
					}

					static constexpr auto MethodCaller = MethodCallDescriptor<TClass, T>::MethodCaller;

					static constexpr auto TypeInformationGetter = MethodCallDescriptor<TClass, T>::TypeInformationGetter;

					static constexpr bool IsArgsSerilizable = MethodCallDescriptor<TClass, T>::IsArgsSerilizable;

					static constexpr bool IsRetSerilizable = MethodCallDescriptor<TClass, T>::IsRetSerilizable;

					using Ret = typename MethodCallDescriptor<TClass, T>::Ret;
					using Args = typename MethodCallDescriptor<TClass, T>::Args;

					ForceInline constexpr MethodDescriptor(TName Name) : Name(Name) {}
				};

				template<typename ... TMethods>
				ForceInline constexpr Containers::Tuple<TMethods...> MakeMethodsTuple(TMethods... Methods) {
					return Containers::Tuple<TMethods...>(Methods...);
				}

				template<typename TTargetClass, typename TName, typename TGet, TGet GetT, typename TSetterMethods>
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
								typename MethodCallDescriptor<TTargetClass, TGet>::Ret
								>
								) &&
								CheckSetterMethods<I + 1>();
						}
					}
				public:
					using PropType = typename MethodCallDescriptor<TTargetClass, TGet>::Ret;
					using PropClass = TTargetClass;
					TName Name;
					static constexpr auto PropTypeName = CTTI::TypeName<PropType>();
					static constexpr auto GetCaller = MethodCallDescriptor<TTargetClass, TGet>::MethodCaller;
					static constexpr auto pGet = GetT;
					static constexpr auto PropTypeInformationGetter = MethodCallDescriptor<TTargetClass, TGet>::TypeInformationGetter;
					static constexpr bool IsRw = (TSetterMethods::Count != 0);
					static constexpr bool IsValueSerilizable = MethodCallDescriptor<TTargetClass, TGet>::IsRetSerilizable;
					static constexpr auto SerilizeFunction = [] (const DynamicObject& obj, const Core::Serilization::TypesRegistry& registry) -> Containers::Blob
					{
						if constexpr (IsValueSerilizable)
						{
							auto pObjGet = pGet;
							auto emptyArgs = Containers::Array<DynamicObject>();
							//HACK: we know that getter use const object but for unification method caller lambda use object without 
							//HACK: const modificator and we should remove it to get property value
							auto propValue = GetCaller(reinterpret_cast<void*&>(pObjGet), const_cast<DynamicObject&>(obj), emptyArgs)
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

					TSetterMethods SetterMethods;

					static_assert(MethodCallDescriptor<TTargetClass, TGet>::IsConst, "Property getter should be const method");
					static_assert(CheckSetterMethods(), "Invalid setters for property");

					ForceInline constexpr PropertyDescriptor(TName Name, TSetterMethods SetterMethods) : Name(Name), SetterMethods(SetterMethods) {}
				};

				template<typename ... TProperty>
				ForceInline constexpr Containers::Tuple<TProperty...> MakePropertiesTuple(TProperty... Methods) {
					return Containers::Tuple<TProperty...>(Methods...);
				}

				template<typename TObj, typename TObjectCtorAndDtor, typename TMethodsTuple, typename TPropertiesTuple>
				struct ObjectDescriptor {
					static constexpr auto Name = CTTI::TypeName<TObj>();

					static constexpr auto RttiTypeInfo = [] () 
					{
						return TypeRecord<TObj>();
					};
						
					static constexpr auto UnifiedCtor = TObjectCtorAndDtor::UnifiedCtor;
					static constexpr auto UnifiedDtor = TObjectCtorAndDtor::UnifiedDtor;

					TMethodsTuple Methods;
					TPropertiesTuple Properties;

					static constexpr auto CtorArgsTypeName = TObjectCtorAndDtor::CtorArgsTypeName;

					static String ArgsMetaData() {
						return TObjectCtorAndDtor::ArgsMetaData();
					}

					ForceInline constexpr ObjectDescriptor(const TMethodsTuple& Methods, const TPropertiesTuple& Properties) : 
						Methods(Methods), Properties(Properties) {}

					ForceInline constexpr ObjectDescriptor(TMethodsTuple&& Methods, const TPropertiesTuple& Properties) : 
						Methods(MoveRValue(Methods)), Properties(Properties) {}

					ForceInline constexpr ObjectDescriptor(const TMethodsTuple& Methods, TPropertiesTuple&& Properties) : 
						Methods(Methods), Properties(MoveRValue(Properties)) {}

					ForceInline constexpr ObjectDescriptor(TMethodsTuple&& Methods, TPropertiesTuple&& Properties) : 
						Methods(MoveRValue(Methods)), Properties(MoveRValue(Properties)) {}

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
								typename TMethodsTuple::ItemType<TId>::MethodClass,
								TObj>::State
								&&
								IsAllMethodsHaveCorrectClassImpl<TId + 1>();
						}
					}
				public:
					static_assert(IsAllMethodsHaveCorrectClassImpl(), "Invalid method passed");
				};
			}

			/// <summary>
			/// Об'єкт який містить схему об'єкта яка складається з набору методів і властивостей та конструктора якщо він заданий
			/// </summary>
			class ObjectRTTIRecord final : IDisposable, public Serilization::ISerilizable<ObjectRTTIRecord> {
			public:
				using CtorCaller = DynamicObject (*)(const Containers::Array<DynamicObject>&);
				using DtorCaller = void (*)(DynamicObject&&);
				using MethodCaller = DynamicObject(*)(const void* const&, DynamicObject&, const Containers::Array<DynamicObject>&);
				using TypeInformationGetterType = Containers::List<DataTypeRecord>(*)();
			public:
				/// <summary>
				/// Запис про метод
				/// </summary>
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
					ForceInline MethodRTTIRecord(TMethodDescriptor MethodDescriptor)
						: data(new Data(
							MethodDescriptor.Name,
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

					/// <summary>
					/// Вертає ім'я метода
					/// </summary>
					ForceInline const String& GetName() const {
						return this->data->MethodName;
					}

					/// <summary>
					/// Вертає покажчик на функцію динамічного виклику методу
					/// </summary>
					ForceInline MethodCaller GetMethodCaller() const {
						return this->data->methodCaller;
					}

					/// <summary>
					/// Вертає ім'я типу кортежу вхідних аргументів
					/// </summary>
					ForceInline const String& GetArgsTypeName() const {
						return this->data->MethodArgs;
					}

					/// <summary>
					/// Вертає ім'я типу кортежу результуючого типу методу
					/// </summary>
					ForceInline const String& GetRetTypeName() const {
						return this->data->MethodRet;
					}

					/// <summary>
					/// Вертає схему кортежу аргументів метода
					/// </summary>
					ForceInline const String& GetArgsMetaData() const {
						return this->data->MethodArgsMetaData;
					}

					/// <summary>
					/// Вертає схему кортежу результуючого типу методу
					/// </summary>
					ForceInline const String& GetRetMetaData() const {
						return this->data->MethodRetMetaData;
					}

					/// <summary>
					/// Вертає список інформацій про кожен аргумент
					/// </summary>
					ForceInline TypeInformationGetterType GetTypeInformationGetter() const {
						return this->data->typeInformationGetter;
					}

					/// <summary>
					/// Вертає логічне значення яке вказує чи результуючийтип серелізується
					/// </summary>
					ForceInline bool GetIsRetSerilizable() const {
						return this->data->IsRetSerilizable;
					}

					/// <summary>
					/// Вертає логічне значення яке вказує чи типи аргументів серелізуються
					/// </summary>
					ForceInline bool GetIsArgsSerilizable() const {
						return this->data->IsArgsSerilizable;
					}

					/// <summary>
					/// Вертає логічне значення яке вказує чи метод має модифікатор константи
					/// </summary>
					ForceInline bool GetIsConst() const {
						return this->data->IsConstMethod;
					}

					/// <summary>
					/// Вертаж логічне значення яке вказує чи метод має модифікатор волатильності 
					/// </summary>
					ForceInline bool GetIsVolatile() const {
						return this->data->IsVolatileMethod;
					}

					/// <summary>
					/// Вертає сирий покажчик на метод
					/// </summary>
					ForceInline const void* const& GetMethodPointer() const {
						return this->data->pMethod;
					}

				public:
					/// <summary>
					/// Ім'я методу
					/// </summary>
					String Property(GetName) Name;

					/// <summary>
					/// Покажчик на функцію динамічного виклику метода
					/// </summary>
					MethodCaller Property(GetMethodCaller) MethodCallerFunction;

					/// <summary>
					/// Ім'я кортежу аргументів метода
					/// </summary>
					String Property(GetArgsTypeName) ArgsTypeName;

					/// <summary>
					/// Ім'я кортежу результуючого типу
					/// </summary>
					String Property(GetRetTypeName) RetTypeName;

					/// <summary>
					/// Схема кортежу аргументів
					/// </summary>
					String Property(GetArgsMetaData) ArgsMetaData;

					/// <summary>
					/// Схема кортежу результуючого типу
					/// </summary>
					String Property(GetRetMetaData) RetMetaData;

					/// <summary>
					/// Список об'єктів інформації аргументів
					/// </summary>
					TypeInformationGetterType Property(GetTypeInformationGetter) TypeInformationGetterFunction;

					/// <summary>
					/// Вказує чи результуючий тип серелізується
					/// </summary>
					bool Property(GetIsRetSerilizable) IsRetSerilizable;

					/// <summary>
					/// Вертає чи тип кортежу аргументів серелізується
					/// </summary>
					bool Property(GetIsArgsSerilizable) IsArgsSerilizable;

					/// <summary>
					/// Вертає чи метода містить модифікатор const
					/// </summary>
					bool Property(GetIsConst) IsConst;

					/// <summary>
					/// Вертає чи метод містить модифікатор volatile
					/// </summary>
					bool Property(GetIsVolatile) IsVolatile;
				public:
					RefCountClass(MethodRTTIRecord, data);
				};

				/// <summary>
				/// Об'єкт мета опису властивості об'єкту
				/// </summary>
				class PropertyRTTIRecord final : IDisposable, public Serilization::ISerilizable<MethodRTTIRecord> {
				public:
					using TSerilizationFunction = Containers::Blob(*)(const DynamicObject&, const Core::Serilization::TypesRegistry&);
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
					ForceInline PropertyRTTIRecord(TPropertyDescriptor PropertyDescriptor) :
						data(new Data(
							PropertyDescriptor.Name,
							TPropertyDescriptor::PropTypeName,
							TPropertyDescriptor::GetCaller,
							GenSettersData<decltype(PropertyDescriptor.SetterMethods), 0>(TypeTraits::TypeTag<decltype(PropertyDescriptor.SetterMethods)>{}),
							TPropertyDescriptor::IsRw,
							reinterpret_cast<const void* const&>(TPropertyDescriptor::pGet),
							TPropertyDescriptor::PropTypeInformationGetter,
							TPropertyDescriptor::IsValueSerilizable,
							TPropertyDescriptor::SerilizeFunction,
							TPropertyDescriptor::SerilizeTypeMetaData
						)) {}
				public:
					/// <summary>
					/// Вертає ім'я властивості
					/// </summary>
					ForceInline String GetName() const {
						return data->Name;
					}

					/// <summary>
					/// Вертає ім'я типу властивості
					/// </summary>
					ForceInline String GetPropTypeName() const {
						return data->PropTypeName;
					}

					/// <summary>
					/// Вертає покажчик на функцію динамічого виклику
					/// </summary>
					ForceInline MethodCaller GetGetter() const {
						return data->Getter;
					}

					/// <summary>
					/// Вертає список об'єктів мета описів сетерів властивосі
					/// </summary>
					ForceInline const Containers::List<CallerData>& GetSetters() const {
						return data->Setters;
					}

					/// <summary>
					/// Вертає значення яке вказує чи властивість має сеттери
					/// </summary>
					ForceInline Boolean GetIsRW() const {
						return data->IsRW;
					}

					/// <summary>
					/// Вертає сирикй покажчик на геттер
					/// </summary>
					ForceInline const void* const& GetGetterAddr() const {
						return data->pGetter;
					}

					/// <summary>
					/// Вертає покажчик на функцію яка вертає опис типу властивості
					/// </summary>
					ForceInline TypeInformationGetterType GetTypeInformationGetter() const {
						return data->typeInformationGetter;
					}

					/// <summary>
					/// Викликає гетер для заданого об'єкту
					/// </summary>
					ForceInline DynamicObject CallGetterForObject(DynamicObject& obj) const {
						auto args = Containers::Array<DynamicObject>();
						return data->Getter(data->pGetter, obj, args);
					}

					/// <summary>
					/// Вказує чи властивість серелізується
					/// </summary>
					ForceInline Boolean GetIsSerilizable() const {
						return data->IsValueSerilizable;
					}

					/// <summary>
					/// Вертає функцію серелізації типу властивості
					/// </summary>
					ForceInline TSerilizationFunction GetSerilizableFunction() const {
						return data->serilizationFunction;
					}

					/// <summary>
					/// Вертає функцію яка надає метадані для серелізації
					/// </summary>
					ForceInline TSerilizationMetaDataGetter GetSerilizableMetaDataGetter() const {
						return data->serilizationMetaDataGetter;
					}
				public:
					/// <summary>
					/// Ім'я властивості
					/// </summary>
					String Property(GetName) Name;

					/// <summary>
					/// Ім'я кортежу типу властивості
					/// </summary>
					String Property(GetPropTypeName) PropertieTypeName;

					/// <summary>
					/// Функція виклику геттера
					/// </summary>
					MethodCaller Property(GetGetter) Getter;

					/// <summary>
					/// Вертає список покажчиків на сеттери і їх мета описів
					/// </summary>
					Containers::List<CallerData> Property(GetSetters) Setters;

					/// <summary>
					/// Вказує чи є сеттери в властивості
					/// </summary>
					Boolean Property(GetIsRW) IsRW;

					/// <summary>
					/// Функція яка вертає мета запис інформації про тип геттера
					/// </summary>
					TypeInformationGetterType Property(GetTypeInformationGetter) TypeInformationGetter;

					/// <summary>
					/// Вказує чи тип властивості підтримує серелізацію
					/// </summary>
					Boolean Property(GetIsSerilizable) IsSerilizable;

					/// <summary>
					/// Вертає функцію серелізації типу властивості
					/// </summary>
					TSerilizationFunction Property(GetSerilizableFunction) SerilizationFunction;

					/// <summary>
					/// Вертає функцію яка вертає мета інформацію про тип властивості
					/// </summary>
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
				ForceInline static Containers::List<MethodRTTIRecord> GenerateRTTIMethodInfo(Containers::Tuple<TMethod...> Methods) {
					if constexpr (TId == sizeof...(TMethod))
					{
						return Containers::List<MethodRTTIRecord>();
					}
					else
					{
						return Containers::List<MethodRTTIRecord>(MoveRValue(
							(Containers::List<MethodRTTIRecord>({
							MethodRTTIRecord(Methods.GetByIdx<TId>())
								})
								+=
								MoveRValue(GenerateRTTIMethodInfo<TId + 1>(Methods)))
						));
					}
				}

				template<SizeT TId, typename ... TProperties>
				ForceInline static Containers::List<PropertyRTTIRecord> GenerateRTTIPropertieInfo(Containers::Tuple<TProperties...> Properties) {
					if constexpr (TId == sizeof...(TProperties))
					{
						return Containers::List<PropertyRTTIRecord>();
					}
					else
					{
						return Containers::List<PropertyRTTIRecord>(MoveRValue(
							(
								Containers::List<PropertyRTTIRecord>({
									PropertyRTTIRecord(Properties.GetByIdx<TId>())
								}) += 
									MoveRValue(GenerateRTTIPropertieInfo<TId + 1>(Properties)
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
				ForceInline ObjectRTTIRecord(TObjectDescriptor ObjectDescriptor) :
					data(new Data(
						TObjectDescriptor::Name,
						TObjectDescriptor::CtorArgsTypeName,
						TObjectDescriptor::ArgsMetaData(),
						TObjectDescriptor::UnifiedCtor,
						TObjectDescriptor::UnifiedDtor,
						TObjectDescriptor::RttiTypeInfo(),
						GenerateRTTIMethodInfo<0>(ObjectDescriptor.Methods),
						GenerateRTTIPropertieInfo<0>(ObjectDescriptor.Properties)
					)) {};
			public:
				/// <summary>
				/// Ім'я типу об'єкта
				/// </summary>
				ForceInline const String& GetName() const {
					return this->data->ClassName;
				}

				/// <summary>
				/// Вертає ім'я кортежу аргументів конструктора
				/// </summary>
				ForceInline const String& GetCtorArgs() const {
					return this->data->CtorArgs;
				}

				/// <summary>
				/// Вертає схему типу кортежу аргументів конструктора
				/// </summary>
				ForceInline const String& GetArgsMetaData() const {
					return this->data->CtorMetaDate;
				}

				/// <summary>
				/// Вертає функцію динамічного створення об'єкта
				/// </summary>
				ForceInline CtorCaller GetCtorCaller() const {
					return this->data->ctorCaller;
				}

				/// <summary>
				/// Вертає функцію знищення об'єкта
				/// </summary>
				ForceInline DtorCaller GetDtorCaller() const {
					return this->data->dtorCaller;
				}

				/// <summary>
				/// Вертає список мета описів методів типу об'єкта
				/// </summary>
				ForceInline const Containers::List<MethodRTTIRecord>& GetAllMethods() const {
					return this->data->Methods;
				}

				/// <summary>
				/// Вертає список мета описів властивостей типу ооб'єкта
				/// </summary>
				ForceInline const Containers::List<PropertyRTTIRecord>& GetAllProperties() const {
					return this->data->Properties;
				}

				/// <summary>
				/// Вертає RTTI інформацію про тип об'єкта 
				/// </summary>
				ForceInline RTTI::RTTITypeRecord GetRTTITypeInfo() const {
					return this->data->RttiTypeInfo;
				}
			public:
				/// <summary>
				/// Ім'я типу
				/// </summary>
				String Property(GetName) ClassName;

				/// <summary>
				/// Ім'я тип кортежа аргументів конструктора
				/// </summary>
				String Property(GetCtorArgs) CtorArgs;

				/// <summary>
				/// Схема кортежа аргументів конструктора
				/// </summary>
				String Property(GetArgsMetaData) ArgsMetaData;

				/// <summary>
				/// Функція динамічного конструкювання об'єкта
				/// </summary>
				CtorCaller Property(GetCtorCaller) CtorCallerFunction;

				/// <summary>
				/// Функція динамічого знищення об'єкта
				/// </summary>
				DtorCaller Property(GetDtorCaller) DtorCallerFunction;

				/// <summary>
				/// Список мета описів методів об'єкта
				/// </summary>
				Containers::List<MethodRTTIRecord> Property(GetAllMethods) Methods;

				/// <summary>
				/// Список мета описів властивостей об'єкта
				/// </summary>
				Containers::List<PropertyRTTIRecord> Property(GetAllProperties) Properties;
			public:
				RefCountClass(ObjectRTTIRecord, data);
			};
		}
	}
}

/// <summary>
/// Збирає описів колекцію методів(MethodDescription або MethodDescriptionWithArgs) в один кортеж
/// </summary>
#define ObjectMethodsDefs(...)                                              					\
	Seed::Core::Reflection::Details::MakeMethodsTuple(__VA_ARGS__)								

/// <summary>
/// Збирає описів колекцію властивостей(ObjectProperty) в один кортеж
/// </summary>
#define ObjectPropsDefs(...) 																	\
	Seed::Core::Reflection::Details::MakePropertiesTuple(__VA_ARGS__)							

/// <summary>
/// Опис метода
/// </summary>
/// <param name="Class">Класс метод якого описується</param>
/// <param name="Method">Ім'я метода</param>
/// <returns>Об'єкт MethodDescriptor який містить опис метода</returns>
#define MethodDescription(Class, Method) 														\
	Seed::Core::Reflection::Details::MethodDescriptor<											\
		Class, 																					\
		decltype(&Class::Method), 																\
		&Class::Method, 																		\
		decltype(Seed::Core::CTTI::Details::CreateTypeStr(L"" #Method))							\
	>(Seed::Core::CTTI::Details::CreateTypeStr(L"" #Method))										


/// <summary>
/// Опис метода з вказуванням типів аргументів для вибору певної перегрузки 
/// </summary>
/// <param name="Class">Класс метод якого описується</param>
/// <param name="Method">Ім'я метода</param>
/// <param name="RetType">Тип повернення з CV Ref модифікаторами</param>
/// <param name="...">Типи аргментів з CV Ref модифікаторами</param>
/// <returns>Об'єкт MethodDescriptor який містить опис метода</returns> 
#define MethodDescriptionWithArgs(Class, Method, RetType, ...) 									\
	Seed::Core::Reflection::Details::MethodDescriptor< 											\
		Class, 																					\
		RetType(Class::*)(__VA_ARGS__), 														\
		&Class::Method, 																		\
		decltype(Seed::Core::CTTI::Details::CreateTypeStr(L"" #Method))							\
	>(Seed::Core::CTTI::Details::CreateTypeStr(L"" #Method))											

/// <summary>
/// Опис властивості
/// </summary>
/// <param name="Class">Класс властивість якого описується</param>
/// <param name="Name">Назва властивості</param>
/// <param name="Getter">Метод який надає значення властивості</param>
/// <param name="...">Нуль або більше об'єктів MethodDescriptor або MethodDescriptionWithArgs 
/// які описують методи встановлення значення властивості якщо такі є</param>
/// <returns>Об'єкт PropertyDescriptor який містить опис властивості</returns>
#define	ObjectProperty(Class, Name, Getter, ...) 												\
	Seed::Core::Reflection::Details::PropertyDescriptor< 										\
		Class, 																					\
		decltype(Seed::Core::CTTI::Details::CreateTypeStr(L"" #Name)), 							\
		decltype(&Class::Getter), 																\
		&Class::Getter, 																		\
		decltype(ObjectMethodsDefs(__VA_ARGS__))												\
	>(Seed::Core::CTTI::Details::CreateTypeStr(L"" #Name), ObjectMethodsDefs(__VA_ARGS__))			
						
/// <summary>
/// Опис констора та деструктора
/// </summary>
/// <param name="Class">Класс конструктор та деструктор якого описується</param>
/// <param name="...">Нуль або більше типів з CV Ref модифікаторами 
/// які є аргументами конструктора</param>
/// <returns>Об'єкт ObjectCtorAndDtor 
/// який містить опис конструктора та деструктора типу</returns>
#define CtorDescription(Class, ...) 															\
	Seed::Core::Reflection::Details::ObjectCtorAndDtor<Class, __VA_ARGS__>						

/// <summary>
/// Опис деструктора і вказування що конструктор(и) недоступний
/// </summary>
/// <param name="Class">Класс деструктор якого описується</param>
/// <returns>Об'єкт ObjectCtorAndDtor 
/// який містить опис деструктора типу</returns> 
#define NullCtorDescription(Class) 																\
	Seed::Core::Reflection::Details::ObjectNullCtorAndDtor<Class>								

/// <summary>
/// Опис схеми класу(створення об'єкта ObjectDescriptor)
/// </summary>
/// <param name="Class">Класс схема якого описується</param>
/// <param name="CtorDesc">Опис деструктора(CtorDescription або NullCtorDescription)</param>
/// <param name="Methods">Список методів классу(ObjectMethodsDefs)</param>
/// <param name="Props">Опис властивостей класу(ObjectPropsDefs)</param>
/// <returns>ОБ'єкт ObjectDescriptor який містить компіл тайм схему типу</returns>
#define ObjectDefinition(Class, CtorDesc, Methods, Props)                                       \
	[]() constexpr {																		    \
		constexpr auto methods = Methods;													    \
		constexpr auto properties = Props;													    \
																							    \
		using methodsTupleType = Seed::Core::TypeTraits::DecayV<decltype(methods)>;			    \
		using propertiesTupleType = Seed::Core::TypeTraits::DecayV<decltype(properties)>;	    \
																							    \
		constexpr auto descriptor = Seed::Core::Reflection::Details::ObjectDescriptor<		    \
			Class, CtorDesc, methodsTupleType, propertiesTupleType>(methods, properties);	    \
																							    \
		return descriptor;																	    \
	}()
	

/// <summary>
/// Генерація об'єкта ObjectRTTIRecord який містить рантайм опис схеми типу
/// </summary>
/// <param name="ObjectTypeRecord">Об'єкт ObjectTypeRecord 
/// який містить компіл тайм опис схеми типуу</param>
/// <returns>Об'єкт ObjectRTTIRecord який містить опис схеми типу</returns>
#define GenObjectRTTIRecord(ObjectTypeRecord)													\
	Seed::Core::Reflection::ObjectRTTIRecord(ObjectTypeRecord)
