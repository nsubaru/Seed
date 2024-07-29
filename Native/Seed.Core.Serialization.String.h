#pragma once
#include "Seed.Core.Serialization.Core.h"

#pragma warning(push)
// This internal implementation file which can use internal API functions
#pragma warning(disable: 4996)

namespace Seed::Core::Serialization
{
	template<typename T>
	constexpr bool IsSerializableString = CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::EType::String
		&& CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerializable;

	/// <summary>
	/// A base serialization handler structure
	/// </summary>
	template<typename TStringHeap>
	struct SerializationHandler<BaseString<TStringHeap>> {
		using CharacterType = typename CTTI::TypeTraits::CTTITypeMetaData<BaseString<TStringHeap>>::Character;

		/// <summary>
		/// Serialization implementstion
		/// </summary>
		/// <param name="data">Data</param>
		/// <param name="rawStream">Raw stream</param>
		/// <param name="offset">Offset</param>
		/// <param name="helper">Additional serialization helper</param>
		static void SerializeImpl(const BaseString<TStringHeap>& data, Byte* rawStream, SizeT& offset, SerializatorHelper helper) {
			SizeT count = data.FullLength;

			if (count <= 1)
			{
				count = 0;
				SerializationHandler<SizeT>::SerializeImpl(count, rawStream, offset, helper);
				return;
			}

			SerializationHandler<SizeT>::SerializeImpl(count, rawStream, offset, helper);

			Memory::Copy(reinterpret_cast<const Byte*>(const_cast<const CharacterType*>(static_cast<const CharacterType* const>(data))),
						rawStream + offset,
						count * sizeof(CharacterType));

			offset += count * sizeof(CharacterType);
		}

		/// <summary>
		/// Update dynamic type full type id
		/// </summary>
		/// <param name="rawStream">Serialized stream</param>
		/// <param name="typeMap">Full type ids map</param>
		/// <param name="offset">Data location in stream</param>
		/// <param name="helper">Optional serialization helper</param>
		static void UpdateImpl(Byte* rawStream, const UpdateTypeMap& typeMap, SizeT& offset, SerializatorHelper helper) {
			auto tmpOffset = offset;
			SizeT count = 0;

			SerializationHandler<SizeT>::UpdateImpl(rawStream, typeMap, tmpOffset, helper);
			SerializationHandler<SizeT>::DeserializeImpl(count, rawStream, offset, helper);

			offset += count;
		};

		/// <summary>
		/// Deserialization implementstion
		/// </summary>
		/// <param name="data">Data</param>
		/// <param name="rawStream">Raw stream</param>
		/// <param name="offset">Offset</param>
		/// <param name="helper">Additional serialization helper</param>
		static void DeserializeImpl(BaseString<TStringHeap>& data, const Byte* rawStream, SizeT& offset, SerializatorHelper helper) {
			SizeT count = 0;
			SerializationHandler<SizeT>::DeserializeImpl(count, rawStream, offset, helper);

			if (count != 0)
			{
				data = BaseString<TStringHeap>(
					reinterpret_cast<CharacterType*>(const_cast<Byte*>(rawStream + offset)), // Text ptr
					count - 1 // Text ptr
				);

				offset += count * sizeof(CharacterType);
			}
			else
			{
				data = BaseString<TStringHeap>();
			}
		}

		/// <summary>
		/// Indicates, is it possible to copy the item without any additional processing
		/// </summary>
		/// <returns>"True" if it is possible to copy the item without any additional processing</returns>
		static constexpr Boolean IsMemCopy() {
			return false;
		}

		/// <summary>
		/// Receiving the data dize
		/// </summary>
		/// <param name="data">RTTI section data</param>
		/// <param name="helper">Additional serialization helper</param>
		/// <returns>Data size</returns>
		static constexpr SizeT DataSize(const BaseString<TStringHeap>& data, SerializatorHelper helper) {
			return data.Size.Bytes + sizeof(SizeT);
		}

		/// <summary>
		/// Returns the additional type information
		/// </summary>
		/// <returns>Additional type information</returns>
		static constexpr auto TypeHint() {
			return
				CTTI::TypeTraits::CTTITypeMetaData<BaseString<TStringHeap>>::CodePage;
		}

		/// <summary>
		/// Returns the compile-time size of type
		/// </summary>
		/// <returns>Compile-time size of type</returns>
		static constexpr auto ConstexprSize() {
			return
				Core::Details::CreateTypeStr(L"0");
		}

