#pragma once
#include "Seed.Core.NewCTTI.TypeTraits.Base.h"
#include "Seed.Core.Math.h"

namespace Seed
{
	inline namespace Core
	{
		namespace NewCTTI
		{
			namespace TypeTraits
			{
				namespace Details
				{
#pragma region Base struct implementation
					template<typename TNumeric, bool TIsConst, bool TIsVolatile>
					struct BaseNumericCTTITypeDataImpl : BaseCTTITypeDataImpl<TNumeric, EType::Numeric, true, TIsConst, TIsVolatile>
					{
						static constexpr TNumeric Max = Math::Max<TNumeric>();
						static constexpr TNumeric Min = Math::Min<TNumeric>();

						static constexpr Boolean IsSigned = Seed::TypeTraits::IsSigned<TNumeric>;
						static constexpr Boolean IsUnsigned = Seed::TypeTraits::IsUnsigned<TNumeric>;
					};

					template<typename TFloat, bool TIsConst, bool TIsVolatile>
					struct BaseFloatCTTITypeDataImpl : BaseCTTITypeDataImpl<TFloat, EType::FloatNumeric, true, TIsConst, TIsVolatile>
					{
						static constexpr TFloat NaN = static_cast<TFloat>(Math::NaN);
						static constexpr TFloat Infinity = Math::Infinity;
						static constexpr TFloat Epsilon = Math::Epsilon<TFloat>();

						static constexpr Boolean IsSigned = true;
					};
#pragma endregion
#pragma region Numeric specializations
					template<bool TIsConst, bool TIsVolatile> struct CTTITypeDataImpl<Int8, TIsConst, TIsVolatile> :BaseNumericCTTITypeDataImpl<Int8, TIsConst, TIsVolatile>
					{};

					template<bool TIsConst, bool TIsVolatile> struct CTTITypeDataImpl<Int16, TIsConst, TIsVolatile> :BaseNumericCTTITypeDataImpl<Int16, TIsConst, TIsVolatile>
					{};

					template<bool TIsConst, bool TIsVolatile> struct CTTITypeDataImpl<Int32, TIsConst, TIsVolatile> :BaseNumericCTTITypeDataImpl<Int32, TIsConst, TIsVolatile>
					{};

					template<bool TIsConst, bool TIsVolatile> struct CTTITypeDataImpl<Int64, TIsConst, TIsVolatile> :BaseNumericCTTITypeDataImpl<Int64, TIsConst, TIsVolatile>
					{};

					template<bool TIsConst, bool TIsVolatile> struct CTTITypeDataImpl<UInt8, TIsConst, TIsVolatile> :BaseNumericCTTITypeDataImpl<UInt8, TIsConst, TIsVolatile>
					{};

					template<bool TIsConst, bool TIsVolatile> struct CTTITypeDataImpl<UInt16, TIsConst, TIsVolatile> :BaseNumericCTTITypeDataImpl<UInt16, TIsConst, TIsVolatile>
					{};

					template<bool TIsConst, bool TIsVolatile> struct CTTITypeDataImpl<UInt32, TIsConst, TIsVolatile> :BaseNumericCTTITypeDataImpl<UInt32, TIsConst, TIsVolatile>
					{};

					template<bool TIsConst, bool TIsVolatile> struct CTTITypeDataImpl<UInt64, TIsConst, TIsVolatile> :BaseNumericCTTITypeDataImpl<UInt64, TIsConst, TIsVolatile>
					{};
#pragma endregion
#pragma region Float specialization
					template<bool TIsConst, bool TIsVolatile> struct CTTITypeDataImpl<Float, TIsConst, TIsVolatile> :BaseFloatCTTITypeDataImpl<Float, TIsConst, TIsVolatile>
					{};

					template<bool TIsConst, bool TIsVolatile> struct CTTITypeDataImpl<Double, TIsConst, TIsVolatile> :BaseFloatCTTITypeDataImpl<Double, TIsConst, TIsVolatile>
					{};
#pragma endregion
#pragma region Character
					template<bool TIsConst, bool TIsVolatile> struct CTTITypeDataImpl<UChar, TIsConst, TIsVolatile> : BaseCTTITypeDataImpl<UChar, EType::Character, true, TIsConst, TIsVolatile>
					{
						static constexpr auto CodePage = NewCTTI::Details::TypeStringToArray(GenNGTypeString("UTF-16LE")());

						static constexpr UChar Max = static_cast<UChar>(Math::Max<UInt16>());
						static constexpr UChar Min = static_cast<UChar>(Math::Max<UInt16>());

						using NumericSynonim = CTTITypeDataImpl<UInt16, TIsConst, TIsVolatile>;

						static_assert(sizeof(UInt16) == sizeof(UChar), "UChar have incorect size");
					};
#pragma endregion
#pragma region Boolean
					template<bool TIsConst, bool TIsVolatile> struct CTTITypeDataImpl<Boolean, TIsConst, TIsVolatile> : BaseCTTITypeDataImpl<Boolean, EType::Logic, true, TIsConst, TIsVolatile>
					{
						static constexpr Boolean LogicTrue = true;
						static constexpr Boolean LogicFalse = false;
					};
#pragma endregion
#pragma region RefCountT
					template<bool TIsConst, bool TIsVolatile> struct CTTITypeDataImpl<RefCountT, TIsConst, TIsVolatile> : BaseCTTITypeDataImpl<Boolean, EType::Numeric, true, TIsConst, TIsVolatile>
					{
						using TNumeric = RefCountT::CounterType;

						static constexpr TNumeric Max = Math::Max<TNumeric>();
						static constexpr TNumeric Min = Math::Min<TNumeric>();

						static constexpr Boolean IsSigned = Seed::TypeTraits::IsSigned<TNumeric>;
						static constexpr Boolean IsUnsigned = Seed::TypeTraits::IsUnsigned<TNumeric>;
					};
#pragma endregion
#pragma region Enumeration
					template<typename T, bool TIsConst, bool TIsVolatile> 
					struct CTTIEnumTypeDataImpl :
						BaseCTTITypeDataImpl<T, EType::Enumeration, true, TIsConst, TIsVolatile> {
						using UnderlyingType = typename Seed::Core::TypeTraits::EnumUnderlyingTypeV<T>;
						using UnderlyingTypeMetaData = CTTITypeMetaData<UnderlyingType>;
					};

					template<typename T>
                    struct CTTITypeDataSpecialization
                        <
                        T,
                        typename Seed::TypeTraits::EnableIfV<Seed::TypeTraits::IsEnumV<typename Seed::TypeTraits::RemoveCV<T>>, void>
                        >
                        : CTTIEnumTypeDataImpl<typename Seed::TypeTraits::RemoveCV<T>, Seed::TypeTraits::IsConstV<T>, Seed::TypeTraits::IsVolatileV<T>> {

                    };
#pragma endregion
#pragma region Date and DateTime
					template<bool TIsConst, bool TIsVolatile> 
					struct CTTITypeDataImpl<DateTime, TIsConst, TIsVolatile> : 
						BaseCTTITypeDataImpl<DateTime, EType::NtTime, true, TIsConst, TIsVolatile>
					{
						using NumericType = UInt64;
						using NumericTypeMetaData = CTTITypeMetaData<NumericType>;
					};

					template<bool TIsConst, bool TIsVolatile>
					struct CTTITypeDataImpl<Date, TIsConst, TIsVolatile> : 
						BaseCTTITypeDataImpl<Date, EType::NtTime, true, TIsConst, TIsVolatile>
					{
						using PlainTime = DateTime;
					};

					template<bool TIsConst, bool TIsVolatile>
					struct CTTITypeDataImpl<DateSpan, TIsConst, TIsVolatile> : 
						BaseCTTITypeDataImpl<DateSpan, EType::NtTime, true, TIsConst, TIsVolatile>
					{
						using PlainTime = DateTime;
					};
#pragma endregion
				}
			}
		}
	}
}