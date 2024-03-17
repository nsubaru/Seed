#pragma once
#include "Seed.Core.Serialization.Box.h"

namespace Seed::Core::Serialization
{
    /// <summary>
    /// Serialization handler structure
    /// </summary>
    /// <typeparam name="...T">Entry values</typeparam>
    template<typename TLeft, typename TRight> requires IsSerializableBox<Functionality::Either<TLeft, TRight>>
    struct SerializationHandler<Functionality::Either<TLeft, TRight>> {
        /// <summary>
        /// Serialization implementation template method
        /// </summary>
        /// <typeparam name="T1">First type</typeparam>
        /// <typeparam name="...TRest">Other optional types</typeparam>
        /// <typeparam name="TI">Number of types counter</typeparam>
        /// <param name="data">Data</param>
        /// <param name="rawStream">Raw stream</param>
        /// <param name="offset">Offset</param>
        template<SizeT TI, typename T1, typename ... TRest>
        static void SerializeImplT(const Functionality::Either<TLeft, TRight>& data, Byte* rawStream, SizeT& offset) {
            if (TI == data.HoldedTypeNumber)
            {
                SerializationHandler<T1>::SerializeImpl(data.GetByIdx<TI>(), rawStream, offset);
            }
            else if constexpr (sizeof...(TRest) != 0)
            {
                SerializeImplT<TI + 1, TRest...>(data, rawStream, offset);
            }
        }

        /// <summary>
        /// Serialization implementation method
        /// </summary>
        /// <param name="data">Data</param>
        /// <param name="rawStream">Raw stream</param>
        /// <param name="offset">Offset</param>
        static void SerializeImpl(const Functionality::Either<TLeft, TRight>& data, Byte* rawStream, SizeT& offset) {
            SerializationHandler<SizeT>::SerializeImpl(data.HoldedTypeNumber, rawStream, offset);
            
            SerializeImplT<0, TLeft, TRight>(data, rawStream, offset);
        }

        /// <summary>
        /// Data size calculation method
        /// </summary>
        /// <param name="data">Data</param>
        /// <returns>Data size</returns>
        static constexpr SizeT DataSize(const Functionality::Either<TLeft, TRight>& data) {
            return sizeof(SizeT) + DataSizeT<0, TLeft, TRight>(data);
        }

        /// <summary>
        /// Data size calculation template method
        /// </summary>
        /// <typeparam name="T1">First type</typeparam>
        /// <typeparam name="...TRest">Other optional types</typeparam>
        /// <typeparam name="TI">Number of types counter</typeparam>
        /// <param name="data">Data</param>
        /// <returns>Data size</returns>
        template<SizeT TI, typename T1, typename ...TRest>
        static constexpr SizeT DataSizeT(const Functionality::Either<TLeft, TRight>& data) {
            if (TI == data.HoldedTypeNumber)
            {
                return SerializationHandler<T1>::DataSize(data.GetByIdx<TI>());
            }
            else if constexpr (sizeof...(TRest) != 0)
            {
                return DataSizeT<TI + 1, TRest...>(data);
            }
            else
            {
                return 0;
            }
        }

        /// <summary>
        /// Getting the boolean value whether the type supports a copying by copying memory
        /// </summary>
        /// <returns>The boolean value whether the type supports a copying by copying memory</returns>
        static constexpr Boolean IsMemCopy() {
            return false;
        }

        /// <summary>
        /// Additional type information
        /// </summary>
        /// <returns>Additional type information</returns>
        static constexpr auto TypeHint() {
            return
                Core::Details::CreateTypeStr(L"");
        }

        /// <summary>
        /// Constant expression size
        /// </summary>
        /// <returns>Constant expression size</returns>
        static constexpr auto ConstexprSize() {
            return
                Core::Details::CreateTypeStr(L"0");
        }

        /// <summary>
        /// Deserialization implementation template method
        /// </summary>
        /// <typeparam name="T1">First type</typeparam>
        /// <typeparam name="...TRest">Other optional types</typeparam>
        /// <typeparam name="TI">Number of types counter</typeparam>
        /// <param name="data">Data</param>
        /// <param name="rawStream">Raw stream</param>
        /// <param name="offset">Offset</param>
        template<SizeT TI, typename T1, typename ... TRest>
        static void DeserializeImplT(SizeT i, Functionality::Either<TLeft, TRight>& data, const Byte* rawStream, SizeT& offset) {
            if (i == TI)
            {
                T1 v = T1();
                SerializationHandler<T1>::DeserializeImpl(v, rawStream, offset);
                data = v;
            }
            else if constexpr (sizeof...(TRest) != 0)
            {
                DeserializeImplT<TI + 1, TRest...>(i, data, rawStream, offset);
            }
        }

        /// <summary>
        /// Deserialization implementation method
        /// </summary>
        /// <param name="data">Data</param>
        /// <param name="rawStream">Raw stream</param>
        /// <param name="offset">Offset</param>
        static void DeserializeImpl(Functionality::Either<TLeft, TRight>& data, const Byte* rawStream, SizeT& offset) {
            SizeT i = {};
            SerializationHandler<SizeT>::DeserializeImpl(i, rawStream, offset);
            
            DeserializeImplT<0, TLeft, TRight>(i, data, rawStream, offset);
        }

