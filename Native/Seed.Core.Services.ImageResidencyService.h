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
        /// <summary>
        /// Creates the image-residency service for active runtime image mappings.
        /// </summary>
        /// <param name="runtimeManagementService">Service enumerating the active runtime images.</param>
        ImageResidencyService(RuntimeManagementService runtimeManagementService);

        /// <summary>
        /// Refreshes known runtime mappings and enables the primary image-residency session.
        /// </summary>
        /// <returns>Native status returned by image-residency activation.</returns>
        Memory::ImageResidency::NativeStatus Enable();

        /// <summary>
        /// Disables the current image-residency session.
        /// </summary>
        /// <returns>Native status returned by image-residency deactivation.</returns>
        Memory::ImageResidency::NativeStatus Disable();

        /// <summary>
        /// Gets the current image-residency state and accounting snapshot.
        /// </summary>
        /// <returns>Snapshot of the image-residency subsystem.</returns>
        Memory::ImageResidency::Snapshot Status();

    public:
        RefCountClassOutsideHeader(ImageResidencyService);
    };
}

extern template Seed::Core::Services::ImageResidencyService Seed::Core::DI::DependencyInjection::Resolve<Seed::Core::Services::ImageResidencyService>(bool);
extern template Seed::Core::RTTI::RttiTypeRecord TypeRecord<Seed::Core::Services::ImageResidencyService>();
