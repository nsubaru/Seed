#pragma once
#include "Seed.Core.Services.Base.h"
#include "Seed.Core.Services.RuntimeManagementService.h"
#include "Seed.Core.Memory.ImageResidency.h"

namespace Seed::Core::Services
{
    /// <summary>
    /// Coordinates process-session residency policy for Seed runtime mappings.
    /// Low-level state is process-lifetime storage so it survives fast reboot.
    /// </summary>
    class ImageResidencyService final : public IDisposable
    {
    public:
        CttiClassRecord(
            ImageResidencyService,
            NoCttiInterfaces(),
            NoCttiConstructors(ImageResidencyService),
            CttiMethodGroup(),
            CttiProperties(),
            CttiStaticMethodGroup())

    private:
        struct Data;

    private:
        Data* This;

    protected:
        void Dispose() override;

    private:
        void RefreshRuntimeMappings();

    public:
        ImageResidencyService(RuntimeManagementService runtimeManagementService);

        Memory::ImageResidency::NativeStatus Enable();
        Memory::ImageResidency::NativeStatus Disable();
        Memory::ImageResidency::Snapshot Status();

    public:
        RefCountClassOutsideHeader(ImageResidencyService);
    };
}

extern template Seed::Core::Services::ImageResidencyService Seed::Core::DI::DependencyInjection::Resolve<Seed::Core::Services::ImageResidencyService>(bool);
extern template Seed::Core::RTTI::RttiTypeRecord TypeRecord<Seed::Core::Services::ImageResidencyService>();
