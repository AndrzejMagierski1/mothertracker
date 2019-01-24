#include "mtAudioEngine.h"



const float filterFreq[478] =
{
20.000000 , 20.279190 , 20.562277 , 20.849315 , 21.140361 , 21.435469 , 21.734697 , 22.038102 , 22.345743 , 22.657678 , 22.973967 , 23.294672 ,
23.619853 , 23.949574 , 24.283898 , 24.622888 , 24.966611 , 25.315132 , 25.668518 , 26.026837 , 26.390158 , 26.758551 , 27.132087 , 27.510836 ,
27.894873 , 28.284271 , 28.679105 , 29.079450 , 29.485384 , 29.896985 , 30.314331 , 30.737504 , 31.166583 , 31.601652 , 32.042795 , 32.490096 ,
32.943641 , 33.403517 , 33.869812 , 34.342617 , 34.822023 , 35.308120 , 35.801003 , 36.300766 , 36.807506 , 37.321320 , 37.842306 , 38.370565 ,
38.906198 , 39.449308 , 40.000000 , 40.558379 , 41.124553 , 41.698630 , 42.280722 , 42.870939 , 43.469395 , 44.076205 , 44.691486 , 45.315355 ,
45.947934 , 46.589343 , 47.239706 , 47.899148 , 48.567795 , 49.245777 , 49.933222 , 50.630264 , 51.337036 , 52.053674 , 52.780316 , 53.517102 ,
54.264173 , 55.021673 , 55.789747 , 56.568542 , 57.358210 , 58.158901 , 58.970769 , 59.793970 , 60.628663 , 61.475007 , 62.333166 , 63.203305 ,
64.085590 , 64.980192 , 65.887281 , 66.807034 , 67.739625 , 68.685235 , 69.644045 , 70.616240 , 71.602006 , 72.601532 , 73.615012 , 74.642639 ,
75.684612 , 76.741130 , 77.812396 , 78.898616 , 80.000000 , 81.116758 , 82.249106 , 83.397261 , 84.561443 , 85.741877 , 86.938789 , 88.152409 ,
89.382971 , 90.630711 , 91.895868 , 93.178687 , 94.479413 , 95.798296 , 97.135591 , 98.491553 , 99.866444 , 101.260528 , 102.674072 , 104.107348 ,
105.560633 , 107.034204 , 108.528346 , 110.043345 , 111.579493 , 113.137085 , 114.716420 , 116.317801 , 117.941537 , 119.587940 , 121.257325 ,
122.950015 , 124.666333 , 126.406610 , 128.171180 , 129.960383 , 131.774563 , 133.614067 , 135.479250 , 137.370470 , 139.288090 , 141.232479 ,
143.204011 , 145.203065 , 147.230024 , 149.285279 , 151.369223 , 153.482259 , 155.624792 , 157.797233 , 160.000000 , 162.233517 , 164.498212 ,
166.794522 , 169.122886 , 171.483754 , 173.877578 , 176.304819 , 178.765942 , 181.261422 , 183.791737 , 186.357374 , 188.958826 , 191.596593 ,
194.271182 , 196.983106 , 199.732888 , 202.521055 , 205.348144 , 208.214697 , 211.121266 , 214.068409 , 217.056692 , 220.086691 , 223.158987 ,
226.274170 , 229.432840 , 232.635603 , 235.883075 , 239.175880 , 242.514651 , 245.900029 , 249.332665 , 252.813220 , 256.342361 , 259.920767 ,
263.549126 , 267.228134 , 270.958500 , 274.740940 , 278.576180 , 282.464959 , 286.408023 , 290.406130 , 294.460048 , 298.570557 , 302.738447 ,
306.964518 , 311.249583 , 315.594465 , 320.000000 , 324.467034 , 328.996425 , 333.589043 , 338.245773 , 342.967508 , 347.755156 , 352.609637 ,
357.531884 , 362.522843 , 367.583474 , 372.714748 , 377.917652 , 383.193185 , 388.542363 , 393.966212 , 399.465776 , 405.042110 , 410.696287 ,
416.429394 , 422.242531 , 428.136818 , 434.113385 , 440.173382 , 446.317973 , 452.548340 , 458.865679 , 465.271206 , 471.766150 , 478.351760 ,
485.029301 , 491.800058 , 498.665331 , 505.626440 , 512.684722 , 519.841534 , 527.098251 , 534.456268 , 541.917000 , 549.481879 , 557.152361 ,
564.929918 , 572.816045 , 580.812259 , 588.920096 , 597.141115 , 605.476894 , 613.929036 , 622.499166 , 631.188931 , 640.000000 , 648.934067 ,
657.992849 , 667.178087 , 676.491546 , 685.935016 , 695.510312 , 705.219274 , 715.063768 , 725.045687 , 735.166947 , 745.429495 , 755.835303 ,
766.386371 , 777.084726 , 787.932425 , 798.931551 , 810.084220 , 821.392574 , 832.858787 , 844.485063 , 856.273635 , 868.226770 , 880.346764 ,
892.635946 , 905.096680 , 917.731359 , 930.542411 , 943.532299 , 956.703519 , 970.058603 , 983.600116 , 997.330662 , 1011.252879 , 1025.369443 ,
1039.683067 , 1054.196502 , 1068.912537 , 1083.834000 , 1098.963759 , 1114.304721 , 1129.859835 , 1145.632091 , 1161.624519 , 1177.840193 ,
1194.282229 , 1210.953788 , 1227.858073 , 1244.998333 , 1262.377862 , 1280.000000 , 1297.868134 , 1315.985698 , 1334.356174 , 1352.983092 ,
1371.870032 , 1391.020624 , 1410.438548 , 1430.127537 , 1450.091373 , 1470.333894 , 1490.858991 , 1511.670607 , 1532.772742 , 1554.169452 ,
1575.864849 , 1597.863103 , 1620.168440 , 1642.785149 , 1665.717575 , 1688.970126 , 1712.547270 , 1736.453539 , 1760.693527 , 1785.271893 ,
1810.193360 , 1835.462717 , 1861.084822 , 1887.064598 , 1913.407038 , 1940.117205 , 1967.200232 , 1994.661324 , 2022.505758 , 2050.738887 ,
2079.366135 , 2108.393004 , 2137.825074 , 2167.668000 , 2197.927517 , 2228.609442 , 2259.719671 , 2291.264182 , 2323.249038 , 2355.680386 ,
2388.564458 , 2421.907576 , 2455.716145 , 2489.996665 , 2524.755724 , 2560.000000 , 2595.736268 , 2631.971396 , 2668.712348 , 2705.966184 ,
2743.740064 , 2782.041248 , 2820.877097 , 2860.255073 , 2900.182746 , 2940.667789 , 2981.717981 , 3023.341213 , 3065.545484 , 3108.338904 ,
3151.729698 , 3195.726205 , 3240.336881 , 3285.570298 , 3331.435150 , 3377.940252 , 3425.094540 , 3472.907078 , 3521.387054 , 3570.543786 ,
3620.386720 , 3670.925435 , 3722.169644 , 3774.129196 , 3826.814077 , 3880.234410 , 3934.400464 , 3989.322648 , 4045.011517 , 4101.477773 ,
4158.732269 , 4216.786008 , 4275.650147 , 4335.335999 , 4395.855035 , 4457.218884 , 4519.439341 , 4582.528363 , 4646.498075 , 4711.360771 ,
4777.128917 , 4843.815151 , 4911.432291 , 4979.993331 , 5049.511447 , 5120.000000 , 5191.472537 , 5263.942792 , 5337.424696 , 5411.932368 ,
5487.480128 , 5564.082496 , 5641.754193 , 5720.510147 , 5800.365493 , 5881.335578 , 5963.435963 , 6046.682427 , 6131.090968 , 6216.677808 ,
6303.459396 , 6391.452410 , 6480.673761 , 6571.140596 , 6662.870300 , 6755.880503 , 6850.189081 , 6945.814157 , 7042.774109 , 7141.087572 ,
7240.773439 , 7341.850870 , 7444.339289 , 7548.258393 , 7653.628153 , 7760.468821 , 7868.800928 , 7978.645296 , 8090.023033 , 8202.955547 ,
8317.464539 , 8433.572017 , 8551.300295 , 8670.671999 , 8791.710069 , 8914.437768 , 9038.878682 , 9165.056726 , 9292.996150 , 9422.721542 ,
9554.257833 , 9687.630302 , 9822.864582 , 9959.986662 , 10099.022894 , 10240.000000 , 10382.945073 , 10527.885585 , 10674.849391 , 10823.864735 ,
10974.960256 , 11128.164992 , 11283.508387 , 11441.020294 , 11600.730985 , 11762.671155 , 11926.871925 , 12093.364853 , 12262.181935 , 12433.355616 ,
12606.918793 , 12782.904821 , 12961.347522 , 13142.281191 , 13325.740600 , 13511.761006 , 13700.378161 , 13891.628313 , 14085.548218 , 14282.175143 ,
14481.546879 , 14683.701740 , 14888.678577

};

