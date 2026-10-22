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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/uchar.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/ushape.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "putilimp.h"
#अन्तर्भाव "ustr_imp.h"
#अन्तर्भाव "ubidi_props.h"
#अन्तर्भाव "uassert.h"

#अन्तर्भाव <limits>
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

#परिभाषय IRRELEVANT 4
#परिभाषय LAMTYPE    16
#परिभाषय ALEFTYPE   32
#परिभाषय LINKR      1
#परिभाषय LINKL      2
#परिभाषय APRESENT   8
#परिभाषय SHADDA     64
#परिभाषय CSHADDA    128
#परिभाषय COMBINE    (SHADDA+CSHADDA)

#परिभाषय HAMZAFE_CHAR       0xfe80
#परिभाषय HAMZA06_CHAR       0x0621
#परिभाषय YEH_HAMZA_CHAR     0x0626
#परिभाषय YEH_HAMZAFE_CHAR   0xFE89
#परिभाषय LAMALEF_SPACE_SUB  0xFFFF
#परिभाषय TASHKEEL_SPACE_SUB 0xFFFE
#परिभाषय NEW_पुच्छ_CHAR      0xFE73
#परिभाषय OLD_पुच्छ_CHAR      0x200B
#परिभाषय LAM_CHAR           0x0644
#परिभाषय SPACE_CHAR         0x0020
#परिभाषय SHADDA_CHAR        0xFE7C
#परिभाषय TATWEEL_CHAR       0x0640
#परिभाषय SHADDA_TATWEEL_CHAR  0xFE7D
#परिभाषय SHADDA06_CHAR      0x0651

#परिभाषय SHAPE_बहुलकम्   0
#परिभाषय DESHAPE_बहुलकम् 1

संरचना uShapeVariables {
     char16_t पुच्छ_Char;
     uint32_t uShapeLamalefBegin;
     uint32_t u_Shape_Lamalef_अन्तः;
     uint32_t uShapeTashkeelBegin;
     uint32_t u_Shape_Tashkeel_अन्तः;
     पूर्णाङ्क spaces_Relative_गन्तव्यम्_Text_Begin_अन्तः;
};

