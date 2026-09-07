param(
    [ValidateSet('Debug', 'Release')][string]$Configuration = 'Debug',
    [ValidateSet('x64', 'Win32')][string]$Platform = 'Win32',
    [ValidateRange(1, 20)][int]$Iterations = 5
)
$ErrorActionPreference = 'Stop'
$taskRoot = Split-Path $PSScriptRoot -Parent
$taskExe = Join-Path $taskRoot "build\$Platform\$Configuration\csc3081_farm_project.exe"
$taskEvidence = Join-Path $taskRoot "build\verification\shutdown-$Platform-$Configuration"
New-Item -ItemType Directory -Force -Path $taskEvidence | Out-Null
for ($taskRun = 1; $taskRun -le $Iterations; $taskRun++) {
    $taskProcess = [System.Diagnostics.Process]::new()
    $taskProcess.StartInfo.FileName = $taskExe
    $taskProcess.StartInfo.WorkingDirectory = $taskRoot
    $taskProcess.StartInfo.UseShellExecute = $false
    $taskProcess.StartInfo.CreateNoWindow = $true
    $taskProcess.StartInfo.RedirectStandardOutput = $true
    $taskProcess.StartInfo.RedirectStandardError = $true
    $taskStarted = $false
    try {
        $taskStarted = $taskProcess.Start()
        $taskOutput = $taskProcess.StandardOutput.ReadToEndAsync()
        $taskError = $taskProcess.StandardError.ReadToEndAsync()
        $taskDeadline = [DateTime]::UtcNow.AddSeconds(10)
        do {
            $taskProcess.Refresh()
            if ($taskProcess.HasExited) { throw 'Farm exited before its window opened.' }
            if ($taskProcess.MainWindowHandle -ne [IntPtr]::Zero) { break }
            Start-Sleep -Milliseconds 20
        } while ([DateTime]::UtcNow -lt $taskDeadline)
        # Close this child process's native window while its recurring timer is active.
        # CloseMainWindow sends the same WM_CLOSE message as the title-bar X.
        Start-Sleep -Milliseconds (300 + 17 * $taskRun)
        if (-not $taskProcess.CloseMainWindow()) { throw 'Could not request native window closure.' }
        if (-not $taskProcess.WaitForExit(5000)) { throw 'Farm did not exit after closing its window.' }
        $taskStdout = $taskOutput.GetAwaiter().GetResult()
        $taskStderr = $taskError.GetAwaiter().GetResult()
        $taskStdout | Set-Content -LiteralPath "$taskEvidence\run-$taskRun.stdout.txt"
        $taskStderr | Set-Content -LiteralPath "$taskEvidence\run-$taskRun.stderr.txt"
        Write-Output "$Platform $Configuration close $taskRun/$Iterations : exit $($taskProcess.ExitCode)"
        if ($taskProcess.ExitCode -ne 0 -or $taskStderr -match 'freeglut.*ERROR|no current window') {
            throw "Unclean shutdown: $taskStderr"
        }
    }
    finally {
        # Only terminate a test process created above if it failed to close normally.
        if ($taskStarted -and -not $taskProcess.HasExited) { $taskProcess.Kill(); $taskProcess.WaitForExit() }
        $taskProcess.Dispose()
    }
}
