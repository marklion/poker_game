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
                <el-button type="primary" round @click="create_room">创建房间</el-button>
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
import messages from "../plugins/pk_game_pb"
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
        create_room: function() {
            var vue_this = this;
            this.pk_ws = new WebSocket("ws://59.110.64.232/game", "binary");
            this.pk_ws.binaryType = "arraybuffer";
            this.pk_ws.onopen = function(evt) {
                console.log(evt);
                var msg = new messages.sync_info();
                var ssid = vue_this.$cookies.get('ssid');
                msg.setSsid(ssid);
                var tlv_v = msg.serializeBinary();
                var tlv_t = 1;
                var tlv_l = tlv_v.byteLength;

                var buffer = new ArrayBuffer(tlv_l + 8);
                new DataView(buffer, 0).setUint32(0, tlv_t);
                new DataView(buffer, 4).setUint32(0, tlv_l);
                new Uint8Array(buffer, 8).set(tlv_v);

                vue_this.pk_ws.send(buffer);
                msg = new messages.create_room();
                msg.setSsid(ssid);
                tlv_v = msg.serializeBinary();
                tlv_t = 2;
                tlv_l = tlv_v.byteLength;

                buffer = new ArrayBuffer(tlv_l + 8);
                new DataView(buffer, 0).setUint32(0, tlv_t);
                new DataView(buffer, 4).setUint32(0, tlv_l);
                new Uint8Array(buffer, 8).set(tlv_v);

                vue_this.pk_ws.send(buffer);
            };
            this.pk_ws.onmessage = function(evt) {
                console.log(evt);
                var total_length = evt.data.byteLength;
                var left_length = total_length;
                while (left_length > 0) {
                    var from_server_data = evt.data.slice(total_length - left_length, total_length);
                    var cur_length = new DataView(from_server_data, 4).getUint32(0);
                    left_length -= cur_length + 8;
                    var cur_type = new DataView(from_server_data, 0).getUint32();
                    switch (cur_type) {
                        case 2:
                            var create_room = messages.create_room.deserializeBinary(from_server_data.slice(8));
                            vue_this.pk_ws.close();
                            vue_this.$router.push({path: '/pk_room/' + create_room.getRoomNo()});
                           break;
                        default:
                            break;
                    }
                }
            }
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