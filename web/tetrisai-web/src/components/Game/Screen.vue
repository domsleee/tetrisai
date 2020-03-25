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
import { ICapturable } from './ICapturable';
import { ScreenT } from '@/types';
import { IScreen } from './IScreen';
import Component from 'vue-class-component';

const SCREEN_HEIGHT = 240;
const SCREEN_WIDTH = 256;

export interface CaptureT {
  buf: string;
}

@Component({ name: 'Screen' })
export default class Screen extends Vue implements IScreen, ICapturable<CaptureT> {
  public screen: ScreenT;
  private canvas: Element | any = null;
  private context: any = null;
  private imageData: any = null;
  private buf: ArrayBuffer | any = null;
  private buf8: Uint8ClampedArray | any = null;
  private SCREEN_WIDTH: number = SCREEN_WIDTH;
  private SCREEN_HEIGHT: number = SCREEN_HEIGHT;

  constructor() {
    super();
    this.screen = {} as ScreenT;
  }

  public mounted() {
    console.log('screen mounted...');
    this.initCanvas();
    this.$emit('myloaded');
    (window as any).screenshot = this.screenshot.bind(this);
  }

  public onMouseDown(e: any) {
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
    const y = Math.round((e.offsetY - parseFloat(style.paddingRight)) * scale);
    console.log('Mouse click', x, y);
  }

  public initCanvas() {
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
    this.screen.buf32 = new Uint32Array(this.buf);

    // Set alpha
    for (let i = 0; i < this.screen.buf32.length; ++i) {
      this.screen.buf32[i] = 0xff000000;
    }
  }
  public capture(): CaptureT {
    return {
      buf: JSON.stringify(this.buf)
    };
  }

  public restoreFromCapture(capture: CaptureT) {
    const newBuf = JSON.parse(capture.buf);
    if (newBuf.length !== this.buf.length) {
      throw new Error('invalid buf in capture');
    }
    for (let i = 0; i < this.buf.length; ++i) {
      this.buf[i] = newBuf[i];
    }
    this.buf8 = new Uint8ClampedArray(this.buf);
    this.screen.buf32 = new Uint32Array(this.buf);
    this.setBuffer(this.buf);
    this.writeBuffer();
  }

  public setBuffer(buffer: any) {
    //console.log('setBuffer called.');
    let i = 0;
    for (let y = 0; y < SCREEN_HEIGHT; ++y) {
      for (let x = 0; x < SCREEN_WIDTH; ++x) {
        i = y * 256 + x;
        // Convert pixel from NES BGR to canvas ABGR
        this.screen.buf32[i] = 0xff000000 | buffer[i]; // Full alpha
      }
    }
  }

  public writeBuffer() {
    this.imageData.data.set(this.buf8);
    this.context.putImageData(this.imageData, 0, 0);
  }

  public screenshot() {
    const img = new Image();
    img.src = this.canvas.toDataURL('image/png');
    document.body.append(img);
    console.log('appended to body');
    return img;
  }
}
</script>

<style>
.Screen {
  image-rendering: pixelated;
}
</style>
