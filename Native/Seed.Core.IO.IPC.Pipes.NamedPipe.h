#pragma once
#include "Seed.Core.IO.IPC.Pipes.Base.h"

namespace Seed {
	inline namespace Core {
		namespace IO {
			namespace IPC {
				namespace Pipes {
					/// <summary>
					/// Режими роботи коналу 
					/// </summary>
					enum class PipeMode : UInt8 {
						In = 0b01, Out = 0b10, Both = 0b11
					};

					/// <summary>
					/// Інтерфейс каналу
					/// </summary>
					class INamedPipe : protected IDisposable {
					public:
						/// <summary>
						/// Зчитує дані з каналу в буфер
						/// </summary>
						/// <param name="data">Буфер даних</param>
						virtual void ReadRaw(Containers::Blob& data) = 0;

						/// <summary>
						/// Запис дані в канал з буферу
						/// </summary>
						/// <param name="rawData">Буфер даних</param>
						virtual void WriteRawData(const Containers::Blob& rawData) = 0;

						/// <summary>
						/// Вказує чи канал робочий
						/// </summary>
						/// <returns>Логічне значення, true, якщо канал робочий</returns>
						virtual Boolean IsValid() const = 0;

						/// <summary>
						/// Закриває канал якщо він робочий, інакше нічого не робить
						/// </summary>
						virtual void Close() = 0;

						/// <summary>
						/// Очікування закритя каналу 
						/// </summary>
						virtual void WaitClosing() const = 0;

						/// <summary>
						/// Деструктор
						/// </summary>
						virtual ~INamedPipe() = 0 {};
					};

					/// <summary>
					/// Клієнт каналу
					/// </summary>
					class NamedPipeClient final : public INamedPipe {
					private:
						struct Data {
							PipeMode mode;
							Handle pipe;
							Threading::SRWLock inputLock;
							Threading::SRWLock ouputLock;
							String name;
							RefCountT RefCount;

							Data(
								Handle pipe,
								PipeMode mode,
								String&& name
							) :
								pipe(pipe),
								mode(mode),
								inputLock(),
								ouputLock(),
								name(MoveRValue(name)),
								RefCount(1) {
							};
						};

						Data* This;
					private:
						NamedPipeClient(Data*&& This) :This(This) {};
					protected:
						virtual void Dispose() override;
					public:
						/// <summary>
						/// Пустий конструктор, створює невалідний канал
						/// </summary>
						NamedPipeClient() :This(nullptr) {}
					public:
						/// <summary>
						/// Зчитує дані з каналу в буфер
						/// </summary>
						/// <param name="data">Буфер даних</param>
						virtual void ReadRaw(Containers::Blob& data) override final;

						/// <summary>
						/// Запис дані в канал з буферу
						/// </summary>
						/// <param name="rawData">Буфер даних</param>
						virtual void WriteRawData(const Containers::Blob& data) override final;

						/// <summary>
						/// Закриває канал якщо він робочий, інакше нічого не робить
						/// </summary>
						virtual void Close() override final;

						/// <summary>
						/// Очікування закритя каналу 
						/// </summary>
						virtual void WaitClosing() const override final;

						/// <summary>
						/// Вказує чи канал робочий
						/// </summary>
						/// <returns>Логічне значення, true, якщо канал робочий</returns>
						virtual Boolean IsValid() const override final { return This != nullptr && This->pipe != nullptr; }
					public:
						/// <summary>
						/// Вертає ім'я каналу
						/// </summary>
						/// <returns>Рядок з іменем каналу</returns>
						const String& GetPipeName() const { return This->name; }

						/// <summary>
						/// Ім'я каналу
						/// </summary>
						String Property(GetPipeName) Name;
					public:
						friend NamedPipeClient ConnectPipe(const String& name, PipeMode mode);
					public:
						RefCountClass(NamedPipeClient, This)
					};

					class NamedPipeServerInstance;

					/// <summary>
					/// Сервер каналу
					/// </summary>
					class NamedPipeServer final : protected IDisposable {
					private:
						struct Data {
							PipeMode mode;
							UInt32 maxInstanceCount;
							Containers::ConcurencyList<NamedPipeServerInstance> instances;
							String name;

							RefCountT RefCount;

							Data(
								PipeMode mode,
								UInt32 maxInstanceCount,
								String name
							) :
								mode(mode),
								maxInstanceCount(maxInstanceCount),
								name(name),
								instances(),
								RefCount(1)
							{};
						};

						Data* This;
					private:
						NamedPipeServer(Data*&& This) :This(This) {};
					private:
						Handle CreateServerPipeHandle();
					protected:
						virtual void Dispose() override;
					public:
						/// <summary>
						/// Пустий констукор каналу, ініціалізує не робочий канал
						/// </summary>
						NamedPipeServer() :This(nullptr) {};

						/// <summary>
						/// Очікує і приєднюється до клієнту вертаюче екземпляр з'єднання
						/// </summary>
						/// <returns>Екземпляр з'єднання каналу з другою стороною</returns>
						NamedPipeServerInstance Connect();

						/// <summary>
						/// Закриває всі з'єднання з клієнтами
						/// </summary>
						void CloseAllInstance();

						/// <summary>
						/// Закриває всі з'єднання і знищує канал
						/// </summary>
						void DestroyPipe();
					public:
						/// <summary>
						/// Вказує чи канал робочий
						/// </summary>
						/// <returns>Логічне значення, true, якщо канал робочий</returns>
						bool GetIsValid() const { return This != nullptr && This->instances.Count <= This->maxInstanceCount; }
						
						/// <summary>
						/// Вертає кількість активних з'єднаннь
						/// </summary>
						/// <returns>Число яке вказує кількість активних з'єднаннь</returns>
						SizeT GetInstanceCount() { return This == nullptr ? 0 : This->instances.Count; }

						/// <summary>
						/// Вертає ім'я каналу
						/// </summary>
						/// <returns>Рядок з іменем каналу</returns>
						const String& GetName() { return This->name; }

						/// <summary>
						/// Режим каналу
						/// </summary>
						/// <returns>Поточний режим каналу</returns>
						PipeMode GetSupportedMode() { return This->mode; }
					public:
						/// <summary>
						/// Вказує чи канал робочий
						/// </summary>
						Boolean Property(GetIsValid) IsValid;

						/// <summary>
						/// Кількість активних з'єднань
						/// </summary>
						SizeT Property(GetInstanceCount) InstanceCount;

						/// <summary>
						/// Ім'я каналу
						/// </summary>
						String Property(GetName) Name;

						/// <summary>
						/// Режим роботи каналу
						/// </summary>
						PipeMode Property(GetSupportedMode) SupportedMode;
					public:
						RefCountClass(NamedPipeServer, This)
					public:
						friend NamedPipeServer CreatePipe(const String& name, UInt8 maxInstanceCount, PipeMode mode);
					};

					/// <summary>
					/// З'єднання серверного каналу
					/// </summary>
					class NamedPipeServerInstance final : public INamedPipe {
					private:
						struct Data {
							String pipeName;
							PipeMode mode;
							Handle pipe;
							RefCountT RefCount;
							Containers::ConcurencyList<NamedPipeServerInstance>::Iterator ThisInstanceLink;
							Threading::SRWLock inputLock;
							Threading::SRWLock ouputLock;

							Data(
								String&& pipeName,
								Handle pipe,
								PipeMode mode
							) :
								pipe(pipe),
								mode(mode),
								RefCount(1),
								inputLock(),
								ouputLock(),
								ThisInstanceLink()
							{}
						};

						Data* This;
					private:
						NamedPipeServerInstance(Data*&& This) :This(This) {};
					protected:
						virtual void Dispose() override;
					public:
						/// <summary>
						/// Пустий конструктор, ініціалізує не робочий канал
						/// </summary>
						NamedPipeServerInstance() :This(nullptr) {};
					public:
						/// <summary>
						/// Зчитує дані з каналу в буфер
						/// </summary>
						/// <param name="data">Буфер даних</param>
						virtual void ReadRaw(Containers::Blob& data) override final;

						/// <summary>
						/// Запис дані в канал з буферу
						/// </summary>
						/// <param name="rawData">Буфер даних</param>
						virtual void WriteRawData(const Containers::Blob& data) override final;

						/// <summary>
						/// Закриває канал якщо він робочий, інакше нічого не робить
						/// </summary>
						virtual void Close() override final;

						/// <summary>
						/// Очікування закритя каналу 
						/// </summary>
						virtual void WaitClosing() const override final;

						/// <summary>
						/// Вказує чи канал робочий
						/// </summary>
						/// <returns>Логічне значення, true, якщо канал робочий</returns>
						virtual Boolean IsValid() const override final { return This != nullptr && This->pipe != nullptr; }
					public:
						RefCountClass(NamedPipeServerInstance, This);
					public:
						friend NamedPipeServerInstance NamedPipeServer::Connect();
					};

					/// <summary>
					/// Створення сервера каналів
					/// </summary>
					/// <param name="name">Ім'я каналу</param>
					/// <param name="maxInstanceCount">Максимальна кільість екземплярів з'єднаннь каналів</param>
					/// <param name="mode">Режим каналів</param>
					/// <returns>Об'єкт сервера каналу</returns>
					NamedPipeServer CreatePipe(const String& name, UInt8 maxInstanceCount = Math::Max<UInt8>(), PipeMode mode = PipeMode::Both);

					/// <summary>
					/// Приєднання до існуючого каналу
					/// </summary>
					/// <param name="name">Ім'я каналу</param>
					/// <param name="mode">Режим каналу</param>
					/// <returns>Об'єкт клієнтського з'єднання каналу</returns>
					NamedPipeClient ConnectPipe(const String& name, PipeMode mode = PipeMode::Both);
				}
			}
		}
	}
}