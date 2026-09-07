param([switch]$SkipBuild, [switch]$RefreshScreenshots)
$ErrorActionPreference = 'Stop'
$taskRoot = Split-Path $PSScriptRoot -Parent
if (-not $SkipBuild) { & "$PSScriptRoot\build.ps1" -Configuration Release }
$taskExe = Join-Path $taskRoot 'build\x64\Release\csc3081_farm_project.exe'
$taskEvidence = Join-Path $taskRoot 'build\verification'
New-Item -ItemType Directory -Force -Path $taskEvidence | Out-Null
Push-Location $taskRoot
try {
    & $taskExe --self-test | Tee-Object -FilePath "$taskEvidence\self-test.txt"
    if ($LASTEXITCODE -ne 0) { throw 'Self-test failed.' }
    $taskCases = @(
        @{ Name = 'overview'; Args = @() },
        @{ Name = 'meadow_still'; Args = @('--view', 'meadow', '--wind', '0') },
        @{ Name = 'meadow_wind'; Args = @('--view', 'meadow', '--wind', '3') },
        @{ Name = 'cutting'; Args = @('--view', 'cutting', '--cut-demo') },
        @{ Name = 'crops'; Args = @('--view', 'crops', '--wind', '3') },
        @{ Name = 'night'; Args = @('--night') },
        @{ Name = 'cows'; Args = @('--view', 'cows') },
        @{ Name = 'small'; Args = @('--width', '640', '--height', '480') },
        @{ Name = 'wireframe'; Args = @('--view', 'meadow', '--wireframe', '--wind', '3') },
        @{ Name = 'no_textures'; Args = @('--no-textures') }
    )
    foreach ($taskCase in $taskCases) {
        $taskArgs = $taskCase.Args
        & $taskExe --capture "$taskEvidence\$($taskCase.Name).bmp" @taskArgs | Tee-Object -FilePath "$taskEvidence\$($taskCase.Name).txt"
        if ($LASTEXITCODE -ne 0) { throw "Render failed: $($taskCase.Name)" }
    }
    & $taskExe --benchmark 180 | Tee-Object -FilePath "$taskEvidence\benchmark.txt"
    if ($LASTEXITCODE -ne 0) { throw 'Animation benchmark reported an error.' }
    & "$PSScriptRoot\test-shutdown.ps1" -Platform x64 -Configuration Release
    # Run an isolated copy with no asset directory to test complete missing-asset startup.
    $taskNoAssets = Join-Path $taskEvidence 'no-assets'
    New-Item -ItemType Directory -Force -Path $taskNoAssets | Out-Null
    if (Test-Path -LiteralPath "$taskNoAssets\assets") { throw 'The no-assets fixture unexpectedly contains assets.' }
    Copy-Item -LiteralPath $taskExe -Destination "$taskNoAssets\farm.exe" -Force
    Push-Location $taskNoAssets
    try {
        & '.\farm.exe' --capture "$taskEvidence\missing_textures.bmp"
        if ($LASTEXITCODE -ne 0) { throw 'Missing-asset startup failed.' }
    }
    finally { Pop-Location }
    if ($RefreshScreenshots) {
        Add-Type -AssemblyName System.Drawing
        $taskImage = [System.Drawing.Image]::FromFile("$taskEvidence\overview.bmp")
        try { $taskImage.Save("$taskRoot\screenshots\overview.png", [System.Drawing.Imaging.ImageFormat]::Png) }
        finally { $taskImage.Dispose() }
    }
    Write-Output "Verification passed. Logs and framebuffer captures: $taskEvidence"
}
finally { Pop-Location }
