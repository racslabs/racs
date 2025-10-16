FROM alpine:latest

ARG GITHUB_TOKEN="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"
ARG REPO_URL="github.com/racslabs/racs.git"

# Core build tools
RUN apk add --no-cache \
    build-base \
    ninja \
    git \
    unzip \
    cmake \
    g++ \
    libstdc++ \
    pkgconfig

# Audio libs
RUN apk add --no-cache \
    lame-dev \
    opus-dev \
    libopusenc-dev

# Other dependencies
RUN apk add --no-cache \
    guile-dev \
    msgpack-c-dev \
    yaml-dev

RUN git clone https://github.com/tlsa/libcyaml.git /tmp/libcyaml \
    && cd /tmp/libcyaml \
    && make \
    && make install

# Clone private repo
RUN git clone --branch dev --single-branch https://${GITHUB_TOKEN}@${REPO_URL} \
    && rm -rf /racs/.git

WORKDIR /racs
RUN ls -l .
# Build
RUN cmake -B build -G Ninja && cmake --build build --target racs -j 10

# Copy binary (inside container, single-stage)
RUN cp build/racs /usr/local/bin/racs

# Copy config
COPY conf.yaml /etc/racs/conf.yaml

# Runtime
ENTRYPOINT ["/usr/local/bin/racs"]
CMD ["--config", "/etc/racs/conf.yml"]