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

#تعریف_کړه IRRELEVANT 4
#تعریف_کړه LAMTYPE    16
#تعریف_کړه ALEFTYPE   32
#تعریف_کړه LINKR      1
#تعریف_کړه LINKL      2
#تعریف_کړه APRESENT   8
#تعریف_کړه SHADDA     64
#تعریف_کړه CSHADDA    128
#تعریف_کړه COMBINE    (SHADDA+CSHADDA)

#تعریف_کړه HAMZAFE_CHAR       0xfe80
#تعریف_کړه HAMZA06_CHAR       0x0621
#تعریف_کړه YEH_HAMZA_CHAR     0x0626
#تعریف_کړه YEH_HAMZAFE_CHAR   0xFE89
#تعریف_کړه LAMALEF_SPACE_SUB  0xFFFF
#تعریف_کړه TASHKEEL_SPACE_SUB 0xFFFE
#تعریف_کړه NEW_لکۍ_CHAR      0xFE73
#تعریف_کړه OLD_لکۍ_CHAR      0x200B
#تعریف_کړه LAM_CHAR           0x0644
#تعریف_کړه SPACE_CHAR         0x0020
#تعریف_کړه SHADDA_CHAR        0xFE7C
#تعریف_کړه TATWEEL_CHAR       0x0640
#تعریف_کړه SHADDA_TATWEEL_CHAR  0xFE7D
#تعریف_کړه SHADDA06_CHAR      0x0651

#تعریف_کړه SHAPE_ډېرپېښ   0
#تعریف_کړه DESHAPE_ډېرپېښ 1

بنية uShapeVariables {
     char16_t لکۍ_Char;
     uint32_t uShapeLamalefBegin;
     uint32_t u_Shape_Lamalef_پای_ټکی;
     uint32_t uShapeTashkeelBegin;
     uint32_t u_Shape_Tashkeel_پای_ټکی;
     صحيح spaces_Relative_منزل_متن_Begin_پای_ټکی;
};

