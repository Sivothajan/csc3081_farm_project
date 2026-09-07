param(
    [switch]$Check,
    [ValidateSet('All', 'Markdown')][string]$Only = 'All'
)
$ErrorActionPreference = 'Stop'
$taskRoot = Split-Path $PSScriptRoot -Parent
$taskCache = Join-Path $taskRoot 'build\format'
$taskPython = Join-Path $taskCache 'python\Scripts\python.exe'
$taskRequirements = Join-Path $PSScriptRoot 'format-requirements.txt'
$taskStamp = Join-Path $taskCache 'requirements.sha256'
$taskUtf8 = [Text.UTF8Encoding]::new($false)
$taskFailed = $false

function Invoke-FormatTool {
    param([string]$Executable, [string[]]$Arguments)
    & $Executable @Arguments
    if ($LASTEXITCODE -ne 0) {
        if (-not $Check) { throw "Formatter failed: $Executable" }
        $script:taskFailed = $true
    }
}

# Enumerate owned source folders, never dependency, build or IDE directories.
$taskFiles = @(Get-ChildItem -LiteralPath $taskRoot -File -Force)
$taskFiles += Get-Item -LiteralPath (Join-Path $taskRoot 'assets\text.txt')
foreach ($taskFolder in @('src', 'docs', 'scripts', 'cmake')) {
    $taskFiles += Get-ChildItem -LiteralPath (Join-Path $taskRoot $taskFolder) -Recurse -File
}
$taskExtensions = @('.cpp', '.h', '.md', '.ps1', '.psd1', '.py', '.cmake', '.xml', '.vcxproj', '.filters', '.slnx', '.txt', '.in')
$taskNames = @('CMakeLists.txt', '.clang-format', '.editorconfig', '.gitattributes', '.gitignore', 'format-requirements.txt')
$taskFiles = @($taskFiles | Where-Object {
        ($_.Extension -in $taskExtensions -or $_.Name -in $taskNames) -and
        ($Only -eq 'All' -or $_.Extension -eq '.md')
    } | Sort-Object FullName)

if ($Only -eq 'All') {
    $taskClang = (Get-Command clang-format -ErrorAction SilentlyContinue).Source
    if (-not $taskClang) {
        $taskVsWhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
        if (Test-Path -LiteralPath $taskVsWhere) {
            $taskVs = & $taskVsWhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
            if ($taskVs) { $taskClang = Join-Path $taskVs 'VC\Tools\Llvm\x64\bin\clang-format.exe' }
        }
    }
    if (-not $taskClang -or -not (Test-Path -LiteralPath $taskClang)) {
        throw 'Install clang-format or the Visual Studio C++ tools, or use -Only Markdown.'
    }
}

if (-not (Test-Path -LiteralPath $taskPython)) {
    $taskSystemPython = (Get-Command python -ErrorAction SilentlyContinue).Source
    if (-not $taskSystemPython) { throw 'Install Python 3.11 or newer with pip and make python available on PATH.' }
    & $taskSystemPython -c 'import sys; sys.exit(sys.version_info < (3, 11))'
    if ($LASTEXITCODE -ne 0) { throw 'Formatting requires Python 3.11 or newer.' }
    & $taskSystemPython -m venv (Join-Path $taskCache 'python')
    if ($LASTEXITCODE -ne 0) { throw 'Could not create the local formatting environment.' }
}
$taskHash = (Get-FileHash -LiteralPath $taskRequirements -Algorithm SHA256).Hash
if (-not (Test-Path -LiteralPath $taskStamp) -or (Get-Content -LiteralPath $taskStamp -Raw).Trim() -ne $taskHash) {
    & $taskPython -m pip install --disable-pip-version-check --requirement $taskRequirements
    if ($LASTEXITCODE -ne 0) { throw 'Formatter setup failed. The first run needs internet access.' }
    [IO.File]::WriteAllText($taskStamp, $taskHash, $taskUtf8)
}

if ($Only -eq 'All') {
    $taskAnalyzerFolder = Join-Path $taskCache 'PSScriptAnalyzer-1.25.0'
    $taskAnalyzer = Join-Path $taskAnalyzerFolder 'PSScriptAnalyzer.psd1'
    if (-not (Test-Path -LiteralPath $taskAnalyzer)) {
        $taskZip = Join-Path $taskCache 'PSScriptAnalyzer.1.25.0.zip'
        Invoke-WebRequest -UseBasicParsing -Uri 'https://github.com/PowerShell/PSScriptAnalyzer/releases/download/1.25.0/PSScriptAnalyzer.1.25.0.nupkg' -OutFile $taskZip
        if ((Get-FileHash -LiteralPath $taskZip -Algorithm SHA256).Hash -ne '14e634c828eb98efb9f40b2918ba90f139ed5eccdf663a2a747736d996995d60') {
            throw 'PSScriptAnalyzer checksum mismatch.'
        }
        Expand-Archive -LiteralPath $taskZip -DestinationPath $taskAnalyzerFolder -Force
    }
    Import-Module $taskAnalyzer -Force
}

