#pragma once
#include "Seed.Core.Services.Base.h"
#include "Seed.Core.Services.BootupServicesManager.h"

namespace Seed
{
	inline namespace Core
	{
		namespace CTTI
		{
			namespace TypeTraits
			{
				namespace Details
				{
					template<bool TIsConst, bool TIsVolatile> 
					struct CTTITypeDataImpl<Containers::Dynamic, TIsConst, TIsVolatile> 
						: BaseCTTITypeDataImpl<Containers::Dynamic, EType::Dynamic, true, TIsConst, TIsVolatile>
					{};

					template<bool TIsConst, bool TIsVolatile> 
					struct CTTITypeDataImpl<Reflection::DynamicObject, TIsConst, TIsVolatile> 
						: BaseCTTITypeDataImpl<Reflection::DynamicObject, EType::Dynamic, true, TIsConst, TIsVolatile>
					{};

					template<>
					struct IsDynamicT<Containers::Dynamic> : Core::TypeTraits::LogicTrue {};

					template<>
					struct IsDynamicT<Reflection::DynamicObject> : Core::TypeTraits::LogicTrue {};
				}
			}
		}

		namespace Serilization
		{
			namespace Details
			{
				template<typename THeap, typename T>
                struct BaseSerlizationHandler<THeap, T, typename TypeTraits::EnableIfV<TypeTraits::IsSameV<T, Containers::Dynamic>, void>> {
                    ForceInline static void SerilizeImpl(const T& data, Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
						if (registry.Any([&] (const CTTI::TypeTraits::CTTITypeMetaData<TypesRegistry>::ItemType& item) InlineLambda
						{
							return item.Key == data.Type;
						}))
						{
							auto& record = registry.First([&] (const CTTI::TypeTraits::CTTITypeMetaData<TypesRegistry>::ItemType& item) InlineLambda
							{
								return item.Key == data.Type;
							});

							BaseSerlizationHandler<THeap, TypeTraits::DecayV<decltype(record.Key.CxxTypeName)>>::SerilizeImpl
								(record.Key.CxxTypeName, rawStream, offset, registry);

							record.Value.SerelizeToDynamicFunction(data, rawStream, offset, registry);
						}
						else
						{
							Throw(InvalidArgumentException(ExceptionPosition, L"Data", L"Invalid data type"));
						}
                    }

                    ForceInline static constexpr SizeT DataSize(const T& data, UnusedData const TypesRegistry& registry) {
						if (registry.Any([&] (const CTTI::TypeTraits::CTTITypeMetaData<TypesRegistry>::ItemType& item) InlineLambda
						{
							return item.Key == data.Type;
						}))
						{
							auto& record = registry.First([&] (const CTTI::TypeTraits::CTTITypeMetaData<TypesRegistry>::ItemType& item) InlineLambda
							{
								return item.Key == data.Type;
							});

							return record.Value.DataSizeDynamicFunction(data, registry) +
								BaseSerlizationHandler<THeap, TypeTraits::DecayV<decltype(record.Key.CxxTypeName)>>::
								DataSize(record.Key.CxxTypeName, registry);
						}
						else
						{
							Throw(InvalidArgumentException(ExceptionPosition, L"Data", L"Invalid data type"));
						}
                    }

                    ForceInline static constexpr Boolean IsMemCopy() {
                        return false;
                    }

                    ForceInline static constexpr auto TypeHint() {
                        return
                            CTTI::Details::CreateTypeStr(L"N/A");
                    }

                    ForceInline static constexpr auto ConstexprSize() {
                        return
                            CTTI::Details::ToCharStr<sizeof(T)>();
                    }

                    ForceInline static void DeserelizeImpl(T& data, const Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
						String cxxTypeName;
						
						BaseSerlizationHandler<THeap, String>::DeserelizeImpl(cxxTypeName, rawStream, offset, registry);

						if (registry.Any([&] (const CTTI::TypeTraits::CTTITypeMetaData<TypesRegistry>::ItemType& item) InlineLambda
						{
							return item.Key.CxxTypeName == cxxTypeName;
						}))
						{
							auto& record = registry.First([&] (const CTTI::TypeTraits::CTTITypeMetaData<TypesRegistry>::ItemType& item) InlineLambda
							{
								return item.Key.CxxTypeName == cxxTypeName;
							});

							record.Value.DeSerelizeToDynamicFunction(data, rawStream, offset, registry);
						}
						else
						{
							Throw(InvalidArgumentException(ExceptionPosition, L"Data", L"Invalid data type"));
						}
                    }

                    ForceInline static constexpr auto TypeMetaData() {
                        return
                            CTTI::Details::CreateTypeStr(L"{") +
                            CTTI::TypeName<T>() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            CTTI::Details::ToCharStr<static_cast<SizeT>(CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass)>() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            CTTI::Details::ToCharStr<sizeof(T)>() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            ConstexprSize() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            TypeHint() +
							CTTI::Details::CreateTypeStr(L"|") +
                            CTTI::Details::CreateTypeStr(L"") +
                            CTTI::Details::CreateTypeStr(L"}");
                    }

					ForceInline static BaseTypeMetaDataRecord<THeap> GetTypeMetaDataRecord() {
						return BaseTypeMetaDataRecord<THeap>(
							CTTI::TypeName<T>(),
							CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass,
							sizeof(T),
							0,
							TypeHint()
						);
					}
                };

				template<typename THeap, typename T>
                struct BaseSerlizationHandler<THeap, T, typename TypeTraits::EnableIfV<TypeTraits::IsSameV<T, Reflection::DynamicObject>, void>> {
                    ForceInline static void SerilizeImpl(const T& data, Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
						if (registry.Any([&] (const CTTI::TypeTraits::CTTITypeMetaData<TypesRegistry>::ItemType& item) InlineLambda
						{
							return item.Key == data.ValueType;
						}))
						{
							auto& record = registry.First([&] (const CTTI::TypeTraits::CTTITypeMetaData<TypesRegistry>::ItemType& item) InlineLambda
							{
								return item.Key == data.ValueType;
							});

							BaseSerlizationHandler<THeap, TypeTraits::DecayV<decltype(record.Key.CxxTypeName)>>::SerilizeImpl
								(record.Key.CxxTypeName, rawStream, offset, registry);

							record.Value.SerelizeFunction(data, rawStream, offset, registry);
						}
						else
						{
							Throw(InvalidArgumentException(ExceptionPosition, L"Data", L"Invalid data type"));
						}
                    }

                    ForceInline static constexpr SizeT DataSize(const T& data, UnusedData const TypesRegistry& registry) {
						if (registry.Any([&] (const CTTI::TypeTraits::CTTITypeMetaData<TypesRegistry>::ItemType& item) InlineLambda
						{
							return item.Key == data.ValueType;
						}))
						{
							auto& record = registry.First([&] (const CTTI::TypeTraits::CTTITypeMetaData<TypesRegistry>::ItemType& item) InlineLambda
							{
								return item.Key == data.ValueType;
							});

							return record.Value.DataSizeFunction(data, registry) +
								BaseSerlizationHandler<THeap, TypeTraits::DecayV<decltype(record.Key.CxxTypeName)>>::
								DataSize(record.Key.CxxTypeName, registry);
						}
						else
						{
							Throw(InvalidArgumentException(ExceptionPosition, L"Data", L"Invalid data type"));
						}
                    }

                    ForceInline static constexpr Boolean IsMemCopy() {
                        return false;
                    }

                    ForceInline static constexpr auto TypeHint() {
                        return
                            CTTI::Details::CreateTypeStr(L"N/A");
                    }

                    ForceInline static constexpr auto ConstexprSize() {
                        return
                            CTTI::Details::ToCharStr<sizeof(T)>();
                    }

                    ForceInline static void DeserelizeImpl(T& data, const Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
						String cxxTypeName;
						
						BaseSerlizationHandler<THeap, String>::DeserelizeImpl(cxxTypeName, rawStream, offset, registry);

						if (registry.Any([&] (const CTTI::TypeTraits::CTTITypeMetaData<TypesRegistry>::ItemType& item) InlineLambda
						{
							return item.Key.CxxTypeName == cxxTypeName;
						}))
						{
							auto& record = registry.First([&] (const CTTI::TypeTraits::CTTITypeMetaData<TypesRegistry>::ItemType& item) InlineLambda
							{
								return item.Key.CxxTypeName == cxxTypeName;
							});

							record.Value.DeSerelizeFunction(data, rawStream, offset, registry);
						}
						else
						{
							Throw(InvalidArgumentException(ExceptionPosition, L"Data", L"Invalid data type"));
						}
                    }

                    ForceInline static constexpr auto TypeMetaData() {
                        return
                            CTTI::Details::CreateTypeStr(L"{") +
                            CTTI::TypeName<T>() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            CTTI::Details::ToCharStr<static_cast<SizeT>(CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass)>() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            CTTI::Details::ToCharStr<sizeof(T)>() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            ConstexprSize() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            TypeHint() +
							CTTI::Details::CreateTypeStr(L"|") +
                            CTTI::Details::CreateTypeStr(L"") +
                            CTTI::Details::CreateTypeStr(L"}");
                    }

					ForceInline static BaseTypeMetaDataRecord<THeap> GetTypeMetaDataRecord() {
						return BaseTypeMetaDataRecord<THeap>(
							CTTI::TypeName<T>(),
							CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass,
							sizeof(T),
							0,
							TypeHint()
							);
					}
                };
			}
		}

		namespace Services
		{
			/// <summary>
			/// Сервіс серелізації даних
			/// </summary>
			class SerilizationService final : public IDisposable, IBootUpService<SerilizationService, DI::IDI> {
			public:
				/// <summary>
				/// Схема сервісу
				/// </summary>
				/// <returns>Рантайм опис типу сервіса</returns>
				static auto GetRttiInformation() {
					constexpr auto SerilizationServiceDefinition = ObjectDefinition(
						Services::SerilizationService,
						NullCtorDescription(Services::SerilizationService),
						ObjectMethodsDefs(
							MethodDescription(Services::SerilizationService, SerelizeDynObj),
							MethodDescription(Services::SerilizationService, DeserelizeDynObj)
						),
						ObjectPropsDefs()
					);

					return GenObjectRTTIRecord(SerilizationServiceDefinition);
				}
			private:
				struct Data {
					DI::IDI idi;
					RefCountT RefCount;

					Data(const DI::IDI& idi) : idi(idi), RefCount(1) {}
				};
			private:
				Data* This;
			protected:
				virtual void Dispose() override {}
			public:
				/// <summary>
				/// Конструктор завантажує інформацію про типи з сховища типів
				/// </summary>
				/// <param name="idi">Сховище типів</param>
				SerilizationService(const DI::IDI& idi) : This(new Data(idi)) {}
			public:
				/// <summary>
				/// Серелізація типу
				/// </summary>
				/// <typeparam name="T">Тип даних</typeparam>
				/// <param name="data">Об'єкт даних</param>
				/// <returns>Масив байтів</returns>
				template<typename T> requires CTTI::Concepts::IsCttiSerelizable<T>
				Containers::Blob Serelize(const T& data) const {
					return Serilization::Serilize(data, This->idi.GetTypesRegistry());
				}

				/// <summary>
				/// Десерелізація
				/// </summary>
				/// <typeparam name="T">Тип даних</typeparam>
				/// <param name="data">Масив байтів який містить дані</param>
				/// <returns>Об'єкт даних</returns>
				template<typename T> requires Concepts::IsConstructibleWith<T> && CTTI::Concepts::IsCttiSerelizable<T>
				T Deserelize(const Containers::Blob& data) const {
					T val = T();
					Serilization::Deserilize(data, val, This->idi.GetTypesRegistry());
					return val;
				}

				/// <summary>
				/// Вертає рядкове представлення схеми типу
				/// </summary>
				/// <typeparam name="T">Тип даних</typeparam>
				/// <returns>Рядкове представлення схеми даних</returns>
				template<typename T> requires CTTI::Concepts::IsCttiSerelizable<T>
				String GetTypeMetaData() const {
					return Serilization::MetaDataGen<T>();
				}

				/// <summary>
				/// Вертає схему типу
				/// </summary>
				/// <typeparam name="T">Тип даних</typeparam>
				/// <returns>Схема типу</returns>
				template<typename T> requires CTTI::Concepts::IsCttiSerelizable<T>
				Serilization::TypeMetaDataRecord GetTypeShema() const {
					return Serilization::GetTypeRecordForType<T>();
				}
				
				/// <summary>
				/// Серелізація динамічного об'єкта
				/// </summary>
				/// <param name="data">Об'єкт даних</param>
				/// <returns>Масив байтів</returns>
				Containers::Blob SerelizeDynObj(const Reflection::DynamicObject& data) const {
					return Serelize(data);
				}

				/// <summary>
				/// Десерелізація в динамічний об'єкт
				/// </summary>
				/// <param name="data">Масив байтів</param>
				/// <returns>Динамічний об'єкт</returns>
				Reflection::DynamicObject DeserelizeDynObj(const Containers::Blob& data) const {
					return Deserelize<Reflection::DynamicObject>(data);
				}
			public:
				RefCountClass(SerilizationService, This);
			};
		}
	}
}