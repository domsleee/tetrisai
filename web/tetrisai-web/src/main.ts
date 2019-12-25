import Vue from 'vue';
import App from './App.vue';
import jsnes from 'jsnes';

Vue.config.productionTip = false;

new Vue({
  render: (h) => h(App),
}).$mount('#app');

let nes = new jsnes.NES({
  onFrame: () => {},
  onAudioSample: () => {}
});



