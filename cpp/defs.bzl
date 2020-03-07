BENCHMARK_COPTS = ["-O3", "-DNDEBUG"]
EXEC_COPTS = ["-O3", "-DNDEBUG"]
EW_MOVE_FINDER_CACHE_COPTS = ["-O3", "-DMOVE_FINDER_CACHE"]

def shared_library(name, deps = []):
  return native.cc_library(
    name=name,
    srcs=get_src_list(name),
    hdrs=get_hdr_list(name),
    deps=deps,
    visibility=["//visibility:public"],
    copts=EXEC_COPTS
  )

def gen_test_rules(deps):
  for file in native.glob(["Test*.cpp", "**/Test*.cpp"]):
    native.cc_test(
      name=file.replace(".","_"),
      srcs=[file],
      deps=deps
    )


def get_src_list(my_str):
  return native.glob(
    [my_str + "/*.cpp", my_str + "/**/*.cpp"],
    exclude=[my_str + "/test/**"]
  )

def get_hdr_list(my_str):
  ext = ["h", "tpp", "hpp"]
  arr = [my_str + "/*." + x for x in ext] + [my_str + "/**/*." + x for x in ext]
  return native.glob(
    arr,
    exclude = ["board/test/**"]
  )