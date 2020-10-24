FROM alpine:3.12

RUN apk add --no-cache build-base python3 gcovr

COPY . /app

WORKDIR /app/tarefa01

RUN make
