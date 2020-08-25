<template>
<div>
    <p>{{room_no}}</p>
</div>
</template>

<script>
import messages from "../plugins/test_pb"
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
        var msg = new messages.Test();
        msg.setTestId(12);
        console.log(msg);
        var bytes = msg.serializeBinary();
        console.log(bytes);
    }
}
</script>

<style>

</style>