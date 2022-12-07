bazel query --noimplicit_deps 'deps(//src/pso/main:get_moves) except @boost//...:*' --output graph > graph.in
dot -Tpng < graph.in > graph.png
rm graph.in