const float filterOctaves[OCTAVES_BUFFOR_SIZE] =
{
 7.0000000 ,  6.9800000 ,  6.9600000 ,  6.9400000 ,  6.9200000 ,  6.9000000 ,  6.8800000 ,  6.8600000 ,  6.8400000 ,  6.8200000 ,  6.8000000 ,  6.7800000 ,
 6.7600000 ,  6.7400000 ,  6.7200000 ,  6.7000000 ,  6.6800000 ,  6.6600000 ,  6.6400000 ,  6.6200000 ,  6.6000000 ,  6.5800000 ,  6.5600000 ,  6.5400000 ,
 6.5200000 ,  6.5000000 ,  6.4800000 ,  6.4600000 ,  6.4400000 ,  6.4200000 ,  6.4000000 ,  6.3800000 ,  6.3600000 ,  6.3400000 ,  6.3200000 ,  6.3000000 ,
 6.2800000 ,  6.2600000 ,  6.2400000 ,  6.2200000 ,  6.2000000 ,  6.1800000 ,  6.1600000 ,  6.1400000 ,  6.1200000 ,  6.1000000 ,  6.0800000 ,  6.0600000 ,
 6.0400000 ,  6.0200000 ,  6.0000000 ,  5.9800000 ,  5.9600000 ,  5.9400000 ,  5.9200000 ,  5.9000000 ,  5.8800000 ,  5.8600000 ,  5.8400000 ,  5.8200000 ,
 5.8000000 ,  5.7800000 ,  5.7600000 ,  5.7400000 ,  5.7200000 ,  5.7000000 ,  5.6800000 ,  5.6600000 ,  5.6400000 ,  5.6200000 ,  5.6000000 ,  5.5800000 ,
 5.5600000 ,  5.5400000 ,  5.5200000 ,  5.5000000 ,  5.4800000 ,  5.4600000 ,  5.4400000 ,  5.4200000 ,  5.4000000 ,  5.3800000 ,  5.3600000 ,  5.3400000 ,
 5.3200000 ,  5.3000000 ,  5.2800000 ,  5.2600000 ,  5.2400000 ,  5.2200000 ,  5.2000000 ,  5.1800000 ,  5.1600000 ,  5.1400000 ,  5.1200000 ,  5.1000000 ,
 5.0800000 ,  5.0600000 ,  5.0400000 ,  5.0200000 ,  5.0000000 ,  4.9800000 ,  4.9600000 ,  4.9400000 ,  4.9200000 ,  4.9000000 ,  4.8800000 ,  4.8600000 ,
 4.8400000 ,  4.8200000 ,  4.8000000 ,  4.7800000 ,  4.7600000 ,  4.7400000 ,  4.7200000 ,  4.7000000 ,  4.6800000 ,  4.6600000 ,  4.6400000 ,  4.6200000 ,
 4.6000000 ,  4.5800000 ,  4.5600000 ,  4.5400000 ,  4.5200000 ,  4.5000000 ,  4.4800000 ,  4.4600000 ,  4.4400000 ,  4.4200000 ,  4.4000000 ,  4.3800000 ,
 4.3600000 ,  4.3400000 ,  4.3200000 ,  4.3000000 ,  4.2800000 ,  4.2600000 ,  4.2400000 ,  4.2200000 ,  4.2000000 ,  4.1800000 ,  4.1600000 ,  4.1400000 ,
 4.1200000 ,  4.1000000 ,  4.0800000 ,  4.0600000 ,  4.0400000 ,  4.0200000 ,  4.0000000 ,  3.9800000 ,  3.9600000 ,  3.9400000 ,  3.9200000 ,  3.9000000 ,
 3.8800000 ,  3.8600000 ,  3.8400000 ,  3.8200000 ,  3.8000000 ,  3.7800000 ,  3.7600000 ,  3.7400000 ,  3.7200000 ,  3.7000000 ,  3.6800000 ,  3.6600000 ,
 3.6400000 ,  3.6200000 ,  3.6000000 ,  3.5800000 ,  3.5600000 ,  3.5400000 ,  3.5200000 ,  3.5000000 ,  3.4800000 ,  3.4600000 ,  3.4400000 ,  3.4200000 ,
 3.4000000 ,  3.3800000 ,  3.3600000 ,  3.3400000 ,  3.3200000 ,  3.3000000 ,  3.2800000 ,  3.2600000 ,  3.2400000 ,  3.2200000 ,  3.2000000 ,  3.1800000 ,
 3.1600000 ,  3.1400000 ,  3.1200000 ,  3.1000000 ,  3.0800000 ,  3.0600000 ,  3.0400000 ,  3.0200000 ,  3.0000000 ,  2.9800000 ,  2.9600000 ,  2.9400000 ,
 2.9200000 ,  2.9000000 ,  2.8800000 ,  2.8600000 ,  2.8400000 ,  2.8200000 ,  2.8000000 ,  2.7800000 ,  2.7600000 ,  2.7400000 ,  2.7200000 ,  2.7000000 ,
 2.6800000 ,  2.6600000 ,  2.6400000 ,  2.6200000 ,  2.6000000 ,  2.5800000 ,  2.5600000 ,  2.5400000 ,  2.5200000 ,  2.5000000 ,  2.4800000 ,  2.4600000 ,
 2.4400000 ,  2.4200000 ,  2.4000000 ,  2.3800000 ,  2.3600000 ,  2.3400000 ,  2.3200000 ,  2.3000000 ,  2.2800000 ,  2.2600000 ,  2.2400000 ,  2.2200000 ,
 2.2000000 ,  2.1800000 ,  2.1600000 ,  2.1400000 ,  2.1200000 ,  2.1000000 ,  2.0800000 ,  2.0600000 ,  2.0400000 ,  2.0200000 ,  2.0000000 ,  1.9800000 ,
 1.9600000 ,  1.9400000 ,  1.9200000 ,  1.9000000 ,  1.8800000 ,  1.8600000 ,  1.8400000 ,  1.8200000 ,  1.8000000 ,  1.7800000 ,  1.7600000 ,  1.7400000 ,
 1.7200000 ,  1.7000000 ,  1.6800000 ,  1.6600000 ,  1.6400000 ,  1.6200000 ,  1.6000000 ,  1.5800000 ,  1.5600000 ,  1.5400000 ,  1.5200000 ,  1.5000000 ,
 1.4800000 ,  1.4600000 ,  1.4400000 ,  1.4200000 ,  1.4000000 ,  1.3800000 ,  1.3600000 ,  1.3400000 ,  1.3200000 ,  1.3000000 ,  1.2800000 ,  1.2600000 ,
 1.2400000 ,  1.2200000 ,  1.2000000 ,  1.1800000 ,  1.1600000 ,  1.1400000 ,  1.1200000 ,  1.1000000 ,  1.0800000 ,  1.0600000 ,  1.0400000 ,  1.0200000 ,
 1.0000000 ,  0.9800000 ,  0.9600000 ,  0.9400000 ,  0.9200000 ,  0.9000000 ,  0.8800000 ,  0.8600000 ,  0.8400000 ,  0.8200000 ,  0.8000000 ,  0.7800000 ,
 0.7600000 ,  0.7400000 ,  0.7200000 ,  0.7000000 ,  0.6800000 ,  0.6600000 ,  0.6400000 ,  0.6200000 ,  0.6000000 ,  0.5800000 ,  0.5600000 ,  0.5400000 ,
 0.5200000 ,  0.5000000 ,  0.4800000 ,  0.4600000 ,  0.4400000 ,  0.4200000 ,  0.4000000 ,  0.3800000 ,  0.3600000 ,  0.3400000 ,  0.3200000 ,  0.3000000 ,
 0.2800000 ,  0.2600000 ,  0.2400000 ,  0.2200000 ,  0.2000000 ,  0.1800000 ,  0.1600000 ,  0.1400000 ,  0.1200000 ,  0.1000000 ,  0.0800000 ,  0.0600000 ,
 0.0400000 ,  0.0200000 ,  0.0000000
};
/*
AudioConnection,	dcToEnvFilter[8] =
{
		AudioConnection(dc[0],envelopeFilter[0]),
		AudioConnection(dc[1],envelopeFilter[1]),
		AudioConnection(dc[2],envelopeFilter[2]),
		AudioConnection(dc[3],envelopeFilter[3]),
		AudioConnection(dc[4],envelopeFilter[4]),
		AudioConnection(dc[5],envelopeFilter[5]),
		AudioConnection(dc[6],envelopeFilter[6]),
		AudioConnection(dc[7],envelopeFilter[7])
};

AudioConnection			playMemToFilter[8] =
{
		AudioConnection(playMem[0],0, filter[0],0),
		AudioConnection(playMem[1],0, filter[1],0),
		AudioConnection(playMem[2],0, filter[2],0),
		AudioConnection(playMem[3],0, filter[3],0),
		AudioConnection(playMem[4],0, filter[4],0),
		AudioConnection(playMem[5],0, filter[5],0),
		AudioConnection(playMem[6],0, filter[6],0),
		AudioConnection(playMem[7],0, filter[7],0)
};

AudioConnection			envToFilter[8] =
{
		AudioConnection(envelopeFilter[0], 0, filter[0], 1),
		AudioConnection(envelopeFilter[1], 0, filter[1], 1),
		AudioConnection(envelopeFilter[2], 0, filter[2], 1),
		AudioConnection(envelopeFilter[3], 0, filter[3], 1),
		AudioConnection(envelopeFilter[4], 0, filter[4], 1),
		AudioConnection(envelopeFilter[5], 0, filter[5], 1),
		AudioConnection(envelopeFilter[6], 0, filter[6], 1),
		AudioConnection(envelopeFilter[7], 0, filter[7], 1)
};

AudioConnection			filterToEnvAmp[8] =
{
		AudioConnection(filter[0], 0, envelopeAmp[0], 0),
		AudioConnection(filter[1], 0, envelopeAmp[1], 0),
		AudioConnection(filter[2], 0, envelopeAmp[2], 0),
		AudioConnection(filter[3], 0, envelopeAmp[3], 0),
		AudioConnection(filter[4], 0, envelopeAmp[4], 0),
		AudioConnection(filter[5], 0, envelopeAmp[5], 0),
		AudioConnection(filter[6], 0, envelopeAmp[6], 0),
		AudioConnection(filter[7], 0, envelopeAmp[7], 0)
};

AudioConnection			envToAmp[8] =
{
		AudioConnection(envelopeAmp[0], amp[0]),
		AudioConnection(envelopeAmp[1], amp[1]),
		AudioConnection(envelopeAmp[2], amp[2]),
		AudioConnection(envelopeAmp[3], amp[3]),
		AudioConnection(envelopeAmp[4], amp[4]),
		AudioConnection(envelopeAmp[5], amp[5]),
		AudioConnection(envelopeAmp[6], amp[6]),
		AudioConnection(envelopeAmp[7], amp[7])
};

AudioConnection			ampToMixerL[8] =
{
		AudioConnection(amp[0], 0, mixerL, 0),
		AudioConnection(amp[1], 0, mixerL, 1),
		AudioConnection(amp[2], 0, mixerL, 2),
		AudioConnection(amp[3], 0, mixerL, 3),
		AudioConnection(amp[4], 0, mixerL, 4),
		AudioConnection(amp[5], 0, mixerL, 5),
		AudioConnection(amp[6], 0, mixerL, 6),
		AudioConnection(amp[7], 0, mixerL, 7)
};

AudioConnection			ampToMixerR[8] =
{
		AudioConnection(amp[0], 0, mixerR, 0),
		AudioConnection(amp[1], 0, mixerR, 1),
		AudioConnection(amp[2], 0, mixerR, 2),
		AudioConnection(amp[3], 0, mixerR, 3),
		AudioConnection(amp[4], 0, mixerR, 4),
		AudioConnection(amp[5], 0, mixerR, 5),
		AudioConnection(amp[6], 0, mixerR, 6),
		AudioConnection(amp[7], 0, mixerR, 7)
};
*/

