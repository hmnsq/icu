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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/uchar.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/ushape.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "putilimp.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"
#ಅನ್ತರ್ಭಾವ "ubidi_props.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"

#ಅನ್ತರ್ಭಾವ <limits>
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

#ವ್ಯಾಖ್ಯಾನಿಸು IRRELEVANT 4
#ವ್ಯಾಖ್ಯಾನಿಸು LAMTYPE    16
#ವ್ಯಾಖ್ಯಾನಿಸು ALEFTYPE   32
#ವ್ಯಾಖ್ಯಾನಿಸು LINKR      1
#ವ್ಯಾಖ್ಯಾನಿಸು LINKL      2
#ವ್ಯಾಖ್ಯಾನಿಸು APRESENT   8
#ವ್ಯಾಖ್ಯಾನಿಸು SHADDA     64
#ವ್ಯಾಖ್ಯಾನಿಸು CSHADDA    128
#ವ್ಯಾಖ್ಯಾನಿಸು COMBINE    (SHADDA+CSHADDA)

#ವ್ಯಾಖ್ಯಾನಿಸು HAMZAFE_CHAR       0xfe80
#ವ್ಯಾಖ್ಯಾನಿಸು HAMZA06_CHAR       0x0621
#ವ್ಯಾಖ್ಯಾನಿಸು YEH_HAMZA_CHAR     0x0626
#ವ್ಯಾಖ್ಯಾನಿಸು YEH_HAMZAFE_CHAR   0xFE89
#ವ್ಯಾಖ್ಯಾನಿಸು LAMALEF_SPACE_SUB  0xFFFF
#ವ್ಯಾಖ್ಯಾನಿಸು TASHKEEL_SPACE_SUB 0xFFFE
#ವ್ಯಾಖ್ಯಾನಿಸು NEW_ಬಾಲ_CHAR      0xFE73
#ವ್ಯಾಖ್ಯಾನಿಸು OLD_ಬಾಲ_CHAR      0x200B
#ವ್ಯಾಖ್ಯಾನಿಸು LAM_CHAR           0x0644
#ವ್ಯಾಖ್ಯಾನಿಸು SPACE_CHAR         0x0020
#ವ್ಯಾಖ್ಯಾನಿಸು SHADDA_CHAR        0xFE7C
#ವ್ಯಾಖ್ಯಾನಿಸು TATWEEL_CHAR       0x0640
#ವ್ಯಾಖ್ಯಾನಿಸು SHADDA_TATWEEL_CHAR  0xFE7D
#ವ್ಯಾಖ್ಯಾನಿಸು SHADDA06_CHAR      0x0651

#ವ್ಯಾಖ್ಯಾನಿಸು SHAPE_ಬಹುಲಕ   0
#ವ್ಯಾಖ್ಯಾನಿಸು DESHAPE_ಬಹುಲಕ 1

