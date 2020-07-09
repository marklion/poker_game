#ifndef GAME_VIEW_H
#define GAME_VIEW_H

#include <QObject>
#include <QLabel>
#include <QWidget>
#include <QMainWindow>
#include "game_control.h"
#include <QLineEdit>
#include <QPushButton>
#include <QSlider>
#include <QTimer>


class refresh_view {
public:
    virtual void refresh() = 0;
};
class game_sub_view_last_result:public QWidget, public refresh_view
{
public:
    explicit game_sub_view_last_result(QWidget *parent);
private:
    QLabel m_pattern_label[5];
    QLabel m_pattern_type_label;
    int m_seat = -1;

    // refresh_view interface
    public:
        void refresh();
        void SetSeat(int _seat);
};
class game_sub_view_operate : public QWidget, public refresh_view
{
    Q_OBJECT
public:
    explicit game_sub_view_operate(QWidget *parent = NULL);
private:
    QLabel m_hand_cards_lable[2];
    QLabel m_curCash_lable;
    QLabel m_batCash_lable;
    QPushButton m_bat_btn;
    QPushButton m_fall_btn;
    QPushButton m_follow_btn;
    QPushButton m_cash_full_pool_btn;
    QPushButton m_cash_half_pool_btn;
    QPushButton m_cash_double_pool_btn;
    QSlider m_cash_sld;
    QLabel m_username_lable;
    QLabel m_SeatPos_lable;
    QPushButton m_ready_btn;
    QLabel m_count_down_lable;
    void _set_bat_cash(int _bat_cash);
    game_sub_view_last_result *m_result = nullptr;
    QTimer m_timer;
private slots:
    void proc_full_pool_btn();
    void proc_half_pool_btn();
    void proc_double_pool_btn();
    void proc_slide_move(int _value);
    void proc_bat_btn();
    void proc_fall_btn();
    void proc_flw_btn();
    void proc_ready_btn();
    void show_count_down();
    void hide_count_down();

    // refresh_view interface
public:
    void refresh();
};

class game_sub_view_otherPlayer;

class game_view : public QMainWindow, public refresh_view
{
    Q_OBJECT
public:
    explicit game_view(QWidget *parent = nullptr);
    ~game_view();
private:
    QLineEdit m_table_input;
    QPushButton m_follow_btn;
    QPushButton m_create_btn;
    QPushButton m_cancle_btn;
    QLabel m_cur_table_no;
    QLabel m_cur_pool;
    QList<refresh_view *> m_sub_view;
    game_sub_view_otherPlayer *m_pastOtherplayer[5];
signals:


    // refresh_view interface
public:
    void refresh();

private slots:
    void proc_follow_table();
    void proc_create_table();
    void proc_cancle();
};

class game_sub_view_otherPlayer:public QWidget, public refresh_view
{
public:
    explicit game_sub_view_otherPlayer(QWidget *parent = nullptr);
    void SetSeat(int _seat);
private:
    int m_iSeat;
    QLabel m_name_label;
    QLabel m_curCash_label;
    QLabel m_batCash_label;
    QLabel m_seat_label;
    QLabel m_ready_label;
    QLabel m_isAction_label;
    QLabel m_isFalled_label;
    game_sub_view_last_result *m_result_wid = NULL;
    QTimer m_timer;
    void hide_count_down();
    void show_count_down();

    // refresh_view interface
public:
    void refresh();
};

class game_sub_view_pub_cards:public QWidget, public refresh_view
{
public:
    explicit game_sub_view_pub_cards(QWidget *parent);
private:
    QLabel m_pub_cards_label[5];

    // refresh_view interface
public:
    void refresh();
};



#endif // GAME_VIEW_H