AudioPlayMemory          playMem[8];
AudioEffectEnvelope      envelopeAmp[8];
AudioEffectEnvelope      envelopeFilter[8];
AudioFilterStateVariable filter[8];
AudioAmplifier           amp[8];
AudioMixer8				 mixerL,mixerR;
AudioOutputI2S           i2s1;
AudioSynthWaveformDc     dc[8];

AudioConnection          connect1(dc[0], envelopeFilter[0]);
AudioConnection          connect2(dc[1], envelopeFilter[1]);
AudioConnection          connect3(dc[2], envelopeFilter[2]);
AudioConnection          connect4(dc[3], envelopeFilter[3]);
AudioConnection          connect5(dc[4], envelopeFilter[4]);
AudioConnection          connect6(dc[5], envelopeFilter[5]);
AudioConnection          connect7(dc[6], envelopeFilter[6]);
AudioConnection          connect8(dc[7], envelopeFilter[7]);

AudioConnection          connect9(playMem[0], 0, filter[0], 0);
AudioConnection          connect10(playMem[1], 0, filter[1], 0);
AudioConnection          connect11(playMem[2], 0, filter[2], 0);
AudioConnection          connect12(playMem[3], 0, filter[3], 0);
AudioConnection          connect13(playMem[4], 0, filter[4], 0);
AudioConnection          connect14(playMem[5], 0, filter[5], 0);
AudioConnection          connect15(playMem[6], 0, filter[6], 0);
AudioConnection          connect16(playMem[7], 0, filter[7], 0);

