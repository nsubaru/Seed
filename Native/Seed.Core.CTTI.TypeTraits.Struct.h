#pragma once
#include "Seed.Core.CTTI.TypeTraits.BaseTypes.h"
#include "Seed.Core.CTTI.TypeTraits.Collections.h"

namespace Seed
{
    inline namespace Core
    {
        namespace CTTI
        {
            namespace TypeTraits
            {
                template<typename Serilizable >
                struct ISerilizable;

                namespace Details
                {
                    template<typename TStruct, typename T, typename TypeStringT, SizeT TFieldOffset>
                    struct FieldTypeData : CTTITypeData<T> {
                        TypeStringT FieldName;
                        static constexpr SizeT FieldSize = sizeof(T);
                        static constexpr SizeT FieldOffset = TFieldOffset;
                        using Type = T;
                        using Struct = TStruct;

                        constexpr FieldTypeData(TypeStringT fieldName) : FieldName(fieldName){}

                        ForceInline constexpr const Type& Selector(const Struct& data) {
                            return *(Type*) (((Byte*) &data) + FieldOffset);
                        }
                    };


                    template<typename T, typename ... TFields>
                    struct CustomStruct {
                        using Type = T;

                        static constexpr SizeT FieldsSize = [] ()
                        {
                            SizeT result = 0; for (auto s : { TFields::FieldSize... }) result += s; return result;
                        }();

                        static constexpr SizeT FieldsCount = sizeof...(TFields);

                        using Fields = typename Containers::Tuple<TFields...>;

                        const Containers::Tuple<TFields...> DataFields;

                        constexpr CustomStruct(TFields... fields) : DataFields(fields...) {}

                        static constexpr Boolean IsSerilizable = [] () constexpr
                        {
                            return (CTTITypeData<typename TFields::Type>::IsSerilizable && ...);
                        }();
                    
                        ForceInline constexpr Containers::Tuple<typename TFields::Type...> ToTuple(const Type& data) const {
                            return DataFields.ConvertTo([&] (auto item)
                            {
                                return item.Selector(data);
                            });
                        };
                    };

                    template<typename TStruct, typename ... TFields>
                    constexpr auto MakeCustomStruct(TFields...fields) {
                        return CustomStruct<TStruct, TFields...>(fields...);
                    }

                    template<
                        typename T, 
                        bool TIsConst, 
                        bool TIsVolatile,
                        bool TIsSerilizable,
                        bool TIsConvertibleToTuple
                    >
                    struct CTTIObjectTypeDataImpl
                        : BaseCTTITypeDataImpl<
                        T,
                        EType::Object,
                        TIsSerilizable,
                        TIsConst,
                        TIsVolatile,
                        TIsConvertibleToTuple
                        > {};

                    template<typename T> requires Concepts::IsObjectCttiType<T>
                    struct CTTITypeDataSpecialization<T, void> : 
                        CTTIObjectTypeDataImpl<
                            typename Seed::TypeTraits::RemoveCV<T>, 
                            Seed::TypeTraits::IsConstV<T>, 
                            Seed::TypeTraits::IsVolatileV<T>,
                            T::FieldMetaData.IsSerilizable,
                            false
                        > 
                    {
                        static constexpr bool IsConvertibleToTuple = false;
                        static constexpr auto Fields = typename T::FieldMetaData.DataFields;
                    };
                }

                template<typename Serilizable >
                struct ISerilizable: public Details::ISerilizableGenericBase {
                protected:
                    virtual Byte* GetDataPtr() const = 0;
                private:
                    template<typename T, typename Enable>
                    friend struct Details::CTTITypeDataSpecialization;

                    template<typename T, bool TIsConst, bool TIsVolatile, bool TIsSerilizable, bool TIsConvertibleToTuple>
                    friend struct Details::CTTIObjectTypeDataImpl;

					template<typename THeap, typename T, typename Enable>
					friend struct Seed::Core::Serilization::Details::BaseSerlizationHandler;
                public:
					virtual ~ISerilizable() = 0 {};
                };

#define DefStructFieldData(Struct, Name) \
    Seed::Core::CTTI::TypeTraits::Details::FieldTypeData<\
        Struct, \
        decltype(Struct::Name), \
        decltype(Seed::Core::CTTI::Details::CreateTypeStr(L"" #Name)), \
        StructOffsetOfMember(Struct, Name) \
    >(Seed::Core::CTTI::Details::CreateTypeStr(L"" #Name))

#define DefStructMetaDataObj(Struct, ...) \
    Seed::Core::CTTI::TypeTraits::Details::MakeCustomStruct<Struct>(__VA_ARGS__)

#define DefStructMetaData(Struct, ...) \
static constexpr auto FieldMetaData = DefStructMetaDataObj(Struct, __VA_ARGS__) 

                
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
            }
        }
    }
}