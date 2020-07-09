#!/bin/bash

nginx
/etc/init.d/redis-server start
/web/game_server/game &


LD_LIBRARY_PATH=/usr/local/lib /root/.ngrest/ngrest-build/deploy/bin/ngrestserver -s /web/rest
