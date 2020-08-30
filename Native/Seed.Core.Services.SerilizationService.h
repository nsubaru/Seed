#pragma once
#include "Seed.Core.Stage1.h"
#include "Seed.Core.DepencyInjection.h"

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
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("N/A")());
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
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("{")()) +
                            CTTI::TypeName<T>().Resize<CTTI::TypeName<T>().Count - 1>() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            CTTI::Details::ToCharStr<static_cast<SizeT>(CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass)>() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            CTTI::Details::ToCharStr<sizeof(T)>() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            ConstexprSize() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            TypeHint() +
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("")()) +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("}")());
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
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("N/A")());
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
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("{")()) +
                            CTTI::TypeName<T>().Resize<CTTI::TypeName<T>().Count - 1>() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            CTTI::Details::ToCharStr<static_cast<SizeT>(CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass)>() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            CTTI::Details::ToCharStr<sizeof(T)>() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            ConstexprSize() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            TypeHint() +
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("")()) +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("}")());
                    }
                };
			}
		}

		namespace Services
		{
			class SerilizationService final : public IDisposable {
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
				SerilizationService(const DI::IDI& idi) : This(new Data(idi)) {}
			public:
				template<typename T>// requires CTTI::Concepts::IsCttiSerelizable<T>
				Containers::Blob Serelize(const T& data) const {
					return Serilization::Serilize(data, This->idi.GetTypesRegistry());
				}

				template<typename T> requires Concepts::IsConstructibleWith<T>// && CTTI::Concepts::IsCttiSerelizable<T>
				T Deserelize(const Containers::Blob& data) const {
					T val = T();
					Serilization::Deserilize(data, val, This->idi.GetTypesRegistry());
					return val;
				}

				template<typename T>// requires CTTI::Concepts::IsCttiSerelizable<T>
				String GetTypeMetaData() const {
					return Serilization::MetaDataGen<T>();
				}

				Containers::Blob SerelizeDynObj(const Reflection::DynamicObject& data) const {
					return Serelize(data);
				}

				Reflection::DynamicObject DeserelizeDynObj(const Containers::Blob& data) const {
					return Deserelize<Reflection::DynamicObject>(data);
				}

				String GetTypeMetaDataDynObj(const Reflection::DynamicObject& data) const;
			public:
				RefCountClass(SerilizationService, This);
			};
		}
	}
}