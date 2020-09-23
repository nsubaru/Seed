#pragma once
#include "Seed.Core.Services.Base.h"
#include "Seed.Core.DepencyInjection.h"

namespace Seed {
	inline namespace Core {
		namespace Services {
			/// <summary>
			/// Базовий тип сервісу з автозавантаженням
			/// </summary>
			class IBaseBootUpService {
			protected:
				using DiRegisterDelegate = void(*)(DI::IDI& IoC);
				using DiLoadDelegate = void(*)(DI::IDI& IoC);
			protected:
				static volatile void* volatile RegisterBootUpServices(String&& typeName, DiRegisterDelegate registrator, DiLoadDelegate loader);
			};

			/// <summary>
			/// Запис про сервіс з підтримкою автозавантаження при запуску ядра
			/// </summary>
			/// <typeparam name="TService">Тип сервіса</typeparam>
			/// <typeparam name="...TCtorArgs">Типи аргументів конструктора</typeparam>
			template<typename TService, typename ... TCtorArgs>
			class IBootUpService : protected IBaseBootUpService {
			private:
				__declspec(noinline) static void RegisterInDi(DI::IDI& IoC) {
					(void)IoC.Register<TService, TCtorArgs...>(TService::GetRttiInformation());
				}

				__declspec(noinline) static void LoadToDi(DI::IDI& IoC) {
					(void)IoC.Resolve<TService>();
				}
			private:
#pragma section(".DIBOOT$a",read, write)
				[[maybe_unused]] __declspec(allocate(".DIBOOT$a")) inline static volatile void* volatile _ = 
					RegisterBootUpServices(CTTI::TypeName<TService>() ,RegisterInDi, LoadToDi);
#pragma comment(linker, "/MERGE:.data=.DIBOOT") 
			};

			/// <summary>
			/// Сервіс менеджера сервісів автозавантаження
			/// </summary>
			class BootupServicesManager final : IDisposable {
			private:
				struct Data {
					RefCountT RefCount;
					DI::IDI idi;

					Data(const DI::IDI& idi) : idi(idi), RefCount(1) {}
				};
			private:
				Data* This;
			protected:
				virtual void Dispose() override {}
			public:
				/// <summary>
				/// Конструктор, завантажує всі сервіси з автозавантаженням
				/// </summary>
				/// <param name="idi"></param>
				BootupServicesManager(const DI::IDI& idi);
			public:
				/// <summary>
				/// Кількість сервісів з автозавантаженням
				/// </summary>
				/// <returns></returns>
				UInt64 GetBootupServiceCount() const;
				/// <summary>
				/// Список типів сервісів з автозавантаженням
				/// </summary>
				/// <returns></returns>
				Containers::List<String> GetBootupServiceList() const;
			public:
				RefCountClass(BootupServicesManager, This)
			};
		}
	}
}