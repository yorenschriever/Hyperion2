$library = ([IO.DirectoryInfo] $PSScriptRoot).parent
$root = $library.Parent
Set-Location $root.FullName

if (! (Test-Path $profile)) {
    $profile_content = @()
    $profile_content += '$HYPERION_LIB_DIR = "$($PWD.Path)\library"'
    $profile_content += '. "$($HYPERION_LIB_DIR)\platform\docker\build.ps1"'
    $profile_content += '. "$($HYPERION_LIB_DIR)\platform\docker\run.ps1"'
    $profile_content | out-file -filepath $profile -force
    . $profile
}
else {
    $AddHyperionLibDir = $true
    $AddBuildScript = $true
    $AddRunScript = $true

    $profile_content = get-content $profile
    foreach ($line in $profile_content) {
        if ($line -like '*$HYPERION_LIB_DIR = "$($PWD.Path)\library"*' ) { $AddHyperionLibDir = $false }
        if ($line -like '*. "$($HYPERION_LIB_DIR)\platform\docker\build.ps1"*' ) { $AddBuildScript = $false }
        if ($line -like '*. "$($HYPERION_LIB_DIR)\platform\docker\run.ps1"*' ) { $AddRunScript = $false }
    }

    if ($AddHyperionLibDir -eq $true) { $profile_content += '$HYPERION_LIB_DIR = "$($PWD.Path)\library"'}
    if ($AddBuildScript -eq $true) { $profile_content += '. "$($HYPERION_LIB_DIR)\platform\docker\build.ps1"'}
    if ($AddRunScript -eq $true) { $profile_content += '. "$($HYPERION_LIB_DIR)\platform\docker\run.ps1"'}
    
    $profile_content | out-file -filepath $profile -force
    . $profile
}
