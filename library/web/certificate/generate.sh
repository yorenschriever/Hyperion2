BASEDIR=$(dirname "$0")

if [ ! -e "$BASEDIR/RootCA.key" ]; then
    # generate root certificate
    # root certificates have a max validity of 397 days. i generate one for 390 days. 
    # after that time you have to run the script again to renew the certificate.
    # (and upload a new build to all your devices)
    openssl req -x509 -nodes -new -sha256 -days 390 -newkey rsa:2048 -keyout "$BASEDIR/RootCA.key" -out "$BASEDIR/RootCA.pem" -subj "/C=de/CN=hyperion.local"
    openssl x509 -outform pem -in "$BASEDIR/RootCA.pem" -out "$BASEDIR/RootCA.crt"
else
    echo "Root certificate already exists, skipping generation"
fi

if [ ! -e "$BASEDIR/hyperion.key" ]; then
    #generate certificate
    openssl req -new -nodes -newkey rsa:2048 -keyout "$BASEDIR/hyperion.key" -out "$BASEDIR/hyperion.csr" -subj "/C=de/ST=State/L=City/O=Organization/CN=hyperion.local"
    openssl x509 -req -sha256 -days 1024 -in "$BASEDIR/hyperion.csr" -CA "$BASEDIR/RootCA.pem" -CAkey "$BASEDIR/RootCA.key" -CAcreateserial -extfile "$BASEDIR/vhosts_domains.ext" -out "$BASEDIR/hyperion.crt"
else
    echo "Certificate already exists, skipping generation"
fi
