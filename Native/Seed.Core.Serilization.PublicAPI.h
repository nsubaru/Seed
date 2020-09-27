#pragma once

#include "Seed.Core.Serilization.Base.h"

#include "Seed.Core.Serilization.TypeMetaRecord.h"

#include "Seed.Core.Serilization.Core.h"
#include "Seed.Core.Serilization.IntegratedTypes.h"
#include "Seed.Core.Serilization.Composits.h"
#include "Seed.Core.Serilization.Collection.h"
#include "Seed.Core.Serilization.Box.h"
#include "Seed.Core.Serilization.Struct.h"
#include "Seed.Core.Serilization.MetaData.h"

namespace Seed
{
    inline namespace Core
    {
        namespace Serilization
        {
            template<typename THeap, typename T>
            ForceInline Containers::BaseArray<THeap, Byte> BaseSerilize(const T& Object, UnusedData const TypesRegistry& registry) {
                SizeT rawStreamSize = Details::BaseSerlizationHandler<THeap, T>::DataSize(Object, registry);
                Byte* rawStream = nullptr;
                try
                {
                    rawStream = new Byte[rawStreamSize];
                    SizeT offset = 0;
                    Details::BaseSerlizationHandler<THeap, T>::SerilizeImpl(Object, rawStream, offset, registry);
                }
                catch (...)
                {
                    delete[] rawStream;
                    ReThrow;
                }
                return Containers::BaseArray<THeap, Byte>(MoveRValue(rawStream), rawStreamSize);
            }

            template<typename THeap, typename T>
            ForceInline void BaseDeserilize(const Containers::BaseArray<THeap, Byte>& stream, T& Object, UnusedData const TypesRegistry& registry) {
                const Byte* rawStream = (const Byte*)stream;
                SizeT offset = 0;
                Details::BaseSerlizationHandler<THeap, T>::DeserelizeImpl(Object, rawStream, offset, registry);
            }

            template<typename T>
            ForceInline Containers::Array<Byte> Serilize(const T& Object, UnusedData const TypesRegistry& registry) {
                return BaseSerilize<Memory::Heap, T>(Object, registry);
            }

            template<typename T>
            ForceInline Containers::SensitiveArray<Byte> SensitiveSerilize(const T& Object, UnusedData const TypesRegistry& registry) {
                return BaseSerilize<Memory::SensitiveDataHeap, T>(Object, registry);
            }

            template<typename T>
            ForceInline void Deserilize(const Containers::Array<Byte>& stream, T& Object, UnusedData const TypesRegistry& registry) {
                BaseDeserilize<Memory::Heap, T>(stream, Object, registry);
            }

            template<typename T>
            ForceInline T SensitiveDeserilize(const Containers::Array<Byte>& stream, T& Object, UnusedData const TypesRegistry& registry) {
                BaseDeserilize<Memory::SensitiveDataHeap, T>(stream, Object, registry);
            }

            template<typename T>
            ForceInline SizeT SerilizedDataSize(const T& object, UnusedData const TypesRegistry& registry) {
                return Details::BaseSerlizationHandler<THeap, T>::DataSize(Object, registry);
            }

            template<typename THeap, typename T>
            ForceInline BaseString<THeap> BaseMetaDataGen() {
                return BaseString<THeap>(Details::BaseSerlizationHandler<THeap, T>::TypeMetaData());
            }

            template<typename T>
            ForceInline String MetaDataGen() {
                return String(Details::BaseSerlizationHandler<Memory::Heap, T>::TypeMetaData());
            }

            template<typename T>
            ForceInline SensitiveString SensitiveMetaDataGen() {
                return SensitiveString(Details::BaseSerlizationHandler<Memory::SensitiveDataHeap, T>::TypeMetaData());
            }

            template<typename T>
            ForceInline constexpr auto CTTIMetaDataGen() {
                return Details::BaseSerlizationHandler<Memory::Heap, T>::TypeMetaData();
            }

            template<typename THeap, typename T>
            ForceInline constexpr BaseTypeMetaDataRecord<THeap> GetBaseTypeRecordForType() {
                return Details::BaseSerlizationHandler<THeap, T>::GetTypeMetaDataRecord();
            }

            template<typename T>
            ForceInline constexpr TypeMetaDataRecord GetTypeRecordForType() {
                return Details::BaseSerlizationHandler<Memory::Heap, T>::GetTypeMetaDataRecord();
            }

            template<typename T>
            ForceInline constexpr SensitiveTypeMetaDataRecord GetSensitiveTypeRecordForType() {
                return Details::BaseSerlizationHandler<Memory::SensitiveDataHeap, T>::GetTypeMetaDataRecord();
            }
        }
    }
}