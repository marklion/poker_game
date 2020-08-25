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
            room_no: '',
        };
    },
    beforeMount: function() {
        this.room_no = this.$route.params.room_no;
        this.pk_ws = new WebSocket("ws://59.110.64.232/game", "binary");
        var msg = new messages.sync_info();
        var ssid = this.$cookies.get('ssid');
        msg.setSsid(ssid);
        console.log(msg.serializeBinary());
        var vue_this = this;
        this.pk_ws.onopen = function(evt) {
            vue_this.pk_ws.send(msg.serializeBinary());
            console.log(evt);
        };
    }
}
</script>

<style>

</style>