#pragma once
#include "Seed.Core.CTTI.TypeTraits.Base.h"
#include <typeinfo>

template<typename T>
struct __attributes : Seed::Core::TypeTraits::LogicFalse{
    constexpr static auto AttributesList() { return Seed::Core::Containers::Tuple<>(); };
};

namespace Seed::Core::CTTI::TypeTraits
{
    enum class EAttributeTargetType : UInt8 {
        Type, Method, Function, Field, Uknown
    };

    /// <summary>
    /// Possible attribute value types
    /// </summary>
    template<typename T>
    concept ITAttributeValue = 
        Core::TypeTraits::IsStandardLayout<T> ||
        Core::TypeTraits::HasConstexprDefaultCtorV<T> ||
        Core::Concepts::IsSame<Char, typename Core::TypeTraits::IsConstexprArrayT<T>::ItemType>;

    template<auto TAttributeTarget, auto TAttributeName, auto TAttributeValue>
    struct AtrributeCttiRecord {
        constexpr static auto Target = TAttributeTarget();
        constexpr static auto Name = TAttributeName();
        constexpr static auto Value = TAttributeValue();

        constexpr static EAttributeTargetType GetTargetType()
        {
            if constexpr (Core::TypeTraits::IsMemberFunctionPointerV<Core::TypeTraits::RemoveConst<decltype(Target)>>)
            {
                return EAttributeTargetType::Method;
            }
            else if constexpr (Core::TypeTraits::IsFunction<Core::TypeTraits::RemoveConst<decltype(Target)>>::State)
            {
                return EAttributeTargetType::Function;
            }
            else if constexpr (Core::TypeTraits::IsSameV<Core::TypeTraits::RemoveConst<decltype(Target)>, NullptrT>)
            {
                return EAttributeTargetType::Type;
            }
            else if constexpr (Core::Concepts::IsSame<Char, typename Core::TypeTraits::IsConstexprArrayT<Core::TypeTraits::RemoveConst<decltype(Target)>>::ItemType>)
            {
                return EAttributeTargetType::Field;
            }
            else 
            {
                return EAttributeTargetType::Uknown;
            }
        }

        static_assert(GetTargetType() != EAttributeTargetType::Uknown);

        static_assert(Core::Concepts::IsSame<Char, typename Core::TypeTraits::IsConstexprArrayT<Core::TypeTraits::RemoveConst<decltype(Name)>>::ItemType>);

        static_assert(ITAttributeValue<Core::TypeTraits::RemoveConst<decltype(Value)>>);

        constexpr AtrributeCttiRecord() = default;
        constexpr AtrributeCttiRecord(const AtrributeCttiRecord&) = default;
        constexpr AtrributeCttiRecord(AtrributeCttiRecord&&) = default;
        constexpr AtrributeCttiRecord& operator=(const AtrributeCttiRecord&) = default;
        constexpr AtrributeCttiRecord& operator=(AtrributeCttiRecord&&) = default;
    };

    template<typename T>
    consteval auto GetAttributes() requires requires{ { T::__attributes };   }
    {
        return  T::__attributes();
    }

    template<typename T>
    consteval auto GetAttributes() requires (!requires{ { T::__attributes }; })
    {
        return  __attributes<T>::AttributesList();
    }
}

#define CttiClassAttributeRecord(AttributeName, AttributeValue) Seed::Core::CTTI::TypeTraits::AtrributeCttiRecord< \
 [] () constexpr { return nullptr; }, \
 [] () constexpr { return Seed::Core::TypeTraits::CreateTypeStr(AttributeName); }, \
 [] () constexpr { return AttributeValue; }>

#define CttiMethodAttributeRecord(AttributeTarget, AttributeName, AttributeValue) Seed::Core::CTTI::TypeTraits::AtrributeCttiRecord< \
 [] () constexpr { return &AttributeTarget; }, \
 [] () constexpr { return Seed::Core::TypeTraits::CreateTypeStr(AttributeName); }, \
 [] () constexpr { return AttributeValue; }>

#define CttiExtendedMethodAttributeRecord(AttributeTargetType, AttributeTarget, AttributeName, AttributeValue) Seed::Core::CTTI::TypeTraits::AtrributeCttiRecord< \
 [] () constexpr { return (AttributeTargetType) &AttributeTarget; }, \
 [] () constexpr { return Seed::Core::TypeTraits::CreateTypeStr(AttributeName); }, \
 [] () constexpr { return AttributeValue; }>

#define CttiFunctionAttributeRecord(AttributeTarget, AttributeName, AttributeValue) Seed::Core::CTTI::TypeTraits::AtrributeCttiRecord< \
 [] () constexpr { return &AttributeTarget; }, \
 [] () constexpr { return Seed::Core::TypeTraits::CreateTypeStr(AttributeName); }, \
 [] () constexpr { return AttributeValue; }>

#define CttiFieldAttributeRecord(AttributeTarget, AttributeName, AttributeValue) Seed::Core::CTTI::TypeTraits::AtrributeCttiRecord< \
 [] () constexpr { return Seed::Core::TypeTraits::CreateTypeStr(AttributeTarget); }, \
 [] () constexpr { return Seed::Core::TypeTraits::CreateTypeStr(AttributeName); }, \
 [] () constexpr { return AttributeValue; }>

#define CttiClassAttributes(...) constexpr static auto __attributes() { return Seed::Core::Containers::Tuple<__VA_ARGS__>(); }

#define CttiAttributeList(Target, ...)  \
template<>\
struct __attributes<Target> : Seed::Core::TypeTraits::LogicTrue { \
    constexpr static auto AttributesList() { return Seed::Core::Containers::Tuple<__VA_ARGS__>(); } \
}