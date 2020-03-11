import { DemoButton } from '../IDemoPlayer';

/**
 * This shows that 12 high on the left is clearable on level 18
 * It shows that LEFT is processed before DOWN
 */


const press = 1055;

export function getLeftVsDown() {

}

const events = [
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
      'frame':306,
      'button':0,
      'isDown':true,
      'startFrame':306
   },
   {
      'frame':307,
      'button':3,
      'isDown':true,
      'startFrame':307
   },
   {
      'frame':313,
      'button':0,
      'isDown':false,
      'startFrame':313
   },
   {
      'frame':313,
      'button':3,
      'isDown':false,
      'startFrame':313
   },
   {
      'frame':357,
      'button':6,
      'isDown':true,
      'startFrame':357
   },
   {
      'frame':360,
      'button':6,
      'isDown':false,
      'startFrame':360
   },
   {
      'frame':366,
      'button':6,
      'isDown':true,
      'startFrame':366
   },
   {
      'frame':369,
      'button':6,
      'isDown':false,
      'startFrame':369
   },
   {
      'frame':373,
      'button':6,
      'isDown':true,
      'startFrame':373
   },
   {
      'frame':379,
      'button':6,
      'isDown':false,
      'startFrame':379
   },
   {
      'frame':397,
      'button':0,
      'isDown':true,
      'startFrame':397
   },
   {
      'frame':403,
      'button':0,
      'isDown':false,
      'startFrame':403
   },
   {
      'frame':408,
      'button':0,
      'isDown':true,
      'startFrame':408
   },
   {
      'frame':415,
      'button':0,
      'isDown':false,
      'startFrame':415
   },
   {
      'frame':486,
      'button':6,
      'isDown':true,
      'startFrame':486
   },
   {
      'frame':491,
      'button':6,
      'isDown':false,
      'startFrame':491
   },
   {
      'frame':495,
      'button':6,
      'isDown':true,
      'startFrame':495
   },
   {
      'frame':500,
      'button':6,
      'isDown':false,
      'startFrame':500
   },
   {
      'frame':502,
      'button':0,
      'isDown':true,
      'startFrame':502
   },
   {
      'frame':505,
      'button':6,
      'isDown':true,
      'startFrame':505
   },
   {
      'frame':508,
      'button':0,
      'isDown':false,
      'startFrame':508
   },
   {
      'frame':510,
      'button':6,
      'isDown':false,
      'startFrame':510
   },
   {
      'frame':515,
      'button':6,
      'isDown':true,
      'startFrame':515
   },
   {
      'frame':520,
      'button':6,
      'isDown':false,
      'startFrame':520
   },
   {
      'frame':550,
      'button':6,
      'isDown':true,
      'startFrame':550
   },
   {
      'frame':552,
      'button':0,
      'isDown':true,
      'startFrame':552
   },
   {
      'frame':554,
      'button':6,
      'isDown':false,
      'startFrame':554
   },
   {
      'frame':558,
      'button':0,
      'isDown':false,
      'startFrame':558
   },
   {
      'frame':588,
      'button':6,
      'isDown':true,
      'startFrame':588
   },
   {
      'frame':593,
      'button':6,
      'isDown':false,
      'startFrame':593
   },
   {
      'frame':614,
      'button':6,
      'isDown':true,
      'startFrame':614
   },
   {
      'frame':619,
      'button':0,
      'isDown':true,
      'startFrame':619
   },
   {
      'frame':620,
      'button':6,
      'isDown':false,
      'startFrame':620
   },
   {
      'frame':624,
      'button':6,
      'isDown':true,
      'startFrame':624
   },
   {
      'frame':627,
      'button':0,
      'isDown':false,
      'startFrame':627
   },
   {
      'frame':629,
      'button':6,
      'isDown':false,
      'startFrame':629
   },
   {
      'frame':634,
      'button':6,
      'isDown':true,
      'startFrame':634
   },
   {
      'frame':639,
      'button':6,
      'isDown':false,
      'startFrame':639
   },
   {
      'frame':644,
      'button':6,
      'isDown':true,
      'startFrame':644
   },
   {
      'frame':650,
      'button':6,
      'isDown':false,
      'startFrame':650
   },
   {
      'frame':674,
      'button':0,
      'isDown':true,
      'startFrame':674
   },
   {
      'frame':675,
      'button':6,
      'isDown':true,
      'startFrame':675
   },
   {
      'frame':680,
      'button':0,
      'isDown':false,
      'startFrame':680
   },
   {
      'frame':681,
      'button':6,
      'isDown':false,
      'startFrame':681
   },
   {
      'frame':744,
      'button':0,
      'isDown':true,
      'startFrame':744
   },
   {
      'frame':746,
      'button':6,
      'isDown':true,
      'startFrame':746
   },
   {
      'frame':752,
      'button':0,
      'isDown':false,
      'startFrame':752
   },
   {
      'frame':752,
      'button':6,
      'isDown':false,
      'startFrame':752
   },
   {
      'frame':758,
      'button':6,
      'isDown':true,
      'startFrame':758
   },
   {
      'frame':764,
      'button':6,
      'isDown':false,
      'startFrame':764
   },
   {
      'frame':772,
      'button':6,
      'isDown':true,
      'startFrame':772
   },
   {
      'frame':778,
      'button':6,
      'isDown':false,
      'startFrame':778
   },
   {
      'frame':799,
      'button':7,
      'isDown':true,
      'startFrame':799
   },
   {
      'frame':804,
      'button':1,
      'isDown':true,
      'startFrame':804
   },
   {
      'frame':806,
      'button':7,
      'isDown':false,
      'startFrame':806
   },
   {
      'frame':810,
      'button':1,
      'isDown':false,
      'startFrame':810
   },
   {
      'frame':865,
      'button':6,
      'isDown':true,
      'startFrame':865
   },
   {
      'frame':866,
      'button':6,
      'isDown':false,
      'startFrame':866
   },
   {
      'frame':869,
      'button':6,
      'isDown':true,
      'startFrame':869
   },
   {
      'frame':871,
      'button':0,
      'isDown':true,
      'startFrame':871
   },
   {
      'frame':871,
      'button':6,
      'isDown':false,
      'startFrame':871
   },
   {
      'frame':872,
      'button':0,
      'isDown':false,
      'startFrame':872
   },
   {
      'frame':873,
      'button':6,
      'isDown':false,
      'startFrame':873
   },
   {
      'frame':874,
      'button':6,
      'isDown':true,
      'startFrame':874
   },
   {
      'frame':875,
      'button':6,
      'isDown':false,
      'startFrame':875
   },
   {
      'frame':883,
      'button':6,
      'isDown':true,
      'startFrame':883
   },
   {
      'frame':885,
      'button':6,
      'isDown':false,
      'startFrame':885
   },
   {
      'frame':917,
      'button':7,
      'isDown':true,
      'startFrame':917
   },
   {
      'frame':918,
      'button':7,
      'isDown':false,
      'startFrame':918
   },
   {
      'frame':920,
      'button':7,
      'isDown':true,
      'startFrame':920
   },
   {
      'frame':921,
      'button':7,
      'isDown':false,
      'startFrame':921
   },
   {
      'frame':922,
      'button':7,
      'isDown':true,
      'startFrame':922
   },
   {
      'frame':924,
      'button':7,
      'isDown':false,
      'startFrame':924
   },
   {
      'frame':985,
      'button':7,
      'isDown':true,
      'startFrame':985
   },
   {
      'frame':986,
      'button':7,
      'isDown':false,
      'startFrame':986
   },
   {
      'frame':987,
      'button':2,
      'isDown':false,
      'startFrame':987
   },
   {
      'frame':988,
      'button':2,
      'isDown':false,
      'startFrame':988
   },
   {
      'frame':989,
      'button':2,
      'isDown':false,
      'startFrame':989
   },
   {
      'frame':990,
      'button':2,
      'isDown':false,
      'startFrame':990
   },
   {
      'frame':991,
      'button':2,
      'isDown':false,
      'startFrame':991
   },
   {
      'frame':992,
      'button':2,
      'isDown':false,
      'startFrame':992
   },
   {
      'frame':993,
      'button':2,
      'isDown':false,
      'startFrame':993
   },
   {
      'frame':994,
      'button':2,
      'isDown':false,
      'startFrame':994
   },
   {
      'frame':995,
      'button':2,
      'isDown':false,
      'startFrame':995
   },
   {
      'frame':996,
      'button':2,
      'isDown':false,
      'startFrame':996
   },
   {
      'frame':997,
      'button':2,
      'isDown':false,
      'startFrame':997
   },
   {
      'frame':998,
      'button':2,
      'isDown':false,
      'startFrame':998
   },
   {
      'frame':999,
      'button':2,
      'isDown':false,
      'startFrame':999
   },
   {
      'frame':1000,
      'button':2,
      'isDown':false,
      'startFrame':1000
   },
   {
      'frame':1001,
      'button':2,
      'isDown':false,
      'startFrame':1001
   },
   {
      'frame':1002,
      'button':2,
      'isDown':false,
      'startFrame':1002
   },
   {
      'frame':1003,
      'button':2,
      'isDown':false,
      'startFrame':1003
   },
   {
      'frame':1004,
      'button':2,
      'isDown':false,
      'startFrame':1004
   },
   {
      'frame':1005,
      'button':2,
      'isDown':false,
      'startFrame':1005
   },
   {
      'frame':1006,
      'button':2,
      'isDown':false,
      'startFrame':1006
   },
   {
      'frame':1007,
      'button':2,
      'isDown':false,
      'startFrame':1007
   },
   {
      'frame':1008,
      'button':2,
      'isDown':false,
      'startFrame':1008
   },
   {
      'frame':1009,
      'button':2,
      'isDown':false,
      'startFrame':1009
   },
   {
      'frame':1010,
      'button':2,
      'isDown':false,
      'startFrame':1010
   },
   {
      'frame':1011,
      'button':2,
      'isDown':false,
      'startFrame':1011
   },
   {
      'frame':1012,
      'button':2,
      'isDown':false,
      'startFrame':1012
   },
   {
      'frame':1013,
      'button':2,
      'isDown':false,
      'startFrame':1013
   },
   {
      'frame':1014,
      'button':2,
      'isDown':false,
      'startFrame':1014
   },
   {
      'frame':1015,
      'button':2,
      'isDown':false,
      'startFrame':1015
   },
   {
      'frame':1016,
      'button':2,
      'isDown':false,
      'startFrame':1016
   },
   {
      'frame':1017,
      'button':2,
      'isDown':false,
      'startFrame':1017
   },
   {
      'frame':1018,
      'button':2,
      'isDown':false,
      'startFrame':1018
   },
   {
      'frame':1021,
      'button':DemoButton.BUTTON_LEFT,
      'isDown':true,
      'startFrame':1018
   },
   {
      'frame':1030,
      'button':DemoButton.BUTTON_SELECT,
      'isDown':false,
      'startFrame':1018
   },
   {
      'frame':1038,
      'button':DemoButton.BUTTON_A,
      'isDown':true,
      'startFrame':1018
   },
   {
      'frame':1039,
      'button':DemoButton.BUTTON_A,
      'isDown':false,
      'startFrame':1018
   },
   {
      'frame':press,
      'button':DemoButton.BUTTON_LEFT,
      'isDown':false,
      'startFrame':1018
   },
   {
      'frame':press+1,
      'button':DemoButton.BUTTON_LEFT,
      'isDown':true,
      'startFrame':1018
   },
   {
      'frame':press+2,
      'button':DemoButton.BUTTON_LEFT,
      'isDown':false,
      'startFrame':1018
   }
   // appears 1034
]