ساكن ثابت uint8_t لکۍ_Family_Isolated_Final[] = {
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

ساكن ثابت char16_t yeh_Hamza_منزل_Yeh[] =
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

ساكن ثابت uint8_t shape_جدول[4][4][4]=
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
shape_منزل_Arabic_Digits_With_Context(char16_t *s, int32_t length,
                                char16_t digit_بنسټ,
                                UBool isLogical, UBool وروستی_Strong_Was_AL) {
    int32_t i;
    char16_t c;

    digit_بنسټ-=0x30;

    /* the iteration direction depends on the type of input */
    إذا(isLogical) {
        لكل(i=0; i<length; ++i) {
            c=s[i];
            بدل(ubidi_getClass(c)) {
            حالة U_کیڼ_منزل_ښی: /* L */
            حالة U_ښی_منزل_کیڼ: /* R */
                وروستی_Strong_Was_AL=خطأ;
                اكسر;
            حالة U_ښی_منزل_کیڼ_ARABIC: /* AL */
                وروستی_Strong_Was_AL=ریښتیا;
                اكسر;
            حالة U_EUROPEAN_شمېره: /* EN */
                إذا (وروستی_Strong_Was_AL && static_cast<uint32_t>(c - 0x30) < 10) {
                    s[i] = static_cast<char16_t>(digit_بنسټ + c); /* digitBase+(c-0x30) - digitBase was modified above */
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
            حالة U_کیڼ_منزل_ښی: /* L */
            حالة U_ښی_منزل_کیڼ: /* R */
                وروستی_Strong_Was_AL=خطأ;
                اكسر;
            حالة U_ښی_منزل_کیڼ_ARABIC: /* AL */
                وروستی_Strong_Was_AL=ریښتیا;
                اكسر;
            حالة U_EUROPEAN_شمېره: /* EN */
                إذا (وروستی_Strong_Was_AL && static_cast<uint32_t>(c - 0x30) < 10) {
                    s[i] = static_cast<char16_t>(digit_بنسټ + c); /* digitBase+(c-0x30) - digitBase was modified above */
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
invert_بفر(char16_t *بفر, int32_t size, uint32_t /*options*/, int32_t lowlimit, int32_t highlimit) {
    char16_t لنډمهاله;
    int32_t i=0,j=0;
    لكل(i=lowlimit,j=size-highlimit-1;i<j;i++,j--) {
        لنډمهاله = بفر[i];
        بفر[i] = بفر[j];
        بفر[j] = لنډمهاله;
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
    إذا(ch >= 0x0622 && ch <= 0x06D3) {
        أرجع(araLink[ch-0x0622]);
    } وإلا إذا(ch == 0x200D) {
        أرجع(3);
    } وإلا إذا(ch >= 0x206D && ch <= 0x206F) {
        أرجع(4);
    }وإلا إذا(ch >= 0xFB50 && ch <= 0xFC62) {
        أرجع(presALink[ch-0xFB50]);
    } وإلا إذا(ch >= 0xFE70 && ch <= 0xFEFC) {
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
شمېر_Spaces(char16_t *dest, int32_t size, uint32_t /*options*/, int32_t *spacesCountl, int32_t *spacesCountr) {
    int32_t i = 0;
    int32_t countl = 0,countr = 0;
    طالما((dest[i] == SPACE_CHAR) && (countl < size)) {
       countl++;
       i++;
    }
    إذا (countl < size) {  /* the entire buffer is not all space */
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
is_لکۍ_Char(char16_t ch) {
    إذا(ch == OLD_لکۍ_CHAR || ch == NEW_لکۍ_CHAR){
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
is_کتل_شوی_لکۍ_Family_Char(char16_t ch) {
    إذا(ch >= 0xfeb1 && ch < 0xfebf){
            أرجع لکۍ_Family_Isolated_Final [ch - 0xFEB1];
    }وإلا{
            أرجع 0;
    }
}

 /* Name     : isSeenFamilyChar
  * Function : returns 1 if the character is a seen family character in the Unicode
  *            06 range otherwise returns 0
 */

ساكن مضمن int32_t
is_کتل_شوی_Family_Char(char16_t  ch){
    إذا(ch >= 0x633 && ch <= 0x636){
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
    إذا((ch==0xFE89)||(ch==0xFE8A)){
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
    إذا(ch >= 0xfe70 && ch <= 0xfe7f && ch != NEW_لکۍ_CHAR && ch != 0xFE75 && ch != SHADDA_TATWEEL_CHAR)
    {
        أرجع tashkeelMedial [ch - 0xFE70];
    }وإلا إذا( (ch >= 0xfcf2 && ch <= 0xfcf4) || (ch == SHADDA_TATWEEL_CHAR)) {
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
    إذا(ch >= 0xfe70 && ch <= 0xfe7f && ch != NEW_لکۍ_CHAR && ch != 0xFE75){
        أرجع (1 - tashkeelMedial [ch - 0xFE70]);
    }وإلا إذا(ch >= 0xfc5e && ch <= 0xfc63){
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
calculate_اندازه(ثابت char16_t *سرچینه, int32_t سرچینه_اوږدتیا,
int32_t dest_اندازه,uint32_t options) {
    int32_t i = 0;

    صحيح lamAlefOption = 0;
    صحيح tashkeelOption = 0;

    dest_اندازه = سرچینه_اوږدتیا;

    إذا (((options&U_SHAPE_LETTERS_ماسک) == U_SHAPE_LETTERS_SHAPE ||
        ((options&U_SHAPE_LETTERS_ماسک) == U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED )) &&
        ((options&U_SHAPE_LAMALEF_ماسک) == U_SHAPE_LAMALEF_RESIZE )){
            lamAlefOption = 1;
    }
    إذا((options&U_SHAPE_LETTERS_ماسک) == U_SHAPE_LETTERS_SHAPE &&
       ((options&U_SHAPE_TASHKEEL_ماسک) == U_SHAPE_TASHKEEL_RESIZE ) ){
            tashkeelOption = 1;
        }

    إذا(lamAlefOption || tashkeelOption){
        إذا((options&U_SHAPE_متن_DIRECTION_ماسک)==U_SHAPE_متن_DIRECTION_VISUAL_LTR) {
            لكل(i=0;i<سرچینه_اوږدتیا;i++) {
                إذا( ((isAlefChar(سرچینه[i]))&& (i<(سرچینه_اوږدتیا-1)) &&(سرچینه[i+1] == LAM_CHAR)) || (isTashkeelCharFE(سرچینه[i])) ) {
                        dest_اندازه--;
                    }
                }
            }وإلا إذا((options&U_SHAPE_متن_DIRECTION_ماسک)==U_SHAPE_متن_DIRECTION_LOGICAL) {
                لكل(i=0;i<سرچینه_اوږدتیا;i++) {
                    إذا( ( (سرچینه[i] == LAM_CHAR) && (i<(سرچینه_اوږدتیا-1)) && (isAlefChar(سرچینه[i+1]))) || (isTashkeelCharFE(سرچینه[i])) ) {
                        dest_اندازه--;
                    }
                }
            }
        }

    إذا ((options&U_SHAPE_LETTERS_ماسک) == U_SHAPE_LETTERS_UNSHAPE){
        إذا ( (options&U_SHAPE_LAMALEF_ماسک) == U_SHAPE_LAMALEF_RESIZE){
            لكل(i=0;i<سرچینه_اوږدتیا;i++) {
                إذا(isLamAlefChar(سرچینه[i]))
                dest_اندازه++;
            }
        }
    }

    أرجع dest_اندازه;
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
handleTashkeelWithTatweel(char16_t *dest, int32_t سرچینه_اوږدتیا,
             int32_t /*destSize*/, uint32_t /*options*/,
             UError_کوډ * /*pErrorCode*/) {
                 صحيح i;
                 لكل(i = 0; i < سرچینه_اوږدتیا; i++){
                     إذا((isTashkeelOnTatweelChar(dest[i]) == 1)){
                         dest[i] = TATWEEL_CHAR;
                    }وإلا إذا((isTashkeelOnTatweelChar(dest[i]) == 2)){
                         dest[i] = SHADDA_TATWEEL_CHAR;
                    }وإلا إذا(isIsolatedTashkeelChar(dest[i]) && dest[i] != SHADDA_CHAR){
                         dest[i] = SPACE_CHAR;
                    }
                 }
                 أرجع سرچینه_اوږدتیا;
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
handleGeneratedSpaces(char16_t *dest, int32_t سرچینه_اوږدتیا,
                    int32_t dest_اندازه,
                    uint32_t options,
                    UError_کوډ *p_Error_کوډ,بنية uShapeVariables shapeVars ) {

    int32_t i = 0, j = 0;
    int32_t count = 0;
    char16_t *tempbuffer=تش_اشاره;

    صحيح lamAlefOption = 0;
    صحيح tashkeelOption = 0;
    صحيح shaping_ډېرپېښ = SHAPE_ډېرپېښ;

    إذا (shaping_ډېرپېښ == 0){
        إذا ( (options&U_SHAPE_LAMALEF_ماسک) == U_SHAPE_LAMALEF_RESIZE ){
            lamAlefOption = 1;
        }
        إذا ( (options&U_SHAPE_TASHKEEL_ماسک) == U_SHAPE_TASHKEEL_RESIZE ){
            tashkeelOption = 1;
        }
    }

    إذا (static_cast<size_t>(سرچینه_اوږدتیا) + 1 > std::numeric_limits<size_t>::max() / U_SIZEOF_UCHAR) {
        *p_Error_کوډ = U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }
    tempbuffer = static_cast<char16_t*>(uprv_malloc((سرچینه_اوږدتیا + 1) * U_SIZEOF_UCHAR));
    /* Test for nullptr */
    إذا(tempbuffer == تش_اشاره) {
        *p_Error_کوډ = U_حافظه_ویش_ERROR;
        أرجع 0;
    }


    إذا (lamAlefOption || tashkeelOption){
        uprv_memset(tempbuffer, 0, (سرچینه_اوږدتیا+1)*U_SIZEOF_UCHAR);

        i = j = 0; count = 0;
        طالما(i < سرچینه_اوږدتیا) {
            إذا ( (lamAlefOption && dest[i] == LAMALEF_SPACE_SUB) ||
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

        u_memcpy(dest, tempbuffer, سرچینه_اوږدتیا);
        dest_اندازه = u_strlen(dest);
    }

      lamAlefOption = 0;

    إذا (shaping_ډېرپېښ == 0){
        إذا ( (options&U_SHAPE_LAMALEF_ماسک) == U_SHAPE_LAMALEF_NEAR ){
            lamAlefOption = 1;
        }
    }

    إذا (lamAlefOption){
        /* Lam+Alef is already shaped into LamAlef + FFFF */
        i = 0;
        طالما(i < سرچینه_اوږدتیا) {
            إذا(lamAlefOption&&dest[i] == LAMALEF_SPACE_SUB){
                dest[i] = SPACE_CHAR;
            }
            i++;
        }
        dest_اندازه = سرچینه_اوږدتیا;
    }
    lamAlefOption = 0;
    tashkeelOption = 0;

    إذا (shaping_ډېرپېښ == 0) {
        إذا ( ((options&U_SHAPE_LAMALEF_ماسک) == shapeVars.uShapeLamalefBegin) ||
              (((options&U_SHAPE_LAMALEF_ماسک) == U_SHAPE_LAMALEF_AUTO )
              && (shapeVars.spaces_Relative_منزل_متن_Begin_پای_ټکی==1)) ) {
            lamAlefOption = 1;
        }
        إذا ( (options&U_SHAPE_TASHKEEL_ماسک) == shapeVars.uShapeTashkeelBegin ) {
            tashkeelOption = 1;
        }
    }

    إذا(lamAlefOption || tashkeelOption){
        uprv_memset(tempbuffer, 0, (سرچینه_اوږدتیا+1)*U_SIZEOF_UCHAR);
        
        i = j = سرچینه_اوږدتیا; count = 0;
        
        طالما(i >= 0) {
            إذا ( (lamAlefOption && dest[i] == LAMALEF_SPACE_SUB) ||
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

        u_memcpy(dest, tempbuffer, سرچینه_اوږدتیا);
        dest_اندازه = سرچینه_اوږدتیا;
    }



    lamAlefOption = 0;
    tashkeelOption = 0;

    إذا (shaping_ډېرپېښ == 0) {
        إذا ( ((options&U_SHAPE_LAMALEF_ماسک) == shapeVars.u_Shape_Lamalef_پای_ټکی) ||
              (((options&U_SHAPE_LAMALEF_ماسک) == U_SHAPE_LAMALEF_AUTO )
              && (shapeVars.spaces_Relative_منزل_متن_Begin_پای_ټکی==0)) ) {
            lamAlefOption = 1;
        }
        إذا ( (options&U_SHAPE_TASHKEEL_ماسک) == shapeVars.u_Shape_Tashkeel_پای_ټکی ){
            tashkeelOption = 1;
        }
    }

    إذا(lamAlefOption || tashkeelOption){
        uprv_memset(tempbuffer, 0, (سرچینه_اوږدتیا+1)*U_SIZEOF_UCHAR);

        i = j = 0; count = 0;
        طالما(i < سرچینه_اوږدتیا) {
            إذا ( (lamAlefOption && dest[i] == LAMALEF_SPACE_SUB) ||
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

        u_memcpy(dest, tempbuffer, سرچینه_اوږدتیا);
        dest_اندازه = سرچینه_اوږدتیا;
    }


    إذا(tempbuffer){
        uprv_free(tempbuffer);
    }

    أرجع dest_اندازه;
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
expandCompositCharAtBegin(char16_t *dest, int32_t سرچینه_اوږدتیا, int32_t dest_اندازه,UError_کوډ *p_Error_کوډ) {
    int32_t      i = 0,j = 0;
    int32_t      countl = 0;
    char16_t *tempbuffer=تش_اشاره;

    tempbuffer = static_cast<char16_t*>(uprv_malloc((سرچینه_اوږدتیا + 1) * U_SIZEOF_UCHAR));

    /* Test for nullptr */
    إذا(tempbuffer == تش_اشاره) {
        *p_Error_کوډ = U_حافظه_ویش_ERROR;
        أرجع 0;
    }

        uprv_memset(tempbuffer, 0, (سرچینه_اوږدتیا+1)*U_SIZEOF_UCHAR);

        i = 0;
        طالما(dest[i] == SPACE_CHAR) {
            countl++;
            i++;
        }

        i = j = سرچینه_اوږدتیا-1;

        طالما(i >= 0 && j >= 0) {
            إذا( countl>0 && isLamAlefChar(dest[i])) {
                tempbuffer[j] = LAM_CHAR;
                /* to ensure the array index is within the range */
                U_ASSERT(dest[i] >= 0xFEF5u
                    && dest[i]-0xFEF5u < UPRV_LENGTHOF(convertLamAlef));
                tempbuffer[j-1] = convertLamAlef[ dest[i] - 0xFEF5 ];
                j--;
                countl--;
            }وإلا {
                 إذا( countl == 0 && isLamAlefChar(dest[i]) ) {
                     *p_Error_کوډ=U_NO_SPACE_شته;
                     }
                 tempbuffer[j] = dest[i];
            }
            i--;
            j--;
        }
        u_memcpy(dest, tempbuffer, سرچینه_اوږدتیا);

        uprv_free(tempbuffer);

        dest_اندازه = سرچینه_اوږدتیا;
        أرجع dest_اندازه;
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
expand_Composit_Char_At_پای_ټکی(char16_t *dest, int32_t سرچینه_اوږدتیا, int32_t dest_اندازه,UError_کوډ *p_Error_کوډ) {
    int32_t      i = 0,j = 0;

    int32_t      countr = 0;
    int32_t  inpsize = سرچینه_اوږدتیا;

    char16_t *tempbuffer=تش_اشاره;
    tempbuffer = static_cast<char16_t*>(uprv_malloc((سرچینه_اوږدتیا + 1) * U_SIZEOF_UCHAR));

    /* Test for nullptr */
    إذا(tempbuffer == تش_اشاره) {
        *p_Error_کوډ = U_حافظه_ویش_ERROR;
         أرجع 0;
    }

    uprv_memset(tempbuffer, 0, (سرچینه_اوږدتیا+1)*U_SIZEOF_UCHAR);

    طالما(inpsize>0 && dest[inpsize-1] == SPACE_CHAR) {
        countr++;
        inpsize--;
    }

    i = سرچینه_اوږدتیا - countr - 1;
    j = سرچینه_اوږدتیا - 1;

    طالما(i >= 0 && j >= 0) {
        إذا( countr>0 && isLamAlefChar(dest[i]) ) {
            tempbuffer[j] = LAM_CHAR;
            tempbuffer[j-1] = convertLamAlef[ dest[i] - 0xFEF5 ];
            j--;
            countr--;
        }وإلا {
            إذا ((countr == 0) && isLamAlefChar(dest[i]) ) {
                *p_Error_کوډ=U_NO_SPACE_شته;
            }
            tempbuffer[j] = dest[i];
        }
        i--;
        j--;
    }

    إذا(countr > 0) {
        u_memmove(tempbuffer, tempbuffer+countr, سرچینه_اوږدتیا);
        إذا(u_strlen(tempbuffer) < سرچینه_اوږدتیا) {
            لكل(i=سرچینه_اوږدتیا-1;i>=سرچینه_اوږدتیا-countr;i--) {
                tempbuffer[i] = SPACE_CHAR;
            }
        }
    }
    u_memcpy(dest, tempbuffer, سرچینه_اوږدتیا);

    uprv_free(tempbuffer);

    dest_اندازه = سرچینه_اوږدتیا;
    أرجع dest_اندازه;
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
expandCompositCharAtNear(char16_t *dest, int32_t سرچینه_اوږدتیا, int32_t dest_اندازه,UError_کوډ *p_Error_کوډ,
                         صحيح yehHamzaOption, صحيح کتل_شوی_لکۍ_Option, صحيح lamAlefOption, بنية uShapeVariables shapeVars) {
    int32_t      i = 0;


    char16_t lamalefChar, yehhamzaChar;

    لكل(i = 0 ;i<=سرچینه_اوږدتیا-1;i++) {
            إذا (کتل_شوی_لکۍ_Option && is_کتل_شوی_لکۍ_Family_Char(dest[i])) {
                إذا ((i>0) && (dest[i-1] == SPACE_CHAR) ) {
                    dest[i-1] = shapeVars.لکۍ_Char;
                }وإلا {
                    *p_Error_کوډ=U_NO_SPACE_شته;
                }
            }وإلا إذا(yehHamzaOption && (isYehHamzaChar(dest[i])) ) {
                إذا ((i>0) && (dest[i-1] == SPACE_CHAR) ) {
                    yehhamzaChar = dest[i];
                    dest[i] = yeh_Hamza_منزل_Yeh[yehhamzaChar - YEH_HAMZAFE_CHAR];
                    dest[i-1] = HAMZAFE_CHAR;
                }وإلا {

                    *p_Error_کوډ=U_NO_SPACE_شته;
                }
            }وإلا إذا(lamAlefOption && isLamAlefChar(dest[i+1])) {
                إذا(dest[i] == SPACE_CHAR){
                    lamalefChar = dest[i+1];
                    dest[i+1] = LAM_CHAR;
                    dest[i] = convertLamAlef[ lamalefChar - 0xFEF5 ];
                }وإلا {
                    *p_Error_کوډ=U_NO_SPACE_شته;
                }
            }
       }
       dest_اندازه = سرچینه_اوږدتیا;
       أرجع dest_اندازه;
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
expandCompositChar(char16_t *dest, int32_t سرچینه_اوږدتیا,
              int32_t dest_اندازه,uint32_t options,
              UError_کوډ *p_Error_کوډ, صحيح shaping_ډېرپېښ,بنية uShapeVariables shapeVars) {

    int32_t      i = 0,j = 0;

    char16_t *tempbuffer=تش_اشاره;
    صحيح yehHamzaOption = 0;
    صحيح کتل_شوی_لکۍ_Option = 0;
    صحيح lamAlefOption = 0;

    إذا (shaping_ډېرپېښ == 1){
        إذا ( (options&U_SHAPE_LAMALEF_ماسک) == U_SHAPE_LAMALEF_AUTO){

            إذا(shapeVars.spaces_Relative_منزل_متن_Begin_پای_ټکی == 0) {
                dest_اندازه = expand_Composit_Char_At_پای_ټکی(dest, سرچینه_اوږدتیا, dest_اندازه, p_Error_کوډ);

                إذا(*p_Error_کوډ == U_NO_SPACE_شته) {
                    *p_Error_کوډ = U_ZERO_ERROR;
                    dest_اندازه = expandCompositCharAtBegin(dest, سرچینه_اوږدتیا, dest_اندازه, p_Error_کوډ);
                }
            }وإلا {
                dest_اندازه = expandCompositCharAtBegin(dest, سرچینه_اوږدتیا, dest_اندازه, p_Error_کوډ);

                إذا(*p_Error_کوډ == U_NO_SPACE_شته) {
                    *p_Error_کوډ = U_ZERO_ERROR;
                    dest_اندازه = expand_Composit_Char_At_پای_ټکی(dest, سرچینه_اوږدتیا, dest_اندازه, p_Error_کوډ);
                }
            }

            إذا(*p_Error_کوډ == U_NO_SPACE_شته) {
                *p_Error_کوډ = U_ZERO_ERROR;
                dest_اندازه = expandCompositCharAtNear(dest, سرچینه_اوږدتیا, dest_اندازه, p_Error_کوډ, yehHamzaOption,
                                                کتل_شوی_لکۍ_Option, 1,shapeVars);
            }
        }
    }

    إذا (shaping_ډېرپېښ == 1){
        إذا ( (options&U_SHAPE_LAMALEF_ماسک) == shapeVars.u_Shape_Lamalef_پای_ټکی){
            dest_اندازه = expand_Composit_Char_At_پای_ټکی(dest, سرچینه_اوږدتیا, dest_اندازه, p_Error_کوډ);
        }
    }

    إذا (shaping_ډېرپېښ == 1){
        إذا ( (options&U_SHAPE_LAMALEF_ماسک) == shapeVars.uShapeLamalefBegin){
            dest_اندازه = expandCompositCharAtBegin(dest, سرچینه_اوږدتیا, dest_اندازه, p_Error_کوډ);
        }
    }

    إذا (shaping_ډېرپېښ == 0){
         إذا ((options&U_SHAPE_YEHHAMZA_ماسک) == U_SHAPE_YEHHAMZA_TWOCELL_NEAR){
             yehHamzaOption = 1;
         }
         إذا ((options&U_SHAPE_کتل_شوی_ماسک) == U_SHAPE_کتل_شوی_TWOCELL_NEAR){
            کتل_شوی_لکۍ_Option = 1;
         }
    }
    إذا (shaping_ډېرپېښ == 1) {
        إذا ( (options&U_SHAPE_LAMALEF_ماسک) == U_SHAPE_LAMALEF_NEAR) {
            lamAlefOption = 1;
        }
    }


    إذا (yehHamzaOption || کتل_شوی_لکۍ_Option || lamAlefOption){
        dest_اندازه = expandCompositCharAtNear(dest, سرچینه_اوږدتیا, dest_اندازه, p_Error_کوډ, yehHamzaOption,
                                            کتل_شوی_لکۍ_Option,lamAlefOption,shapeVars);
    }


    إذا (shaping_ډېرپېښ == 1){
        إذا ( (options&U_SHAPE_LAMALEF_ماسک) == U_SHAPE_LAMALEF_RESIZE){
            dest_اندازه = calculate_اندازه(dest,سرچینه_اوږدتیا,dest_اندازه,options);
            tempbuffer = static_cast<char16_t*>(uprv_malloc((dest_اندازه + 1) * U_SIZEOF_UCHAR));

            /* Test for nullptr */
            إذا(tempbuffer == تش_اشاره) {
                *p_Error_کوډ = U_حافظه_ویش_ERROR;
                أرجع 0;
            }

            uprv_memset(tempbuffer, 0, (dest_اندازه+1)*U_SIZEOF_UCHAR);

            i = j = 0;
            طالما(i < dest_اندازه && j < dest_اندازه) {
                إذا(isLamAlefChar(dest[i]) ) {
                    tempbuffer[j] = convertLamAlef[ dest[i] - 0xFEF5 ];
                    tempbuffer[j+1] = LAM_CHAR;
                    j++;
                }وإلا {
                    tempbuffer[j] = dest[i];
                }
                i++;
                j++;
            }

            u_memcpy(dest, tempbuffer, dest_اندازه);
        }
    }

    إذا(tempbuffer) {
        uprv_free(tempbuffer);
    }
    أرجع dest_اندازه;
}

/*
 *Name     : shapeUnicode
 *Function : Converts an Arabic Unicode buffer in 06xx Range into a shaped
 *           arabic Unicode buffer in FExx Range
 */
ساكن int32_t
shapeUnicode(char16_t *dest, int32_t سرچینه_اوږدتیا,
             int32_t dest_اندازه,uint32_t options,
             UError_کوډ *p_Error_کوډ,
             صحيح tashkeelFlag, بنية uShapeVariables shapeVars) {

    int32_t          i, iend;
    int32_t          ګام;
    int32_t          وروستی_Pos,Nx, Nw;
    بینښه صحيح     Shape;
    int32_t          lamalef_وموندل = 0;
    int32_t seenfam_وموندل = 0, yehhamza_وموندل =0, tashkeel_وموندل  = 0;
    char16_t         prevLink = 0, وروستی_Link = 0, currLink, بل_Link = 0;
    char16_t         wLamalef;

    /*
     * Converts the input buffer from FExx Range into 06xx Range
     * to make sure that all characters are in the 06xx range
     * even the lamalef is converted to the special region in
     * the 06xx range
     */
    إذا ((options & U_SHAPE_PRESERVE_PRESENTATION_ماسک)  == U_SHAPE_PRESERVE_PRESENTATION_NOOP) {
        لكل (i = 0; i < سرچینه_اوږدتیا; i++) {
            char16_t ننوت_Char  = dest[i];
            إذا ( (ننوت_Char >= 0xFB50) && (ننوت_Char <= 0xFBFF)) {
                char16_t c = convertFBto06 [ (ننوت_Char - 0xFB50) ];
                إذا (c != 0)
                    dest[i] = c;
            } وإلا إذا ( (ننوت_Char >= 0xFE70) && (ننوت_Char <= 0xFEFC)) {
                dest[i] = convertFEto06 [ (ننوت_Char - 0xFE70) ] ;
            } وإلا {
                dest[i] = ننوت_Char ;
            }
        }
    }


    /* sets the index to the end of the buffer, together with the step point to -1 */
    i = سرچینه_اوږدتیا - 1;
    iend = -1;
    ګام = -1;

    /*
     * This function resolves the link between the characters .
     * Arabic characters have four forms :
     * Isolated Form, Initial Form, Middle Form and Final Form
     */
    currLink = getLink(dest[i]);

    وروستی_Pos = i;
    Nx = -2, Nw = 0;

    طالما (i != iend) {
        /* If high byte of currLink > 0 then more than one shape */
        إذا ((currLink & 0xFF00) > 0 || (getLink(dest[i]) & IRRELEVANT) != 0) {
            Nw = i + ګام;
            طالما (Nx < 0) {         /* we need to know about next char */
                إذا(Nw == iend) {
                    بل_Link = 0;
                    Nx = 3000;
                } وإلا {
                    بل_Link = getLink(dest[Nw]);
                    إذا((بل_Link & IRRELEVANT) == 0) {
                        Nx = Nw;
                    } وإلا {
                        Nw = Nw + ګام;
                    }
                }
            }

            إذا ( ((currLink & ALEFTYPE) > 0)  &&  ((وروستی_Link & LAMTYPE) > 0) ) {
                lamalef_وموندل = 1;
                wLamalef = changeLamAlef(dest[i]); /*get from 0x065C-0x065f */
                إذا ( wLamalef != 0) {
                    dest[i] = LAMALEF_SPACE_SUB;            /* The default case is to drop the Alef and replace */
                    dest[وروستی_Pos] =wLamalef;     /* it by LAMALEF_SPACE_SUB which is the last character in the  */
                    i=وروستی_Pos;                   /* unicode private use area, this is done to make   */
                }                                /* sure that removeLamAlefSpaces() handles only the */
                وروستی_Link = prevLink;             /* spaces generated during lamalef generation.      */
                currLink = getLink(wLamalef);    /* LAMALEF_SPACE_SUB is added here and is replaced by spaces   */
            }                                    /* in removeLamAlefSpaces()                         */

            إذا ((i > 0) && (dest[i-1] == SPACE_CHAR)){
                إذا ( is_کتل_شوی_Family_Char(dest[i])) {
                    seenfam_وموندل = 1;
                } وإلا إذا (dest[i] == YEH_HAMZA_CHAR) {
                    yehhamza_وموندل = 1;
                }
            }
            وإلا إذا(i==0){
                إذا ( is_کتل_شوی_Family_Char(dest[i])){
                    seenfam_وموندل = 1;
                } وإلا إذا (dest[i] == YEH_HAMZA_CHAR) {
                    yehhamza_وموندل = 1;
                }
            }

            /*
             * get the proper shape according to link ability of neighbors
             * and of character; depends on the order of the shapes
             * (isolated, initial, middle, final) in the compatibility area
             */
            Shape = shape_جدول[بل_Link & (LINKR + LINKL)]
                              [وروستی_Link & (LINKR + LINKL)]
                              [currLink & (LINKR + LINKL)];

            إذا ((currLink & (LINKR+LINKL)) == 1) {
                Shape &= 1;
            } وإلا إذا(isTashkeelChar(dest[i])) {
                إذا( (وروستی_Link & LINKL) && (بل_Link & LINKR) && (tashkeelFlag == 1) &&
                     dest[i] != 0x064C && dest[i] != 0x064D )
                {
                    Shape = 1;
                    إذا( (بل_Link&ALEFTYPE) == ALEFTYPE && (وروستی_Link&LAMTYPE) == LAMTYPE ) {
                        Shape = 0;
                    }
                } وإلا إذا(tashkeelFlag == 2 && dest[i] == SHADDA06_CHAR){
                    Shape = 1;
                } وإلا {
                    Shape = 0;
                }
            }
            إذا ((dest[i] ^ 0x0600) < 0x100) {
                إذا ( isTashkeelChar(dest[i]) ){
                    إذا (tashkeelFlag == 2  && dest[i] != SHADDA06_CHAR){
                        dest[i] = TASHKEEL_SPACE_SUB;
                        tashkeel_وموندل  = 1;
                    } وإلا {
                        /* to ensure the array index is within the range */
                        U_ASSERT(dest[i] >= 0x064Bu
                            && dest[i]-0x064Bu < UPRV_LENGTHOF(IrrelevantPos));
                        dest[i] =  0xFE70 + IrrelevantPos[(dest[i] - 0x064B)] + static_cast<char16_t>(Shape);
                    }
                }وإلا إذا ((currLink & APRESENT) > 0) {
                    dest[i] = static_cast<char16_t>(0xFB50 + (currLink >> 8) + Shape);
                }وإلا إذا ((currLink >> 8) > 0 && (currLink & IRRELEVANT) == 0) {
                    dest[i] = static_cast<char16_t>(0xFE70 + (currLink >> 8) + Shape);
                }
            }
        }

        /* move one notch forward */
        إذا ((currLink & IRRELEVANT) == 0) {
            prevLink = وروستی_Link;
            وروستی_Link = currLink;
            وروستی_Pos = i;
        }

        i = i + ګام;
        إذا (i == Nx) {
            currLink = بل_Link;
            Nx = -2;
        } وإلا إذا(i != iend) {
            currLink = getLink(dest[i]);
        }
    }
    dest_اندازه = سرچینه_اوږدتیا;
    إذا ( (lamalef_وموندل != 0 ) || (tashkeel_وموندل  != 0) ){
        dest_اندازه = handleGeneratedSpaces(dest,سرچینه_اوږدتیا,dest_اندازه,options,p_Error_کوډ, shapeVars);
    }

    إذا ( (seenfam_وموندل != 0) || (yehhamza_وموندل != 0) ) {
        dest_اندازه = expandCompositChar(dest, سرچینه_اوږدتیا,dest_اندازه,options,p_Error_کوډ, SHAPE_ډېرپېښ,shapeVars);
    }
    أرجع dest_اندازه;
}

/*
 *Name     : deShapeUnicode
 *Function : Converts an Arabic Unicode buffer in FExx Range into unshaped
 *           arabic Unicode buffer in 06xx Range
 */
ساكن int32_t
deShapeUnicode(char16_t *dest, int32_t سرچینه_اوږدتیا,
               int32_t dest_اندازه,uint32_t options,
               UError_کوډ *p_Error_کوډ, بنية uShapeVariables shapeVars) {
    int32_t i = 0;
    int32_t lamalef_وموندل = 0;
    int32_t yehHamzaComposeEnabled = 0;
    int32_t کتل_شوی_Compose_Enabled = 0;

    yehHamzaComposeEnabled = ((options&U_SHAPE_YEHHAMZA_ماسک) == U_SHAPE_YEHHAMZA_TWOCELL_NEAR) ? 1 : 0;
    کتل_شوی_Compose_Enabled = ((options&U_SHAPE_کتل_شوی_ماسک) == U_SHAPE_کتل_شوی_TWOCELL_NEAR)? 1 : 0;

    /*
     *This for loop changes the buffer from the Unicode FE range to
     *the Unicode 06 range
     */

    لكل(i = 0; i < سرچینه_اوږدتیا; i++) {
        char16_t  ننوت_Char = dest[i];
        إذا ( (ننوت_Char >= 0xFB50) && (ننوت_Char <= 0xFBFF)) { /* FBxx Arabic range */
            char16_t c = convertFBto06 [ (ننوت_Char - 0xFB50) ];
            إذا (c != 0)
                dest[i] = c;
        } وإلا إذا( (yehHamzaComposeEnabled == 1) && ((ننوت_Char == HAMZA06_CHAR) || (ننوت_Char == HAMZAFE_CHAR))
                && (i < (سرچینه_اوږدتیا - 1)) && isAlefMaksouraChar(dest[i+1] )) {
                 dest[i] = SPACE_CHAR;
                 dest[i+1] = YEH_HAMZA_CHAR;
        } وإلا إذا ( (کتل_شوی_Compose_Enabled == 1) && (is_لکۍ_Char(ننوت_Char)) && (i< (سرچینه_اوږدتیا - 1))
                        && (is_کتل_شوی_لکۍ_Family_Char(dest[i+1])) ) {
                dest[i] = SPACE_CHAR;
        } وإلا إذا (( ننوت_Char >= 0xFE70) && (ننوت_Char <= 0xFEF4 )) { /* FExx Arabic range */
                dest[i] = convertFEto06 [ (ننوت_Char - 0xFE70) ];
        } وإلا {
            dest[i] = ننوت_Char ;
        }

        إذا( isLamAlefChar(dest[i]) )
            lamalef_وموندل = 1;
    }

   dest_اندازه = سرچینه_اوږدتیا;
   إذا (lamalef_وموندل != 0){
          dest_اندازه = expandCompositChar(dest,سرچینه_اوږدتیا,dest_اندازه,options,p_Error_کوډ,DESHAPE_ډېرپېښ, shapeVars);
   }
   أرجع dest_اندازه;
}

/*
 ****************************************
 * u_shapeArabic
 ****************************************
 */

U_CAPI int32_t U_EXPORT2
u_shapeArabic(ثابت char16_t *سرچینه, int32_t سرچینه_اوږدتیا,
              char16_t *dest, int32_t dest_ظرفیت,
              uint32_t options,
              UError_کوډ *p_Error_کوډ) {

    int32_t dest_اوږدتیا;
    بنية  uShapeVariables shapeVars = { OLD_لکۍ_CHAR,U_SHAPE_LAMALEF_BEGIN,U_SHAPE_LAMALEF_پای_ټکی,U_SHAPE_TASHKEEL_BEGIN,U_SHAPE_TASHKEEL_پای_ټکی,0};

    /* usual error checking */
    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        أرجع 0;
    }

    /* make sure that no reserved options values are used; allow dest==nullptr only for preflighting */
    إذا( سرچینه==تش_اشاره || سرچینه_اوږدتیا<-1 || (dest==تش_اشاره && dest_ظرفیت!=0) || dest_ظرفیت<0 ||
                (((options&U_SHAPE_TASHKEEL_ماسک) > 0) &&
                 ((options&U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED) == U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED) ) ||
                (((options&U_SHAPE_TASHKEEL_ماسک) > 0) &&
                 ((options&U_SHAPE_LETTERS_ماسک) == U_SHAPE_LETTERS_UNSHAPE)) ||
                (options&U_SHAPE_DIGIT_TYPE_RESERVED)==U_SHAPE_DIGIT_TYPE_RESERVED ||
                (options&U_SHAPE_DIGITS_ماسک)==U_SHAPE_DIGITS_RESERVED ||
                ((options&U_SHAPE_LAMALEF_ماسک) != U_SHAPE_LAMALEF_RESIZE  &&
                (options&U_SHAPE_AGGREGATE_TASHKEEL_ماسک) != 0) ||
                ((options&U_SHAPE_AGGREGATE_TASHKEEL_ماسک) == U_SHAPE_AGGREGATE_TASHKEEL &&
                (options&U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED) != U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED)
    )
    {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }
    /* Validate  lamalef options */
    إذا(((options&U_SHAPE_LAMALEF_ماسک) > 0)&&
              !(((options & U_SHAPE_LAMALEF_ماسک)==U_SHAPE_LAMALEF_BEGIN) ||
                ((options & U_SHAPE_LAMALEF_ماسک)==U_SHAPE_LAMALEF_پای_ټکی ) ||
                ((options & U_SHAPE_LAMALEF_ماسک)==U_SHAPE_LAMALEF_RESIZE )||
                 ((options & U_SHAPE_LAMALEF_ماسک)==U_SHAPE_LAMALEF_AUTO) ||
                 ((options & U_SHAPE_LAMALEF_ماسک)==U_SHAPE_LAMALEF_NEAR)))
    {
         *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }
    /* Validate  Tashkeel options */
    إذا(((options&U_SHAPE_TASHKEEL_ماسک) > 0)&&
                   !(((options & U_SHAPE_TASHKEEL_ماسک)==U_SHAPE_TASHKEEL_BEGIN) ||
                     ((options & U_SHAPE_TASHKEEL_ماسک)==U_SHAPE_TASHKEEL_پای_ټکی )
                    ||((options & U_SHAPE_TASHKEEL_ماسک)==U_SHAPE_TASHKEEL_RESIZE )||
                    ((options & U_SHAPE_TASHKEEL_ماسک)==U_SHAPE_TASHKEEL_REPLACE_BY_TATWEEL)))
    {
         *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }
    /* determine the source length */
    إذا(سرچینه_اوږدتیا==-1) {
        سرچینه_اوږدتیا=u_strlen(سرچینه);
    }
    إذا(سرچینه_اوږدتیا<=0) {
        أرجع u_terminateUChars(dest, dest_ظرفیت, 0, p_Error_کوډ);
    }

    /* check that source and destination do not overlap */
    إذا( dest!=تش_اشاره &&
        ((سرچینه<=dest && dest<سرچینه+سرچینه_اوږدتیا) ||
         (dest<=سرچینه && سرچینه<dest+dest_ظرفیت))) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    /* Does Options contain the new Seen Tail Unicode code point option */
    إذا ( (options&U_SHAPE_لکۍ_TYPE_ماسک) == U_SHAPE_لکۍ_NEW_UNICODE){
        shapeVars.لکۍ_Char = NEW_لکۍ_CHAR;
    }وإلا {
        shapeVars.لکۍ_Char = OLD_لکۍ_CHAR;
    }

    إذا((options&U_SHAPE_LETTERS_ماسک)!=U_SHAPE_LETTERS_NOOP) {
        char16_t بفر[300];
        char16_t *tempbuffer, *tempsource = تش_اشاره;
        int32_t output_اندازه, spacesCountl=0, spacesCountr=0;

        إذا((options&U_SHAPE_AGGREGATE_TASHKEEL_ماسک)>0) {
            int32_t logical_ترتیب = (options&U_SHAPE_متن_DIRECTION_ماسک) == U_SHAPE_متن_DIRECTION_LOGICAL;
            int32_t aggregate_tashkeel =
                        (options&(U_SHAPE_AGGREGATE_TASHKEEL_ماسک+U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED)) ==
                        (U_SHAPE_AGGREGATE_TASHKEEL+U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED);
            صحيح ګام=logical_ترتیب?1:-1;
            صحيح j=logical_ترتیب?-1:2*سرچینه_اوږدتیا;
            صحيح i=logical_ترتیب?-1:سرچینه_اوږدتیا;
            صحيح پای_ټکی=logical_ترتیب?سرچینه_اوږدتیا:-1;
            صحيح aggregation_possible = 1;
            char16_t prev = 0;
            char16_t prevLink, currLink = 0;
            صحيح new_سرچینه_اوږدتیا = 0;
            tempsource = (char16_t *)uprv_malloc(2*سرچینه_اوږدتیا*U_SIZEOF_UCHAR);
            إذا(tempsource == تش_اشاره) {
                *p_Error_کوډ = U_حافظه_ویش_ERROR;
                أرجع 0;
            }

            طالما ((i+=ګام) != پای_ټکی) {
                prevLink = currLink;
                currLink = getLink(سرچینه[i]);
                إذا (aggregate_tashkeel && ((prevLink|currLink)&COMBINE) == COMBINE && aggregation_possible) {
                    إذا (j<0 || j>=2*سرچینه_اوږدتیا) {
                        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
                        إذا (tempsource != تش_اشاره) uprv_free(tempsource);
                        أرجع 0;
                    }
                    aggregation_possible = 0;
                    tempsource[j] = (prev<سرچینه[i]?prev:سرچینه[i])-0x064C+0xFC5E;
                    currLink = getLink(tempsource[j]);
                } وإلا {
                    aggregation_possible = 1;
                    tempsource[j+=ګام] = سرچینه[i];
                    prev = سرچینه[i];
                    new_سرچینه_اوږدتیا++;
                }
            }
            سرچینه = tempsource+(logical_ترتیب?0:j);
            سرچینه_اوږدتیا = new_سرچینه_اوږدتیا;
        }

        /* calculate destination size */
        /* TODO: do we ever need to do this pure preflighting? */
        إذا(((options&U_SHAPE_LAMALEF_ماسک)==U_SHAPE_LAMALEF_RESIZE) ||
           ((options&U_SHAPE_TASHKEEL_ماسک)==U_SHAPE_TASHKEEL_RESIZE)) {
            output_اندازه=calculate_اندازه(سرچینه,سرچینه_اوږدتیا,dest_ظرفیت,options);
        } وإلا {
            output_اندازه=سرچینه_اوږدتیا;
        }

        إذا(output_اندازه>dest_ظرفیت) {
            *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
                إذا (tempsource != تش_اشاره) uprv_free(tempsource);
            أرجع output_اندازه;
        }

        /*
         * need a temporary buffer of size max(outputSize, sourceLength)
         * because at first we copy source->temp
         */
        إذا(سرچینه_اوږدتیا>output_اندازه) {
            output_اندازه=سرچینه_اوږدتیا;
        }

        /* Start of Arabic letter shaping part */
        إذا(output_اندازه<=UPRV_LENGTHOF(بفر)) {
            output_اندازه=UPRV_LENGTHOF(بفر);
            tempbuffer=بفر;
        } وإلا {
            tempbuffer = (char16_t *)uprv_malloc(output_اندازه*U_SIZEOF_UCHAR);

            /*Test for nullptr*/
            إذا(tempbuffer == تش_اشاره) {
                *p_Error_کوډ = U_حافظه_ویش_ERROR;
                إذا (tempsource != تش_اشاره) uprv_free(tempsource);
                أرجع 0;
            }
        }
        u_memcpy(tempbuffer, سرچینه, سرچینه_اوږدتیا);
        إذا (tempsource != تش_اشاره){
            uprv_free(tempsource);
        }

        إذا(سرچینه_اوږدتیا<output_اندازه) {
            uprv_memset(tempbuffer+سرچینه_اوږدتیا, 0, (output_اندازه-سرچینه_اوږدتیا)*U_SIZEOF_UCHAR);
        }

        إذا((options&U_SHAPE_متن_DIRECTION_ماسک) == U_SHAPE_متن_DIRECTION_LOGICAL) {
            شمېر_Spaces(tempbuffer,سرچینه_اوږدتیا,options,&spacesCountl,&spacesCountr);
            invert_بفر(tempbuffer,سرچینه_اوږدتیا,options,spacesCountl,spacesCountr);
        }

        إذا((options&U_SHAPE_متن_DIRECTION_ماسک) == U_SHAPE_متن_DIRECTION_VISUAL_LTR) {
            إذا((options&U_SHAPE_SPACES_RELATIVE_منزل_متن_ماسک) == U_SHAPE_SPACES_RELATIVE_منزل_متن_BEGIN_پای_ټکی) {
                shapeVars.spaces_Relative_منزل_متن_Begin_پای_ټکی = 1;
                shapeVars.uShapeLamalefBegin = U_SHAPE_LAMALEF_پای_ټکی;
                shapeVars.u_Shape_Lamalef_پای_ټکی = U_SHAPE_LAMALEF_BEGIN;
                shapeVars.uShapeTashkeelBegin = U_SHAPE_TASHKEEL_پای_ټکی;
                shapeVars.u_Shape_Tashkeel_پای_ټکی = U_SHAPE_TASHKEEL_BEGIN;
            }
        }

        بدل(options&U_SHAPE_LETTERS_ماسک) {
        حالة U_SHAPE_LETTERS_SHAPE :
             إذا( (options&U_SHAPE_TASHKEEL_ماسک)> 0
                 && ((options&U_SHAPE_TASHKEEL_ماسک) !=U_SHAPE_TASHKEEL_REPLACE_BY_TATWEEL)) {
                /* Call the shaping function with tashkeel flag == 2 for removal of tashkeel */
                dest_اوږدتیا = shapeUnicode(tempbuffer,سرچینه_اوږدتیا,dest_ظرفیت,options,p_Error_کوډ,2,shapeVars);
             }وإلا {
                /* default Call the shaping function with tashkeel flag == 1 */
                dest_اوږدتیا = shapeUnicode(tempbuffer,سرچینه_اوږدتیا,dest_ظرفیت,options,p_Error_کوډ,1,shapeVars);

                /*After shaping text check if user wants to remove tashkeel and replace it with tatweel*/
                إذا( (options&U_SHAPE_TASHKEEL_ماسک) == U_SHAPE_TASHKEEL_REPLACE_BY_TATWEEL){
                  dest_اوږدتیا = handleTashkeelWithTatweel(tempbuffer,dest_اوږدتیا,dest_ظرفیت,options,p_Error_کوډ);
                }
            }
            اكسر;
        حالة U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED :
            /* Call the shaping function with tashkeel flag == 0 */
            dest_اوږدتیا = shapeUnicode(tempbuffer,سرچینه_اوږدتیا,dest_ظرفیت,options,p_Error_کوډ,0,shapeVars);
            اكسر;

        حالة U_SHAPE_LETTERS_UNSHAPE :
            /* Call the deshaping function */
            dest_اوږدتیا = deShapeUnicode(tempbuffer,سرچینه_اوږدتیا,dest_ظرفیت,options,p_Error_کوډ,shapeVars);
            اكسر;
        افتراضي :
            /* will never occur because of validity checks above */
            dest_اوږدتیا = 0;
            اكسر;
        }

        /*
         * TODO: (markus 2002aug01)
         * For as long as we always preflight the outputSize above
         * we should U_ASSERT(outputSize==destLength)
         * except for the adjustment above before the tempbuffer allocation
         */

        إذا((options&U_SHAPE_متن_DIRECTION_ماسک) == U_SHAPE_متن_DIRECTION_LOGICAL) {
            شمېر_Spaces(tempbuffer,dest_اوږدتیا,options,&spacesCountl,&spacesCountr);
            invert_بفر(tempbuffer,dest_اوږدتیا,options,spacesCountl,spacesCountr);
        }
        u_memcpy(dest, tempbuffer, uprv_min(dest_اوږدتیا, dest_ظرفیت));

        إذا(tempbuffer!=بفر) {
            uprv_free(tempbuffer);
        }

        إذا(dest_اوږدتیا>dest_ظرفیت) {
            *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
            أرجع dest_اوږدتیا;
        }

        /* End of Arabic letter shaping part */
    } وإلا {
        /*
         * No letter shaping:
         * just make sure the destination is large enough and copy the string.
         */
        إذا(dest_ظرفیت<سرچینه_اوږدتیا) {
            /* this catches preflighting, too */
            *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
            أرجع سرچینه_اوږدتیا;
        }
        u_memcpy(dest, سرچینه, سرچینه_اوږدتیا);
        dest_اوږدتیا=سرچینه_اوږدتیا;
    }

    /*
     * Perform number shaping.
     * With UTF-16 or UTF-32, the length of the string is constant.
     * The easiest way to do this is to operate on the destination and
     * "shape" the digits in-place.
     */
    إذا((options&U_SHAPE_DIGITS_ماسک)!=U_SHAPE_DIGITS_NOOP) {
        char16_t digit_بنسټ;
        int32_t i;

        /* select the requested digit group */
        بدل(options&U_SHAPE_DIGIT_TYPE_ماسک) {
        حالة U_SHAPE_DIGIT_TYPE_AN:
            digit_بنسټ=0x660; /* Unicode: "Arabic-Indic digits" */
            اكسر;
        حالة U_SHAPE_DIGIT_TYPE_AN_EXTENDED:
            digit_بنسټ=0x6f0; /* Unicode: "Eastern Arabic-Indic digits (Persian and Urdu)" */
            اكسر;
        افتراضي:
            /* will never occur because of validity checks above */
            digit_بنسټ=0;
            اكسر;
        }

        /* perform the requested operation */
        بدل(options&U_SHAPE_DIGITS_ماسک) {
        حالة U_SHAPE_DIGITS_EN2AN:
            /* add (digitBase-'0') to each European (ASCII) digit code point */
            digit_بنسټ-=0x30;
            لكل(i=0; i<dest_اوږدتیا; ++i) {
                إذا(((uint32_t)dest[i]-0x30)<10) {
                    dest[i]+=digit_بنسټ;
                }
            }
            اكسر;
        حالة U_SHAPE_DIGITS_AN2EN:
            /* subtract (digitBase-'0') from each Arabic digit code point */
            لكل(i=0; i<dest_اوږدتیا; ++i) {
                إذا(((uint32_t)dest[i]-(uint32_t)digit_بنسټ)<10) {
                    dest[i]-=digit_بنسټ-0x30;
                }
            }
            اكسر;
        حالة U_SHAPE_DIGITS_ALEN2AN_INIT_LR:
            shape_منزل_Arabic_Digits_With_Context(dest, dest_اوږدتیا,
                                            digit_بنسټ,
                                            (options & U_SHAPE_متن_DIRECTION_ماسک) == U_SHAPE_متن_DIRECTION_LOGICAL,
                                            خطأ);
            اكسر;
        حالة U_SHAPE_DIGITS_ALEN2AN_INIT_AL:
            shape_منزل_Arabic_Digits_With_Context(dest, dest_اوږدتیا,
                                            digit_بنسټ,
                                            (options & U_SHAPE_متن_DIRECTION_ماسک) == U_SHAPE_متن_DIRECTION_LOGICAL,
                                            ریښتیا);
            اكسر;
        افتراضي:
            /* will never occur because of validity checks above */
            اكسر;
        }
    }

    أرجع u_terminateUChars(dest, dest_ظرفیت, dest_اوږدتیا, p_Error_کوډ);
}
