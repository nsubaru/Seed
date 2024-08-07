#pragma once
#include "Seed.Core.Serialization.Custom.Base.h"

#include "Seed.Core.Crypt.Algorithms.Totp.h"

namespace Seed::Core::Serialization
{
	/// <summary>
	/// A base serialization handler structure
	/// </summary>
	template<typename THashProvider>
	struct SerializationHandler<Crypt::Algorithms::TOTP<THashProvider>> {
		/// <summary>
		/// Serialization implementstion
		/// </summary>
		/// <param name="data">Data</param>
		/// <param name="rawStream">Raw stream</param>
		/// <param name="offset">Offset</param>
		/// <param name="helper">Additional serialization helper</param>
		static void SerializeImpl(const Crypt::Algorithms::TOTP<THashProvider>& data, Byte* rawStream, SizeT& offset, SerializatorHelper helper) {
			SerializationHandler<decltype(data.This->secretHash)>::SerializeImpl(data.This->secretHash, rawStream, offset, helper);
			SerializationHandler<decltype(data.This->passwordSize)>::SerializeImpl(data.This->passwordSize, rawStream, offset, helper);
			SerializationHandler<decltype(data.This->startTime)>::SerializeImpl(data.This->startTime, rawStream, offset, helper);
			SerializationHandler<decltype(data.This->timeIntervalInSeconds)>::SerializeImpl(data.This->timeIntervalInSeconds, rawStream, offset, helper);
		}

		/// <summary>
		/// Deserialization implementstion
		/// </summary>
		/// <param name="data">Data</param>
		/// <param name="rawStream">Raw stream</param>
		/// <param name="offset">Offset</param>
		/// <param name="helper">Additional serialization helper</param>
		static void DeserializeImpl(Crypt::Algorithms::TOTP<THashProvider>& data, const Byte* rawStream, SizeT& offset, SerializatorHelper helper) {
			data = Crypt::Algorithms::TOTP<THashProvider>();

			SerializationHandler<decltype(data.This->secretHash)>::DeserializeImpl(data.This->secretHash, rawStream, offset, helper);
			SerializationHandler<decltype(data.This->passwordSize)>::DeserializeImpl(data.This->passwordSize, rawStream, offset, helper);
			SerializationHandler<decltype(data.This->startTime)>::DeserializeImpl(data.This->startTime, rawStream, offset, helper);
			SerializationHandler<decltype(data.This->timeIntervalInSeconds)>::DeserializeImpl(data.This->timeIntervalInSeconds, rawStream, offset, helper);
		}

		/// <summary>
		/// Update dynamic type full type id
		/// </summary>
		/// <param name="rawStream">Serialized stream</param>
		/// <param name="typeMap">Full type ids map</param>
		/// <param name="offset">Data location in stream</param>
		/// <param name="helper">Optional serialization helper</param>
		static UInt64 UpdateImpl(Byte* rawStream, const UpdateTypeMap& typeMap, SizeT& offset, SerializatorHelper helper) {
			UInt64 changesCount = 0;

			changesCount += SerializationHandler<decltype(Crypt::Algorithms::TOTP<THashProvider>().This->secretHash)>::UpdateImpl(rawStream, typeMap, offset, helper);
			changesCount += SerializationHandler<decltype(Crypt::Algorithms::TOTP<THashProvider>().This->passwordSize)>::UpdateImpl(rawStream, typeMap, offset, helper);
			changesCount += SerializationHandler<decltype(Crypt::Algorithms::TOTP<THashProvider>().This->startTime)>::UpdateImpl(rawStream, typeMap, offset, helper);
			changesCount += SerializationHandler<decltype(Crypt::Algorithms::TOTP<THashProvider>().This->timeIntervalInSeconds)>::UpdateImpl(rawStream, typeMap, offset, helper);

			return changesCount;
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
		static constexpr SizeT DataSize(const Crypt::Algorithms::TOTP<THashProvider>& data, SerializatorHelper helper) {
			return
				SerializationHandler<decltype(data.This->secretHash)>::DataSize(data.This->secretHash, helper) +
				SerializationHandler<decltype(data.This->passwordSize)>::DataSize(data.This->passwordSize, helper) +
				SerializationHandler<decltype(data.This->startTime)>::DataSize(data.This->startTime, helper) +
				SerializationHandler<decltype(data.This->timeIntervalInSeconds)>::DataSize(data.This->timeIntervalInSeconds, helper);
		}

		/// <summary>
		/// Returns the additional type information
		/// </summary>
		/// <returns>Additional type information</returns>
		static constexpr auto TypeHint() {
			return
				Core::Details::CreateTypeStr(L"CRYPT");
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
				CTTI::TypeName<Crypt::Algorithms::TOTP<THashProvider>>() +
				Core::Details::CreateTypeStr(L"|") +
				Core::Details::ToCharStr<static_cast<SizeT>(CTTI::TypeTraits::CTTITypeMetaData<Crypt::Algorithms::TOTP<THashProvider>>::TypeClass)>() +
				Core::Details::CreateTypeStr(L"|") +
				Core::Details::ToCharStr<sizeof(Crypt::Algorithms::TOTP<THashProvider>)>() +
				Core::Details::CreateTypeStr(L"|") +
				ConstexprSize() +
				Core::Details::CreateTypeStr(L"|") +
				TypeHint() +
				Core::Details::CreateTypeStr(L"|") +
				Core::Details::CreateTypeStr(L"}");
		}

		/// <summary>
		/// Receiving the type metadata record
		/// </summary>
		/// <typeparam name="THeap">Heap type</typeparam>
		/// <returns>Type metadata record</returns>
		template<typename THeap>
		static BaseTypeMetaDataRecord<THeap> GetTypeMetaDataRecord() {
			return BaseTypeMetaDataRecord<THeap>(
				CTTI::TypeName<Crypt::Algorithms::TOTP<THashProvider>>(),
				CTTI::TypeTraits::CTTITypeMetaData<Crypt::Algorithms::TOTP<THashProvider>>::TypeClass,
				sizeof(Crypt::Algorithms::TOTP<THashProvider>),
				0,
				TypeHint(),
				{}
			);
		}
	};
}