// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Seed.Core.Threading.Thread.h"
#include "Seed.Core.Serilization.h"

#include "Seed.Core.Testing.Tests.MultiThreading.ProducerAndConsumer.h"


namespace Seed
{
	class Processing final : IDisposable {
    private:
        struct Data {
            struct SharedData {
                Containers::ConcurencyPriorityQueue<UInt8, String> queue;
                RefCountT createdCount;
                RefCountT removedCount;
                SharedData() : createdCount(0), removedCount(0) {}
            };

            SharedData sharedData;

            SizeT itemsCount;
            RefCountT RefCount;

            Data(SizeT itemsCount) : itemsCount(itemsCount), RefCount(1) {}
        };
    private:
        Data* This;
    protected:
        void Dispose() override {}
    public:
        Processing(SizeT itemsCount) : This(new Data(itemsCount)) {}

        RefCountClass(Processing, This);

        void Creating() const
        {
            for (SizeT i = 0; i < This->itemsCount; i++)
            {
                This->sharedData.queue.Insert(static_cast<UInt8>((This->itemsCount / 100) * i), String::toString(i));
                ++This->sharedData.createdCount;
            }
        }

        void Removing() const
        {
            for (SizeT i = 0; i < This->itemsCount; i++) {
                This->sharedData.queue.Dequeue();
                ++This->sharedData.removedCount;
            }
        }

		/// <summary>
		/// Гетер поля RemovedCount з значення This->sharedData.removedCount
		/// </summary>
		/// <returns>Поточне значення</returns>
        SizeT GetRemovedCount() const { return This->sharedData.removedCount; }

        /// <summary>
		/// Властивість RemovedCount
		/// </summary>
        SizeT Property(GetRemovedCount) RemovedCount;

		/// <summary>
		/// Гетер поля
		/// </summary>
		/// <returns>Поточне значення</returns>
        SizeT GetCreatedCount() const { return This->sharedData.createdCount; }

        /// <summary>
		/// Властивість CreatedCount
		/// </summary>
        SizeT Property(GetCreatedCount) CreatedCount;
    };
}

bool Seed::Testing::Test::MultiThreading::ProducerAndConcumer::Excecute()
{
    constexpr SizeT itemCountPerThread = 10000;
	Processing processing(itemCountPerThread);

	Threading::Thread creatingThread1(L"PriorityQueue Test: Creating Thread 1", Delegate([](Processing& processing) {processing.Creating(); }, processing));
	Threading::Thread creatingThread2(L"PriorityQueue Test: Creating Thread 2", Delegate([](Processing& processing) {processing.Creating(); }, processing));
	Threading::Thread deletingThread1(L"PriorityQueue Test: Deleting Thread 1", Delegate([](Processing& processing) {processing.Removing(); }, processing));
	Threading::Thread deletingThread2(L"PriorityQueue Test: Deleting Thread 2", Delegate([](Processing& processing) {processing.Removing(); }, processing));

	creatingThread1.Start();
    creatingThread2.Start();
    deletingThread1.Start();
    deletingThread2.Start();
	
    creatingThread1.Join();
    creatingThread2.Join();
    deletingThread1.Join();
    deletingThread2.Join();

	return processing.RemovedCount == processing.CreatedCount;
}
