
set -ex;
srcs=(~/cpp_bin/*);

for x in "${srcs[@]}"; do
  if test -f "${x}"; then
    echo "OK file ${x}"
  else
    echo "file ${x} does not exist"
    exit 1;
  fi
done

for x in "${srcs[@]}"; do
  ./main.py --binary "$x";
done
