<!-- src/components/Hello.vue -->
<template>
  <div style="position:absolute; right:10px">
    <tbody class="mytable">
      <tr v-for="row in rows" :class="row">
        <td v-for="column in columns" :class="pieceColour(row, column)"></td>
      </tr>
    </tbody>
  </div>
</template>

<script lang="ts">
import { IBoard } from './Game/common/Board';
import Vue from 'vue';

interface Data {
  board: null | IBoard;
  rows: number[];
  columns: number[];
}

export default Vue.extend({
  data(): Data {
    return {
      board: null,
      rows: [...Array(20).keys()],
      columns: [...Array(10).keys()]
    };
  },
  methods: {
    pieceColour(row: number, column: number) {
      if (!this.board) {
        return 'black';
      }
      if (!this.board.vacant(row, column)) {
        return 'white';
      }
      return 'red';
    }
  }
});
</script>

<style>
.mytable td {
  padding: 3.5px;
  border: 0.5px solid black;
  border-collapse: collapse;
  background-color: white;
}
.mytable td.red {
  background-color: red;
}
</style>
