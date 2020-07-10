#include "game_view.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsGridLayout>
#include <qgraphicslayout.h>
#include <QSize>


void show_cards_img_inLabel(QLabel &_lab, game_msg_card *_card)
{
    QString color_map[game_card_invalid] = {
        [game_card_heart] = "heart",
        [game_card_spade] = "spade",
        [game_card_diamond] = "diamond",
        [game_card_club] = "club"
    };
    if (_card->number == 0)
    {
        QString filename = ":/cards_imgs/little_joker.jpg";
        _lab.setPixmap(QPixmap(filename));
    }
    else
    {
        QString filename = QString(":/cards_imgs/") + color_map[_card->color] + "_" + QString::number(_card->number) + ".jpg";
        _lab.setPixmap(QPixmap(filename));
    }
}

game_view::game_view(QWidget *parent) : QMainWindow(parent)
{
    game_control::GetController()->AddView(this);
    this->setWindowTitle(tr("游戏"));
    auto cenwid = new QWidget(this);
    auto cenlayout = new QVBoxLayout(cenwid);
    setCentralWidget(cenwid);

    auto table_info_layout = new QHBoxLayout(this);
    cenlayout->addLayout(table_info_layout);

    m_table_input.setPlaceholderText("请输入桌号");
    m_table_input.setInputMethodHints(Qt::ImhDigitsOnly);
    table_info_layout->addWidget(&m_table_input);

    m_follow_btn.setText("跟随");
    connect(&m_follow_btn, SIGNAL(clicked()), this, SLOT(proc_follow_table()));
    table_info_layout->addWidget(&m_follow_btn);

    m_create_btn.setText("开新桌");
    connect(&m_create_btn, SIGNAL(clicked()), this, SLOT(proc_create_table()));
    table_info_layout->addWidget(&m_create_btn);

    m_cur_table_no.setText("当前桌号：");
    table_info_layout->addWidget(&m_cur_table_no);

    m_cancle_btn.setText("退出");
    table_info_layout->addWidget(&m_cancle_btn);
    connect(&m_cancle_btn, SIGNAL(clicked()), this, SLOT(proc_cancle()));

    auto player_zone_layout = new QHBoxLayout();
    cenlayout->addLayout(player_zone_layout);


    for (int i = 0; i < 5; i++)
    {
        auto player_info = new game_sub_view_otherPlayer(this);
        m_sub_view.push_back(player_info);
        player_zone_layout->addWidget(player_info);
        player_info->show();
        m_pastOtherplayer[i] = player_info;
    }

    auto pub_cards_view = new game_sub_view_pub_cards(this);
    cenlayout->addWidget(pub_cards_view);
    m_sub_view.push_back(pub_cards_view);
    pub_cards_view->show();

    cenlayout->addWidget(&m_cur_pool);

    auto sub_op_view = new game_sub_view_operate(this);
    cenlayout->addWidget(sub_op_view);
    sub_op_view->show();
    m_sub_view.push_back(sub_op_view);
}

game_view::~game_view()
{
    game_control::GetController()->DelView(this);
}

void game_view::refresh()
{
    int iTableNo = game_control::GetController()->m_game_module.GetCurTableNo();
    if (iTableNo != -1)
    {
        m_cur_table_no.setText(QString("当前桌号： ") + QString::number(iTableNo));
        m_table_input.hide();
        m_follow_btn.hide();
        m_create_btn.hide();
    }

    m_cur_pool.setText(QString("主池金额：") + QString::number(game_control::GetController()->m_game_module.GetPoolCash()));

    int iCurPos = game_control::GetController()->m_game_module.GetMySeat();
    int iNextSeat = (iCurPos + 1) % 6;
    int i = 0;
    while (iNextSeat != iCurPos)
    {
        m_pastOtherplayer[i++]->SetSeat(iNextSeat);
        iNextSeat++;
        iNextSeat %= 6;
    }

    for (auto itr:m_sub_view)
    {
        itr->refresh();
    }
}

void game_view::proc_follow_table()
{
    game_control::GetController()->ReqToFollowTable(m_table_input.text().toInt());
}

void game_view::proc_create_table()
{
    game_control::GetController()->ReqToCreateTable();
}

void game_view::proc_cancle()
{
    this->close();
}

