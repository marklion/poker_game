module.exports = {
  devServer: {
    proxy: {
      // 匹配所有以api开头的请求路径
      '/user_manage': {
        target: 'http://59.110.64.232/',
        ws: true,
        changeOrigin: true,
        secure: false
      }
    }
  }
}