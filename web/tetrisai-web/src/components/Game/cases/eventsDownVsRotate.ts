import { DemoButton, DemoEntry } from '../IDemoPlayer';
import { eventsForSinglePress } from './utility';

// this demonstrates that ROTATE happens BEFORE down
// we have here 527 rotate, 527 down

export function getDownVsRotate() {
   let events = downVsRotateEvents;
   return events.concat(eventsForSinglePress(DemoButton.BUTTON_A, 525));
}

const downVsRotateEvents: DemoEntry[] = [
  {
     'frame':270,
     'button':3,
     'isDown':true,
     'startFrame':270
  },
  {
     'frame':271,
     'button':3,
     'isDown':false,
     'startFrame':271
  },
  {
     'frame':275,
     'button':3,
     'isDown':true,
     'startFrame':275
  },
  {
     'frame':276,
     'button':3,
     'isDown':false,
     'startFrame':276
  },
  {
     'frame':280,
     'button':3,
     'isDown':true,
     'startFrame':280
  },
  {
     'frame':281,
     'button':3,
     'isDown':false,
     'startFrame':281
  },
  {
     'frame':285,
     'button':7,
     'isDown':true,
     'startFrame':285
  },
  {
     'frame':286,
     'button':7,
     'isDown':false,
     'startFrame':286
  },
  {
     'frame':290,
     'button':7,
     'isDown':true,
     'startFrame':290
  },
  {
     'frame':291,
     'button':7,
     'isDown':false,
     'startFrame':291
  },
  {
     'frame':295,
     'button':7,
     'isDown':true,
     'startFrame':295
  },
  {
     'frame':296,
     'button':7,
     'isDown':false,
     'startFrame':296
  },
  {
     'frame':300,
     'button':5,
     'isDown':true,
     'startFrame':300
  },
  {
     'frame':301,
     'button':5,
     'isDown':false,
     'startFrame':301
  },
  {
     'frame':305,
     'button':0,
     'isDown':true,
     'startFrame':305
  },
  {
     'frame':306,
     'button':3,
     'isDown':true,
     'startFrame':306
  },
  {
     'frame':312,
     'button':0,
     'isDown':false,
     'startFrame':312
  },
  {
     'frame':312,
     'button':3,
     'isDown':false,
     'startFrame':312
  },
  {
     'frame':479,
     'button':7,
     'isDown':true,
     'startFrame':479
  },
  {
     'frame':480,
     'button':0,
     'isDown':true,
     'startFrame':480
  },
  {
     'frame':481,
     'button':7,
     'isDown':false,
     'startFrame':481
  },
  {
     'frame':482,
     'button':0,
     'isDown':false,
     'startFrame':482
  },
  {
     'frame':490,
     'button':7,
     'isDown':true,
     'startFrame':490
  },
  {
     'frame':492,
     'button':7,
     'isDown':false,
     'startFrame':492
  },
  {
     'frame':493,
     'button':2,
     'isDown':false,
     'startFrame':493
  },
  {
     'frame':494,
     'button':2,
     'isDown':false,
     'startFrame':494
  },
  {
     'frame':495,
     'button':2,
     'isDown':false,
     'startFrame':495
  },
  {
     'frame':496,
     'button':2,
     'isDown':false,
     'startFrame':496
  },
  {
     'frame':497,
     'button':2,
     'isDown':false,
     'startFrame':497
  },
  {
     'frame':498,
     'button':2,
     'isDown':false,
     'startFrame':498
  }
]