स्थिर ध्रुव uint8_t पुच्छ_Family_Isolated_Final[] = {
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

स्थिर ध्रुव uint8_t tashkeelMedial[] = {
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

स्थिर ध्रुव char16_t yeh_Hamza_गन्तव्यम्_Yeh[] =
{
/* isolated*/ 0xFEEF,
/* final   */ 0xFEF0
};

स्थिर ध्रुव uint8_t IrrelevantPos[] = {
    0x0, 0x2, 0x4, 0x6,
    0x8, 0xA, 0xC, 0xE
};


स्थिर ध्रुव char16_t convertLamAlef[] =
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

स्थिर ध्रुव char16_t araLink[178]=
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

स्थिर ध्रुव uint8_t presALink[] = {
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

स्थिर ध्रुव uint8_t presBLink[]=
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

स्थिर ध्रुव char16_t convertFBto06[] =
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

स्थिर ध्रुव char16_t convertFEto06[] =
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

स्थिर ध्रुव uint8_t shape_सारणी[4][4][4]=
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
स्थिर शून्य
shape_गन्तव्यम्_Arabic_Digits_With_Context(char16_t *s, int32_t length,
                                char16_t digit_आधारः,
                                UBool isLogical, UBool चरमः_Strong_Was_AL) {
    int32_t i;
    char16_t c;

    digit_आधारः-=0x30;

    /* the iteration direction depends on the type of input */
    यदि(isLogical) {
        प्रति(i=0; i<length; ++i) {
            c=s[i];
            विकल्प(ubidi_getClass(c)) {
            पक्ष U_वामः_गन्तव्यम्_दक्षिणः: /* L */
            पक्ष U_दक्षिणः_गन्तव्यम्_वामः: /* R */
                चरमः_Strong_Was_AL=असत्य;
                भङ्ग;
            पक्ष U_दक्षिणः_गन्तव्यम्_वामः_ARABIC: /* AL */
                चरमः_Strong_Was_AL=सत्य;
                भङ्ग;
            पक्ष U_EUROPEAN_सङ्ख्या: /* EN */
                यदि (चरमः_Strong_Was_AL && static_cast<uint32_t>(c - 0x30) < 10) {
                    s[i] = static_cast<char16_t>(digit_आधारः + c); /* digitBase+(c-0x30) - digitBase was modified above */
                }
                भङ्ग;
            स्वभाव :
                भङ्ग;
            }
        }
    } अन्यथा {
        प्रति(i=length; i>0; /* pre-decrement in the body */) {
            c=s[--i];
            विकल्प(ubidi_getClass(c)) {
            पक्ष U_वामः_गन्तव्यम्_दक्षिणः: /* L */
            पक्ष U_दक्षिणः_गन्तव्यम्_वामः: /* R */
                चरमः_Strong_Was_AL=असत्य;
                भङ्ग;
            पक्ष U_दक्षिणः_गन्तव्यम्_वामः_ARABIC: /* AL */
                चरमः_Strong_Was_AL=सत्य;
                भङ्ग;
            पक्ष U_EUROPEAN_सङ्ख्या: /* EN */
                यदि (चरमः_Strong_Was_AL && static_cast<uint32_t>(c - 0x30) < 10) {
                    s[i] = static_cast<char16_t>(digit_आधारः + c); /* digitBase+(c-0x30) - digitBase was modified above */
                }
                भङ्ग;
            स्वभाव :
                भङ्ग;
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
स्थिर शून्य
invert_बफर(char16_t *बफर, int32_t size, uint32_t /*options*/, int32_t lowlimit, int32_t highlimit) {
    char16_t अस्थायी;
    int32_t i=0,j=0;
    प्रति(i=lowlimit,j=size-highlimit-1;i<j;i++,j--) {
        अस्थायी = बफर[i];
        बफर[i] = बफर[j];
        बफर[j] = अस्थायी;
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
स्थिर अन्तःस्थ char16_t
changeLamAlef(char16_t ch) {
    विकल्प(ch) {
    पक्ष 0x0622 :
        प्रत्यागम 0x065C;
    पक्ष 0x0623 :
        प्रत्यागम 0x065D;
    पक्ष 0x0625 :
        प्रत्यागम 0x065E;
    पक्ष 0x0627 :
        प्रत्यागम 0x065F;
    }
    प्रत्यागम 0;
}

/*
 *Name     : getLink
 *Function : Resolves the link between the characters as
 *           Arabic characters have four forms :
 *           Isolated, Initial, Middle and Final Form
 */
स्थिर char16_t
getLink(char16_t ch) {
    यदि(ch >= 0x0622 && ch <= 0x06D3) {
        प्रत्यागम(araLink[ch-0x0622]);
    } अन्यथा यदि(ch == 0x200D) {
        प्रत्यागम(3);
    } अन्यथा यदि(ch >= 0x206D && ch <= 0x206F) {
        प्रत्यागम(4);
    }अन्यथा यदि(ch >= 0xFB50 && ch <= 0xFC62) {
        प्रत्यागम(presALink[ch-0xFB50]);
    } अन्यथा यदि(ch >= 0xFE70 && ch <= 0xFEFC) {
        प्रत्यागम(presBLink[ch-0xFE70]);
    }अन्यथा {
        प्रत्यागम(0);
    }
}

/*
 *Name     : countSpaces
 *Function : Counts the number of spaces
 *           at each end of the logical buffer
 */
स्थिर शून्य
गणनम्_Spaces(char16_t *dest, int32_t size, uint32_t /*options*/, int32_t *spacesCountl, int32_t *spacesCountr) {
    int32_t i = 0;
    int32_t countl = 0,countr = 0;
    यावत्((dest[i] == SPACE_CHAR) && (countl < size)) {
       countl++;
       i++;
    }
    यदि (countl < size) {  /* the entire buffer is not all space */
        यावत्(dest[size-1] == SPACE_CHAR) {
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
स्थिर अन्तःस्थ int32_t
isTashkeelChar(char16_t ch) {
    प्रत्यागम static_cast<int32_t>(ch >= 0x064B && ch <= 0x0652);
}

/*
 *Name     : isTashkeelCharFE
 *Function : Returns 1 for Tashkeel characters in FE range else return 0
 */
स्थिर अन्तःस्थ int32_t
isTashkeelCharFE(char16_t ch) {
    प्रत्यागम static_cast<int32_t>(ch >= 0xFE70 && ch <= 0xFE7F);
}

/*
 *Name     : isAlefChar
 *Function : Returns 1 for Alef characters else return 0
 */
स्थिर अन्तःस्थ int32_t
isAlefChar(char16_t ch) {
    प्रत्यागम static_cast<int32_t>(ch == 0x0622 || ch == 0x0623 || ch == 0x0625 || ch == 0x0627);
}

/*
 *Name     : isLamAlefChar
 *Function : Returns 1 for LamAlef characters else return 0
 */
स्थिर अन्तःस्थ int32_t
isLamAlefChar(char16_t ch) {
    प्रत्यागम static_cast<int32_t>(ch >= 0xFEF5 && ch <= 0xFEFC);
}

/*BIDI
 *Name     : isTailChar
 *Function : returns 1 if the character matches one of the tail characters (0xfe73 or 0x200b) otherwise returns 0
 */

स्थिर अन्तःस्थ int32_t
is_पुच्छ_Char(char16_t ch) {
    यदि(ch == OLD_पुच्छ_CHAR || ch == NEW_पुच्छ_CHAR){
            प्रत्यागम 1;
    }अन्यथा{
            प्रत्यागम 0;
    }
}

/*BIDI
 *Name     : isSeenTailFamilyChar
 *Function : returns 1 if the character is a seen family isolated character
 *           in the FE range otherwise returns 0
 */

स्थिर अन्तःस्थ int32_t
is_आलोकितम्_पुच्छ_Family_Char(char16_t ch) {
    यदि(ch >= 0xfeb1 && ch < 0xfebf){
            प्रत्यागम पुच्छ_Family_Isolated_Final [ch - 0xFEB1];
    }अन्यथा{
            प्रत्यागम 0;
    }
}

 /* Name     : isSeenFamilyChar
  * Function : returns 1 if the character is a seen family character in the Unicode
  *            06 range otherwise returns 0
 */

स्थिर अन्तःस्थ int32_t
is_आलोकितम्_Family_Char(char16_t  ch){
    यदि(ch >= 0x633 && ch <= 0x636){
        प्रत्यागम 1;
    }अन्यथा {
        प्रत्यागम 0;
    }
}

/*Start of BIDI*/
/*
 *Name     : isAlefMaksouraChar
 *Function : returns 1 if the character is a Alef Maksoura Final or isolated
 *           otherwise returns 0
 */
स्थिर अन्तःस्थ int32_t
isAlefMaksouraChar(char16_t ch) {
    प्रत्यागम static_cast<int32_t>(ch == 0xFEEF || ch == 0xFEF0 || ch == 0x0649);
}

/*
 * Name     : isYehHamzaChar
 * Function : returns 1 if the character is a yehHamza isolated or yehhamza
 *            final is found otherwise returns 0
 */
स्थिर अन्तःस्थ int32_t
isYehHamzaChar(char16_t ch) {
    यदि((ch==0xFE89)||(ch==0xFE8A)){
        प्रत्यागम 1;
    }अन्यथा{
        प्रत्यागम 0;
    }
}

 /*
 * Name: isTashkeelOnTatweelChar
 * Function: Checks if the Tashkeel Character is on Tatweel or not,if the
 *           Tashkeel on tatweel (FE range), it returns 1 else if the
 *           Tashkeel with shadda on tatweel (FC range)return 2 otherwise
 *           returns 0
 */
स्थिर अन्तःस्थ int32_t
isTashkeelOnTatweelChar(char16_t ch){
    यदि(ch >= 0xfe70 && ch <= 0xfe7f && ch != NEW_पुच्छ_CHAR && ch != 0xFE75 && ch != SHADDA_TATWEEL_CHAR)
    {
        प्रत्यागम tashkeelMedial [ch - 0xFE70];
    }अन्यथा यदि( (ch >= 0xfcf2 && ch <= 0xfcf4) || (ch == SHADDA_TATWEEL_CHAR)) {
        प्रत्यागम 2;
    }अन्यथा{
        प्रत्यागम 0;
    }
}

/*
 * Name: isIsolatedTashkeelChar
 * Function: Checks if the Tashkeel Character is in the isolated form
 *           (i.e. Unicode FE range) returns 1 else if the Tashkeel
 *           with shadda is in the isolated form (i.e. Unicode FC range)
 *           returns 2 otherwise returns 0
 */
स्थिर अन्तःस्थ int32_t
isIsolatedTashkeelChar(char16_t ch){
    यदि(ch >= 0xfe70 && ch <= 0xfe7f && ch != NEW_पुच्छ_CHAR && ch != 0xFE75){
        प्रत्यागम (1 - tashkeelMedial [ch - 0xFE70]);
    }अन्यथा यदि(ch >= 0xfc5e && ch <= 0xfc63){
        प्रत्यागम 1;
    }अन्यथा{
        प्रत्यागम 0;
    }
}




/*
 *Name     : calculateSize
 *Function : This function calculates the destSize to be used in preflighting
 *           when the destSize is equal to 0
 *           It is used also to calculate the new destsize in case the
 *           destination buffer will be resized.
 */

स्थिर int32_t
calculate_आकार(ध्रुव char16_t *source, int32_t source_दीर्घता,
int32_t dest_आकार,uint32_t options) {
    int32_t i = 0;

    पूर्णाङ्क lamAlefOption = 0;
    पूर्णाङ्क tashkeelOption = 0;

    dest_आकार = source_दीर्घता;

    यदि (((options&U_SHAPE_LETTERS_आवरणम्) == U_SHAPE_LETTERS_SHAPE ||
        ((options&U_SHAPE_LETTERS_आवरणम्) == U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED )) &&
        ((options&U_SHAPE_LAMALEF_आवरणम्) == U_SHAPE_LAMALEF_RESIZE )){
            lamAlefOption = 1;
    }
    यदि((options&U_SHAPE_LETTERS_आवरणम्) == U_SHAPE_LETTERS_SHAPE &&
       ((options&U_SHAPE_TASHKEEL_आवरणम्) == U_SHAPE_TASHKEEL_RESIZE ) ){
            tashkeelOption = 1;
        }

    यदि(lamAlefOption || tashkeelOption){
        यदि((options&U_SHAPE_TEXT_DIRECTION_आवरणम्)==U_SHAPE_TEXT_DIRECTION_VISUAL_LTR) {
            प्रति(i=0;i<source_दीर्घता;i++) {
                यदि( ((isAlefChar(source[i]))&& (i<(source_दीर्घता-1)) &&(source[i+1] == LAM_CHAR)) || (isTashkeelCharFE(source[i])) ) {
                        dest_आकार--;
                    }
                }
            }अन्यथा यदि((options&U_SHAPE_TEXT_DIRECTION_आवरणम्)==U_SHAPE_TEXT_DIRECTION_LOGICAL) {
                प्रति(i=0;i<source_दीर्घता;i++) {
                    यदि( ( (source[i] == LAM_CHAR) && (i<(source_दीर्घता-1)) && (isAlefChar(source[i+1]))) || (isTashkeelCharFE(source[i])) ) {
                        dest_आकार--;
                    }
                }
            }
        }

    यदि ((options&U_SHAPE_LETTERS_आवरणम्) == U_SHAPE_LETTERS_UNSHAPE){
        यदि ( (options&U_SHAPE_LAMALEF_आवरणम्) == U_SHAPE_LAMALEF_RESIZE){
            प्रति(i=0;i<source_दीर्घता;i++) {
                यदि(isLamAlefChar(source[i]))
                dest_आकार++;
            }
        }
    }

    प्रत्यागम dest_आकार;
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
स्थिर int32_t
handleTashkeelWithTatweel(char16_t *dest, int32_t source_दीर्घता,
             int32_t /*destSize*/, uint32_t /*options*/,
             UError_संहिता * /*pErrorCode*/) {
                 पूर्णाङ्क i;
                 प्रति(i = 0; i < source_दीर्घता; i++){
                     यदि((isTashkeelOnTatweelChar(dest[i]) == 1)){
                         dest[i] = TATWEEL_CHAR;
                    }अन्यथा यदि((isTashkeelOnTatweelChar(dest[i]) == 2)){
                         dest[i] = SHADDA_TATWEEL_CHAR;
                    }अन्यथा यदि(isIsolatedTashkeelChar(dest[i]) && dest[i] != SHADDA_CHAR){
                         dest[i] = SPACE_CHAR;
                    }
                 }
                 प्रत्यागम source_दीर्घता;
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

स्थिर int32_t
handleGeneratedSpaces(char16_t *dest, int32_t source_दीर्घता,
                    int32_t dest_आकार,
                    uint32_t options,
                    UError_संहिता *p_Error_संहिता,संरचना uShapeVariables shapeVars ) {

    int32_t i = 0, j = 0;
    int32_t count = 0;
    char16_t *tempbuffer=शून्यसूचकः;

    पूर्णाङ्क lamAlefOption = 0;
    पूर्णाङ्क tashkeelOption = 0;
    पूर्णाङ्क shaping_बहुलकम् = SHAPE_बहुलकम्;

    यदि (shaping_बहुलकम् == 0){
        यदि ( (options&U_SHAPE_LAMALEF_आवरणम्) == U_SHAPE_LAMALEF_RESIZE ){
            lamAlefOption = 1;
        }
        यदि ( (options&U_SHAPE_TASHKEEL_आवरणम्) == U_SHAPE_TASHKEEL_RESIZE ){
            tashkeelOption = 1;
        }
    }

    यदि (static_cast<size_t>(source_दीर्घता) + 1 > std::numeric_limits<size_t>::max() / U_SIZEOF_UCHAR) {
        *p_Error_संहिता = U_सूचकांक_OUTOFBOUNDS_ERROR;
        प्रत्यागम 0;
    }
    tempbuffer = static_cast<char16_t*>(uprv_malloc((source_दीर्घता + 1) * U_SIZEOF_UCHAR));
    /* Test for nullptr */
    यदि(tempbuffer == शून्यसूचकः) {
        *p_Error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम 0;
    }


    यदि (lamAlefOption || tashkeelOption){
        uprv_memset(tempbuffer, 0, (source_दीर्घता+1)*U_SIZEOF_UCHAR);

        i = j = 0; count = 0;
        यावत्(i < source_दीर्घता) {
            यदि ( (lamAlefOption && dest[i] == LAMALEF_SPACE_SUB) ||
               (tashkeelOption && dest[i] == TASHKEEL_SPACE_SUB) ){
                j--;
                count++;
            } अन्यथा {
                tempbuffer[j] = dest[i];
            }
            i++;
            j++;
        }

        यावत्(count >= 0) {
            tempbuffer[i] = 0x0000;
            i--;
            count--;
        }

        u_memcpy(dest, tempbuffer, source_दीर्घता);
        dest_आकार = u_strlen(dest);
    }

      lamAlefOption = 0;

    यदि (shaping_बहुलकम् == 0){
        यदि ( (options&U_SHAPE_LAMALEF_आवरणम्) == U_SHAPE_LAMALEF_NEAR ){
            lamAlefOption = 1;
        }
    }

    यदि (lamAlefOption){
        /* Lam+Alef is already shaped into LamAlef + FFFF */
        i = 0;
        यावत्(i < source_दीर्घता) {
            यदि(lamAlefOption&&dest[i] == LAMALEF_SPACE_SUB){
                dest[i] = SPACE_CHAR;
            }
            i++;
        }
        dest_आकार = source_दीर्घता;
    }
    lamAlefOption = 0;
    tashkeelOption = 0;

    यदि (shaping_बहुलकम् == 0) {
        यदि ( ((options&U_SHAPE_LAMALEF_आवरणम्) == shapeVars.uShapeLamalefBegin) ||
              (((options&U_SHAPE_LAMALEF_आवरणम्) == U_SHAPE_LAMALEF_AUTO )
              && (shapeVars.spaces_Relative_गन्तव्यम्_Text_Begin_अन्तः==1)) ) {
            lamAlefOption = 1;
        }
        यदि ( (options&U_SHAPE_TASHKEEL_आवरणम्) == shapeVars.uShapeTashkeelBegin ) {
            tashkeelOption = 1;
        }
    }

    यदि(lamAlefOption || tashkeelOption){
        uprv_memset(tempbuffer, 0, (source_दीर्घता+1)*U_SIZEOF_UCHAR);
        
        i = j = source_दीर्घता; count = 0;
        
        यावत्(i >= 0) {
            यदि ( (lamAlefOption && dest[i] == LAMALEF_SPACE_SUB) ||
                 (tashkeelOption && dest[i] == TASHKEEL_SPACE_SUB) ){
                j++;
                count++;
            }अन्यथा {
                tempbuffer[j] = dest[i];
            }
            i--;
            j--;
        }

        प्रति(i=0 ;i < count; i++){
                tempbuffer[i] = SPACE_CHAR;
        }

        u_memcpy(dest, tempbuffer, source_दीर्घता);
        dest_आकार = source_दीर्घता;
    }



    lamAlefOption = 0;
    tashkeelOption = 0;

    यदि (shaping_बहुलकम् == 0) {
        यदि ( ((options&U_SHAPE_LAMALEF_आवरणम्) == shapeVars.u_Shape_Lamalef_अन्तः) ||
              (((options&U_SHAPE_LAMALEF_आवरणम्) == U_SHAPE_LAMALEF_AUTO )
              && (shapeVars.spaces_Relative_गन्तव्यम्_Text_Begin_अन्तः==0)) ) {
            lamAlefOption = 1;
        }
        यदि ( (options&U_SHAPE_TASHKEEL_आवरणम्) == shapeVars.u_Shape_Tashkeel_अन्तः ){
            tashkeelOption = 1;
        }
    }

    यदि(lamAlefOption || tashkeelOption){
        uprv_memset(tempbuffer, 0, (source_दीर्घता+1)*U_SIZEOF_UCHAR);

        i = j = 0; count = 0;
        यावत्(i < source_दीर्घता) {
            यदि ( (lamAlefOption && dest[i] == LAMALEF_SPACE_SUB) ||
                 (tashkeelOption && dest[i] == TASHKEEL_SPACE_SUB) ){
                j--;
                count++;
            }अन्यथा {
                tempbuffer[j] = dest[i];
            }
            i++;
            j++;
        }

        यावत्(count >= 0) {
            tempbuffer[i] = SPACE_CHAR;
            i--;
            count--;
        }

        u_memcpy(dest, tempbuffer, source_दीर्घता);
        dest_आकार = source_दीर्घता;
    }


    यदि(tempbuffer){
        uprv_free(tempbuffer);
    }

    प्रत्यागम dest_आकार;
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

स्थिर int32_t
expandCompositCharAtBegin(char16_t *dest, int32_t source_दीर्घता, int32_t dest_आकार,UError_संहिता *p_Error_संहिता) {
    int32_t      i = 0,j = 0;
    int32_t      countl = 0;
    char16_t *tempbuffer=शून्यसूचकः;

    tempbuffer = static_cast<char16_t*>(uprv_malloc((source_दीर्घता + 1) * U_SIZEOF_UCHAR));

    /* Test for nullptr */
    यदि(tempbuffer == शून्यसूचकः) {
        *p_Error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम 0;
    }

        uprv_memset(tempbuffer, 0, (source_दीर्घता+1)*U_SIZEOF_UCHAR);

        i = 0;
        यावत्(dest[i] == SPACE_CHAR) {
            countl++;
            i++;
        }

        i = j = source_दीर्घता-1;

        यावत्(i >= 0 && j >= 0) {
            यदि( countl>0 && isLamAlefChar(dest[i])) {
                tempbuffer[j] = LAM_CHAR;
                /* to ensure the array index is within the range */
                U_ASSERT(dest[i] >= 0xFEF5u
                    && dest[i]-0xFEF5u < UPRV_LENGTHOF(convertLamAlef));
                tempbuffer[j-1] = convertLamAlef[ dest[i] - 0xFEF5 ];
                j--;
                countl--;
            }अन्यथा {
                 यदि( countl == 0 && isLamAlefChar(dest[i]) ) {
                     *p_Error_संहिता=U_NO_SPACE_उपलब्धम्;
                     }
                 tempbuffer[j] = dest[i];
            }
            i--;
            j--;
        }
        u_memcpy(dest, tempbuffer, source_दीर्घता);

        uprv_free(tempbuffer);

        dest_आकार = source_दीर्घता;
        प्रत्यागम dest_आकार;
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

स्थिर int32_t
expand_Composit_Char_At_अन्तः(char16_t *dest, int32_t source_दीर्घता, int32_t dest_आकार,UError_संहिता *p_Error_संहिता) {
    int32_t      i = 0,j = 0;

    int32_t      countr = 0;
    int32_t  inpsize = source_दीर्घता;

    char16_t *tempbuffer=शून्यसूचकः;
    tempbuffer = static_cast<char16_t*>(uprv_malloc((source_दीर्घता + 1) * U_SIZEOF_UCHAR));

    /* Test for nullptr */
    यदि(tempbuffer == शून्यसूचकः) {
        *p_Error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
         प्रत्यागम 0;
    }

    uprv_memset(tempbuffer, 0, (source_दीर्घता+1)*U_SIZEOF_UCHAR);

    यावत्(inpsize>0 && dest[inpsize-1] == SPACE_CHAR) {
        countr++;
        inpsize--;
    }

    i = source_दीर्घता - countr - 1;
    j = source_दीर्घता - 1;

    यावत्(i >= 0 && j >= 0) {
        यदि( countr>0 && isLamAlefChar(dest[i]) ) {
            tempbuffer[j] = LAM_CHAR;
            tempbuffer[j-1] = convertLamAlef[ dest[i] - 0xFEF5 ];
            j--;
            countr--;
        }अन्यथा {
            यदि ((countr == 0) && isLamAlefChar(dest[i]) ) {
                *p_Error_संहिता=U_NO_SPACE_उपलब्धम्;
            }
            tempbuffer[j] = dest[i];
        }
        i--;
        j--;
    }

    यदि(countr > 0) {
        u_memmove(tempbuffer, tempbuffer+countr, source_दीर्घता);
        यदि(u_strlen(tempbuffer) < source_दीर्घता) {
            प्रति(i=source_दीर्घता-1;i>=source_दीर्घता-countr;i--) {
                tempbuffer[i] = SPACE_CHAR;
            }
        }
    }
    u_memcpy(dest, tempbuffer, source_दीर्घता);

    uprv_free(tempbuffer);

    dest_आकार = source_दीर्घता;
    प्रत्यागम dest_आकार;
}

/*
 *Name     : expandCompositCharAtNear
 *Function : Expands the LamAlef character into Lam + Alef, YehHamza character
 *           into Yeh + Hamza, SeenFamily character into SeenFamily character
 *           + Tail, while consuming the space next to the character.
 *           If there are no spaces next to the character, an error
 *           will be set to U_NO_SPACE_AVAILABLE as defined in utypes.h
 */

स्थिर int32_t
expandCompositCharAtNear(char16_t *dest, int32_t source_दीर्घता, int32_t dest_आकार,UError_संहिता *p_Error_संहिता,
                         पूर्णाङ्क yehHamzaOption, पूर्णाङ्क आलोकितम्_पुच्छ_Option, पूर्णाङ्क lamAlefOption, संरचना uShapeVariables shapeVars) {
    int32_t      i = 0;


    char16_t lamalefChar, yehhamzaChar;

    प्रति(i = 0 ;i<=source_दीर्घता-1;i++) {
            यदि (आलोकितम्_पुच्छ_Option && is_आलोकितम्_पुच्छ_Family_Char(dest[i])) {
                यदि ((i>0) && (dest[i-1] == SPACE_CHAR) ) {
                    dest[i-1] = shapeVars.पुच्छ_Char;
                }अन्यथा {
                    *p_Error_संहिता=U_NO_SPACE_उपलब्धम्;
                }
            }अन्यथा यदि(yehHamzaOption && (isYehHamzaChar(dest[i])) ) {
                यदि ((i>0) && (dest[i-1] == SPACE_CHAR) ) {
                    yehhamzaChar = dest[i];
                    dest[i] = yeh_Hamza_गन्तव्यम्_Yeh[yehhamzaChar - YEH_HAMZAFE_CHAR];
                    dest[i-1] = HAMZAFE_CHAR;
                }अन्यथा {

                    *p_Error_संहिता=U_NO_SPACE_उपलब्धम्;
                }
            }अन्यथा यदि(lamAlefOption && isLamAlefChar(dest[i+1])) {
                यदि(dest[i] == SPACE_CHAR){
                    lamalefChar = dest[i+1];
                    dest[i+1] = LAM_CHAR;
                    dest[i] = convertLamAlef[ lamalefChar - 0xFEF5 ];
                }अन्यथा {
                    *p_Error_संहिता=U_NO_SPACE_उपलब्धम्;
                }
            }
       }
       dest_आकार = source_दीर्घता;
       प्रत्यागम dest_आकार;
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

स्थिर int32_t
expandCompositChar(char16_t *dest, int32_t source_दीर्घता,
              int32_t dest_आकार,uint32_t options,
              UError_संहिता *p_Error_संहिता, पूर्णाङ्क shaping_बहुलकम्,संरचना uShapeVariables shapeVars) {

    int32_t      i = 0,j = 0;

    char16_t *tempbuffer=शून्यसूचकः;
    पूर्णाङ्क yehHamzaOption = 0;
    पूर्णाङ्क आलोकितम्_पुच्छ_Option = 0;
    पूर्णाङ्क lamAlefOption = 0;

    यदि (shaping_बहुलकम् == 1){
        यदि ( (options&U_SHAPE_LAMALEF_आवरणम्) == U_SHAPE_LAMALEF_AUTO){

            यदि(shapeVars.spaces_Relative_गन्तव्यम्_Text_Begin_अन्तः == 0) {
                dest_आकार = expand_Composit_Char_At_अन्तः(dest, source_दीर्घता, dest_आकार, p_Error_संहिता);

                यदि(*p_Error_संहिता == U_NO_SPACE_उपलब्धम्) {
                    *p_Error_संहिता = U_ZERO_ERROR;
                    dest_आकार = expandCompositCharAtBegin(dest, source_दीर्घता, dest_आकार, p_Error_संहिता);
                }
            }अन्यथा {
                dest_आकार = expandCompositCharAtBegin(dest, source_दीर्घता, dest_आकार, p_Error_संहिता);

                यदि(*p_Error_संहिता == U_NO_SPACE_उपलब्धम्) {
                    *p_Error_संहिता = U_ZERO_ERROR;
                    dest_आकार = expand_Composit_Char_At_अन्तः(dest, source_दीर्घता, dest_आकार, p_Error_संहिता);
                }
            }

            यदि(*p_Error_संहिता == U_NO_SPACE_उपलब्धम्) {
                *p_Error_संहिता = U_ZERO_ERROR;
                dest_आकार = expandCompositCharAtNear(dest, source_दीर्घता, dest_आकार, p_Error_संहिता, yehHamzaOption,
                                                आलोकितम्_पुच्छ_Option, 1,shapeVars);
            }
        }
    }

    यदि (shaping_बहुलकम् == 1){
        यदि ( (options&U_SHAPE_LAMALEF_आवरणम्) == shapeVars.u_Shape_Lamalef_अन्तः){
            dest_आकार = expand_Composit_Char_At_अन्तः(dest, source_दीर्घता, dest_आकार, p_Error_संहिता);
        }
    }

    यदि (shaping_बहुलकम् == 1){
        यदि ( (options&U_SHAPE_LAMALEF_आवरणम्) == shapeVars.uShapeLamalefBegin){
            dest_आकार = expandCompositCharAtBegin(dest, source_दीर्घता, dest_आकार, p_Error_संहिता);
        }
    }

    यदि (shaping_बहुलकम् == 0){
         यदि ((options&U_SHAPE_YEHHAMZA_आवरणम्) == U_SHAPE_YEHHAMZA_TWOCELL_NEAR){
             yehHamzaOption = 1;
         }
         यदि ((options&U_SHAPE_आलोकितम्_आवरणम्) == U_SHAPE_आलोकितम्_TWOCELL_NEAR){
            आलोकितम्_पुच्छ_Option = 1;
         }
    }
    यदि (shaping_बहुलकम् == 1) {
        यदि ( (options&U_SHAPE_LAMALEF_आवरणम्) == U_SHAPE_LAMALEF_NEAR) {
            lamAlefOption = 1;
        }
    }


    यदि (yehHamzaOption || आलोकितम्_पुच्छ_Option || lamAlefOption){
        dest_आकार = expandCompositCharAtNear(dest, source_दीर्घता, dest_आकार, p_Error_संहिता, yehHamzaOption,
                                            आलोकितम्_पुच्छ_Option,lamAlefOption,shapeVars);
    }


    यदि (shaping_बहुलकम् == 1){
        यदि ( (options&U_SHAPE_LAMALEF_आवरणम्) == U_SHAPE_LAMALEF_RESIZE){
            dest_आकार = calculate_आकार(dest,source_दीर्घता,dest_आकार,options);
            tempbuffer = static_cast<char16_t*>(uprv_malloc((dest_आकार + 1) * U_SIZEOF_UCHAR));

            /* Test for nullptr */
            यदि(tempbuffer == शून्यसूचकः) {
                *p_Error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
                प्रत्यागम 0;
            }

            uprv_memset(tempbuffer, 0, (dest_आकार+1)*U_SIZEOF_UCHAR);

            i = j = 0;
            यावत्(i < dest_आकार && j < dest_आकार) {
                यदि(isLamAlefChar(dest[i]) ) {
                    tempbuffer[j] = convertLamAlef[ dest[i] - 0xFEF5 ];
                    tempbuffer[j+1] = LAM_CHAR;
                    j++;
                }अन्यथा {
                    tempbuffer[j] = dest[i];
                }
                i++;
                j++;
            }

            u_memcpy(dest, tempbuffer, dest_आकार);
        }
    }

    यदि(tempbuffer) {
        uprv_free(tempbuffer);
    }
    प्रत्यागम dest_आकार;
}

/*
 *Name     : shapeUnicode
 *Function : Converts an Arabic Unicode buffer in 06xx Range into a shaped
 *           arabic Unicode buffer in FExx Range
 */
स्थिर int32_t
shapeUnicode(char16_t *dest, int32_t source_दीर्घता,
             int32_t dest_आकार,uint32_t options,
             UError_संहिता *p_Error_संहिता,
             पूर्णाङ्क tashkeelFlag, संरचना uShapeVariables shapeVars) {

    int32_t          i, iend;
    int32_t          चरणः;
    int32_t          चरमः_Pos,Nx, Nw;
    अचिह्नित पूर्णाङ्क     Shape;
    int32_t          lamalef_प्राप्तम् = 0;
    int32_t seenfam_प्राप्तम् = 0, yehhamza_प्राप्तम् =0, tashkeel_प्राप्तम्  = 0;
    char16_t         पूर्व_Link = 0, चरमः_Link = 0, currLink, अग्र_Link = 0;
    char16_t         wLamalef;

    /*
     * Converts the input buffer from FExx Range into 06xx Range
     * to make sure that all characters are in the 06xx range
     * even the lamalef is converted to the special region in
     * the 06xx range
     */
    यदि ((options & U_SHAPE_PRESERVE_PRESENTATION_आवरणम्)  == U_SHAPE_PRESERVE_PRESENTATION_NOOP) {
        प्रति (i = 0; i < source_दीर्घता; i++) {
            char16_t निवेशः_Char  = dest[i];
            यदि ( (निवेशः_Char >= 0xFB50) && (निवेशः_Char <= 0xFBFF)) {
                char16_t c = convertFBto06 [ (निवेशः_Char - 0xFB50) ];
                यदि (c != 0)
                    dest[i] = c;
            } अन्यथा यदि ( (निवेशः_Char >= 0xFE70) && (निवेशः_Char <= 0xFEFC)) {
                dest[i] = convertFEto06 [ (निवेशः_Char - 0xFE70) ] ;
            } अन्यथा {
                dest[i] = निवेशः_Char ;
            }
        }
    }


    /* sets the index to the end of the buffer, together with the step point to -1 */
    i = source_दीर्घता - 1;
    iend = -1;
    चरणः = -1;

    /*
     * This function resolves the link between the characters .
     * Arabic characters have four forms :
     * Isolated Form, Initial Form, Middle Form and Final Form
     */
    currLink = getLink(dest[i]);

    चरमः_Pos = i;
    Nx = -2, Nw = 0;

    यावत् (i != iend) {
        /* If high byte of currLink > 0 then more than one shape */
        यदि ((currLink & 0xFF00) > 0 || (getLink(dest[i]) & IRRELEVANT) != 0) {
            Nw = i + चरणः;
            यावत् (Nx < 0) {         /* we need to know about next char */
                यदि(Nw == iend) {
                    अग्र_Link = 0;
                    Nx = 3000;
                } अन्यथा {
                    अग्र_Link = getLink(dest[Nw]);
                    यदि((अग्र_Link & IRRELEVANT) == 0) {
                        Nx = Nw;
                    } अन्यथा {
                        Nw = Nw + चरणः;
                    }
                }
            }

            यदि ( ((currLink & ALEFTYPE) > 0)  &&  ((चरमः_Link & LAMTYPE) > 0) ) {
                lamalef_प्राप्तम् = 1;
                wLamalef = changeLamAlef(dest[i]); /*get from 0x065C-0x065f */
                यदि ( wLamalef != 0) {
                    dest[i] = LAMALEF_SPACE_SUB;            /* The default case is to drop the Alef and replace */
                    dest[चरमः_Pos] =wLamalef;     /* it by LAMALEF_SPACE_SUB which is the last character in the  */
                    i=चरमः_Pos;                   /* unicode private use area, this is done to make   */
                }                                /* sure that removeLamAlefSpaces() handles only the */
                चरमः_Link = पूर्व_Link;             /* spaces generated during lamalef generation.      */
                currLink = getLink(wLamalef);    /* LAMALEF_SPACE_SUB is added here and is replaced by spaces   */
            }                                    /* in removeLamAlefSpaces()                         */

            यदि ((i > 0) && (dest[i-1] == SPACE_CHAR)){
                यदि ( is_आलोकितम्_Family_Char(dest[i])) {
                    seenfam_प्राप्तम् = 1;
                } अन्यथा यदि (dest[i] == YEH_HAMZA_CHAR) {
                    yehhamza_प्राप्तम् = 1;
                }
            }
            अन्यथा यदि(i==0){
                यदि ( is_आलोकितम्_Family_Char(dest[i])){
                    seenfam_प्राप्तम् = 1;
                } अन्यथा यदि (dest[i] == YEH_HAMZA_CHAR) {
                    yehhamza_प्राप्तम् = 1;
                }
            }

            /*
             * get the proper shape according to link ability of neighbors
             * and of character; depends on the order of the shapes
             * (isolated, initial, middle, final) in the compatibility area
             */
            Shape = shape_सारणी[अग्र_Link & (LINKR + LINKL)]
                              [चरमः_Link & (LINKR + LINKL)]
                              [currLink & (LINKR + LINKL)];

            यदि ((currLink & (LINKR+LINKL)) == 1) {
                Shape &= 1;
            } अन्यथा यदि(isTashkeelChar(dest[i])) {
                यदि( (चरमः_Link & LINKL) && (अग्र_Link & LINKR) && (tashkeelFlag == 1) &&
                     dest[i] != 0x064C && dest[i] != 0x064D )
                {
                    Shape = 1;
                    यदि( (अग्र_Link&ALEFTYPE) == ALEFTYPE && (चरमः_Link&LAMTYPE) == LAMTYPE ) {
                        Shape = 0;
                    }
                } अन्यथा यदि(tashkeelFlag == 2 && dest[i] == SHADDA06_CHAR){
                    Shape = 1;
                } अन्यथा {
                    Shape = 0;
                }
            }
            यदि ((dest[i] ^ 0x0600) < 0x100) {
                यदि ( isTashkeelChar(dest[i]) ){
                    यदि (tashkeelFlag == 2  && dest[i] != SHADDA06_CHAR){
                        dest[i] = TASHKEEL_SPACE_SUB;
                        tashkeel_प्राप्तम्  = 1;
                    } अन्यथा {
                        /* to ensure the array index is within the range */
                        U_ASSERT(dest[i] >= 0x064Bu
                            && dest[i]-0x064Bu < UPRV_LENGTHOF(IrrelevantPos));
                        dest[i] =  0xFE70 + IrrelevantPos[(dest[i] - 0x064B)] + static_cast<char16_t>(Shape);
                    }
                }अन्यथा यदि ((currLink & APRESENT) > 0) {
                    dest[i] = static_cast<char16_t>(0xFB50 + (currLink >> 8) + Shape);
                }अन्यथा यदि ((currLink >> 8) > 0 && (currLink & IRRELEVANT) == 0) {
                    dest[i] = static_cast<char16_t>(0xFE70 + (currLink >> 8) + Shape);
                }
            }
        }

        /* move one notch forward */
        यदि ((currLink & IRRELEVANT) == 0) {
            पूर्व_Link = चरमः_Link;
            चरमः_Link = currLink;
            चरमः_Pos = i;
        }

        i = i + चरणः;
        यदि (i == Nx) {
            currLink = अग्र_Link;
            Nx = -2;
        } अन्यथा यदि(i != iend) {
            currLink = getLink(dest[i]);
        }
    }
    dest_आकार = source_दीर्घता;
    यदि ( (lamalef_प्राप्तम् != 0 ) || (tashkeel_प्राप्तम्  != 0) ){
        dest_आकार = handleGeneratedSpaces(dest,source_दीर्घता,dest_आकार,options,p_Error_संहिता, shapeVars);
    }

    यदि ( (seenfam_प्राप्तम् != 0) || (yehhamza_प्राप्तम् != 0) ) {
        dest_आकार = expandCompositChar(dest, source_दीर्घता,dest_आकार,options,p_Error_संहिता, SHAPE_बहुलकम्,shapeVars);
    }
    प्रत्यागम dest_आकार;
}

/*
 *Name     : deShapeUnicode
 *Function : Converts an Arabic Unicode buffer in FExx Range into unshaped
 *           arabic Unicode buffer in 06xx Range
 */
स्थिर int32_t
deShapeUnicode(char16_t *dest, int32_t source_दीर्घता,
               int32_t dest_आकार,uint32_t options,
               UError_संहिता *p_Error_संहिता, संरचना uShapeVariables shapeVars) {
    int32_t i = 0;
    int32_t lamalef_प्राप्तम् = 0;
    int32_t yehHamzaComposeEnabled = 0;
    int32_t आलोकितम्_Compose_Enabled = 0;

    yehHamzaComposeEnabled = ((options&U_SHAPE_YEHHAMZA_आवरणम्) == U_SHAPE_YEHHAMZA_TWOCELL_NEAR) ? 1 : 0;
    आलोकितम्_Compose_Enabled = ((options&U_SHAPE_आलोकितम्_आवरणम्) == U_SHAPE_आलोकितम्_TWOCELL_NEAR)? 1 : 0;

    /*
     *This for loop changes the buffer from the Unicode FE range to
     *the Unicode 06 range
     */

    प्रति(i = 0; i < source_दीर्घता; i++) {
        char16_t  निवेशः_Char = dest[i];
        यदि ( (निवेशः_Char >= 0xFB50) && (निवेशः_Char <= 0xFBFF)) { /* FBxx Arabic range */
            char16_t c = convertFBto06 [ (निवेशः_Char - 0xFB50) ];
            यदि (c != 0)
                dest[i] = c;
        } अन्यथा यदि( (yehHamzaComposeEnabled == 1) && ((निवेशः_Char == HAMZA06_CHAR) || (निवेशः_Char == HAMZAFE_CHAR))
                && (i < (source_दीर्घता - 1)) && isAlefMaksouraChar(dest[i+1] )) {
                 dest[i] = SPACE_CHAR;
                 dest[i+1] = YEH_HAMZA_CHAR;
        } अन्यथा यदि ( (आलोकितम्_Compose_Enabled == 1) && (is_पुच्छ_Char(निवेशः_Char)) && (i< (source_दीर्घता - 1))
                        && (is_आलोकितम्_पुच्छ_Family_Char(dest[i+1])) ) {
                dest[i] = SPACE_CHAR;
        } अन्यथा यदि (( निवेशः_Char >= 0xFE70) && (निवेशः_Char <= 0xFEF4 )) { /* FExx Arabic range */
                dest[i] = convertFEto06 [ (निवेशः_Char - 0xFE70) ];
        } अन्यथा {
            dest[i] = निवेशः_Char ;
        }

        यदि( isLamAlefChar(dest[i]) )
            lamalef_प्राप्तम् = 1;
    }

   dest_आकार = source_दीर्घता;
   यदि (lamalef_प्राप्तम् != 0){
          dest_आकार = expandCompositChar(dest,source_दीर्घता,dest_आकार,options,p_Error_संहिता,DESHAPE_बहुलकम्, shapeVars);
   }
   प्रत्यागम dest_आकार;
}

/*
 ****************************************
 * u_shapeArabic
 ****************************************
 */

U_CAPI int32_t U_EXPORT2
u_shapeArabic(ध्रुव char16_t *source, int32_t source_दीर्घता,
              char16_t *dest, int32_t dest_क्षमता,
              uint32_t options,
              UError_संहिता *p_Error_संहिता) {

    int32_t dest_दीर्घता;
    संरचना  uShapeVariables shapeVars = { OLD_पुच्छ_CHAR,U_SHAPE_LAMALEF_BEGIN,U_SHAPE_LAMALEF_अन्तः,U_SHAPE_TASHKEEL_BEGIN,U_SHAPE_TASHKEEL_अन्तः,0};

    /* usual error checking */
    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }

    /* make sure that no reserved options values are used; allow dest==nullptr only for preflighting */
    यदि( source==शून्यसूचकः || source_दीर्घता<-1 || (dest==शून्यसूचकः && dest_क्षमता!=0) || dest_क्षमता<0 ||
                (((options&U_SHAPE_TASHKEEL_आवरणम्) > 0) &&
                 ((options&U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED) == U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED) ) ||
                (((options&U_SHAPE_TASHKEEL_आवरणम्) > 0) &&
                 ((options&U_SHAPE_LETTERS_आवरणम्) == U_SHAPE_LETTERS_UNSHAPE)) ||
                (options&U_SHAPE_DIGIT_TYPE_RESERVED)==U_SHAPE_DIGIT_TYPE_RESERVED ||
                (options&U_SHAPE_DIGITS_आवरणम्)==U_SHAPE_DIGITS_RESERVED ||
                ((options&U_SHAPE_LAMALEF_आवरणम्) != U_SHAPE_LAMALEF_RESIZE  &&
                (options&U_SHAPE_AGGREGATE_TASHKEEL_आवरणम्) != 0) ||
                ((options&U_SHAPE_AGGREGATE_TASHKEEL_आवरणम्) == U_SHAPE_AGGREGATE_TASHKEEL &&
                (options&U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED) != U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED)
    )
    {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }
    /* Validate  lamalef options */
    यदि(((options&U_SHAPE_LAMALEF_आवरणम्) > 0)&&
              !(((options & U_SHAPE_LAMALEF_आवरणम्)==U_SHAPE_LAMALEF_BEGIN) ||
                ((options & U_SHAPE_LAMALEF_आवरणम्)==U_SHAPE_LAMALEF_अन्तः ) ||
                ((options & U_SHAPE_LAMALEF_आवरणम्)==U_SHAPE_LAMALEF_RESIZE )||
                 ((options & U_SHAPE_LAMALEF_आवरणम्)==U_SHAPE_LAMALEF_AUTO) ||
                 ((options & U_SHAPE_LAMALEF_आवरणम्)==U_SHAPE_LAMALEF_NEAR)))
    {
         *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }
    /* Validate  Tashkeel options */
    यदि(((options&U_SHAPE_TASHKEEL_आवरणम्) > 0)&&
                   !(((options & U_SHAPE_TASHKEEL_आवरणम्)==U_SHAPE_TASHKEEL_BEGIN) ||
                     ((options & U_SHAPE_TASHKEEL_आवरणम्)==U_SHAPE_TASHKEEL_अन्तः )
                    ||((options & U_SHAPE_TASHKEEL_आवरणम्)==U_SHAPE_TASHKEEL_RESIZE )||
                    ((options & U_SHAPE_TASHKEEL_आवरणम्)==U_SHAPE_TASHKEEL_REPLACE_BY_TATWEEL)))
    {
         *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }
    /* determine the source length */
    यदि(source_दीर्घता==-1) {
        source_दीर्घता=u_strlen(source);
    }
    यदि(source_दीर्घता<=0) {
        प्रत्यागम u_terminateUChars(dest, dest_क्षमता, 0, p_Error_संहिता);
    }

    /* check that source and destination do not overlap */
    यदि( dest!=शून्यसूचकः &&
        ((source<=dest && dest<source+source_दीर्घता) ||
         (dest<=source && source<dest+dest_क्षमता))) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    /* Does Options contain the new Seen Tail Unicode code point option */
    यदि ( (options&U_SHAPE_पुच्छ_TYPE_आवरणम्) == U_SHAPE_पुच्छ_NEW_UNICODE){
        shapeVars.पुच्छ_Char = NEW_पुच्छ_CHAR;
    }अन्यथा {
        shapeVars.पुच्छ_Char = OLD_पुच्छ_CHAR;
    }

    यदि((options&U_SHAPE_LETTERS_आवरणम्)!=U_SHAPE_LETTERS_NOOP) {
        char16_t बफर[300];
        char16_t *tempbuffer, *tempsource = शून्यसूचकः;
        int32_t output_आकार, spacesCountl=0, spacesCountr=0;

        यदि((options&U_SHAPE_AGGREGATE_TASHKEEL_आवरणम्)>0) {
            int32_t logical_क्रमः = (options&U_SHAPE_TEXT_DIRECTION_आवरणम्) == U_SHAPE_TEXT_DIRECTION_LOGICAL;
            int32_t aggregate_tashkeel =
                        (options&(U_SHAPE_AGGREGATE_TASHKEEL_आवरणम्+U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED)) ==
                        (U_SHAPE_AGGREGATE_TASHKEEL+U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED);
            पूर्णाङ्क चरणः=logical_क्रमः?1:-1;
            पूर्णाङ्क j=logical_क्रमः?-1:2*source_दीर्घता;
            पूर्णाङ्क i=logical_क्रमः?-1:source_दीर्घता;
            पूर्णाङ्क अन्तः=logical_क्रमः?source_दीर्घता:-1;
            पूर्णाङ्क aggregation_possible = 1;
            char16_t पूर्व = 0;
            char16_t पूर्व_Link, currLink = 0;
            पूर्णाङ्क new_Source_दीर्घता = 0;
            tempsource = (char16_t *)uprv_malloc(2*source_दीर्घता*U_SIZEOF_UCHAR);
            यदि(tempsource == शून्यसूचकः) {
                *p_Error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
                प्रत्यागम 0;
            }

            यावत् ((i+=चरणः) != अन्तः) {
                पूर्व_Link = currLink;
                currLink = getLink(source[i]);
                यदि (aggregate_tashkeel && ((पूर्व_Link|currLink)&COMBINE) == COMBINE && aggregation_possible) {
                    यदि (j<0 || j>=2*source_दीर्घता) {
                        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
                        यदि (tempsource != शून्यसूचकः) uprv_free(tempsource);
                        प्रत्यागम 0;
                    }
                    aggregation_possible = 0;
                    tempsource[j] = (पूर्व<source[i]?पूर्व:source[i])-0x064C+0xFC5E;
                    currLink = getLink(tempsource[j]);
                } अन्यथा {
                    aggregation_possible = 1;
                    tempsource[j+=चरणः] = source[i];
                    पूर्व = source[i];
                    new_Source_दीर्घता++;
                }
            }
            source = tempsource+(logical_क्रमः?0:j);
            source_दीर्घता = new_Source_दीर्घता;
        }

        /* calculate destination size */
        /* TODO: do we ever need to do this pure preflighting? */
        यदि(((options&U_SHAPE_LAMALEF_आवरणम्)==U_SHAPE_LAMALEF_RESIZE) ||
           ((options&U_SHAPE_TASHKEEL_आवरणम्)==U_SHAPE_TASHKEEL_RESIZE)) {
            output_आकार=calculate_आकार(source,source_दीर्घता,dest_क्षमता,options);
        } अन्यथा {
            output_आकार=source_दीर्घता;
        }

        यदि(output_आकार>dest_क्षमता) {
            *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                यदि (tempsource != शून्यसूचकः) uprv_free(tempsource);
            प्रत्यागम output_आकार;
        }

        /*
         * need a temporary buffer of size max(outputSize, sourceLength)
         * because at first we copy source->temp
         */
        यदि(source_दीर्घता>output_आकार) {
            output_आकार=source_दीर्घता;
        }

        /* Start of Arabic letter shaping part */
        यदि(output_आकार<=UPRV_LENGTHOF(बफर)) {
            output_आकार=UPRV_LENGTHOF(बफर);
            tempbuffer=बफर;
        } अन्यथा {
            tempbuffer = (char16_t *)uprv_malloc(output_आकार*U_SIZEOF_UCHAR);

            /*Test for nullptr*/
            यदि(tempbuffer == शून्यसूचकः) {
                *p_Error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
                यदि (tempsource != शून्यसूचकः) uprv_free(tempsource);
                प्रत्यागम 0;
            }
        }
        u_memcpy(tempbuffer, source, source_दीर्घता);
        यदि (tempsource != शून्यसूचकः){
            uprv_free(tempsource);
        }

        यदि(source_दीर्घता<output_आकार) {
            uprv_memset(tempbuffer+source_दीर्घता, 0, (output_आकार-source_दीर्घता)*U_SIZEOF_UCHAR);
        }

        यदि((options&U_SHAPE_TEXT_DIRECTION_आवरणम्) == U_SHAPE_TEXT_DIRECTION_LOGICAL) {
            गणनम्_Spaces(tempbuffer,source_दीर्घता,options,&spacesCountl,&spacesCountr);
            invert_बफर(tempbuffer,source_दीर्घता,options,spacesCountl,spacesCountr);
        }

        यदि((options&U_SHAPE_TEXT_DIRECTION_आवरणम्) == U_SHAPE_TEXT_DIRECTION_VISUAL_LTR) {
            यदि((options&U_SHAPE_SPACES_RELATIVE_गन्तव्यम्_TEXT_आवरणम्) == U_SHAPE_SPACES_RELATIVE_गन्तव्यम्_TEXT_BEGIN_अन्तः) {
                shapeVars.spaces_Relative_गन्तव्यम्_Text_Begin_अन्तः = 1;
                shapeVars.uShapeLamalefBegin = U_SHAPE_LAMALEF_अन्तः;
                shapeVars.u_Shape_Lamalef_अन्तः = U_SHAPE_LAMALEF_BEGIN;
                shapeVars.uShapeTashkeelBegin = U_SHAPE_TASHKEEL_अन्तः;
                shapeVars.u_Shape_Tashkeel_अन्तः = U_SHAPE_TASHKEEL_BEGIN;
            }
        }

        विकल्प(options&U_SHAPE_LETTERS_आवरणम्) {
        पक्ष U_SHAPE_LETTERS_SHAPE :
             यदि( (options&U_SHAPE_TASHKEEL_आवरणम्)> 0
                 && ((options&U_SHAPE_TASHKEEL_आवरणम्) !=U_SHAPE_TASHKEEL_REPLACE_BY_TATWEEL)) {
                /* Call the shaping function with tashkeel flag == 2 for removal of tashkeel */
                dest_दीर्घता = shapeUnicode(tempbuffer,source_दीर्घता,dest_क्षमता,options,p_Error_संहिता,2,shapeVars);
             }अन्यथा {
                /* default Call the shaping function with tashkeel flag == 1 */
                dest_दीर्घता = shapeUnicode(tempbuffer,source_दीर्घता,dest_क्षमता,options,p_Error_संहिता,1,shapeVars);

                /*After shaping text check if user wants to remove tashkeel and replace it with tatweel*/
                यदि( (options&U_SHAPE_TASHKEEL_आवरणम्) == U_SHAPE_TASHKEEL_REPLACE_BY_TATWEEL){
                  dest_दीर्घता = handleTashkeelWithTatweel(tempbuffer,dest_दीर्घता,dest_क्षमता,options,p_Error_संहिता);
                }
            }
            भङ्ग;
        पक्ष U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED :
            /* Call the shaping function with tashkeel flag == 0 */
            dest_दीर्घता = shapeUnicode(tempbuffer,source_दीर्घता,dest_क्षमता,options,p_Error_संहिता,0,shapeVars);
            भङ्ग;

        पक्ष U_SHAPE_LETTERS_UNSHAPE :
            /* Call the deshaping function */
            dest_दीर्घता = deShapeUnicode(tempbuffer,source_दीर्घता,dest_क्षमता,options,p_Error_संहिता,shapeVars);
            भङ्ग;
        स्वभाव :
            /* will never occur because of validity checks above */
            dest_दीर्घता = 0;
            भङ्ग;
        }

        /*
         * TODO: (markus 2002aug01)
         * For as long as we always preflight the outputSize above
         * we should U_ASSERT(outputSize==destLength)
         * except for the adjustment above before the tempbuffer allocation
         */

        यदि((options&U_SHAPE_TEXT_DIRECTION_आवरणम्) == U_SHAPE_TEXT_DIRECTION_LOGICAL) {
            गणनम्_Spaces(tempbuffer,dest_दीर्घता,options,&spacesCountl,&spacesCountr);
            invert_बफर(tempbuffer,dest_दीर्घता,options,spacesCountl,spacesCountr);
        }
        u_memcpy(dest, tempbuffer, uprv_min(dest_दीर्घता, dest_क्षमता));

        यदि(tempbuffer!=बफर) {
            uprv_free(tempbuffer);
        }

        यदि(dest_दीर्घता>dest_क्षमता) {
            *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
            प्रत्यागम dest_दीर्घता;
        }

        /* End of Arabic letter shaping part */
    } अन्यथा {
        /*
         * No letter shaping:
         * just make sure the destination is large enough and copy the string.
         */
        यदि(dest_क्षमता<source_दीर्घता) {
            /* this catches preflighting, too */
            *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
            प्रत्यागम source_दीर्घता;
        }
        u_memcpy(dest, source, source_दीर्घता);
        dest_दीर्घता=source_दीर्घता;
    }

    /*
     * Perform number shaping.
     * With UTF-16 or UTF-32, the length of the string is constant.
     * The easiest way to do this is to operate on the destination and
     * "shape" the digits in-place.
     */
    यदि((options&U_SHAPE_DIGITS_आवरणम्)!=U_SHAPE_DIGITS_NOOP) {
        char16_t digit_आधारः;
        int32_t i;

        /* select the requested digit group */
        विकल्प(options&U_SHAPE_DIGIT_TYPE_आवरणम्) {
        पक्ष U_SHAPE_DIGIT_TYPE_AN:
            digit_आधारः=0x660; /* Unicode: "Arabic-Indic digits" */
            भङ्ग;
        पक्ष U_SHAPE_DIGIT_TYPE_AN_EXTENDED:
            digit_आधारः=0x6f0; /* Unicode: "Eastern Arabic-Indic digits (Persian and Urdu)" */
            भङ्ग;
        स्वभाव:
            /* will never occur because of validity checks above */
            digit_आधारः=0;
            भङ्ग;
        }

        /* perform the requested operation */
        विकल्प(options&U_SHAPE_DIGITS_आवरणम्) {
        पक्ष U_SHAPE_DIGITS_EN2AN:
            /* add (digitBase-'0') to each European (ASCII) digit code point */
            digit_आधारः-=0x30;
            प्रति(i=0; i<dest_दीर्घता; ++i) {
                यदि(((uint32_t)dest[i]-0x30)<10) {
                    dest[i]+=digit_आधारः;
                }
            }
            भङ्ग;
        पक्ष U_SHAPE_DIGITS_AN2EN:
            /* subtract (digitBase-'0') from each Arabic digit code point */
            प्रति(i=0; i<dest_दीर्घता; ++i) {
                यदि(((uint32_t)dest[i]-(uint32_t)digit_आधारः)<10) {
                    dest[i]-=digit_आधारः-0x30;
                }
            }
            भङ्ग;
        पक्ष U_SHAPE_DIGITS_ALEN2AN_INIT_LR:
            shape_गन्तव्यम्_Arabic_Digits_With_Context(dest, dest_दीर्घता,
                                            digit_आधारः,
                                            (options & U_SHAPE_TEXT_DIRECTION_आवरणम्) == U_SHAPE_TEXT_DIRECTION_LOGICAL,
                                            असत्य);
            भङ्ग;
        पक्ष U_SHAPE_DIGITS_ALEN2AN_INIT_AL:
            shape_गन्तव्यम्_Arabic_Digits_With_Context(dest, dest_दीर्घता,
                                            digit_आधारः,
                                            (options & U_SHAPE_TEXT_DIRECTION_आवरणम्) == U_SHAPE_TEXT_DIRECTION_LOGICAL,
                                            सत्य);
            भङ्ग;
        स्वभाव:
            /* will never occur because of validity checks above */
            भङ्ग;
        }
    }

    प्रत्यागम u_terminateUChars(dest, dest_क्षमता, dest_दीर्घता, p_Error_संहिता);
}