Push-Location $taskRoot
try {
    $taskCheckFlag = @()
    if ($Check) { $taskCheckFlag = @('--check') }
    $taskMarkdown = @($taskFiles | Where-Object Extension -EQ '.md' | Select-Object -ExpandProperty FullName)
    if ($taskMarkdown.Count) {
        Invoke-FormatTool $taskPython (@('-m', 'mdformat', '--wrap', '80', '--end-of-line', 'lf') + $taskCheckFlag + $taskMarkdown)
    }
    if ($Only -eq 'All') {
        $taskCpp = @($taskFiles | Where-Object { $_.Extension -in '.cpp', '.h' } | Select-Object -ExpandProperty FullName)
        [string[]]$taskClangFlags = if ($Check) { @('--dry-run', '--Werror') } else { @('-i') }
        if ($taskCpp.Count) { Invoke-FormatTool $taskClang ($taskClangFlags + $taskCpp) }

        $taskPyFiles = @($taskFiles | Where-Object Extension -EQ '.py' | Select-Object -ExpandProperty FullName)
        if ($taskPyFiles.Count) {
            Invoke-FormatTool $taskPython (@('-m', 'ruff', 'format', '--no-cache', '--line-length', '100') + $taskCheckFlag + $taskPyFiles)
        }
        $taskCmakeFiles = @($taskFiles | Where-Object { $_.Extension -eq '.cmake' -or $_.Name -eq 'CMakeLists.txt' } | Select-Object -ExpandProperty FullName)
        $taskCmakeFlag = if ($Check) { '--check' } else { '--in-place' }
        if ($taskCmakeFiles.Count) {
            Invoke-FormatTool $taskPython (@('-m', 'cmakelang.format', $taskCmakeFlag, '--line-width', '100', '--tab-size', '2', '--line-ending', 'unix', '--enable-sort', 'false') + $taskCmakeFiles)
        }
    }

    foreach ($taskFile in $taskFiles) {
        $taskOriginal = [IO.File]::ReadAllText($taskFile.FullName)
        $taskFormatted = $taskOriginal
        if ($taskFile.Extension -in '.ps1', '.psd1') {
            $taskTokens = $null
            $taskParseErrors = $null
            [void][Management.Automation.Language.Parser]::ParseInput($taskOriginal, [ref]$taskTokens, [ref]$taskParseErrors)
            if ($taskParseErrors.Count) { throw "Invalid PowerShell: $($taskFile.FullName)" }
            $taskFormatted = Invoke-Formatter -ScriptDefinition $taskOriginal
        }
        elseif ($taskFile.Extension -in '.xml', '.vcxproj', '.filters', '.slnx') {
            # Preserve command text and attributes while indenting XML elements.
            $taskXml = [xml]$taskOriginal
            $taskSettings = [Xml.XmlWriterSettings]::new()
            $taskSettings.Indent = $true
            $taskSettings.IndentChars = '  '
            $taskSettings.NewLineChars = "`n"
            $taskSettings.NewLineHandling = [Xml.NewLineHandling]::None
            $taskSettings.Encoding = $taskUtf8
            $taskSettings.OmitXmlDeclaration = -not $taskOriginal.TrimStart().StartsWith('<?xml')
            $taskStream = [IO.MemoryStream]::new()
            $taskWriter = [Xml.XmlWriter]::Create($taskStream, $taskSettings)
            try {
                $taskXml.Save($taskWriter)
                $taskWriter.Flush()
                $taskFormatted = $taskUtf8.GetString($taskStream.ToArray())
            }
            finally {
                $taskWriter.Dispose()
                $taskStream.Dispose()
            }
        }
        # Markdown hard breaks are handled by mdformat; preserve significant trailing spaces.
        $taskFormatted = $taskFormatted.Replace("`r`n", "`n").Replace("`r", "`n")
        if ($taskFile.Extension -notin '.md', '.ps1', '.psd1', '.py', '.cpp', '.h', '.cmake', '.xml', '.vcxproj', '.filters', '.slnx' -and $taskFile.Name -ne 'CMakeLists.txt') {
            $taskFormatted = [regex]::Replace($taskFormatted, '(?m)[ \t]+$', '')
        }
        $taskFormatted = $taskFormatted.TrimEnd("`n") + "`n"
        $taskBytes = [IO.File]::ReadAllBytes($taskFile.FullName)
        $taskHasBom = $taskBytes.Length -ge 3 -and $taskBytes[0] -eq 239 -and $taskBytes[1] -eq 187 -and $taskBytes[2] -eq 191
        if ($taskOriginal -cne $taskFormatted -or $taskHasBom) {
            $taskRelative = $taskFile.FullName.Substring($taskRoot.Length + 1)
            if ($Check) {
                Write-Output "Needs formatting: $taskRelative"
                $taskFailed = $true
            }
            else {
                [IO.File]::WriteAllText($taskFile.FullName, $taskFormatted, $taskUtf8)
                Write-Output "Formatted: $taskRelative"
            }
        }
    }
    if ($taskFailed) { throw 'Formatting check failed. Run scripts/format.ps1 to apply formatting.' }
    $taskAction = if ($Check) { 'Checked' } else { 'Formatted' }
    Write-Output "$taskAction $($taskFiles.Count) project text files."
}
finally { Pop-Location }
