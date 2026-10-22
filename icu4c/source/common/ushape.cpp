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

#شامل "unicode/utypes.h"
#شامل "unicode/uchar.h"
#شامل "unicode/ustring.h"
#شامل "unicode/ushape.h"
#شامل "cmemory.h"
#شامل "putilimp.h"
#شامل "ustr_imp.h"
#شامل "ubidi_props.h"
#شامل "uassert.h"

#شامل <limits>
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

#تعریف_کرو IRRELEVANT 4
#تعریف_کرو LAMTYPE    16
#تعریف_کرو ALEFTYPE   32
#تعریف_کرو LINKR      1
#تعریف_کرو LINKL      2
#تعریف_کرو APRESENT   8
#تعریف_کرو SHADDA     64
#تعریف_کرو CSHADDA    128
#تعریف_کرو COMBINE    (SHADDA+CSHADDA)

#تعریف_کرو HAMZAFE_CHAR       0xfe80
#تعریف_کرو HAMZA06_CHAR       0x0621
#تعریف_کرو YEH_HAMZA_CHAR     0x0626
#تعریف_کرو YEH_HAMZAFE_CHAR   0xFE89
#تعریف_کرو LAMALEF_SPACE_SUB  0xFFFF
#تعریف_کرو TASHKEEL_SPACE_SUB 0xFFFE
#تعریف_کرو NEW_دم_CHAR      0xFE73
#تعریف_کرو OLD_دم_CHAR      0x200B
#تعریف_کرو LAM_CHAR           0x0644
#تعریف_کرو SPACE_CHAR         0x0020
#تعریف_کرو SHADDA_CHAR        0xFE7C
#تعریف_کرو TATWEEL_CHAR       0x0640
#تعریف_کرو SHADDA_TATWEEL_CHAR  0xFE7D
#تعریف_کرو SHADDA06_CHAR      0x0651

#تعریف_کرو SHAPE_منوال   0
#تعریف_کرو DESHAPE_منوال 1

ساخت uShapeVariables {
     char16_t دم_Char;
     uint32_t uShapeLamalefBegin;
     uint32_t u_Shape_Lamalef_انتہا;
     uint32_t uShapeTashkeelBegin;
     uint32_t u_Shape_Tashkeel_انتہا;
     صحیح spaces_Relative_مقصد_متن_Begin_انتہا;
};

