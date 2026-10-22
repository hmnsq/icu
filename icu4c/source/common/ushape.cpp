// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 ******************************************************************************
 *
 *   Copyright (C) 2000-2016, International Business Machines
 *   Corporation and others.  All Rights Reserved.
 *
 ******************************************************************************
 *   file name:  ushape.cpp
 *   encoding:   UTF-8
 *   tab size:   8 (not used)
 *   indentation:4
 *
 *   created on: 2000jun29
 *   created by: Markus W. Scherer
 *
 *   Arabic letter shaping implemented by Ayman Roshdy
 */

#ضمن "unicode/utypes.h"
#ضمن "unicode/uchar.h"
#ضمن "unicode/ustring.h"
#ضمن "unicode/ushape.h"
#ضمن "cmemory.h"
#ضمن "putilimp.h"
#ضمن "ustr_imp.h"
#ضمن "ubidi_props.h"
#ضمن "uassert.h"

#ضمن <limits>
/*
 * This implementation is designed for 16-bit Unicode strings.
 * The main assumption is that the Arabic characters and their
 * presentation forms each fit into a single char16_t.
 * With UTF-8, they occupy 2 or 3 bytes, and more than the ASCII
 * characters.
 */

/*
 * ### TODO in general for letter shaping:
 * - the letter shaping code is UTF-16-unaware; needs update
 *   + especially invertBuffer()?!
 * - needs to handle the "Arabic Tail" that is used in some legacy codepages
 *   as a glyph fragment of wide-glyph letters
 *   + IBM Unicode conversion tables map it to U+200B (ZWSP)
 *   + IBM Egypt has proposed to encode the tail in Unicode among Arabic Presentation Forms
 *   + Unicode 3.2 added U+FE73 ARABIC TAIL FRAGMENT
 */

/* definitions for Arabic letter shaping ------------------------------------ */

#define IRRELEVANT 4
#define LAMTYPE    16
#define ALEFTYPE   32
#define LINKR      1
#define LINKL      2
#define APRESENT   8
#define SHADDA     64
#define CSHADDA    128
#define COMBINE    (SHADDA+CSHADDA)

#define HAMZAFE_CHAR       0xfe80
#define HAMZA06_CHAR       0x0621
#define YEH_HAMZA_CHAR     0x0626
#define YEH_HAMZAFE_CHAR   0xFE89
#define LAMALEF_SPACE_SUB  0xFFFF
#define TASHKEEL_SPACE_SUB 0xFFFE
#define NEW_TAIL_CHAR      0xFE73
#define OLD_TAIL_CHAR      0x200B
#define LAM_CHAR           0x0644
#define SPACE_CHAR         0x0020
#define SHADDA_CHAR        0xFE7C
#define TATWEEL_CHAR       0x0640
#define SHADDA_TATWEEL_CHAR  0xFE7D
#define SHADDA06_CHAR      0x0651

#define SHAPE_MODE   0
#define DESHAPE_MODE 1

بنية uShapeVariables {
     char16_t tailChar;
     uint32_t uShapeLamalefBegin;
     uint32_t uShapeLamalefEnd;
     uint32_t uShapeTashkeelBegin;
     uint32_t uShapeTashkeelEnd;
     صحيح spacesRelativeToTextBeginEnd;
};

ساكن ثابت uint8_t tailFamilyIsolatedFinal[] = {
    /* FEB1 */ 1,
    /* FEB2 */ 1,
    /* FEB3 */ 0,
    /* FEB4 */ 0,
    /* FEB5 */ 1,
    /* FEB6 */ 1,
    /* FEB7 */ 0,
    /* FEB8 */ 0,
    /* FEB9 */ 1,
    /* FEBA */ 1,
    /* FEBB */ 0,
    /* FEBC */ 0,
    /* FEBD */ 1,
    /* FEBE */ 1
};

ساكن ثابت uint8_t tashkeelMedial[] = {
    /* FE70 */ 0,
    /* FE71 */ 1,
    /* FE72 */ 0,
    /* FE73 */ 0,
    /* FE74 */ 0,
    /* FE75 */ 0,
    /* FE76 */ 0,
    /* FE77 */ 1,
    /* FE78 */ 0,
    /* FE79 */ 1,
    /* FE7A */ 0,
    /* FE7B */ 1,
    /* FE7C */ 0,
    /* FE7D */ 1,
    /* FE7E */ 0,
    /* FE7F */ 1
};

ساكن ثابت char16_t yehHamzaToYeh[] =
{
/* isolated*/ 0xFEEF,
/* final   */ 0xFEF0
};

ساكن ثابت uint8_t IrrelevantPos[] = {
    0x0, 0x2, 0x4, 0x6,
    0x8, 0xA, 0xC, 0xE
};


ساكن ثابت char16_t convertLamAlef[] =
{
/*FEF5*/    0x0622,
/*FEF6*/    0x0622,
/*FEF7*/    0x0623,
/*FEF8*/    0x0623,
/*FEF9*/    0x0625,
/*FEFA*/    0x0625,
/*FEFB*/    0x0627,
/*FEFC*/    0x0627
};

ساكن ثابت char16_t araLink[178]=
{
  1           + 32 + 256 * 0x11,/*0x0622*/
  1           + 32 + 256 * 0x13,/*0x0623*/
  1                + 256 * 0x15,/*0x0624*/
  1           + 32 + 256 * 0x17,/*0x0625*/
  1 + 2            + 256 * 0x19,/*0x0626*/
  1           + 32 + 256 * 0x1D,/*0x0627*/
  1 + 2            + 256 * 0x1F,/*0x0628*/
  1                + 256 * 0x23,/*0x0629*/
  1 + 2            + 256 * 0x25,/*0x062A*/
  1 + 2            + 256 * 0x29,/*0x062B*/
  1 + 2            + 256 * 0x2D,/*0x062C*/
  1 + 2            + 256 * 0x31,/*0x062D*/
  1 + 2            + 256 * 0x35,/*0x062E*/
  1                + 256 * 0x39,/*0x062F*/
  1                + 256 * 0x3B,/*0x0630*/
  1                + 256 * 0x3D,/*0x0631*/
  1                + 256 * 0x3F,/*0x0632*/
  1 + 2            + 256 * 0x41,/*0x0633*/
  1 + 2            + 256 * 0x45,/*0x0634*/
  1 + 2            + 256 * 0x49,/*0x0635*/
  1 + 2            + 256 * 0x4D,/*0x0636*/
  1 + 2            + 256 * 0x51,/*0x0637*/
  1 + 2            + 256 * 0x55,/*0x0638*/
  1 + 2            + 256 * 0x59,/*0x0639*/
  1 + 2            + 256 * 0x5D,/*0x063A*/
  0, 0, 0, 0, 0,                /*0x063B-0x063F*/
  1 + 2,                        /*0x0640*/
  1 + 2            + 256 * 0x61,/*0x0641*/
  1 + 2            + 256 * 0x65,/*0x0642*/
  1 + 2            + 256 * 0x69,/*0x0643*/
  1 + 2       + 16 + 256 * 0x6D,/*0x0644*/
  1 + 2            + 256 * 0x71,/*0x0645*/
  1 + 2            + 256 * 0x75,/*0x0646*/
  1 + 2            + 256 * 0x79,/*0x0647*/
  1                + 256 * 0x7D,/*0x0648*/
  1                + 256 * 0x7F,/*0x0649*/
  1 + 2            + 256 * 0x81,/*0x064A*/
         4         + 256 * 1,   /*0x064B*/
         4 + 128   + 256 * 1,   /*0x064C*/
         4 + 128   + 256 * 1,   /*0x064D*/
         4 + 128   + 256 * 1,   /*0x064E*/
         4 + 128   + 256 * 1,   /*0x064F*/
         4 + 128   + 256 * 1,   /*0x0650*/
         4 + 64    + 256 * 3,   /*0x0651*/
         4         + 256 * 1,   /*0x0652*/
         4         + 256 * 7,   /*0x0653*/
         4         + 256 * 8,   /*0x0654*/
         4         + 256 * 8,   /*0x0655*/
         4         + 256 * 1,   /*0x0656*/
  0, 0, 0, 0, 0,                /*0x0657-0x065B*/
  1                + 256 * 0x85,/*0x065C*/
  1                + 256 * 0x87,/*0x065D*/
  1                + 256 * 0x89,/*0x065E*/
  1                + 256 * 0x8B,/*0x065F*/
  0, 0, 0, 0, 0,                /*0x0660-0x0664*/
  0, 0, 0, 0, 0,                /*0x0665-0x0669*/
  0, 0, 0, 0, 0, 0,             /*0x066A-0x066F*/
         4         + 256 * 6,   /*0x0670*/
  1        + 8     + 256 * 0x00,/*0x0671*/
  1            + 32,            /*0x0672*/
  1            + 32,            /*0x0673*/
  0,                            /*0x0674*/
  1            + 32,            /*0x0675*/
  1, 1,                         /*0x0676-0x0677*/
  1 + 2,                        /*0x0678*/
  1 + 2 + 8        + 256 * 0x16,/*0x0679*/
  1 + 2 + 8        + 256 * 0x0E,/*0x067A*/
  1 + 2 + 8        + 256 * 0x02,/*0x067B*/
  1+2, 1+2,                     /*0x67C-0x067D*/
  1+2+8+256 * 0x06, 1+2, 1+2, 1+2, 1+2, 1+2, /*0x067E-0x0683*/
  1+2, 1+2, 1+2+8+256 * 0x2A, 1+2,           /*0x0684-0x0687*/
  1     + 8        + 256 * 0x38,/*0x0688*/
  1, 1, 1,                      /*0x0689-0x068B*/
  1     + 8        + 256 * 0x34,/*0x068C*/
  1     + 8        + 256 * 0x32,/*0x068D*/
  1     + 8        + 256 * 0x36,/*0x068E*/
  1, 1,                         /*0x068F-0x0690*/
  1     + 8        + 256 * 0x3C,/*0x0691*/
  1, 1, 1, 1, 1, 1, 1+8+256 * 0x3A, 1,       /*0x0692-0x0699*/
  1+2, 1+2, 1+2, 1+2, 1+2, 1+2, /*0x069A-0x06A3*/
  1+2, 1+2, 1+2, 1+2,           /*0x069A-0x06A3*/
  1+2, 1+2, 1+2, 1+2, 1+2, 1+2+8+256 * 0x3E, /*0x06A4-0x06AD*/
  1+2, 1+2, 1+2, 1+2,           /*0x06A4-0x06AD*/
  1+2, 1+2+8+256 * 0x42, 1+2, 1+2, 1+2, 1+2, /*0x06AE-0x06B7*/
  1+2, 1+2, 1+2, 1+2,           /*0x06AE-0x06B7*/
  1+2, 1+2,                     /*0x06B8-0x06B9*/
  1     + 8        + 256 * 0x4E,/*0x06BA*/
  1 + 2 + 8        + 256 * 0x50,/*0x06BB*/
  1+2, 1+2,                     /*0x06BC-0x06BD*/
  1 + 2 + 8        + 256 * 0x5A,/*0x06BE*/
  1+2,                          /*0x06BF*/
  1     + 8        + 256 * 0x54,/*0x06C0*/
  1 + 2 + 8        + 256 * 0x56,/*0x06C1*/
  1, 1, 1,                      /*0x06C2-0x06C4*/
  1     + 8        + 256 * 0x90,/*0x06C5*/
  1     + 8        + 256 * 0x89,/*0x06C6*/
  1     + 8        + 256 * 0x87,/*0x06C7*/
  1     + 8        + 256 * 0x8B,/*0x06C8*/
  1     + 8        + 256 * 0x92,/*0x06C9*/
  1,                            /*0x06CA*/
  1     + 8        + 256 * 0x8E,/*0x06CB*/
  1 + 2 + 8        + 256 * 0xAC,/*0x06CC*/
  1,                            /*0x06CD*/
  1+2, 1+2,                     /*0x06CE-0x06CF*/
  1 + 2 + 8        + 256 * 0x94,/*0x06D0*/
  1+2,                          /*0x06D1*/
  1     + 8        + 256 * 0x5E,/*0x06D2*/
  1     + 8        + 256 * 0x60 /*0x06D3*/
};

