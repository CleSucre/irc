FROM ubuntu:latest

LABEL authors="CleSucre"

RUN apt-get update && apt-get install libssl-dev vim g++ valgrind make -y

ENTRYPOINT ["/bin/bash"]