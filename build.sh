#!/bin/bash

PRJ_SRC_DIR=$(dirname `realpath $0`)
BUILD_DIR=$PRJ_SRC_DIR/../build

build_front_end()
{
    echo "building front end"
    mkdir -p "$BUILD_DIR/front-end-build"
    docker run --rm -v "${PRJ_SRC_DIR}/front-end/poker_game/":/src -v "${BUILD_DIR}/front-end-build/":/build_dir  -u 0:0 front-web-assembly-build:latest
}

build_back_end()
{
    echo "building back end"
    mkdir -p "$BUILD_DIR/back-end-build"
    docker run --rm -v "${PRJ_SRC_DIR}/back-end/user_manage_rest_9098/user_manage":/rest_source -v "${BUILD_DIR}/back-end-build:/rest_build"  back-end-build:latest /root/build-rest.sh
    docker run --rm -v "${PRJ_SRC_DIR}":/game_build -w /game_build/back-end/game_tcp_50050 back-end-build:latest make
    mv "${PRJ_SRC_DIR}/back-end/game_tcp_50050/game" "${BUILD_DIR}/back-end-build"
}

prepare_docker()
{
    echo "prepare docker"
}
main()
{
    prepare_docker
    if [ "$1" == "front-end" ] || [ "$1" == "" ]
    then
        build_front_end
    fi
    if [ "$1" == "back-end" ] || [ "$1" == "" ]
    then
        build_back_end
    fi
}

main $@
