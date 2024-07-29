#!/usr/bin/env bash

RELEASE=$1

cd ${MAIN_DIR}

git archive --format=tgz --prefix=chunks/ -o chunks-${RELEASE}.tgz HEAD:src/