		/// <summary>
		/// Returns the runtime information for serialization
		/// </summary>
		/// <returns>Runtime information for serialization</returns>
		static constexpr auto TypeMetaData() {
			return
				Core::Details::CreateTypeStr(L"{") +
				CTTI::TypeName<BaseString<TStringHeap>>() +
				Core::Details::CreateTypeStr(L"|") +
				Core::Details::ToCharStr<static_cast<SizeT>(CTTI::TypeTraits::CTTITypeMetaData<BaseString<TStringHeap>>::TypeClass)>() +
				Core::Details::CreateTypeStr(L"|") +
				Core::Details::ToCharStr<sizeof(BaseString<TStringHeap>)>() +
				Core::Details::CreateTypeStr(L"|") +
				ConstexprSize() +
				Core::Details::CreateTypeStr(L"|") +
				TypeHint() +
				Core::Details::CreateTypeStr(L"|") +
				SerializationHandler<CharacterType>::TypeMetaData() +
				Core::Details::CreateTypeStr(L"}");
		}

		/// <summary>
		/// Receiving the type metadata record
		/// </summary>
		/// <typeparam name="THeap">Heap type</typeparam>
		/// <returns>Type metadata record</returns>
		template<typename THeap>
		static BaseTypeMetaDataRecord<THeap> GetTypeMetaDataRecord() {
			return TypeMetaDataRecord(
				CTTI::TypeName<BaseString<TStringHeap>>(),
				CTTI::TypeTraits::CTTITypeMetaData<BaseString<TStringHeap>>::TypeClass,
				sizeof(BaseString<TStringHeap>),
				0,
				TypeHint(),
				{ SerializationHandler<CharacterType>::template GetTypeMetaDataRecord<THeap>() }
			);
		}
	};

	/// <summary>
	/// A base serialization handler structure
	/// </summary>
	template<typename THeap>
	struct SerializationHandler<BaseUniString<THeap>> {
		/// <summary>
		/// Serialization implementstion
		/// </summary>
		/// <param name="data">Data</param>
		/// <param name="rawStream">Raw stream</param>
		/// <param name="offset">Offset</param>
		/// <param name="helper">Additional serialization helper</param>
		static void SerializeImpl(const BaseUniString<THeap>& data, Byte* rawStream, SizeT& offset, SerializatorHelper helper) {
			if (!data.IsConstMode)
			{
				SerializationHandler<BaseString<THeap>>::SerializeImpl(data.dynamicText, rawStream, offset, helper);

				return;
			}

			const ConstString& text = data.staticText;

			if (text.Length == 0)
			{
				auto val = 0Sz;
				SerializationHandler<SizeT>::SerializeImpl(val, rawStream, offset, helper);

				return;
			}

			auto val = text.Length + 1;
			SerializationHandler<SizeT>::SerializeImpl(val, rawStream, offset, helper);

			Memory::Copy(text.Str, rawStream + offset, text.Length * sizeof(Char)); offset += text.Length * sizeof(Char);
			Memory::Copy(L'\0', rawStream + offset, sizeof(Char)); offset += sizeof(Char);
		}

		/// <summary>
		/// Update dynamic type full type id
		/// </summary>
		/// <param name="rawStream">Serialized stream</param>
		/// <param name="typeMap">Full type ids map</param>
		/// <param name="offset">Data location in stream</param>
		/// <param name="helper">Optional serialization helper</param>
		static void UpdateImpl(Byte* rawStream, const UpdateTypeMap& typeMap, SizeT& offset, SerializatorHelper helper) {
			auto tmpOffset = offset;
			SizeT count = 0;
			SerializationHandler<SizeT>::UpdateImpl(rawStream, typeMap, tmpOffset, helper);
			SerializationHandler<SizeT>::DeserializeImpl(count, rawStream, offset, helper);

			offset += count;
		};

		/// <summary>
		/// Deserialization implementstion
		/// </summary>
		/// <param name="data">Data</param>
		/// <param name="rawStream">Raw stream</param>
		/// <param name="offset">Offset</param>
		/// <param name="helper">Additional serialization helper</param>
		static void DeserializeImpl(BaseUniString<THeap>& data, const Byte* rawStream, SizeT& offset, SerializatorHelper helper) {
			SizeT count = 0;
			SerializationHandler<SizeT>::DeserializeImpl(count, rawStream, offset, helper);

			if (count != 0)
			{
				BaseString<THeap> str = BaseString<THeap>(
					reinterpret_cast<Char*>(const_cast<Byte*>(rawStream + offset)), // Text ptr
					count - 1 // Text length (without nullterminator)
				);

				data = BaseUniString<THeap>(str);
				offset += count * sizeof(Char);
			}
			else
			{
				data = BaseUniString<THeap>();
			}
		}

		/// <summary>
		/// Indicates, is it possible to copy the item without any additional processing
		/// </summary>
		/// <returns>"True" if it is possible to copy the item without any additional processing</returns>
		static constexpr Boolean IsMemCopy() {
			return false;
		}
	};
}

#pragma warning(pop)