ساكن ثابت uint8_t presALink[] = {
/***********0*****1*****2*****3*****4*****5*****6*****7*****8*****9*****A*****B*****C*****D*****E*****F*/
/*FB5*/    0,    1,    0,    0,    0,    0,    0,    1,    2,1 + 2,    0,    0,    0,    0,    0,    0,
/*FB6*/    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/*FB7*/    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    1,    2,1 + 2,    0,    0,
/*FB8*/    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    1,    0,    0,    0,    1,
/*FB9*/    2,1 + 2,    0,    1,    2,1 + 2,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/*FBA*/    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/*FBB*/    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/*FBC*/    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/*FBD*/    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/*FBE*/    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/*FBF*/    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    1,    2,1 + 2,
/*FC0*/    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/*FC1*/    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/*FC2*/    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/*FC3*/    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/*FC4*/    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/*FC5*/    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    4,    4,
/*FC6*/    4,    4,    4
};

ساكن ثابت uint8_t presBLink[]=
{
/***********0*****1*****2*****3*****4*****5*****6*****7*****8*****9*****A*****B*****C*****D*****E*****F*/
/*FE7*/1 + 2,1 + 2,1 + 2,    0,1 + 2,    0,1 + 2,1 + 2,1 + 2,1 + 2,1 + 2,1 + 2,1 + 2,1 + 2,1 + 2,1 + 2,
/*FE8*/    0,    0,    1,    0,    1,    0,    1,    0,    1,    0,    1,    2,1 + 2,    0,    1,    0,
/*FE9*/    1,    2,1 + 2,    0,    1,    0,    1,    2,1 + 2,    0,    1,    2,1 + 2,    0,    1,    2,
/*FEA*/1 + 2,    0,    1,    2,1 + 2,    0,    1,    2,1 + 2,    0,    1,    0,    1,    0,    1,    0,
/*FEB*/    1,    0,    1,    2,1 + 2,    0,    1,    2,1 + 2,    0,    1,    2,1 + 2,    0,    1,    2,
/*FEC*/1 + 2,    0,    1,    2,1 + 2,    0,    1,    2,1 + 2,    0,    1,    2,1 + 2,    0,    1,    2,
/*FED*/1 + 2,    0,    1,    2,1 + 2,    0,    1,    2,1 + 2,    0,    1,    2,1 + 2,    0,    1,    2,
/*FEE*/1 + 2,    0,    1,    2,1 + 2,    0,    1,    2,1 + 2,    0,    1,    2,1 + 2,    0,    1,    0,
/*FEF*/    1,    0,    1,    2,1 + 2,    0,    1,    0,    1,    0,    1,    0,    1,    0,    0,    0
};

ساكن ثابت char16_t convertFBto06[] =
{
/***********0******1******2******3******4******5******6******7******8******9******A******B******C******D******E******F***/
/*FB5*/   0x671, 0x671, 0x67B, 0x67B, 0x67B, 0x67B, 0x67E, 0x67E, 0x67E, 0x67E,     0,     0,     0,     0, 0x67A, 0x67A,
/*FB6*/   0x67A, 0x67A,     0,     0,     0,     0, 0x679, 0x679, 0x679, 0x679,     0,     0,     0,     0,     0,     0,
/*FB7*/       0,     0,     0,     0,     0,     0,     0,     0,     0,     0, 0x686, 0x686, 0x686, 0x686,     0,     0,
/*FB8*/       0,     0, 0x68D, 0x68D, 0x68C, 0x68C, 0x68E, 0x68E, 0x688, 0x688, 0x698, 0x698, 0x691, 0x691, 0x6A9, 0x6A9,
/*FB9*/   0x6A9, 0x6A9, 0x6AF, 0x6AF, 0x6AF, 0x6AF,     0,     0,     0,     0,     0,     0,     0,     0, 0x6BA, 0x6BA,
/*FBA*/   0x6BB, 0x6BB, 0x6BB, 0x6BB, 0x6C0, 0x6C0, 0x6C1, 0x6C1, 0x6C1, 0x6C1, 0x6BE, 0x6BE, 0x6BE, 0x6BE, 0x6d2, 0x6D2,
/*FBB*/   0x6D3, 0x6D3,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
/*FBC*/       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
/*FBD*/       0,     0,     0,     0,     0,     0,     0, 0x6C7, 0x6C7, 0x6C6, 0x6C6, 0x6C8, 0x6C8,     0, 0x6CB, 0x6CB,
/*FBE*/   0x6C5, 0x6C5, 0x6C9, 0x6C9, 0x6D0, 0x6D0, 0x6D0, 0x6D0,     0,     0,     0,     0,     0,     0,     0,     0,
/*FBF*/       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0, 0x6CC, 0x6CC, 0x6CC, 0x6CC
};

ساكن ثابت char16_t convertFEto06[] =
{
/***********0******1******2******3******4******5******6******7******8******9******A******B******C******D******E******F***/
/*FE7*/   0x64B, 0x64B, 0x64C, 0x64C, 0x64D, 0x64D, 0x64E, 0x64E, 0x64F, 0x64F, 0x650, 0x650, 0x651, 0x651, 0x652, 0x652,
/*FE8*/   0x621, 0x622, 0x622, 0x623, 0x623, 0x624, 0x624, 0x625, 0x625, 0x626, 0x626, 0x626, 0x626, 0x627, 0x627, 0x628,
/*FE9*/   0x628, 0x628, 0x628, 0x629, 0x629, 0x62A, 0x62A, 0x62A, 0x62A, 0x62B, 0x62B, 0x62B, 0x62B, 0x62C, 0x62C, 0x62C,
/*FEA*/   0x62C, 0x62D, 0x62D, 0x62D, 0x62D, 0x62E, 0x62E, 0x62E, 0x62E, 0x62F, 0x62F, 0x630, 0x630, 0x631, 0x631, 0x632,
/*FEB*/   0x632, 0x633, 0x633, 0x633, 0x633, 0x634, 0x634, 0x634, 0x634, 0x635, 0x635, 0x635, 0x635, 0x636, 0x636, 0x636,
/*FEC*/   0x636, 0x637, 0x637, 0x637, 0x637, 0x638, 0x638, 0x638, 0x638, 0x639, 0x639, 0x639, 0x639, 0x63A, 0x63A, 0x63A,
/*FED*/   0x63A, 0x641, 0x641, 0x641, 0x641, 0x642, 0x642, 0x642, 0x642, 0x643, 0x643, 0x643, 0x643, 0x644, 0x644, 0x644,
/*FEE*/   0x644, 0x645, 0x645, 0x645, 0x645, 0x646, 0x646, 0x646, 0x646, 0x647, 0x647, 0x647, 0x647, 0x648, 0x648, 0x649,
/*FEF*/   0x649, 0x64A, 0x64A, 0x64A, 0x64A, 0x65C, 0x65C, 0x65D, 0x65D, 0x65E, 0x65E, 0x65F, 0x65F
};

ساكن ثابت uint8_t shapeTable[4][4][4]=
{
  { {0,0,0,0}, {0,0,0,0}, {0,1,0,3}, {0,1,0,1} },
  { {0,0,2,2}, {0,0,1,2}, {0,1,1,2}, {0,1,1,3} },
  { {0,0,0,0}, {0,0,0,0}, {0,1,0,3}, {0,1,0,3} },
  { {0,0,1,2}, {0,0,1,2}, {0,1,1,2}, {0,1,1,3} }
};

