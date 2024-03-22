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
        /// Serialization implementation method
        /// </summary>
        /// <param name="data">Data</param>
        /// <param name="rawStream">Raw stream</param>
        /// <param name="offset">Offset</param>
        static void SerializeImpl(const Functionality::Either<TLeft, TRight>& data, Byte* rawStream, SizeT& offset)
        {
            SerializationHandler<SizeT>::SerializeImpl(data.HoldedTypeNumber, rawStream, offset);

            if (data.HoldedTypeNumber == 0)
            {
                SerializationHandler<TLeft>::SerializeImpl(data.GetAs<TLeft>(), rawStream, offset);
            }
            else
            {
                SerializationHandler<TRight>::SerializeImpl(data.GetAs<TRight>(), rawStream, offset);
            }
        }

        /// <summary>
        /// Data size calculation method
        /// </summary>
        /// <param name="data">Data</param>
        /// <returns>Data size</returns>
        static constexpr SizeT DataSize(const Functionality::Either<TLeft, TRight>& data) {
            return sizeof(SizeT) + (data.HoldedTypeNumber == 0
                ? SerializationHandler<TLeft>::DataSize(data.GetAs<TLeft>())
                : SerializationHandler<TRight>::DataSize(data.GetAs<TRight>()));
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
        /// Deserialization implementation method
        /// </summary>
        /// <param name="data">Data</param>
        /// <param name="rawStream">Raw stream</param>
        /// <param name="offset">Offset</param>
        static void DeserializeImpl(Functionality::Either<TLeft, TRight>& data, const Byte* rawStream, SizeT& offset) {
            SizeT i = {};
            SerializationHandler<SizeT>::DeserializeImpl(i, rawStream, offset);
            
            if (i == 0)
            {
                TLeft obj = {};

                SerializationHandler<TLeft>::DeserializeImpl(obj, rawStream, offset);

                data = obj;
            }
            else
            {
                TRight obj = {};

                SerializationHandler<TRight>::DeserializeImpl(obj, rawStream, offset);

                data = obj;
            }
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