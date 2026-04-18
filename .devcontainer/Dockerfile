FROM ubuntu:18.04

ENV DEBIAN_FRONTEND=noninteractive

# Enable ppa:git-core/ppa so we get a current git on the EOL Bionic base.
# Ubuntu 18.04 ships git 2.17 (2018); the PPA ships 2.50+ for Bionic.
RUN apt-get update && apt-get install -y --no-install-recommends \
        software-properties-common gnupg ca-certificates \
    && add-apt-repository -y ppa:git-core/ppa \
    && apt-get update && apt-get install -y \
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
    sudo \
    xsltproc \
    fop \
    docbook-xsl \
    docbook-xsl-ns \
    && rm -rf /var/lib/apt/lists/*

RUN sh -c "$(curl --location https://taskfile.dev/install.sh)" -- -d -b /usr/local/bin

ARG USERNAME=vscode
ARG USER_UID=1000
ARG USER_GID=1000
RUN groupadd --gid $USER_GID $USERNAME \
 && useradd --uid $USER_UID --gid $USER_GID -m -s /bin/bash $USERNAME \
 && echo "$USERNAME ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/$USERNAME \
 && chmod 0440 /etc/sudoers.d/$USERNAME
USER $USERNAME

ENV QT_X11_NO_MITSHM=1
