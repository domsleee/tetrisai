DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
ROOT="$( cd '${DIR}'; cd ..; pwd )";

docker run --rm -v ${ROOT}:/project --cap-add=SYS_PTRACE --security-opt seccomp=unconfined -it domslee/bazel:ubuntu-18.04-gdb bash
