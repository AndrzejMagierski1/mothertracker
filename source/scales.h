#ifndef _SCALES_H_
#define _SCALES_H_

#include <stdint.h>

const uint8_t MIN_SCALE = 0;
const uint8_t MAX_SCALE = 39;


uint8_t isInScale(uint8_t note,		// nuta do sprawdzenia ,
                  uint8_t root,		// root note
                  uint8_t scale);	// numer skali


const uint16_t scaleDef[] =
{
	0b111111111111, // 0
	0b111111111111, // 1
	0b010110101101, // 2 minor 
	0b101010110101, // 3 major
	0b011010101101, // 4
	0b101011010101, // 5
	0b010111010101, // 6
	0b010101101011, // 7
	0b010110101011, // 8
	0b010110110011, // 9
	0b011010110101, // 10
	0b011010101101, // 11
	0b100110101101, // 12
	0b101110110101, // 13
	0b011010111101, // 14
	0b111010110101, // 15
	0b010011101001, // 16
	0b110110101101, // 17
	0b010010101001, // 18
	0b001010010101, // 19
	0b100111001101, // 20
	0b011011001101, // 21
	0b101011010011, // 22
	0b100011101011, // 23
	0b010101010101, // 24
	0b101101101101, // 25
	0b010101011011, // 26
	0b000110001101, // 27
	0b010010100011, // 28
	0b001010100101, // 29
	0b010001100011, // 30
	0b101101101101, // 31
	0b001010001101, // 32
	0b011011010101, // 33
	0b100110110011, // 34
	0b010110110001, // 35
	0b100110110011, // 36
	0b101010101011, // 37
	0b100110101011, // 38
	0b110101010011, // 39
	0b101101101101, // 40
	0b101101101101, // 41
	0b101101101101, // 42
	0b101101101101, // 43
	0b101101101101, // 44
	0b101101101101, // 45
	0b101101101101, // 46
	0b101101101101, // 47
	0b101101101101, // 48
	0b101101101101, // 49
	0b101101101101, // 50
	0b101101101101, // 51
	0b101101101101, // 52
	0b101101101101, // 53
	0b101101101101, // 54
	0b101101101101, // 55
	0b101101101101, // 56
	0b101101101101, // 57
	0b101101101101, // 58
	0b101101101101, // 59
	0b101101101101, // 60
	0b101101101101, // 61
	0b101101101101, // 62
	0b101101101101, // 63
	0b101101101101, // 64
	0b101101101101, // 65
	0b101101101101, // 66
	0b101101101101, // 67
	0b101101101101, // 68
	0b101101101101, // 69
	0b101101101101, // 70
	0b101101101101, // 71
	0b101101101101, // 72
	0b101101101101, // 73
	0b101101101101, // 74
	0b101101101101, // 75
	0b101101101101, // 76
	0b101101101101, // 77
	0b101101101101, // 78
	0b101101101101, // 79
	0b101101101101, // 80
	0b101101101101, // 81
	0b101101101101, // 82
	0b101101101101, // 83
	0b101101101101, // 84
	0b101101101101, // 85
	0b101101101101, // 86
	0b101101101101, // 87
	0b101101101101, // 88
	0b101101101101, // 89
	0b101101101101, // 90
	0b101101101101, // 91
	0b101101101101, // 92
	0b101101101101, // 93
	0b101101101101, // 94
	0b101101101101, // 95
	0b101101101101, // 96
	0b101101101101, // 97
	0b101101101101, // 98
	0b101101101101, // 99
	0b101101101101, // 100

};

const char scaleNames[][10] =
{
"No scale" ,
"Chromatic",
"Minor"    ,
"Major"    ,
"Dorian"   ,
"Lyd Maj"  ,
"Lyd Min"  ,
"Locrian"  ,
"Phrygian ",
"PhrygDom" ,
"Mixlydian",
"Melo Min" ,
"Harm Min" ,
"BeBopMaj" ,
"BeBopDor" ,
"BeBopMix" ,
"Blues Min",
"Blues Maj",
"Penta Min",
"Penta Maj",
"Hung Min" ,
"Ukrainian",
"Marva"    ,
"Todi"     ,
"Wholetone",
"Dim"      ,
"SLocrian" ,
"Hirajoshi",
"In Sen"   ,
"Yo"       ,
"Iwato"    ,
"WholeHalf",
"Kumoi"    ,
"Overtone" ,
"DoubHarm" ,
"Indian"   ,
"Gipsy"    ,
"NeapoMaj" ,
"NeapoMin" ,
"Enigmatic",
"test40"   ,
"test41"   ,
"test42"   ,
"test43"   ,
"test44"   ,
"test45"   ,
"test46"   ,
"test47"   ,
"test48"   ,
"test49"   ,
"test50"   ,
"test51"   ,
"test52"   ,
"test53"   ,
"test54"   ,
"test55"   ,
"test56"   ,
"test57"   ,
"test58"   ,
"test59"   ,
"test60"   ,
"test61"   ,
"test62"   ,
"test63"   ,
"test64"   ,
"test65"   ,
"test66"   ,
"test67"   ,
"test68"   ,
"test69"   ,
"test70"   ,
"test71"   ,
"test72"   ,
"test73"   ,
"test74"   ,
"test75"   ,
"test76"   ,
"test77"   ,
"test78"   ,
"test79"   ,
"test80"   ,
"test81"   ,
"test82"   ,
"test83"   ,
"test84"   ,
"test85"   ,
"test86"   ,
"test87"   ,
"test88"   ,
"test89"   ,
"test90"   ,
"test91"   ,
"test92"   ,
"test93"   ,
"test94"   ,
"test95"   ,
"test96"   ,
"test97"   ,
"test98"   ,
"test99"   ,
"test100"  ,
};


