#!/bin/bash

PRJ_SRC_DIR=$(dirname `realpath $0`)
BUILD_DIR=$PRJ_SRC_DIR/../build

build_front_end()
{
    echo "building front end"
    mkdir -p "$BUILD_DIR/front-end-build"
    docker run --rm -v "${PRJ_SRC_DIR}/front-end/poker_game/":/src -v "${BUILD_DIR}/front-end-build/":/build_dir  -u 0:0 marklion/front-web-assembly-build:v1.0
    [ $? != 0 ] && exit 1
}

build_back_end()
{
    echo "building back end"
    mkdir -p "$BUILD_DIR/back-end-build"
    docker run --rm -v "${PRJ_SRC_DIR}/back-end/user_manage_rest_9098/user_manage":/rest_source -v "${BUILD_DIR}/back-end-build:/rest_build"  marklion/back-end-build:v1.0 /root/build-rest.sh
    [ $? != 0 ] && exit 1
    docker run --rm -v "${PRJ_SRC_DIR}":/game_build -w /game_build/back-end/game_tcp_50050 marklion/back-end-build:v1.0 make
    [ $? != 0 ] && exit 1
    mv "${PRJ_SRC_DIR}/back-end/game_tcp_50050/game" "${BUILD_DIR}/back-end-build"
}

prepare_docker()
{
    echo "prepare docker"
    docker pull marklion/front-web-assembly-build:v1.0
    docker pull marklion/back-end-build:v1.0
    docker pull marklion/poker_deploy:v1.0
}
make_package()
{
    local timestamp=`date +%Y%m%d%H%M%S`
    local package_path="$BUILD_DIR/package-$timestamp"
    mkdir -p $package_path

    cp ${BUILD_DIR}/front-end-build/poker_game.html   ${package_path}/index.html
    cp ${BUILD_DIR}/front-end-build/poker_game.js ${package_path}
    cp ${BUILD_DIR}/front-end-build/poker_game.wasm    ${package_path}
    cp ${BUILD_DIR}/front-end-build/qtloader.js ${package_path}
    cp ${BUILD_DIR}/front-end-build/qtlogo.svg ${package_path}
    mkdir -p $package_path/resource
    cp ${PRJ_SRC_DIR}/back-end/resouce_http/* -a ${package_path}/resource
    mkdir -p $package_path/game_server
    cp $BUILD_DIR/back-end-build/game $package_path/game_server
    mkdir -p $package_path/rest
    cp $BUILD_DIR/back-end-build/deploy/share/ngrest/services/* ${package_path}/rest

    pushd $BUILD_DIR > /dev/null
    tar zcf "${package_path}.tar.gz" "package-$timestamp" > /dev/null
    popd > /dev/null
    realpath "${package_path}.tar.gz"
}
main()
{
    if [ "$1" == "pack" ]
    then
        make_package
        return $?
    fi
    prepare_docker > /dev/null
    if [ "$1" == "front-end" ] || [ "$1" == "" ]
    then
        build_front_end > /dev/null
    fi
    if [ "$1" == "back-end" ] || [ "$1" == "" ]
    then
        build_back_end > /dev/null
    fi
}

main $@