ರಚನೆ uShapeVariables {
     char16_t ಬಾಲ_Char;
     uint32_t uShapeLamalefBegin;
     uint32_t u_Shape_Lamalef_ಕೊನೆ;
     uint32_t uShapeTashkeelBegin;
     uint32_t u_Shape_Tashkeel_ಕೊನೆ;
     ಪೂರ್ಣಾಂಕ spaces_Relative_ಗಮ್ಯ_ಪಠ್ಯ_Begin_ಕೊನೆ;
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ uint8_t ಬಾಲ_Family_Isolated_Final[] = {
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

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ uint8_t tashkeelMedial[] = {
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

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t yeh_Hamza_ಗಮ್ಯ_Yeh[] =
{
/* isolated*/ 0xFEEF,
/* final   */ 0xFEF0
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ uint8_t IrrelevantPos[] = {
    0x0, 0x2, 0x4, 0x6,
    0x8, 0xA, 0xC, 0xE
};


ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t convertLamAlef[] =
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

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t araLink[178]=
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

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ uint8_t presALink[] = {
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

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ uint8_t presBLink[]=
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

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t convertFBto06[] =
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

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t convertFEto06[] =
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

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ uint8_t shape_ಕೋಷ್ಟಕ[4][4][4]=
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
ಸ್ಥಿರ ಶೂನ್ಯ
shape_ಗಮ್ಯ_Arabic_Digits_With_Context(char16_t *s, int32_t length,
                                char16_t digit_ತಳ,
                                UBool isLogical, UBool ಕೊನೆಯ_Strong_Was_AL) {
    int32_t i;
    char16_t c;

    digit_ತಳ-=0x30;

    /* the iteration direction depends on the type of input */
    ಆದರೆ(isLogical) {
        ಗಾಗಿ(i=0; i<length; ++i) {
            c=s[i];
            ಆಯ್ಕೆ(ubidi_getClass(c)) {
            ಸ್ಥಿತಿ U_ಎಡ_ಗಮ್ಯ_ಬಲ: /* L */
            ಸ್ಥಿತಿ U_ಬಲ_ಗಮ್ಯ_ಎಡ: /* R */
                ಕೊನೆಯ_Strong_Was_AL=ಸುಳ್ಳು;
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ U_ಬಲ_ಗಮ್ಯ_ಎಡ_ARABIC: /* AL */
                ಕೊನೆಯ_Strong_Was_AL=ನಿಜ;
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ U_EUROPEAN_ಸಂಖ್ಯೆ: /* EN */
                ಆದರೆ (ಕೊನೆಯ_Strong_Was_AL && static_cast<uint32_t>(c - 0x30) < 10) {
                    s[i] = static_cast<char16_t>(digit_ತಳ + c); /* digitBase+(c-0x30) - digitBase was modified above */
                }
                ನಿಲ್ಲಿಸು;
            ಪೂರ್ವನಿಯೋಜಿತ :
                ನಿಲ್ಲಿಸು;
            }
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಗಾಗಿ(i=length; i>0; /* pre-decrement in the body */) {
            c=s[--i];
            ಆಯ್ಕೆ(ubidi_getClass(c)) {
            ಸ್ಥಿತಿ U_ಎಡ_ಗಮ್ಯ_ಬಲ: /* L */
            ಸ್ಥಿತಿ U_ಬಲ_ಗಮ್ಯ_ಎಡ: /* R */
                ಕೊನೆಯ_Strong_Was_AL=ಸುಳ್ಳು;
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ U_ಬಲ_ಗಮ್ಯ_ಎಡ_ARABIC: /* AL */
                ಕೊನೆಯ_Strong_Was_AL=ನಿಜ;
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ U_EUROPEAN_ಸಂಖ್ಯೆ: /* EN */
                ಆದರೆ (ಕೊನೆಯ_Strong_Was_AL && static_cast<uint32_t>(c - 0x30) < 10) {
                    s[i] = static_cast<char16_t>(digit_ತಳ + c); /* digitBase+(c-0x30) - digitBase was modified above */
                }
                ನಿಲ್ಲಿಸು;
            ಪೂರ್ವನಿಯೋಜಿತ :
                ನಿಲ್ಲಿಸು;
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
ಸ್ಥಿರ ಶೂನ್ಯ
invert_ಬಫರ್(char16_t *ಬಫರ್, int32_t size, uint32_t /*options*/, int32_t lowlimit, int32_t highlimit) {
    char16_t ತಾತ್ಕಾಲಿಕ;
    int32_t i=0,j=0;
    ಗಾಗಿ(i=lowlimit,j=size-highlimit-1;i<j;i++,j--) {
        ತಾತ್ಕಾಲಿಕ = ಬಫರ್[i];
        ಬಫರ್[i] = ಬಫರ್[j];
        ಬಫರ್[j] = ತಾತ್ಕಾಲಿಕ;
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
ಸ್ಥಿರ ಸಾಲೊಳಗೆ char16_t
changeLamAlef(char16_t ch) {
    ಆಯ್ಕೆ(ch) {
    ಸ್ಥಿತಿ 0x0622 :
        ಹಿಂತಿರುಗಿ 0x065C;
    ಸ್ಥಿತಿ 0x0623 :
        ಹಿಂತಿರುಗಿ 0x065D;
    ಸ್ಥಿತಿ 0x0625 :
        ಹಿಂತಿರುಗಿ 0x065E;
    ಸ್ಥಿತಿ 0x0627 :
        ಹಿಂತಿರುಗಿ 0x065F;
    }
    ಹಿಂತಿರುಗಿ 0;
}

/*
 *Name     : getLink
 *Function : Resolves the link between the characters as
 *           Arabic characters have four forms :
 *           Isolated, Initial, Middle and Final Form
 */
ಸ್ಥಿರ char16_t
getLink(char16_t ch) {
    ಆದರೆ(ch >= 0x0622 && ch <= 0x06D3) {
        ಹಿಂತಿರುಗಿ(araLink[ch-0x0622]);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ch == 0x200D) {
        ಹಿಂತಿರುಗಿ(3);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ch >= 0x206D && ch <= 0x206F) {
        ಹಿಂತಿರುಗಿ(4);
    }ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ch >= 0xFB50 && ch <= 0xFC62) {
        ಹಿಂತಿರುಗಿ(presALink[ch-0xFB50]);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ch >= 0xFE70 && ch <= 0xFEFC) {
        ಹಿಂತಿರುಗಿ(presBLink[ch-0xFE70]);
    }ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ(0);
    }
}

/*
 *Name     : countSpaces
 *Function : Counts the number of spaces
 *           at each end of the logical buffer
 */
ಸ್ಥಿರ ಶೂನ್ಯ
ಎಣಿಕೆ_Spaces(char16_t *dest, int32_t size, uint32_t /*options*/, int32_t *spacesCountl, int32_t *spacesCountr) {
    int32_t i = 0;
    int32_t countl = 0,countr = 0;
    ಆಗ((dest[i] == SPACE_CHAR) && (countl < size)) {
       countl++;
       i++;
    }
    ಆದರೆ (countl < size) {  /* the entire buffer is not all space */
        ಆಗ(dest[size-1] == SPACE_CHAR) {
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
ಸ್ಥಿರ ಸಾಲೊಳಗೆ int32_t
isTashkeelChar(char16_t ch) {
    ಹಿಂತಿರುಗಿ static_cast<int32_t>(ch >= 0x064B && ch <= 0x0652);
}

/*
 *Name     : isTashkeelCharFE
 *Function : Returns 1 for Tashkeel characters in FE range else return 0
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ int32_t
isTashkeelCharFE(char16_t ch) {
    ಹಿಂತಿರುಗಿ static_cast<int32_t>(ch >= 0xFE70 && ch <= 0xFE7F);
}

/*
 *Name     : isAlefChar
 *Function : Returns 1 for Alef characters else return 0
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ int32_t
isAlefChar(char16_t ch) {
    ಹಿಂತಿರುಗಿ static_cast<int32_t>(ch == 0x0622 || ch == 0x0623 || ch == 0x0625 || ch == 0x0627);
}

/*
 *Name     : isLamAlefChar
 *Function : Returns 1 for LamAlef characters else return 0
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ int32_t
isLamAlefChar(char16_t ch) {
    ಹಿಂತಿರುಗಿ static_cast<int32_t>(ch >= 0xFEF5 && ch <= 0xFEFC);
}

/*BIDI
 *Name     : isTailChar
 *Function : returns 1 if the character matches one of the tail characters (0xfe73 or 0x200b) otherwise returns 0
 */

ಸ್ಥಿರ ಸಾಲೊಳಗೆ int32_t
is_ಬಾಲ_Char(char16_t ch) {
    ಆದರೆ(ch == OLD_ಬಾಲ_CHAR || ch == NEW_ಬಾಲ_CHAR){
            ಹಿಂತಿರುಗಿ 1;
    }ಇಲ್ಲದಿದ್ದರೆ{
            ಹಿಂತಿರುಗಿ 0;
    }
}

/*BIDI
 *Name     : isSeenTailFamilyChar
 *Function : returns 1 if the character is a seen family isolated character
 *           in the FE range otherwise returns 0
 */

ಸ್ಥಿರ ಸಾಲೊಳಗೆ int32_t
is_ನೋಡಿದ_ಬಾಲ_Family_Char(char16_t ch) {
    ಆದರೆ(ch >= 0xfeb1 && ch < 0xfebf){
            ಹಿಂತಿರುಗಿ ಬಾಲ_Family_Isolated_Final [ch - 0xFEB1];
    }ಇಲ್ಲದಿದ್ದರೆ{
            ಹಿಂತಿರುಗಿ 0;
    }
}

 /* Name     : isSeenFamilyChar
  * Function : returns 1 if the character is a seen family character in the Unicode
  *            06 range otherwise returns 0
 */

ಸ್ಥಿರ ಸಾಲೊಳಗೆ int32_t
is_ನೋಡಿದ_Family_Char(char16_t  ch){
    ಆದರೆ(ch >= 0x633 && ch <= 0x636){
        ಹಿಂತಿರುಗಿ 1;
    }ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ 0;
    }
}

/*Start of BIDI*/
/*
 *Name     : isAlefMaksouraChar
 *Function : returns 1 if the character is a Alef Maksoura Final or isolated
 *           otherwise returns 0
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ int32_t
isAlefMaksouraChar(char16_t ch) {
    ಹಿಂತಿರುಗಿ static_cast<int32_t>(ch == 0xFEEF || ch == 0xFEF0 || ch == 0x0649);
}

/*
 * Name     : isYehHamzaChar
 * Function : returns 1 if the character is a yehHamza isolated or yehhamza
 *            final is found otherwise returns 0
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ int32_t
isYehHamzaChar(char16_t ch) {
    ಆದರೆ((ch==0xFE89)||(ch==0xFE8A)){
        ಹಿಂತಿರುಗಿ 1;
    }ಇಲ್ಲದಿದ್ದರೆ{
        ಹಿಂತಿರುಗಿ 0;
    }
}

 /*
 * Name: isTashkeelOnTatweelChar
 * Function: Checks if the Tashkeel Character is on Tatweel or not,if the
 *           Tashkeel on tatweel (FE range), it returns 1 else if the
 *           Tashkeel with shadda on tatweel (FC range)return 2 otherwise
 *           returns 0
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ int32_t
isTashkeelOnTatweelChar(char16_t ch){
    ಆದರೆ(ch >= 0xfe70 && ch <= 0xfe7f && ch != NEW_ಬಾಲ_CHAR && ch != 0xFE75 && ch != SHADDA_TATWEEL_CHAR)
    {
        ಹಿಂತಿರುಗಿ tashkeelMedial [ch - 0xFE70];
    }ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ( (ch >= 0xfcf2 && ch <= 0xfcf4) || (ch == SHADDA_TATWEEL_CHAR)) {
        ಹಿಂತಿರುಗಿ 2;
    }ಇಲ್ಲದಿದ್ದರೆ{
        ಹಿಂತಿರುಗಿ 0;
    }
}

/*
 * Name: isIsolatedTashkeelChar
 * Function: Checks if the Tashkeel Character is in the isolated form
 *           (i.e. Unicode FE range) returns 1 else if the Tashkeel
 *           with shadda is in the isolated form (i.e. Unicode FC range)
 *           returns 2 otherwise returns 0
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ int32_t
isIsolatedTashkeelChar(char16_t ch){
    ಆದರೆ(ch >= 0xfe70 && ch <= 0xfe7f && ch != NEW_ಬಾಲ_CHAR && ch != 0xFE75){
        ಹಿಂತಿರುಗಿ (1 - tashkeelMedial [ch - 0xFE70]);
    }ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ch >= 0xfc5e && ch <= 0xfc63){
        ಹಿಂತಿರುಗಿ 1;
    }ಇಲ್ಲದಿದ್ದರೆ{
        ಹಿಂತಿರುಗಿ 0;
    }
}




/*
 *Name     : calculateSize
 *Function : This function calculates the destSize to be used in preflighting
 *           when the destSize is equal to 0
 *           It is used also to calculate the new destsize in case the
 *           destination buffer will be resized.
 */

ಸ್ಥಿರ int32_t
calculate_ಗಾತ್ರ(ಸ್ಥಿರಾಂಕ char16_t *ಮೂಲ, int32_t ಮೂಲ_ಉದ್ದಳತೆ,
int32_t dest_ಗಾತ್ರ,uint32_t options) {
    int32_t i = 0;

    ಪೂರ್ಣಾಂಕ lamAlefOption = 0;
    ಪೂರ್ಣಾಂಕ tashkeelOption = 0;

    dest_ಗಾತ್ರ = ಮೂಲ_ಉದ್ದಳತೆ;

    ಆದರೆ (((options&U_SHAPE_LETTERS_ಮುಖವಾಡ) == U_SHAPE_LETTERS_SHAPE ||
        ((options&U_SHAPE_LETTERS_ಮುಖವಾಡ) == U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED )) &&
        ((options&U_SHAPE_LAMALEF_ಮುಖವಾಡ) == U_SHAPE_LAMALEF_RESIZE )){
            lamAlefOption = 1;
    }
    ಆದರೆ((options&U_SHAPE_LETTERS_ಮುಖವಾಡ) == U_SHAPE_LETTERS_SHAPE &&
       ((options&U_SHAPE_TASHKEEL_ಮುಖವಾಡ) == U_SHAPE_TASHKEEL_RESIZE ) ){
            tashkeelOption = 1;
        }

    ಆದರೆ(lamAlefOption || tashkeelOption){
        ಆದರೆ((options&U_SHAPE_ಪಠ್ಯ_DIRECTION_ಮುಖವಾಡ)==U_SHAPE_ಪಠ್ಯ_DIRECTION_VISUAL_LTR) {
            ಗಾಗಿ(i=0;i<ಮೂಲ_ಉದ್ದಳತೆ;i++) {
                ಆದರೆ( ((isAlefChar(ಮೂಲ[i]))&& (i<(ಮೂಲ_ಉದ್ದಳತೆ-1)) &&(ಮೂಲ[i+1] == LAM_CHAR)) || (isTashkeelCharFE(ಮೂಲ[i])) ) {
                        dest_ಗಾತ್ರ--;
                    }
                }
            }ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((options&U_SHAPE_ಪಠ್ಯ_DIRECTION_ಮುಖವಾಡ)==U_SHAPE_ಪಠ್ಯ_DIRECTION_LOGICAL) {
                ಗಾಗಿ(i=0;i<ಮೂಲ_ಉದ್ದಳತೆ;i++) {
                    ಆದರೆ( ( (ಮೂಲ[i] == LAM_CHAR) && (i<(ಮೂಲ_ಉದ್ದಳತೆ-1)) && (isAlefChar(ಮೂಲ[i+1]))) || (isTashkeelCharFE(ಮೂಲ[i])) ) {
                        dest_ಗಾತ್ರ--;
                    }
                }
            }
        }

    ಆದರೆ ((options&U_SHAPE_LETTERS_ಮುಖವಾಡ) == U_SHAPE_LETTERS_UNSHAPE){
        ಆದರೆ ( (options&U_SHAPE_LAMALEF_ಮುಖವಾಡ) == U_SHAPE_LAMALEF_RESIZE){
            ಗಾಗಿ(i=0;i<ಮೂಲ_ಉದ್ದಳತೆ;i++) {
                ಆದರೆ(isLamAlefChar(ಮೂಲ[i]))
                dest_ಗಾತ್ರ++;
            }
        }
    }

    ಹಿಂತಿರುಗಿ dest_ಗಾತ್ರ;
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
ಸ್ಥಿರ int32_t
handleTashkeelWithTatweel(char16_t *dest, int32_t ಮೂಲ_ಉದ್ದಳತೆ,
             int32_t /*destSize*/, uint32_t /*options*/,
             UError_ಸಂಕೇತ * /*pErrorCode*/) {
                 ಪೂರ್ಣಾಂಕ i;
                 ಗಾಗಿ(i = 0; i < ಮೂಲ_ಉದ್ದಳತೆ; i++){
                     ಆದರೆ((isTashkeelOnTatweelChar(dest[i]) == 1)){
                         dest[i] = TATWEEL_CHAR;
                    }ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((isTashkeelOnTatweelChar(dest[i]) == 2)){
                         dest[i] = SHADDA_TATWEEL_CHAR;
                    }ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(isIsolatedTashkeelChar(dest[i]) && dest[i] != SHADDA_CHAR){
                         dest[i] = SPACE_CHAR;
                    }
                 }
                 ಹಿಂತಿರುಗಿ ಮೂಲ_ಉದ್ದಳತೆ;
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

ಸ್ಥಿರ int32_t
handleGeneratedSpaces(char16_t *dest, int32_t ಮೂಲ_ಉದ್ದಳತೆ,
                    int32_t dest_ಗಾತ್ರ,
                    uint32_t options,
                    UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ,ರಚನೆ uShapeVariables shapeVars ) {

    int32_t i = 0, j = 0;
    int32_t count = 0;
    char16_t *tempbuffer=ಶೂನ್ಯಸೂಚಕ;

    ಪೂರ್ಣಾಂಕ lamAlefOption = 0;
    ಪೂರ್ಣಾಂಕ tashkeelOption = 0;
    ಪೂರ್ಣಾಂಕ shaping_ಬಹುಲಕ = SHAPE_ಬಹುಲಕ;

    ಆದರೆ (shaping_ಬಹುಲಕ == 0){
        ಆದರೆ ( (options&U_SHAPE_LAMALEF_ಮುಖವಾಡ) == U_SHAPE_LAMALEF_RESIZE ){
            lamAlefOption = 1;
        }
        ಆದರೆ ( (options&U_SHAPE_TASHKEEL_ಮುಖವಾಡ) == U_SHAPE_TASHKEEL_RESIZE ){
            tashkeelOption = 1;
        }
    }

    ಆದರೆ (static_cast<size_t>(ಮೂಲ_ಉದ್ದಳತೆ) + 1 > std::numeric_limits<size_t>::max() / U_SIZEOF_UCHAR) {
        *p_Error_ಸಂಕೇತ = U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }
    tempbuffer = static_cast<char16_t*>(uprv_malloc((ಮೂಲ_ಉದ್ದಳತೆ + 1) * U_SIZEOF_UCHAR));
    /* Test for nullptr */
    ಆದರೆ(tempbuffer == ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }


    ಆದರೆ (lamAlefOption || tashkeelOption){
        uprv_memset(tempbuffer, 0, (ಮೂಲ_ಉದ್ದಳತೆ+1)*U_SIZEOF_UCHAR);

        i = j = 0; count = 0;
        ಆಗ(i < ಮೂಲ_ಉದ್ದಳತೆ) {
            ಆದರೆ ( (lamAlefOption && dest[i] == LAMALEF_SPACE_SUB) ||
               (tashkeelOption && dest[i] == TASHKEEL_SPACE_SUB) ){
                j--;
                count++;
            } ಇಲ್ಲದಿದ್ದರೆ {
                tempbuffer[j] = dest[i];
            }
            i++;
            j++;
        }

        ಆಗ(count >= 0) {
            tempbuffer[i] = 0x0000;
            i--;
            count--;
        }

        u_memcpy(dest, tempbuffer, ಮೂಲ_ಉದ್ದಳತೆ);
        dest_ಗಾತ್ರ = u_strlen(dest);
    }

      lamAlefOption = 0;

    ಆದರೆ (shaping_ಬಹುಲಕ == 0){
        ಆದರೆ ( (options&U_SHAPE_LAMALEF_ಮುಖವಾಡ) == U_SHAPE_LAMALEF_NEAR ){
            lamAlefOption = 1;
        }
    }

    ಆದರೆ (lamAlefOption){
        /* Lam+Alef is already shaped into LamAlef + FFFF */
        i = 0;
        ಆಗ(i < ಮೂಲ_ಉದ್ದಳತೆ) {
            ಆದರೆ(lamAlefOption&&dest[i] == LAMALEF_SPACE_SUB){
                dest[i] = SPACE_CHAR;
            }
            i++;
        }
        dest_ಗಾತ್ರ = ಮೂಲ_ಉದ್ದಳತೆ;
    }
    lamAlefOption = 0;
    tashkeelOption = 0;

    ಆದರೆ (shaping_ಬಹುಲಕ == 0) {
        ಆದರೆ ( ((options&U_SHAPE_LAMALEF_ಮುಖವಾಡ) == shapeVars.uShapeLamalefBegin) ||
              (((options&U_SHAPE_LAMALEF_ಮುಖವಾಡ) == U_SHAPE_LAMALEF_AUTO )
              && (shapeVars.spaces_Relative_ಗಮ್ಯ_ಪಠ್ಯ_Begin_ಕೊನೆ==1)) ) {
            lamAlefOption = 1;
        }
        ಆದರೆ ( (options&U_SHAPE_TASHKEEL_ಮುಖವಾಡ) == shapeVars.uShapeTashkeelBegin ) {
            tashkeelOption = 1;
        }
    }

    ಆದರೆ(lamAlefOption || tashkeelOption){
        uprv_memset(tempbuffer, 0, (ಮೂಲ_ಉದ್ದಳತೆ+1)*U_SIZEOF_UCHAR);
        
        i = j = ಮೂಲ_ಉದ್ದಳತೆ; count = 0;
        
        ಆಗ(i >= 0) {
            ಆದರೆ ( (lamAlefOption && dest[i] == LAMALEF_SPACE_SUB) ||
                 (tashkeelOption && dest[i] == TASHKEEL_SPACE_SUB) ){
                j++;
                count++;
            }ಇಲ್ಲದಿದ್ದರೆ {
                tempbuffer[j] = dest[i];
            }
            i--;
            j--;
        }

        ಗಾಗಿ(i=0 ;i < count; i++){
                tempbuffer[i] = SPACE_CHAR;
        }

        u_memcpy(dest, tempbuffer, ಮೂಲ_ಉದ್ದಳತೆ);
        dest_ಗಾತ್ರ = ಮೂಲ_ಉದ್ದಳತೆ;
    }



    lamAlefOption = 0;
    tashkeelOption = 0;

    ಆದರೆ (shaping_ಬಹುಲಕ == 0) {
        ಆದರೆ ( ((options&U_SHAPE_LAMALEF_ಮುಖವಾಡ) == shapeVars.u_Shape_Lamalef_ಕೊನೆ) ||
              (((options&U_SHAPE_LAMALEF_ಮುಖವಾಡ) == U_SHAPE_LAMALEF_AUTO )
              && (shapeVars.spaces_Relative_ಗಮ್ಯ_ಪಠ್ಯ_Begin_ಕೊನೆ==0)) ) {
            lamAlefOption = 1;
        }
        ಆದರೆ ( (options&U_SHAPE_TASHKEEL_ಮುಖವಾಡ) == shapeVars.u_Shape_Tashkeel_ಕೊನೆ ){
            tashkeelOption = 1;
        }
    }

    ಆದರೆ(lamAlefOption || tashkeelOption){
        uprv_memset(tempbuffer, 0, (ಮೂಲ_ಉದ್ದಳತೆ+1)*U_SIZEOF_UCHAR);

        i = j = 0; count = 0;
        ಆಗ(i < ಮೂಲ_ಉದ್ದಳತೆ) {
            ಆದರೆ ( (lamAlefOption && dest[i] == LAMALEF_SPACE_SUB) ||
                 (tashkeelOption && dest[i] == TASHKEEL_SPACE_SUB) ){
                j--;
                count++;
            }ಇಲ್ಲದಿದ್ದರೆ {
                tempbuffer[j] = dest[i];
            }
            i++;
            j++;
        }

        ಆಗ(count >= 0) {
            tempbuffer[i] = SPACE_CHAR;
            i--;
            count--;
        }

        u_memcpy(dest, tempbuffer, ಮೂಲ_ಉದ್ದಳತೆ);
        dest_ಗಾತ್ರ = ಮೂಲ_ಉದ್ದಳತೆ;
    }


    ಆದರೆ(tempbuffer){
        uprv_free(tempbuffer);
    }

    ಹಿಂತಿರುಗಿ dest_ಗಾತ್ರ;
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

ಸ್ಥಿರ int32_t
expandCompositCharAtBegin(char16_t *dest, int32_t ಮೂಲ_ಉದ್ದಳತೆ, int32_t dest_ಗಾತ್ರ,UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    int32_t      i = 0,j = 0;
    int32_t      countl = 0;
    char16_t *tempbuffer=ಶೂನ್ಯಸೂಚಕ;

    tempbuffer = static_cast<char16_t*>(uprv_malloc((ಮೂಲ_ಉದ್ದಳತೆ + 1) * U_SIZEOF_UCHAR));

    /* Test for nullptr */
    ಆದರೆ(tempbuffer == ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

        uprv_memset(tempbuffer, 0, (ಮೂಲ_ಉದ್ದಳತೆ+1)*U_SIZEOF_UCHAR);

        i = 0;
        ಆಗ(dest[i] == SPACE_CHAR) {
            countl++;
            i++;
        }

        i = j = ಮೂಲ_ಉದ್ದಳತೆ-1;

        ಆಗ(i >= 0 && j >= 0) {
            ಆದರೆ( countl>0 && isLamAlefChar(dest[i])) {
                tempbuffer[j] = LAM_CHAR;
                /* to ensure the array index is within the range */
                U_ASSERT(dest[i] >= 0xFEF5u
                    && dest[i]-0xFEF5u < UPRV_LENGTHOF(convertLamAlef));
                tempbuffer[j-1] = convertLamAlef[ dest[i] - 0xFEF5 ];
                j--;
                countl--;
            }ಇಲ್ಲದಿದ್ದರೆ {
                 ಆದರೆ( countl == 0 && isLamAlefChar(dest[i]) ) {
                     *p_Error_ಸಂಕೇತ=U_NO_SPACE_ಲಭ್ಯ;
                     }
                 tempbuffer[j] = dest[i];
            }
            i--;
            j--;
        }
        u_memcpy(dest, tempbuffer, ಮೂಲ_ಉದ್ದಳತೆ);

        uprv_free(tempbuffer);

        dest_ಗಾತ್ರ = ಮೂಲ_ಉದ್ದಳತೆ;
        ಹಿಂತಿರುಗಿ dest_ಗಾತ್ರ;
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

ಸ್ಥಿರ int32_t
expand_Composit_Char_At_ಕೊನೆ(char16_t *dest, int32_t ಮೂಲ_ಉದ್ದಳತೆ, int32_t dest_ಗಾತ್ರ,UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    int32_t      i = 0,j = 0;

    int32_t      countr = 0;
    int32_t  inpsize = ಮೂಲ_ಉದ್ದಳತೆ;

    char16_t *tempbuffer=ಶೂನ್ಯಸೂಚಕ;
    tempbuffer = static_cast<char16_t*>(uprv_malloc((ಮೂಲ_ಉದ್ದಳತೆ + 1) * U_SIZEOF_UCHAR));

    /* Test for nullptr */
    ಆದರೆ(tempbuffer == ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
         ಹಿಂತಿರುಗಿ 0;
    }

    uprv_memset(tempbuffer, 0, (ಮೂಲ_ಉದ್ದಳತೆ+1)*U_SIZEOF_UCHAR);

    ಆಗ(inpsize>0 && dest[inpsize-1] == SPACE_CHAR) {
        countr++;
        inpsize--;
    }

    i = ಮೂಲ_ಉದ್ದಳತೆ - countr - 1;
    j = ಮೂಲ_ಉದ್ದಳತೆ - 1;

    ಆಗ(i >= 0 && j >= 0) {
        ಆದರೆ( countr>0 && isLamAlefChar(dest[i]) ) {
            tempbuffer[j] = LAM_CHAR;
            tempbuffer[j-1] = convertLamAlef[ dest[i] - 0xFEF5 ];
            j--;
            countr--;
        }ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ ((countr == 0) && isLamAlefChar(dest[i]) ) {
                *p_Error_ಸಂಕೇತ=U_NO_SPACE_ಲಭ್ಯ;
            }
            tempbuffer[j] = dest[i];
        }
        i--;
        j--;
    }

    ಆದರೆ(countr > 0) {
        u_memmove(tempbuffer, tempbuffer+countr, ಮೂಲ_ಉದ್ದಳತೆ);
        ಆದರೆ(u_strlen(tempbuffer) < ಮೂಲ_ಉದ್ದಳತೆ) {
            ಗಾಗಿ(i=ಮೂಲ_ಉದ್ದಳತೆ-1;i>=ಮೂಲ_ಉದ್ದಳತೆ-countr;i--) {
                tempbuffer[i] = SPACE_CHAR;
            }
        }
    }
    u_memcpy(dest, tempbuffer, ಮೂಲ_ಉದ್ದಳತೆ);

    uprv_free(tempbuffer);

    dest_ಗಾತ್ರ = ಮೂಲ_ಉದ್ದಳತೆ;
    ಹಿಂತಿರುಗಿ dest_ಗಾತ್ರ;
}

/*
 *Name     : expandCompositCharAtNear
 *Function : Expands the LamAlef character into Lam + Alef, YehHamza character
 *           into Yeh + Hamza, SeenFamily character into SeenFamily character
 *           + Tail, while consuming the space next to the character.
 *           If there are no spaces next to the character, an error
 *           will be set to U_NO_SPACE_AVAILABLE as defined in utypes.h
 */

ಸ್ಥಿರ int32_t
expandCompositCharAtNear(char16_t *dest, int32_t ಮೂಲ_ಉದ್ದಳತೆ, int32_t dest_ಗಾತ್ರ,UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ,
                         ಪೂರ್ಣಾಂಕ yehHamzaOption, ಪೂರ್ಣಾಂಕ ನೋಡಿದ_ಬಾಲ_Option, ಪೂರ್ಣಾಂಕ lamAlefOption, ರಚನೆ uShapeVariables shapeVars) {
    int32_t      i = 0;


    char16_t lamalefChar, yehhamzaChar;

    ಗಾಗಿ(i = 0 ;i<=ಮೂಲ_ಉದ್ದಳತೆ-1;i++) {
            ಆದರೆ (ನೋಡಿದ_ಬಾಲ_Option && is_ನೋಡಿದ_ಬಾಲ_Family_Char(dest[i])) {
                ಆದರೆ ((i>0) && (dest[i-1] == SPACE_CHAR) ) {
                    dest[i-1] = shapeVars.ಬಾಲ_Char;
                }ಇಲ್ಲದಿದ್ದರೆ {
                    *p_Error_ಸಂಕೇತ=U_NO_SPACE_ಲಭ್ಯ;
                }
            }ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(yehHamzaOption && (isYehHamzaChar(dest[i])) ) {
                ಆದರೆ ((i>0) && (dest[i-1] == SPACE_CHAR) ) {
                    yehhamzaChar = dest[i];
                    dest[i] = yeh_Hamza_ಗಮ್ಯ_Yeh[yehhamzaChar - YEH_HAMZAFE_CHAR];
                    dest[i-1] = HAMZAFE_CHAR;
                }ಇಲ್ಲದಿದ್ದರೆ {

                    *p_Error_ಸಂಕೇತ=U_NO_SPACE_ಲಭ್ಯ;
                }
            }ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(lamAlefOption && isLamAlefChar(dest[i+1])) {
                ಆದರೆ(dest[i] == SPACE_CHAR){
                    lamalefChar = dest[i+1];
                    dest[i+1] = LAM_CHAR;
                    dest[i] = convertLamAlef[ lamalefChar - 0xFEF5 ];
                }ಇಲ್ಲದಿದ್ದರೆ {
                    *p_Error_ಸಂಕೇತ=U_NO_SPACE_ಲಭ್ಯ;
                }
            }
       }
       dest_ಗಾತ್ರ = ಮೂಲ_ಉದ್ದಳತೆ;
       ಹಿಂತಿರುಗಿ dest_ಗಾತ್ರ;
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

ಸ್ಥಿರ int32_t
expandCompositChar(char16_t *dest, int32_t ಮೂಲ_ಉದ್ದಳತೆ,
              int32_t dest_ಗಾತ್ರ,uint32_t options,
              UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ, ಪೂರ್ಣಾಂಕ shaping_ಬಹುಲಕ,ರಚನೆ uShapeVariables shapeVars) {

    int32_t      i = 0,j = 0;

    char16_t *tempbuffer=ಶೂನ್ಯಸೂಚಕ;
    ಪೂರ್ಣಾಂಕ yehHamzaOption = 0;
    ಪೂರ್ಣಾಂಕ ನೋಡಿದ_ಬಾಲ_Option = 0;
    ಪೂರ್ಣಾಂಕ lamAlefOption = 0;

    ಆದರೆ (shaping_ಬಹುಲಕ == 1){
        ಆದರೆ ( (options&U_SHAPE_LAMALEF_ಮುಖವಾಡ) == U_SHAPE_LAMALEF_AUTO){

            ಆದರೆ(shapeVars.spaces_Relative_ಗಮ್ಯ_ಪಠ್ಯ_Begin_ಕೊನೆ == 0) {
                dest_ಗಾತ್ರ = expand_Composit_Char_At_ಕೊನೆ(dest, ಮೂಲ_ಉದ್ದಳತೆ, dest_ಗಾತ್ರ, p_Error_ಸಂಕೇತ);

                ಆದರೆ(*p_Error_ಸಂಕೇತ == U_NO_SPACE_ಲಭ್ಯ) {
                    *p_Error_ಸಂಕೇತ = U_ZERO_ERROR;
                    dest_ಗಾತ್ರ = expandCompositCharAtBegin(dest, ಮೂಲ_ಉದ್ದಳತೆ, dest_ಗಾತ್ರ, p_Error_ಸಂಕೇತ);
                }
            }ಇಲ್ಲದಿದ್ದರೆ {
                dest_ಗಾತ್ರ = expandCompositCharAtBegin(dest, ಮೂಲ_ಉದ್ದಳತೆ, dest_ಗಾತ್ರ, p_Error_ಸಂಕೇತ);

                ಆದರೆ(*p_Error_ಸಂಕೇತ == U_NO_SPACE_ಲಭ್ಯ) {
                    *p_Error_ಸಂಕೇತ = U_ZERO_ERROR;
                    dest_ಗಾತ್ರ = expand_Composit_Char_At_ಕೊನೆ(dest, ಮೂಲ_ಉದ್ದಳತೆ, dest_ಗಾತ್ರ, p_Error_ಸಂಕೇತ);
                }
            }

            ಆದರೆ(*p_Error_ಸಂಕೇತ == U_NO_SPACE_ಲಭ್ಯ) {
                *p_Error_ಸಂಕೇತ = U_ZERO_ERROR;
                dest_ಗಾತ್ರ = expandCompositCharAtNear(dest, ಮೂಲ_ಉದ್ದಳತೆ, dest_ಗಾತ್ರ, p_Error_ಸಂಕೇತ, yehHamzaOption,
                                                ನೋಡಿದ_ಬಾಲ_Option, 1,shapeVars);
            }
        }
    }

    ಆದರೆ (shaping_ಬಹುಲಕ == 1){
        ಆದರೆ ( (options&U_SHAPE_LAMALEF_ಮುಖವಾಡ) == shapeVars.u_Shape_Lamalef_ಕೊನೆ){
            dest_ಗಾತ್ರ = expand_Composit_Char_At_ಕೊನೆ(dest, ಮೂಲ_ಉದ್ದಳತೆ, dest_ಗಾತ್ರ, p_Error_ಸಂಕೇತ);
        }
    }

    ಆದರೆ (shaping_ಬಹುಲಕ == 1){
        ಆದರೆ ( (options&U_SHAPE_LAMALEF_ಮುಖವಾಡ) == shapeVars.uShapeLamalefBegin){
            dest_ಗಾತ್ರ = expandCompositCharAtBegin(dest, ಮೂಲ_ಉದ್ದಳತೆ, dest_ಗಾತ್ರ, p_Error_ಸಂಕೇತ);
        }
    }

    ಆದರೆ (shaping_ಬಹುಲಕ == 0){
         ಆದರೆ ((options&U_SHAPE_YEHHAMZA_ಮುಖವಾಡ) == U_SHAPE_YEHHAMZA_TWOCELL_NEAR){
             yehHamzaOption = 1;
         }
         ಆದರೆ ((options&U_SHAPE_ನೋಡಿದ_ಮುಖವಾಡ) == U_SHAPE_ನೋಡಿದ_TWOCELL_NEAR){
            ನೋಡಿದ_ಬಾಲ_Option = 1;
         }
    }
    ಆದರೆ (shaping_ಬಹುಲಕ == 1) {
        ಆದರೆ ( (options&U_SHAPE_LAMALEF_ಮುಖವಾಡ) == U_SHAPE_LAMALEF_NEAR) {
            lamAlefOption = 1;
        }
    }


    ಆದರೆ (yehHamzaOption || ನೋಡಿದ_ಬಾಲ_Option || lamAlefOption){
        dest_ಗಾತ್ರ = expandCompositCharAtNear(dest, ಮೂಲ_ಉದ್ದಳತೆ, dest_ಗಾತ್ರ, p_Error_ಸಂಕೇತ, yehHamzaOption,
                                            ನೋಡಿದ_ಬಾಲ_Option,lamAlefOption,shapeVars);
    }


    ಆದರೆ (shaping_ಬಹುಲಕ == 1){
        ಆದರೆ ( (options&U_SHAPE_LAMALEF_ಮುಖವಾಡ) == U_SHAPE_LAMALEF_RESIZE){
            dest_ಗಾತ್ರ = calculate_ಗಾತ್ರ(dest,ಮೂಲ_ಉದ್ದಳತೆ,dest_ಗಾತ್ರ,options);
            tempbuffer = static_cast<char16_t*>(uprv_malloc((dest_ಗಾತ್ರ + 1) * U_SIZEOF_UCHAR));

            /* Test for nullptr */
            ಆದರೆ(tempbuffer == ಶೂನ್ಯಸೂಚಕ) {
                *p_Error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                ಹಿಂತಿರುಗಿ 0;
            }

            uprv_memset(tempbuffer, 0, (dest_ಗಾತ್ರ+1)*U_SIZEOF_UCHAR);

            i = j = 0;
            ಆಗ(i < dest_ಗಾತ್ರ && j < dest_ಗಾತ್ರ) {
                ಆದರೆ(isLamAlefChar(dest[i]) ) {
                    tempbuffer[j] = convertLamAlef[ dest[i] - 0xFEF5 ];
                    tempbuffer[j+1] = LAM_CHAR;
                    j++;
                }ಇಲ್ಲದಿದ್ದರೆ {
                    tempbuffer[j] = dest[i];
                }
                i++;
                j++;
            }

            u_memcpy(dest, tempbuffer, dest_ಗಾತ್ರ);
        }
    }

    ಆದರೆ(tempbuffer) {
        uprv_free(tempbuffer);
    }
    ಹಿಂತಿರುಗಿ dest_ಗಾತ್ರ;
}

/*
 *Name     : shapeUnicode
 *Function : Converts an Arabic Unicode buffer in 06xx Range into a shaped
 *           arabic Unicode buffer in FExx Range
 */
ಸ್ಥಿರ int32_t
shapeUnicode(char16_t *dest, int32_t ಮೂಲ_ಉದ್ದಳತೆ,
             int32_t dest_ಗಾತ್ರ,uint32_t options,
             UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ,
             ಪೂರ್ಣಾಂಕ tashkeelFlag, ರಚನೆ uShapeVariables shapeVars) {

    int32_t          i, iend;
    int32_t          ಹೆಜ್ಜೆ;
    int32_t          ಕೊನೆಯ_Pos,Nx, Nw;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ     Shape;
    int32_t          lamalef_ಸಿಕ್ಕಿತು = 0;
    int32_t seenfam_ಸಿಕ್ಕಿತು = 0, yehhamza_ಸಿಕ್ಕಿತು =0, tashkeel_ಸಿಕ್ಕಿತು  = 0;
    char16_t         prevLink = 0, ಕೊನೆಯ_Link = 0, currLink, ಮುಂದಿನ_Link = 0;
    char16_t         wLamalef;

    /*
     * Converts the input buffer from FExx Range into 06xx Range
     * to make sure that all characters are in the 06xx range
     * even the lamalef is converted to the special region in
     * the 06xx range
     */
    ಆದರೆ ((options & U_SHAPE_PRESERVE_PRESENTATION_ಮುಖವಾಡ)  == U_SHAPE_PRESERVE_PRESENTATION_NOOP) {
        ಗಾಗಿ (i = 0; i < ಮೂಲ_ಉದ್ದಳತೆ; i++) {
            char16_t ಒಳಹರಿವು_Char  = dest[i];
            ಆದರೆ ( (ಒಳಹರಿವು_Char >= 0xFB50) && (ಒಳಹರಿವು_Char <= 0xFBFF)) {
                char16_t c = convertFBto06 [ (ಒಳಹರಿವು_Char - 0xFB50) ];
                ಆದರೆ (c != 0)
                    dest[i] = c;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ( (ಒಳಹರಿವು_Char >= 0xFE70) && (ಒಳಹರಿವು_Char <= 0xFEFC)) {
                dest[i] = convertFEto06 [ (ಒಳಹರಿವು_Char - 0xFE70) ] ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                dest[i] = ಒಳಹರಿವು_Char ;
            }
        }
    }


    /* sets the index to the end of the buffer, together with the step point to -1 */
    i = ಮೂಲ_ಉದ್ದಳತೆ - 1;
    iend = -1;
    ಹೆಜ್ಜೆ = -1;

    /*
     * This function resolves the link between the characters .
     * Arabic characters have four forms :
     * Isolated Form, Initial Form, Middle Form and Final Form
     */
    currLink = getLink(dest[i]);

    ಕೊನೆಯ_Pos = i;
    Nx = -2, Nw = 0;

    ಆಗ (i != iend) {
        /* If high byte of currLink > 0 then more than one shape */
        ಆದರೆ ((currLink & 0xFF00) > 0 || (getLink(dest[i]) & IRRELEVANT) != 0) {
            Nw = i + ಹೆಜ್ಜೆ;
            ಆಗ (Nx < 0) {         /* we need to know about next char */
                ಆದರೆ(Nw == iend) {
                    ಮುಂದಿನ_Link = 0;
                    Nx = 3000;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ಮುಂದಿನ_Link = getLink(dest[Nw]);
                    ಆದರೆ((ಮುಂದಿನ_Link & IRRELEVANT) == 0) {
                        Nx = Nw;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        Nw = Nw + ಹೆಜ್ಜೆ;
                    }
                }
            }

            ಆದರೆ ( ((currLink & ALEFTYPE) > 0)  &&  ((ಕೊನೆಯ_Link & LAMTYPE) > 0) ) {
                lamalef_ಸಿಕ್ಕಿತು = 1;
                wLamalef = changeLamAlef(dest[i]); /*get from 0x065C-0x065f */
                ಆದರೆ ( wLamalef != 0) {
                    dest[i] = LAMALEF_SPACE_SUB;            /* The default case is to drop the Alef and replace */
                    dest[ಕೊನೆಯ_Pos] =wLamalef;     /* it by LAMALEF_SPACE_SUB which is the last character in the  */
                    i=ಕೊನೆಯ_Pos;                   /* unicode private use area, this is done to make   */
                }                                /* sure that removeLamAlefSpaces() handles only the */
                ಕೊನೆಯ_Link = prevLink;             /* spaces generated during lamalef generation.      */
                currLink = getLink(wLamalef);    /* LAMALEF_SPACE_SUB is added here and is replaced by spaces   */
            }                                    /* in removeLamAlefSpaces()                         */

            ಆದರೆ ((i > 0) && (dest[i-1] == SPACE_CHAR)){
                ಆದರೆ ( is_ನೋಡಿದ_Family_Char(dest[i])) {
                    seenfam_ಸಿಕ್ಕಿತು = 1;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dest[i] == YEH_HAMZA_CHAR) {
                    yehhamza_ಸಿಕ್ಕಿತು = 1;
                }
            }
            ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(i==0){
                ಆದರೆ ( is_ನೋಡಿದ_Family_Char(dest[i])){
                    seenfam_ಸಿಕ್ಕಿತು = 1;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dest[i] == YEH_HAMZA_CHAR) {
                    yehhamza_ಸಿಕ್ಕಿತು = 1;
                }
            }

            /*
             * get the proper shape according to link ability of neighbors
             * and of character; depends on the order of the shapes
             * (isolated, initial, middle, final) in the compatibility area
             */
            Shape = shape_ಕೋಷ್ಟಕ[ಮುಂದಿನ_Link & (LINKR + LINKL)]
                              [ಕೊನೆಯ_Link & (LINKR + LINKL)]
                              [currLink & (LINKR + LINKL)];

            ಆದರೆ ((currLink & (LINKR+LINKL)) == 1) {
                Shape &= 1;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(isTashkeelChar(dest[i])) {
                ಆದರೆ( (ಕೊನೆಯ_Link & LINKL) && (ಮುಂದಿನ_Link & LINKR) && (tashkeelFlag == 1) &&
                     dest[i] != 0x064C && dest[i] != 0x064D )
                {
                    Shape = 1;
                    ಆದರೆ( (ಮುಂದಿನ_Link&ALEFTYPE) == ALEFTYPE && (ಕೊನೆಯ_Link&LAMTYPE) == LAMTYPE ) {
                        Shape = 0;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(tashkeelFlag == 2 && dest[i] == SHADDA06_CHAR){
                    Shape = 1;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    Shape = 0;
                }
            }
            ಆದರೆ ((dest[i] ^ 0x0600) < 0x100) {
                ಆದರೆ ( isTashkeelChar(dest[i]) ){
                    ಆದರೆ (tashkeelFlag == 2  && dest[i] != SHADDA06_CHAR){
                        dest[i] = TASHKEEL_SPACE_SUB;
                        tashkeel_ಸಿಕ್ಕಿತು  = 1;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        /* to ensure the array index is within the range */
                        U_ASSERT(dest[i] >= 0x064Bu
                            && dest[i]-0x064Bu < UPRV_LENGTHOF(IrrelevantPos));
                        dest[i] =  0xFE70 + IrrelevantPos[(dest[i] - 0x064B)] + static_cast<char16_t>(Shape);
                    }
                }ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((currLink & APRESENT) > 0) {
                    dest[i] = static_cast<char16_t>(0xFB50 + (currLink >> 8) + Shape);
                }ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((currLink >> 8) > 0 && (currLink & IRRELEVANT) == 0) {
                    dest[i] = static_cast<char16_t>(0xFE70 + (currLink >> 8) + Shape);
                }
            }
        }

        /* move one notch forward */
        ಆದರೆ ((currLink & IRRELEVANT) == 0) {
            prevLink = ಕೊನೆಯ_Link;
            ಕೊನೆಯ_Link = currLink;
            ಕೊನೆಯ_Pos = i;
        }

        i = i + ಹೆಜ್ಜೆ;
        ಆದರೆ (i == Nx) {
            currLink = ಮುಂದಿನ_Link;
            Nx = -2;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(i != iend) {
            currLink = getLink(dest[i]);
        }
    }
    dest_ಗಾತ್ರ = ಮೂಲ_ಉದ್ದಳತೆ;
    ಆದರೆ ( (lamalef_ಸಿಕ್ಕಿತು != 0 ) || (tashkeel_ಸಿಕ್ಕಿತು  != 0) ){
        dest_ಗಾತ್ರ = handleGeneratedSpaces(dest,ಮೂಲ_ಉದ್ದಳತೆ,dest_ಗಾತ್ರ,options,p_Error_ಸಂಕೇತ, shapeVars);
    }

    ಆದರೆ ( (seenfam_ಸಿಕ್ಕಿತು != 0) || (yehhamza_ಸಿಕ್ಕಿತು != 0) ) {
        dest_ಗಾತ್ರ = expandCompositChar(dest, ಮೂಲ_ಉದ್ದಳತೆ,dest_ಗಾತ್ರ,options,p_Error_ಸಂಕೇತ, SHAPE_ಬಹುಲಕ,shapeVars);
    }
    ಹಿಂತಿರುಗಿ dest_ಗಾತ್ರ;
}

/*
 *Name     : deShapeUnicode
 *Function : Converts an Arabic Unicode buffer in FExx Range into unshaped
 *           arabic Unicode buffer in 06xx Range
 */
ಸ್ಥಿರ int32_t
deShapeUnicode(char16_t *dest, int32_t ಮೂಲ_ಉದ್ದಳತೆ,
               int32_t dest_ಗಾತ್ರ,uint32_t options,
               UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ, ರಚನೆ uShapeVariables shapeVars) {
    int32_t i = 0;
    int32_t lamalef_ಸಿಕ್ಕಿತು = 0;
    int32_t yehHamzaComposeEnabled = 0;
    int32_t ನೋಡಿದ_Compose_Enabled = 0;

    yehHamzaComposeEnabled = ((options&U_SHAPE_YEHHAMZA_ಮುಖವಾಡ) == U_SHAPE_YEHHAMZA_TWOCELL_NEAR) ? 1 : 0;
    ನೋಡಿದ_Compose_Enabled = ((options&U_SHAPE_ನೋಡಿದ_ಮುಖವಾಡ) == U_SHAPE_ನೋಡಿದ_TWOCELL_NEAR)? 1 : 0;

    /*
     *This for loop changes the buffer from the Unicode FE range to
     *the Unicode 06 range
     */

    ಗಾಗಿ(i = 0; i < ಮೂಲ_ಉದ್ದಳತೆ; i++) {
        char16_t  ಒಳಹರಿವು_Char = dest[i];
        ಆದರೆ ( (ಒಳಹರಿವು_Char >= 0xFB50) && (ಒಳಹರಿವು_Char <= 0xFBFF)) { /* FBxx Arabic range */
            char16_t c = convertFBto06 [ (ಒಳಹರಿವು_Char - 0xFB50) ];
            ಆದರೆ (c != 0)
                dest[i] = c;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ( (yehHamzaComposeEnabled == 1) && ((ಒಳಹರಿವು_Char == HAMZA06_CHAR) || (ಒಳಹರಿವು_Char == HAMZAFE_CHAR))
                && (i < (ಮೂಲ_ಉದ್ದಳತೆ - 1)) && isAlefMaksouraChar(dest[i+1] )) {
                 dest[i] = SPACE_CHAR;
                 dest[i+1] = YEH_HAMZA_CHAR;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ( (ನೋಡಿದ_Compose_Enabled == 1) && (is_ಬಾಲ_Char(ಒಳಹರಿವು_Char)) && (i< (ಮೂಲ_ಉದ್ದಳತೆ - 1))
                        && (is_ನೋಡಿದ_ಬಾಲ_Family_Char(dest[i+1])) ) {
                dest[i] = SPACE_CHAR;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (( ಒಳಹರಿವು_Char >= 0xFE70) && (ಒಳಹರಿವು_Char <= 0xFEF4 )) { /* FExx Arabic range */
                dest[i] = convertFEto06 [ (ಒಳಹರಿವು_Char - 0xFE70) ];
        } ಇಲ್ಲದಿದ್ದರೆ {
            dest[i] = ಒಳಹರಿವು_Char ;
        }

        ಆದರೆ( isLamAlefChar(dest[i]) )
            lamalef_ಸಿಕ್ಕಿತು = 1;
    }

   dest_ಗಾತ್ರ = ಮೂಲ_ಉದ್ದಳತೆ;
   ಆದರೆ (lamalef_ಸಿಕ್ಕಿತು != 0){
          dest_ಗಾತ್ರ = expandCompositChar(dest,ಮೂಲ_ಉದ್ದಳತೆ,dest_ಗಾತ್ರ,options,p_Error_ಸಂಕೇತ,DESHAPE_ಬಹುಲಕ, shapeVars);
   }
   ಹಿಂತಿರುಗಿ dest_ಗಾತ್ರ;
}

/*
 ****************************************
 * u_shapeArabic
 ****************************************
 */

U_CAPI int32_t U_EXPORT2
u_shapeArabic(ಸ್ಥಿರಾಂಕ char16_t *ಮೂಲ, int32_t ಮೂಲ_ಉದ್ದಳತೆ,
              char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
              uint32_t options,
              UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {

    int32_t dest_ಉದ್ದಳತೆ;
    ರಚನೆ  uShapeVariables shapeVars = { OLD_ಬಾಲ_CHAR,U_SHAPE_LAMALEF_BEGIN,U_SHAPE_LAMALEF_ಕೊನೆ,U_SHAPE_TASHKEEL_BEGIN,U_SHAPE_TASHKEEL_ಕೊನೆ,0};

    /* usual error checking */
    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }

    /* make sure that no reserved options values are used; allow dest==nullptr only for preflighting */
    ಆದರೆ( ಮೂಲ==ಶೂನ್ಯಸೂಚಕ || ಮೂಲ_ಉದ್ದಳತೆ<-1 || (dest==ಶೂನ್ಯಸೂಚಕ && dest_ಸಾಮರ್ಥ್ಯ!=0) || dest_ಸಾಮರ್ಥ್ಯ<0 ||
                (((options&U_SHAPE_TASHKEEL_ಮುಖವಾಡ) > 0) &&
                 ((options&U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED) == U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED) ) ||
                (((options&U_SHAPE_TASHKEEL_ಮುಖವಾಡ) > 0) &&
                 ((options&U_SHAPE_LETTERS_ಮುಖವಾಡ) == U_SHAPE_LETTERS_UNSHAPE)) ||
                (options&U_SHAPE_DIGIT_TYPE_RESERVED)==U_SHAPE_DIGIT_TYPE_RESERVED ||
                (options&U_SHAPE_DIGITS_ಮುಖವಾಡ)==U_SHAPE_DIGITS_RESERVED ||
                ((options&U_SHAPE_LAMALEF_ಮುಖವಾಡ) != U_SHAPE_LAMALEF_RESIZE  &&
                (options&U_SHAPE_AGGREGATE_TASHKEEL_ಮುಖವಾಡ) != 0) ||
                ((options&U_SHAPE_AGGREGATE_TASHKEEL_ಮುಖವಾಡ) == U_SHAPE_AGGREGATE_TASHKEEL &&
                (options&U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED) != U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED)
    )
    {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }
    /* Validate  lamalef options */
    ಆದರೆ(((options&U_SHAPE_LAMALEF_ಮುಖವಾಡ) > 0)&&
              !(((options & U_SHAPE_LAMALEF_ಮುಖವಾಡ)==U_SHAPE_LAMALEF_BEGIN) ||
                ((options & U_SHAPE_LAMALEF_ಮುಖವಾಡ)==U_SHAPE_LAMALEF_ಕೊನೆ ) ||
                ((options & U_SHAPE_LAMALEF_ಮುಖವಾಡ)==U_SHAPE_LAMALEF_RESIZE )||
                 ((options & U_SHAPE_LAMALEF_ಮುಖವಾಡ)==U_SHAPE_LAMALEF_AUTO) ||
                 ((options & U_SHAPE_LAMALEF_ಮುಖವಾಡ)==U_SHAPE_LAMALEF_NEAR)))
    {
         *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }
    /* Validate  Tashkeel options */
    ಆದರೆ(((options&U_SHAPE_TASHKEEL_ಮುಖವಾಡ) > 0)&&
                   !(((options & U_SHAPE_TASHKEEL_ಮುಖವಾಡ)==U_SHAPE_TASHKEEL_BEGIN) ||
                     ((options & U_SHAPE_TASHKEEL_ಮುಖವಾಡ)==U_SHAPE_TASHKEEL_ಕೊನೆ )
                    ||((options & U_SHAPE_TASHKEEL_ಮುಖವಾಡ)==U_SHAPE_TASHKEEL_RESIZE )||
                    ((options & U_SHAPE_TASHKEEL_ಮುಖವಾಡ)==U_SHAPE_TASHKEEL_REPLACE_BY_TATWEEL)))
    {
         *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }
    /* determine the source length */
    ಆದರೆ(ಮೂಲ_ಉದ್ದಳತೆ==-1) {
        ಮೂಲ_ಉದ್ದಳತೆ=u_strlen(ಮೂಲ);
    }
    ಆದರೆ(ಮೂಲ_ಉದ್ದಳತೆ<=0) {
        ಹಿಂತಿರುಗಿ u_terminateUChars(dest, dest_ಸಾಮರ್ಥ್ಯ, 0, p_Error_ಸಂಕೇತ);
    }

    /* check that source and destination do not overlap */
    ಆದರೆ( dest!=ಶೂನ್ಯಸೂಚಕ &&
        ((ಮೂಲ<=dest && dest<ಮೂಲ+ಮೂಲ_ಉದ್ದಳತೆ) ||
         (dest<=ಮೂಲ && ಮೂಲ<dest+dest_ಸಾಮರ್ಥ್ಯ))) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    /* Does Options contain the new Seen Tail Unicode code point option */
    ಆದರೆ ( (options&U_SHAPE_ಬಾಲ_TYPE_ಮುಖವಾಡ) == U_SHAPE_ಬಾಲ_NEW_UNICODE){
        shapeVars.ಬಾಲ_Char = NEW_ಬಾಲ_CHAR;
    }ಇಲ್ಲದಿದ್ದರೆ {
        shapeVars.ಬಾಲ_Char = OLD_ಬಾಲ_CHAR;
    }

    ಆದರೆ((options&U_SHAPE_LETTERS_ಮುಖವಾಡ)!=U_SHAPE_LETTERS_NOOP) {
        char16_t ಬಫರ್[300];
        char16_t *tempbuffer, *tempsource = ಶೂನ್ಯಸೂಚಕ;
        int32_t output_ಗಾತ್ರ, spacesCountl=0, spacesCountr=0;

        ಆದರೆ((options&U_SHAPE_AGGREGATE_TASHKEEL_ಮುಖವಾಡ)>0) {
            int32_t logical_ಕ್ರಮ = (options&U_SHAPE_ಪಠ್ಯ_DIRECTION_ಮುಖವಾಡ) == U_SHAPE_ಪಠ್ಯ_DIRECTION_LOGICAL;
            int32_t aggregate_tashkeel =
                        (options&(U_SHAPE_AGGREGATE_TASHKEEL_ಮುಖವಾಡ+U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED)) ==
                        (U_SHAPE_AGGREGATE_TASHKEEL+U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED);
            ಪೂರ್ಣಾಂಕ ಹೆಜ್ಜೆ=logical_ಕ್ರಮ?1:-1;
            ಪೂರ್ಣಾಂಕ j=logical_ಕ್ರಮ?-1:2*ಮೂಲ_ಉದ್ದಳತೆ;
            ಪೂರ್ಣಾಂಕ i=logical_ಕ್ರಮ?-1:ಮೂಲ_ಉದ್ದಳತೆ;
            ಪೂರ್ಣಾಂಕ ಕೊನೆ=logical_ಕ್ರಮ?ಮೂಲ_ಉದ್ದಳತೆ:-1;
            ಪೂರ್ಣಾಂಕ aggregation_possible = 1;
            char16_t prev = 0;
            char16_t prevLink, currLink = 0;
            ಪೂರ್ಣಾಂಕ new_ಮೂಲ_ಉದ್ದಳತೆ = 0;
            tempsource = (char16_t *)uprv_malloc(2*ಮೂಲ_ಉದ್ದಳತೆ*U_SIZEOF_UCHAR);
            ಆದರೆ(tempsource == ಶೂನ್ಯಸೂಚಕ) {
                *p_Error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                ಹಿಂತಿರುಗಿ 0;
            }

            ಆಗ ((i+=ಹೆಜ್ಜೆ) != ಕೊನೆ) {
                prevLink = currLink;
                currLink = getLink(ಮೂಲ[i]);
                ಆದರೆ (aggregate_tashkeel && ((prevLink|currLink)&COMBINE) == COMBINE && aggregation_possible) {
                    ಆದರೆ (j<0 || j>=2*ಮೂಲ_ಉದ್ದಳತೆ) {
                        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
                        ಆದರೆ (tempsource != ಶೂನ್ಯಸೂಚಕ) uprv_free(tempsource);
                        ಹಿಂತಿರುಗಿ 0;
                    }
                    aggregation_possible = 0;
                    tempsource[j] = (prev<ಮೂಲ[i]?prev:ಮೂಲ[i])-0x064C+0xFC5E;
                    currLink = getLink(tempsource[j]);
                } ಇಲ್ಲದಿದ್ದರೆ {
                    aggregation_possible = 1;
                    tempsource[j+=ಹೆಜ್ಜೆ] = ಮೂಲ[i];
                    prev = ಮೂಲ[i];
                    new_ಮೂಲ_ಉದ್ದಳತೆ++;
                }
            }
            ಮೂಲ = tempsource+(logical_ಕ್ರಮ?0:j);
            ಮೂಲ_ಉದ್ದಳತೆ = new_ಮೂಲ_ಉದ್ದಳತೆ;
        }

        /* calculate destination size */
        /* TODO: do we ever need to do this pure preflighting? */
        ಆದರೆ(((options&U_SHAPE_LAMALEF_ಮುಖವಾಡ)==U_SHAPE_LAMALEF_RESIZE) ||
           ((options&U_SHAPE_TASHKEEL_ಮುಖವಾಡ)==U_SHAPE_TASHKEEL_RESIZE)) {
            output_ಗಾತ್ರ=calculate_ಗಾತ್ರ(ಮೂಲ,ಮೂಲ_ಉದ್ದಳತೆ,dest_ಸಾಮರ್ಥ್ಯ,options);
        } ಇಲ್ಲದಿದ್ದರೆ {
            output_ಗಾತ್ರ=ಮೂಲ_ಉದ್ದಳತೆ;
        }

        ಆದರೆ(output_ಗಾತ್ರ>dest_ಸಾಮರ್ಥ್ಯ) {
            *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                ಆದರೆ (tempsource != ಶೂನ್ಯಸೂಚಕ) uprv_free(tempsource);
            ಹಿಂತಿರುಗಿ output_ಗಾತ್ರ;
        }

        /*
         * need a temporary buffer of size max(outputSize, sourceLength)
         * because at first we copy source->temp
         */
        ಆದರೆ(ಮೂಲ_ಉದ್ದಳತೆ>output_ಗಾತ್ರ) {
            output_ಗಾತ್ರ=ಮೂಲ_ಉದ್ದಳತೆ;
        }

        /* Start of Arabic letter shaping part */
        ಆದರೆ(output_ಗಾತ್ರ<=UPRV_LENGTHOF(ಬಫರ್)) {
            output_ಗಾತ್ರ=UPRV_LENGTHOF(ಬಫರ್);
            tempbuffer=ಬಫರ್;
        } ಇಲ್ಲದಿದ್ದರೆ {
            tempbuffer = (char16_t *)uprv_malloc(output_ಗಾತ್ರ*U_SIZEOF_UCHAR);

            /*Test for nullptr*/
            ಆದರೆ(tempbuffer == ಶೂನ್ಯಸೂಚಕ) {
                *p_Error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                ಆದರೆ (tempsource != ಶೂನ್ಯಸೂಚಕ) uprv_free(tempsource);
                ಹಿಂತಿರುಗಿ 0;
            }
        }
        u_memcpy(tempbuffer, ಮೂಲ, ಮೂಲ_ಉದ್ದಳತೆ);
        ಆದರೆ (tempsource != ಶೂನ್ಯಸೂಚಕ){
            uprv_free(tempsource);
        }

        ಆದರೆ(ಮೂಲ_ಉದ್ದಳತೆ<output_ಗಾತ್ರ) {
            uprv_memset(tempbuffer+ಮೂಲ_ಉದ್ದಳತೆ, 0, (output_ಗಾತ್ರ-ಮೂಲ_ಉದ್ದಳತೆ)*U_SIZEOF_UCHAR);
        }

        ಆದರೆ((options&U_SHAPE_ಪಠ್ಯ_DIRECTION_ಮುಖವಾಡ) == U_SHAPE_ಪಠ್ಯ_DIRECTION_LOGICAL) {
            ಎಣಿಕೆ_Spaces(tempbuffer,ಮೂಲ_ಉದ್ದಳತೆ,options,&spacesCountl,&spacesCountr);
            invert_ಬಫರ್(tempbuffer,ಮೂಲ_ಉದ್ದಳತೆ,options,spacesCountl,spacesCountr);
        }

        ಆದರೆ((options&U_SHAPE_ಪಠ್ಯ_DIRECTION_ಮುಖವಾಡ) == U_SHAPE_ಪಠ್ಯ_DIRECTION_VISUAL_LTR) {
            ಆದರೆ((options&U_SHAPE_SPACES_RELATIVE_ಗಮ್ಯ_ಪಠ್ಯ_ಮುಖವಾಡ) == U_SHAPE_SPACES_RELATIVE_ಗಮ್ಯ_ಪಠ್ಯ_BEGIN_ಕೊನೆ) {
                shapeVars.spaces_Relative_ಗಮ್ಯ_ಪಠ್ಯ_Begin_ಕೊನೆ = 1;
                shapeVars.uShapeLamalefBegin = U_SHAPE_LAMALEF_ಕೊನೆ;
                shapeVars.u_Shape_Lamalef_ಕೊನೆ = U_SHAPE_LAMALEF_BEGIN;
                shapeVars.uShapeTashkeelBegin = U_SHAPE_TASHKEEL_ಕೊನೆ;
                shapeVars.u_Shape_Tashkeel_ಕೊನೆ = U_SHAPE_TASHKEEL_BEGIN;
            }
        }

        ಆಯ್ಕೆ(options&U_SHAPE_LETTERS_ಮುಖವಾಡ) {
        ಸ್ಥಿತಿ U_SHAPE_LETTERS_SHAPE :
             ಆದರೆ( (options&U_SHAPE_TASHKEEL_ಮುಖವಾಡ)> 0
                 && ((options&U_SHAPE_TASHKEEL_ಮುಖವಾಡ) !=U_SHAPE_TASHKEEL_REPLACE_BY_TATWEEL)) {
                /* Call the shaping function with tashkeel flag == 2 for removal of tashkeel */
                dest_ಉದ್ದಳತೆ = shapeUnicode(tempbuffer,ಮೂಲ_ಉದ್ದಳತೆ,dest_ಸಾಮರ್ಥ್ಯ,options,p_Error_ಸಂಕೇತ,2,shapeVars);
             }ಇಲ್ಲದಿದ್ದರೆ {
                /* default Call the shaping function with tashkeel flag == 1 */
                dest_ಉದ್ದಳತೆ = shapeUnicode(tempbuffer,ಮೂಲ_ಉದ್ದಳತೆ,dest_ಸಾಮರ್ಥ್ಯ,options,p_Error_ಸಂಕೇತ,1,shapeVars);

                /*After shaping text check if user wants to remove tashkeel and replace it with tatweel*/
                ಆದರೆ( (options&U_SHAPE_TASHKEEL_ಮುಖವಾಡ) == U_SHAPE_TASHKEEL_REPLACE_BY_TATWEEL){
                  dest_ಉದ್ದಳತೆ = handleTashkeelWithTatweel(tempbuffer,dest_ಉದ್ದಳತೆ,dest_ಸಾಮರ್ಥ್ಯ,options,p_Error_ಸಂಕೇತ);
                }
            }
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED :
            /* Call the shaping function with tashkeel flag == 0 */
            dest_ಉದ್ದಳತೆ = shapeUnicode(tempbuffer,ಮೂಲ_ಉದ್ದಳತೆ,dest_ಸಾಮರ್ಥ್ಯ,options,p_Error_ಸಂಕೇತ,0,shapeVars);
            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ U_SHAPE_LETTERS_UNSHAPE :
            /* Call the deshaping function */
            dest_ಉದ್ದಳತೆ = deShapeUnicode(tempbuffer,ಮೂಲ_ಉದ್ದಳತೆ,dest_ಸಾಮರ್ಥ್ಯ,options,p_Error_ಸಂಕೇತ,shapeVars);
            ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ :
            /* will never occur because of validity checks above */
            dest_ಉದ್ದಳತೆ = 0;
            ನಿಲ್ಲಿಸು;
        }

        /*
         * TODO: (markus 2002aug01)
         * For as long as we always preflight the outputSize above
         * we should U_ASSERT(outputSize==destLength)
         * except for the adjustment above before the tempbuffer allocation
         */

        ಆದರೆ((options&U_SHAPE_ಪಠ್ಯ_DIRECTION_ಮುಖವಾಡ) == U_SHAPE_ಪಠ್ಯ_DIRECTION_LOGICAL) {
            ಎಣಿಕೆ_Spaces(tempbuffer,dest_ಉದ್ದಳತೆ,options,&spacesCountl,&spacesCountr);
            invert_ಬಫರ್(tempbuffer,dest_ಉದ್ದಳತೆ,options,spacesCountl,spacesCountr);
        }
        u_memcpy(dest, tempbuffer, uprv_min(dest_ಉದ್ದಳತೆ, dest_ಸಾಮರ್ಥ್ಯ));

        ಆದರೆ(tempbuffer!=ಬಫರ್) {
            uprv_free(tempbuffer);
        }

        ಆದರೆ(dest_ಉದ್ದಳತೆ>dest_ಸಾಮರ್ಥ್ಯ) {
            *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            ಹಿಂತಿರುಗಿ dest_ಉದ್ದಳತೆ;
        }

        /* End of Arabic letter shaping part */
    } ಇಲ್ಲದಿದ್ದರೆ {
        /*
         * No letter shaping:
         * just make sure the destination is large enough and copy the string.
         */
        ಆದರೆ(dest_ಸಾಮರ್ಥ್ಯ<ಮೂಲ_ಉದ್ದಳತೆ) {
            /* this catches preflighting, too */
            *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            ಹಿಂತಿರುಗಿ ಮೂಲ_ಉದ್ದಳತೆ;
        }
        u_memcpy(dest, ಮೂಲ, ಮೂಲ_ಉದ್ದಳತೆ);
        dest_ಉದ್ದಳತೆ=ಮೂಲ_ಉದ್ದಳತೆ;
    }

    /*
     * Perform number shaping.
     * With UTF-16 or UTF-32, the length of the string is constant.
     * The easiest way to do this is to operate on the destination and
     * "shape" the digits in-place.
     */
    ಆದರೆ((options&U_SHAPE_DIGITS_ಮುಖವಾಡ)!=U_SHAPE_DIGITS_NOOP) {
        char16_t digit_ತಳ;
        int32_t i;

        /* select the requested digit group */
        ಆಯ್ಕೆ(options&U_SHAPE_DIGIT_TYPE_ಮುಖವಾಡ) {
        ಸ್ಥಿತಿ U_SHAPE_DIGIT_TYPE_AN:
            digit_ತಳ=0x660; /* Unicode: "Arabic-Indic digits" */
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ U_SHAPE_DIGIT_TYPE_AN_EXTENDED:
            digit_ತಳ=0x6f0; /* Unicode: "Eastern Arabic-Indic digits (Persian and Urdu)" */
            ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
            /* will never occur because of validity checks above */
            digit_ತಳ=0;
            ನಿಲ್ಲಿಸು;
        }

        /* perform the requested operation */
        ಆಯ್ಕೆ(options&U_SHAPE_DIGITS_ಮುಖವಾಡ) {
        ಸ್ಥಿತಿ U_SHAPE_DIGITS_EN2AN:
            /* add (digitBase-'0') to each European (ASCII) digit code point */
            digit_ತಳ-=0x30;
            ಗಾಗಿ(i=0; i<dest_ಉದ್ದಳತೆ; ++i) {
                ಆದರೆ(((uint32_t)dest[i]-0x30)<10) {
                    dest[i]+=digit_ತಳ;
                }
            }
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ U_SHAPE_DIGITS_AN2EN:
            /* subtract (digitBase-'0') from each Arabic digit code point */
            ಗಾಗಿ(i=0; i<dest_ಉದ್ದಳತೆ; ++i) {
                ಆದರೆ(((uint32_t)dest[i]-(uint32_t)digit_ತಳ)<10) {
                    dest[i]-=digit_ತಳ-0x30;
                }
            }
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ U_SHAPE_DIGITS_ALEN2AN_INIT_LR:
            shape_ಗಮ್ಯ_Arabic_Digits_With_Context(dest, dest_ಉದ್ದಳತೆ,
                                            digit_ತಳ,
                                            (options & U_SHAPE_ಪಠ್ಯ_DIRECTION_ಮುಖವಾಡ) == U_SHAPE_ಪಠ್ಯ_DIRECTION_LOGICAL,
                                            ಸುಳ್ಳು);
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ U_SHAPE_DIGITS_ALEN2AN_INIT_AL:
            shape_ಗಮ್ಯ_Arabic_Digits_With_Context(dest, dest_ಉದ್ದಳತೆ,
                                            digit_ತಳ,
                                            (options & U_SHAPE_ಪಠ್ಯ_DIRECTION_ಮುಖವಾಡ) == U_SHAPE_ಪಠ್ಯ_DIRECTION_LOGICAL,
                                            ನಿಜ);
            ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
            /* will never occur because of validity checks above */
            ನಿಲ್ಲಿಸು;
        }
    }

    ಹಿಂತಿರುಗಿ u_terminateUChars(dest, dest_ಸಾಮರ್ಥ್ಯ, dest_ಉದ್ದಳತೆ, p_Error_ಸಂಕೇತ);
}
