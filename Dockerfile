FROM alpine:latest

EXPOSE 6381

RUN apk add \
    build-base \
    ninja \
    git \
    unzip \
    cmake \
    g++ \
    libstdc++ \
    pkgconfig

RUN apk add \
    lame-dev \
    opus-dev \
    libopusenc-dev

RUN apk add \
    guile-dev \
    msgpack-c-dev \
    yaml-dev

RUN git clone https://github.com/tlsa/libcyaml.git /tmp/libcyaml \
    && cd /tmp/libcyaml \
    && make \
    && make install

RUN git clone --branch main --single-branch https://github.com/racslabs/racs.git \
    && rm -rf /racs/.git

WORKDIR /racs

RUN cmake -B build -G Ninja && cmake --build build --target racs -j 10

RUN cp build/racs /usr/local/bin/racs

COPY conf.yaml /etc/racs/conf.yaml

ENTRYPOINT ["/usr/local/bin/racs"]

CMD ["--config", "/etc/racs/conf.yaml"]