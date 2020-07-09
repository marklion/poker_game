#!/bin/bash

echo "deploying"

PRJ_SRC_DIR=$(dirname `realpath $0`)
PRJ_DEPLOY_DIR=$PRJ_SRC_DIR/../deploy
BUILD_DIR=$PRJ_SRC_DIR/../build

rm -rf $PRJ_DEPLOY_DIR
mkdir -p $PRJ_DEPLOY_DIR

front_end_deploy()
{
    echo "front"
    cp ${BUILD_DIR}/front-end-build/poker_game.html   ${PRJ_DEPLOY_DIR}/index.html
    cp ${BUILD_DIR}/front-end-build/poker_game.js ${PRJ_DEPLOY_DIR}
    cp ${BUILD_DIR}/front-end-build/poker_game.wasm    ${PRJ_DEPLOY_DIR}
    cp ${BUILD_DIR}/front-end-build/qtloader.js ${PRJ_DEPLOY_DIR}
    cp ${BUILD_DIR}/front-end-build/qtlogo.svg ${PRJ_DEPLOY_DIR}
    cp ${BUILD_DIR}/front-end-build/poker_game.worker.js  ${PRJ_DEPLOY_DIR}
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
    docker run -p 80:80 -d -v /poker_game_project/deploy/:/web  marklion/poker_deploy:v1.0 /root/run-server.sh
}
main()
{
    front_end_deploy
    resouce_deploy
    game_deploy
    user_mng_deploy
    start_server
}

main