/*
 * This function shapes European digits to Arabic-Indic digits
 * in-place, writing over the input characters.
 * Since we know that we are only looking for BMP code points,
 * we can safely just work with code units (again, at least UTF-16).
 */
ساكن فراغ
_shapeToArabicDigitsWithContext(char16_t *s, int32_t length,
                                char16_t digitBase,
                                UBool isLogical, UBool lastStrongWasAL) {
    int32_t i;
    char16_t c;

    digitBase-=0x30;

    /* the iteration direction depends on the type of input */
    𐡄𐡍(isLogical) {
        لكل(i=0; i<length; ++i) {
            c=s[i];
            بدل(ubidi_getClass(c)) {
            حالة U_LEFT_TO_RIGHT: /* L */
            حالة U_RIGHT_TO_LEFT: /* R */
                lastStrongWasAL=خطأ;
                اكسر;
            حالة U_RIGHT_TO_LEFT_ARABIC: /* AL */
                lastStrongWasAL=𐡒𐡔𐡈;
                اكسر;
            حالة U_EUROPEAN_NUMBER: /* EN */
                𐡄𐡍 (lastStrongWasAL && static_cast<uint32_t>(c - 0x30) < 10) {
                    s[i] = static_cast<char16_t>(digitBase + c); /* digitBase+(c-0x30) - digitBase was modified above */
                }
                اكسر;
            افتراضي :
                اكسر;
            }
        }
    } وإلا {
        لكل(i=length; i>0; /* pre-decrement in the body */) {
            c=s[--i];
            بدل(ubidi_getClass(c)) {
            حالة U_LEFT_TO_RIGHT: /* L */
            حالة U_RIGHT_TO_LEFT: /* R */
                lastStrongWasAL=خطأ;
                اكسر;
            حالة U_RIGHT_TO_LEFT_ARABIC: /* AL */
                lastStrongWasAL=𐡒𐡔𐡈;
                اكسر;
            حالة U_EUROPEAN_NUMBER: /* EN */
                𐡄𐡍 (lastStrongWasAL && static_cast<uint32_t>(c - 0x30) < 10) {
                    s[i] = static_cast<char16_t>(digitBase + c); /* digitBase+(c-0x30) - digitBase was modified above */
                }
                اكسر;
            افتراضي :
                اكسر;
            }
        }
    }
}

/*
 *Name     : invertBuffer
 *Function : This function inverts the buffer, it's used
 *           in case the user specifies the buffer to be
 *           U_SHAPE_TEXT_DIRECTION_LOGICAL
 */
ساكن فراغ
invertBuffer(char16_t *buffer, int32_t size, uint32_t /*options*/, int32_t lowlimit, int32_t highlimit) {
    char16_t temp;
    int32_t i=0,j=0;
    لكل(i=lowlimit,j=size-highlimit-1;i<j;i++,j--) {
        temp = buffer[i];
        buffer[i] = buffer[j];
        buffer[j] = temp;
    }
}

/*
 *Name     : changeLamAlef
 *Function : Converts the Alef characters into an equivalent
 *           LamAlef location in the 0x06xx Range, this is an
 *           intermediate stage in the operation of the program
 *           later it'll be converted into the 0xFExx LamAlefs
 *           in the shaping function.
 */
ساكن مضمن char16_t
changeLamAlef(char16_t ch) {
    بدل(ch) {
    حالة 0x0622 :
        أرجع 0x065C;
    حالة 0x0623 :
        أرجع 0x065D;
    حالة 0x0625 :
        أرجع 0x065E;
    حالة 0x0627 :
        أرجع 0x065F;
    }
    أرجع 0;
}

/*
 *Name     : getLink
 *Function : Resolves the link between the characters as
 *           Arabic characters have four forms :
 *           Isolated, Initial, Middle and Final Form
 */
ساكن char16_t
getLink(char16_t ch) {
    𐡄𐡍(ch >= 0x0622 && ch <= 0x06D3) {
        أرجع(araLink[ch-0x0622]);
    } وإلا 𐡄𐡍(ch == 0x200D) {
        أرجع(3);
    } وإلا 𐡄𐡍(ch >= 0x206D && ch <= 0x206F) {
        أرجع(4);
    }وإلا 𐡄𐡍(ch >= 0xFB50 && ch <= 0xFC62) {
        أرجع(presALink[ch-0xFB50]);
    } وإلا 𐡄𐡍(ch >= 0xFE70 && ch <= 0xFEFC) {
        أرجع(presBLink[ch-0xFE70]);
    }وإلا {
        أرجع(0);
    }
}

/*
 *Name     : countSpaces
 *Function : Counts the number of spaces
 *           at each end of the logical buffer
 */
ساكن فراغ
countSpaces(char16_t *dest, int32_t size, uint32_t /*options*/, int32_t *spacesCountl, int32_t *spacesCountr) {
    int32_t i = 0;
    int32_t countl = 0,countr = 0;
    طالما((dest[i] == SPACE_CHAR) && (countl < size)) {
       countl++;
       i++;
    }
    𐡄𐡍 (countl < size) {  /* the entire buffer is not all space */
        طالما(dest[size-1] == SPACE_CHAR) {
            countr++;
            size--;
        }
    }
    *spacesCountl = countl;
    *spacesCountr = countr;
}

/*
 *Name     : isTashkeelChar
 *Function : Returns 1 for Tashkeel characters in 06 range else return 0
 */
ساكن مضمن int32_t
isTashkeelChar(char16_t ch) {
    أرجع static_cast<int32_t>(ch >= 0x064B && ch <= 0x0652);
}

/*
 *Name     : isTashkeelCharFE
 *Function : Returns 1 for Tashkeel characters in FE range else return 0
 */
ساكن مضمن int32_t
isTashkeelCharFE(char16_t ch) {
    أرجع static_cast<int32_t>(ch >= 0xFE70 && ch <= 0xFE7F);
}

/*
 *Name     : isAlefChar
 *Function : Returns 1 for Alef characters else return 0
 */
ساكن مضمن int32_t
isAlefChar(char16_t ch) {
    أرجع static_cast<int32_t>(ch == 0x0622 || ch == 0x0623 || ch == 0x0625 || ch == 0x0627);
}

/*
 *Name     : isLamAlefChar
 *Function : Returns 1 for LamAlef characters else return 0
 */
ساكن مضمن int32_t
isLamAlefChar(char16_t ch) {
    أرجع static_cast<int32_t>(ch >= 0xFEF5 && ch <= 0xFEFC);
}

/*BIDI
 *Name     : isTailChar
 *Function : returns 1 if the character matches one of the tail characters (0xfe73 or 0x200b) otherwise returns 0
 */

ساكن مضمن int32_t
isTailChar(char16_t ch) {
    𐡄𐡍(ch == OLD_TAIL_CHAR || ch == NEW_TAIL_CHAR){
            أرجع 1;
    }وإلا{
            أرجع 0;
    }
}

/*BIDI
 *Name     : isSeenTailFamilyChar
 *Function : returns 1 if the character is a seen family isolated character
 *           in the FE range otherwise returns 0
 */

ساكن مضمن int32_t
isSeenTailFamilyChar(char16_t ch) {
    𐡄𐡍(ch >= 0xfeb1 && ch < 0xfebf){
            أرجع tailFamilyIsolatedFinal [ch - 0xFEB1];
    }وإلا{
            أرجع 0;
    }
}

 /* Name     : isSeenFamilyChar
  * Function : returns 1 if the character is a seen family character in the Unicode
  *            06 range otherwise returns 0
 */

ساكن مضمن int32_t
isSeenFamilyChar(char16_t  ch){
    𐡄𐡍(ch >= 0x633 && ch <= 0x636){
        أرجع 1;
    }وإلا {
        أرجع 0;
    }
}

/*Start of BIDI*/
/*
 *Name     : isAlefMaksouraChar
 *Function : returns 1 if the character is a Alef Maksoura Final or isolated
 *           otherwise returns 0
 */
ساكن مضمن int32_t
isAlefMaksouraChar(char16_t ch) {
    أرجع static_cast<int32_t>(ch == 0xFEEF || ch == 0xFEF0 || ch == 0x0649);
}

/*
 * Name     : isYehHamzaChar
 * Function : returns 1 if the character is a yehHamza isolated or yehhamza
 *            final is found otherwise returns 0
 */
ساكن مضمن int32_t
isYehHamzaChar(char16_t ch) {
    𐡄𐡍((ch==0xFE89)||(ch==0xFE8A)){
        أرجع 1;
    }وإلا{
        أرجع 0;
    }
}

 /*
 * Name: isTashkeelOnTatweelChar
 * Function: Checks if the Tashkeel Character is on Tatweel or not,if the
 *           Tashkeel on tatweel (FE range), it returns 1 else if the
 *           Tashkeel with shadda on tatweel (FC range)return 2 otherwise
 *           returns 0
 */
ساكن مضمن int32_t
isTashkeelOnTatweelChar(char16_t ch){
    𐡄𐡍(ch >= 0xfe70 && ch <= 0xfe7f && ch != NEW_TAIL_CHAR && ch != 0xFE75 && ch != SHADDA_TATWEEL_CHAR)
    {
        أرجع tashkeelMedial [ch - 0xFE70];
    }وإلا 𐡄𐡍( (ch >= 0xfcf2 && ch <= 0xfcf4) || (ch == SHADDA_TATWEEL_CHAR)) {
        أرجع 2;
    }وإلا{
        أرجع 0;
    }
}

/*
 * Name: isIsolatedTashkeelChar
 * Function: Checks if the Tashkeel Character is in the isolated form
 *           (i.e. Unicode FE range) returns 1 else if the Tashkeel
 *           with shadda is in the isolated form (i.e. Unicode FC range)
 *           returns 2 otherwise returns 0
 */
