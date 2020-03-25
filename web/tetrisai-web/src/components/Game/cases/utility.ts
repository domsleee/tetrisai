import { getDemoEntry } from '../DemoEntryHelpers'
import { DemoButton } from '../IDemoPlayer'

export function eventsForSinglePress(demoButton: DemoButton, frame: number) {
  return [
     getDemoEntry(frame, demoButton, true),
     getDemoEntry(frame+1, demoButton, false)
  ]
}

export function eventsForUnpressThenPress(demoButton: DemoButton, frame: number) {
  return [getDemoEntry(frame, demoButton, false)].concat(eventsForSinglePress(demoButton, frame+1));
}