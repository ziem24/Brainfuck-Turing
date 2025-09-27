TIMEFORMAT="Compilation time: %Rs"
time {
    cd build
    ./main "$@"
    cd ..
}