ساكن مضمن int32_t
isIsolatedTashkeelChar(char16_t ch){
    𐡄𐡍(ch >= 0xfe70 && ch <= 0xfe7f && ch != NEW_TAIL_CHAR && ch != 0xFE75){
        أرجع (1 - tashkeelMedial [ch - 0xFE70]);
    }وإلا 𐡄𐡍(ch >= 0xfc5e && ch <= 0xfc63){
        أرجع 1;
    }وإلا{
        أرجع 0;
    }
}




/*
 *Name     : calculateSize
 *Function : This function calculates the destSize to be used in preflighting
 *           when the destSize is equal to 0
 *           It is used also to calculate the new destsize in case the
 *           destination buffer will be resized.
 */

ساكن int32_t
calculateSize(ثابت char16_t *source, int32_t sourceLength,
int32_t destSize,uint32_t options) {
    int32_t i = 0;

    صحيح lamAlefOption = 0;
    صحيح tashkeelOption = 0;

    destSize = sourceLength;

    𐡄𐡍 (((options&U_SHAPE_LETTERS_MASK) == U_SHAPE_LETTERS_SHAPE ||
        ((options&U_SHAPE_LETTERS_MASK) == U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED )) &&
        ((options&U_SHAPE_LAMALEF_MASK) == U_SHAPE_LAMALEF_RESIZE )){
            lamAlefOption = 1;
    }
    𐡄𐡍((options&U_SHAPE_LETTERS_MASK) == U_SHAPE_LETTERS_SHAPE &&
       ((options&U_SHAPE_TASHKEEL_MASK) == U_SHAPE_TASHKEEL_RESIZE ) ){
            tashkeelOption = 1;
        }

    𐡄𐡍(lamAlefOption || tashkeelOption){
        𐡄𐡍((options&U_SHAPE_TEXT_DIRECTION_MASK)==U_SHAPE_TEXT_DIRECTION_VISUAL_LTR) {
            لكل(i=0;i<sourceLength;i++) {
                𐡄𐡍( ((isAlefChar(source[i]))&& (i<(sourceLength-1)) &&(source[i+1] == LAM_CHAR)) || (isTashkeelCharFE(source[i])) ) {
                        destSize--;
                    }
                }
            }وإلا 𐡄𐡍((options&U_SHAPE_TEXT_DIRECTION_MASK)==U_SHAPE_TEXT_DIRECTION_LOGICAL) {
                لكل(i=0;i<sourceLength;i++) {
                    𐡄𐡍( ( (source[i] == LAM_CHAR) && (i<(sourceLength-1)) && (isAlefChar(source[i+1]))) || (isTashkeelCharFE(source[i])) ) {
                        destSize--;
                    }
                }
            }
        }

    𐡄𐡍 ((options&U_SHAPE_LETTERS_MASK) == U_SHAPE_LETTERS_UNSHAPE){
        𐡄𐡍 ( (options&U_SHAPE_LAMALEF_MASK) == U_SHAPE_LAMALEF_RESIZE){
            لكل(i=0;i<sourceLength;i++) {
                𐡄𐡍(isLamAlefChar(source[i]))
                destSize++;
            }
        }
    }

    أرجع destSize;
}

/*
 *Name     : handleTashkeelWithTatweel
 *Function : Replaces Tashkeel as following:
 *            Case 1 :if the Tashkeel on tatweel, replace it with Tatweel.
 *            Case 2 :if the Tashkeel aggregated with Shadda on Tatweel, replace
 *                   it with Shadda on Tatweel.
 *            Case 3: if the Tashkeel is isolated replace it with Space.
 *
 */
ساكن int32_t
handleTashkeelWithTatweel(char16_t *dest, int32_t sourceLength,
             int32_t /*destSize*/, uint32_t /*options*/,
             UErrorCode * /*pErrorCode*/) {
                 صحيح i;
                 لكل(i = 0; i < sourceLength; i++){
                     𐡄𐡍((isTashkeelOnTatweelChar(dest[i]) == 1)){
                         dest[i] = TATWEEL_CHAR;
                    }وإلا 𐡄𐡍((isTashkeelOnTatweelChar(dest[i]) == 2)){
                         dest[i] = SHADDA_TATWEEL_CHAR;
                    }وإلا 𐡄𐡍(isIsolatedTashkeelChar(dest[i]) && dest[i] != SHADDA_CHAR){
                         dest[i] = SPACE_CHAR;
                    }
                 }
                 أرجع sourceLength;
}



/*
 *Name     : handleGeneratedSpaces
 *Function : The shapeUnicode function converts Lam + Alef into LamAlef + space,
 *           and Tashkeel to space.
 *           handleGeneratedSpaces function puts these generated spaces
 *           according to the options the user specifies. LamAlef and Tashkeel
 *           spaces can be replaced at begin, at end, at near or decrease the
 *           buffer size.
 *
 *           There is also Auto option for LamAlef and tashkeel, which will put
 *           the spaces at end of the buffer (or end of text if the user used
 *           the option U_SHAPE_SPACES_RELATIVE_TO_TEXT_BEGIN_END).
 *
 *           If the text type was visual_LTR and the option
 *           U_SHAPE_SPACES_RELATIVE_TO_TEXT_BEGIN_END was selected the END
 *           option will place the space at the beginning of the buffer and
 *           BEGIN will place the space at the end of the buffer.
 */

ساكن int32_t
handleGeneratedSpaces(char16_t *dest, int32_t sourceLength,
                    int32_t destSize,
                    uint32_t options,
                    UErrorCode *pErrorCode,بنية uShapeVariables shapeVars ) {

    int32_t i = 0, j = 0;
    int32_t count = 0;
    char16_t *tempbuffer=nullptr;

    صحيح lamAlefOption = 0;
    صحيح tashkeelOption = 0;
    صحيح shapingMode = SHAPE_MODE;

    𐡄𐡍 (shapingMode == 0){
        𐡄𐡍 ( (options&U_SHAPE_LAMALEF_MASK) == U_SHAPE_LAMALEF_RESIZE ){
            lamAlefOption = 1;
        }
        𐡄𐡍 ( (options&U_SHAPE_TASHKEEL_MASK) == U_SHAPE_TASHKEEL_RESIZE ){
            tashkeelOption = 1;
        }
    }

    𐡄𐡍 (static_cast<size_t>(sourceLength) + 1 > std::numeric_limits<size_t>::max() / U_SIZEOF_UCHAR) {
        *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }
    tempbuffer = static_cast<char16_t*>(uprv_malloc((sourceLength + 1) * U_SIZEOF_UCHAR));
    /* Test for nullptr */
    𐡄𐡍(tempbuffer == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        أرجع 0;
    }


    𐡄𐡍 (lamAlefOption || tashkeelOption){
        uprv_memset(tempbuffer, 0, (sourceLength+1)*U_SIZEOF_UCHAR);

        i = j = 0; count = 0;
        طالما(i < sourceLength) {
            𐡄𐡍 ( (lamAlefOption && dest[i] == LAMALEF_SPACE_SUB) ||
               (tashkeelOption && dest[i] == TASHKEEL_SPACE_SUB) ){
                j--;
                count++;
            } وإلا {
                tempbuffer[j] = dest[i];
            }
            i++;
            j++;
        }

        طالما(count >= 0) {
            tempbuffer[i] = 0x0000;
            i--;
            count--;
        }

        u_memcpy(dest, tempbuffer, sourceLength);
        destSize = u_strlen(dest);
    }

      lamAlefOption = 0;

    𐡄𐡍 (shapingMode == 0){
        𐡄𐡍 ( (options&U_SHAPE_LAMALEF_MASK) == U_SHAPE_LAMALEF_NEAR ){
            lamAlefOption = 1;
        }
    }

    𐡄𐡍 (lamAlefOption){
        /* Lam+Alef is already shaped into LamAlef + FFFF */
        i = 0;
        طالما(i < sourceLength) {
            𐡄𐡍(lamAlefOption&&dest[i] == LAMALEF_SPACE_SUB){
                dest[i] = SPACE_CHAR;
            }
            i++;
        }
        destSize = sourceLength;
    }
    lamAlefOption = 0;
    tashkeelOption = 0;

    𐡄𐡍 (shapingMode == 0) {
        𐡄𐡍 ( ((options&U_SHAPE_LAMALEF_MASK) == shapeVars.uShapeLamalefBegin) ||
              (((options&U_SHAPE_LAMALEF_MASK) == U_SHAPE_LAMALEF_AUTO )
              && (shapeVars.spacesRelativeToTextBeginEnd==1)) ) {
            lamAlefOption = 1;
        }
        𐡄𐡍 ( (options&U_SHAPE_TASHKEEL_MASK) == shapeVars.uShapeTashkeelBegin ) {
            tashkeelOption = 1;
        }
    }

    𐡄𐡍(lamAlefOption || tashkeelOption){
        uprv_memset(tempbuffer, 0, (sourceLength+1)*U_SIZEOF_UCHAR);
        
        i = j = sourceLength; count = 0;
        
        طالما(i >= 0) {
            𐡄𐡍 ( (lamAlefOption && dest[i] == LAMALEF_SPACE_SUB) ||
                 (tashkeelOption && dest[i] == TASHKEEL_SPACE_SUB) ){
                j++;
                count++;
            }وإلا {
                tempbuffer[j] = dest[i];
            }
            i--;
            j--;
        }

        لكل(i=0 ;i < count; i++){
                tempbuffer[i] = SPACE_CHAR;
        }

        u_memcpy(dest, tempbuffer, sourceLength);
        destSize = sourceLength;
    }



    lamAlefOption = 0;
    tashkeelOption = 0;

    𐡄𐡍 (shapingMode == 0) {
        𐡄𐡍 ( ((options&U_SHAPE_LAMALEF_MASK) == shapeVars.uShapeLamalefEnd) ||
              (((options&U_SHAPE_LAMALEF_MASK) == U_SHAPE_LAMALEF_AUTO )
              && (shapeVars.spacesRelativeToTextBeginEnd==0)) ) {
            lamAlefOption = 1;
        }
        𐡄𐡍 ( (options&U_SHAPE_TASHKEEL_MASK) == shapeVars.uShapeTashkeelEnd ){
            tashkeelOption = 1;
        }
    }

    𐡄𐡍(lamAlefOption || tashkeelOption){
        uprv_memset(tempbuffer, 0, (sourceLength+1)*U_SIZEOF_UCHAR);

        i = j = 0; count = 0;
        طالما(i < sourceLength) {
            𐡄𐡍 ( (lamAlefOption && dest[i] == LAMALEF_SPACE_SUB) ||
                 (tashkeelOption && dest[i] == TASHKEEL_SPACE_SUB) ){
                j--;
                count++;
            }وإلا {
                tempbuffer[j] = dest[i];
            }
            i++;
            j++;
        }

        طالما(count >= 0) {
            tempbuffer[i] = SPACE_CHAR;
            i--;
            count--;
        }

        u_memcpy(dest, tempbuffer, sourceLength);
        destSize = sourceLength;
    }


    𐡄𐡍(tempbuffer){
        uprv_free(tempbuffer);
    }

    أرجع destSize;
}

