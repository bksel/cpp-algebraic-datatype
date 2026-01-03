# CXX=g++
CXX=clang++

build_dir=build_make
include_dir=inc
source_dir=src
binary_name=adt

run: build
	./$(build_dir)/$(binary_name)

build: setup
	$(CXX) -std=c++17 -I$(include_dir) -o $(build_dir)/$(binary_name) $(source_dir)/main.cpp -I $(include_dir)

setup:
	mkdir -p $(build_dir)
	echo "*" > $(build_dir)/.gitignore

clean:
	rm -rf $(build_dir)

.PHONY: run build setup clean