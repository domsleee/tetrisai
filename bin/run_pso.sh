DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
ROOT="$( cd ${DIR}; cd ..; pwd )";

echo "${ROOT}"
cd "${ROOT}"
python3 ./python/local.py ./bazel-bin/src/ew