/*
 *Name     :expandCompositCharAtBegin
 *Function :Expands the LamAlef character to Lam and Alef consuming the required
 *         space from beginning of the buffer. If the text type was visual_LTR
 *         and the option U_SHAPE_SPACES_RELATIVE_TO_TEXT_BEGIN_END was selected
 *         the spaces will be located at end of buffer.
 *         If there are no spaces to expand the LamAlef, an error
 *         will be set to U_NO_SPACE_AVAILABLE as defined in utypes.h
 */

ساكن int32_t
expandCompositCharAtBegin(char16_t *dest, int32_t sourceLength, int32_t destSize,UErrorCode *pErrorCode) {
    int32_t      i = 0,j = 0;
    int32_t      countl = 0;
    char16_t *tempbuffer=nullptr;

    tempbuffer = static_cast<char16_t*>(uprv_malloc((sourceLength + 1) * U_SIZEOF_UCHAR));

    /* Test for nullptr */
    𐡄𐡍(tempbuffer == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        أرجع 0;
    }

        uprv_memset(tempbuffer, 0, (sourceLength+1)*U_SIZEOF_UCHAR);

        i = 0;
        طالما(dest[i] == SPACE_CHAR) {
            countl++;
            i++;
        }

        i = j = sourceLength-1;

        طالما(i >= 0 && j >= 0) {
            𐡄𐡍( countl>0 && isLamAlefChar(dest[i])) {
                tempbuffer[j] = LAM_CHAR;
                /* to ensure the array index is within the range */
                U_ASSERT(dest[i] >= 0xFEF5u
                    && dest[i]-0xFEF5u < UPRV_LENGTHOF(convertLamAlef));
                tempbuffer[j-1] = convertLamAlef[ dest[i] - 0xFEF5 ];
                j--;
                countl--;
            }وإلا {
                 𐡄𐡍( countl == 0 && isLamAlefChar(dest[i]) ) {
                     *pErrorCode=U_NO_SPACE_AVAILABLE;
                     }
                 tempbuffer[j] = dest[i];
            }
            i--;
            j--;
        }
        u_memcpy(dest, tempbuffer, sourceLength);

        uprv_free(tempbuffer);

        destSize = sourceLength;
        أرجع destSize;
}

/*
 *Name     : expandCompositCharAtEnd
 *Function : Expands the LamAlef character to Lam and Alef consuming the
 *           required space from end of the buffer. If the text type was
 *           Visual LTR and the option U_SHAPE_SPACES_RELATIVE_TO_TEXT_BEGIN_END
 *           was used, the spaces will be consumed from begin of buffer. If
 *           there are no spaces to expand the LamAlef, an error
 *           will be set to U_NO_SPACE_AVAILABLE as defined in utypes.h
 */

ساكن int32_t
expandCompositCharAtEnd(char16_t *dest, int32_t sourceLength, int32_t destSize,UErrorCode *pErrorCode) {
    int32_t      i = 0,j = 0;

    int32_t      countr = 0;
    int32_t  inpsize = sourceLength;

    char16_t *tempbuffer=nullptr;
    tempbuffer = static_cast<char16_t*>(uprv_malloc((sourceLength + 1) * U_SIZEOF_UCHAR));

    /* Test for nullptr */
    𐡄𐡍(tempbuffer == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
         أرجع 0;
    }

    uprv_memset(tempbuffer, 0, (sourceLength+1)*U_SIZEOF_UCHAR);

    طالما(inpsize>0 && dest[inpsize-1] == SPACE_CHAR) {
        countr++;
        inpsize--;
    }

    i = sourceLength - countr - 1;
    j = sourceLength - 1;

    طالما(i >= 0 && j >= 0) {
        𐡄𐡍( countr>0 && isLamAlefChar(dest[i]) ) {
            tempbuffer[j] = LAM_CHAR;
            tempbuffer[j-1] = convertLamAlef[ dest[i] - 0xFEF5 ];
            j--;
            countr--;
        }وإلا {
            𐡄𐡍 ((countr == 0) && isLamAlefChar(dest[i]) ) {
                *pErrorCode=U_NO_SPACE_AVAILABLE;
            }
            tempbuffer[j] = dest[i];
        }
        i--;
        j--;
    }

    𐡄𐡍(countr > 0) {
        u_memmove(tempbuffer, tempbuffer+countr, sourceLength);
        𐡄𐡍(u_strlen(tempbuffer) < sourceLength) {
            لكل(i=sourceLength-1;i>=sourceLength-countr;i--) {
                tempbuffer[i] = SPACE_CHAR;
            }
        }
    }
    u_memcpy(dest, tempbuffer, sourceLength);

    uprv_free(tempbuffer);

    destSize = sourceLength;
    أرجع destSize;
}

/*
 *Name     : expandCompositCharAtNear
 *Function : Expands the LamAlef character into Lam + Alef, YehHamza character
 *           into Yeh + Hamza, SeenFamily character into SeenFamily character
 *           + Tail, while consuming the space next to the character.
 *           If there are no spaces next to the character, an error
 *           will be set to U_NO_SPACE_AVAILABLE as defined in utypes.h
 */

ساكن int32_t
expandCompositCharAtNear(char16_t *dest, int32_t sourceLength, int32_t destSize,UErrorCode *pErrorCode,
                         صحيح yehHamzaOption, صحيح seenTailOption, صحيح lamAlefOption, بنية uShapeVariables shapeVars) {
    int32_t      i = 0;


    char16_t lamalefChar, yehhamzaChar;

    لكل(i = 0 ;i<=sourceLength-1;i++) {
            𐡄𐡍 (seenTailOption && isSeenTailFamilyChar(dest[i])) {
                𐡄𐡍 ((i>0) && (dest[i-1] == SPACE_CHAR) ) {
                    dest[i-1] = shapeVars.tailChar;
                }وإلا {
                    *pErrorCode=U_NO_SPACE_AVAILABLE;
                }
            }وإلا 𐡄𐡍(yehHamzaOption && (isYehHamzaChar(dest[i])) ) {
                𐡄𐡍 ((i>0) && (dest[i-1] == SPACE_CHAR) ) {
                    yehhamzaChar = dest[i];
                    dest[i] = yehHamzaToYeh[yehhamzaChar - YEH_HAMZAFE_CHAR];
                    dest[i-1] = HAMZAFE_CHAR;
                }وإلا {

                    *pErrorCode=U_NO_SPACE_AVAILABLE;
                }
            }وإلا 𐡄𐡍(lamAlefOption && isLamAlefChar(dest[i+1])) {
                𐡄𐡍(dest[i] == SPACE_CHAR){
                    lamalefChar = dest[i+1];
                    dest[i+1] = LAM_CHAR;
                    dest[i] = convertLamAlef[ lamalefChar - 0xFEF5 ];
                }وإلا {
                    *pErrorCode=U_NO_SPACE_AVAILABLE;
                }
            }
       }
       destSize = sourceLength;
       أرجع destSize;
}
 /*
 * Name     : expandCompositChar
 * Function : LamAlef, need special handling, since it expands from one
 *            character into two characters while shaping or deshaping.
 *            In order to expand it, near or far spaces according to the
 *            options user specifies. Also buffer size can be increased.
 *
 *            For SeenFamily characters and YehHamza only the near option is
 *            supported, while for LamAlef we can take spaces from begin, end,
 *            near or even increase the buffer size.
 *            There is also the Auto option for LamAlef only, which will first
 *            search for a space at end, begin then near, respectively.
 *            If there are no spaces to expand these characters, an error will be set to
 *            U_NO_SPACE_AVAILABLE as defined in utypes.h
 */

