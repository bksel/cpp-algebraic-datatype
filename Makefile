# CXX=g++
CXX=clang++

build_dir=build_make
binary_name=adt

run: build
	./$(build_dir)/$(binary_name)

build: setup
	$(CXX) -std=c++17 -o $(build_dir)/$(binary_name) src/main.cpp

setup:
	mkdir -p $(build_dir)
	echo "*" > $(build_dir)/.gitignore

clean:
	rm -rf $(build_dir)

.PHONY: run build setup clean