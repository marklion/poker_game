#!/bin/bash

PRJ_SRC_DIR=$(dirname `realpath $0`)
BUILD_DIR=$PRJ_SRC_DIR/../build

build_front_end()
{
    echo "building front end"
    mkdir -p "$BUILD_DIR/front-end-build"
    docker run --rm -v "${PRJ_SRC_DIR}/front-end/poker_game/":/src -v "${BUILD_DIR}/front-end-build/":/build_dir  -u 0:0 marklion/front-web-assembly-build:v1.0
}

build_back_end()
{
    echo "building back end"
    mkdir -p "$BUILD_DIR/back-end-build"
    docker run --rm -v "${PRJ_SRC_DIR}/back-end/user_manage_rest_9098/user_manage":/rest_source -v "${BUILD_DIR}/back-end-build:/rest_build"  marklion/back-end-build:v1.0 /root/build-rest.sh
    docker run --rm -v "${PRJ_SRC_DIR}":/game_build -w /game_build/back-end/game_tcp_50050 marklion/back-end-build:v1.0 make
    mv "${PRJ_SRC_DIR}/back-end/game_tcp_50050/game" "${BUILD_DIR}/back-end-build"
}

prepare_docker()
{
    echo "prepare docker"
    docker pull marklion/front-web-assembly-build:v1.0
    docker pull marklion/back-end-build:v1.0
    docker pull marklion/poker_deploy:v1.0
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