ساكن int32_t
expandCompositChar(char16_t *dest, int32_t sourceLength,
              int32_t destSize,uint32_t options,
              UErrorCode *pErrorCode, صحيح shapingMode,بنية uShapeVariables shapeVars) {

    int32_t      i = 0,j = 0;

    char16_t *tempbuffer=nullptr;
    صحيح yehHamzaOption = 0;
    صحيح seenTailOption = 0;
    صحيح lamAlefOption = 0;

    𐡄𐡍 (shapingMode == 1){
        𐡄𐡍 ( (options&U_SHAPE_LAMALEF_MASK) == U_SHAPE_LAMALEF_AUTO){

            𐡄𐡍(shapeVars.spacesRelativeToTextBeginEnd == 0) {
                destSize = expandCompositCharAtEnd(dest, sourceLength, destSize, pErrorCode);

                𐡄𐡍(*pErrorCode == U_NO_SPACE_AVAILABLE) {
                    *pErrorCode = U_ZERO_ERROR;
                    destSize = expandCompositCharAtBegin(dest, sourceLength, destSize, pErrorCode);
                }
            }وإلا {
                destSize = expandCompositCharAtBegin(dest, sourceLength, destSize, pErrorCode);

                𐡄𐡍(*pErrorCode == U_NO_SPACE_AVAILABLE) {
                    *pErrorCode = U_ZERO_ERROR;
                    destSize = expandCompositCharAtEnd(dest, sourceLength, destSize, pErrorCode);
                }
            }

            𐡄𐡍(*pErrorCode == U_NO_SPACE_AVAILABLE) {
                *pErrorCode = U_ZERO_ERROR;
                destSize = expandCompositCharAtNear(dest, sourceLength, destSize, pErrorCode, yehHamzaOption,
                                                seenTailOption, 1,shapeVars);
            }
        }
    }

    𐡄𐡍 (shapingMode == 1){
        𐡄𐡍 ( (options&U_SHAPE_LAMALEF_MASK) == shapeVars.uShapeLamalefEnd){
            destSize = expandCompositCharAtEnd(dest, sourceLength, destSize, pErrorCode);
        }
    }

    𐡄𐡍 (shapingMode == 1){
        𐡄𐡍 ( (options&U_SHAPE_LAMALEF_MASK) == shapeVars.uShapeLamalefBegin){
            destSize = expandCompositCharAtBegin(dest, sourceLength, destSize, pErrorCode);
        }
    }

    𐡄𐡍 (shapingMode == 0){
         𐡄𐡍 ((options&U_SHAPE_YEHHAMZA_MASK) == U_SHAPE_YEHHAMZA_TWOCELL_NEAR){
             yehHamzaOption = 1;
         }
         𐡄𐡍 ((options&U_SHAPE_SEEN_MASK) == U_SHAPE_SEEN_TWOCELL_NEAR){
            seenTailOption = 1;
         }
    }
    𐡄𐡍 (shapingMode == 1) {
        𐡄𐡍 ( (options&U_SHAPE_LAMALEF_MASK) == U_SHAPE_LAMALEF_NEAR) {
            lamAlefOption = 1;
        }
    }


    𐡄𐡍 (yehHamzaOption || seenTailOption || lamAlefOption){
        destSize = expandCompositCharAtNear(dest, sourceLength, destSize, pErrorCode, yehHamzaOption,
                                            seenTailOption,lamAlefOption,shapeVars);
    }


    𐡄𐡍 (shapingMode == 1){
        𐡄𐡍 ( (options&U_SHAPE_LAMALEF_MASK) == U_SHAPE_LAMALEF_RESIZE){
            destSize = calculateSize(dest,sourceLength,destSize,options);
            tempbuffer = static_cast<char16_t*>(uprv_malloc((destSize + 1) * U_SIZEOF_UCHAR));

            /* Test for nullptr */
            𐡄𐡍(tempbuffer == nullptr) {
                *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
                أرجع 0;
            }

            uprv_memset(tempbuffer, 0, (destSize+1)*U_SIZEOF_UCHAR);

            i = j = 0;
            طالما(i < destSize && j < destSize) {
                𐡄𐡍(isLamAlefChar(dest[i]) ) {
                    tempbuffer[j] = convertLamAlef[ dest[i] - 0xFEF5 ];
                    tempbuffer[j+1] = LAM_CHAR;
                    j++;
                }وإلا {
                    tempbuffer[j] = dest[i];
                }
                i++;
                j++;
            }

            u_memcpy(dest, tempbuffer, destSize);
        }
    }

    𐡄𐡍(tempbuffer) {
        uprv_free(tempbuffer);
    }
    أرجع destSize;
}

/*
 *Name     : shapeUnicode
 *Function : Converts an Arabic Unicode buffer in 06xx Range into a shaped
 *           arabic Unicode buffer in FExx Range
 */
ساكن int32_t
shapeUnicode(char16_t *dest, int32_t sourceLength,
             int32_t destSize,uint32_t options,
             UErrorCode *pErrorCode,
             صحيح tashkeelFlag, بنية uShapeVariables shapeVars) {

    int32_t          i, iend;
    int32_t          step;
    int32_t          lastPos,Nx, Nw;
    unsigned صحيح     Shape;
    int32_t          lamalef_found = 0;
    int32_t seenfamFound = 0, yehhamzaFound =0, tashkeelFound  = 0;
    char16_t         prevLink = 0, lastLink = 0, currLink, nextLink = 0;
    char16_t         wLamalef;

    /*
     * Converts the input buffer from FExx Range into 06xx Range
     * to make sure that all characters are in the 06xx range
     * even the lamalef is converted to the special region in
     * the 06xx range
     */
    𐡄𐡍 ((options & U_SHAPE_PRESERVE_PRESENTATION_MASK)  == U_SHAPE_PRESERVE_PRESENTATION_NOOP) {
        لكل (i = 0; i < sourceLength; i++) {
            char16_t inputChar  = dest[i];
            𐡄𐡍 ( (inputChar >= 0xFB50) && (inputChar <= 0xFBFF)) {
                char16_t c = convertFBto06 [ (inputChar - 0xFB50) ];
                𐡄𐡍 (c != 0)
                    dest[i] = c;
            } وإلا 𐡄𐡍 ( (inputChar >= 0xFE70) && (inputChar <= 0xFEFC)) {
                dest[i] = convertFEto06 [ (inputChar - 0xFE70) ] ;
            } وإلا {
                dest[i] = inputChar ;
            }
        }
    }


    /* sets the index to the end of the buffer, together with the step point to -1 */
    i = sourceLength - 1;
    iend = -1;
    step = -1;

    /*
     * This function resolves the link between the characters .
     * Arabic characters have four forms :
     * Isolated Form, Initial Form, Middle Form and Final Form
     */
    currLink = getLink(dest[i]);

    lastPos = i;
    Nx = -2, Nw = 0;

    طالما (i != iend) {
        /* If high byte of currLink > 0 then more than one shape */
        𐡄𐡍 ((currLink & 0xFF00) > 0 || (getLink(dest[i]) & IRRELEVANT) != 0) {
            Nw = i + step;
            طالما (Nx < 0) {         /* we need to know about next char */
                𐡄𐡍(Nw == iend) {
                    nextLink = 0;
                    Nx = 3000;
                } وإلا {
                    nextLink = getLink(dest[Nw]);
                    𐡄𐡍((nextLink & IRRELEVANT) == 0) {
                        Nx = Nw;
                    } وإلا {
                        Nw = Nw + step;
                    }
                }
            }

            𐡄𐡍 ( ((currLink & ALEFTYPE) > 0)  &&  ((lastLink & LAMTYPE) > 0) ) {
                lamalef_found = 1;
                wLamalef = changeLamAlef(dest[i]); /*get from 0x065C-0x065f */
                𐡄𐡍 ( wLamalef != 0) {
                    dest[i] = LAMALEF_SPACE_SUB;            /* The default case is to drop the Alef and replace */
                    dest[lastPos] =wLamalef;     /* it by LAMALEF_SPACE_SUB which is the last character in the  */
                    i=lastPos;                   /* unicode private use area, this is done to make   */
                }                                /* sure that removeLamAlefSpaces() handles only the */
                lastLink = prevLink;             /* spaces generated during lamalef generation.      */
                currLink = getLink(wLamalef);    /* LAMALEF_SPACE_SUB is added here and is replaced by spaces   */
            }                                    /* in removeLamAlefSpaces()                         */

            𐡄𐡍 ((i > 0) && (dest[i-1] == SPACE_CHAR)){
                𐡄𐡍 ( isSeenFamilyChar(dest[i])) {
                    seenfamFound = 1;
                } وإلا 𐡄𐡍 (dest[i] == YEH_HAMZA_CHAR) {
                    yehhamzaFound = 1;
                }
            }
            وإلا 𐡄𐡍(i==0){
                𐡄𐡍 ( isSeenFamilyChar(dest[i])){
                    seenfamFound = 1;
                } وإلا 𐡄𐡍 (dest[i] == YEH_HAMZA_CHAR) {
                    yehhamzaFound = 1;
                }
            }

            /*
             * get the proper shape according to link ability of neighbors
             * and of character; depends on the order of the shapes
             * (isolated, initial, middle, final) in the compatibility area
             */
            Shape = shapeTable[nextLink & (LINKR + LINKL)]
                              [lastLink & (LINKR + LINKL)]
                              [currLink & (LINKR + LINKL)];

            𐡄𐡍 ((currLink & (LINKR+LINKL)) == 1) {
                Shape &= 1;
            } وإلا 𐡄𐡍(isTashkeelChar(dest[i])) {
                𐡄𐡍( (lastLink & LINKL) && (nextLink & LINKR) && (tashkeelFlag == 1) &&
                     dest[i] != 0x064C && dest[i] != 0x064D )
                {
                    Shape = 1;
                    𐡄𐡍( (nextLink&ALEFTYPE) == ALEFTYPE && (lastLink&LAMTYPE) == LAMTYPE ) {
                        Shape = 0;
                    }
                } وإلا 𐡄𐡍(tashkeelFlag == 2 && dest[i] == SHADDA06_CHAR){
                    Shape = 1;
                } وإلا {
                    Shape = 0;
                }
            }
            𐡄𐡍 ((dest[i] ^ 0x0600) < 0x100) {
                𐡄𐡍 ( isTashkeelChar(dest[i]) ){
                    𐡄𐡍 (tashkeelFlag == 2  && dest[i] != SHADDA06_CHAR){
                        dest[i] = TASHKEEL_SPACE_SUB;
                        tashkeelFound  = 1;
                    } وإلا {
                        /* to ensure the array index is within the range */
                        U_ASSERT(dest[i] >= 0x064Bu
                            && dest[i]-0x064Bu < UPRV_LENGTHOF(IrrelevantPos));
                        dest[i] =  0xFE70 + IrrelevantPos[(dest[i] - 0x064B)] + static_cast<char16_t>(Shape);
                    }
                }وإلا 𐡄𐡍 ((currLink & APRESENT) > 0) {
                    dest[i] = static_cast<char16_t>(0xFB50 + (currLink >> 8) + Shape);
                }وإلا 𐡄𐡍 ((currLink >> 8) > 0 && (currLink & IRRELEVANT) == 0) {
                    dest[i] = static_cast<char16_t>(0xFE70 + (currLink >> 8) + Shape);
                }
            }
        }

        /* move one notch forward */
        𐡄𐡍 ((currLink & IRRELEVANT) == 0) {
            prevLink = lastLink;
            lastLink = currLink;
            lastPos = i;
        }

        i = i + step;
        𐡄𐡍 (i == Nx) {
            currLink = nextLink;
            Nx = -2;
        } وإلا 𐡄𐡍(i != iend) {
            currLink = getLink(dest[i]);
        }
    }
    destSize = sourceLength;
    𐡄𐡍 ( (lamalef_found != 0 ) || (tashkeelFound  != 0) ){
        destSize = handleGeneratedSpaces(dest,sourceLength,destSize,options,pErrorCode, shapeVars);
    }

    𐡄𐡍 ( (seenfamFound != 0) || (yehhamzaFound != 0) ) {
        destSize = expandCompositChar(dest, sourceLength,destSize,options,pErrorCode, SHAPE_MODE,shapeVars);
    }
    أرجع destSize;
}

