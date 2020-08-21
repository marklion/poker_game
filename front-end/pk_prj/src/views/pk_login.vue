<template>
    <div>
        <el-row :gutter="10">
            <el-col :span="6">手机号</el-col>
            <el-col :span="18">
                <el-input v-model="user_name_input"></el-input>
            </el-col>
        </el-row>
        <el-row :gutter="10">
            <el-col :span="6">密码</el-col>
            <el-col :span="18">
                <el-input v-model="user_pwd_input" show-password></el-input>
            </el-col>
        </el-row>
        <el-button @click="login_btn">登录</el-button>
        <el-button @click="nav_to_reg">注册</el-button>
    </div>
</template>

<script>
export default {
    name: 'pk_login',
    data: function () {
        return {
            user_name_input:'',
            user_pwd_input:''
        }
    },
    methods: {
        nav_to_reg: function() {
            this.$router.push({name: 'pk_register'});
        },
        login_btn: function() {
            var vue_this = this;
            this.axios.post('/user_manage/login', {
                text:{
                    login_id: this.user_name_input,
                    login_pwd: this.user_pwd_input
                }
            }).then(function(resp) {
                console.log(resp.data.result);
                if (resp.data.result.status == 'success')
                {
                    vue_this.$cookies.set('ssid', resp.data.result.ssid);
                    vue_this.$notify({
                        title: '成功',
                        message: '登录成功',
                        type: 'success'
                    });
                    vue_this.$router.push({path:'/'});
                    }
                else
                {
                    vue_this.$notify({
                        title: '失败',
                        message: '用户名或密码错误',
                        type: 'error'
                    });
                }
            }).catch(function(error) {
                console.log(error);
                vue_this.$notify({
                    title: '失败',
                    message: '用户名或密码错误',
                    type: 'error'
                });
            });
        }
    }
}
</script>

<style lang="scss" scoped>

.el-row {
    margin-bottom: 20px;
    &:last-child {
        margin-bottom: 0;
    }
}
</style>