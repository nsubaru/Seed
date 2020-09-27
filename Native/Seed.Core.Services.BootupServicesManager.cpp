#include "Seed.Core.Services.BootupServicesManager.h"

namespace Seed {
    inline namespace Core {
        namespace Services {
            using DiRegisterDelegate = void(*)(DI::IDI& IoC);
            using DiLoadDelegate = void(*)(DI::IDI& IoC);

            struct BootupServiceLoader {
                String ServiceTypeName;
                DiRegisterDelegate registrator;
                DiLoadDelegate loader;
            };

            Seed::Core::Containers::ConcurencyList<BootupServiceLoader>& GetBootupServices() {
                static auto bootupServiceLoader = Seed::Core::Containers::ConcurencyList<BootupServiceLoader>();
                return bootupServiceLoader;
            }

            BootupServicesManager::BootupServicesManager(const DI::IDI& idi) : This(new Data(idi))
            {
                GetBootupServices()
                    .ForEach([&](auto& service) { service.registrator(This->idi); })
                    .ForEach([&](auto& service) { service.loader(This->idi); });
            }

            UInt64 BootupServicesManager::GetBootupServiceCount() const
            {
                return GetBootupServices().Count;
            }

            Containers::List<String> BootupServicesManager::GetBootupServiceList() const
            {
                return GetBootupServices().CopyTo<Containers::List>(ArrowFunct(service, service.ServiceTypeName));
            }
        }
    }
}   

volatile void* volatile Seed::Core::Services::IBaseBootUpService::RegisterBootUpServices(String&& serviceTypeName, DiRegisterDelegate registrator, DiLoadDelegate loader)
{
    return (RawMemBlk)((SizeT)registrator | (SizeT)loader | (GetBootupServices() += BootupServiceLoader{ MoveRValue(serviceTypeName), registrator, loader }).Count);
}
