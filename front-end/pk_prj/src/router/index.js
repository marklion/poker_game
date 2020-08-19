import Vue from 'vue'
import VueRouter from 'vue-router'
import pk_login from '../views/pk_login.vue'
import pk_register from '../views/pk_register.vue'

Vue.use(VueRouter)

  const routes = [
  {
    path: '/',
    name: 'pk_login',
    component: pk_login
  },
  {
    path: '/pk_register',
    name: 'pk_register',
    component: pk_register
  },
]

const router = new VueRouter({
  routes
})

export default router
