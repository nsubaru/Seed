#pragma once
#include "Seed.Core.Stage1.h"

namespace Seed {
	inline namespace Core {
		namespace Serlization {
			enum ETypeClass {
				Numeric,
				Character,
				String,
				Container,
				AssociativeContainer,
				PodStruct,
				Struct,
				Unknown,
				UnknownContainer
			};

			template<typename T,typename Enable = void>
			struct TypeMetaData
			{
				using Type = T;
				static constexpr bool IsDynamic = true;
				static constexpr ETypeClass TypeClass = ETypeClass::Unknown;
				static constexpr SizeT Size = sizeof(T);
			};

			template<typename T>
			struct TypeMetaData<T, typename TypeTraits::EnableIfTV<TypeTraits::IsSameT<TypeTraits::Signed<T>, T>, bool>>
			{
				using Type = T;
				static constexpr bool IsDynamic = TypeTraits::IsPodV<T>;
				static constexpr ETypeClass TypeClass = ETypeClass::Numeric;
				static constexpr SizeT Size = sizeof(T);
			};

			template<typename T>
			struct TypeMetaData<T, typename TypeTraits::EnableIfTV<TypeTraits::IsSameT<TypeTraits::Unsigned<T>, T>, bool>>
			{
				using Type = T;
				using SubType = void;
				static constexpr bool IsDynamic = TypeTraits::IsPodV<T>;
				static constexpr ETypeClass TypeClass = ETypeClass::Numeric;
				static constexpr SizeT Size = sizeof(T);
			};

			template<typename T>
			struct IsContainer : TypeTraits::LogicFalse {};

			template<template <typename THeap, typename T> typename TContainer, typename THeap, typename T>
			struct IsContainer<TContainer<THeap, T>> : TypeTraits::LogicTrue
			{
				using Container = TContainer;
				using Type = T;
				static constexpr ETypeClass ContainerClass = ETypeClass::UnknownContainer;
			};

			template<typename THeap>
			struct IsContainer<BaseString<THeap>> : TypeTraits::LogicTrue
			{
				using Container = BaseString;
				using Type = UChar;
				static constexpr ETypeClass ContainerClass = ETypeClass::String;
			};

			template<typename THeap, typename T>
			struct IsContainer<BaseList<THeap, T>> : TypeTraits::LogicTrue
			{
				using Container = BaseList;
				using Type = T;
				static constexpr ETypeClass ContainerClass = ETypeClass::Container;
			};

			template<typename T>
			struct IsContainer<Array<T>> : TypeTraits::LogicTrue
			{
				using Container = Array;
				using Type = T;
				static constexpr ETypeClass ContainerClass = ETypeClass::Container;
			};


			template<typename T>
			struct TypeMetaData<T, typename TypeTraits::EnableIfTV<IsContainer<T>, bool>>
			{
				using Type = typename IsContainer<T>::Container;
				using SubType = typename IsContainer<T>::IsConType;
				static constexpr bool IsDynamic = true;
				static constexpr ETypeClass TypeClass = IsContainer<T>::ContainerClass;
				static constexpr SizeT Size = sizeof(T);

				using SubTypeMetaData = TypeMetaData<SubType>;
			};

			template<typename T>
			struct TypeMetaData<T, typename TypeTraits::EnableIfTV<TypeTraits::IsPodT<T>, bool>>
			{
				using Type = T;
				static constexpr bool IsDynamic = false;
				static constexpr ETypeClass TypeClass = ETypeClass::PodStruct;
				static constexpr SizeT Size = sizeof(T);
			};

			template<typename T>
			class IReceiveable {
				static Array<Byte> ReceiveFormat;
			private:
				virtual static void InitReceiveFormat(Array<Byte>&& format) = 0;
			public:
				virtual static Array<Byte> SerilizeImpl();
			public:
				virtual ~IReceiveable() = 0 {};
			public:
				friend Array<Byte> Serilize(const T& Data);
			};

			template<typename T>
			class ISendable {
				static Array<Byte> SendFormat;
			private:
				virtual static void InitSendFormat(Array<Byte>&& format) = 0;
			public:
				virtual static T DeSerilize(conts Array<Byte>);
			public:
				virtual ~ISendable() = 0 {};
			public:
				friend T Deserilise(const Array<Byte>& Data);
			};

			template<typename T>
			class ISeriliable : public IReceiveable<T>, public ISendable<T> {
			};

			struct Header {
				Char MAGIC[12];
				ETypeClass TypeClass;
				SizeT TypeNameSize;
				SizeT DataSize;
			};

			template<typename T>
			Array<Byte> Serilize(const T& Data) {
				if constexpr (TypeMetaData<T>::TypeClass == ETypeClass::Numeric) {
					SizeT TypeNameSize = 
					struct Data : public Header {
						T Data;
					};
				}
			}

			template<typename T>
			T Deserilise(const Array<Byte>& Data) {

			}
		}
	}
}