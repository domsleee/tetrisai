<template>
  <div id="app">
    <Tetris/>
  </div>
</template>

<script lang="ts">
import Vue from 'vue';
import Tetris from './components/Tetris.vue';

export default Vue.extend({
  name: 'app',
  components: {
    Tetris,
  },
});
</script>

<style>
#app {
  font-family: 'Avenir', Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  text-align: center;
  color: #2c3e50;
  height:100%;
  width: 100%;
}
html, body {
  height: 100%;
  margin:0 !important;
  padding:0 !important;
}
</style>
