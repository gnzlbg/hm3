#!/bin/bash
echo "Configuring HOM3..."

rm -rf build
mkdir build
cd build

export CC=/usr/local/bin/clang
export CXX=/usr/local/bin/clang++

export CMAKE_FLAGS="../ -DCMAKE_CXX_FLAGS=\"-stdlib=libc++ -I${LIBCXX_INCLUDE}\" -DCMAKE_CXX_COMPILE_FLAGS=\"-nostdinc++\" -DCMAKE_EXE_LINKER_FLAGS=\"-L${LIBCXX_LIB}\""

echo "Running cmake..."
while getopts "drcsfahx" option
do
    case "${option}" in
        h)
            echo "HOM3 build script. Following build modes are available:"
            echo "-d    Debug"
            echo "-r    Release"
            echo "-f    Fast (Release without assertions)"
            echo "-a    Asan"
            echo "-c    Coverage"
            echo "-m    Msan"
            echo "-t    Tsan"
            exit 1
            ;;
        d)
            eval "cmake $CMAKE_FLAGS -DCMAKE_BUILD_TYPE=Debug"
            echo "... hom3-debug configuration finished!"
            exit 1
            ;;
        s)
            eval "cmake $CMAKE_FLAGS -DCMAKE_BUILD_TYPE=Debug -DH3_WITH_ASAN=Off"
            echo "... hom3-debug no-sanitiziers configuration finished!"
            exit 1
            ;;
        r)
            eval "cmake $CMAKE_FLAGS -DCMAKE_BUILD_TYPE=Release"
            echo "... hom3-release configuration finished!"
            exit 1
            ;;
        f)
            eval "cmake $CMAKE_FLAGS -DCMAKE_BUILD_TYPE=Release -DH3_ENABLE_DEBUG_INFO=Off -DH3_ENABLE_ASSERTIONS=Off -DH3_WITH_ASAN=Off"
            echo "... hom3-release configuration finished!"
            exit 1
            ;;
        c)
            eval "cmake $CMAKE_FLAGS -DCMAKE_BUILD_TYPE=Debug -DH3_COLLECT_COVERAGE=On"
            echo "... hom3-coverage configuration finished!"
            exit 1
            ;;
    esac
done
