<template>
  <div
    v-if="!romLoaded"
    class="ListPage my-4"
    @dragover="handleDragOver"
    @drop="handleDrop"
  >
    <div class="container">
      <div class="row justify-content-center">
        <div class="col-md-8">
          <header class="mb-4">
            <h1 class="mb-3" style="margin-top:0px!important">JSNES</h1>
            <p>
              A JavaScript NES emulator. :)
              <a href="https://github.com/bfirsh/jsnes">
                Source on GitHub.
              </a>
            </p>
          </header>
          <p>Drop your tetris ROM here</p>
        </div>
      </div>
    </div>
  </div>
  <div v-else>
    <Screen class="myscreen" ref="screen" @myloaded='screenLoaded' />
    <pre style="position:absolute; top:10px; text-align:left;">Debug info<br />{{JSON.stringify(debug, null, 2)}}</pre>
    <TableBoard ref="tableboard" />
  </div>
</template>

<script lang="ts">
import Vue from 'vue';
import jsnes from 'jsnes';
// @ts-ignore
import Screen from './Game/Screen';
import TableBoard from './TableBoard.vue';
import { GameLogic } from './Game/GameLogic';

const ROM_LOCAL_STORAGE_KEY = 'romlocalstorage';

export default Vue.extend({
  name: 'Tetris',
  components: {
    Screen,
    TableBoard,
  },
  data(): {
    romLoaded: boolean,
    debug: any,
  } {
    return {
      romLoaded: false,
      debug: {
        frame: 0,
        nextPiece: '',
        board: '',
        fps: '',
        totalLineClears: '',
      },
    };
  },
  mounted() {
    if (ROM_LOCAL_STORAGE_KEY in localStorage) {
      this.romLoaded = true;
    }
  },
  methods: {
    screenLoaded() {
      this.onScreenMounted();
    },
    handleDragOver(e: DragEvent) {
      e.preventDefault();
      if (e.dataTransfer) {
        e.dataTransfer.dropEffect = 'copy';
      }
    },
    handleDrop(e: DragEvent) {
      e.preventDefault();
      if (!e.dataTransfer) { return; }
      const file = e.dataTransfer.items
        ? e.dataTransfer.items[0].getAsFile()
        : e.dataTransfer.files[0];

      if (file == null) { return; }
      const reader = new FileReader();
      reader.readAsBinaryString(file);
      reader.onload = (e2) => {
        this.saveRomToLocalStorage(reader.result);
        this.romLoaded = true;
      };
    },
    saveRomToLocalStorage(res: any) {
      localStorage.setItem(ROM_LOCAL_STORAGE_KEY, res);
    },
    loadRomFromLocalStorage() {
      return localStorage.getItem(ROM_LOCAL_STORAGE_KEY);
    },
    onScreenMounted() {
      const screen: Screen = this.$refs.screen;
      const tableBoard: any = this.$refs.tableboard;
      const gl = new GameLogic();
      gl.run(screen, this.loadRomFromLocalStorage(), tableBoard, this.debug);
    },
  },
});
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
.ListPage {
  height: 100vh!important;
}
h3 {
  margin: 40px 0 0;
}
ul {
  list-style-type: none;
  padding: 0;
}
li {
  display: inline-block;
  margin: 0 10px;
}
a {
  color: #42b983;
}
.myscreen {
  box-sizing: border-box;
  padding: 2em;
  width: 100vw; 
  height: 93.75vw; /* height:width ratio = 240/256 = .9375  */
  background: pink;
  max-height: 100vh;
  max-width: 106.66vh; /* 16/9 = 1.778 */
  margin: auto;
  position: absolute;
  top:0;bottom:0; /* vertical center */
  left:0;right:0; /* horizontal center */
}
</style>
