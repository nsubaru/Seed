#pragma once
#include "Seed.Core.Base.Concepts.h"

namespace Seed::Core
{
	/// <summary>
    /// Enum bit flag validator type
	/// </summary>
	/// <typeparam name="TValueType">Enum bit flag enum underlying type</typeparam>
	template<Concepts::Integer TValueType>
    using EnumBitFlagValidator = bool(*)(TValueType value);

	/// <summary>
    /// Default enum bit flag validator
	/// </summary>
	/// <typeparam name="TValueType">Enum bit flag enum underlying type</typeparam>
	template<Concepts::Integer TValueType>
    constexpr auto EnumBitFlagDefaultValidator = [] (TValueType) { return true; };

	/// <summary>
    /// Enum bit flags, a set of flags where enum values are used as flags values
	/// </summary>
	/// <typeparam name="TEnum">Enum type</typeparam>
	/// <typeparam name="TValueType">Integer or bool type that used for storing flags value</typeparam>
	/// <typeparam name="TfIsValid">Additional validator for validating that flags applicable</typeparam>
	template<typename TEnum, Concepts::Integer TValueType = TypeTraits::EnumUnderlyingTypeV<TEnum>, EnumBitFlagValidator<TValueType> TfIsValid = EnumBitFlagDefaultValidator<TValueType>>
	struct EnumBitFlags {
	private:
		TValueType value;

	private:
		constexpr EnumBitFlags(TValueType value) : value(value) {}

	public:
		/// <summary>
		/// An empty constructor which initializes zero value
		/// </summary>
		constexpr EnumBitFlags() : value(0) {}

		/// <summary>
		/// A constructor with possible rights value
		/// </summary>
		/// <param name="value">Rights value</param>
		constexpr EnumBitFlags(TEnum value) : value(static_cast<TValueType>(value)) {}

	public:
		constexpr EnumBitFlags(const EnumBitFlags&) = default;
		constexpr EnumBitFlags& operator=(const EnumBitFlags&) = default;

	public:
		/// <summary>
		/// Rights addition operator to a current object
		/// </summary>
		/// <param name="keyHandleAccess">Additional rights</param>
		/// <returns>A reference to a current object</returns>
		constexpr EnumBitFlags& operator|=(TEnum keyHandleAccess)
		{
			this->value |= static_cast<TValueType>(keyHandleAccess);
			return *this;
		}

		/// <summary>
		/// Rights addition operator to a current object
		/// </summary>
		/// <param name="keyHandleAccess">Additional rights</param>
		/// <returns>A reference to a current object</returns>
		constexpr EnumBitFlags& operator|=(EnumBitFlags keyHandleAccess)
		{
			this->value |= keyHandleAccess.value;
			return *this;
		}

		/// <summary>
		/// Rights sum operator on a registry object
		/// </summary>
		/// <param name="left">Left operand</param>
		/// <param name="right">Right operand</param>
		/// <returns>New object</returns>
		constexpr friend EnumBitFlags operator|(EnumBitFlags left, EnumBitFlags right)
		{
			return EnumBitFlags(left.value | right.value);
		}

		/// <summary>
		/// Rights sum operator on a registry object
		/// </summary>
		/// <param name="left">Left operand</param>
		/// <param name="right">Right operand</param>
		/// <returns>New object</returns>
		constexpr friend EnumBitFlags operator|(TEnum left, EnumBitFlags right)
		{
			return EnumBitFlags(static_cast<TValueType>(left) | right.value);
		}

		/// <summary>
		/// Rights sum operator on a registry object
		/// </summary>
		/// <param name="left">Left operand</param>
		/// <param name="right">Right operand</param>
		/// <returns>New object</returns>
		constexpr friend EnumBitFlags operator|(EnumBitFlags left, TEnum right)
		{
			return EnumBitFlags(left.value | static_cast<TValueType>(right));
		}

		/// <summary>
		/// Rights sum operator on a registry object
		/// </summary>
		/// <param name="left">Left operand</param>
		/// <param name="right">Right operand</param>
		/// <returns>New object</returns>
		constexpr friend EnumBitFlags operator|(EnumBitFlags left, EnumBitFlags right);

	public:
		/// <summary>
		/// Returns the current list of rights as a bit mask
		/// </summary>
		/// <returns>Set of bits which indicates on an active rights</returns>
		constexpr UInt64 GetValue()
		{
			return value;
		}

	public:
		/// <summary>
		/// Checking if the specified right is active
		/// </summary>
		/// <param name="level">A right which is need to be checked</param>
		/// <returns>The logical value</returns>
		constexpr bool operator&(TEnum level)
		{
			return this->value & static_cast<TValueType>(level);
		}

	public:
		/// <summary>
		/// A current list of rights as a bit mask
		/// </summary>
		UInt64 Property(GetValue) Value;

	public:
		/// <summary>
		/// Checking if active rights on an object is allowable
		/// </summary>
		/// <returns>The logical value</returns>
		constexpr bool GetIsValid() const
		{
			return TfIsValid(this->value);
		}

	public:
		/// <summary>
		/// Indicates whether current rights on a registry object is allowable
		/// </summary>
		bool Property(GetIsValid) IsValid;
	};
}

namespace Seed::Core::TypeTraits 
{
	template<typename T>
	struct IsEnumBitFlagsT : LogicFalse {};

	template<typename T, typename TValue, auto TfIsValid>
	struct IsEnumBitFlagsT<EnumBitFlags<T, TValue, TfIsValid>> : LogicTrue {};

	template<typename T>
	constexpr Boolean IsEnumBitFlagsV = IsEnumBitFlagsT<T>::State;
}