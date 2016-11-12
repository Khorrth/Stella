#!/bin/sh

gcc engine/sources/extended-stella-example.c \
    engine/sources/stella.c \
    engine/sources/xdg-shell-protocol.c\
    -I engine/sources/ \
    -o engine/binaries/extended-stella-example\
    -g \
    $(pkg-config --libs --cflags gtk+-3.0 wayland-client cairo)
