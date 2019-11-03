DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
ROOT="$( cd "${DIR}"; cd ..; pwd )"

cd "${DIR}";
if [[ -d "aws-context" ]]; then rm -rf aws-context; fi;
mkdir aws-context

docker run --rm -it -v "${ROOT}:/project" -v "${DIR}/aws-context:/out" domslee/fedora-bazel bash -c "bazel clean && bazel build //src:ew && cp bazel-bin/src/ew /out/ew"