game_sub_view_operate::game_sub_view_operate(QWidget *parent):QWidget(parent),
    m_cash_full_pool_btn("全池"),
    m_cash_half_pool_btn("半池"),
    m_cash_double_pool_btn("两倍池"),
    m_cash_sld(Qt::Horizontal, nullptr),
    m_result(new game_sub_view_last_result(this))
{
    auto op_layout = new QHBoxLayout(this);
    op_layout->addWidget(&m_hand_cards_lable[0]);
    op_layout->addWidget(&m_hand_cards_lable[1]);

    auto cash_zone_layout = new QGridLayout(this);
    op_layout->addLayout(cash_zone_layout);
    cash_zone_layout->addWidget(&m_SeatPos_lable, 0, 0);
    cash_zone_layout->addWidget(&m_username_lable, 0, 1);
    cash_zone_layout->addWidget(new QLabel("当前金额", this), 1,0);
    cash_zone_layout->addWidget(&m_curCash_lable, 1, 1);
    cash_zone_layout->addWidget(new QLabel("下注金额", this), 2, 0);
    cash_zone_layout->addWidget(&m_batCash_lable, 2, 1);
    cash_zone_layout->addWidget(new QLabel("行动时间",this), 3, 0);
    cash_zone_layout->addWidget(&m_count_down_lable, 3, 1);

    auto bat_zone_layout = new QVBoxLayout(this);
    op_layout->addLayout(bat_zone_layout);

    bat_zone_layout->addWidget(&m_ready_btn);
    connect(&m_ready_btn, SIGNAL(clicked()), this, SLOT(proc_ready_btn()));

    bat_zone_layout->addWidget(&m_cash_sld);
    m_cash_sld.setRange(0, 100);
    connect(&m_cash_sld, SIGNAL(sliderMoved(int)), this, SLOT(proc_slide_move(int)));

    auto bat_cash_layout = new QHBoxLayout(this);
    bat_zone_layout->addLayout(bat_cash_layout);
    bat_cash_layout->addWidget(&m_cash_half_pool_btn);
    bat_cash_layout->addWidget(&m_cash_full_pool_btn);
    bat_cash_layout->addWidget(&m_cash_double_pool_btn);

    connect(&m_cash_half_pool_btn, SIGNAL(clicked()), this, SLOT(proc_half_pool_btn()));
    connect(&m_cash_full_pool_btn, SIGNAL(clicked()), this, SLOT(proc_full_pool_btn()));
    connect(&m_cash_double_pool_btn, SIGNAL(clicked()), this, SLOT(proc_double_pool_btn()));

    bat_zone_layout->addWidget(&m_bat_btn);
    connect(&m_bat_btn, SIGNAL(clicked()), this, SLOT(proc_bat_btn()));

    bat_zone_layout->addWidget(&m_follow_btn);
    connect(&m_follow_btn, SIGNAL(clicked()), this, SLOT(proc_flw_btn()));

    bat_zone_layout->addWidget(&m_fall_btn);
    m_fall_btn.setText("弃牌");
    connect(&m_fall_btn, SIGNAL(clicked()), this , SLOT(proc_fall_btn()));

    op_layout->addWidget(m_result);
}

void game_sub_view_operate::_set_bat_cash(int _bat_cash)
{
    int cur_cash = m_curCash_lable.text().toUInt() + game_control::GetController()->m_game_module.GetMybatCash();
    int set_value = _bat_cash <= cur_cash ? _bat_cash : cur_cash;
    if (cur_cash != 0)
    {
        m_cash_sld.setRange(game_control::GetController()->m_game_module.GetMinBatCash() * 2,cur_cash);
        m_cash_sld.setValue(set_value);
    }
}

void game_sub_view_operate::proc_full_pool_btn()
{
    int pool_cash = game_control::GetController()->m_game_module.GetPoolCash();
    int bat_cash = pool_cash;

    _set_bat_cash(bat_cash);
}

void game_sub_view_operate::proc_half_pool_btn()
{
    int pool_cash = game_control::GetController()->m_game_module.GetPoolCash();
    int bat_cash = pool_cash / 2;

    _set_bat_cash(bat_cash);
}

void game_sub_view_operate::proc_double_pool_btn()
{
    int pool_cash = game_control::GetController()->m_game_module.GetPoolCash();
    int bat_cash = pool_cash * 2;

    _set_bat_cash(bat_cash);
}