AudioConnection          connect17(envelopeFilter[0], 0, filter[0], 1);
AudioConnection          connect18(envelopeFilter[1], 0, filter[1], 1);
AudioConnection          connect19(envelopeFilter[2], 0, filter[2], 1);
AudioConnection          connect20(envelopeFilter[3], 0, filter[3], 1);
AudioConnection          connect21(envelopeFilter[4], 0, filter[4], 1);
AudioConnection          connect22(envelopeFilter[5], 0, filter[5], 1);
AudioConnection          connect23(envelopeFilter[6], 0, filter[6], 1);
AudioConnection          connect24(envelopeFilter[7], 0, filter[7], 1);

AudioConnection          connect25(filter[0], 0, envelopeAmp[0], 0);
AudioConnection          connect26(filter[1], 0, envelopeAmp[1], 0);
AudioConnection          connect27(filter[2], 0, envelopeAmp[2], 0);
AudioConnection          connect28(filter[3], 0, envelopeAmp[3], 0);
AudioConnection          connect29(filter[4], 0, envelopeAmp[4], 0);
AudioConnection          connect30(filter[5], 0, envelopeAmp[5], 0);
AudioConnection          connect31(filter[6], 0, envelopeAmp[6], 0);
AudioConnection          connect32(filter[7], 0, envelopeAmp[7], 0);

