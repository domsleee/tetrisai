#!/usr/bin/env bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

cd "${DIR}";
cp amazonlinux-ext/Dockerfile aws-context/Dockerfile;
cd aws-context;
docker build -t amazonlinux-ew .;
