BASEDIR=$(dirname "$0")
if [ -e "$BASEDIR/hyperion.key" ]; then
    #Not generating a new certificate, because one already exists. To generate a new one, manualy remove hyperion.key and ruin this script again.
    echo "Skipping certificate generation"
    exit 1;
fi

# generate root certificate
# root certificates have a max validity of 397 days. i generate one for 390 days. 
# after that time you have to run the script again to renew the certificate.
# (and upload a new build to all your devices)
openssl req -x509 -nodes -new -sha256 -days 390 -newkey rsa:2048 -keyout "$BASEDIR/RootCA.key" -out "$BASEDIR/RootCA.pem" -subj "/C=de/CN=hyperion.local"
openssl x509 -outform pem -in "$BASEDIR/RootCA.pem" -out "$BASEDIR/RootCA.crt"

#generate certificate
openssl req -new -nodes -newkey rsa:2048 -keyout "$BASEDIR/hyperion.key" -out "$BASEDIR/hyperion.csr" -subj "/C=de/ST=State/L=City/O=Organization/CN=hyperion.local"
openssl x509 -req -sha256 -days 1024 -in "$BASEDIR/hyperion.csr" -CA "$BASEDIR/RootCA.pem" -CAkey "$BASEDIR/RootCA.key" -CAcreateserial -extfile "$BASEDIR/vhosts_domains.ext" -out "$BASEDIR/hyperion.crt"

