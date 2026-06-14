FROM debian:bookworm-slim

ENV DEBIAN_FRONTEND=noninteractive
ENV LC_ALL=C.UTF-8

RUN apt-get update && apt-get install -y --no-install-recommends \
        autoconf automake autopoint bash bison bzip2 flex g++ g++-multilib \
        gettext git gperf intltool libc6-dev-i386 libgdk-pixbuf2.0-dev \
        libltdl-dev libssl-dev libtool-bin libxml-parser-perl lzip make \
        openssl p7zip-full patch perl python3 python3-mako python3-pkg-resources \
        python-is-python3 ruby sed unzip wget xz-utils ca-certificates \
        cmake pkg-config rsync curl sudo file \
 && rm -rf /var/lib/apt/lists/*

RUN sh -c "$(curl --location https://taskfile.dev/install.sh)" -- -d -b /usr/local/bin

# Qt4 is deprecated upstream; if `make qt` fails after bumping MXE_REF,
# roll back to a commit predating the Qt4 package removal.
ARG MXE_REF=master
ENV MXE_DIR=/opt/mxe
ENV MXE_TARGET=x86_64-w64-mingw32.static

RUN git clone https://github.com/mxe/mxe.git ${MXE_DIR} \
 && cd ${MXE_DIR} \
 && git checkout ${MXE_REF} \
 && sed -i '/^$(PKG)_DEPS/s| postgresql||' src/qt.mk \
 && sed -i '/^$(PKG)_DEPS/s| openssl||' src/qt.mk \
 && sed -i 's|-qt-sql-psql|-no-sql-psql|g' src/qt.mk \
 && sed -i 's|-openssl-linked|-no-openssl|g' src/qt.mk

RUN cd ${MXE_DIR} \
 && make MXE_TARGETS="${MXE_TARGET}" \
         qt nsis cmake \
         -j$(nproc) JOBS=$(nproc) \
 && rm -rf ${MXE_DIR}/pkg ${MXE_DIR}/log ${MXE_DIR}/.ccache

# ccache.cmake redirects CMAKE_C_COMPILER through /opt/mxe/.ccache/ wrappers;
# we deleted .ccache above, so the override points at non-existent paths. Drop
# the snippet instead of keeping ccache (one-off builds don't benefit from it).
RUN rm -f ${MXE_DIR}/usr/${MXE_TARGET}/share/cmake/mxe-conf.d/ccache.cmake

ENV PATH=${MXE_DIR}/usr/bin:${PATH}

# Match host UID/GID so bind-mounted /src artifacts aren't owned by root.
ARG USERNAME=builder
ARG USER_UID=1000
ARG USER_GID=1000
RUN groupadd --gid ${USER_GID} ${USERNAME} \
 && useradd --uid ${USER_UID} --gid ${USER_GID} -m -s /bin/bash ${USERNAME} \
 && echo "${USERNAME} ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/${USERNAME} \
 && chmod 0440 /etc/sudoers.d/${USERNAME}

USER ${USERNAME}
WORKDIR /src