void game_sub_view_operate::proc_slide_move(int _value)
{
    _set_bat_cash(_value);
}

void game_sub_view_operate::proc_bat_btn()
{
    game_control::GetController()->ReqToBatCash(game_msg_action_bat, m_cash_sld.value());
    m_cash_sld.setValue(m_cash_sld.minimum());
}

void game_sub_view_operate::proc_fall_btn()
{
    game_control::GetController()->ReqToBatCash(game_msg_action_fall, 0);
    m_cash_sld.setValue(m_cash_sld.minimum());
}

void game_sub_view_operate::proc_flw_btn()
{
    m_cash_sld.setValue(m_cash_sld.minimum());
    int iSelfSeat = game_control::GetController()->m_game_module.GetMySeat();
    int iLastSeat = (iSelfSeat + 5) % 6;
    while (iSelfSeat != iLastSeat)
    {
        game_msg_player_info *lastPlayer = game_control::GetController()->m_game_module.GetPlayerBySeatpos(iLastSeat);
        if (nullptr != lastPlayer && !(lastPlayer->bFalled))
        {
            game_control::GetController()->ReqToBatCash(game_msg_action_call, lastPlayer->ibatCash);
            break;
        }
        iLastSeat = (iLastSeat + 5) % 6;
    }
}

void game_sub_view_operate::proc_ready_btn()
{
    auto self_seat = game_control::GetController()->m_game_module.GetMySeat();
    auto self_player = game_control::GetController()->m_game_module.GetPlayerBySeatpos(self_seat);

    if (self_player->bReady != 0)
    {
        game_control::GetController()->ReqToGetReady(false);
    }
    else
    {
        game_control::GetController()->ReqToGetReady(true);
    }
}

void game_sub_view_operate::show_count_down()
{
    if (m_count_down_lable.isHidden())
    {
        m_count_down_lable.show();
        m_timer.start(18000);
    }

    int left_sec = m_timer.remainingTime() / 1000;
    m_count_down_lable.setText(QString::number(left_sec));
}

void game_sub_view_operate::hide_count_down()
{
    m_timer.stop();
    m_count_down_lable.hide();
}

void game_sub_view_operate::refresh()
{
    for (int i = 0; i < 2; i++)
    {
        show_cards_img_inLabel(m_hand_cards_lable[i], game_control::GetController()->m_game_module.GetMyhandCardbyIndex(i));
    }
    auto &gm = game_control::GetController()->m_game_module;
    qDebug("refresh-view op %s", gm.GetSelfName().toUtf8().data());
    m_SeatPos_lable.setText(QString::number( gm.GetMySeat() + 1) + "号位");
    m_username_lable.setText(gm.GetSelfName());

    if (!m_curCash_lable.text().isEmpty() && gm.GetPoolCash() == 0)
    {
        auto last_cash = m_curCash_lable.text().toInt();
        auto cur_cash = gm.GetMyCurCash();
        if (cur_cash > last_cash)
        {
            m_curCash_lable.setStyleSheet("background-color: green;");
        }
        else if (cur_cash < last_cash)
        {
            m_curCash_lable.setStyleSheet("background-color: red;");
        }
    }
    m_curCash_lable.setText(QString::number(gm.GetMyCurCash()));

    auto self_seat = game_control::GetController()->m_game_module.GetMySeat();
    auto self_player = game_control::GetController()->m_game_module.GetPlayerBySeatpos(self_seat);
    if (self_player!= nullptr && self_player->bReady != 0)
    {
        m_ready_btn.setText("取消准备");
    }
    else
    {
        m_ready_btn.setText("准备");
    }

    if (game_control::GetController()->m_game_module.IsGameStarted())
    {
        m_ready_btn.hide();
    }
    else
    {
        m_ready_btn.show();
    }

    m_batCash_lable.setText(QString::number(gm.GetMybatCash()));
    int cur_cash = m_curCash_lable.text().toUInt() + game_control::GetController()->m_game_module.GetMybatCash();
    m_cash_sld.setRange(game_control::GetController()->m_game_module.GetMinBatCash() * 2, cur_cash);


    int iSelfSeat = game_control::GetController()->m_game_module.GetMySeat();
    int iLastSeat = (iSelfSeat + 5) % 6;
    while (iSelfSeat != iLastSeat)
    {
        game_msg_player_info *lastPlayer = game_control::GetController()->m_game_module.GetPlayerBySeatpos(iLastSeat);
        if (nullptr != lastPlayer && !lastPlayer->bFalled)
        {
            if (lastPlayer->ibatCash <= 0)
            {
                auto bat_cash_now = QString::number( m_cash_sld.value());
                m_bat_btn.setText(QString("下注:") + bat_cash_now);
                m_follow_btn.setText("过牌");
            }
            else if (lastPlayer->ibatCash >= game_control::GetController()->m_game_module.GetMybatCash() * 2)
            {
                auto bat_cash_now = QString::number( m_cash_sld.value());
                m_bat_btn.setText(QString("加注:") + bat_cash_now);
                m_follow_btn.setText("跟注");
            }
            else
            {
                auto bat_cash_now = QString::number( m_cash_sld.value());
                m_bat_btn.setText(QString("加注:") + bat_cash_now);
                m_follow_btn.setText("过牌");
            }
            break;
        }
        iLastSeat = (iLastSeat + 5) % 6;
    }

    int iActionPos = game_control::GetController()->m_game_module.GetActionPos();
    if (iActionPos != self_seat)
    {
        m_bat_btn.setDisabled(true);
        m_fall_btn.setDisabled(true);
        m_follow_btn.setDisabled(true);
        m_cash_sld.hide();
        m_cash_half_pool_btn.hide();
        m_cash_full_pool_btn.hide();
        m_cash_double_pool_btn.hide();
        hide_count_down();
    }
    else
    {
        m_bat_btn.setDisabled(false);
        m_fall_btn.setDisabled(false);
        m_follow_btn.setDisabled(false);
        m_cash_sld.show();
        m_cash_half_pool_btn.show();
        m_cash_full_pool_btn.show();
        m_cash_double_pool_btn.show();
        show_count_down();
    }
    m_result->SetSeat(gm.GetMySeat());
    m_result->refresh();
}


