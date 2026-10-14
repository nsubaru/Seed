# escape=`
ARG BUILD_TOOLS_IMAGE=seed-build-tools:vs2026
ARG RUNTIME_IMAGE=mcr.microsoft.com/dotnet/runtime:10.0-windowsservercore-ltsc2025
FROM ${BUILD_TOOLS_IMAGE} AS build
ARG CONFIGURATION=Release
ARG SUBSYSTEM=Native
ENV SEED_BUILD_CONFIGURATION=${CONFIGURATION}
ENV SEED_BUILD_SUBSYSTEM=${SUBSYSTEM}
SHELL ["powershell", "-NoLogo", "-NoProfile", "-Command", "$ErrorActionPreference = 'Stop'; $ProgressPreference = 'SilentlyContinue';"]
WORKDIR C:\src
COPY . .
RUN & C:\src\Docker\Build-SeedImage.ps1 -Configuration $env:SEED_BUILD_CONFIGURATION -Subsystem $env:SEED_BUILD_SUBSYSTEM

# Optional complete build-output export, including matching PDB and MAP files.
FROM mcr.microsoft.com/windows/servercore:ltsc2025 AS artifacts
COPY --from=build C:/src/Artifacts/ C:/out/Artifacts/
COPY --from=build C:/src/Documentation/ C:/out/Documentation/
CMD ["cmd", "/c", "exit", "0"]

# Test tools stay in this image; each E2E case owns its native runtime.
FROM build AS test
RUN New-Item -ItemType Directory C:\results -Force | Out-Null; `
    & icacls C:\src\Artifacts /grant '*S-1-5-32-545:(OI)(CI)M'; if ($LASTEXITCODE -ne 0) { throw 'Artifact permissions failed' }; `
    & icacls C:\results /grant '*S-1-5-32-545:(OI)(CI)M'; if ($LASTEXITCODE -ne 0) { throw 'Result permissions failed' }
USER ContainerUser
ENTRYPOINT ["powershell", "-NoLogo", "-NoProfile", "-File", "C:/src/Docker/Invoke-SeedContainerTests.ps1"]

FROM ${RUNTIME_IMAGE} AS runtime
SHELL ["powershell", "-NoLogo", "-NoProfile", "-Command", "$ErrorActionPreference = 'Stop';"]
WORKDIR C:\Seed
RUN New-Item -ItemType Directory C:\Seed.Data -Force | Out-Null; `
    & icacls C:\Seed.Data /grant '*S-1-5-32-545:(OI)(CI)M'; if ($LASTEXITCODE -ne 0) { throw 'Data permissions failed' }
COPY --from=build C:/payload/Runtime/ C:/Seed/Runtime/
COPY --from=build C:/payload/Console/ C:/Seed/Console/
COPY --from=build C:/src/Docker/Start-SeedContainer.ps1 C:/Seed/Start-SeedContainer.ps1
COPY --from=build C:/src/LICENSE C:/Seed/LICENSE
COPY --from=build C:/src/NOTICE C:/Seed/NOTICE
COPY --from=build C:/src/THIRD-PARTY-NOTICES.md C:/Seed/THIRD-PARTY-NOTICES.md
COPY --from=build C:/src/Lib/NtApi/LICENSE.phnt C:/Seed/licenses/LICENSE.phnt
ENV SEED_CONFIG_DIRECTORY="C:\Seed.Data"
USER ContainerUser
ENTRYPOINT ["powershell", "-NoLogo", "-NoProfile", "-File", "C:/Seed/Start-SeedContainer.ps1"]
LABEL org.opencontainers.image.title="Seed HTTP/TLS and local console demonstrations"