        /// <summary>
        /// Additional type information fields implementation
        /// </summary>
        /// <typeparam name="T1">First type</typeparam>
        /// <typeparam name="...TRest">Other optional types</typeparam>
        /// <typeparam name="I">Number of types counter</typeparam>
        /// <returns>Additional type information fields</returns>
        template<SizeT I, typename T1, typename ... TRest>
        constexpr static auto TypeHintFieldsImp() {
            if constexpr (sizeof...(TRest) != 0)
            {
                return
                    Core::Details::CreateTypeStr(L"[") +
                    Core::TypeTraits::ToCharStrEx<SizeT, I>() +
                    Core::Details::CreateTypeStr(L"-") +
                    CTTI::TypeName<T1>() +
                    Core::Details::CreateTypeStr(L"]")
                    +
                    TypeHintFieldsImp<I + 1, TRest...>();
            }
            else
            {
                return
                    Core::Details::CreateTypeStr(L"[") +
                    Core::TypeTraits::ToCharStrEx<SizeT, I>() +
                    Core::Details::CreateTypeStr(L"-") +
                    CTTI::TypeName<T1>() +
                    Core::Details::CreateTypeStr(L"]");
            }
        }

        /// <summary>
        /// Additional type metadata fields implementation
        /// </summary>
        /// <typeparam name="T1">First type</typeparam>
        /// <typeparam name="...TRest">Other optional types</typeparam>
        /// <returns>Additional type metadata fields</returns>
        template<typename T1, typename ... TRest>
        constexpr static auto TypeMetaDataFieldsImp() {
            if constexpr (sizeof...(TRest) != 0)
            {
                return SerializationHandler<T1>::TypeMetaData()
                    +
                    TypeMetaDataFieldsImp<TRest...>();
            }
            else
            {
                return SerializationHandler<T1>::TypeMetaData();
            }
        }

        /// <summary>
        /// Type metadata fields
        /// </summary>
        /// <returns>Type metadata fields</returns>
        static constexpr auto TypeMetaData() {
            return
                Core::Details::CreateTypeStr(L"{") +
                CTTI::TypeName<Functionality::Either<TLeft, TRight>>() +
                Core::Details::CreateTypeStr(L"|") +
                Core::Details::ToCharStr<static_cast<SizeT>(CTTI::TypeTraits::CTTITypeMetaData<Functionality::Either<TLeft, TRight>>::TypeClass)>() +
                Core::Details::CreateTypeStr(L"|") +
                Core::Details::ToCharStr<2>() +
                Core::Details::CreateTypeStr(L"|") +
                ConstexprSize() +
                Core::Details::CreateTypeStr(L"|") +
                TypeHintFieldsImp<0, TLeft, TRight>() +
                Core::Details::CreateTypeStr(L"|") +
                TypeMetaDataFieldsImp<TLeft, TRight>() +
                Core::Details::CreateTypeStr(L"}");
        }

        /// <summary>
        /// Type metadata fields record getter
        /// </summary>
        /// <typeparam name="THeap">Target heap</typeparam>
        /// <typeparam name="T1">First type</typeparam>
        /// <typeparam name="...TRest">Other optional types</typeparam>
        /// <returns>Type metadata fields record</returns>
        template<typename THeap, typename T1, typename ... TRest>
        static Containers::BaseList<THeap, BaseTypeMetaDataRecord<THeap>> GetFieldsTypeMetaDataRecord() {
            return Containers::BaseList<THeap, BaseTypeMetaDataRecord<THeap>>{SerializationHandler<T1>::template GetTypeMetaDataRecord<THeap>() }
                +=
                    GetFieldsTypeMetaDataRecord<THeap, TRest...>();
        }

        /// <summary>
        /// Type metadata fields record getter
        /// </summary>
        /// <typeparam name="THeap">Target heap</typeparam>
        /// <typeparam name="T1">First type</typeparam>
        /// <returns>Type metadata fields record</returns>
        template<typename THeap, typename T1>
        static Containers::BaseList<THeap, BaseTypeMetaDataRecord<THeap>> GetFieldsTypeMetaDataRecord() {
            return Containers::BaseList<THeap, BaseTypeMetaDataRecord<THeap>>{SerializationHandler<T1>::template GetTypeMetaDataRecord<THeap>() };
        }

        /// <summary>
        /// Type metadata fields record getter
        /// </summary>
        /// <typeparam name="THeap">Target heap</typeparam>
        /// <returns>Type metadata fields record</returns>
        template<typename THeap>
        static BaseTypeMetaDataRecord<THeap> GetTypeMetaDataRecord() {
            return BaseTypeMetaDataRecord<THeap>(
                CTTI::TypeName<Functionality::Either<TLeft, TRight>>(),
                CTTI::TypeTraits::CTTITypeMetaData<Functionality::Either<TLeft, TRight>>::TypeClass,
                sizeof(Functionality::Either<TLeft, TRight>),
                0,
                TypeHint(),
                GetFieldsTypeMetaDataRecord<THeap, TLeft, TRight>()
            );
        }
    };
}