#pragma once
#include "Seed.Core.Containers.h"
#include "Seed.Core.CTTI.h"

#pragma warning(push)
//This internal implementation file which can use internal API functions
#pragma warning(disable: 4996)

namespace Seed
{
    inline namespace Core
    {
        namespace Reflection
        {
            class DataTypeRecord;
        }

        namespace RTTI
        {
            class RTTITypeRecord;
        }

        namespace Serilization
        {
            ///<summary>Не можливо серелізувати або десерелізувати даний тип</summary>
			class NonSerelizableType : public Exception {
			private:
				/// <summary>
				/// Ім'я типу який не знайдено
				/// </summary>
				ExceptionString typeName;
            public:
                NonSerelizableType(const NonSerelizableType& orig) : Exception(orig) {}
                NonSerelizableType(NonSerelizableType&& from) : Exception(MoveRValue(from)) {}
				NonSerelizableType& operator=(const NonSerelizableType& from) { this->~NonSerelizableType(); this->NonSerelizableType::NonSerelizableType(from); return *this; }
				NonSerelizableType& operator=(NonSerelizableType&& from) { this->~NonSerelizableType(); this->NonSerelizableType::NonSerelizableType(MoveRValue(from)); return *this; }
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				///<param Name = 'Type'>Тип який DI не зумів знайти</param>
				NonSerelizableType(const ConstAString& File, const ConstAString& FunctName, SizeT Line, ExceptionString&& typeName)
					:Exception(File, FunctName, Line, L"Неможливо серелізувати даний тип"), typeName(MoveRValue(typeName)) {}

				///<summary>Текстове представлення інформації про помилки помилки</summary>
				virtual ExceptionString toString() const override final {
                    return ExceptionString::Combine({
                        ExceptionString(exceptionName),
                        ExceptionString(L". Ім'я типу: "), typeName, ExceptionString(L". "),
                        location.toString(), 
						(innerException == nullptr) ? ExceptionString(L".") : (L". Також є вкладена помилка: " + InnerException->toString())
                    });
				}

                /// <summary>
				/// Вертає копію об'єкта помилки
				/// </summary>
				/// <returns>Розумний покажчик на об'єкт помилки</returns>
				virtual ExceptionHolderType Clone() const override {
					return ExceptionHolderType(new NonSerelizableType(*this));
				}

				virtual ~NonSerelizableType() {}
			};

            using TypesRegistry = Containers::ConcurencyList<Containers::Pair<RTTI::RTTITypeRecord, Reflection::DataTypeRecord>>;

            namespace Details
            {
#if __has_include("Seed.Core.Containers.API.LINQ.Base.h")
#else
                template<typename THeap, typename T, typename Enable = void>
                struct BaseSerlizationHandler;
#endif
                template<typename T>
                constexpr bool IsNotSerlizable = 
                    !CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable;

                template<typename THeap, typename T>
                struct BaseSerlizationHandler<THeap, T, typename TypeTraits::EnableIfV<IsNotSerlizable<T>, void>> {
                    ForceInline static void SerilizeImpl(const T& data, Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
                        Throw(NonSerelizableType(ExceptionPosition, CTTI::TypeName<T>()));
                        __assume(false);
                    }

                    ForceInline static constexpr SizeT DataSize(const T& data, UnusedData const TypesRegistry& registry) {
                        Throw(NonSerelizableType(ExceptionPosition, CTTI::TypeName<T>()));
                        __assume(false);
                    }

                    ForceInline static constexpr Boolean IsMemCopy() {
                        Throw(NonSerelizableType(ExceptionPosition, CTTI::TypeName<T>()));
                        __assume(false);
                    }

                    ForceInline static constexpr auto TypeHint() {
                        return
                            CTTI::Details::CreateTypeStr("N/A");
                    }

                    ForceInline static constexpr auto ConstexprSize() {
                        return
                            CTTI::Details::ToCharStr<sizeof(T)>();
                    }

                    ForceInline static void DeserelizeImpl(T& data, const Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
                         Throw(NonSerelizableType(ExceptionPosition, CTTI::TypeName<T>()));
                         __assume(false);
                    }

                    ForceInline static constexpr auto TypeMetaData() {
                        return
                            CTTI::Details::CreateTypeStr("{") +
                            CTTI::TypeName<T>().Resize<CTTI::TypeName<T>().Count - 1>() +
                            CTTI::Details::CreateTypeStr("|") +
                            CTTI::Details::ToCharStr<static_cast<SizeT>(CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass)>() +
                            CTTI::Details::CreateTypeStr("|") +
                            CTTI::Details::ToCharStr<sizeof(T)>() +
                            CTTI::Details::CreateTypeStr("|") +
                            ConstexprSize() +
                            CTTI::Details::CreateTypeStr("|") +
                            TypeHint() +
							CTTI::Details::CreateTypeStr("|") +
                            CTTI::Details::CreateTypeStr("") +
                            CTTI::Details::CreateTypeStr("}");
                    }
                };

                template<typename T>
                constexpr bool IsSerilizablePrimitive = (
                    CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::TypeTraits::EType::Numeric ||
                    CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::TypeTraits::EType::Character ||
                    CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::TypeTraits::EType::FloatNumeric ||
                    CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::TypeTraits::EType::Logic ||
                    CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::TypeTraits::EType::Pod
                    )
                    &&
                    CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable;

                template<typename THeap, typename T>
                struct BaseSerlizationHandler<THeap, T,typename TypeTraits::EnableIfV<IsSerilizablePrimitive<T>, void>> {
                    ForceInline static void SerilizeImpl(const T& data, Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
                        Memory::Copy(reinterpret_cast<const Byte*>(&data), rawStream + offset, sizeof(T)); offset += sizeof(T);
                    }

                    ForceInline static constexpr SizeT DataSize(const T& data, UnusedData const TypesRegistry& registry) {
                        return sizeof(data);
                    }

                    ForceInline static constexpr Boolean IsMemCopy() {
                        return true;
                    }

                    ForceInline static constexpr auto TypeHint() {
                        return
                            CTTI::Details::CreateTypeStr("");
                    }

                    ForceInline static constexpr auto ConstexprSize() {
                        return
                            CTTI::Details::ToCharStr<sizeof(T)>();
                    }

                    ForceInline static void DeserelizeImpl(T& data, const Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
                        data = *(reinterpret_cast<const T*>(rawStream + offset)); offset += sizeof(T);
                    }

                    ForceInline static constexpr auto TypeMetaData() {
                        return
                            CTTI::Details::CreateTypeStr("{") +
                            CTTI::TypeName<T>().Resize<CTTI::TypeName<T>().Count - 1>() +
                            CTTI::Details::CreateTypeStr("|") +
                            CTTI::Details::ToCharStr<static_cast<SizeT>(CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass)>() +
                            CTTI::Details::CreateTypeStr("|") +
                            CTTI::Details::ToCharStr<sizeof(T)>() +
                            CTTI::Details::CreateTypeStr("|") +
                            ConstexprSize() +
                            CTTI::Details::CreateTypeStr("|") +
                            TypeHint() +
							CTTI::Details::CreateTypeStr("|") +
                            CTTI::Details::CreateTypeStr("") +
                            CTTI::Details::CreateTypeStr("}");
                    }
                };

                template<typename T>
                constexpr bool IsCFixedArraySerlizable = 
                    CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::TypeTraits::EType::FixedArray &&
                    CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable;

                template<typename THeap, typename T>
                struct BaseSerlizationHandler<THeap, T, typename TypeTraits::EnableIfV<IsCFixedArraySerlizable<T>, void>> {
                    using ItemType = typename CTTI::TypeTraits::CTTITypeMetaData<T>::ItemType::Type;

                    ForceInline static void SerilizeImpl(const T& data, Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
                        if constexpr (IsMemCopy())
                        {
                            SizeT dataSize = DataSize(data, registry);
                            Memory::Copy(reinterpret_cast<const Byte*>(&data), rawStream + offset, dataSize); 
                            offset += dataSize;
                        }
                        else
                        {
                            for (SizeT i = 0; i < CTTI::TypeTraits::CTTITypeMetaData<T>::Count; i++)
                            {
                                BaseSerlizationHandler<THeap, ItemType>::SerilizeImpl(data[i], rawStream, offset, registry);
                            }
                        }
                        
                    }

                    ForceInline static constexpr SizeT DataSize(const T& data, UnusedData const TypesRegistry& registry) {
                        if constexpr (IsMemCopy())
                        {
                            return sizeof(ItemType) * CTTI::TypeTraits::CTTITypeMetaData<T>::Count;
                        }
                        else
                        {
                            SizeT Size = 1;
                            for (SizeT i = 0; i < CTTI::TypeTraits::CTTITypeMetaData<T>::Count; i++)
                            {
                                Size *= BaseSerlizationHandler<THeap, ItemType>::DataSize(data[i]);
                            }
                            return Size;
                        }
                    }

                    ForceInline static constexpr Boolean IsMemCopy() {
                        return BaseSerlizationHandler<THeap, ItemType>::IsMemCopy();
                    }

                    ForceInline static constexpr auto TypeHint() {
                        return
                            CTTI::Details::CreateTypeStr("");
                    }

                    ForceInline static constexpr auto ConstexprSize() {
                        return
                            CTTI::Details::ToCharStr<CTTI::TypeTraits::CTTITypeMetaData<T>::Count>();
                    }

                    ForceInline static void DeserelizeImpl(T& data, const Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
                        if constexpr (IsMemCopy())
                        {
                            SizeT dataSize = sizeof(T);
                            Memory::Copy(rawStream + offset, data, dataSize); offset += dataSize;
                        }
                        else
                        {
                            SizeT Size = 1;
                            for (SizeT i = 0; i < CTTI::TypeTraits::CTTITypeMetaData<T>::Count; i++)
                            {
                                Size *= BaseSerlizationHandler<THeap, ItemType>::DeserelizeImpl(data[i], rawStream, offset, registry);
                            }
                        }
                    }

                    ForceInline static constexpr auto TypeMetaData() {
                        return
                            CTTI::Details::CreateTypeStr("{") +
                            CTTI::TypeName<T>().Resize<CTTI::TypeName<T>().Count - 1>() +
                            CTTI::Details::CreateTypeStr("|") +
                            CTTI::Details::ToCharStr<static_cast<SizeT>(CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass)>() +
                            CTTI::Details::CreateTypeStr("|") +
                            CTTI::Details::ToCharStr<sizeof(T)>() +
                            CTTI::Details::CreateTypeStr("|") +
                            ConstexprSize() +
                            CTTI::Details::CreateTypeStr("|") +
                            TypeHint() +
                            CTTI::Details::CreateTypeStr("|") +
                            BaseSerlizationHandler<THeap, ItemType>::TypeMetaData() +
                            CTTI::Details::CreateTypeStr("}");
                    }
                };
            }

            template<typename THeap, typename T>
            ForceInline Containers::BaseArray<THeap, Byte> BaseSerilize(const T& Object, UnusedData const TypesRegistry& registry) {
                SizeT rawStreamSize = Details::BaseSerlizationHandler<THeap, T>::DataSize(Object, registry);
                Byte* rawStream = nullptr;
                try
                {
                    rawStream = new Byte[rawStreamSize];
                    SizeT offset = 0;
                    Details::BaseSerlizationHandler<THeap, T>::SerilizeImpl(Object, rawStream, offset, registry);
                }
                catch (...)
                {
                    delete[] rawStream;
                    ReThrow;
                }
                return Containers::BaseArray<THeap, Byte>(MoveRValue(rawStream), rawStreamSize);
            }

            template<typename THeap, typename T>
            ForceInline void BaseDeserilize(const Containers::BaseArray<THeap, Byte>& stream, T& Object, UnusedData const TypesRegistry& registry) {
                const Byte* rawStream = (const Byte*) stream;
                SizeT offset = 0;
                Details::BaseSerlizationHandler<THeap, T>::DeserelizeImpl(Object, rawStream, offset, registry);
            }

            template<typename T>
            ForceInline Containers::Array<Byte> Serilize(const T& Object, UnusedData const TypesRegistry& registry) {
                return BaseSerilize<Memory::Heap, T>(Object, registry);
            }

            template<typename T>
            ForceInline Containers::SensitiveArray<Byte> SensitiveSerilize(const T& Object, UnusedData const TypesRegistry& registry) {
                return BaseSerilize<Memory::SensitiveDataHeap, T>(Object, registry);
            }

            template<typename T>
            ForceInline void Deserilize(const Containers::Array<Byte> & stream, T& Object, UnusedData const TypesRegistry& registry) {
                BaseDeserilize<Memory::Heap, T>(stream, Object, registry);
            }

            template<typename T>
            ForceInline T SensitiveDeserilize(const Containers::Array<Byte>& stream, T& Object, UnusedData const TypesRegistry& registry) {
                BaseDeserilize<Memory::SensitiveDataHeap, T>(stream, Object, registry);
            }

            template<typename T>
            ForceInline SizeT SerilizedDataSize(const T& object, UnusedData const TypesRegistry& registry) {
                return Details::BaseSerlizationHandler<THeap, T>::DataSize(Object, registry);
            }

            template<typename THeap, typename T>
            ForceInline BaseString<THeap> BaseMetaDataGen() {
                return BaseString<THeap>(Details::BaseSerlizationHandler<THeap, T>::TypeMetaData() + CTTI::Details::CreateTypeStr("\0"));
            }

            template<typename T>
            ForceInline String MetaDataGen() {
                return String(Details::BaseSerlizationHandler<Memory::Heap, T>::TypeMetaData() + CTTI::Details::CreateTypeStr("\0"));
            }

            template<typename T>
            ForceInline SensitiveString SensitiveMetaDataGen() {
                return SensitiveString(Details::BaseSerlizationHandler<Memory::SensitiveDataHeap, T>::TypeMetaData() + CTTI::Details::CreateTypeStr("\0"));
            }

            template<typename T>
            ForceInline constexpr auto CTTIMetaDataGen() {
                return Details::BaseSerlizationHandler<Memory::Heap, T>::TypeMetaData() + CTTI::Details::CreateTypeStr("\0");
            }
        }
    }
}

#pragma warning(pop)