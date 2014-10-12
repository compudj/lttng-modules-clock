How to build this clock plugin example module against lttng-modules:

make LTTNG_MODULES_DIR=/path/to/lttng-modules KERNELDIR=/path/to/linux/tree

Setting the variable KERNELDIR is only required if you build against a
kernel different from the one currently running. See lttng-modules
README.md file for more details about KERNELDIR.

Load the clock plugin before lttng-modules lttng-tracer if you want the
tracer to use your clock plugin.
