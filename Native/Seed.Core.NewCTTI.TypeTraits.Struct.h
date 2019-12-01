#pragma once
#include "Seed.Core.NewCTTI.TypeTraits.Base.h"
#include "Seed.Core.NewCTTI.TypeTraits.Collections.h"

namespace Seed
{
    inline namespace Core
    {
        namespace NewCTTI
        {
            namespace TypeTraits
            {
                

                template<typename Serilizable >
                struct ISerilizable;

                namespace Details
                {
                    template<typename T, typename TypeStringT, SizeT Offset>
                    struct FieldTypeData : CTTITypeData<T> {
                        static constexpr auto FieldName = NewCTTI::Details::TypeStringToArray(TypeStringT());
                        static constexpr SizeT FieldSize = sizeof(T);
                        static constexpr SizeT FieldOffset = Offset;
                        using Type = T;
                    };


                    template<typename T, typename ... TFields>
                    struct CustomStruct {
                        using Type = T;

                        static constexpr SizeT FieldsSize = [&] ()
                        {
                            SizeT result = 0; for (auto s : { TFields::FieldSize... }) result += s; return result;
                        }();

                        static constexpr SizeT FieldsCount = sizeof...(TFields);
                       
                        using Fields = typename Containers::Tuple<TFields...>;

                        static constexpr Boolean IsSerilizable = Seed::TypeTraits::OrPack(CTTITypeData<typename TFields::Type>::IsSerilizable...);
                    };

                    template<
                        typename T, 
                        bool TIsConst, 
                        bool TIsVolatile,
                        bool TIsSerilizable
                    >
                    struct CTTIStructTypeDataImpl
                        : BaseCTTITypeDataImpl<
                        T,
                        EType::Struct,
                        TIsSerilizable,
                        TIsConst,
                        TIsVolatile
                        > {};

                    template<typename T>
                    struct CTTITypeDataSpecialization<
                        T, 
                        typename Seed::Core::TypeTraits::EnableIfV<Seed::Core::TypeTraits::IsBasedOnV<ISerilizableBase, T>, void>
                    >
                        : CTTIStructTypeDataImpl<
                        typename Seed::TypeTraits::RemoveCV<T>, 
                        Seed::TypeTraits::IsConstV<T>, 
                        Seed::TypeTraits::IsVolatile<T>,
                        T::FieldMetaData::IsSerilizable
                        > 
                    {
                        using Fields = typename T::FieldMetaData::Fields;
                    };
                }

                template<typename Serilizable >
                struct ISerilizable: public ISerilizableBase {
                protected:
                    virtual Byte* GetDataPtr() const = 0;
                private:
                    template<typename T, typename Enable>
                    friend struct Details::CTTITypeDataSpecialization;

                    template<typename T, bool TIsConst, bool TIsVolatile, bool TIsSerilizable>
                    friend struct Details::CTTIStructTypeDataImpl;

					template<typename THeap, typename T, typename Enable>
					friend struct Seed::Core::NewSerilization::Details::BaseSerlizationHandler;
                public:
					virtual ~ISerilizable() = 0 {};
                };

#define DefStructFieldData(Struct, Name) \
    Seed::Core::NewCTTI::TypeTraits::Details::FieldTypeData<decltype(Struct::Name), GenNGTypeString(#Name) , StructOffsetOfMember(Struct, Name)>

#define DefStructMetaData(Struct, ...) \
using FieldMetaData = typename Seed::Core::NewCTTI::TypeTraits::Details::CustomStruct<Struct, __VA_ARGS__>

                class SerilizableClass : Seed::IDisposable, public ISerilizable<SerilizableClass> {
                private:
                    struct Data {
                        Seed::RefCountT RefCount;
                        String Name;
                        Containers::Map<String, String> Storage;
                        Data() : RefCount(1) {}
                    };
                private:
                    Data* This;
                public:
                    DefStructMetaData(Data,
                        DefStructFieldData(Data, Name),
                        DefStructFieldData(Data, Storage)
                    );
                private:
                    virtual Byte* GetDataPtr() const override {
                        return reinterpret_cast<Byte*>(This);
                    }
                protected:
                    virtual void Dispose() override {}
                public:
                    String& Name() {
                        return This->Name;
                    }
                    Containers::Map<String, String>& Storage() {
                        return This->Storage;
                    }
                public:
                    SerilizableClass() : This(new Data) {}
                public:
                    RefCountClass(SerilizableClass, This);
                };

				constexpr auto v2 = SerilizableClass::FieldMetaData::Fields::Count;
            }
        }
    }
}