game_sub_view_otherPlayer::game_sub_view_otherPlayer(QWidget *parent):QWidget(parent),m_result_wid(new game_sub_view_last_result(this))
{
    auto player_info_layout = new QVBoxLayout(this);
    player_info_layout->addWidget(&m_seat_label);
    player_info_layout->addWidget(&m_name_label);
    player_info_layout->addWidget(&m_curCash_label);
    player_info_layout->addWidget(&m_batCash_label);
    player_info_layout->addWidget(&m_ready_label);
    player_info_layout->addWidget(&m_isAction_label);
    player_info_layout->addWidget(&m_isFalled_label);
    player_info_layout->addWidget(m_result_wid);
}

void game_sub_view_otherPlayer::SetSeat(int _seat)
{
    m_iSeat = _seat;
    m_result_wid->SetSeat(m_iSeat);
}

void game_sub_view_otherPlayer::hide_count_down()
{
    m_isAction_label.hide();
    m_timer.stop();
}

void game_sub_view_otherPlayer::show_count_down()
{
    if (m_isAction_label.isHidden())
    {
        m_timer.start(18000);
        m_isAction_label.show();
    }

    int left_sec = m_timer.remainingTime() / 1000;
    m_isAction_label.setText(QString("行动中，剩余") + QString::number(left_sec) + "秒");


}

