FROM debian:latest

RUN apt-get update && apt-get install -y clang protobuf-compiler make python3

WORKDIR /work

# run this if you might have compiled it on another platform
# RUN make re

ENTRYPOINT "./entrypoint.sh"