// const uint8_t scaleNames[]
#define SCALENAME0 "No scale"
#define SCALENAME1 "Chromatic"
#define SCALENAME2 "Minor"
#define SCALENAME3 "Major"
#define SCALENAME4 "Dorian"
#define SCALENAME5 "Lyd Maj"
#define SCALENAME6 "Lyd Min"
#define SCALENAME7 "Locrian"
#define SCALENAME8 "Phrygian "
#define SCALENAME9 "PhrygDom"
#define SCALENAME10 "Mixlydian"
#define SCALENAME11 "Melo Min"
#define SCALENAME12 "Harm Min"
#define SCALENAME13 "BeBopMaj"
#define SCALENAME14 "BeBopDor"
#define SCALENAME15 "BeBopMix"
#define SCALENAME16 "Blues Min"
#define SCALENAME17 "Blues Maj"
#define SCALENAME18 "Penta Min"
#define SCALENAME19 "Penta Maj"
#define SCALENAME20 "Hung Min"
#define SCALENAME21 "Ukrainian"
#define SCALENAME22 "Marva"
#define SCALENAME23 "Todi"
#define SCALENAME24 "Wholetone"
#define SCALENAME25 "Dim"
#define SCALENAME26 "SLocrian"
#define SCALENAME27 "Hirajoshi"
#define SCALENAME28 "In Sen"
#define SCALENAME29 "Yo"
#define SCALENAME30 "Iwato"
#define SCALENAME31 "WholeHalf"
#define SCALENAME32 "Kumoi"
#define SCALENAME33 "Overtone"
#define SCALENAME34 "DoubHarm"
#define SCALENAME35 "Indian"
#define SCALENAME36 "Gipsy"
#define SCALENAME37 "NeapoMaj"
#define SCALENAME38 "NeapoMin"
#define SCALENAME39 "Enigmatic"
#define SCALENAME40 "test40"
#define SCALENAME41 "test41"
#define SCALENAME42 "test42"
#define SCALENAME43 "test43"
#define SCALENAME44 "test44"
#define SCALENAME45 "test45"
#define SCALENAME46 "test46"
#define SCALENAME47 "test47"
#define SCALENAME48 "test48"
#define SCALENAME49 "test49"
#define SCALENAME50 "test50"
#define SCALENAME51 "test51"
#define SCALENAME52 "test52"
#define SCALENAME53 "test53"
#define SCALENAME54 "test54"
#define SCALENAME55 "test55"
#define SCALENAME56 "test56"
#define SCALENAME57 "test57"
#define SCALENAME58 "test58"
#define SCALENAME59 "test59"
#define SCALENAME60 "test60"
#define SCALENAME61 "test61"
#define SCALENAME62 "test62"
#define SCALENAME63 "test63"
#define SCALENAME64 "test64"
#define SCALENAME65 "test65"
#define SCALENAME66 "test66"
#define SCALENAME67 "test67"
#define SCALENAME68 "test68"
#define SCALENAME69 "test69"
#define SCALENAME70 "test70"
#define SCALENAME71 "test71"
#define SCALENAME72 "test72"
#define SCALENAME73 "test73"
#define SCALENAME74 "test74"
#define SCALENAME75 "test75"
#define SCALENAME76 "test76"
#define SCALENAME77 "test77"
#define SCALENAME78 "test78"
#define SCALENAME79 "test79"
#define SCALENAME80 "test80"
#define SCALENAME81 "test81"
#define SCALENAME82 "test82"
#define SCALENAME83 "test83"
#define SCALENAME84 "test84"
#define SCALENAME85 "test85"
#define SCALENAME86 "test86"
#define SCALENAME87 "test87"
#define SCALENAME88 "test88"
#define SCALENAME89 "test89"
#define SCALENAME90 "test90"
#define SCALENAME91 "test91"
#define SCALENAME92 "test92"
#define SCALENAME93 "test93"
#define SCALENAME94 "test94"
#define SCALENAME95 "test95"
#define SCALENAME96 "test96"
#define SCALENAME97 "test97"
#define SCALENAME98 "test98"
#define SCALENAME99 "test99"
#define SCALENAME100 "test100"



