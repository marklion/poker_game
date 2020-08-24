import Vue from 'vue'
import VueRouter from 'vue-router'
import pk_login from '../views/pk_login.vue'
import pk_register from '../views/pk_register.vue'
import pk_hall from '../views/pk_hall.vue'
import pk_room from '../views/pk_room.vue'

Vue.use(VueRouter)

  const routes = [
  {
    path: '/',
    name: 'pk_hall',
    component: pk_hall
  },
  {
    path: '/pk_login',
    name: 'pk_login',
    component: pk_login
  },
  {
    path: '/pk_register',
    name: 'pk_register',
    component: pk_register
  },
  {
    path: '/pk_room/:room_no',
    name: 'pk_room',
    component: pk_room
  },
]

const router = new VueRouter({
  routes
})

export default router
