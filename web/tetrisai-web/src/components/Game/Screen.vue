<template>
  <canvas
    class="Screen"
    :width="SCREEN_WIDTH"
    :height="SCREEN_HEIGHT"
    @mousedown="onMouseDown"
    ref="input"
  />
</template>

<script lang="ts">
import Vue from 'vue';

const SCREEN_HEIGHT = 240;
const SCREEN_WIDTH = 256;

interface CaptureT {
  buf: string;
}

export default Vue.extend({
  name: 'Screen',
  mounted() {
    console.log('screen mounted...');
    this.initCanvas();
    this.$emit('myloaded');
    (window as any).screenshot = this.screenshot.bind(this);
  },
  data(): {
    canvas: Element | any;
    context: any;
    imageData: any;
    buf: ArrayBuffer | any;
    buf8: Uint8ClampedArray | any;
    buf32: Uint32Array | any;
    SCREEN_WIDTH: number;
    SCREEN_HEIGHT: number;
  } {
    return {
      canvas: null,
      context: null,
      imageData: null,
      buf: null,
      buf8: null,
      buf32: null,
      SCREEN_WIDTH,
      SCREEN_HEIGHT
    };
  },
  methods: {
    onMouseDown(e: any) {
      // Make coordinates unscaled
      const rect = this.canvas.getBoundingClientRect();

      const style = window.getComputedStyle
        ? getComputedStyle(this.canvas, null)
        : this.canvas.currentStyle;
      const actWidth =
        parseFloat(style.width) -
        parseFloat(style.paddingLeft) -
        parseFloat(style.paddingRight);
      const actHeight =
        parseFloat(style.height) -
        parseFloat(style.paddingTop) -
        parseFloat(style.paddingBottom);
      const scale = SCREEN_WIDTH / actWidth;

      const x = Math.round((e.offsetX - parseFloat(style.paddingLeft)) * scale);
      const y = Math.round(
        (e.offsetY - parseFloat(style.paddingRight)) * scale
      );
      console.log('Mouse click', x, y);
    },
    initCanvas() {
      this.canvas = this.$refs.input;
      this.context = this.canvas.getContext('2d');
      this.imageData = this.context.getImageData(
        0,
        0,
        SCREEN_WIDTH,
        SCREEN_HEIGHT
      );

      // [45, 89] ==> [73, 204]

      this.context.fillStyle = 'black';
      // set alpha to opaque
      this.context.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

      // buffer to write on next animation frame
      this.buf = new ArrayBuffer(this.imageData.data.length);
      // Get the canvas buffer in 8bit and 32bit
      this.buf8 = new Uint8ClampedArray(this.buf);
      this.buf32 = new Uint32Array(this.buf);

      // Set alpha
      for (let i = 0; i < this.buf32.length; ++i) {
        this.buf32[i] = 0xff000000;
      }
    },
    capture(): CaptureT {
      return {
        buf: JSON.stringify(this.buf)
      };
    },
    restoreFromCapture(capture: CaptureT) {
      const newBuf = JSON.parse(capture.buf);
      if (newBuf.length !== this.buf.length) {
        throw new Error('invalid buf in capture');
      }
      for (let i = 0; i < this.buf.length; ++i) {
        this.buf[i] = newBuf[i];
      }
      //this.buf = newBuf;
      this.buf8 = new Uint8ClampedArray(this.buf);
      this.buf32 = new Uint32Array(this.buf);
      this.setBuffer(this.buf);
      this.writeBuffer();
    },
    setBuffer(buffer: any) {
      console.log('setBuffer called.');
      let i = 0;
      for (let y = 0; y < SCREEN_HEIGHT; ++y) {
        for (let x = 0; x < SCREEN_WIDTH; ++x) {
          i = y * 256 + x;
          // Convert pixel from NES BGR to canvas ABGR
          this.buf32[i] = 0xff000000 | buffer[i]; // Full alpha
        }
      }
    },

    writeBuffer() {
      this.imageData.data.set(this.buf8);
      this.context.putImageData(this.imageData, 0, 0);
    },

    screenshot() {
      const img = new Image();
      img.src = this.canvas.toDataURL('image/png');
      document.body.append(img);
      console.log('appended to body');
      return img;
    }
  }
});
</script>

<style>
.Screen {
  image-rendering: pixelated;
}
</style>