ساکن ثابت uint8_t دم_Family_Isolated_Final[] = {
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

ساکن ثابت uint8_t tashkeelMedial[] = {
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

ساکن ثابت char16_t yeh_Hamza_مقصد_Yeh[] =
{
/* isolated*/ 0xFEEF,
/* final   */ 0xFEF0
};

ساکن ثابت uint8_t IrrelevantPos[] = {
    0x0, 0x2, 0x4, 0x6,
    0x8, 0xA, 0xC, 0xE
};


ساکن ثابت char16_t convertLamAlef[] =
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

ساکن ثابت char16_t araLink[178]=
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

ساکن ثابت uint8_t presALink[] = {
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

ساکن ثابت uint8_t presBLink[]=
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

ساکن ثابت char16_t convertFBto06[] =
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

ساکن ثابت char16_t convertFEto06[] =
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

ساکن ثابت uint8_t shape_جدول[4][4][4]=
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
ساکن خالی
shape_مقصد_Arabic_Digits_With_Context(char16_t *s, int32_t length,
                                char16_t digit_بنیاد,
                                UBool isLogical, UBool آخری_Strong_Was_AL) {
    int32_t i;
    char16_t c;

    digit_بنیاد-=0x30;

    /* the iteration direction depends on the type of input */
    اگر(isLogical) {
        برائے(i=0; i<length; ++i) {
            c=s[i];
            انتخاب(ubidi_getClass(c)) {
            حالت U_بایاں_مقصد_دایاں: /* L */
            حالت U_دایاں_مقصد_بایاں: /* R */
                آخری_Strong_Was_AL=جھوٹ;
                توڑو;
            حالت U_دایاں_مقصد_بایاں_ARABIC: /* AL */
                آخری_Strong_Was_AL=سچ;
                توڑو;
            حالت U_EUROPEAN_نمبر: /* EN */
                اگر (آخری_Strong_Was_AL && static_cast<uint32_t>(c - 0x30) < 10) {
                    s[i] = static_cast<char16_t>(digit_بنیاد + c); /* digitBase+(c-0x30) - digitBase was modified above */
                }
                توڑو;
            طےشدہ :
                توڑو;
            }
        }
    } ورنہ {
        برائے(i=length; i>0; /* pre-decrement in the body */) {
            c=s[--i];
            انتخاب(ubidi_getClass(c)) {
            حالت U_بایاں_مقصد_دایاں: /* L */
            حالت U_دایاں_مقصد_بایاں: /* R */
                آخری_Strong_Was_AL=جھوٹ;
                توڑو;
            حالت U_دایاں_مقصد_بایاں_ARABIC: /* AL */
                آخری_Strong_Was_AL=سچ;
                توڑو;
            حالت U_EUROPEAN_نمبر: /* EN */
                اگر (آخری_Strong_Was_AL && static_cast<uint32_t>(c - 0x30) < 10) {
                    s[i] = static_cast<char16_t>(digit_بنیاد + c); /* digitBase+(c-0x30) - digitBase was modified above */
                }
                توڑو;
            طےشدہ :
                توڑو;
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
ساکن خالی
invert_بفر(char16_t *بفر, int32_t size, uint32_t /*options*/, int32_t lowlimit, int32_t highlimit) {
    char16_t عارضی;
    int32_t i=0,j=0;
    برائے(i=lowlimit,j=size-highlimit-1;i<j;i++,j--) {
        عارضی = بفر[i];
        بفر[i] = بفر[j];
        بفر[j] = عارضی;
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
ساکن اندرونی char16_t
changeLamAlef(char16_t ch) {
    انتخاب(ch) {
    حالت 0x0622 :
        واپس 0x065C;
    حالت 0x0623 :
        واپس 0x065D;
    حالت 0x0625 :
        واپس 0x065E;
    حالت 0x0627 :
        واپس 0x065F;
    }
    واپس 0;
}

/*
 *Name     : getLink
 *Function : Resolves the link between the characters as
 *           Arabic characters have four forms :
 *           Isolated, Initial, Middle and Final Form
 */
ساکن char16_t
getLink(char16_t ch) {
    اگر(ch >= 0x0622 && ch <= 0x06D3) {
        واپس(araLink[ch-0x0622]);
    } ورنہ اگر(ch == 0x200D) {
        واپس(3);
    } ورنہ اگر(ch >= 0x206D && ch <= 0x206F) {
        واپس(4);
    }ورنہ اگر(ch >= 0xFB50 && ch <= 0xFC62) {
        واپس(presALink[ch-0xFB50]);
    } ورنہ اگر(ch >= 0xFE70 && ch <= 0xFEFC) {
        واپس(presBLink[ch-0xFE70]);
    }ورنہ {
        واپس(0);
    }
}

/*
 *Name     : countSpaces
 *Function : Counts the number of spaces
 *           at each end of the logical buffer
 */
ساکن خالی
گنتی_Spaces(char16_t *dest, int32_t size, uint32_t /*options*/, int32_t *spacesCountl, int32_t *spacesCountr) {
    int32_t i = 0;
    int32_t countl = 0,countr = 0;
    جب((dest[i] == SPACE_CHAR) && (countl < size)) {
       countl++;
       i++;
    }
    اگر (countl < size) {  /* the entire buffer is not all space */
        جب(dest[size-1] == SPACE_CHAR) {
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
ساکن اندرونی int32_t
isTashkeelChar(char16_t ch) {
    واپس static_cast<int32_t>(ch >= 0x064B && ch <= 0x0652);
}

/*
 *Name     : isTashkeelCharFE
 *Function : Returns 1 for Tashkeel characters in FE range else return 0
 */
ساکن اندرونی int32_t
isTashkeelCharFE(char16_t ch) {
    واپس static_cast<int32_t>(ch >= 0xFE70 && ch <= 0xFE7F);
}

/*
 *Name     : isAlefChar
 *Function : Returns 1 for Alef characters else return 0
 */
ساکن اندرونی int32_t
isAlefChar(char16_t ch) {
    واپس static_cast<int32_t>(ch == 0x0622 || ch == 0x0623 || ch == 0x0625 || ch == 0x0627);
}

/*
 *Name     : isLamAlefChar
 *Function : Returns 1 for LamAlef characters else return 0
 */
ساکن اندرونی int32_t
isLamAlefChar(char16_t ch) {
    واپس static_cast<int32_t>(ch >= 0xFEF5 && ch <= 0xFEFC);
}

/*BIDI
 *Name     : isTailChar
 *Function : returns 1 if the character matches one of the tail characters (0xfe73 or 0x200b) otherwise returns 0
 */

ساکن اندرونی int32_t
is_دم_Char(char16_t ch) {
    اگر(ch == OLD_دم_CHAR || ch == NEW_دم_CHAR){
            واپس 1;
    }ورنہ{
            واپس 0;
    }
}

/*BIDI
 *Name     : isSeenTailFamilyChar
 *Function : returns 1 if the character is a seen family isolated character
 *           in the FE range otherwise returns 0
 */

ساکن اندرونی int32_t
is_نظر_آیا_دم_Family_Char(char16_t ch) {
    اگر(ch >= 0xfeb1 && ch < 0xfebf){
            واپس دم_Family_Isolated_Final [ch - 0xFEB1];
    }ورنہ{
            واپس 0;
    }
}

 /* Name     : isSeenFamilyChar
  * Function : returns 1 if the character is a seen family character in the Unicode
  *            06 range otherwise returns 0
 */

ساکن اندرونی int32_t
is_نظر_آیا_Family_Char(char16_t  ch){
    اگر(ch >= 0x633 && ch <= 0x636){
        واپس 1;
    }ورنہ {
        واپس 0;
    }
}

/*Start of BIDI*/
/*
 *Name     : isAlefMaksouraChar
 *Function : returns 1 if the character is a Alef Maksoura Final or isolated
 *           otherwise returns 0
 */
ساکن اندرونی int32_t
isAlefMaksouraChar(char16_t ch) {
    واپس static_cast<int32_t>(ch == 0xFEEF || ch == 0xFEF0 || ch == 0x0649);
}

/*
 * Name     : isYehHamzaChar
 * Function : returns 1 if the character is a yehHamza isolated or yehhamza
 *            final is found otherwise returns 0
 */
ساکن اندرونی int32_t
isYehHamzaChar(char16_t ch) {
    اگر((ch==0xFE89)||(ch==0xFE8A)){
        واپس 1;
    }ورنہ{
        واپس 0;
    }
}

 /*
 * Name: isTashkeelOnTatweelChar
 * Function: Checks if the Tashkeel Character is on Tatweel or not,if the
 *           Tashkeel on tatweel (FE range), it returns 1 else if the
 *           Tashkeel with shadda on tatweel (FC range)return 2 otherwise
 *           returns 0
 */
ساکن اندرونی int32_t
isTashkeelOnTatweelChar(char16_t ch){
    اگر(ch >= 0xfe70 && ch <= 0xfe7f && ch != NEW_دم_CHAR && ch != 0xFE75 && ch != SHADDA_TATWEEL_CHAR)
    {
        واپس tashkeelMedial [ch - 0xFE70];
    }ورنہ اگر( (ch >= 0xfcf2 && ch <= 0xfcf4) || (ch == SHADDA_TATWEEL_CHAR)) {
        واپس 2;
    }ورنہ{
        واپس 0;
    }
}

/*
 * Name: isIsolatedTashkeelChar
 * Function: Checks if the Tashkeel Character is in the isolated form
 *           (i.e. Unicode FE range) returns 1 else if the Tashkeel
 *           with shadda is in the isolated form (i.e. Unicode FC range)
 *           returns 2 otherwise returns 0
 */
ساکن اندرونی int32_t
isIsolatedTashkeelChar(char16_t ch){
    اگر(ch >= 0xfe70 && ch <= 0xfe7f && ch != NEW_دم_CHAR && ch != 0xFE75){
        واپس (1 - tashkeelMedial [ch - 0xFE70]);
    }ورنہ اگر(ch >= 0xfc5e && ch <= 0xfc63){
        واپس 1;
    }ورنہ{
        واپس 0;
    }
}




/*
 *Name     : calculateSize
 *Function : This function calculates the destSize to be used in preflighting
 *           when the destSize is equal to 0
 *           It is used also to calculate the new destsize in case the
 *           destination buffer will be resized.
 */

ساکن int32_t
calculate_سائز(ثابت char16_t *ماخذ, int32_t ماخذ_لمبائی,
int32_t dest_سائز,uint32_t options) {
    int32_t i = 0;

    صحیح lamAlefOption = 0;
    صحیح tashkeelOption = 0;

    dest_سائز = ماخذ_لمبائی;

    اگر (((options&U_SHAPE_LETTERS_نقاب) == U_SHAPE_LETTERS_SHAPE ||
        ((options&U_SHAPE_LETTERS_نقاب) == U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED )) &&
        ((options&U_SHAPE_LAMALEF_نقاب) == U_SHAPE_LAMALEF_RESIZE )){
            lamAlefOption = 1;
    }
    اگر((options&U_SHAPE_LETTERS_نقاب) == U_SHAPE_LETTERS_SHAPE &&
       ((options&U_SHAPE_TASHKEEL_نقاب) == U_SHAPE_TASHKEEL_RESIZE ) ){
            tashkeelOption = 1;
        }

    اگر(lamAlefOption || tashkeelOption){
        اگر((options&U_SHAPE_متن_DIRECTION_نقاب)==U_SHAPE_متن_DIRECTION_VISUAL_LTR) {
            برائے(i=0;i<ماخذ_لمبائی;i++) {
                اگر( ((isAlefChar(ماخذ[i]))&& (i<(ماخذ_لمبائی-1)) &&(ماخذ[i+1] == LAM_CHAR)) || (isTashkeelCharFE(ماخذ[i])) ) {
                        dest_سائز--;
                    }
                }
            }ورنہ اگر((options&U_SHAPE_متن_DIRECTION_نقاب)==U_SHAPE_متن_DIRECTION_LOGICAL) {
                برائے(i=0;i<ماخذ_لمبائی;i++) {
                    اگر( ( (ماخذ[i] == LAM_CHAR) && (i<(ماخذ_لمبائی-1)) && (isAlefChar(ماخذ[i+1]))) || (isTashkeelCharFE(ماخذ[i])) ) {
                        dest_سائز--;
                    }
                }
            }
        }

    اگر ((options&U_SHAPE_LETTERS_نقاب) == U_SHAPE_LETTERS_UNSHAPE){
        اگر ( (options&U_SHAPE_LAMALEF_نقاب) == U_SHAPE_LAMALEF_RESIZE){
            برائے(i=0;i<ماخذ_لمبائی;i++) {
                اگر(isLamAlefChar(ماخذ[i]))
                dest_سائز++;
            }
        }
    }

    واپس dest_سائز;
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
ساکن int32_t
handleTashkeelWithTatweel(char16_t *dest, int32_t ماخذ_لمبائی,
             int32_t /*destSize*/, uint32_t /*options*/,
             UError_رمز * /*pErrorCode*/) {
                 صحیح i;
                 برائے(i = 0; i < ماخذ_لمبائی; i++){
                     اگر((isTashkeelOnTatweelChar(dest[i]) == 1)){
                         dest[i] = TATWEEL_CHAR;
                    }ورنہ اگر((isTashkeelOnTatweelChar(dest[i]) == 2)){
                         dest[i] = SHADDA_TATWEEL_CHAR;
                    }ورنہ اگر(isIsolatedTashkeelChar(dest[i]) && dest[i] != SHADDA_CHAR){
                         dest[i] = SPACE_CHAR;
                    }
                 }
                 واپس ماخذ_لمبائی;
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

ساکن int32_t
handleGeneratedSpaces(char16_t *dest, int32_t ماخذ_لمبائی,
                    int32_t dest_سائز,
                    uint32_t options,
                    UError_رمز *p_Error_رمز,ساخت uShapeVariables shapeVars ) {

    int32_t i = 0, j = 0;
    int32_t count = 0;
    char16_t *tempbuffer=خالی_اشاریہ;

    صحیح lamAlefOption = 0;
    صحیح tashkeelOption = 0;
    صحیح shaping_منوال = SHAPE_منوال;

    اگر (shaping_منوال == 0){
        اگر ( (options&U_SHAPE_LAMALEF_نقاب) == U_SHAPE_LAMALEF_RESIZE ){
            lamAlefOption = 1;
        }
        اگر ( (options&U_SHAPE_TASHKEEL_نقاب) == U_SHAPE_TASHKEEL_RESIZE ){
            tashkeelOption = 1;
        }
    }

    اگر (static_cast<size_t>(ماخذ_لمبائی) + 1 > std::numeric_limits<size_t>::max() / U_SIZEOF_UCHAR) {
        *p_Error_رمز = U_اشاریہ_OUTOFBOUNDS_ERROR;
        واپس 0;
    }
    tempbuffer = static_cast<char16_t*>(uprv_malloc((ماخذ_لمبائی + 1) * U_SIZEOF_UCHAR));
    /* Test for nullptr */
    اگر(tempbuffer == خالی_اشاریہ) {
        *p_Error_رمز = U_حافظہ_تخصیص_ERROR;
        واپس 0;
    }


    اگر (lamAlefOption || tashkeelOption){
        uprv_memset(tempbuffer, 0, (ماخذ_لمبائی+1)*U_SIZEOF_UCHAR);

        i = j = 0; count = 0;
        جب(i < ماخذ_لمبائی) {
            اگر ( (lamAlefOption && dest[i] == LAMALEF_SPACE_SUB) ||
               (tashkeelOption && dest[i] == TASHKEEL_SPACE_SUB) ){
                j--;
                count++;
            } ورنہ {
                tempbuffer[j] = dest[i];
            }
            i++;
            j++;
        }

        جب(count >= 0) {
            tempbuffer[i] = 0x0000;
            i--;
            count--;
        }

        u_memcpy(dest, tempbuffer, ماخذ_لمبائی);
        dest_سائز = u_strlen(dest);
    }

      lamAlefOption = 0;

    اگر (shaping_منوال == 0){
        اگر ( (options&U_SHAPE_LAMALEF_نقاب) == U_SHAPE_LAMALEF_NEAR ){
            lamAlefOption = 1;
        }
    }

    اگر (lamAlefOption){
        /* Lam+Alef is already shaped into LamAlef + FFFF */
        i = 0;
        جب(i < ماخذ_لمبائی) {
            اگر(lamAlefOption&&dest[i] == LAMALEF_SPACE_SUB){
                dest[i] = SPACE_CHAR;
            }
            i++;
        }
        dest_سائز = ماخذ_لمبائی;
    }
    lamAlefOption = 0;
    tashkeelOption = 0;

    اگر (shaping_منوال == 0) {
        اگر ( ((options&U_SHAPE_LAMALEF_نقاب) == shapeVars.uShapeLamalefBegin) ||
              (((options&U_SHAPE_LAMALEF_نقاب) == U_SHAPE_LAMALEF_AUTO )
              && (shapeVars.spaces_Relative_مقصد_متن_Begin_انتہا==1)) ) {
            lamAlefOption = 1;
        }
        اگر ( (options&U_SHAPE_TASHKEEL_نقاب) == shapeVars.uShapeTashkeelBegin ) {
            tashkeelOption = 1;
        }
    }

    اگر(lamAlefOption || tashkeelOption){
        uprv_memset(tempbuffer, 0, (ماخذ_لمبائی+1)*U_SIZEOF_UCHAR);
        
        i = j = ماخذ_لمبائی; count = 0;
        
        جب(i >= 0) {
            اگر ( (lamAlefOption && dest[i] == LAMALEF_SPACE_SUB) ||
                 (tashkeelOption && dest[i] == TASHKEEL_SPACE_SUB) ){
                j++;
                count++;
            }ورنہ {
                tempbuffer[j] = dest[i];
            }
            i--;
            j--;
        }

        برائے(i=0 ;i < count; i++){
                tempbuffer[i] = SPACE_CHAR;
        }

        u_memcpy(dest, tempbuffer, ماخذ_لمبائی);
        dest_سائز = ماخذ_لمبائی;
    }



    lamAlefOption = 0;
    tashkeelOption = 0;

    اگر (shaping_منوال == 0) {
        اگر ( ((options&U_SHAPE_LAMALEF_نقاب) == shapeVars.u_Shape_Lamalef_انتہا) ||
              (((options&U_SHAPE_LAMALEF_نقاب) == U_SHAPE_LAMALEF_AUTO )
              && (shapeVars.spaces_Relative_مقصد_متن_Begin_انتہا==0)) ) {
            lamAlefOption = 1;
        }
        اگر ( (options&U_SHAPE_TASHKEEL_نقاب) == shapeVars.u_Shape_Tashkeel_انتہا ){
            tashkeelOption = 1;
        }
    }

    اگر(lamAlefOption || tashkeelOption){
        uprv_memset(tempbuffer, 0, (ماخذ_لمبائی+1)*U_SIZEOF_UCHAR);

        i = j = 0; count = 0;
        جب(i < ماخذ_لمبائی) {
            اگر ( (lamAlefOption && dest[i] == LAMALEF_SPACE_SUB) ||
                 (tashkeelOption && dest[i] == TASHKEEL_SPACE_SUB) ){
                j--;
                count++;
            }ورنہ {
                tempbuffer[j] = dest[i];
            }
            i++;
            j++;
        }

        جب(count >= 0) {
            tempbuffer[i] = SPACE_CHAR;
            i--;
            count--;
        }

        u_memcpy(dest, tempbuffer, ماخذ_لمبائی);
        dest_سائز = ماخذ_لمبائی;
    }


    اگر(tempbuffer){
        uprv_free(tempbuffer);
    }

    واپس dest_سائز;
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

ساکن int32_t
expandCompositCharAtBegin(char16_t *dest, int32_t ماخذ_لمبائی, int32_t dest_سائز,UError_رمز *p_Error_رمز) {
    int32_t      i = 0,j = 0;
    int32_t      countl = 0;
    char16_t *tempbuffer=خالی_اشاریہ;

    tempbuffer = static_cast<char16_t*>(uprv_malloc((ماخذ_لمبائی + 1) * U_SIZEOF_UCHAR));

    /* Test for nullptr */
    اگر(tempbuffer == خالی_اشاریہ) {
        *p_Error_رمز = U_حافظہ_تخصیص_ERROR;
        واپس 0;
    }

        uprv_memset(tempbuffer, 0, (ماخذ_لمبائی+1)*U_SIZEOF_UCHAR);

        i = 0;
        جب(dest[i] == SPACE_CHAR) {
            countl++;
            i++;
        }

        i = j = ماخذ_لمبائی-1;

        جب(i >= 0 && j >= 0) {
            اگر( countl>0 && isLamAlefChar(dest[i])) {
                tempbuffer[j] = LAM_CHAR;
                /* to ensure the array index is within the range */
                U_ASSERT(dest[i] >= 0xFEF5u
                    && dest[i]-0xFEF5u < UPRV_LENGTHOF(convertLamAlef));
                tempbuffer[j-1] = convertLamAlef[ dest[i] - 0xFEF5 ];
                j--;
                countl--;
            }ورنہ {
                 اگر( countl == 0 && isLamAlefChar(dest[i]) ) {
                     *p_Error_رمز=U_NO_SPACE_دستیاب;
                     }
                 tempbuffer[j] = dest[i];
            }
            i--;
            j--;
        }
        u_memcpy(dest, tempbuffer, ماخذ_لمبائی);

        uprv_free(tempbuffer);

        dest_سائز = ماخذ_لمبائی;
        واپس dest_سائز;
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

ساکن int32_t
expand_Composit_Char_At_انتہا(char16_t *dest, int32_t ماخذ_لمبائی, int32_t dest_سائز,UError_رمز *p_Error_رمز) {
    int32_t      i = 0,j = 0;

    int32_t      countr = 0;
    int32_t  inpsize = ماخذ_لمبائی;

    char16_t *tempbuffer=خالی_اشاریہ;
    tempbuffer = static_cast<char16_t*>(uprv_malloc((ماخذ_لمبائی + 1) * U_SIZEOF_UCHAR));

    /* Test for nullptr */
    اگر(tempbuffer == خالی_اشاریہ) {
        *p_Error_رمز = U_حافظہ_تخصیص_ERROR;
         واپس 0;
    }

    uprv_memset(tempbuffer, 0, (ماخذ_لمبائی+1)*U_SIZEOF_UCHAR);

    جب(inpsize>0 && dest[inpsize-1] == SPACE_CHAR) {
        countr++;
        inpsize--;
    }

    i = ماخذ_لمبائی - countr - 1;
    j = ماخذ_لمبائی - 1;

    جب(i >= 0 && j >= 0) {
        اگر( countr>0 && isLamAlefChar(dest[i]) ) {
            tempbuffer[j] = LAM_CHAR;
            tempbuffer[j-1] = convertLamAlef[ dest[i] - 0xFEF5 ];
            j--;
            countr--;
        }ورنہ {
            اگر ((countr == 0) && isLamAlefChar(dest[i]) ) {
                *p_Error_رمز=U_NO_SPACE_دستیاب;
            }
            tempbuffer[j] = dest[i];
        }
        i--;
        j--;
    }

    اگر(countr > 0) {
        u_memmove(tempbuffer, tempbuffer+countr, ماخذ_لمبائی);
        اگر(u_strlen(tempbuffer) < ماخذ_لمبائی) {
            برائے(i=ماخذ_لمبائی-1;i>=ماخذ_لمبائی-countr;i--) {
                tempbuffer[i] = SPACE_CHAR;
            }
        }
    }
    u_memcpy(dest, tempbuffer, ماخذ_لمبائی);

    uprv_free(tempbuffer);

    dest_سائز = ماخذ_لمبائی;
    واپس dest_سائز;
}

/*
 *Name     : expandCompositCharAtNear
 *Function : Expands the LamAlef character into Lam + Alef, YehHamza character
 *           into Yeh + Hamza, SeenFamily character into SeenFamily character
 *           + Tail, while consuming the space next to the character.
 *           If there are no spaces next to the character, an error
 *           will be set to U_NO_SPACE_AVAILABLE as defined in utypes.h
 */

ساکن int32_t
expandCompositCharAtNear(char16_t *dest, int32_t ماخذ_لمبائی, int32_t dest_سائز,UError_رمز *p_Error_رمز,
                         صحیح yehHamzaOption, صحیح نظر_آیا_دم_Option, صحیح lamAlefOption, ساخت uShapeVariables shapeVars) {
    int32_t      i = 0;


    char16_t lamalefChar, yehhamzaChar;

    برائے(i = 0 ;i<=ماخذ_لمبائی-1;i++) {
            اگر (نظر_آیا_دم_Option && is_نظر_آیا_دم_Family_Char(dest[i])) {
                اگر ((i>0) && (dest[i-1] == SPACE_CHAR) ) {
                    dest[i-1] = shapeVars.دم_Char;
                }ورنہ {
                    *p_Error_رمز=U_NO_SPACE_دستیاب;
                }
            }ورنہ اگر(yehHamzaOption && (isYehHamzaChar(dest[i])) ) {
                اگر ((i>0) && (dest[i-1] == SPACE_CHAR) ) {
                    yehhamzaChar = dest[i];
                    dest[i] = yeh_Hamza_مقصد_Yeh[yehhamzaChar - YEH_HAMZAFE_CHAR];
                    dest[i-1] = HAMZAFE_CHAR;
                }ورنہ {

                    *p_Error_رمز=U_NO_SPACE_دستیاب;
                }
            }ورنہ اگر(lamAlefOption && isLamAlefChar(dest[i+1])) {
                اگر(dest[i] == SPACE_CHAR){
                    lamalefChar = dest[i+1];
                    dest[i+1] = LAM_CHAR;
                    dest[i] = convertLamAlef[ lamalefChar - 0xFEF5 ];
                }ورنہ {
                    *p_Error_رمز=U_NO_SPACE_دستیاب;
                }
            }
       }
       dest_سائز = ماخذ_لمبائی;
       واپس dest_سائز;
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

ساکن int32_t
expandCompositChar(char16_t *dest, int32_t ماخذ_لمبائی,
              int32_t dest_سائز,uint32_t options,
              UError_رمز *p_Error_رمز, صحیح shaping_منوال,ساخت uShapeVariables shapeVars) {

    int32_t      i = 0,j = 0;

    char16_t *tempbuffer=خالی_اشاریہ;
    صحیح yehHamzaOption = 0;
    صحیح نظر_آیا_دم_Option = 0;
    صحیح lamAlefOption = 0;

    اگر (shaping_منوال == 1){
        اگر ( (options&U_SHAPE_LAMALEF_نقاب) == U_SHAPE_LAMALEF_AUTO){

            اگر(shapeVars.spaces_Relative_مقصد_متن_Begin_انتہا == 0) {
                dest_سائز = expand_Composit_Char_At_انتہا(dest, ماخذ_لمبائی, dest_سائز, p_Error_رمز);

                اگر(*p_Error_رمز == U_NO_SPACE_دستیاب) {
                    *p_Error_رمز = U_ZERO_ERROR;
                    dest_سائز = expandCompositCharAtBegin(dest, ماخذ_لمبائی, dest_سائز, p_Error_رمز);
                }
            }ورنہ {
                dest_سائز = expandCompositCharAtBegin(dest, ماخذ_لمبائی, dest_سائز, p_Error_رمز);

                اگر(*p_Error_رمز == U_NO_SPACE_دستیاب) {
                    *p_Error_رمز = U_ZERO_ERROR;
                    dest_سائز = expand_Composit_Char_At_انتہا(dest, ماخذ_لمبائی, dest_سائز, p_Error_رمز);
                }
            }

            اگر(*p_Error_رمز == U_NO_SPACE_دستیاب) {
                *p_Error_رمز = U_ZERO_ERROR;
                dest_سائز = expandCompositCharAtNear(dest, ماخذ_لمبائی, dest_سائز, p_Error_رمز, yehHamzaOption,
                                                نظر_آیا_دم_Option, 1,shapeVars);
            }
        }
    }

    اگر (shaping_منوال == 1){
        اگر ( (options&U_SHAPE_LAMALEF_نقاب) == shapeVars.u_Shape_Lamalef_انتہا){
            dest_سائز = expand_Composit_Char_At_انتہا(dest, ماخذ_لمبائی, dest_سائز, p_Error_رمز);
        }
    }

    اگر (shaping_منوال == 1){
        اگر ( (options&U_SHAPE_LAMALEF_نقاب) == shapeVars.uShapeLamalefBegin){
            dest_سائز = expandCompositCharAtBegin(dest, ماخذ_لمبائی, dest_سائز, p_Error_رمز);
        }
    }

    اگر (shaping_منوال == 0){
         اگر ((options&U_SHAPE_YEHHAMZA_نقاب) == U_SHAPE_YEHHAMZA_TWOCELL_NEAR){
             yehHamzaOption = 1;
         }
         اگر ((options&U_SHAPE_نظر_آیا_نقاب) == U_SHAPE_نظر_آیا_TWOCELL_NEAR){
            نظر_آیا_دم_Option = 1;
         }
    }
    اگر (shaping_منوال == 1) {
        اگر ( (options&U_SHAPE_LAMALEF_نقاب) == U_SHAPE_LAMALEF_NEAR) {
            lamAlefOption = 1;
        }
    }


    اگر (yehHamzaOption || نظر_آیا_دم_Option || lamAlefOption){
        dest_سائز = expandCompositCharAtNear(dest, ماخذ_لمبائی, dest_سائز, p_Error_رمز, yehHamzaOption,
                                            نظر_آیا_دم_Option,lamAlefOption,shapeVars);
    }


    اگر (shaping_منوال == 1){
        اگر ( (options&U_SHAPE_LAMALEF_نقاب) == U_SHAPE_LAMALEF_RESIZE){
            dest_سائز = calculate_سائز(dest,ماخذ_لمبائی,dest_سائز,options);
            tempbuffer = static_cast<char16_t*>(uprv_malloc((dest_سائز + 1) * U_SIZEOF_UCHAR));

            /* Test for nullptr */
            اگر(tempbuffer == خالی_اشاریہ) {
                *p_Error_رمز = U_حافظہ_تخصیص_ERROR;
                واپس 0;
            }

            uprv_memset(tempbuffer, 0, (dest_سائز+1)*U_SIZEOF_UCHAR);

            i = j = 0;
            جب(i < dest_سائز && j < dest_سائز) {
                اگر(isLamAlefChar(dest[i]) ) {
                    tempbuffer[j] = convertLamAlef[ dest[i] - 0xFEF5 ];
                    tempbuffer[j+1] = LAM_CHAR;
                    j++;
                }ورنہ {
                    tempbuffer[j] = dest[i];
                }
                i++;
                j++;
            }

            u_memcpy(dest, tempbuffer, dest_سائز);
        }
    }

    اگر(tempbuffer) {
        uprv_free(tempbuffer);
    }
    واپس dest_سائز;
}

/*
 *Name     : shapeUnicode
 *Function : Converts an Arabic Unicode buffer in 06xx Range into a shaped
 *           arabic Unicode buffer in FExx Range
 */
ساکن int32_t
shapeUnicode(char16_t *dest, int32_t ماخذ_لمبائی,
             int32_t dest_سائز,uint32_t options,
             UError_رمز *p_Error_رمز,
             صحیح tashkeelFlag, ساخت uShapeVariables shapeVars) {

    int32_t          i, iend;
    int32_t          قدم;
    int32_t          آخری_Pos,Nx, Nw;
    غیرعلامتی صحیح     Shape;
    int32_t          lamalef_ملا = 0;
    int32_t seenfam_ملا = 0, yehhamza_ملا =0, tashkeel_ملا  = 0;
    char16_t         پچھلا_Link = 0, آخری_Link = 0, currLink, اگلا_Link = 0;
    char16_t         wLamalef;

    /*
     * Converts the input buffer from FExx Range into 06xx Range
     * to make sure that all characters are in the 06xx range
     * even the lamalef is converted to the special region in
     * the 06xx range
     */
    اگر ((options & U_SHAPE_PRESERVE_PRESENTATION_نقاب)  == U_SHAPE_PRESERVE_PRESENTATION_NOOP) {
        برائے (i = 0; i < ماخذ_لمبائی; i++) {
            char16_t داخلہ_Char  = dest[i];
            اگر ( (داخلہ_Char >= 0xFB50) && (داخلہ_Char <= 0xFBFF)) {
                char16_t c = convertFBto06 [ (داخلہ_Char - 0xFB50) ];
                اگر (c != 0)
                    dest[i] = c;
            } ورنہ اگر ( (داخلہ_Char >= 0xFE70) && (داخلہ_Char <= 0xFEFC)) {
                dest[i] = convertFEto06 [ (داخلہ_Char - 0xFE70) ] ;
            } ورنہ {
                dest[i] = داخلہ_Char ;
            }
        }
    }


    /* sets the index to the end of the buffer, together with the step point to -1 */
    i = ماخذ_لمبائی - 1;
    iend = -1;
    قدم = -1;

    /*
     * This function resolves the link between the characters .
     * Arabic characters have four forms :
     * Isolated Form, Initial Form, Middle Form and Final Form
     */
    currLink = getLink(dest[i]);

    آخری_Pos = i;
    Nx = -2, Nw = 0;

    جب (i != iend) {
        /* If high byte of currLink > 0 then more than one shape */
        اگر ((currLink & 0xFF00) > 0 || (getLink(dest[i]) & IRRELEVANT) != 0) {
            Nw = i + قدم;
            جب (Nx < 0) {         /* we need to know about next char */
                اگر(Nw == iend) {
                    اگلا_Link = 0;
                    Nx = 3000;
                } ورنہ {
                    اگلا_Link = getLink(dest[Nw]);
                    اگر((اگلا_Link & IRRELEVANT) == 0) {
                        Nx = Nw;
                    } ورنہ {
                        Nw = Nw + قدم;
                    }
                }
            }

            اگر ( ((currLink & ALEFTYPE) > 0)  &&  ((آخری_Link & LAMTYPE) > 0) ) {
                lamalef_ملا = 1;
                wLamalef = changeLamAlef(dest[i]); /*get from 0x065C-0x065f */
                اگر ( wLamalef != 0) {
                    dest[i] = LAMALEF_SPACE_SUB;            /* The default case is to drop the Alef and replace */
                    dest[آخری_Pos] =wLamalef;     /* it by LAMALEF_SPACE_SUB which is the last character in the  */
                    i=آخری_Pos;                   /* unicode private use area, this is done to make   */
                }                                /* sure that removeLamAlefSpaces() handles only the */
                آخری_Link = پچھلا_Link;             /* spaces generated during lamalef generation.      */
                currLink = getLink(wLamalef);    /* LAMALEF_SPACE_SUB is added here and is replaced by spaces   */
            }                                    /* in removeLamAlefSpaces()                         */

            اگر ((i > 0) && (dest[i-1] == SPACE_CHAR)){
                اگر ( is_نظر_آیا_Family_Char(dest[i])) {
                    seenfam_ملا = 1;
                } ورنہ اگر (dest[i] == YEH_HAMZA_CHAR) {
                    yehhamza_ملا = 1;
                }
            }
            ورنہ اگر(i==0){
                اگر ( is_نظر_آیا_Family_Char(dest[i])){
                    seenfam_ملا = 1;
                } ورنہ اگر (dest[i] == YEH_HAMZA_CHAR) {
                    yehhamza_ملا = 1;
                }
            }

            /*
             * get the proper shape according to link ability of neighbors
             * and of character; depends on the order of the shapes
             * (isolated, initial, middle, final) in the compatibility area
             */
            Shape = shape_جدول[اگلا_Link & (LINKR + LINKL)]
                              [آخری_Link & (LINKR + LINKL)]
                              [currLink & (LINKR + LINKL)];

            اگر ((currLink & (LINKR+LINKL)) == 1) {
                Shape &= 1;
            } ورنہ اگر(isTashkeelChar(dest[i])) {
                اگر( (آخری_Link & LINKL) && (اگلا_Link & LINKR) && (tashkeelFlag == 1) &&
                     dest[i] != 0x064C && dest[i] != 0x064D )
                {
                    Shape = 1;
                    اگر( (اگلا_Link&ALEFTYPE) == ALEFTYPE && (آخری_Link&LAMTYPE) == LAMTYPE ) {
                        Shape = 0;
                    }
                } ورنہ اگر(tashkeelFlag == 2 && dest[i] == SHADDA06_CHAR){
                    Shape = 1;
                } ورنہ {
                    Shape = 0;
                }
            }
            اگر ((dest[i] ^ 0x0600) < 0x100) {
                اگر ( isTashkeelChar(dest[i]) ){
                    اگر (tashkeelFlag == 2  && dest[i] != SHADDA06_CHAR){
                        dest[i] = TASHKEEL_SPACE_SUB;
                        tashkeel_ملا  = 1;
                    } ورنہ {
                        /* to ensure the array index is within the range */
                        U_ASSERT(dest[i] >= 0x064Bu
                            && dest[i]-0x064Bu < UPRV_LENGTHOF(IrrelevantPos));
                        dest[i] =  0xFE70 + IrrelevantPos[(dest[i] - 0x064B)] + static_cast<char16_t>(Shape);
                    }
                }ورنہ اگر ((currLink & APRESENT) > 0) {
                    dest[i] = static_cast<char16_t>(0xFB50 + (currLink >> 8) + Shape);
                }ورنہ اگر ((currLink >> 8) > 0 && (currLink & IRRELEVANT) == 0) {
                    dest[i] = static_cast<char16_t>(0xFE70 + (currLink >> 8) + Shape);
                }
            }
        }

        /* move one notch forward */
        اگر ((currLink & IRRELEVANT) == 0) {
            پچھلا_Link = آخری_Link;
            آخری_Link = currLink;
            آخری_Pos = i;
        }

        i = i + قدم;
        اگر (i == Nx) {
            currLink = اگلا_Link;
            Nx = -2;
        } ورنہ اگر(i != iend) {
            currLink = getLink(dest[i]);
        }
    }
    dest_سائز = ماخذ_لمبائی;
    اگر ( (lamalef_ملا != 0 ) || (tashkeel_ملا  != 0) ){
        dest_سائز = handleGeneratedSpaces(dest,ماخذ_لمبائی,dest_سائز,options,p_Error_رمز, shapeVars);
    }

    اگر ( (seenfam_ملا != 0) || (yehhamza_ملا != 0) ) {
        dest_سائز = expandCompositChar(dest, ماخذ_لمبائی,dest_سائز,options,p_Error_رمز, SHAPE_منوال,shapeVars);
    }
    واپس dest_سائز;
}

/*
 *Name     : deShapeUnicode
 *Function : Converts an Arabic Unicode buffer in FExx Range into unshaped
 *           arabic Unicode buffer in 06xx Range
 */
ساکن int32_t
deShapeUnicode(char16_t *dest, int32_t ماخذ_لمبائی,
               int32_t dest_سائز,uint32_t options,
               UError_رمز *p_Error_رمز, ساخت uShapeVariables shapeVars) {
    int32_t i = 0;
    int32_t lamalef_ملا = 0;
    int32_t yehHamzaComposeEnabled = 0;
    int32_t نظر_آیا_Compose_Enabled = 0;

    yehHamzaComposeEnabled = ((options&U_SHAPE_YEHHAMZA_نقاب) == U_SHAPE_YEHHAMZA_TWOCELL_NEAR) ? 1 : 0;
    نظر_آیا_Compose_Enabled = ((options&U_SHAPE_نظر_آیا_نقاب) == U_SHAPE_نظر_آیا_TWOCELL_NEAR)? 1 : 0;

    /*
     *This for loop changes the buffer from the Unicode FE range to
     *the Unicode 06 range
     */

    برائے(i = 0; i < ماخذ_لمبائی; i++) {
        char16_t  داخلہ_Char = dest[i];
        اگر ( (داخلہ_Char >= 0xFB50) && (داخلہ_Char <= 0xFBFF)) { /* FBxx Arabic range */
            char16_t c = convertFBto06 [ (داخلہ_Char - 0xFB50) ];
            اگر (c != 0)
                dest[i] = c;
        } ورنہ اگر( (yehHamzaComposeEnabled == 1) && ((داخلہ_Char == HAMZA06_CHAR) || (داخلہ_Char == HAMZAFE_CHAR))
                && (i < (ماخذ_لمبائی - 1)) && isAlefMaksouraChar(dest[i+1] )) {
                 dest[i] = SPACE_CHAR;
                 dest[i+1] = YEH_HAMZA_CHAR;
        } ورنہ اگر ( (نظر_آیا_Compose_Enabled == 1) && (is_دم_Char(داخلہ_Char)) && (i< (ماخذ_لمبائی - 1))
                        && (is_نظر_آیا_دم_Family_Char(dest[i+1])) ) {
                dest[i] = SPACE_CHAR;
        } ورنہ اگر (( داخلہ_Char >= 0xFE70) && (داخلہ_Char <= 0xFEF4 )) { /* FExx Arabic range */
                dest[i] = convertFEto06 [ (داخلہ_Char - 0xFE70) ];
        } ورنہ {
            dest[i] = داخلہ_Char ;
        }

        اگر( isLamAlefChar(dest[i]) )
            lamalef_ملا = 1;
    }

   dest_سائز = ماخذ_لمبائی;
   اگر (lamalef_ملا != 0){
          dest_سائز = expandCompositChar(dest,ماخذ_لمبائی,dest_سائز,options,p_Error_رمز,DESHAPE_منوال, shapeVars);
   }
   واپس dest_سائز;
}

/*
 ****************************************
 * u_shapeArabic
 ****************************************
 */

U_CAPI int32_t U_EXPORT2
u_shapeArabic(ثابت char16_t *ماخذ, int32_t ماخذ_لمبائی,
              char16_t *dest, int32_t dest_استعداد,
              uint32_t options,
              UError_رمز *p_Error_رمز) {

    int32_t dest_لمبائی;
    ساخت  uShapeVariables shapeVars = { OLD_دم_CHAR,U_SHAPE_LAMALEF_BEGIN,U_SHAPE_LAMALEF_انتہا,U_SHAPE_TASHKEEL_BEGIN,U_SHAPE_TASHKEEL_انتہا,0};

    /* usual error checking */
    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }

    /* make sure that no reserved options values are used; allow dest==nullptr only for preflighting */
    اگر( ماخذ==خالی_اشاریہ || ماخذ_لمبائی<-1 || (dest==خالی_اشاریہ && dest_استعداد!=0) || dest_استعداد<0 ||
                (((options&U_SHAPE_TASHKEEL_نقاب) > 0) &&
                 ((options&U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED) == U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED) ) ||
                (((options&U_SHAPE_TASHKEEL_نقاب) > 0) &&
                 ((options&U_SHAPE_LETTERS_نقاب) == U_SHAPE_LETTERS_UNSHAPE)) ||
                (options&U_SHAPE_DIGIT_TYPE_RESERVED)==U_SHAPE_DIGIT_TYPE_RESERVED ||
                (options&U_SHAPE_DIGITS_نقاب)==U_SHAPE_DIGITS_RESERVED ||
                ((options&U_SHAPE_LAMALEF_نقاب) != U_SHAPE_LAMALEF_RESIZE  &&
                (options&U_SHAPE_AGGREGATE_TASHKEEL_نقاب) != 0) ||
                ((options&U_SHAPE_AGGREGATE_TASHKEEL_نقاب) == U_SHAPE_AGGREGATE_TASHKEEL &&
                (options&U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED) != U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED)
    )
    {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }
    /* Validate  lamalef options */
    اگر(((options&U_SHAPE_LAMALEF_نقاب) > 0)&&
              !(((options & U_SHAPE_LAMALEF_نقاب)==U_SHAPE_LAMALEF_BEGIN) ||
                ((options & U_SHAPE_LAMALEF_نقاب)==U_SHAPE_LAMALEF_انتہا ) ||
                ((options & U_SHAPE_LAMALEF_نقاب)==U_SHAPE_LAMALEF_RESIZE )||
                 ((options & U_SHAPE_LAMALEF_نقاب)==U_SHAPE_LAMALEF_AUTO) ||
                 ((options & U_SHAPE_LAMALEF_نقاب)==U_SHAPE_LAMALEF_NEAR)))
    {
         *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }
    /* Validate  Tashkeel options */
    اگر(((options&U_SHAPE_TASHKEEL_نقاب) > 0)&&
                   !(((options & U_SHAPE_TASHKEEL_نقاب)==U_SHAPE_TASHKEEL_BEGIN) ||
                     ((options & U_SHAPE_TASHKEEL_نقاب)==U_SHAPE_TASHKEEL_انتہا )
                    ||((options & U_SHAPE_TASHKEEL_نقاب)==U_SHAPE_TASHKEEL_RESIZE )||
                    ((options & U_SHAPE_TASHKEEL_نقاب)==U_SHAPE_TASHKEEL_REPLACE_BY_TATWEEL)))
    {
         *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }
    /* determine the source length */
    اگر(ماخذ_لمبائی==-1) {
        ماخذ_لمبائی=u_strlen(ماخذ);
    }
    اگر(ماخذ_لمبائی<=0) {
        واپس u_terminateUChars(dest, dest_استعداد, 0, p_Error_رمز);
    }

    /* check that source and destination do not overlap */
    اگر( dest!=خالی_اشاریہ &&
        ((ماخذ<=dest && dest<ماخذ+ماخذ_لمبائی) ||
         (dest<=ماخذ && ماخذ<dest+dest_استعداد))) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    /* Does Options contain the new Seen Tail Unicode code point option */
    اگر ( (options&U_SHAPE_دم_TYPE_نقاب) == U_SHAPE_دم_NEW_UNICODE){
        shapeVars.دم_Char = NEW_دم_CHAR;
    }ورنہ {
        shapeVars.دم_Char = OLD_دم_CHAR;
    }

    اگر((options&U_SHAPE_LETTERS_نقاب)!=U_SHAPE_LETTERS_NOOP) {
        char16_t بفر[300];
        char16_t *tempbuffer, *tempsource = خالی_اشاریہ;
        int32_t output_سائز, spacesCountl=0, spacesCountr=0;

        اگر((options&U_SHAPE_AGGREGATE_TASHKEEL_نقاب)>0) {
            int32_t logical_ترتیب = (options&U_SHAPE_متن_DIRECTION_نقاب) == U_SHAPE_متن_DIRECTION_LOGICAL;
            int32_t aggregate_tashkeel =
                        (options&(U_SHAPE_AGGREGATE_TASHKEEL_نقاب+U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED)) ==
                        (U_SHAPE_AGGREGATE_TASHKEEL+U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED);
            صحیح قدم=logical_ترتیب?1:-1;
            صحیح j=logical_ترتیب?-1:2*ماخذ_لمبائی;
            صحیح i=logical_ترتیب?-1:ماخذ_لمبائی;
            صحیح انتہا=logical_ترتیب?ماخذ_لمبائی:-1;
            صحیح aggregation_possible = 1;
            char16_t پچھلا = 0;
            char16_t پچھلا_Link, currLink = 0;
            صحیح new_ماخذ_لمبائی = 0;
            tempsource = (char16_t *)uprv_malloc(2*ماخذ_لمبائی*U_SIZEOF_UCHAR);
            اگر(tempsource == خالی_اشاریہ) {
                *p_Error_رمز = U_حافظہ_تخصیص_ERROR;
                واپس 0;
            }

            جب ((i+=قدم) != انتہا) {
                پچھلا_Link = currLink;
                currLink = getLink(ماخذ[i]);
                اگر (aggregate_tashkeel && ((پچھلا_Link|currLink)&COMBINE) == COMBINE && aggregation_possible) {
                    اگر (j<0 || j>=2*ماخذ_لمبائی) {
                        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
                        اگر (tempsource != خالی_اشاریہ) uprv_free(tempsource);
                        واپس 0;
                    }
                    aggregation_possible = 0;
                    tempsource[j] = (پچھلا<ماخذ[i]?پچھلا:ماخذ[i])-0x064C+0xFC5E;
                    currLink = getLink(tempsource[j]);
                } ورنہ {
                    aggregation_possible = 1;
                    tempsource[j+=قدم] = ماخذ[i];
                    پچھلا = ماخذ[i];
                    new_ماخذ_لمبائی++;
                }
            }
            ماخذ = tempsource+(logical_ترتیب?0:j);
            ماخذ_لمبائی = new_ماخذ_لمبائی;
        }

        /* calculate destination size */
        /* TODO: do we ever need to do this pure preflighting? */
        اگر(((options&U_SHAPE_LAMALEF_نقاب)==U_SHAPE_LAMALEF_RESIZE) ||
           ((options&U_SHAPE_TASHKEEL_نقاب)==U_SHAPE_TASHKEEL_RESIZE)) {
            output_سائز=calculate_سائز(ماخذ,ماخذ_لمبائی,dest_استعداد,options);
        } ورنہ {
            output_سائز=ماخذ_لمبائی;
        }

        اگر(output_سائز>dest_استعداد) {
            *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                اگر (tempsource != خالی_اشاریہ) uprv_free(tempsource);
            واپس output_سائز;
        }

        /*
         * need a temporary buffer of size max(outputSize, sourceLength)
         * because at first we copy source->temp
         */
        اگر(ماخذ_لمبائی>output_سائز) {
            output_سائز=ماخذ_لمبائی;
        }

        /* Start of Arabic letter shaping part */
        اگر(output_سائز<=UPRV_LENGTHOF(بفر)) {
            output_سائز=UPRV_LENGTHOF(بفر);
            tempbuffer=بفر;
        } ورنہ {
            tempbuffer = (char16_t *)uprv_malloc(output_سائز*U_SIZEOF_UCHAR);

            /*Test for nullptr*/
            اگر(tempbuffer == خالی_اشاریہ) {
                *p_Error_رمز = U_حافظہ_تخصیص_ERROR;
                اگر (tempsource != خالی_اشاریہ) uprv_free(tempsource);
                واپس 0;
            }
        }
        u_memcpy(tempbuffer, ماخذ, ماخذ_لمبائی);
        اگر (tempsource != خالی_اشاریہ){
            uprv_free(tempsource);
        }

        اگر(ماخذ_لمبائی<output_سائز) {
            uprv_memset(tempbuffer+ماخذ_لمبائی, 0, (output_سائز-ماخذ_لمبائی)*U_SIZEOF_UCHAR);
        }

        اگر((options&U_SHAPE_متن_DIRECTION_نقاب) == U_SHAPE_متن_DIRECTION_LOGICAL) {
            گنتی_Spaces(tempbuffer,ماخذ_لمبائی,options,&spacesCountl,&spacesCountr);
            invert_بفر(tempbuffer,ماخذ_لمبائی,options,spacesCountl,spacesCountr);
        }

        اگر((options&U_SHAPE_متن_DIRECTION_نقاب) == U_SHAPE_متن_DIRECTION_VISUAL_LTR) {
            اگر((options&U_SHAPE_SPACES_RELATIVE_مقصد_متن_نقاب) == U_SHAPE_SPACES_RELATIVE_مقصد_متن_BEGIN_انتہا) {
                shapeVars.spaces_Relative_مقصد_متن_Begin_انتہا = 1;
                shapeVars.uShapeLamalefBegin = U_SHAPE_LAMALEF_انتہا;
                shapeVars.u_Shape_Lamalef_انتہا = U_SHAPE_LAMALEF_BEGIN;
                shapeVars.uShapeTashkeelBegin = U_SHAPE_TASHKEEL_انتہا;
                shapeVars.u_Shape_Tashkeel_انتہا = U_SHAPE_TASHKEEL_BEGIN;
            }
        }

        انتخاب(options&U_SHAPE_LETTERS_نقاب) {
        حالت U_SHAPE_LETTERS_SHAPE :
             اگر( (options&U_SHAPE_TASHKEEL_نقاب)> 0
                 && ((options&U_SHAPE_TASHKEEL_نقاب) !=U_SHAPE_TASHKEEL_REPLACE_BY_TATWEEL)) {
                /* Call the shaping function with tashkeel flag == 2 for removal of tashkeel */
                dest_لمبائی = shapeUnicode(tempbuffer,ماخذ_لمبائی,dest_استعداد,options,p_Error_رمز,2,shapeVars);
             }ورنہ {
                /* default Call the shaping function with tashkeel flag == 1 */
                dest_لمبائی = shapeUnicode(tempbuffer,ماخذ_لمبائی,dest_استعداد,options,p_Error_رمز,1,shapeVars);

                /*After shaping text check if user wants to remove tashkeel and replace it with tatweel*/
                اگر( (options&U_SHAPE_TASHKEEL_نقاب) == U_SHAPE_TASHKEEL_REPLACE_BY_TATWEEL){
                  dest_لمبائی = handleTashkeelWithTatweel(tempbuffer,dest_لمبائی,dest_استعداد,options,p_Error_رمز);
                }
            }
            توڑو;
        حالت U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED :
            /* Call the shaping function with tashkeel flag == 0 */
            dest_لمبائی = shapeUnicode(tempbuffer,ماخذ_لمبائی,dest_استعداد,options,p_Error_رمز,0,shapeVars);
            توڑو;

        حالت U_SHAPE_LETTERS_UNSHAPE :
            /* Call the deshaping function */
            dest_لمبائی = deShapeUnicode(tempbuffer,ماخذ_لمبائی,dest_استعداد,options,p_Error_رمز,shapeVars);
            توڑو;
        طےشدہ :
            /* will never occur because of validity checks above */
            dest_لمبائی = 0;
            توڑو;
        }

        /*
         * TODO: (markus 2002aug01)
         * For as long as we always preflight the outputSize above
         * we should U_ASSERT(outputSize==destLength)
         * except for the adjustment above before the tempbuffer allocation
         */

        اگر((options&U_SHAPE_متن_DIRECTION_نقاب) == U_SHAPE_متن_DIRECTION_LOGICAL) {
            گنتی_Spaces(tempbuffer,dest_لمبائی,options,&spacesCountl,&spacesCountr);
            invert_بفر(tempbuffer,dest_لمبائی,options,spacesCountl,spacesCountr);
        }
        u_memcpy(dest, tempbuffer, uprv_min(dest_لمبائی, dest_استعداد));

        اگر(tempbuffer!=بفر) {
            uprv_free(tempbuffer);
        }

        اگر(dest_لمبائی>dest_استعداد) {
            *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
            واپس dest_لمبائی;
        }

        /* End of Arabic letter shaping part */
    } ورنہ {
        /*
         * No letter shaping:
         * just make sure the destination is large enough and copy the string.
         */
        اگر(dest_استعداد<ماخذ_لمبائی) {
            /* this catches preflighting, too */
            *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
            واپس ماخذ_لمبائی;
        }
        u_memcpy(dest, ماخذ, ماخذ_لمبائی);
        dest_لمبائی=ماخذ_لمبائی;
    }

    /*
     * Perform number shaping.
     * With UTF-16 or UTF-32, the length of the string is constant.
     * The easiest way to do this is to operate on the destination and
     * "shape" the digits in-place.
     */
    اگر((options&U_SHAPE_DIGITS_نقاب)!=U_SHAPE_DIGITS_NOOP) {
        char16_t digit_بنیاد;
        int32_t i;

        /* select the requested digit group */
        انتخاب(options&U_SHAPE_DIGIT_TYPE_نقاب) {
        حالت U_SHAPE_DIGIT_TYPE_AN:
            digit_بنیاد=0x660; /* Unicode: "Arabic-Indic digits" */
            توڑو;
        حالت U_SHAPE_DIGIT_TYPE_AN_EXTENDED:
            digit_بنیاد=0x6f0; /* Unicode: "Eastern Arabic-Indic digits (Persian and Urdu)" */
            توڑو;
        طےشدہ:
            /* will never occur because of validity checks above */
            digit_بنیاد=0;
            توڑو;
        }

        /* perform the requested operation */
        انتخاب(options&U_SHAPE_DIGITS_نقاب) {
        حالت U_SHAPE_DIGITS_EN2AN:
            /* add (digitBase-'0') to each European (ASCII) digit code point */
            digit_بنیاد-=0x30;
            برائے(i=0; i<dest_لمبائی; ++i) {
                اگر(((uint32_t)dest[i]-0x30)<10) {
                    dest[i]+=digit_بنیاد;
                }
            }
            توڑو;
        حالت U_SHAPE_DIGITS_AN2EN:
            /* subtract (digitBase-'0') from each Arabic digit code point */
            برائے(i=0; i<dest_لمبائی; ++i) {
                اگر(((uint32_t)dest[i]-(uint32_t)digit_بنیاد)<10) {
                    dest[i]-=digit_بنیاد-0x30;
                }
            }
            توڑو;
        حالت U_SHAPE_DIGITS_ALEN2AN_INIT_LR:
            shape_مقصد_Arabic_Digits_With_Context(dest, dest_لمبائی,
                                            digit_بنیاد,
                                            (options & U_SHAPE_متن_DIRECTION_نقاب) == U_SHAPE_متن_DIRECTION_LOGICAL,
                                            جھوٹ);
            توڑو;
        حالت U_SHAPE_DIGITS_ALEN2AN_INIT_AL:
            shape_مقصد_Arabic_Digits_With_Context(dest, dest_لمبائی,
                                            digit_بنیاد,
                                            (options & U_SHAPE_متن_DIRECTION_نقاب) == U_SHAPE_متن_DIRECTION_LOGICAL,
                                            سچ);
            توڑو;
        طےشدہ:
            /* will never occur because of validity checks above */
            توڑو;
        }
    }

    واپس u_terminateUChars(dest, dest_استعداد, dest_لمبائی, p_Error_رمز);
}
