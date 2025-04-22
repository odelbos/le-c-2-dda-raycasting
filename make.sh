#!/bin/sh

CC=gcc
CFLAGS="-Wall -Wextra"

echo "Compiling ..."

set -xe

$CC $CFLAGS -o raycast main.c
