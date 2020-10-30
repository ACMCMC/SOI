FROM ubuntu:20.10

COPY . /carrerascriticas

WORKDIR /carrerascriticas

RUN apt-get update
RUN apt-get install -y make
RUN make

CMD ["./estadisticas.out", "0", "500", "5000", "0", "500", "5000"]