$env:HYPERION_LIB_DIR = ([IO.DirectoryInfo] $PSScriptRoot).Parent.FullName
Set-Alias -Name build         -Value "$($env:HYPERION_LIB_DIR)\scripts\build.ps1"
Set-Alias -Name run           -Value "$($env:HYPERION_LIB_DIR)\scripts\run.ps1"
Set-Alias -Name interactive   -Value "$($env:HYPERION_LIB_DIR)\platform\docker\run-interactive.ps1"

Set-Alias -Name hyper         -Value "$($env:HYPERION_LIB_DIR)\scripts\hyper.ps1"

# Write-Host "Export complete"  