FROM ubuntu:18.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    curl \
    qt4-default \
    libqt4-dev \
    libqt4-svg \
    libqtwebkit-dev \
    libhunspell-dev \
    gdb \
    xvfb \
    && rm -rf /var/lib/apt/lists/*

RUN sh -c "$(curl --location https://taskfile.dev/install.sh)" -- -d -b /usr/local/bin

ENV QT_X11_NO_MITSHM=1