void game_sub_view_otherPlayer::refresh()
{
    m_seat_label.setText(QString::number(m_iSeat + 1) + "号位");
    game_msg_player_info *player = game_control::GetController()->m_game_module.GetPlayerBySeatpos(m_iSeat);
    if (NULL != player)
    {
        m_name_label.setText(QString(player->szName));
        if (!m_curCash_label.text().isEmpty() && game_control::GetController()->m_game_module.GetPoolCash() == 0)
        {
            auto last_cash = m_curCash_label.text().split(":").back().toInt();
            if (player->icurrentCash > last_cash)
            {
                m_curCash_label.setStyleSheet("background-color: green;");
            }
            else if (player->icurrentCash < last_cash)
            {
                m_curCash_label.setStyleSheet("background-color: red;");
            }
        }
        m_curCash_label.setText(QString("拥有金额:") +  QString::number(player->icurrentCash));
        m_batCash_label.setText(QString("下注金额:") + QString::number(player->ibatCash));
        if (0 != player->bReady)
        {
            m_ready_label.setText("已准备");
        }
        else
        {
            m_ready_label.setText("未准备");
        }
        if (game_control::GetController()->m_game_module.IsGameStarted())
        {
            m_ready_label.hide();
            if (game_control::GetController()->m_game_module.GetActionPos() == m_iSeat)
            {
                show_count_down();
                m_isAction_label.setStyleSheet("background-color: green;");
            }
            else
            {
                if (player->bFalled)
                {
                    m_isFalled_label.show();
                    m_isFalled_label.setText("弃牌");
                    m_isFalled_label.setStyleSheet("background-color: red;");
                }
                else if (player->bAllined)
                {
                    m_isFalled_label.show();
                    m_isFalled_label.setText("全下");
                    m_isFalled_label.setStyleSheet("background-color: red;");
                }
                else
                {
                    m_isFalled_label.hide();
                }
                hide_count_down();
            }
        }
        else
        {
            m_ready_label.show();
        }
        m_result_wid->show();
        m_result_wid->refresh();
    }
    else
    {
        m_name_label.setText("无玩家");
        m_curCash_label.setText("");
        m_batCash_label.setText("");
        m_ready_label.setText("");
        m_result_wid->hide();
    }
}

game_sub_view_pub_cards::game_sub_view_pub_cards(QWidget *parent):QWidget(parent)
{
    auto pub_card_layout = new QHBoxLayout(this);
    for (int i = 0; i < 5; i++) {
        pub_card_layout->addWidget(&m_pub_cards_label[i]);
    }
}

void game_sub_view_pub_cards::refresh()
{
    for (int i = 0; i < 5; i++)
    {
        auto single_card = game_control::GetController()->m_game_module.GetPubCardByIndex(i);
        if (NULL != single_card)
        {
            show_cards_img_inLabel(m_pub_cards_label[i], single_card);
            m_pub_cards_label[i].show();
        }
        else
        {
            m_pub_cards_label[i].hide();
        }
    }
}

game_sub_view_last_result::game_sub_view_last_result(QWidget *parent):QWidget(parent)
{
    auto result_layout = new QVBoxLayout(this);
    result_layout->addWidget(&m_pattern_type_label);
    auto patt_layout = new QHBoxLayout(this);
    result_layout->addLayout((patt_layout));
    patt_layout->addWidget(&m_pattern_label[0]);
    patt_layout->addWidget(&m_pattern_label[1]);
    patt_layout->addWidget(&m_pattern_label[2]);
    patt_layout->addWidget(&m_pattern_label[3]);
    patt_layout->addWidget(&m_pattern_label[4]);
}

void game_sub_view_last_result::refresh()
{
    auto single_result = game_control::GetController()->m_game_module.GetsingleResult(m_seat);
    if (single_result->bShow)
    {
        this->show();
        switch (single_result->enType) {
        case game_msg_pattern_type_hight:
            m_pattern_type_label.setText("高牌");
            break;
        case game_msg_pattern_type_pair:
            m_pattern_type_label.setText("对子");
            break;
        case game_msg_pattern_type_two_pair:
            m_pattern_type_label.setText("两对");
            break;
        case game_msg_pattern_type_set:
            m_pattern_type_label.setText("三条");
            break;
        case game_msg_pattern_type_straiht:
            m_pattern_type_label.setText("顺子");
            break;
        case game_msg_pattern_type_flush:
            m_pattern_type_label.setText("同花");
            break;
        case game_msg_pattern_type_fullhouse:
            m_pattern_type_label.setText("葫芦");
            break;
        case game_msg_pattern_type_fok:
            m_pattern_type_label.setText("四条");
            break;
        case game_msg_pattern_type_flush_straight:
            m_pattern_type_label.setText("同花顺");
            break;
        case game_msg_pattern_type_max:
            m_pattern_type_label.setText("");
            break;
        }
        for (int i = 0; i < 5; ++i) {
            show_cards_img_inLabel(m_pattern_label[i], &single_result->finalPattern[i]);
            m_pattern_label[i].setPixmap(m_pattern_label[i].pixmap()->scaled(60,90));
        }
    }
    else
    {
        this->hide();
    }
}

void game_sub_view_last_result::SetSeat(int _seat)
{
    m_seat = _seat;
}