const char* const ptrScaleNames[40] =
{
		&scaleNames[0][0],
		&scaleNames[1][0],
		&scaleNames[2][0],
		&scaleNames[3][0],
		&scaleNames[4][0],
		&scaleNames[5][0],
		&scaleNames[6][0],
		&scaleNames[7][0],
		&scaleNames[8][0],
		&scaleNames[9][0],
		&scaleNames[10][0],
		&scaleNames[11][0],
		&scaleNames[12][0],
		&scaleNames[13][0],
		&scaleNames[14][0],
		&scaleNames[15][0],
		&scaleNames[16][0],
		&scaleNames[17][0],
		&scaleNames[18][0],
		&scaleNames[19][0],
		&scaleNames[20][0],
		&scaleNames[21][0],
		&scaleNames[22][0],
		&scaleNames[23][0],
		&scaleNames[24][0],
		&scaleNames[25][0],
		&scaleNames[26][0],
		&scaleNames[27][0],
		&scaleNames[28][0],
		&scaleNames[29][0],
		&scaleNames[30][0],
		&scaleNames[31][0],
		&scaleNames[32][0],
		&scaleNames[33][0],
		&scaleNames[34][0],
		&scaleNames[35][0],
		&scaleNames[36][0],
		&scaleNames[37][0],
		&scaleNames[38][0],
		&scaleNames[39][0],
};


const char allNotes[128][5] =
{
	"C0",	"C#0",	"D0",	"D#0",	"E0",	"F0",	"F#0",	"G0",	"G#0",	"A0",	"A#0",	"B0",
	"C1",	"C#1",	"D1",	"D#1",	"E1",	"F1",	"F#1",	"G1",	"G#1",	"A1",	"A#1",	"B1",
	"C2",	"C#2",	"D2",	"D#2",	"E2",	"F2",	"F#2",	"G2",	"G#2",	"A2",	"A#2",	"B2",
	"C3",	"C#3",	"D3",	"D#3",	"E3",	"F3",	"F#3",	"G3",	"G#3",	"A3",	"A#3",	"B3",
	"C4",	"C#4",	"D4",	"D#4",	"E4",	"F4",	"F#4",	"G4",	"G#4",	"A4",	"A#4",	"B4",
	"C5",	"C#5",	"D5",	"D#5",	"E5",	"F5",	"F#5",	"G5",	"G#5",	"A5",	"A#5",	"B5",
	"C6",	"C#6",	"D6",	"D#6",	"E6",	"F6",	"F#6",	"G6",	"G#6",	"A6",	"A#6",	"B6",
	"C7",	"C#7",	"D7",	"D#7",	"E7",	"F7",	"F#7",	"G7",	"G#7",	"A7",	"A#7",	"B7",
	"C8",	"C#8",	"D8",	"D#8",	"E8",	"F8",	"F#8",	"G8",	"G#8",	"A8",	"A#8",	"B8",
	"C9",	"C#9",	"D9",	"D#9",	"E9",	"F9",	"F#9",	"G9",	"G#9",	"A9",	"A#9",	"B9",
	"C10",	"C#10",	"D10",	"D#10",	"E10",	"F10",	"F#10",	"G10"
};

const char* const ptrAllNotes[128] =
{
		&allNotes[0][0],
		&allNotes[1][0],
		&allNotes[2][0],
		&allNotes[3][0],
		&allNotes[4][0],
		&allNotes[5][0],
		&allNotes[6][0],
		&allNotes[7][0],
		&allNotes[8][0],
		&allNotes[9][0],
		&allNotes[10][0],
		&allNotes[11][0],
		&allNotes[12][0],
		&allNotes[13][0],
		&allNotes[14][0],
		&allNotes[15][0],
		&allNotes[16][0],
		&allNotes[17][0],
		&allNotes[18][0],
		&allNotes[19][0],
		&allNotes[20][0],
		&allNotes[21][0],
		&allNotes[22][0],
		&allNotes[23][0],
		&allNotes[24][0],
		&allNotes[25][0],
		&allNotes[26][0],
		&allNotes[27][0],
		&allNotes[28][0],
		&allNotes[29][0],
		&allNotes[30][0],
		&allNotes[31][0],
		&allNotes[32][0],
		&allNotes[33][0],
		&allNotes[34][0],
		&allNotes[35][0],
		&allNotes[36][0],
		&allNotes[37][0],
		&allNotes[38][0],
		&allNotes[39][0],
		&allNotes[40][0],
		&allNotes[41][0],
		&allNotes[42][0],
		&allNotes[43][0],
		&allNotes[44][0],
		&allNotes[45][0],
		&allNotes[46][0],
		&allNotes[47][0],
		&allNotes[48][0],
		&allNotes[49][0],
		&allNotes[50][0],
		&allNotes[51][0],
		&allNotes[52][0],
		&allNotes[53][0],
		&allNotes[54][0],
		&allNotes[55][0],
		&allNotes[56][0],
		&allNotes[57][0],
		&allNotes[58][0],
		&allNotes[59][0],
		&allNotes[60][0],
		&allNotes[61][0],
};

#endif
