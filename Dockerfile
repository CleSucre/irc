FROM ubuntu:latest

LABEL authors="CleSucre"

RUN apt-get update && apt-get install libssl-dev vim g++ valgrind make -y

COPY ./irc /app
COPY ./start.sh /app

WORKDIR /app

RUN chmod +x start.sh

ENTRYPOINT ["./start.sh"]