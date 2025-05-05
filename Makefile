all: run

build:
	@mkdir -p build && cd build && cmake .. && make

run:
	cd build && ./fsmtool

clean:
	@rm -rf build

doxygen:
	doxygen Doxyfile

pack:
	make clean
	mkdir -p package && cp -r src examples README.txt doc Makefile package/
	zip -r xname01-xname02.zip package
