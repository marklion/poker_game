<template>
<div>
    <p>{{room_no}}</p>
</div>
</template>

<script>
export default {
    name: 'pk_room',
    data: function() {
        return {
            room_no: '',
        };
    },
    beforeMount: function() {
        this.room_no = this.$route.params.room_no;
    }
}
</script>

<style>

</style>