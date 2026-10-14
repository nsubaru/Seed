# escape=`
ARG SDK_IMAGE=mcr.microsoft.com/dotnet/sdk:10.0-windowsservercore-ltsc2025
FROM ${SDK_IMAGE}

SHELL ["powershell", "-NoLogo", "-NoProfile", "-Command", "$ErrorActionPreference = 'Stop'; $ProgressPreference = 'SilentlyContinue';"]

# The toolchain lives only in this build image, not in the runtime image.
RUN Invoke-WebRequest 'https://aka.ms/vs/stable/vs_BuildTools.exe' -OutFile C:\vs_buildtools.exe; `
    $installer = Start-Process C:\vs_buildtools.exe -WindowStyle Hidden -Wait -PassThru -ArgumentList `
        '--quiet', '--wait', '--norestart', '--nocache', '--installPath', 'C:\BuildTools', `
        '--add', 'Microsoft.VisualStudio.Workload.MSBuildTools', `
        '--add', 'Microsoft.VisualStudio.Workload.VCTools', `
        '--add', 'Microsoft.NetCore.Component.SDK', `
        '--add', 'Microsoft.VisualStudio.Component.NuGet.BuildTools', `
        '--add', 'Microsoft.VisualStudio.Component.VC.Tools.x86.x64', `
        '--add', 'Microsoft.VisualStudio.Component.Windows11SDK.26100', `
        '--add', 'Microsoft.VisualStudio.Component.TestTools.BuildTools'; `
    if ($installer.ExitCode -notin @(0, 3010)) { throw "Build Tools installation failed: $($installer.ExitCode)" }; `
    Remove-Item -LiteralPath C:\vs_buildtools.exe

ENV MSBUILD_EXE_PATH="C:\BuildTools\MSBuild\Current\Bin\MSBuild.exe"
RUN & $env:MSBUILD_EXE_PATH /version /nologo; if ($LASTEXITCODE -ne 0) { throw 'MSBuild unavailable' }; `
    dotnet --version; if ($LASTEXITCODE -ne 0) { throw '.NET SDK unavailable' }; `
    if (-not (Test-Path 'C:\BuildTools\MSBuild\Microsoft\VC\v180\Platforms\x64\PlatformToolsets\v145\Toolset.props')) { throw 'MSVC v145 unavailable' }; `
    dotnet new classlib --framework net10.0 --output C:\seed-toolchain-probe --no-restore; if ($LASTEXITCODE -ne 0) { throw 'SDK probe creation failed' }; `
    & $env:MSBUILD_EXE_PATH C:\seed-toolchain-probe\seed-toolchain-probe.csproj /m /restore /nologo; if ($LASTEXITCODE -ne 0) { throw 'Visual Studio .NET SDK resolution failed' }

LABEL org.opencontainers.image.title="Seed Windows build tools"
