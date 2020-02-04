import Vue from 'vue';
import VueRouter from 'vue-router';
import App from './App.vue';

Vue.use(VueRouter);

Vue.config.productionTip = false;

const routes = [
  { path: '/', name: 'ok', component: App },
  { path: '/player', component: App }
];

const router = new VueRouter({
  routes,
  mode: 'history'
});

const app = new Vue({
  router
}).$mount('#app');
