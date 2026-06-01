.PHONY: all configure build clean

all: build

configure:
	cmake -S . \
		-B build \
		-G Ninja \
		-DCMAKE_BUILD_TYPE=Debug \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
		-DCMAKE_TOOLCHAIN_FILE=/home/Fion1x/.nix-profile/share/vcpkg/scripts/buildsystems/vcpkg.cmake \
		-DVCPKG_TARGET_TRIPLET=x64-linux \
		-DVCPKG_MANIFEST_MODE=ON \
		-DVCPKG_EXECUTABLE=$$(which vcpkg) \
		-DCMAKE_CXX_COMPILER=clang++

build:
	cmake --build build --parallel $$(nproc)

clean:
	rm -rf build
