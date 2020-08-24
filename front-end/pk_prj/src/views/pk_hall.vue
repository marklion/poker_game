<template>
<div>
    <div v-if="is_login">
        <el-card>
            <div slot="header" class="clearfix">
                <span>{{player_name_show}}</span>
                <el-button style="float: right; padding: 3px 0" type="text" @click="logout_btn">退出登录</el-button>
            </div>
            <el-row :gutter="10">
                <el-col :span="12">金额</el-col>
                <el-col :span="12">{{player_cash_show}}</el-col>
            </el-row>
        </el-card>
        <hr>
        <el-row :gutter="20">
            <el-col :span="12">
                <el-button type="success" round>随机入座</el-button>
            </el-col>
            <el-col :span="12">
                <el-button type="primary" round>创建房间</el-button>
            </el-col>
        </el-row>
        <el-row :gutter="10">
            <el-col :span="18">
                <el-input v-model="room_no_input" placeholder="请输入房间号"></el-input>
            </el-col>
            <el-col :span="6">
                <el-button type="primary" plain @click="enter_room(room_no_input)">进入房间</el-button>
            </el-col>
        </el-row>
    </div>
    <div v-else>
        <el-button @click="nav_login">请先登录</el-button>
    </div>
</div>
</template>

<script>
import {Base64} from 'js-base64'
export default {
    name: 'pk_hall',
    data: function() {
        return {
            is_login:false,
            player_name_show: '',
            player_cash_show: '',
            room_no_input: '',
        };
    },
    methods: {
        nav_login: function() {
            this.$router.push({name: 'pk_login'});
        },
        logout_btn: function() {
            this.axios.get('/user_manage/logoff/' + this.$cookies.get('ssid'));
            this.$cookies.remove('ssid');
            location.reload(true);
        },
        enter_room:function(room_no) {
            this.$router.push({path: '/pk_room/' + room_no});
        },
    },
    beforeMount: function() {
        Base64.extendString();
        var ssid = this.$cookies.get('ssid');
        var vue_this = this;
        this.axios.get("/user_manage/user_info/" + ssid)
        .then(function(resp) {
            console.log(resp.data.result);
            if (resp.data.result.status == "success")
            {
                vue_this.player_name_show = resp.data.result.name.fromBase64();
                vue_this.player_cash_show = resp.data.result.cash;
                vue_this.is_login = true;
            }
        }).catch(function(error) {
            console.log(error);
        });
    }
}
</script>

<style lang="scss" scoped>
    .clearfix:before,
    .clearfix:after {
        display: table;
        content: "";
    }
    .clearfix:after {
        clear: both
    }
    .el-row {
    margin-bottom: 20px;
    &:last-child {
        margin-bottom: 0;
    }
    .el-button {
        display: block;
        width: 100%;
        padding-left: 4px;
        padding-right: 4px;
    }
}
</style>