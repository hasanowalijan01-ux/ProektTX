# Combined builder image for both server and client (CLI build).
# Uses Ubuntu with Qt6 packages.
FROM ubuntu:24.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential cmake git \
        qt6-base-dev qt6-base-dev-tools \
        libqt6sql6-sqlite libqt6network6 libqt6gui6 libqt6widgets6 \
        qt6-tools-dev qt6-tools-dev-tools \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY . .

RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_CLIENT=OFF \
 && cmake --build build -j

# ---- runtime stage (server only by default) ----
FROM ubuntu:24.04 AS server
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
        libqt6core6 libqt6network6 libqt6sql6 libqt6sql6-sqlite \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /app
COPY --from=builder /src/build/server/pd_server /app/pd_server
EXPOSE 5555
VOLUME ["/data"]
CMD ["/app/pd_server", "--port", "5555", "--db", "/data/pd_server.db"]
