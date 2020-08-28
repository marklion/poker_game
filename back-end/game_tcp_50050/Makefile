all: game

game:game_msg_router.cpp pk_game.pb.cc main.cpp game_table.cpp game_msg_processor.cpp timer_channel.cpp ../../front-end/poker_game/msg-encode-utils/*.cpp
	g++ -std=gnu++11 -Wall -g $^ -o $@ -lzinx -lhiredis -lprotobuf

test:test_pattern.cpp game_table.cpp game_msg_processor.cpp timer_channel.cpp ../../front-end/poker_game/msg-encode-utils/*.cpp
	g++ -std=gnu++11 -Wall -g $^ -o $@ -lzinx -lhiredis