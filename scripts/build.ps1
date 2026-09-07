param(
    [ValidateSet('Debug', 'Release')][string]$Configuration = 'Release',
    [ValidateSet('x64', 'Win32')][string]$Platform = 'x64',
    [switch]$Test
)
$ErrorActionPreference = 'Stop'
$taskRoot = Split-Path $PSScriptRoot -Parent
$taskVsWhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
if (-not (Test-Path -LiteralPath $taskVsWhere)) { throw 'Visual Studio C++ build tools were not found.' }
$taskVs = & $taskVsWhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (-not $taskVs) { throw 'Install the Visual Studio Desktop development with C++ workload.' }
& "$taskVs\MSBuild\Current\Bin\MSBuild.exe" "$taskRoot\csc3081_farm_project.vcxproj" "/p:Configuration=$Configuration" "/p:Platform=$Platform" /v:minimal /nologo
if ($LASTEXITCODE -ne 0) { throw 'Farm build failed.' }
if ($Test) {
    Push-Location $taskRoot
    try {
        & "$taskRoot\build\$Platform\$Configuration\csc3081_farm_project.exe" --self-test
        if ($LASTEXITCODE -ne 0) { throw 'Farm self-test failed.' }
    }
    finally { Pop-Location }
}
