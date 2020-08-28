#if !defined(_GAME_ROOM_H_)
#define _GAME_ROOM_H_

class game_room
{
private:
    int m_room_no = 0;
    /* data */
public:
    game_room(/* args */){}
    ~game_room(){}
    void SetRoomNo(int _room_no) {
        m_room_no = _room_no;
    }
    int GetRoomNo() {
        return m_room_no;
    }
};

#endif // _GAME_ROOM_H_
