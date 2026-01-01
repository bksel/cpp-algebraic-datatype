build_directory := "builddir"

default: run

setup:
    meson setup --reconfigure {{build_directory}}

build: setup
    meson compile -C {{build_directory}}

run: build
    builddir/adt

clean:
    rm -r builddir

help:
    @echo "Justfile commands:"
    @echo "  just run      - Compile and run the project"
    @echo "  just compile  - Compile the project"
    @echo "  just init     - Initialize or reconfigure the build directory"
    @echo "  just clean    - Remove the build directory"
