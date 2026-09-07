# Register project files and matching Solution Explorer folders after adding modules.
$ErrorActionPreference = 'Stop'
$taskRoot = Split-Path $PSScriptRoot -Parent
$taskProject = Join-Path $taskRoot 'csc3081_farm_project.vcxproj'
[xml]$taskXml = Get-Content -LiteralPath $taskProject
$taskNs = $taskXml.DocumentElement.NamespaceURI
$taskGroup = $taskXml.Project.ItemGroup | Where-Object { $_.Label -eq 'FarmSources' }
$taskGroup.RemoveAll()
$taskGroup.SetAttribute('Label', 'FarmSources')
[xml]$taskFilters = '<?xml version="1.0" encoding="utf-8"?><Project ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003"><ItemGroup/><ItemGroup/></Project>'
$taskFilterGroups = $taskFilters.DocumentElement.ChildNodes
$taskFiles = Get-ChildItem -LiteralPath "$taskRoot\src" -Recurse -File | Where-Object { $_.Extension -in '.cpp', '.h' } | Sort-Object FullName
$taskFolders = @{}
foreach ($taskFile in $taskFiles) {
    $taskRelative = $taskFile.FullName.Substring($taskRoot.Length + 1)
    $taskType = if ($taskFile.Extension -eq '.cpp') { 'ClCompile' } else { 'ClInclude' }
    $taskNode = $taskXml.CreateElement($taskType, $taskNs)
    $taskNode.SetAttribute('Include', $taskRelative)
    [void]$taskGroup.AppendChild($taskNode)
    $taskFolder = Split-Path $taskRelative -Parent
    $taskFolders[$taskFolder] = $true
    $taskFilterNode = $taskFilters.CreateElement($taskType, $taskNs)
    $taskFilterNode.SetAttribute('Include', $taskRelative)
    $taskFilterValue = $taskFilters.CreateElement('Filter', $taskNs)
    $taskFilterValue.InnerText = $taskFolder
    [void]$taskFilterNode.AppendChild($taskFilterValue)
    [void]$taskFilterGroups[1].AppendChild($taskFilterNode)
}
foreach ($taskFolder in ($taskFolders.Keys | Sort-Object)) {
    $taskNode = $taskFilters.CreateElement('Filter', $taskNs)
    $taskNode.SetAttribute('Include', $taskFolder)
    [void]$taskFilterGroups[0].AppendChild($taskNode)
}
$taskXml.Save($taskProject)
$taskFilters.Save("$taskProject.filters")
