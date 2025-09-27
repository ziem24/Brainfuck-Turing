TIMEFORMAT="Build time: %Rs"
time {
    if [ -x build/main ]; then
        rm build/main
    fi
    cmake -S . -B build -Wno-dev
    cmake --build build
    if [ "$1" != "no-tests" ]; then
        cd build
        ctest --output-on-failure
        cd ..
    fi
    if [ -x build/main ]; then
        echo "Build finished without errors. Use 'bash run.sh path/to/file' to run the program."
    else
        echo "Build unsuccessful!!!"
    fi
}