# IRC

## Install lib for SSL support

```sh
sudo apt-get install libssl-dev
```

## Generate SSL certificat

```sh
openssl req -x509 -newkey rsa:2048 -keyout key.pem -out cert.pem -days 365 -nodes
```

## Connect to IRC without SSL support using `netcat`

```sh
nc -C ADDRESS PORT
```

## Connect to IRC without SSL support using `telnet`

```sh
telnet ADDRESS PORT
```

## Connect to IRC with SSL support

```sh
openssl s_client -connect ADDRESS:PORT
```