/*
 *Name     : deShapeUnicode
 *Function : Converts an Arabic Unicode buffer in FExx Range into unshaped
 *           arabic Unicode buffer in 06xx Range
 */
ساكن int32_t
deShapeUnicode(char16_t *dest, int32_t sourceLength,
               int32_t destSize,uint32_t options,
               UErrorCode *pErrorCode, بنية uShapeVariables shapeVars) {
    int32_t i = 0;
    int32_t lamalef_found = 0;
    int32_t yehHamzaComposeEnabled = 0;
    int32_t seenComposeEnabled = 0;

    yehHamzaComposeEnabled = ((options&U_SHAPE_YEHHAMZA_MASK) == U_SHAPE_YEHHAMZA_TWOCELL_NEAR) ? 1 : 0;
    seenComposeEnabled = ((options&U_SHAPE_SEEN_MASK) == U_SHAPE_SEEN_TWOCELL_NEAR)? 1 : 0;

    /*
     *This for loop changes the buffer from the Unicode FE range to
     *the Unicode 06 range
     */

    لكل(i = 0; i < sourceLength; i++) {
        char16_t  inputChar = dest[i];
        𐡄𐡍 ( (inputChar >= 0xFB50) && (inputChar <= 0xFBFF)) { /* FBxx Arabic range */
            char16_t c = convertFBto06 [ (inputChar - 0xFB50) ];
            𐡄𐡍 (c != 0)
                dest[i] = c;
        } وإلا 𐡄𐡍( (yehHamzaComposeEnabled == 1) && ((inputChar == HAMZA06_CHAR) || (inputChar == HAMZAFE_CHAR))
                && (i < (sourceLength - 1)) && isAlefMaksouraChar(dest[i+1] )) {
                 dest[i] = SPACE_CHAR;
                 dest[i+1] = YEH_HAMZA_CHAR;
        } وإلا 𐡄𐡍 ( (seenComposeEnabled == 1) && (isTailChar(inputChar)) && (i< (sourceLength - 1))
                        && (isSeenTailFamilyChar(dest[i+1])) ) {
                dest[i] = SPACE_CHAR;
        } وإلا 𐡄𐡍 (( inputChar >= 0xFE70) && (inputChar <= 0xFEF4 )) { /* FExx Arabic range */
                dest[i] = convertFEto06 [ (inputChar - 0xFE70) ];
        } وإلا {
            dest[i] = inputChar ;
        }

        𐡄𐡍( isLamAlefChar(dest[i]) )
            lamalef_found = 1;
    }

   destSize = sourceLength;
   𐡄𐡍 (lamalef_found != 0){
          destSize = expandCompositChar(dest,sourceLength,destSize,options,pErrorCode,DESHAPE_MODE, shapeVars);
   }
   أرجع destSize;
}

/*
 ****************************************
 * u_shapeArabic
 ****************************************
 */

