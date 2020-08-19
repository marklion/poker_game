#!/bin/bash

echo "deploying"

PRJ_SRC_DIR=$(dirname `realpath $0`)
PRJ_DEPLOY_DIR=$PRJ_SRC_DIR/../deploy
BUILD_DIR=$PRJ_SRC_DIR/../build
PRJ_DATABASE_DIR=$PRJ_SRC_DIR/../database

rm -rf $PRJ_DEPLOY_DIR
mkdir -p $PRJ_DEPLOY_DIR

front_end_deploy()
{
    echo "front"
    cp ${BUILD_DIR}/front-end-build/* -a ${PRJ_DEPLOY_DIR}/
}
resouce_deploy()
{
    echo "resource"
    mkdir -p $PRJ_DEPLOY_DIR/resource
    cp ${PRJ_SRC_DIR}/back-end/resouce_http/* -a ${PRJ_DEPLOY_DIR}/resource

}
game_deploy()
{
    echo "game"
    mkdir -p $PRJ_DEPLOY_DIR/game_server
    cp $BUILD_DIR/back-end-build/game $PRJ_DEPLOY_DIR/game_server
}
user_mng_deploy()
{
    echo "user_mng"
    mkdir -p ${PRJ_DEPLOY_DIR}/rest
    cp $BUILD_DIR/back-end-build/deploy/share/ngrest/services/* ${PRJ_DEPLOY_DIR}/rest
}
start_server()
{
    docker rm -f `docker ps -a | grep poker_deploy | awk '{print $1}'`
    docker run -p 80:80 -d -v "$PRJ_DEPLOY_DIR":/web  -v "$PRJ_DATABASE_DIR":/database marklion/poker_deploy:v1.0 /root/run-server.sh
}
main()
{
    if [ -f "$1" ]
    then
        tar xf "$1" -C $PRJ_DEPLOY_DIR
        cp $PRJ_DEPLOY_DIR/package*/* $PRJ_DEPLOY_DIR/ -a
        if [ -d "$2" ]
        then
            PRJ_DATABASE_DIR="$2"
        fi
    else
        front_end_deploy
        resouce_deploy
        game_deploy
        user_mng_deploy
    fi
    start_server
}

main $@
