#pragma once
#include "Seed.Core.Serilization.Base.h"
#include "Seed.Core.Tokenizer.h"

namespace Seed
{
    inline namespace Core
    {
        namespace Serilization
        {
            /// <summary>
            /// Опис схеми типу
            /// </summary>
            template<typename THeap>
            class BaseTypeMetaDataRecord final : public Memory::Allocator<THeap>, protected IDisposable {
            private:
                struct Data : public Memory::Allocator<THeap> {
                    BaseString<THeap> TypeName;
                    CTTI::EType Type;
                    SizeT SizeOfSize;
                    SizeT StaticSize;
                    BaseString<THeap> AdditionalInfo;
                    Containers::BaseList<THeap, BaseTypeMetaDataRecord> NestedTypes;

                    RefCountT RefCount;

                    Data(
                        BaseString<THeap>&& TypeName,
                        CTTI::EType Type,
                        SizeT SizeOfSize,
                        SizeT StaticSize,
                        BaseString<THeap>&& AdditionalInfo,
                        Containers::BaseList<THeap, BaseTypeMetaDataRecord>&& NestedTypes
                    ) :
                        TypeName(MoveRValue(TypeName)),
                        Type(Type),
                        SizeOfSize(SizeOfSize),
                        StaticSize(StaticSize),
                        AdditionalInfo(MoveRValue(AdditionalInfo)),
                        NestedTypes(MoveRValue(NestedTypes)),
                        RefCount(1)
                    {}

                    Data(
                        BaseString<THeap>&& TypeName,
                        CTTI::EType Type,
                        SizeT SizeOfSize,
                        SizeT StaticSize,
                        BaseString<THeap>&& AdditionalInfo
                    ) :
                        TypeName(MoveRValue(TypeName)),
                        Type(Type),
                        SizeOfSize(SizeOfSize),
                        StaticSize(StaticSize),
                        AdditionalInfo(MoveRValue(AdditionalInfo)),
                        NestedTypes(),
                        RefCount(1)
                    {}
                };
            private:
                Data* This;
            protected:
                virtual void Dispose() override {}
            public:
                /// <summary>
                /// Стандартний конструктор, містить дані про тип пустоти
                /// </summary>
                BaseTypeMetaDataRecord() : This(new Data(
                    BaseString<THeap>(CTTI::TypeTraits::CTTITypeMetaData<void>::TypeName),
                    CTTI::TypeTraits::CTTITypeMetaData<void>::TypeClass,
                    0,
                    0,
                    BaseString<THeap>(L"N\\A")
                )) {}

                /// <summary>
                /// Конструктор з даними типу і його підтипів
                /// </summary>
                /// <param name="TypeName">Ім'я типу</param>
                /// <param name="TypeClass">Класс типу</param>
                /// <param name="SizeOfSize">Розмір в пам'яті об'єкту типу без вкладених типів</param>
                /// <param name="StaticSize">Статичний розмір типу або 0, якщо тип є динамічним або містить динамічні поля</param>
                /// <param name="AdditionalInfo">Додаткова інформація якщо вона є, або пустия рядок якщо ні</param>
                /// <param name="NestedTypes">Інформація про вкладені типи</param>
                BaseTypeMetaDataRecord(
                    BaseString<THeap>&& TypeName,
                    CTTI::EType TypeClass,
                    SizeT SizeOfSize,
                    SizeT StaticSize,
                    BaseString<THeap>&& AdditionalInfo,
                    Containers::BaseList<THeap, BaseTypeMetaDataRecord>&& NestedTypes
                ) : This(new Data(MoveRValue(TypeName), TypeClass, SizeOfSize, StaticSize, MoveRValue(AdditionalInfo), MoveRValue(NestedTypes))) {}

                /// <summary>
                /// Конструктор з даними типу
                /// </summary>
                /// <param name="TypeName">Ім'я типу</param>
                /// <param name="TypeClass">Класс типу</param>
                /// <param name="SizeOfSize">Розмір в пам'яті об'єкту типу без вкладених типів</param>
                /// <param name="StaticSize">Статичний розмір типу або 0, якщо тип є динамічним або містить динамічні поля</param>
                /// <param name="AdditionalInfo">Додаткова інформація якщо вона є, або пустия рядок якщо ні</param>
                BaseTypeMetaDataRecord(
                    BaseString<THeap>&& TypeName,
                    CTTI::EType TypeClass,
                    SizeT SizeOfSize,
                    SizeT StaticSize,
                    BaseString<THeap>&& AdditionalInfo
                ) : This(new Data(MoveRValue(TypeName), TypeClass, SizeOfSize, StaticSize, MoveRValue(AdditionalInfo))) {}

                /// <summary>
                /// Конструктор з даними типу без додаткової інформації
                /// </summary>
                /// <param name="TypeName">Ім'я типу</param>
                /// <param name="TypeClass">Класс типу</param>
                /// <param name="SizeOfSize">Розмір в пам'яті об'єкту типу без вкладених типів</param>
                /// <param name="StaticSize">Статичний розмір типу або 0, якщо тип є динамічним або містить динамічні поля</param>
                BaseTypeMetaDataRecord(
                    BaseString<THeap>&& TypeName,
                    CTTI::EType TypeClass,
                    SizeT SizeOfSize,
                    SizeT StaticSize
                ) : This(new Data(MoveRValue(TypeName), TypeClass, SizeOfSize, StaticSize, String())) {}
            public:
                /// <summary>
                /// Ім'я типу
                /// </summary>
                /// <returns>Ім'я типу</returns>
                const BaseString<THeap>& GetTypeName() const {
                    return This->TypeName;
                }

                /// <summary>
                /// Класс типу
                /// </summary>
                /// <returns>Класс типу</returns>
                CTTI::EType GetTypeClass() const {
                    return This->Type;
                }

                /// <summary>
                /// Розмір в пам'яті об'єкту типу без вкладених типів
                /// </summary>
                /// <returns>Розмір в пам'яті об'єкту типу без вкладених типів</returns>
                SizeT GetSizeOfSize() const {
                    return This->SizeOfSize;
                }

                /// <summary>
                /// Статичний розмір типу або 0, якщо тип є динамічним або містить динамічні поля
                /// </summary>
                /// <returns>Статичний розмір типу або 0, якщо тип є динамічним або містить динамічні поля</returns>
                SizeT GetStaticSize() const {
                    return This->StaticSize;
                }

                /// <summary>
                /// Додаткова інформація якщо вона є, або пустия рядок якщо ні
                /// </summary>
                /// <returns>Додаткова інформація якщо вона є, або пустия рядок якщо ні</returns>
                const BaseString<THeap>& GetAdditionalInfo() const {
                    return This->AdditionalInfo;
                }

                /// <summary>
                /// Інформація про вкладені типи, якщо вони є, інакше список пустий
                /// </summary>
                /// <returns>Інформація про вкладені типи, якщо вони є, інакше список пустий</returns>
                const Containers::BaseList<THeap, BaseTypeMetaDataRecord>& GetNestedTypes() const {
                    return This->NestedTypes;
                }
            public:
                /// <summary>
                /// Ім'я типу
                /// </summary>
                BaseString<THeap> Property(GetTypeName) TypeName;

                /// <summary>
                /// Класс типу
                /// </summary>
                CTTI::EType Property(GetTypeClass) TypeClass;

                /// <summary>
                /// Розмір в пам'яті об'єкту типу без вкладених типів
                /// </summary>
                SizeT Property(GetSizeOfSize) SizeOfSize;

                /// <summary>
                /// Статичний розмір типу або 0, якщо тип є динамічним або містить динамічні поля
                /// </summary>
                SizeT Property(GetStaticSize) StaticSize;

                /// <summary>
                /// Додаткова інформація якщо вона є, або пустия рядок якщо ні
                /// </summary>
                BaseString<THeap> Property(GetAdditionalInfo) AdditionalInfo;

                /// <summary>
                /// Інформація про вкладені типи, якщо вони є, інакше список пустий
                /// </summary>
                Containers::BaseList<THeap, BaseTypeMetaDataRecord> Property(GetNestedTypes) NestedTypes;
            private:
                SizeT GetMetaDataMaxStringSize() const {
                    
                    constexpr auto GetTypeNameCharacterTypeSize = 
                        CTTI::TypeTraits::CTTITypeMetaData<typename Core::TypeTraits::RemoveCVRefV<decltype(GetTypeName())>>::CharacterTypeMetaData::Size;
                    constexpr auto GetGetAdditionalInfoCharacterTypeSize =
                        CTTI::TypeTraits::CTTITypeMetaData<typename Core::TypeTraits::RemoveCVRefV<decltype(GetAdditionalInfo())>>::CharacterTypeMetaData::Size;

                    constexpr auto BracketSize = sizeof(L'{') + sizeof(L'}');
                    constexpr auto DelimiterSize = sizeof(L'|');
                    constexpr auto ETypeValueMaxSize =
                        CTTI::Details::ToCharStr<CTTI::TypeTraits::CTTITypeMetaData<CTTI::EType>::UnderlyingTypeMetaData::Max>().Count;
                    constexpr auto SizeTValueMaxSize = 
                        CTTI::Details::ToCharStr<CTTI::TypeTraits::CTTITypeMetaData<SizeT>::Max>().Count;
                    
                    return BracketSize +
                        (TypeName.Size * sizeof(GetTypeNameCharacterTypeSize)) + DelimiterSize +
                        ETypeValueMaxSize + DelimiterSize +
                        SizeTValueMaxSize + DelimiterSize +
                        SizeTValueMaxSize + DelimiterSize +
                        (AdditionalInfo.Size * sizeof(GetGetAdditionalInfoCharacterTypeSize)) + DelimiterSize +
                        NestedTypes.Aggregate<SizeT>(ArrowFunct(value, item, value + item.GetMetaDataMaxStringSize()));
                }
            private:
                void ToStringImpl(BaseString<THeap>& buffer) const {
                    buffer += L'{';
                    buffer += this->TypeName;
                    buffer += L'|';
                    buffer += BaseString<THeap>::toString(this->TypeClass);
                    buffer += L'|';
                    buffer += BaseString<THeap>::toString(this->SizeOfSize);
                    buffer += L'|';
                    buffer += BaseString<THeap>::toString(this->StaticSize);    
                    buffer += L'|';
                    buffer += AdditionalInfo;
                    buffer += L'|';
                    this->NestedTypes.ForEach(ArrowFunctWithClosure(item, item.ToStringImpl(buffer)));
                    buffer += L'}';
                }
            public:
                BaseString<THeap> ToString() const {
                    BaseString<THeap> buffer = BaseString<THeap>(GetMetaDataMaxStringSize());
                    ToStringImpl(buffer);
                    return MoveRValue(buffer);
                }
            private:
                static BaseTypeMetaDataRecord DecodeTypeMetaDataRecord(typename BaseTokenizer<THeap>::Token& token) {
                    auto typeName = token.View.Value;
                    ++token;
                    auto typeClass = token.View.Value.toEnum<CTTI::EType>();
                    ++token;
                    auto sizeOfSize = token.View.Value.toInt<SizeT>();
                    ++token;
                    auto staticSize = token.View.Value.toInt<SizeT>();
                    ++token;
                    auto additionalInformation = token.View.Value;
                    if (token.View.Delimiter == L"}")
                        return BaseTypeMetaDataRecord(MoveRValue(typeName), typeClass, sizeOfSize, staticSize, MoveRValue(additionalInformation));
                    else {
                        ++token;
                        auto nestedTypes = Containers::BaseList<THeap, BaseTypeMetaDataRecord>();
                        do {
                            nestedTypes += DecodeTypeMetaDataRecord(token);
                        } while (token.View.Delimiter == L"{");
                        ++token;
                        return BaseTypeMetaDataRecord(MoveRValue(typeName), typeClass, sizeOfSize, staticSize, MoveRValue(additionalInformation));
                    }
                }
            public:
                static BaseTypeMetaDataRecord DecodeTypeMetaDataRecord(const BaseString<THeap>& value) {
                    auto tokenizer = BaseTokenizer<THeap>(value, { L"{", L"|", L"}" });
                    auto token = tokenizer.begin();
                    return DecodeTypeMetaDataRecord(token);
                }
            public:
                RefCountClass(BaseTypeMetaDataRecord, This);
            };

            namespace {
                using TypeMetaDataRecord = BaseTypeMetaDataRecord<Memory::Heap>;
            }

            namespace {
                using SensitiveTypeMetaDataRecord = BaseTypeMetaDataRecord<Memory::SensitiveDataHeap>;
            }
        }

        namespace CTTI {
            namespace TypeTraits {
                namespace Details {
                    template<typename THeap, bool TIsConst, bool TIsVolatile>
                    struct CTTITypeDataImpl<Serilization::BaseTypeMetaDataRecord<THeap>, TIsConst, TIsVolatile> : 
                        BaseHeapCTTITypeData<Serilization::BaseTypeMetaDataRecord<THeap>, THeap, EType::MetaData, true, TIsConst, TIsVolatile>
                    {
                        using StringType = BaseString<THeap>;
                        using StringTypeMetaData = CTTITypeMetaData<StringType>;
                    };
                }
            }
        }
    }
}