FROM ubuntu:24.04

LABEL authors="CleSucre"

RUN apt-get update && apt-get install libssl-dev vim g++ valgrind make -y

COPY ./irc /app
COPY ./start.sh /app/start.sh

WORKDIR /app

RUN make

RUN chmod +x /app/start.sh

ENTRYPOINT ["/app/start.sh"]