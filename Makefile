do: build
	cd build && make -j32

upload: do
	./upload.sh

build:
	mkdir build 
	cd build && cmake ..

clean:
	rm -rf build
