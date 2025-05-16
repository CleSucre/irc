FROM ubuntu:latest

LABEL authors="CleSucre"

RUN apt-get update && apt-get install vim g++ valgrind make -y

ENTRYPOINT ["/bin/bash"]