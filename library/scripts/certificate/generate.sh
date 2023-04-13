# generate root certificate
# root certificates have a max validity of 397 days. i generate one for 390 days. 
# after that time you have to run trust stript again to renew the certificate.
# (and upload a new build to all your devices)
openssl req -x509 -nodes -new -sha256 -days 390 -newkey rsa:2048 -keyout "RootCA.key" -out "RootCA.pem" -subj "/C=de/CN=hyperion.local"
openssl x509 -outform pem -in "RootCA.pem" -out "RootCA.crt"

#generate certificate
openssl req -new -nodes -newkey rsa:2048 -keyout hyperion.key -out hyperion.csr -subj "/C=de/ST=State/L=City/O=Organization/CN=hyperion.local"
openssl x509 -req -sha256 -days 1024 -in hyperion.csr -CA RootCA.pem -CAkey RootCA.key -CAcreateserial -extfile vhosts_domains.ext -out hyperion.crt

#add to keychain
sudo security add-trusted-cert -d -r trustRoot -k /Library/Keychains/System.keychain hyperion.crt
sudo security add-trusted-cert -d -r trustRoot -k /Library/Keychains/System.keychain RootCA.crt