U_CAPI int32_t U_EXPORT2
u_shapeArabic(ثابت char16_t *source, int32_t sourceLength,
              char16_t *dest, int32_t destCapacity,
              uint32_t options,
              UErrorCode *pErrorCode) {

    int32_t destLength;
    بنية  uShapeVariables shapeVars = { OLD_TAIL_CHAR,U_SHAPE_LAMALEF_BEGIN,U_SHAPE_LAMALEF_END,U_SHAPE_TASHKEEL_BEGIN,U_SHAPE_TASHKEEL_END,0};

    /* usual error checking */
    𐡄𐡍(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        أرجع 0;
    }

    /* make sure that no reserved options values are used; allow dest==nullptr only for preflighting */
    𐡄𐡍( source==nullptr || sourceLength<-1 || (dest==nullptr && destCapacity!=0) || destCapacity<0 ||
                (((options&U_SHAPE_TASHKEEL_MASK) > 0) &&
                 ((options&U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED) == U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED) ) ||
                (((options&U_SHAPE_TASHKEEL_MASK) > 0) &&
                 ((options&U_SHAPE_LETTERS_MASK) == U_SHAPE_LETTERS_UNSHAPE)) ||
                (options&U_SHAPE_DIGIT_TYPE_RESERVED)==U_SHAPE_DIGIT_TYPE_RESERVED ||
                (options&U_SHAPE_DIGITS_MASK)==U_SHAPE_DIGITS_RESERVED ||
                ((options&U_SHAPE_LAMALEF_MASK) != U_SHAPE_LAMALEF_RESIZE  &&
                (options&U_SHAPE_AGGREGATE_TASHKEEL_MASK) != 0) ||
                ((options&U_SHAPE_AGGREGATE_TASHKEEL_MASK) == U_SHAPE_AGGREGATE_TASHKEEL &&
                (options&U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED) != U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED)
    )
    {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }
    /* Validate  lamalef options */
    𐡄𐡍(((options&U_SHAPE_LAMALEF_MASK) > 0)&&
              !(((options & U_SHAPE_LAMALEF_MASK)==U_SHAPE_LAMALEF_BEGIN) ||
                ((options & U_SHAPE_LAMALEF_MASK)==U_SHAPE_LAMALEF_END ) ||
                ((options & U_SHAPE_LAMALEF_MASK)==U_SHAPE_LAMALEF_RESIZE )||
                 ((options & U_SHAPE_LAMALEF_MASK)==U_SHAPE_LAMALEF_AUTO) ||
                 ((options & U_SHAPE_LAMALEF_MASK)==U_SHAPE_LAMALEF_NEAR)))
    {
         *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }
    /* Validate  Tashkeel options */
    𐡄𐡍(((options&U_SHAPE_TASHKEEL_MASK) > 0)&&
                   !(((options & U_SHAPE_TASHKEEL_MASK)==U_SHAPE_TASHKEEL_BEGIN) ||
                     ((options & U_SHAPE_TASHKEEL_MASK)==U_SHAPE_TASHKEEL_END )
                    ||((options & U_SHAPE_TASHKEEL_MASK)==U_SHAPE_TASHKEEL_RESIZE )||
                    ((options & U_SHAPE_TASHKEEL_MASK)==U_SHAPE_TASHKEEL_REPLACE_BY_TATWEEL)))
    {
         *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }
    /* determine the source length */
    𐡄𐡍(sourceLength==-1) {
        sourceLength=u_strlen(source);
    }
    𐡄𐡍(sourceLength<=0) {
        أرجع u_terminateUChars(dest, destCapacity, 0, pErrorCode);
    }

    /* check that source and destination do not overlap */
    𐡄𐡍( dest!=nullptr &&
        ((source<=dest && dest<source+sourceLength) ||
         (dest<=source && source<dest+destCapacity))) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }

    /* Does Options contain the new Seen Tail Unicode code point option */
    𐡄𐡍 ( (options&U_SHAPE_TAIL_TYPE_MASK) == U_SHAPE_TAIL_NEW_UNICODE){
        shapeVars.tailChar = NEW_TAIL_CHAR;
    }وإلا {
        shapeVars.tailChar = OLD_TAIL_CHAR;
    }

    𐡄𐡍((options&U_SHAPE_LETTERS_MASK)!=U_SHAPE_LETTERS_NOOP) {
        char16_t buffer[300];
        char16_t *tempbuffer, *tempsource = nullptr;
        int32_t outputSize, spacesCountl=0, spacesCountr=0;

        𐡄𐡍((options&U_SHAPE_AGGREGATE_TASHKEEL_MASK)>0) {
            int32_t logical_order = (options&U_SHAPE_TEXT_DIRECTION_MASK) == U_SHAPE_TEXT_DIRECTION_LOGICAL;
            int32_t aggregate_tashkeel =
                        (options&(U_SHAPE_AGGREGATE_TASHKEEL_MASK+U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED)) ==
                        (U_SHAPE_AGGREGATE_TASHKEEL+U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED);
            صحيح step=logical_order?1:-1;
            صحيح j=logical_order?-1:2*sourceLength;
            صحيح i=logical_order?-1:sourceLength;
            صحيح end=logical_order?sourceLength:-1;
            صحيح aggregation_possible = 1;
            char16_t prev = 0;
            char16_t prevLink, currLink = 0;
            صحيح newSourceLength = 0;
            tempsource = (char16_t *)uprv_malloc(2*sourceLength*U_SIZEOF_UCHAR);
            𐡄𐡍(tempsource == nullptr) {
                *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
                أرجع 0;
            }

            طالما ((i+=step) != end) {
                prevLink = currLink;
                currLink = getLink(source[i]);
                𐡄𐡍 (aggregate_tashkeel && ((prevLink|currLink)&COMBINE) == COMBINE && aggregation_possible) {
                    𐡄𐡍 (j<0 || j>=2*sourceLength) {
                        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
                        𐡄𐡍 (tempsource != nullptr) uprv_free(tempsource);
                        أرجع 0;
                    }
                    aggregation_possible = 0;
                    tempsource[j] = (prev<source[i]?prev:source[i])-0x064C+0xFC5E;
                    currLink = getLink(tempsource[j]);
                } وإلا {
                    aggregation_possible = 1;
                    tempsource[j+=step] = source[i];
                    prev = source[i];
                    newSourceLength++;
                }
            }
            source = tempsource+(logical_order?0:j);
            sourceLength = newSourceLength;
        }

        /* calculate destination size */
        /* TODO: do we ever need to do this pure preflighting? */
        𐡄𐡍(((options&U_SHAPE_LAMALEF_MASK)==U_SHAPE_LAMALEF_RESIZE) ||
           ((options&U_SHAPE_TASHKEEL_MASK)==U_SHAPE_TASHKEEL_RESIZE)) {
            outputSize=calculateSize(source,sourceLength,destCapacity,options);
        } وإلا {
            outputSize=sourceLength;
        }

        𐡄𐡍(outputSize>destCapacity) {
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
                𐡄𐡍 (tempsource != nullptr) uprv_free(tempsource);
            أرجع outputSize;
        }

        /*
         * need a temporary buffer of size max(outputSize, sourceLength)
         * because at first we copy source->temp
         */
        𐡄𐡍(sourceLength>outputSize) {
            outputSize=sourceLength;
        }

        /* Start of Arabic letter shaping part */
        𐡄𐡍(outputSize<=UPRV_LENGTHOF(buffer)) {
            outputSize=UPRV_LENGTHOF(buffer);
            tempbuffer=buffer;
        } وإلا {
            tempbuffer = (char16_t *)uprv_malloc(outputSize*U_SIZEOF_UCHAR);

            /*Test for nullptr*/
            𐡄𐡍(tempbuffer == nullptr) {
                *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
                𐡄𐡍 (tempsource != nullptr) uprv_free(tempsource);
                أرجع 0;
            }
        }
        u_memcpy(tempbuffer, source, sourceLength);
        𐡄𐡍 (tempsource != nullptr){
            uprv_free(tempsource);
        }

        𐡄𐡍(sourceLength<outputSize) {
            uprv_memset(tempbuffer+sourceLength, 0, (outputSize-sourceLength)*U_SIZEOF_UCHAR);
        }

        𐡄𐡍((options&U_SHAPE_TEXT_DIRECTION_MASK) == U_SHAPE_TEXT_DIRECTION_LOGICAL) {
            countSpaces(tempbuffer,sourceLength,options,&spacesCountl,&spacesCountr);
            invertBuffer(tempbuffer,sourceLength,options,spacesCountl,spacesCountr);
        }

        𐡄𐡍((options&U_SHAPE_TEXT_DIRECTION_MASK) == U_SHAPE_TEXT_DIRECTION_VISUAL_LTR) {
            𐡄𐡍((options&U_SHAPE_SPACES_RELATIVE_TO_TEXT_MASK) == U_SHAPE_SPACES_RELATIVE_TO_TEXT_BEGIN_END) {
                shapeVars.spacesRelativeToTextBeginEnd = 1;
                shapeVars.uShapeLamalefBegin = U_SHAPE_LAMALEF_END;
                shapeVars.uShapeLamalefEnd = U_SHAPE_LAMALEF_BEGIN;
                shapeVars.uShapeTashkeelBegin = U_SHAPE_TASHKEEL_END;
                shapeVars.uShapeTashkeelEnd = U_SHAPE_TASHKEEL_BEGIN;
            }
        }

        بدل(options&U_SHAPE_LETTERS_MASK) {
        حالة U_SHAPE_LETTERS_SHAPE :
             𐡄𐡍( (options&U_SHAPE_TASHKEEL_MASK)> 0
                 && ((options&U_SHAPE_TASHKEEL_MASK) !=U_SHAPE_TASHKEEL_REPLACE_BY_TATWEEL)) {
                /* Call the shaping function with tashkeel flag == 2 for removal of tashkeel */
                destLength = shapeUnicode(tempbuffer,sourceLength,destCapacity,options,pErrorCode,2,shapeVars);
             }وإلا {
                /* default Call the shaping function with tashkeel flag == 1 */
                destLength = shapeUnicode(tempbuffer,sourceLength,destCapacity,options,pErrorCode,1,shapeVars);

                /*After shaping text check if user wants to remove tashkeel and replace it with tatweel*/
                𐡄𐡍( (options&U_SHAPE_TASHKEEL_MASK) == U_SHAPE_TASHKEEL_REPLACE_BY_TATWEEL){
                  destLength = handleTashkeelWithTatweel(tempbuffer,destLength,destCapacity,options,pErrorCode);
                }
            }
            اكسر;
        حالة U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED :
            /* Call the shaping function with tashkeel flag == 0 */
            destLength = shapeUnicode(tempbuffer,sourceLength,destCapacity,options,pErrorCode,0,shapeVars);
            اكسر;

        حالة U_SHAPE_LETTERS_UNSHAPE :
            /* Call the deshaping function */
            destLength = deShapeUnicode(tempbuffer,sourceLength,destCapacity,options,pErrorCode,shapeVars);
            اكسر;
        افتراضي :
            /* will never occur because of validity checks above */
            destLength = 0;
            اكسر;
        }

        /*
         * TODO: (markus 2002aug01)
         * For as long as we always preflight the outputSize above
         * we should U_ASSERT(outputSize==destLength)
         * except for the adjustment above before the tempbuffer allocation
         */

        𐡄𐡍((options&U_SHAPE_TEXT_DIRECTION_MASK) == U_SHAPE_TEXT_DIRECTION_LOGICAL) {
            countSpaces(tempbuffer,destLength,options,&spacesCountl,&spacesCountr);
            invertBuffer(tempbuffer,destLength,options,spacesCountl,spacesCountr);
        }
        u_memcpy(dest, tempbuffer, uprv_min(destLength, destCapacity));

        𐡄𐡍(tempbuffer!=buffer) {
            uprv_free(tempbuffer);
        }

        𐡄𐡍(destLength>destCapacity) {
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
            أرجع destLength;
        }

        /* End of Arabic letter shaping part */
    } وإلا {
        /*
         * No letter shaping:
         * just make sure the destination is large enough and copy the string.
         */
        𐡄𐡍(destCapacity<sourceLength) {
            /* this catches preflighting, too */
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
            أرجع sourceLength;
        }
        u_memcpy(dest, source, sourceLength);
        destLength=sourceLength;
    }

    /*
     * Perform number shaping.
     * With UTF-16 or UTF-32, the length of the string is constant.
     * The easiest way to do this is to operate on the destination and
     * "shape" the digits in-place.
     */
    𐡄𐡍((options&U_SHAPE_DIGITS_MASK)!=U_SHAPE_DIGITS_NOOP) {
        char16_t digitBase;
        int32_t i;

        /* select the requested digit group */
        بدل(options&U_SHAPE_DIGIT_TYPE_MASK) {
        حالة U_SHAPE_DIGIT_TYPE_AN:
            digitBase=0x660; /* Unicode: "Arabic-Indic digits" */
            اكسر;
        حالة U_SHAPE_DIGIT_TYPE_AN_EXTENDED:
            digitBase=0x6f0; /* Unicode: "Eastern Arabic-Indic digits (Persian and Urdu)" */
            اكسر;
        افتراضي:
            /* will never occur because of validity checks above */
            digitBase=0;
            اكسر;
        }

        /* perform the requested operation */
        بدل(options&U_SHAPE_DIGITS_MASK) {
        حالة U_SHAPE_DIGITS_EN2AN:
            /* add (digitBase-'0') to each European (ASCII) digit code point */
            digitBase-=0x30;
            لكل(i=0; i<destLength; ++i) {
                𐡄𐡍(((uint32_t)dest[i]-0x30)<10) {
                    dest[i]+=digitBase;
                }
            }
            اكسر;
        حالة U_SHAPE_DIGITS_AN2EN:
            /* subtract (digitBase-'0') from each Arabic digit code point */
            لكل(i=0; i<destLength; ++i) {
                𐡄𐡍(((uint32_t)dest[i]-(uint32_t)digitBase)<10) {
                    dest[i]-=digitBase-0x30;
                }
            }
            اكسر;
        حالة U_SHAPE_DIGITS_ALEN2AN_INIT_LR:
            _shapeToArabicDigitsWithContext(dest, destLength,
                                            digitBase,
                                            (options & U_SHAPE_TEXT_DIRECTION_MASK) == U_SHAPE_TEXT_DIRECTION_LOGICAL,
                                            خطأ);
            اكسر;
        حالة U_SHAPE_DIGITS_ALEN2AN_INIT_AL:
            _shapeToArabicDigitsWithContext(dest, destLength,
                                            digitBase,
                                            (options & U_SHAPE_TEXT_DIRECTION_MASK) == U_SHAPE_TEXT_DIRECTION_LOGICAL,
                                            𐡒𐡔𐡈);
            اكسر;
        افتراضي:
            /* will never occur because of validity checks above */
            اكسر;
        }
    }

    أرجع u_terminateUChars(dest, destCapacity, destLength, pErrorCode);
}
