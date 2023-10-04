#add to keychain
sudo security add-trusted-cert -d -r trustRoot -k /Library/Keychains/System.keychain hyperion.crt
sudo security add-trusted-cert -d -r trustRoot -k /Library/Keychains/System.keychain RootCA.crt
