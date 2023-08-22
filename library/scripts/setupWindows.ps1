$library = ([IO.DirectoryInfo] $PSScriptRoot).Parent.FullName

# create a profile if it does not yet exist
if (! (Test-Path $profile)) {
    New-Item -Path $profile -Type File -Force
}

# Add a line that calls the export.ps1 script to the profile and write out the file
$profile_content = ". "+$PSScriptRoot+"\export.ps1"
$profile_content | out-file -filepath $profile -force

#Write-Output $profile_content

# start a new shell with the profile loaded
. $profile