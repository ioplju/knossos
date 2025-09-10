# syntax=docker/dockerfile:1

# TODO: we should use an alpine or something reproductible but create_appimage.sh assumes a archlinux
FROM archlinux:base-devel AS build
LABEL org.opencontainers.image.authors="Alexandre Baconnet <alexandre.baconnet@ariadne.ai>"

# build deps
# qt modules when building appimage
# tools used in create_appimage.sh
# hack to make the glibc package exist in /var/cache/pacman/pkg
RUN pacman-key --init && pacman -Syu --noconfirm \
    git python3 qt5-base boost snappy quazip-qt5 qtkeychain-qt5 glu \
    gst-plugins-bad-libs postgresql-libs unixodbc freetds mariadb-libs \
    expac patchelf \
    glibc

# WARN: Bad security-wise but makepkg wants a non-root sudoer to run it
RUN useradd -m builder && echo "builder ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers
USER builder

# TODO: Bad security-wise to use the AUR to get a package unless we are making the PKGBUILD (are we?)
RUN cd /home/builder && git clone "https://aur.archlinux.org/yay-bin.git" && cd yay-bin && makepkg -si --noconfirm && yay -S pythonqt-knossos-git --noconfirm && cd .. && rm -rf yay-bin

# building knossos
WORKDIR /home/builder
SHELL ["/bin/bash", "-c"]
CMD mkdir -p build && cd build &&  cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -G Ninja -DCMAKE_BUILD_TYPE=RELEASE ../knossos && ninja \
    # APPIMAGE_EXTRACT_AND_RUN=1 because appimage don't work within a docker image
    && APPIMAGE_EXTRACT_AND_RUN=1 ../knossos/installer/create_appimage.sh \
    && mv compile_commands.json ../knossos/
