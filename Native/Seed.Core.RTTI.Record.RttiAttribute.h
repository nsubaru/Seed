#pragma once
#include "Seed.Core.RTTI.Record.Base.h"
#include "Seed.Core.RTTI.Record.RttiIterator.h"

namespace Seed::Core::RTTI::Record
{
	/// <summary>
	/// A base RTTI attribute class
	/// </summary>
	/// <typeparam name="THeap">Heap type</typeparam>
	template<typename THeap>
	class BaseRttiAttribute final : public Memory::Allocator<THeap> {
		BaseRttiIterator<THeap, BaseRttiAttribute> it;
	public:
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="it">Iterator</param>
		BaseRttiAttribute(BaseRttiIterator<THeap, BaseRttiAttribute> it) : it(it) {}

	private:
		ForceInline const MetaData::AttributeRecord* GetRecord() const {
			return (const MetaData::AttributeRecord*) it.item;
		}

	public:
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="from">Source</param>
		BaseRttiAttribute(const BaseRttiAttribute& from) : it(from.it) {}

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="from">RValue reference to a source</param>
		BaseRttiAttribute(BaseRttiAttribute&& from) : it(MoveRValue(from.it)) {}

		/// <summary>
		/// Assignment operator
		/// </summary>
		/// <param name="from">Source</param>
		/// <returns>Assigned value</returns>
		BaseRttiAttribute& operator=(const BaseRttiAttribute& from) {
			this->~BaseRttiAttribute();
			this->BaseRttiAttribute::BaseRttiAttribute(from);
			return *this;
		}

		/// <summary>
		/// Assignment operator
		/// </summary>
		/// <param name="from">RValue reference to a source</param>
		/// <returns>Assigned value</returns>
		BaseRttiAttribute& operator=(BaseRttiAttribute&& from) {
			this->~BaseRttiAttribute();
			this->BaseRttiAttribute::BaseRttiAttribute(MoveRValue(from));
			return *this;
		}

		/// <summary>
		/// Destructor
		/// </summary>
		~BaseRttiAttribute() {
			it.~BaseRttiIterator();
		}

	public:
		/// <summary>
		/// Returns atrtibute target type
		/// </summary>
		/// <returns>Attribute target type</returns>
		CTTI::TypeTraits::EAttributeTargetType GetAttributeTarget() const { return GetRecord()->AttributeTargetType; }

		/// <summary>
		/// Returns attribute name
		/// </summary>
		/// <returns>Attribute name</returns>
		ConstString GetAttributeName() const { return ConstString(GetRecord()->AttributeName.Text,GetRecord()->AttributeName.Size); }

	public:
		/// <summary>
		/// Receiving the field record with type ID
		/// </summary>
		/// <returns>Record with type ID</returns>
		SizeT GetTypeId() const {
			return ((SizeT) GetRecord()->ValueTypeId.TypeId) + it.imageBase;
		}

		/// <summary>
		/// Receiving the field record with full type ID
		/// </summary>
		/// <returns>Record with type ID</returns>
		UniversalTypeIdentifier GetFullTypeId() const {
			return GetRecord()->ValueTypeId;
		}

		/// <summary>
		/// Receiving the field record with binary size
		/// </summary>
		/// <returns>Record with binary size</returns>
		SizeT GetBinarySize() const {
			return GetRecord()->ValueTypeInformation.BinarySize;
		}

		/// <summary>
		/// Receiving the field record with type name
		/// </summary>
		/// <returns>Record with type name</returns>
		ConstString GetTypeName() const {
			auto rawData = Memory::UnSafe::AddOffsetAs<MetaData::TextMetaData>(GetRecord(), GetRecord()->OffsetToAttributeValueTypeName);

			return ConstString(rawData->Text, rawData->Size);
		}

		/// <summary>
		/// Receiving the field record with type information
		/// </summary>
		/// <returns>Record with type information</returns>
		CxxTypeInfoT GetTypeInfo() const {
			if (it.isValidCxxTypeId)
			{
				auto f = (CxxTypeInfoT(*)())(GetRecord()->ValueTypeId.TypeId + it.imageBase);
				return f();
			}
			else
			{
				return nullptr;
			}
		}

		/// <summary>
		/// Receiving the field type record
		/// </summary>
		/// <returns>Fielf type record</returns>
		BaseRttiTypeRecord<THeap> GetAttributeValueTypeRecord() const;

	public:
		/// <summary>
		/// Attribute target type
		/// </summary>
		CTTI::TypeTraits::EAttributeTargetType Property(GetAttributeTarget) AttributeTarget;

		/// <summary>
		/// Attribute name
		/// </summary>
		ConstString Property(GetAttributeName) AttributeName;

		/// <summary>
		/// Attribute value type id
		/// </summary>
		SizeT Property(GetTypeId) TypeId;

		/// <summary>
		/// Attribute value full type id
		/// </summary>
		UniversalTypeIdentifier Property(GetFullTypeId) FullTypeId;

		/// <summary>
		/// Attribute value binary size
		/// </summary>
		SizeT Property(GetBinarySize) BinarySize;

		/// <summary>
		/// Attribute value type name
		/// </summary>
		ConstString Property(GetTypeName) TypeName;

		/// <summary>
		/// Attribute value Cxx type info
		/// </summary>
		CxxTypeInfoT Property(GetTypeInfo) TypeInfo;

	public:
		/// <summary>
		/// Returns attribute value as T
		/// </summary>
		/// <typeparam name="T">Target type</typeparam>
		/// <returns>Attribute value or exception if type incorrect</returns>
		template<typename T> requires CTTI::TypeTraits::ITAttributeValue<T> || Concepts::IsSame<ConstString, T>
		T GetAttributeValueAs() const { 
			// Use type name as it will peristent accross compilations
			if (ConstString(CTTI::TypeName<T>()) != this->GetTypeName())
			{
				Throw(InvalidArgumentException(SourcePosition, nameof(T), L"Invalid type"));
			}

			if constexpr (Concepts::IsSame<T, ConstString>)
			{
				auto rawData = Memory::UnSafe::AddOffsetAs<MetaData::TextMetaData>(GetRecord(), GetRecord()->OffsetToAttributeValue);

				return ConstString(rawData->Text, rawData->Size);
			}
			else
			{
				return *Memory::UnSafe::AddOffsetAs<T>(GetRecord(), GetRecord()->OffsetToAttributeValue);
			}
		}

	private:
		static BaseRttiCollection<THeap, BaseRttiAttribute> AsAttributesCollection(const MetaData::Collection* record, bool isValidCxxTypeId, SizeT imageBase) {
			return BaseRttiCollection<THeap, BaseRttiAttribute>(record, isValidCxxTypeId, imageBase);
		}

	private:
		friend class Internals::RttiInternals;

		template<typename UHeap>
		friend class BaseRttiProperty;

		template<typename UHeap>
		friend class BaseRttiMethodRecord;

		template<typename UHeap>
		friend class BaseRttiTypeRecord;

		template<typename THeap>
		friend class BaseRttiField;
	};
}