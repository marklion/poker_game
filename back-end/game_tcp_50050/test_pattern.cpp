#include "game_table.h"
#include <iostream>

void test_all()
{
    single_card test_case[][7] = {
        {
            single_card(single_card::diamond, 1),
            single_card(single_card::diamond, 13),
            single_card(single_card::diamond, 3),
            single_card(single_card::diamond, 5),
            single_card(single_card::diamond, 4),
            single_card(single_card::diamond, 2),
            single_card(single_card::diamond, 11),
        },
        {
            single_card(single_card::diamond, 4),
            single_card(single_card::club, 6),
            single_card(single_card::heart, 8),
            single_card(single_card::spade, 1),
            single_card(single_card::diamond, 7),
            single_card(single_card::diamond, 2),
            single_card(single_card::diamond, 3),

        },
        {

            single_card(single_card::diamond, 4),
            single_card(single_card::club, 4),
            single_card(single_card::heart, 8),
            single_card(single_card::spade, 1),
            single_card(single_card::diamond, 7),
            single_card(single_card::diamond, 2),
            single_card(single_card::diamond, 3),
        },
        {

            single_card(single_card::diamond, 4),
            single_card(single_card::club, 4),
            single_card(single_card::heart, 8),
            single_card(single_card::spade, 1),
            single_card(single_card::diamond, 2),
            single_card(single_card::spade, 2),
            single_card(single_card::diamond, 3),
        },
        {

            single_card(single_card::diamond, 4),
            single_card(single_card::club, 1),
            single_card(single_card::heart, 8),
            single_card(single_card::spade, 2),
            single_card(single_card::diamond, 2),
            single_card(single_card::spade, 2),
            single_card(single_card::diamond, 3),
        },
        {
            single_card(single_card::diamond, 4),
            single_card(single_card::club, 1),
            single_card(single_card::heart, 5),
            single_card(single_card::spade, 2),
            single_card(single_card::diamond, 2),
            single_card(single_card::spade, 2),
            single_card(single_card::diamond, 3),
        },
        {

            single_card(single_card::diamond, 4),
            single_card(single_card::club, 1),
            single_card(single_card::heart, 13),
            single_card(single_card::spade, 12),
            single_card(single_card::diamond, 10),
            single_card(single_card::spade, 2),
            single_card(single_card::diamond, 11),
        },
        {

            single_card(single_card::diamond, 4),
            single_card(single_card::club, 1),
            single_card(single_card::heart, 13),
            single_card(single_card::spade, 12),
            single_card(single_card::diamond, 10),
            single_card(single_card::spade, 4),
            single_card(single_card::diamond, 11),
        },
        {

            single_card(single_card::diamond, 1),
            single_card(single_card::club, 1),
            single_card(single_card::heart, 13),
            single_card(single_card::spade, 12),
            single_card(single_card::diamond, 10),
            single_card(single_card::spade, 1),
            single_card(single_card::diamond, 11),
        },
        {
            single_card(single_card::diamond, 1),
            single_card(single_card::club, 1),
            single_card(single_card::heart, 3),
            single_card(single_card::diamond, 5),
            single_card(single_card::diamond, 4),
            single_card(single_card::diamond, 2),
            single_card(single_card::diamond, 11),
        },
        {
            single_card(single_card::diamond, 1),
            single_card(single_card::club, 1),
            single_card(single_card::heart, 1),
            single_card(single_card::diamond, 5),
            single_card(single_card::diamond, 4),
            single_card(single_card::diamond, 2),
            single_card(single_card::diamond, 11),
        },
        {
            single_card(single_card::diamond, 1),
            single_card(single_card::club, 1),
            single_card(single_card::heart, 1),
            single_card(single_card::diamond, 5),
            single_card(single_card::diamond, 4),
            single_card(single_card::diamond, 2),
            single_card(single_card::heart, 2),
        },
        {
            single_card(single_card::diamond, 1),
            single_card(single_card::heart, 1),
            single_card(single_card::diamond, 5),
            single_card(single_card::diamond, 4),
            single_card(single_card::club, 1),
            single_card(single_card::diamond, 2),
            single_card(single_card::heart, 2),
        },
        {
            single_card(single_card::diamond, 1),
            single_card(single_card::heart, 1),
            single_card(single_card::spade, 2),
            single_card(single_card::club, 2),
            single_card(single_card::club, 1),
            single_card(single_card::diamond, 2),
            single_card(single_card::heart, 2),
        },
        {
            single_card(single_card::diamond, 2),
            single_card(single_card::heart, 2),
            single_card(single_card::spade, 1),
            single_card(single_card::club, 1),
            single_card(single_card::club, 2),
            single_card(single_card::diamond, 1),
            single_card(single_card::heart, 1),
        },
        {
            single_card(single_card::spade, 5),
            single_card(single_card::diamond, 4),
            single_card(single_card::heart, 4),
            single_card(single_card::spade, 4),
            single_card(single_card::club, 4),
            single_card(single_card::club, 3),
            single_card(single_card::club, 2),
        },
        {
            single_card(single_card::club, 1),
            single_card(single_card::club, 7),
            single_card(single_card::club, 5),
            single_card(single_card::heart, 4),
            single_card(single_card::club, 4),
            single_card(single_card::club, 3),
            single_card(single_card::club, 2),
        },
        {
            single_card(single_card::club, 1),
            single_card(single_card::diamond, 1),
            single_card(single_card::club, 8),
            single_card(single_card::heart, 7),
            single_card(single_card::club, 6),
            single_card(single_card::club, 5),
            single_card(single_card::spade, 5),
        },
        {
            single_card(single_card::club, 1),
            single_card(single_card::diamond, 1),
            single_card(single_card::club, 12),
            single_card(single_card::heart, 10),
            single_card(single_card::club, 7),
            single_card(single_card::spade, 7),
            single_card(single_card::spade, 5),
        },
    };
    for (auto &single_case : test_case)
    {
        auto final_pattern = card_pattern::MakeFinalPattern(single_case);
        std::cout << final_pattern.GetType() << std::endl;

        for (int i = 0; i < 5; i++)
        {
            std::cout << final_pattern.GetFinalCard(i).m_color << final_pattern.GetFinalCard(i).m_num << std::endl;
        }
    }
}

int main(int argc, const char **argv)
{
    test_all();
    return 0;
}