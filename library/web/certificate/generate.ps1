$BaseDir = $PSScriptRoot

if (Test-Path (Join-Path $BaseDir "hyperion.key")) {
    # Not generating a new certificate because one already exists. To generate a new one, manually remove hyperion.key and run this script again.
    Write-Host "Skipping certificate generation"
    exit 1
}

# Generate root certificate
# Root certificates have a max validity of 397 days. Generate one for 390 days.
# After that time you have to run the script again to renew the certificate.
# (and upload a new build to all your devices)
& openssl req -x509 -nodes -new -sha256 -days 390 -newkey rsa:2048 -keyout (Join-Path $BaseDir "RootCA.key") -out (Join-Path $BaseDir "RootCA.pem") -subj "/C=de/CN=hyperion.local"
& openssl x509 -outform pem -in (Join-Path $BaseDir "RootCA.pem") -out (Join-Path $BaseDir "RootCA.crt")

# Generate certificate
& openssl req -new -nodes -newkey rsa:2048 -keyout (Join-Path $BaseDir "hyperion.key") -out (Join-Path $BaseDir "hyperion.csr") -subj "/C=de/ST=State/L=City/O=Organization/CN=hyperion.local"
& openssl x509 -req -sha256 -days 1024 -in (Join-Path $BaseDir "hyperion.csr") -CA (Join-Path $BaseDir "RootCA.pem") -CAkey (Join-Path $BaseDir "RootCA.key") -CAcreateserial -extfile (Join-Path $BaseDir "vhosts_domains.ext") -out (Join-Path $BaseDir "hyperion.crt")
