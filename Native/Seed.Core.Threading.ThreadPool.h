#pragma once
#include "Seed.Core.Threading.h"
#include "Seed.Core.TaskShedule.Sheduler.h"

namespace Seed {
	inline namespace Core {
		namespace Threading {
			//Клас роботи
			// екземпляри роботи виконуються в пулі потоків
			class SeedKernelCall Job : protected IDisposable {
			public:
				//Дані задачі
				struct data {
					//Чи задача запущена
					bool IsStarted;
					//Івент який буде переведено в сигнальний стан лиш коли задача завершиться
					Handle event;
					//Дескриптор задачі в пулі потоків NT
					ImplSpecificT work;
					//Кількість силок на задачу
					RefCountT RefCount;
					//Делегат
					Delegate delegate;
					//ID потоку в якому виконується задача
					SizeT TID;
					//Конструктор
					data(Handle event, Delegate&& delegate, ImplSpecificT work)
						:
						event(event),
						work(work),
						delegate(MoveRValue(delegate)),
						RefCount(1),
						IsStarted(false),
						TID(0) {}
				};
			private:
				//Покажчик на дані екземляра задачі
				data* This;
			private:
				//Пустий конструктор
				__forceinline Job() :This(nullptr) {};

				virtual void Dispose() override final;
			public:
				RefCountClass(Job, This);

				//Очікує на завершення події
				__forceinline void Wait() {}

				////Вертає чи запущена задача
				//__forceinline Boolean IsStarted() {}

				////Вертає чи завершена задача
				//__forceinline Boolean IsEnded() {}

				//Перериває роботу
				void Interupt();

				//Вертає результат задачі,
				//генерує NullPointExeption якщо задача 
				//не запущена або об'єкт не ініціалізований 
				template<typename T>
				__forceinline T GetResult() {
					if (Data == nullptr)
						Throw(NullPointException(
							ExceptionPosition,
							reinterpret_cast<SizeT>(Data),
							NullPointException::Types::Read)
						);
					Wait();
					return Data->delegate.GetResult<T>();
				}

				friend class Dispatcher;
			};

			//Диспетчер пула потоків
			//Метод Dispatch запускає делегат в пулі потоків
			//Деструктор останього екземпляра буде очікувати на завершення всіх задач в пулі потоків
			class SeedKernelCall Dispatcher : protected IDisposable {
			private:
				//Дані диспетчера 
				struct __dispatcher {
					//Керуюча структура пула потоків
					ImplSpecificT pool;
					//Настройки середовищп для пула потоків
					ImplSpecificT env;

					ImplSpecificT cleanupData;
					//Синхронізація задач з деструктором і підрахунок активних задач
					Thread::GuardLock lock;
					//Кількість задач в пулі
					RefCountT JobsCounts;
					//Кількість силок на даний екземаляр
					RefCountT RefCount;
				};
				//Дані даного екземаляра
				__dispatcher* This;
			protected:
				virtual void Dispose() override final;
			public:
				RefCountClass(Dispatcher, This);
				//Конструктор диспетчера пула потоків
				//Створює новий пул потоків
				//Стартова кількість потоків рівна настройці THREAD_POOL_THREADS_MIN_COUNT
				//Максимальна кількість потоків відповідно THREAD_POOL_THREADS_MAX_COUNT
				//Стартовий і максимальний розмір стеку рівний відповідним параметрам потоків
				Dispatcher();

				//Запускає задачу
				//Аргументом є функція з її аргументами
				Job Dispatch(Seed::Core::Delegate&& EntryPoint);

				//Вертає кількість задач в пулі
				__forceinline UInt64 GetJobsCount() noexcept { return This->JobsCounts; }

				//Вертає кількіть активних задач
				__forceinline UInt64 GetActiveJobCount() noexcept { return This->lock.GetGuardedScopesCount(); }
			};
		}
	}
}