AudioConnection          connect33(envelopeAmp[0], amp[0]);
AudioConnection          connect34(envelopeAmp[1], amp[1]);
AudioConnection          connect35(envelopeAmp[2], amp[2]);
AudioConnection          connect36(envelopeAmp[3], amp[3]);
AudioConnection          connect37(envelopeAmp[4], amp[4]);
AudioConnection          connect38(envelopeAmp[5], amp[5]);
AudioConnection          connect39(envelopeAmp[6], amp[6]);
AudioConnection          connect40(envelopeAmp[7], amp[7]);

AudioConnection          connect41(amp[0], 0, mixerL, 0);
AudioConnection          connect42(amp[1], 0, mixerL, 1);
AudioConnection          connect43(amp[2], 0, mixerL, 2);
AudioConnection          connect44(amp[3], 0, mixerL, 3);
AudioConnection          connect45(amp[4], 0, mixerL, 4);
AudioConnection          connect46(amp[5], 0, mixerL, 5);
AudioConnection          connect47(amp[6], 0, mixerL, 6);
AudioConnection          connect48(amp[7], 0, mixerL, 7);


AudioConnection          connect49(amp[0], 0, mixerR, 0);
AudioConnection          connect50(amp[1], 0, mixerR, 1);
AudioConnection          connect51(amp[2], 0, mixerR, 2);
AudioConnection          connect52(amp[3], 0, mixerR, 3);
AudioConnection          connect53(amp[4], 0, mixerR, 4);
AudioConnection          connect54(amp[5], 0, mixerR, 5);
AudioConnection          connect55(amp[6], 0, mixerR, 6);
AudioConnection          connect56(amp[7], 0, mixerR, 7);

