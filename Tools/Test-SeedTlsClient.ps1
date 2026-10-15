param([ValidateSet('Debug', 'Release', 'Coverage')][string] $Configuration = 'Debug')

$ErrorActionPreference = 'Stop'
$seedRoot = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..')).Path
$seedPeerExe = Join-Path $seedRoot "Artifacts/x64/$Configuration/Seed.Tls.TestPeer/Seed.Tls.TestPeer.exe"
$seedClientExe = Join-Path $seedRoot "Artifacts/x64/$Configuration/Seed.Native.HttpClient/Seed.Native.HttpClient.exe"
$seedRuntime = Join-Path $seedRoot "Artifacts/x64/$Configuration/Runtime/Seed.Runtime.exe"
$seedOutput = Join-Path $seedRoot 'Artifacts/tls-client'
if ([Security.Principal.WindowsPrincipal]::new([Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator))
{
    throw 'Run TLS client integration under a non-elevated account.'
}

foreach ($seedFile in @($seedPeerExe, $seedClientExe, $seedRuntime))
{
    if (-not (Test-Path -LiteralPath $seedFile))
    {
        throw "Build the matching Seed solution first: $seedFile"
    }
}

function Invoke-SeedTlsRequest(
    [string] $Url, 
    [string] $Directory,
    [string] $Root, 
    [string] $Protocol,
    [string] $Name, 
    [string] $ExpectedFailure = '', 
    [string] $ConnectAddress = '127.0.0.1')
{
    $seedBody      = Join-Path $Directory "$Name.txt"
    $seedClientLog = Join-Path $Directory "$Name.console.log"

    & $seedClientExe --runtime $seedRuntime --url $Url --output $seedBody --trust-root $Root --tls $Protocol --connect-address $ConnectAddress *> $seedClientLog

    $seedExit = $LASTEXITCODE

    if ($ExpectedFailure)
    {
        if ($seedExit -eq 0 -or -not (Test-Path -LiteralPath "$seedBody.error.txt"))
        {
            throw "TLS failure was not rejected: $Name"
        }

        if (-not (Select-String -LiteralPath "$seedBody.error.txt" -Pattern $ExpectedFailure -Quiet))
        {
            throw "Unexpected TLS failure: $Name; see $seedClientLog"
        }
    }
    else
    {
        if ($seedExit -ne 0)
        {
            throw "Native TLS client failed: $Name; see $seedClientLog"
        }

        if ([IO.File]::ReadAllText($seedBody) -cne "Seed TLS test OK`n")
        {
            throw "Incorrect TLS response: $Name"
        }

        $seedModules = @(Get-Content -LiteralPath "$seedBody.modules.txt" | Sort-Object)

        if (($seedModules -join ',') -ine 'ntdll.dll,Seed.Runtime.exe')
        {
            throw "Unexpected native TLS modules: $seedModules"
        }
    }
}

$seedLocation = Get-Location
Set-Location -LiteralPath $seedRoot

try
{
    foreach ($seedCase in @(@('1.2', ''), @('1.3', ''), @('1.3', 'expired'), @('1.3', 'ipv6')))
    {
        $seedProtocol  = $seedCase[0]
        $seedDirectory = Join-Path $seedOutput ($seedProtocol + $seedCase[1])

        New-Item -ItemType Directory -Path $seedDirectory -Force | Out-Null

        $seedUrlPath = Join-Path $seedDirectory 'url.txt'

        if (Test-Path -LiteralPath $seedUrlPath)
        {
            Remove-Item -LiteralPath $seedUrlPath
        }

        $seedArguments = @('"' + $seedDirectory + '"', $seedProtocol)

        if ($seedCase[1])
        {
            $seedArguments += $seedCase[1]
        }

        $seedPeer = Start-Process -FilePath $seedPeerExe -ArgumentList $seedArguments -WindowStyle Hidden -PassThru -RedirectStandardOutput (Join-Path $seedDirectory 'peer.log') -RedirectStandardError (Join-Path $seedDirectory 'peer-error.log')

        try
        {
            $seedDeadline = [DateTime]::UtcNow.AddSeconds(10)

            while (-not (Test-Path -LiteralPath $seedUrlPath))
            {
                if ($seedPeer.HasExited -or [DateTime]::UtcNow -gt $seedDeadline)
                {
                    $seedPeerError = Get-Content -LiteralPath (Join-Path $seedDirectory 'peer-error.log') -Raw
                    throw "TLS peer did not start: $seedDirectory`n$seedPeerError"
                }

                Start-Sleep -Milliseconds 100
            }

            $seedUrl     = [IO.File]::ReadAllText($seedUrlPath)
            $seedRootDer = Join-Path $seedDirectory 'root.der'

            if ($seedCase[1] -eq 'ipv6')
            {
                Invoke-SeedTlsRequest $seedUrl $seedDirectory $seedRootDer $seedProtocol 'valid-ipv6' '' '::1'
                Invoke-SeedTlsRequest ($seedUrl.Replace('[::1]', '[::2]')) $seedDirectory $seedRootDer $seedProtocol 'wrong-ipv6' 'not valid for the requested server name' '::1'
            }

            elseif ($seedCase[1] -eq 'expired')
            {
                Invoke-SeedTlsRequest $seedUrl $seedDirectory $seedRootDer $seedProtocol 'expired' 'not valid'
            }
            else
            {
                Invoke-SeedTlsRequest $seedUrl $seedDirectory $seedRootDer $seedProtocol 'valid'
                Invoke-SeedTlsRequest ($seedUrl.Replace('localhost', '127.0.0.1')) $seedDirectory $seedRootDer $seedProtocol 'valid-ipv4'
                Invoke-SeedTlsRequest ($seedUrl.Replace('localhost', '127.0.0.2')) $seedDirectory $seedRootDer $seedProtocol 'wrong-ipv4' 'not valid for the requested server name'
                Invoke-SeedTlsRequest ($seedUrl.Replace('localhost', 'wrong.example')) $seedDirectory $seedRootDer $seedProtocol 'wrong-host' 'not valid for the requested server name'
                
                if ($seedProtocol -eq '1.3')
                {
                    $seedOrigin = $seedUrl.Substring(0, $seedUrl.LastIndexOf('/'))

                    foreach ($seedPath in @('redirect-relative', 'redirect-absolute', 'chunked', 'informational', 'duplicate-length'))
                    {
                        Invoke-SeedTlsRequest "$seedOrigin/$seedPath" $seedDirectory $seedRootDer $seedProtocol $seedPath
                    }

                    foreach ($seedPath in @('redirect-loop', 'conflicting-length', 'ambiguous-framing', 'truncated', 'invalid-chunk'))
                    {
                        Invoke-SeedTlsRequest "$seedOrigin/$seedPath" $seedDirectory $seedRootDer $seedProtocol $seedPath 'HTTP'
                    }

                    Invoke-SeedTlsRequest $seedUrl $seedDirectory (Join-Path $seedOutput '1.2/root.der') $seedProtocol 'wrong-root' 'not trusted'
                    Invoke-SeedTlsRequest $seedUrl $seedDirectory $seedRootDer '1.2' 'unsupported-protocol' 'TLS'
                }
            }

            Write-Host "TLS client scenario passed: $seedProtocol $($seedCase[1])"
        }
        finally
        {
            if (-not $seedPeer.HasExited)
            {
                $seedPeer.Kill()
                $seedPeer.WaitForExit()
            }

            $seedPeer.Dispose()
        }
    }
}
finally
{
    Set-Location -LiteralPath $seedLocation.Path
}

Write-Host "Independent TLS client checks passed. Reports: $seedOutput"
$global:LASTEXITCODE = 0
