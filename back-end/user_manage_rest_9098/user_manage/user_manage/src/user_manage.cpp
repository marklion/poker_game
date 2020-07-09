// This file generated by ngrestcg
// For more information, please visit: https://github.com/loentar/ngrest

#include "user_manage.h"
#include "random_user.h"

register_resp user_manage::proc_register(const register_req& text)
{
    register_resp ret;

    ret.type = "response";
    ret.action = text.action;
    ret.status = "success";

    return ret;
}


hello_resp user_manage::proc_get_hello()
{
    hello_resp ret = {"hello mvc"};

    return ret;
}


login_random_resp user_manage::proc_login_random()
{
    login_random_resp ret;

    auto random_user_ssid = RandomUserGenerat();

    if (random_user_ssid.length() >= 0)
    {
        ret.status = "success";
        ret.type = "response";
        ret.ssid = random_user_ssid;
    }


    return ret;
}