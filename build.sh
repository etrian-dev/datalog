#!/bin/env bash

set -eux

meson compile -C build
meson compile -C build doxygen
