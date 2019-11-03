FROM fedora:31

RUN set -e; \
  dnf install gcc -y;

RUN set -e; \
  dnf install dnf-plugins-core -y; \
  dnf copr enable vbatts/bazel -y; \
  dnf install bazel -y

RUN set -e; \
  dnf install git -y;

RUN set -e; \
  dnf install gcc-g++ -y;

WORKDIR /project
