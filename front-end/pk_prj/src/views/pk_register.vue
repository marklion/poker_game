<template>
    <div>
        <el-row :gutter="10">
            <el-col :span="6">手机号</el-col>
            <el-col :span="18">
                <el-input v-model="reg_username_input"></el-input>
            </el-col>
        </el-row>
        <el-row :gutter="10">
            <el-col :span="6">密码</el-col>
            <el-col :span="18">
                <el-input v-model="reg_password_input" show-password></el-input>
            </el-col>
        </el-row>
        <el-row :gutter="10">
            <el-col :span="6">确认密码</el-col>
            <el-col :span="18">
                <el-input v-model="reg_confirm_password_input" show-password></el-input>
            </el-col>
        </el-row>
        <el-row :gutter="10">
            <el-col :span="6">昵称</el-col>
            <el-col :span="18">
                <el-input v-model="reg_player_name_input"></el-input>
            </el-col>
        </el-row>
        <el-button @click="submit_reg_btn" type="primary">提交</el-button>
        <el-button @click="return_btn">返回</el-button>
    </div>
</template>

<script>
import {Base64} from 'js-base64'
export default {
    name: 'pk_register',
    data: function() {
        return {
            reg_username_input: '',
            reg_password_input:'',
            reg_confirm_password_input: '',
            reg_player_name_input:'',
        }
    },
    methods: {
        return_btn: function() {
            this.$router.push({name: 'pk_login'});
        },
        submit_reg_btn: function () {
            var vue_this = this;
            Base64.extendString();
            this.axios.post('/user_manage/register', {
                text:{
                    reg_number: this.reg_username_input,
                    reg_password: this.reg_password_input,
                    reg_name: this.reg_player_name_input.toBase64()}
                }
            ).then(function(response) {
                switch (response.data.result) {
                    case 'success':
                        vue_this.$notify({
                            title: '成功',
                            message: '注册成功',
                            type: 'success'
                        });
                        vue_this.$router.push({name: 'pk_login'});
                        break;
                    case 'exit':
                        vue_this.$notify({
                            title: '失败',
                            message: '用户已存在',
                            type: 'error'
                        });
                        break;
                    default:
                        vue_this.$notify({
                            title: '失败',
                            message: '注册失败',
                            type: 'error'
                        });
                        break;
                }
            }).catch(function(error) {
                vue_this.$notify({
                    title: '失败',
                    message: '注册失败',
                    type: 'error'
                });
                console.log(error);
            })
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