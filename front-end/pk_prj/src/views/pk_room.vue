<template>
<div>
    <p>{{room_no}}</p>
</div>
</template>

<script>
import messages from "../plugins/pk_game_pb"
export default {
    name: 'pk_room',
    data: function() {
        return {
            room_no: 0,
            all_players: [],
            round: 0,
            action_seat: 0,
            big_blind: 0,
            min_bat: 0,
        };
    },
    created: function() {
        var ssid = this.$cookies.get('ssid');
        var vue_this = this;
        this.axios.get("/user_manage/user_info/" + ssid)
        .then(function(resp) {
            if (resp.data.result.status != "success")
            {
                vue_this.$router.push({path: '/'});
            }
        }).catch(function(error) {
            console.log(error);
        });
    },
    beforeMount: function() {
        this.room_no = this.$route.params.room_no;
        this.pk_ws = new WebSocket("ws://59.110.64.232/game", "binary");
        this.pk_ws.binaryType = "arraybuffer";
        var msg = new messages.sync_info();
        var ssid = this.$cookies.get('ssid');
        msg.setSsid(ssid);
        console.log(msg.serializeBinary());
        var vue_this = this;
        this.pk_ws.onopen = function(evt) {
            var tlv_v = msg.serializeBinary();
            var tlv_t = 1;
            var tlv_l = tlv_v.byteLength;

            var buffer = new ArrayBuffer(tlv_l + 8);
            new DataView(buffer, 0).setUint32(0, tlv_t);
            new DataView(buffer, 4).setUint32(0, tlv_l);
            new Uint8Array(buffer, 8).set(tlv_v);

            vue_this.pk_ws.send(buffer);
            console.log(buffer);
            console.log(evt);
        };
        this.pk_ws.onclose = function(evt) {
            console.log(evt);
            vue_this.$notify({
                title: '网络断开',
                message: '网络出现问题，请重新连接',
                type: 'error'
            });
        }
        this.pk_ws.onmessage = function(evt) {
            var total_length = evt.data.byteLength;
            var left_length = total_length;
            while (left_length > 0) {
                var from_server_data = evt.data.slice(total_length - left_length, total_length);
                var cur_length = new DataView(from_server_data, 4).getUint32(0);
                left_length -= cur_length + 8;
                var cur_type = new DataView(from_server_data, 0).getUint32();
                switch (cur_type) {
                    case 3:
                        var room_info = messages.room_info.deserializeBinary(from_server_data.slice(8));
                        vue_this.room_no = room_info.getRoomNo();
                        var all_players = room_info.getAllPlayersList();
                        all_players.forEach((element, index) => {
                            vue_this.all_players[index] = {
                                ssid: element.getSsid(),
                                seat: element.getSeat(),
                                name: element.getName(),
                                bat_cash: element.getBatCash(),
                                cur_cash: element.getCurCash(),
                            };
                        });
                        vue_this.round = room_info.getRound();
                        vue_this.action_seat = room_info.getActionSeat();
                        vue_this.big_blind = room_info.getBigBlind();
                        vue_this.min_bat = room_info.getMinBat();
                        break;
                    default:
                        break;
                }
            }
        }
    }
}
</script>

<style>

</style>