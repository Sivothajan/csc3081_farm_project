param(
    [ValidateSet('Debug', 'Release')][string]$Configuration = 'Debug',
    [ValidateSet('x64', 'Win32')][string]$Platform = 'x64'
)
$ErrorActionPreference = 'Stop'
$taskRoot = Split-Path $PSScriptRoot -Parent
$taskCmake = (Get-Command cmake -ErrorAction SilentlyContinue).Source
if (-not $taskCmake) {
    $taskVsWhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
    if (-not (Test-Path -LiteralPath $taskVsWhere)) { throw 'Visual Studio C++ build tools were not found.' }
    $taskVs = & $taskVsWhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
    if (-not $taskVs) { throw 'Install the Visual Studio Desktop development with C++ workload.' }
    $taskCmake = Join-Path $taskVs 'Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe'
}
if (-not (Test-Path -LiteralPath $taskCmake)) { throw 'Install the Visual Studio Desktop development with C++ workload, including CMake tools.' }
$taskBuild = Join-Path $taskRoot "build\deps\$Platform"
# Reconfigure to pick up dependency changes; FetchContent reuses its cached download.
& $taskCmake -S "$taskRoot\cmake\dependencies" -B $taskBuild -A $Platform
if ($LASTEXITCODE -ne 0) { throw 'FreeGLUT setup failed. The first build needs internet access to download the pinned release.' }
& $taskCmake --build $taskBuild --config $Configuration --target freeglut_static --parallel
if ($LASTEXITCODE -ne 0) { throw 'FreeGLUT build failed.' }