AudioConnection         connect57(mixerL, 0, i2s1, 1);
AudioConnection         connect58(mixerR, 0, i2s1, 0);



	instrumentEngine instrumentPlayer[8];
	strMtModAudioEngine modAudioEngine[8];
	audioEngine engine;

	void audioEngine::init()
	{
		for(int i=0;i<8; i++)
		{
			instrumentPlayer[i].init(&playMem[i],&envelopeFilter[i],&dc[i],&filter[i],&envelopeAmp[i], &amp[i], i);
		}

	}
	void audioEngine::update()
	{
		for(int i=0; i<8; i++)
		{
			instrumentPlayer[i].update();
		}
	}

	void instrumentEngine::init(AudioPlayMemory * playMem,AudioEffectEnvelope * envFilter,AudioSynthWaveformDc * dcFilter,AudioFilterStateVariable * filter,
			AudioEffectEnvelope * envAmp, AudioAmplifier * amp, uint8_t panCh)
	{
		playMemPtr=playMem;
		envelopeAmpPtr=envAmp;
		envelopeFilterPtr=envFilter;
		dcFilterPtr=dcFilter;
		filterPtr=filter;
		ampPtr=amp;
		numPanChannel=panCh;
		switch(panCh)
		{
		case 0:
			conFilterToAmpPtr=&connect25;
			break;
		case 1:
			conFilterToAmpPtr=&connect26;
			break;
		case 2:
			conFilterToAmpPtr=&connect27;
			break;
		case 3:
			conFilterToAmpPtr=&connect28;
			break;
		case 4:
			conFilterToAmpPtr=&connect29;
			break;
		case 5:
			conFilterToAmpPtr=&connect30;
			break;
		case 6:
			conFilterToAmpPtr=&connect31;
			break;
		case 7:
			conFilterToAmpPtr=&connect32;
			break;
		default:
			break;
		}

	}

	uint8_t instrumentEngine :: play(strStep * step,strMtModAudioEngine * mod)
	{
		uint8_t status;
		float gainL=0,gainR=0;

		/*================================================ENVELOPE AMP==========================================*/
		envelopeAmpPtr->delay(mtProject.instrument[step->instrumentIndex].envelopeAmp.delay);
		envelopeAmpPtr->attack(mtProject.instrument[step->instrumentIndex].envelopeAmp.attack);
		envelopeAmpPtr->hold(mtProject.instrument[step->instrumentIndex].envelopeAmp.hold);
		envelopeAmpPtr->decay(mtProject.instrument[step->instrumentIndex].envelopeAmp.decay);
		envelopeAmpPtr->sustain(mtProject.instrument[step->instrumentIndex].envelopeAmp.sustain);
		envelopeAmpPtr->release(mtProject.instrument[step->instrumentIndex].envelopeAmp.release);
		/*======================================================================================================*/
		/*================================================ENVELOPE FILTER=======================================*/
		envelopeFilterPtr->delay(mtProject.instrument[step->instrumentIndex].envelopeFilter.delay);
		envelopeFilterPtr->attack(mtProject.instrument[step->instrumentIndex].envelopeFilter.attack);
		envelopeFilterPtr->hold(mtProject.instrument[step->instrumentIndex].envelopeFilter.hold);
		envelopeFilterPtr->decay(mtProject.instrument[step->instrumentIndex].envelopeFilter.decay);
		envelopeFilterPtr->sustain(mtProject.instrument[step->instrumentIndex].envelopeFilter.sustain);
		envelopeFilterPtr->release(mtProject.instrument[step->instrumentIndex].envelopeFilter.release);

		/*======================================================================================================*/
		/*================================================FILTER================================================*/
		changeFilterType(mtProject.instrument[step->instrumentIndex].filterType);
		filterPtr->resonance(mtProject.instrument[step->instrumentIndex].resonance);

		if(mtProject.instrument[step->instrumentIndex].filterEnvelope == envelopeOff)
			{
			filterPtr->frequency(filterFreq[mtProject.instrument[step->instrumentIndex].freq]);
			dcFilterPtr->amplitude(0.0);
			}
		else if(mtProject.instrument[step->instrumentIndex].filterEnvelope == envelopeOn)
		{
			if(mtProject.instrument[step->instrumentIndex].filterType == highPass)
			{
				dcFilterPtr->amplitude(-1.0);

				filterPtr->frequency(HIGH_PASS_FILTER_FREQ);
				filterPtr->octaveControl(filterOctaves[mtProject.instrument[step->instrumentIndex].freq], highPass);
			}
			else if(mtProject.instrument[step->instrumentIndex].filterType == lowPass)
			{
				dcFilterPtr->amplitude(1.0);

				filterPtr->frequency(LOW_PASS_FILTER_FREQ);
				filterPtr->octaveControl(filterOctaves[OCTAVES_BUFFOR_SIZE - mtProject.instrument[step->instrumentIndex].freq - 1],lowPass);
			}
		}
		/*======================================================================================================*/
		/*==================================================GAIN================================================*/
		ampPtr->gain( (step->volume/100.0) * mtProject.instrument[step->instrumentIndex].volume);
		/*======================================================================================================*/
		/*===============================================PANNING================================================*/

		gainL=mtProject.instrument[step->instrumentIndex].panning/100.0;
		gainR=(100-mtProject.instrument[step->instrumentIndex].panning)/100.0;

		mixerL.gain(numPanChannel,gainL);
		mixerR.gain(numPanChannel,gainR);
		/*======================================================================================================*/

		status = playMemPtr->play(step,mod);
		envelopeAmpPtr->noteOn();
		if(mtProject.instrument[step->instrumentIndex].filterEnvelope == envelopeOn) envelopeFilterPtr->noteOn();

		return status;

	}
	uint8_t instrumentEngine :: play(strInstrument * instr,strMtModAudioEngine * mod)
	{
		uint8_t status=0;
		/*================================================ENVELOPE AMP==========================================*/
		envelopeAmpPtr->delay(instr->envelopeAmp.delay);
		envelopeAmpPtr->attack(instr->envelopeAmp.attack);
		envelopeAmpPtr->hold(instr->envelopeAmp.hold);
		envelopeAmpPtr->decay(instr->envelopeAmp.decay);
		envelopeAmpPtr->sustain(instr->envelopeAmp.sustain);
		envelopeAmpPtr->release(instr->envelopeAmp.release);
		/*======================================================================================================*/
		/*================================================ENVELOPE FILTER=======================================*/
		envelopeFilterPtr->delay(instr->envelopeFilter.delay);
		envelopeFilterPtr->attack(instr->envelopeFilter.attack);
		envelopeFilterPtr->hold(instr->envelopeFilter.hold);
		envelopeFilterPtr->decay(instr->envelopeFilter.decay);
		envelopeFilterPtr->sustain(instr->envelopeFilter.sustain);
		envelopeFilterPtr->release(instr->envelopeFilter.release);
		/*======================================================================================================*/
		/*================================================FILTER================================================*/
		changeFilterType(instr->filterType);
		filterPtr->resonance(instr->resonance);

		if(instr->filterEnvelope == envelopeOff)
			{
			filterPtr->frequency(filterFreq[instr->freq]);
			dcFilterPtr->amplitude(0.0);
			}
		else if(instr->filterEnvelope == envelopeOn)
		{
			if(instr->filterType == highPass)
			{
				dcFilterPtr->amplitude(-1.0);

				filterPtr->frequency(HIGH_PASS_FILTER_FREQ);
				filterPtr->octaveControl(filterOctaves[instr->freq], highPass);
			}
			else if(instr->filterType == lowPass)
			{
				dcFilterPtr->amplitude(1.0);
				filterPtr->frequency(LOW_PASS_FILTER_FREQ);
				filterPtr->octaveControl(filterOctaves[OCTAVES_BUFFOR_SIZE - instr->freq - 1],lowPass);
			}
		}
		/*======================================================================================================*/
		/*==================================================GAIN================================================*/
		ampPtr->gain(instr->volume);
		/*======================================================================================================*/
		/*===============================================PANNING================================================*/
		mixerL.gain(numPanChannel,instr->panning/100.0);
		mixerR.gain(numPanChannel,(100-instr->panning)/100.0);
		/*======================================================================================================*/
		status = playMemPtr->play(instr,mod,1.0,24);
		envelopeAmpPtr->noteOn();

		return status;

	}


	uint8_t instrumentEngine :: change(strStep * step, strMtModAudioEngine * mod)
	{
		uint8_t status=0;
		float gainL=0,gainR=0;

		/*================================================FILTER================================================*/
		changeFilterType(mtProject.instrument[step->instrumentIndex].filterType);
		filterPtr->resonance(mtProject.instrument[step->instrumentIndex].resonance);

		if(mtProject.instrument[step->instrumentIndex].filterEnvelope == envelopeOff)
		{
			filterPtr->frequency(filterFreq[mtProject.instrument[step->instrumentIndex].freq]);
			dcFilterPtr->amplitude(0.0);
		}
		else if(mtProject.instrument[step->instrumentIndex].filterEnvelope == envelopeOn)
		{
			if(mtProject.instrument[step->instrumentIndex].filterType == highPass)
			{
				dcFilterPtr->amplitude(-1.0);
				if(mtProject.instrument[step->instrumentIndex].freq > OCTAVES_BUFFOR_SIZE)
				{
					filterPtr->frequency(HIGH_PASS_FILTER_FREQ_7UP);
					filterPtr->octaveControl(filterOctaves[mtProject.instrument[step->instrumentIndex].freq- OCTAVES_BUFFOR_SIZE - 1], highPass);
				}
				else
				{
					filterPtr->frequency(HIGH_PASS_FILTER_FREQ);
					filterPtr->octaveControl(filterOctaves[mtProject.instrument[step->instrumentIndex].freq], highPass);
				}

			}
			else if(mtProject.instrument[step->instrumentIndex].filterType == lowPass)
			{
				dcFilterPtr->amplitude(1.0);
				if(mtProject.instrument[step->instrumentIndex].freq > OCTAVES_BUFFOR_SIZE)
				{
					filterPtr->frequency(LOW_PASS_FILTER_FREQ_7DOWN);
					filterPtr->octaveControl(filterOctaves[OCTAVES_BUFFOR_SIZE -(mtProject.instrument[step->instrumentIndex].freq - OCTAVES_BUFFOR_SIZE) - 1],lowPass);
				}
				else
				{
					filterPtr->frequency(LOW_PASS_FILTER_FREQ);
					filterPtr->octaveControl(filterOctaves[OCTAVES_BUFFOR_SIZE - mtProject.instrument[step->instrumentIndex].freq - 1],lowPass);
				}
			}
		}
		/*======================================================================================================*/

		/*==================================================GAIN================================================*/
		ampPtr->gain( (step->volume/100.0) * mtProject.instrument[step->instrumentIndex].volume);
		/*======================================================================================================*/
		/*===============================================PANNING================================================*/
		gainL=mtProject.instrument[step->instrumentIndex].panning/100.0;
		gainR=(100-mtProject.instrument[step->instrumentIndex].panning)/100.0;

		mixerL.gain(numPanChannel,gainL);
		mixerR.gain(numPanChannel,gainR);
		/*======================================================================================================*/

		status=playMemPtr->setMod(step,mod);


		return status;
	}
	uint8_t instrumentEngine :: change(strInstrument * instr, strMtModAudioEngine * mod)
	{
		uint8_t status=0;
		/*================================================FILTER================================================*/
		changeFilterType(instr->filterType);
		filterPtr->resonance(instr->resonance);

		if(instr->filterEnvelope == envelopeOff)
			{
			filterPtr->frequency(filterFreq[instr->freq]);
			dcFilterPtr->amplitude(0.0);
			}
		else if(instr->filterEnvelope == envelopeOn)
		{
			if(instr->filterType == highPass)
			{
				dcFilterPtr->amplitude(-1.0);
				if(instr->freq > OCTAVES_BUFFOR_SIZE)
				{
					filterPtr->frequency(HIGH_PASS_FILTER_FREQ_7UP);
					filterPtr->octaveControl(filterOctaves[instr->freq- OCTAVES_BUFFOR_SIZE - 1], highPass);
				}
				else
				{
					filterPtr->frequency(HIGH_PASS_FILTER_FREQ);
					filterPtr->octaveControl(filterOctaves[instr->freq], highPass);
				}

			}
			else if(instr->filterType == lowPass)
			{
				dcFilterPtr->amplitude(1.0);
				if(instr->freq > OCTAVES_BUFFOR_SIZE)
				{
					filterPtr->frequency(LOW_PASS_FILTER_FREQ_7DOWN);
					filterPtr->octaveControl(filterOctaves[OCTAVES_BUFFOR_SIZE -(instr->freq - OCTAVES_BUFFOR_SIZE)  - 1],lowPass);
				}
				else
				{
					filterPtr->frequency(LOW_PASS_FILTER_FREQ);
					filterPtr->octaveControl(filterOctaves[OCTAVES_BUFFOR_SIZE - instr->freq - 1],lowPass);
				}
			}
		}
		/*======================================================================================================*/
		/*==================================================GAIN================================================*/
		ampPtr->gain(instr->volume);
		/*======================================================================================================*/
		/*===============================================PANNING================================================*/
		mixerL.gain(numPanChannel,instr->panning/100.0);
		mixerR.gain(numPanChannel,(100-instr->panning)/100.0);
		/*======================================================================================================*/

		status=playMemPtr->setMod(instr,mod,24);

		return status;
	}

	void instrumentEngine:: update()
	{
		if(envelopeAmpPtr->endRelease()) playMemPtr->stop();
	}

	void instrumentEngine :: stop()
	{
		envelopeAmpPtr->noteOff();
		envelopeFilterPtr->noteOff();
	}

	void instrumentEngine :: changeFilterType(uint8_t type)
	{
		conFilterToAmpPtr->disconnect();
		if(type == lowPass) conFilterToAmpPtr->src_index=0;
		else if(type == bandPass) conFilterToAmpPtr->src_index=1;
		else if(type ==  highPass) conFilterToAmpPtr->src_index=2;
		conFilterToAmpPtr->connect();
	}

