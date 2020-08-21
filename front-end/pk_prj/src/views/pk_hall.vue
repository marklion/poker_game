<template>
<div>
    <p>{{is_login}}</p>
    <div v-if="is_login">
        <p>昵称：{{player_name_show}}</p>
        <p>余额：{{player_cash_show}}</p>
    </div>
    <div v-else>
        <el-button @click="nav_login">请先登录</el-button>
    </div>
</div>
</template>

<script>
export default {
    name: 'pk_hall',
    data: function() {
        return {
            is_login:false,
            player_name_show: '',
            player_cash_show: '',
        };
    },
    methods: {
        nav_login: function() {
            this.$router.push({name: 'pk_login'});
        }
    },
    beforeMount: function() {
        var ssid = this.$cookies.get('ssid');
        var vue_this = this;
        this.axios.get("/user_manage/user_info?ssid=" + ssid)
        .then(function(resp) {
            console.log(resp.data.result);
            if (resp.data.result.status == "success")
            {
                vue_this.player_name_show = resp.data.result.name;
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

</style>