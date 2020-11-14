#pragma once
#include "Seed.Core.IO.IPC.Pipes.Base.h"

namespace Seed {
	inline namespace Core {
		namespace IO {
			namespace IPC {
				namespace Pipes {
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
						virtual Boolean GetIsValid() const = 0;

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
							Boolean isNoCache;
							Handle pipe;
							Threading::SRWLock inputLock;
							Threading::SRWLock ouputLock;
							String name;
							RefCountT RefCount;

							Data(
								Handle pipe,
								Boolean isNoCache,
								PipeMode mode,
								String&& name
							) :
								pipe(pipe),
								isNoCache(isNoCache),
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
						/// Очікування на очищення буферів
						/// </summary>
						void Flush();
					public:
						/// <summary>
						/// Вказує чи канал робочий
						/// </summary>
						/// <returns>Логічне значення, true, якщо канал робочий</returns>
						virtual Boolean GetIsValid() const override final { return This != nullptr && This->pipe != nullptr; }

						/// <summary>
						/// Вертає ім'я каналу
						/// </summary>
						/// <returns>Рядок з іменем каналу</returns>
						const String& GetName() const { return This->name; }

						/// <summary>
						/// Режим каналу
						/// </summary>
						/// <returns>Поточний режим каналу</returns>
						PipeMode GetSupportedMode() const { return This->mode; }

						/// <summary>
						/// Вказує чи даному екземпляру виконувати передачу даних без буферизації
						/// </summary>
						/// <returns>Логічне значення яке вказує чи даному екземпляру виконувати передачу даних без буферизації</returns>
						Boolean GetIsNoCache() const { return This->isNoCache; }
					public:
						/// <summary>
						/// Вказує чи канал робочий
						/// </summary>
						Boolean Property(GetIsValid) IsValid;

						/// <summary>
						/// Ім'я каналу
						/// </summary>
						String Property(GetName) Name;

						/// <summary>
						/// Режим роботи каналу
						/// </summary>
						PipeMode Property(GetSupportedMode) SupportedMode;

						/// <summary>
						/// Логічне значення яке вказує чи даному екземпляру виконувати передачу даних без буферизації
						/// </summary>
						Boolean Property(GetIsNoCache) IsNoCache;
					public:
						friend NamedPipeClient ConnectPipe(const String& name, PipeMode mode, bool isNoCache);
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
							Boolean isNoCache;
							UInt32 maxInstanceCount;
							Containers::ConcurencyList<NamedPipeServerInstance> instances;
							String name;

							RefCountT RefCount;

							Data(
								PipeMode mode,
								Boolean isNoCache,
								UInt32 maxInstanceCount,
								String name
							) :
								mode(mode),
								isNoCache(isNoCache),
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
						Handle CreateServerPipeHandle(bool isNoCache);
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
						/// Очікує і приєднюється до клієнту вертаюче екземпляр з'єднання
						/// </summary>
						/// <param name="isNoCache">Вказує чи даному екземпляру виконувати передачу даних без буферизації</param>
						/// <returns>Екземпляр з'єднання каналу з другою стороною</returns>
						NamedPipeServerInstance Connect(Boolean isNoCache);

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
						SizeT GetInstanceCount() const { return This == nullptr ? 0 : This->instances.Count; }

						/// <summary>
						/// Вертає ім'я каналу
						/// </summary>
						/// <returns>Рядок з іменем каналу</returns>
						const String& GetName() const { return This->name; }

						/// <summary>
						/// Режим каналу
						/// </summary>
						/// <returns>Поточний режим каналу</returns>
						PipeMode GetSupportedMode() const { return This->mode; }

						/// <summary>
						/// Вказує чи даному екземпляру виконувати передачу даних без буферизації
						/// </summary>
						/// <returns>Логічне значення яке вказує чи даному екземпляру виконувати передачу даних без буферизації</returns>
						Boolean GetIsNoCache() const { return This->isNoCache; }
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

						/// <summary>
						/// Логічне значення яке вказує чи підтримує автоскидання буферів після кожної операції
						/// </summary>
						Boolean Property(GetIsNoCache) IsNoCache;
					public:
						RefCountClass(NamedPipeServer, This)
					public:
						friend NamedPipeServer CreatePipe(const String& name, UInt8 maxInstanceCount, PipeMode mode, bool isNoCache);
					};

					/// <summary>
					/// З'єднання серверного каналу
					/// </summary>
					class NamedPipeServerInstance final : public INamedPipe {
					private:
						struct Data {
							String pipeName;
							PipeMode mode;
							Boolean isNoCache;
							Handle pipe;
							RefCountT RefCount;
							Containers::ConcurencyList<NamedPipeServerInstance>::Iterator ThisInstanceLink;
							Threading::SRWLock inputLock;
							Threading::SRWLock ouputLock;

							Data(
								String&& pipeName,
								Boolean isNoCache,
								Handle pipe,
								PipeMode mode
							) :
								pipe(pipe),
								isNoCache(isNoCache),
								mode(mode),
								pipeName(MoveRValue(pipeName)),
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
						/// Очікування на очищення буферів
						/// </summary>
						void Flush();
					public:
						/// <summary>
						/// Вказує чи канал робочий
						/// </summary>
						/// <returns>Логічне значення, true, якщо канал робочий</returns>
						virtual Boolean GetIsValid() const override final { return This != nullptr && This->pipe != nullptr; }

						/// <summary>
						/// Вертає ім'я каналу
						/// </summary>
						/// <returns>Рядок з іменем каналу</returns>
						const String& GetName() const { return This->pipeName; }

						/// <summary>
						/// Режим каналу
						/// </summary>
						/// <returns>Поточний режим каналу</returns>
						PipeMode GetSupportedMode() const { return This->mode; }

						/// <summary>
						/// Вказує чи даному екземпляру виконувати передачу даних без буферизації
						/// </summary>
						/// <returns>Логічне значення яке вказує чи даному екземпляру виконувати передачу даних без буферизації</returns>
						Boolean GetIsNoCache() const { return This->isNoCache; }

					public:
						/// <summary>
						/// Вказує чи канал робочий
						/// </summary>
						Boolean Property(GetIsValid) IsValid;

						/// <summary>
						/// Ім'я каналу
						/// </summary>
						String Property(GetName) Name;

						/// <summary>
						/// Режим роботи каналу
						/// </summary>
						PipeMode Property(GetSupportedMode) SupportedMode;

						/// <summary>
						/// Логічне значення яке вказує чи даному екземпляру виконувати передачу даних без буферизації
						/// </summary>
						Boolean Property(GetIsNoCache) IsNoCache;
					public:
						RefCountClass(NamedPipeServerInstance, This);
					public:
						friend NamedPipeServerInstance NamedPipeServer::Connect(bool isNoCache);
					};

					ForceInline NamedPipeServerInstance Seed::Core::IO::IPC::Pipes::NamedPipeServer::Connect()
					{
						return this->Connect(This->isNoCache);
					}

					/// <summary>
					/// Створення сервера каналів
					/// </summary>
					/// <param name="name">Ім'я каналу</param>
					/// <param name="maxInstanceCount">Максимальна кільість екземплярів з'єднаннь каналів</param>
					/// <param name="mode">Режим каналів</param>
					/// <returns>Об'єкт сервера каналу</returns>
					NamedPipeServer CreatePipe(const String& name, UInt8 maxInstanceCount = Math::Max<UInt8>(), PipeMode mode = PipeMode::Both, bool isNoCache = true);

					/// <summary>
					/// Приєднання до існуючого каналу
					/// </summary>
					/// <param name="name">Ім'я каналу</param>
					/// <param name="mode">Режим каналу</param>
					/// <returns>Об'єкт клієнтського з'єднання каналу</returns>
					NamedPipeClient ConnectPipe(const String& name, PipeMode mode = PipeMode::Both, bool isNoCache = true);
				}
			}
		}
	}
}