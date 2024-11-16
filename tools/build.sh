#!/bin/bash

build_type_var=debug

make BUILD_TYPE=${ build_type_var } clean
make