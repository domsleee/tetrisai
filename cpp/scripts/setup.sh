#!/usr/bin/env bash
echo "${BASH_SOURCE[0]}"
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )";
PARENT_DIR=$( cd ${DIR}/.. && pwd )

echo "DIR: ${DIR}"
echo "${PARENT_DIR}"
echo "WORKSPACE_DIR='$PARENT_DIR'" > "${PARENT_DIR}/path.bzl"