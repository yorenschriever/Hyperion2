$env:HYPERION_LIB_DIR = ([IO.DirectoryInfo] $PSScriptRoot).Parent.Parent.FullName

Set-Alias -Name hyper-interactive   -Value "$($env:HYPERION_LIB_DIR)\platform\docker\run-interactive.ps1"

Set-Alias -Name hyper               -Value "$($env:HYPERION_LIB_DIR)\build-utils\shell\hyper.ps1"

# Write-Host "Export complete"  