param(
    [ValidateSet('Debug', 'Release')][string] $Configuration = 'Debug',
    [switch] $TlsProbe
)

$ErrorActionPreference = 'Stop'

if ($env:GITHUB_ACTIONS -cne 'true' -or -not $env:GITHUB_WORKSPACE -or -not $env:RUNNER_TEMP)
{
    throw 'This account-creation helper is restricted to disposable GitHub Actions runners.'
}

$seedRoot = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..')).Path

if ($seedRoot -ine (Resolve-Path -LiteralPath $env:GITHUB_WORKSPACE).Path)
{
    throw 'Validation root differs from the GitHub checkout.'
}

$seedUserName   = 'SeedTest' + [Guid]::NewGuid().ToString('N').Substring(0, 8)
$seedPassword   = 'S!' + [Convert]::ToBase64String([Security.Cryptography.RandomNumberGenerator]::GetBytes(32))
$seedCredential = [Management.Automation.PSCredential]::new("$env:COMPUTERNAME\$seedUserName", (ConvertTo-SecureString $seedPassword -AsPlainText -Force))
$seedUser       = $null
$seedAddedTlsCurve = $false

try
{
    # Seed implements X25519 key exchange. The hosted image can enable only NIST
    # curves, so configure the independent Schannel peers before dropping rights.
    $seedTlsCurves = Get-TlsEccCurve

    Write-Output ('Initial CI TLS curves: ' + ($seedTlsCurves -join ', '))
    $seedTlsPolicy = Get-ItemProperty -LiteralPath 'HKLM:\SOFTWARE\Policies\Microsoft\Cryptography\Configuration\SSL\00010002' -ErrorAction SilentlyContinue
    Write-Output ('CI TLS curve policy: ' + ($seedTlsPolicy.EccCurves -join ', '))

    if ($seedTlsCurves -notcontains 'curve25519')
    {
        Enable-TlsEccCurve -Name 'curve25519' -Position 0
        $seedAddedTlsCurve = $true
        $seedTlsCurves = Get-TlsEccCurve

    }

    Write-Output ('Seed CI TLS curves: ' + ($seedTlsCurves -join ', '))

    $seedUser = New-LocalUser -Name $seedUserName -Password $seedCredential.Password -AccountExpires ([DateTime]::Now.AddHours(6)) -Description 'Disposable Seed CI validation account'
    Add-LocalGroupMember -SID 'S-1-5-32-545' -Member $seedUser

    $seedSourceAcl = Get-Acl -LiteralPath $seedRoot
    $seedSourceAcl.AddAccessRule([Security.AccessControl.FileSystemAccessRule]::new($seedUser.SID, 'ReadAndExecute', 'ContainerInherit,ObjectInherit', 'None', 'Allow'))

    Set-Acl -LiteralPath $seedRoot -AclObject $seedSourceAcl

    # Native tests verify that this token is not elevated. Grant only artifact writes.
    $seedArtifacts = Join-Path $seedRoot 'Artifacts'
    New-Item -ItemType Directory -Path $seedArtifacts -Force | Out-Null

    $seedAcl  = Get-Acl -LiteralPath $seedArtifacts
    $seedRule = [Security.AccessControl.FileSystemAccessRule]::new($seedUser.SID, 'FullControl', 'ContainerInherit,ObjectInherit', 'None', 'Allow')

    $seedAcl.AddAccessRule($seedRule)

    Set-Acl -LiteralPath $seedArtifacts -AclObject $seedAcl

    $seedShell    = (Get-Process -Id $PID).Path
    $seedScript  = Join-Path $PSScriptRoot 'Invoke-SeedValidation.ps1'
    $seedStdout  = Join-Path $seedArtifacts 'ci-ordinary-user.log'
    $seedStderr  = Join-Path $seedArtifacts 'ci-ordinary-user-errors.log'
    $seedArguments = @('-NoProfile', '-NonInteractive', '-File', ('"' + $seedScript + '"'), '-Configuration', $Configuration)

    if ($TlsProbe)
    {
        $seedArguments += '-TlsProbe'
    }

    $seedProcess = Start-Process -FilePath $seedShell -Credential $seedCredential -LoadUserProfile -WindowStyle Hidden -WorkingDirectory $seedRoot -ArgumentList $seedArguments -RedirectStandardOutput $seedStdout -RedirectStandardError $seedStderr -PassThru

    try
    {
        # Keep the runner responsive and retain useful logs if a validation stalls.
        $seedDeadline = [DateTime]::UtcNow.AddMinutes(40)

        while (-not $seedProcess.WaitForExit(30000))
        {
            Write-Output 'Ordinary-user Seed validation is running.'

            if ([DateTime]::UtcNow -gt $seedDeadline)
            {
                $seedProcess.Kill()
                throw 'Ordinary-user validation exceeded its deadline.'
            }
        }

        Get-Content -LiteralPath $seedStdout

        if (Test-Path -LiteralPath $seedStderr)
        {
            Get-Content -LiteralPath $seedStderr
        }

        if ($seedProcess.ExitCode -ne 0)
        {
            throw "Ordinary-user Seed validation failed: $($seedProcess.ExitCode)."
        }
    }
    finally
    {
        $seedProcess.Dispose()
    }
}
finally
{
    try
    {
        if ($seedUser)
        {
            Remove-LocalUser -SID $seedUser.SID
        }
    }
    finally
    {
        if ($seedAddedTlsCurve)
        {
            Disable-TlsEccCurve -Name 'curve25519'
        }

        $seedPassword   = $null
        $seedCredential = $null
    }
}
