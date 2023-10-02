$env:HYPERION_LIB_DIR = ([IO.DirectoryInfo] $PSScriptRoot).Parent.FullName
Set-Alias -Name build -Value "$($env:HYPERION_LIB_DIR)\scripts\build.ps1"
Set-Alias -Name run   -Value "$($env:HYPERION_LIB_DIR)\scripts\run.ps1"
# Write-Host "Export complete"  