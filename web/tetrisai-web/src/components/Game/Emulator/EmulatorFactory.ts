import jsnes from 'jsnes';
import { Emulator } from './Emulator';
import { IEmulator } from './IEmulator';


export class EmulatorFactory {
  private exists: boolean = false;
  private screen: any;
  private rom: any;

  constructor(screen: any, rom: any) {
    this.screen = screen;
    this.rom = rom;
  }

  public getInstance(): IEmulator {
    if (this.exists) {
      throw new Error('only one instance allowed');
    }

    const screen = this.screen;
    const nes = new jsnes.NES({
      onFrame: (buffer: any) => {
        screen.setBuffer(buffer);
        screen.writeBuffer();
      },
      onAudioSample: () => {
        // console.log('audio sample');
      },
    });
    nes.loadROM(this.rom);
    const mynes = new Emulator(nes);
    this.exists = true;
    return mynes;
  }
}
