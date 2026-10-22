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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uchar.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ushape.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "putilimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustr_imp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ubidi_props.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <limits>
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

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IRRELEVANT 4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LAMTYPE    16
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ALEFTYPE   32
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LINKR      1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LINKL      2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APRESENT   8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHADDA     64
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSHADDA    128
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COMBINE    (SHADDA+CSHADDA)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAMZAFE_CHAR       0xfe80
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAMZA06_CHAR       0x0621
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ YEH_HAMZA_CHAR     0x0626
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ YEH_HAMZAFE_CHAR   0xFE89
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LAMALEF_SPACE_SUB  0xFFFF
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TASHKEEL_SPACE_SUB 0xFFFE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NEW_ਪੂਛ_CHAR      0xFE73
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OLD_ਪੂਛ_CHAR      0x200B
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LAM_CHAR           0x0644
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPACE_CHAR         0x0020
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHADDA_CHAR        0xFE7C
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TATWEEL_CHAR       0x0640
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHADDA_TATWEEL_CHAR  0xFE7D
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHADDA06_CHAR      0x0651

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHAPE_ਬਹੁਲਕ   0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DESHAPE_ਬਹੁਲਕ 1

ਬਣਤਰ uShapeVariables {
     char16_t ਪੂਛ_Char;
     uint32_t uShapeLamalefBegin;
     uint32_t u_Shape_Lamalef_ਸਿਰਾ;
     uint32_t uShapeTashkeelBegin;
     uint32_t u_Shape_Tashkeel_ਸਿਰਾ;
     ਪੂਰਨ spaces_Relative_ਮੰਜ਼ਿਲ_ਪਾਠ_Begin_ਸਿਰਾ;
};

ਸਥਿਰ ਅਚਲ uint8_t ਪੂਛ_Family_Isolated_Final[] = {
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

ਸਥਿਰ ਅਚਲ uint8_t tashkeelMedial[] = {
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

ਸਥਿਰ ਅਚਲ char16_t yeh_Hamza_ਮੰਜ਼ਿਲ_Yeh[] =
{
/* isolated*/ 0xFEEF,
/* final   */ 0xFEF0
};

ਸਥਿਰ ਅਚਲ uint8_t IrrelevantPos[] = {
    0x0, 0x2, 0x4, 0x6,
    0x8, 0xA, 0xC, 0xE
};


ਸਥਿਰ ਅਚਲ char16_t convertLamAlef[] =
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

ਸਥਿਰ ਅਚਲ char16_t araLink[178]=
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

ਸਥਿਰ ਅਚਲ uint8_t presALink[] = {
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

ਸਥਿਰ ਅਚਲ uint8_t presBLink[]=
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

ਸਥਿਰ ਅਚਲ char16_t convertFBto06[] =
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

ਸਥਿਰ ਅਚਲ char16_t convertFEto06[] =
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

ਸਥਿਰ ਅਚਲ uint8_t shape_ਸਾਰਣੀ[4][4][4]=
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
ਸਥਿਰ ਖਾਲੀ
shape_ਮੰਜ਼ਿਲ_Arabic_Digits_With_Context(char16_t *s, int32_t length,
                                char16_t digit_ਅਧਾਰ,
                                UBool isLogical, UBool ਆਖਰੀ_Strong_Was_AL) {
    int32_t i;
    char16_t c;

    digit_ਅਧਾਰ-=0x30;

    /* the iteration direction depends on the type of input */
    ਜੇ(isLogical) {
        ਲਈ(i=0; i<length; ++i) {
            c=s[i];
            ਚੋਣ(ubidi_getClass(c)) {
            ਹਾਲਤ U_ਖੱਬਾ_ਮੰਜ਼ਿਲ_ਸੱਜਾ: /* L */
            ਹਾਲਤ U_ਸੱਜਾ_ਮੰਜ਼ਿਲ_ਖੱਬਾ: /* R */
                ਆਖਰੀ_Strong_Was_AL=ਝੂਠ;
                ਰੋਕੋ;
            ਹਾਲਤ U_ਸੱਜਾ_ਮੰਜ਼ਿਲ_ਖੱਬਾ_ARABIC: /* AL */
                ਆਖਰੀ_Strong_Was_AL=ਸੱਚ;
                ਰੋਕੋ;
            ਹਾਲਤ U_EUROPEAN_ਸੰਖਿਆ: /* EN */
                ਜੇ (ਆਖਰੀ_Strong_Was_AL && static_cast<uint32_t>(c - 0x30) < 10) {
                    s[i] = static_cast<char16_t>(digit_ਅਧਾਰ + c); /* digitBase+(c-0x30) - digitBase was modified above */
                }
                ਰੋਕੋ;
            ਮੂਲ :
                ਰੋਕੋ;
            }
        }
    } ਨਹੀਂਤਾਂ {
        ਲਈ(i=length; i>0; /* pre-decrement in the body */) {
            c=s[--i];
            ਚੋਣ(ubidi_getClass(c)) {
            ਹਾਲਤ U_ਖੱਬਾ_ਮੰਜ਼ਿਲ_ਸੱਜਾ: /* L */
            ਹਾਲਤ U_ਸੱਜਾ_ਮੰਜ਼ਿਲ_ਖੱਬਾ: /* R */
                ਆਖਰੀ_Strong_Was_AL=ਝੂਠ;
                ਰੋਕੋ;
            ਹਾਲਤ U_ਸੱਜਾ_ਮੰਜ਼ਿਲ_ਖੱਬਾ_ARABIC: /* AL */
                ਆਖਰੀ_Strong_Was_AL=ਸੱਚ;
                ਰੋਕੋ;
            ਹਾਲਤ U_EUROPEAN_ਸੰਖਿਆ: /* EN */
                ਜੇ (ਆਖਰੀ_Strong_Was_AL && static_cast<uint32_t>(c - 0x30) < 10) {
                    s[i] = static_cast<char16_t>(digit_ਅਧਾਰ + c); /* digitBase+(c-0x30) - digitBase was modified above */
                }
                ਰੋਕੋ;
            ਮੂਲ :
                ਰੋਕੋ;
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
ਸਥਿਰ ਖਾਲੀ
invert_ਬਫਰ(char16_t *ਬਫਰ, int32_t size, uint32_t /*options*/, int32_t lowlimit, int32_t highlimit) {
    char16_t ਆਰਜ਼ੀ;
    int32_t i=0,j=0;
    ਲਈ(i=lowlimit,j=size-highlimit-1;i<j;i++,j--) {
        ਆਰਜ਼ੀ = ਬਫਰ[i];
        ਬਫਰ[i] = ਬਫਰ[j];
        ਬਫਰ[j] = ਆਰਜ਼ੀ;
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
ਸਥਿਰ ਸਤਰਅੰਦਰ char16_t
changeLamAlef(char16_t ch) {
    ਚੋਣ(ch) {
    ਹਾਲਤ 0x0622 :
        ਵਾਪਸ 0x065C;
    ਹਾਲਤ 0x0623 :
        ਵਾਪਸ 0x065D;
    ਹਾਲਤ 0x0625 :
        ਵਾਪਸ 0x065E;
    ਹਾਲਤ 0x0627 :
        ਵਾਪਸ 0x065F;
    }
    ਵਾਪਸ 0;
}

/*
 *Name     : getLink
 *Function : Resolves the link between the characters as
 *           Arabic characters have four forms :
 *           Isolated, Initial, Middle and Final Form
 */
ਸਥਿਰ char16_t
getLink(char16_t ch) {
    ਜੇ(ch >= 0x0622 && ch <= 0x06D3) {
        ਵਾਪਸ(araLink[ch-0x0622]);
    } ਨਹੀਂਤਾਂ ਜੇ(ch == 0x200D) {
        ਵਾਪਸ(3);
    } ਨਹੀਂਤਾਂ ਜੇ(ch >= 0x206D && ch <= 0x206F) {
        ਵਾਪਸ(4);
    }ਨਹੀਂਤਾਂ ਜੇ(ch >= 0xFB50 && ch <= 0xFC62) {
        ਵਾਪਸ(presALink[ch-0xFB50]);
    } ਨਹੀਂਤਾਂ ਜੇ(ch >= 0xFE70 && ch <= 0xFEFC) {
        ਵਾਪਸ(presBLink[ch-0xFE70]);
    }ਨਹੀਂਤਾਂ {
        ਵਾਪਸ(0);
    }
}

/*
 *Name     : countSpaces
 *Function : Counts the number of spaces
 *           at each end of the logical buffer
 */
ਸਥਿਰ ਖਾਲੀ
ਗਿਣਤੀ_Spaces(char16_t *dest, int32_t size, uint32_t /*options*/, int32_t *spacesCountl, int32_t *spacesCountr) {
    int32_t i = 0;
    int32_t countl = 0,countr = 0;
    ਜਦੋਂ((dest[i] == SPACE_CHAR) && (countl < size)) {
       countl++;
       i++;
    }
    ਜੇ (countl < size) {  /* the entire buffer is not all space */
        ਜਦੋਂ(dest[size-1] == SPACE_CHAR) {
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
ਸਥਿਰ ਸਤਰਅੰਦਰ int32_t
isTashkeelChar(char16_t ch) {
    ਵਾਪਸ static_cast<int32_t>(ch >= 0x064B && ch <= 0x0652);
}

/*
 *Name     : isTashkeelCharFE
 *Function : Returns 1 for Tashkeel characters in FE range else return 0
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ int32_t
isTashkeelCharFE(char16_t ch) {
    ਵਾਪਸ static_cast<int32_t>(ch >= 0xFE70 && ch <= 0xFE7F);
}

/*
 *Name     : isAlefChar
 *Function : Returns 1 for Alef characters else return 0
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ int32_t
isAlefChar(char16_t ch) {
    ਵਾਪਸ static_cast<int32_t>(ch == 0x0622 || ch == 0x0623 || ch == 0x0625 || ch == 0x0627);
}

/*
 *Name     : isLamAlefChar
 *Function : Returns 1 for LamAlef characters else return 0
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ int32_t
isLamAlefChar(char16_t ch) {
    ਵਾਪਸ static_cast<int32_t>(ch >= 0xFEF5 && ch <= 0xFEFC);
}

/*BIDI
 *Name     : isTailChar
 *Function : returns 1 if the character matches one of the tail characters (0xfe73 or 0x200b) otherwise returns 0
 */

ਸਥਿਰ ਸਤਰਅੰਦਰ int32_t
is_ਪੂਛ_Char(char16_t ch) {
    ਜੇ(ch == OLD_ਪੂਛ_CHAR || ch == NEW_ਪੂਛ_CHAR){
            ਵਾਪਸ 1;
    }ਨਹੀਂਤਾਂ{
            ਵਾਪਸ 0;
    }
}

/*BIDI
 *Name     : isSeenTailFamilyChar
 *Function : returns 1 if the character is a seen family isolated character
 *           in the FE range otherwise returns 0
 */

ਸਥਿਰ ਸਤਰਅੰਦਰ int32_t
is_ਵੇਖਿਆ_ਪੂਛ_Family_Char(char16_t ch) {
    ਜੇ(ch >= 0xfeb1 && ch < 0xfebf){
            ਵਾਪਸ ਪੂਛ_Family_Isolated_Final [ch - 0xFEB1];
    }ਨਹੀਂਤਾਂ{
            ਵਾਪਸ 0;
    }
}

 /* Name     : isSeenFamilyChar
  * Function : returns 1 if the character is a seen family character in the Unicode
  *            06 range otherwise returns 0
 */

ਸਥਿਰ ਸਤਰਅੰਦਰ int32_t
is_ਵੇਖਿਆ_Family_Char(char16_t  ch){
    ਜੇ(ch >= 0x633 && ch <= 0x636){
        ਵਾਪਸ 1;
    }ਨਹੀਂਤਾਂ {
        ਵਾਪਸ 0;
    }
}

/*Start of BIDI*/
/*
 *Name     : isAlefMaksouraChar
 *Function : returns 1 if the character is a Alef Maksoura Final or isolated
 *           otherwise returns 0
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ int32_t
isAlefMaksouraChar(char16_t ch) {
    ਵਾਪਸ static_cast<int32_t>(ch == 0xFEEF || ch == 0xFEF0 || ch == 0x0649);
}

/*
 * Name     : isYehHamzaChar
 * Function : returns 1 if the character is a yehHamza isolated or yehhamza
 *            final is found otherwise returns 0
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ int32_t
isYehHamzaChar(char16_t ch) {
    ਜੇ((ch==0xFE89)||(ch==0xFE8A)){
        ਵਾਪਸ 1;
    }ਨਹੀਂਤਾਂ{
        ਵਾਪਸ 0;
    }
}

 /*
 * Name: isTashkeelOnTatweelChar
 * Function: Checks if the Tashkeel Character is on Tatweel or not,if the
 *           Tashkeel on tatweel (FE range), it returns 1 else if the
 *           Tashkeel with shadda on tatweel (FC range)return 2 otherwise
 *           returns 0
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ int32_t
isTashkeelOnTatweelChar(char16_t ch){
    ਜੇ(ch >= 0xfe70 && ch <= 0xfe7f && ch != NEW_ਪੂਛ_CHAR && ch != 0xFE75 && ch != SHADDA_TATWEEL_CHAR)
    {
        ਵਾਪਸ tashkeelMedial [ch - 0xFE70];
    }ਨਹੀਂਤਾਂ ਜੇ( (ch >= 0xfcf2 && ch <= 0xfcf4) || (ch == SHADDA_TATWEEL_CHAR)) {
        ਵਾਪਸ 2;
    }ਨਹੀਂਤਾਂ{
        ਵਾਪਸ 0;
    }
}

/*
 * Name: isIsolatedTashkeelChar
 * Function: Checks if the Tashkeel Character is in the isolated form
 *           (i.e. Unicode FE range) returns 1 else if the Tashkeel
 *           with shadda is in the isolated form (i.e. Unicode FC range)
 *           returns 2 otherwise returns 0
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ int32_t
isIsolatedTashkeelChar(char16_t ch){
    ਜੇ(ch >= 0xfe70 && ch <= 0xfe7f && ch != NEW_ਪੂਛ_CHAR && ch != 0xFE75){
        ਵਾਪਸ (1 - tashkeelMedial [ch - 0xFE70]);
    }ਨਹੀਂਤਾਂ ਜੇ(ch >= 0xfc5e && ch <= 0xfc63){
        ਵਾਪਸ 1;
    }ਨਹੀਂਤਾਂ{
        ਵਾਪਸ 0;
    }
}




/*
 *Name     : calculateSize
 *Function : This function calculates the destSize to be used in preflighting
 *           when the destSize is equal to 0
 *           It is used also to calculate the new destsize in case the
 *           destination buffer will be resized.
 */

ਸਥਿਰ int32_t
calculate_ਮਾਪ(ਅਚਲ char16_t *ਸਰੋਤ, int32_t ਸਰੋਤ_ਲੰਮਾਈ,
int32_t dest_ਮਾਪ,uint32_t options) {
    int32_t i = 0;

    ਪੂਰਨ lamAlefOption = 0;
    ਪੂਰਨ tashkeelOption = 0;

    dest_ਮਾਪ = ਸਰੋਤ_ਲੰਮਾਈ;

    ਜੇ (((options&U_SHAPE_LETTERS_ਨਕਾਬ) == U_SHAPE_LETTERS_SHAPE ||
        ((options&U_SHAPE_LETTERS_ਨਕਾਬ) == U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED )) &&
        ((options&U_SHAPE_LAMALEF_ਨਕਾਬ) == U_SHAPE_LAMALEF_RESIZE )){
            lamAlefOption = 1;
    }
    ਜੇ((options&U_SHAPE_LETTERS_ਨਕਾਬ) == U_SHAPE_LETTERS_SHAPE &&
       ((options&U_SHAPE_TASHKEEL_ਨਕਾਬ) == U_SHAPE_TASHKEEL_RESIZE ) ){
            tashkeelOption = 1;
        }

    ਜੇ(lamAlefOption || tashkeelOption){
        ਜੇ((options&U_SHAPE_ਪਾਠ_DIRECTION_ਨਕਾਬ)==U_SHAPE_ਪਾਠ_DIRECTION_VISUAL_LTR) {
            ਲਈ(i=0;i<ਸਰੋਤ_ਲੰਮਾਈ;i++) {
                ਜੇ( ((isAlefChar(ਸਰੋਤ[i]))&& (i<(ਸਰੋਤ_ਲੰਮਾਈ-1)) &&(ਸਰੋਤ[i+1] == LAM_CHAR)) || (isTashkeelCharFE(ਸਰੋਤ[i])) ) {
                        dest_ਮਾਪ--;
                    }
                }
            }ਨਹੀਂਤਾਂ ਜੇ((options&U_SHAPE_ਪਾਠ_DIRECTION_ਨਕਾਬ)==U_SHAPE_ਪਾਠ_DIRECTION_LOGICAL) {
                ਲਈ(i=0;i<ਸਰੋਤ_ਲੰਮਾਈ;i++) {
                    ਜੇ( ( (ਸਰੋਤ[i] == LAM_CHAR) && (i<(ਸਰੋਤ_ਲੰਮਾਈ-1)) && (isAlefChar(ਸਰੋਤ[i+1]))) || (isTashkeelCharFE(ਸਰੋਤ[i])) ) {
                        dest_ਮਾਪ--;
                    }
                }
            }
        }

    ਜੇ ((options&U_SHAPE_LETTERS_ਨਕਾਬ) == U_SHAPE_LETTERS_UNSHAPE){
        ਜੇ ( (options&U_SHAPE_LAMALEF_ਨਕਾਬ) == U_SHAPE_LAMALEF_RESIZE){
            ਲਈ(i=0;i<ਸਰੋਤ_ਲੰਮਾਈ;i++) {
                ਜੇ(isLamAlefChar(ਸਰੋਤ[i]))
                dest_ਮਾਪ++;
            }
        }
    }

    ਵਾਪਸ dest_ਮਾਪ;
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
ਸਥਿਰ int32_t
handleTashkeelWithTatweel(char16_t *dest, int32_t ਸਰੋਤ_ਲੰਮਾਈ,
             int32_t /*destSize*/, uint32_t /*options*/,
             UError_ਕੋਡ * /*pErrorCode*/) {
                 ਪੂਰਨ i;
                 ਲਈ(i = 0; i < ਸਰੋਤ_ਲੰਮਾਈ; i++){
                     ਜੇ((isTashkeelOnTatweelChar(dest[i]) == 1)){
                         dest[i] = TATWEEL_CHAR;
                    }ਨਹੀਂਤਾਂ ਜੇ((isTashkeelOnTatweelChar(dest[i]) == 2)){
                         dest[i] = SHADDA_TATWEEL_CHAR;
                    }ਨਹੀਂਤਾਂ ਜੇ(isIsolatedTashkeelChar(dest[i]) && dest[i] != SHADDA_CHAR){
                         dest[i] = SPACE_CHAR;
                    }
                 }
                 ਵਾਪਸ ਸਰੋਤ_ਲੰਮਾਈ;
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

ਸਥਿਰ int32_t
handleGeneratedSpaces(char16_t *dest, int32_t ਸਰੋਤ_ਲੰਮਾਈ,
                    int32_t dest_ਮਾਪ,
                    uint32_t options,
                    UError_ਕੋਡ *p_Error_ਕੋਡ,ਬਣਤਰ uShapeVariables shapeVars ) {

    int32_t i = 0, j = 0;
    int32_t count = 0;
    char16_t *tempbuffer=ਖਾਲੀ_ਸੂਚਕ;

    ਪੂਰਨ lamAlefOption = 0;
    ਪੂਰਨ tashkeelOption = 0;
    ਪੂਰਨ shaping_ਬਹੁਲਕ = SHAPE_ਬਹੁਲਕ;

    ਜੇ (shaping_ਬਹੁਲਕ == 0){
        ਜੇ ( (options&U_SHAPE_LAMALEF_ਨਕਾਬ) == U_SHAPE_LAMALEF_RESIZE ){
            lamAlefOption = 1;
        }
        ਜੇ ( (options&U_SHAPE_TASHKEEL_ਨਕਾਬ) == U_SHAPE_TASHKEEL_RESIZE ){
            tashkeelOption = 1;
        }
    }

    ਜੇ (static_cast<size_t>(ਸਰੋਤ_ਲੰਮਾਈ) + 1 > std::numeric_limits<size_t>::max() / U_SIZEOF_UCHAR) {
        *p_Error_ਕੋਡ = U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        ਵਾਪਸ 0;
    }
    tempbuffer = static_cast<char16_t*>(uprv_malloc((ਸਰੋਤ_ਲੰਮਾਈ + 1) * U_SIZEOF_UCHAR));
    /* Test for nullptr */
    ਜੇ(tempbuffer == ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ 0;
    }


    ਜੇ (lamAlefOption || tashkeelOption){
        uprv_memset(tempbuffer, 0, (ਸਰੋਤ_ਲੰਮਾਈ+1)*U_SIZEOF_UCHAR);

        i = j = 0; count = 0;
        ਜਦੋਂ(i < ਸਰੋਤ_ਲੰਮਾਈ) {
            ਜੇ ( (lamAlefOption && dest[i] == LAMALEF_SPACE_SUB) ||
               (tashkeelOption && dest[i] == TASHKEEL_SPACE_SUB) ){
                j--;
                count++;
            } ਨਹੀਂਤਾਂ {
                tempbuffer[j] = dest[i];
            }
            i++;
            j++;
        }

        ਜਦੋਂ(count >= 0) {
            tempbuffer[i] = 0x0000;
            i--;
            count--;
        }

        u_memcpy(dest, tempbuffer, ਸਰੋਤ_ਲੰਮਾਈ);
        dest_ਮਾਪ = u_strlen(dest);
    }

      lamAlefOption = 0;

    ਜੇ (shaping_ਬਹੁਲਕ == 0){
        ਜੇ ( (options&U_SHAPE_LAMALEF_ਨਕਾਬ) == U_SHAPE_LAMALEF_NEAR ){
            lamAlefOption = 1;
        }
    }

    ਜੇ (lamAlefOption){
        /* Lam+Alef is already shaped into LamAlef + FFFF */
        i = 0;
        ਜਦੋਂ(i < ਸਰੋਤ_ਲੰਮਾਈ) {
            ਜੇ(lamAlefOption&&dest[i] == LAMALEF_SPACE_SUB){
                dest[i] = SPACE_CHAR;
            }
            i++;
        }
        dest_ਮਾਪ = ਸਰੋਤ_ਲੰਮਾਈ;
    }
    lamAlefOption = 0;
    tashkeelOption = 0;

    ਜੇ (shaping_ਬਹੁਲਕ == 0) {
        ਜੇ ( ((options&U_SHAPE_LAMALEF_ਨਕਾਬ) == shapeVars.uShapeLamalefBegin) ||
              (((options&U_SHAPE_LAMALEF_ਨਕਾਬ) == U_SHAPE_LAMALEF_AUTO )
              && (shapeVars.spaces_Relative_ਮੰਜ਼ਿਲ_ਪਾਠ_Begin_ਸਿਰਾ==1)) ) {
            lamAlefOption = 1;
        }
        ਜੇ ( (options&U_SHAPE_TASHKEEL_ਨਕਾਬ) == shapeVars.uShapeTashkeelBegin ) {
            tashkeelOption = 1;
        }
    }

    ਜੇ(lamAlefOption || tashkeelOption){
        uprv_memset(tempbuffer, 0, (ਸਰੋਤ_ਲੰਮਾਈ+1)*U_SIZEOF_UCHAR);
        
        i = j = ਸਰੋਤ_ਲੰਮਾਈ; count = 0;
        
        ਜਦੋਂ(i >= 0) {
            ਜੇ ( (lamAlefOption && dest[i] == LAMALEF_SPACE_SUB) ||
                 (tashkeelOption && dest[i] == TASHKEEL_SPACE_SUB) ){
                j++;
                count++;
            }ਨਹੀਂਤਾਂ {
                tempbuffer[j] = dest[i];
            }
            i--;
            j--;
        }

        ਲਈ(i=0 ;i < count; i++){
                tempbuffer[i] = SPACE_CHAR;
        }

        u_memcpy(dest, tempbuffer, ਸਰੋਤ_ਲੰਮਾਈ);
        dest_ਮਾਪ = ਸਰੋਤ_ਲੰਮਾਈ;
    }



    lamAlefOption = 0;
    tashkeelOption = 0;

    ਜੇ (shaping_ਬਹੁਲਕ == 0) {
        ਜੇ ( ((options&U_SHAPE_LAMALEF_ਨਕਾਬ) == shapeVars.u_Shape_Lamalef_ਸਿਰਾ) ||
              (((options&U_SHAPE_LAMALEF_ਨਕਾਬ) == U_SHAPE_LAMALEF_AUTO )
              && (shapeVars.spaces_Relative_ਮੰਜ਼ਿਲ_ਪਾਠ_Begin_ਸਿਰਾ==0)) ) {
            lamAlefOption = 1;
        }
        ਜੇ ( (options&U_SHAPE_TASHKEEL_ਨਕਾਬ) == shapeVars.u_Shape_Tashkeel_ਸਿਰਾ ){
            tashkeelOption = 1;
        }
    }

    ਜੇ(lamAlefOption || tashkeelOption){
        uprv_memset(tempbuffer, 0, (ਸਰੋਤ_ਲੰਮਾਈ+1)*U_SIZEOF_UCHAR);

        i = j = 0; count = 0;
        ਜਦੋਂ(i < ਸਰੋਤ_ਲੰਮਾਈ) {
            ਜੇ ( (lamAlefOption && dest[i] == LAMALEF_SPACE_SUB) ||
                 (tashkeelOption && dest[i] == TASHKEEL_SPACE_SUB) ){
                j--;
                count++;
            }ਨਹੀਂਤਾਂ {
                tempbuffer[j] = dest[i];
            }
            i++;
            j++;
        }

        ਜਦੋਂ(count >= 0) {
            tempbuffer[i] = SPACE_CHAR;
            i--;
            count--;
        }

        u_memcpy(dest, tempbuffer, ਸਰੋਤ_ਲੰਮਾਈ);
        dest_ਮਾਪ = ਸਰੋਤ_ਲੰਮਾਈ;
    }


    ਜੇ(tempbuffer){
        uprv_free(tempbuffer);
    }

    ਵਾਪਸ dest_ਮਾਪ;
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

ਸਥਿਰ int32_t
expandCompositCharAtBegin(char16_t *dest, int32_t ਸਰੋਤ_ਲੰਮਾਈ, int32_t dest_ਮਾਪ,UError_ਕੋਡ *p_Error_ਕੋਡ) {
    int32_t      i = 0,j = 0;
    int32_t      countl = 0;
    char16_t *tempbuffer=ਖਾਲੀ_ਸੂਚਕ;

    tempbuffer = static_cast<char16_t*>(uprv_malloc((ਸਰੋਤ_ਲੰਮਾਈ + 1) * U_SIZEOF_UCHAR));

    /* Test for nullptr */
    ਜੇ(tempbuffer == ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ 0;
    }

        uprv_memset(tempbuffer, 0, (ਸਰੋਤ_ਲੰਮਾਈ+1)*U_SIZEOF_UCHAR);

        i = 0;
        ਜਦੋਂ(dest[i] == SPACE_CHAR) {
            countl++;
            i++;
        }

        i = j = ਸਰੋਤ_ਲੰਮਾਈ-1;

        ਜਦੋਂ(i >= 0 && j >= 0) {
            ਜੇ( countl>0 && isLamAlefChar(dest[i])) {
                tempbuffer[j] = LAM_CHAR;
                /* to ensure the array index is within the range */
                U_ASSERT(dest[i] >= 0xFEF5u
                    && dest[i]-0xFEF5u < UPRV_LENGTHOF(convertLamAlef));
                tempbuffer[j-1] = convertLamAlef[ dest[i] - 0xFEF5 ];
                j--;
                countl--;
            }ਨਹੀਂਤਾਂ {
                 ਜੇ( countl == 0 && isLamAlefChar(dest[i]) ) {
                     *p_Error_ਕੋਡ=U_NO_SPACE_ਉਪਲਬਧ;
                     }
                 tempbuffer[j] = dest[i];
            }
            i--;
            j--;
        }
        u_memcpy(dest, tempbuffer, ਸਰੋਤ_ਲੰਮਾਈ);

        uprv_free(tempbuffer);

        dest_ਮਾਪ = ਸਰੋਤ_ਲੰਮਾਈ;
        ਵਾਪਸ dest_ਮਾਪ;
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

ਸਥਿਰ int32_t
expand_Composit_Char_At_ਸਿਰਾ(char16_t *dest, int32_t ਸਰੋਤ_ਲੰਮਾਈ, int32_t dest_ਮਾਪ,UError_ਕੋਡ *p_Error_ਕੋਡ) {
    int32_t      i = 0,j = 0;

    int32_t      countr = 0;
    int32_t  inpsize = ਸਰੋਤ_ਲੰਮਾਈ;

    char16_t *tempbuffer=ਖਾਲੀ_ਸੂਚਕ;
    tempbuffer = static_cast<char16_t*>(uprv_malloc((ਸਰੋਤ_ਲੰਮਾਈ + 1) * U_SIZEOF_UCHAR));

    /* Test for nullptr */
    ਜੇ(tempbuffer == ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
         ਵਾਪਸ 0;
    }

    uprv_memset(tempbuffer, 0, (ਸਰੋਤ_ਲੰਮਾਈ+1)*U_SIZEOF_UCHAR);

    ਜਦੋਂ(inpsize>0 && dest[inpsize-1] == SPACE_CHAR) {
        countr++;
        inpsize--;
    }

    i = ਸਰੋਤ_ਲੰਮਾਈ - countr - 1;
    j = ਸਰੋਤ_ਲੰਮਾਈ - 1;

    ਜਦੋਂ(i >= 0 && j >= 0) {
        ਜੇ( countr>0 && isLamAlefChar(dest[i]) ) {
            tempbuffer[j] = LAM_CHAR;
            tempbuffer[j-1] = convertLamAlef[ dest[i] - 0xFEF5 ];
            j--;
            countr--;
        }ਨਹੀਂਤਾਂ {
            ਜੇ ((countr == 0) && isLamAlefChar(dest[i]) ) {
                *p_Error_ਕੋਡ=U_NO_SPACE_ਉਪਲਬਧ;
            }
            tempbuffer[j] = dest[i];
        }
        i--;
        j--;
    }

    ਜੇ(countr > 0) {
        u_memmove(tempbuffer, tempbuffer+countr, ਸਰੋਤ_ਲੰਮਾਈ);
        ਜੇ(u_strlen(tempbuffer) < ਸਰੋਤ_ਲੰਮਾਈ) {
            ਲਈ(i=ਸਰੋਤ_ਲੰਮਾਈ-1;i>=ਸਰੋਤ_ਲੰਮਾਈ-countr;i--) {
                tempbuffer[i] = SPACE_CHAR;
            }
        }
    }
    u_memcpy(dest, tempbuffer, ਸਰੋਤ_ਲੰਮਾਈ);

    uprv_free(tempbuffer);

    dest_ਮਾਪ = ਸਰੋਤ_ਲੰਮਾਈ;
    ਵਾਪਸ dest_ਮਾਪ;
}

/*
 *Name     : expandCompositCharAtNear
 *Function : Expands the LamAlef character into Lam + Alef, YehHamza character
 *           into Yeh + Hamza, SeenFamily character into SeenFamily character
 *           + Tail, while consuming the space next to the character.
 *           If there are no spaces next to the character, an error
 *           will be set to U_NO_SPACE_AVAILABLE as defined in utypes.h
 */

ਸਥਿਰ int32_t
expandCompositCharAtNear(char16_t *dest, int32_t ਸਰੋਤ_ਲੰਮਾਈ, int32_t dest_ਮਾਪ,UError_ਕੋਡ *p_Error_ਕੋਡ,
                         ਪੂਰਨ yehHamzaOption, ਪੂਰਨ ਵੇਖਿਆ_ਪੂਛ_Option, ਪੂਰਨ lamAlefOption, ਬਣਤਰ uShapeVariables shapeVars) {
    int32_t      i = 0;


    char16_t lamalefChar, yehhamzaChar;

    ਲਈ(i = 0 ;i<=ਸਰੋਤ_ਲੰਮਾਈ-1;i++) {
            ਜੇ (ਵੇਖਿਆ_ਪੂਛ_Option && is_ਵੇਖਿਆ_ਪੂਛ_Family_Char(dest[i])) {
                ਜੇ ((i>0) && (dest[i-1] == SPACE_CHAR) ) {
                    dest[i-1] = shapeVars.ਪੂਛ_Char;
                }ਨਹੀਂਤਾਂ {
                    *p_Error_ਕੋਡ=U_NO_SPACE_ਉਪਲਬਧ;
                }
            }ਨਹੀਂਤਾਂ ਜੇ(yehHamzaOption && (isYehHamzaChar(dest[i])) ) {
                ਜੇ ((i>0) && (dest[i-1] == SPACE_CHAR) ) {
                    yehhamzaChar = dest[i];
                    dest[i] = yeh_Hamza_ਮੰਜ਼ਿਲ_Yeh[yehhamzaChar - YEH_HAMZAFE_CHAR];
                    dest[i-1] = HAMZAFE_CHAR;
                }ਨਹੀਂਤਾਂ {

                    *p_Error_ਕੋਡ=U_NO_SPACE_ਉਪਲਬਧ;
                }
            }ਨਹੀਂਤਾਂ ਜੇ(lamAlefOption && isLamAlefChar(dest[i+1])) {
                ਜੇ(dest[i] == SPACE_CHAR){
                    lamalefChar = dest[i+1];
                    dest[i+1] = LAM_CHAR;
                    dest[i] = convertLamAlef[ lamalefChar - 0xFEF5 ];
                }ਨਹੀਂਤਾਂ {
                    *p_Error_ਕੋਡ=U_NO_SPACE_ਉਪਲਬਧ;
                }
            }
       }
       dest_ਮਾਪ = ਸਰੋਤ_ਲੰਮਾਈ;
       ਵਾਪਸ dest_ਮਾਪ;
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

ਸਥਿਰ int32_t
expandCompositChar(char16_t *dest, int32_t ਸਰੋਤ_ਲੰਮਾਈ,
              int32_t dest_ਮਾਪ,uint32_t options,
              UError_ਕੋਡ *p_Error_ਕੋਡ, ਪੂਰਨ shaping_ਬਹੁਲਕ,ਬਣਤਰ uShapeVariables shapeVars) {

    int32_t      i = 0,j = 0;

    char16_t *tempbuffer=ਖਾਲੀ_ਸੂਚਕ;
    ਪੂਰਨ yehHamzaOption = 0;
    ਪੂਰਨ ਵੇਖਿਆ_ਪੂਛ_Option = 0;
    ਪੂਰਨ lamAlefOption = 0;

    ਜੇ (shaping_ਬਹੁਲਕ == 1){
        ਜੇ ( (options&U_SHAPE_LAMALEF_ਨਕਾਬ) == U_SHAPE_LAMALEF_AUTO){

            ਜੇ(shapeVars.spaces_Relative_ਮੰਜ਼ਿਲ_ਪਾਠ_Begin_ਸਿਰਾ == 0) {
                dest_ਮਾਪ = expand_Composit_Char_At_ਸਿਰਾ(dest, ਸਰੋਤ_ਲੰਮਾਈ, dest_ਮਾਪ, p_Error_ਕੋਡ);

                ਜੇ(*p_Error_ਕੋਡ == U_NO_SPACE_ਉਪਲਬਧ) {
                    *p_Error_ਕੋਡ = U_ZERO_ERROR;
                    dest_ਮਾਪ = expandCompositCharAtBegin(dest, ਸਰੋਤ_ਲੰਮਾਈ, dest_ਮਾਪ, p_Error_ਕੋਡ);
                }
            }ਨਹੀਂਤਾਂ {
                dest_ਮਾਪ = expandCompositCharAtBegin(dest, ਸਰੋਤ_ਲੰਮਾਈ, dest_ਮਾਪ, p_Error_ਕੋਡ);

                ਜੇ(*p_Error_ਕੋਡ == U_NO_SPACE_ਉਪਲਬਧ) {
                    *p_Error_ਕੋਡ = U_ZERO_ERROR;
                    dest_ਮਾਪ = expand_Composit_Char_At_ਸਿਰਾ(dest, ਸਰੋਤ_ਲੰਮਾਈ, dest_ਮਾਪ, p_Error_ਕੋਡ);
                }
            }

            ਜੇ(*p_Error_ਕੋਡ == U_NO_SPACE_ਉਪਲਬਧ) {
                *p_Error_ਕੋਡ = U_ZERO_ERROR;
                dest_ਮਾਪ = expandCompositCharAtNear(dest, ਸਰੋਤ_ਲੰਮਾਈ, dest_ਮਾਪ, p_Error_ਕੋਡ, yehHamzaOption,
                                                ਵੇਖਿਆ_ਪੂਛ_Option, 1,shapeVars);
            }
        }
    }

    ਜੇ (shaping_ਬਹੁਲਕ == 1){
        ਜੇ ( (options&U_SHAPE_LAMALEF_ਨਕਾਬ) == shapeVars.u_Shape_Lamalef_ਸਿਰਾ){
            dest_ਮਾਪ = expand_Composit_Char_At_ਸਿਰਾ(dest, ਸਰੋਤ_ਲੰਮਾਈ, dest_ਮਾਪ, p_Error_ਕੋਡ);
        }
    }

    ਜੇ (shaping_ਬਹੁਲਕ == 1){
        ਜੇ ( (options&U_SHAPE_LAMALEF_ਨਕਾਬ) == shapeVars.uShapeLamalefBegin){
            dest_ਮਾਪ = expandCompositCharAtBegin(dest, ਸਰੋਤ_ਲੰਮਾਈ, dest_ਮਾਪ, p_Error_ਕੋਡ);
        }
    }

    ਜੇ (shaping_ਬਹੁਲਕ == 0){
         ਜੇ ((options&U_SHAPE_YEHHAMZA_ਨਕਾਬ) == U_SHAPE_YEHHAMZA_TWOCELL_NEAR){
             yehHamzaOption = 1;
         }
         ਜੇ ((options&U_SHAPE_ਵੇਖਿਆ_ਨਕਾਬ) == U_SHAPE_ਵੇਖਿਆ_TWOCELL_NEAR){
            ਵੇਖਿਆ_ਪੂਛ_Option = 1;
         }
    }
    ਜੇ (shaping_ਬਹੁਲਕ == 1) {
        ਜੇ ( (options&U_SHAPE_LAMALEF_ਨਕਾਬ) == U_SHAPE_LAMALEF_NEAR) {
            lamAlefOption = 1;
        }
    }


    ਜੇ (yehHamzaOption || ਵੇਖਿਆ_ਪੂਛ_Option || lamAlefOption){
        dest_ਮਾਪ = expandCompositCharAtNear(dest, ਸਰੋਤ_ਲੰਮਾਈ, dest_ਮਾਪ, p_Error_ਕੋਡ, yehHamzaOption,
                                            ਵੇਖਿਆ_ਪੂਛ_Option,lamAlefOption,shapeVars);
    }


    ਜੇ (shaping_ਬਹੁਲਕ == 1){
        ਜੇ ( (options&U_SHAPE_LAMALEF_ਨਕਾਬ) == U_SHAPE_LAMALEF_RESIZE){
            dest_ਮਾਪ = calculate_ਮਾਪ(dest,ਸਰੋਤ_ਲੰਮਾਈ,dest_ਮਾਪ,options);
            tempbuffer = static_cast<char16_t*>(uprv_malloc((dest_ਮਾਪ + 1) * U_SIZEOF_UCHAR));

            /* Test for nullptr */
            ਜੇ(tempbuffer == ਖਾਲੀ_ਸੂਚਕ) {
                *p_Error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                ਵਾਪਸ 0;
            }

            uprv_memset(tempbuffer, 0, (dest_ਮਾਪ+1)*U_SIZEOF_UCHAR);

            i = j = 0;
            ਜਦੋਂ(i < dest_ਮਾਪ && j < dest_ਮਾਪ) {
                ਜੇ(isLamAlefChar(dest[i]) ) {
                    tempbuffer[j] = convertLamAlef[ dest[i] - 0xFEF5 ];
                    tempbuffer[j+1] = LAM_CHAR;
                    j++;
                }ਨਹੀਂਤਾਂ {
                    tempbuffer[j] = dest[i];
                }
                i++;
                j++;
            }

            u_memcpy(dest, tempbuffer, dest_ਮਾਪ);
        }
    }

    ਜੇ(tempbuffer) {
        uprv_free(tempbuffer);
    }
    ਵਾਪਸ dest_ਮਾਪ;
}

/*
 *Name     : shapeUnicode
 *Function : Converts an Arabic Unicode buffer in 06xx Range into a shaped
 *           arabic Unicode buffer in FExx Range
 */
ਸਥਿਰ int32_t
shapeUnicode(char16_t *dest, int32_t ਸਰੋਤ_ਲੰਮਾਈ,
             int32_t dest_ਮਾਪ,uint32_t options,
             UError_ਕੋਡ *p_Error_ਕੋਡ,
             ਪੂਰਨ tashkeelFlag, ਬਣਤਰ uShapeVariables shapeVars) {

    int32_t          i, iend;
    int32_t          ਕਦਮ;
    int32_t          ਆਖਰੀ_Pos,Nx, Nw;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ     Shape;
    int32_t          lamalef_ਮਿਲਿਆ = 0;
    int32_t seenfam_ਮਿਲਿਆ = 0, yehhamza_ਮਿਲਿਆ =0, tashkeel_ਮਿਲਿਆ  = 0;
    char16_t         prevLink = 0, ਆਖਰੀ_Link = 0, currLink, ਅਗਲਾ_Link = 0;
    char16_t         wLamalef;

    /*
     * Converts the input buffer from FExx Range into 06xx Range
     * to make sure that all characters are in the 06xx range
     * even the lamalef is converted to the special region in
     * the 06xx range
     */
    ਜੇ ((options & U_SHAPE_PRESERVE_PRESENTATION_ਨਕਾਬ)  == U_SHAPE_PRESERVE_PRESENTATION_NOOP) {
        ਲਈ (i = 0; i < ਸਰੋਤ_ਲੰਮਾਈ; i++) {
            char16_t ਨਿਵੇਸ਼_Char  = dest[i];
            ਜੇ ( (ਨਿਵੇਸ਼_Char >= 0xFB50) && (ਨਿਵੇਸ਼_Char <= 0xFBFF)) {
                char16_t c = convertFBto06 [ (ਨਿਵੇਸ਼_Char - 0xFB50) ];
                ਜੇ (c != 0)
                    dest[i] = c;
            } ਨਹੀਂਤਾਂ ਜੇ ( (ਨਿਵੇਸ਼_Char >= 0xFE70) && (ਨਿਵੇਸ਼_Char <= 0xFEFC)) {
                dest[i] = convertFEto06 [ (ਨਿਵੇਸ਼_Char - 0xFE70) ] ;
            } ਨਹੀਂਤਾਂ {
                dest[i] = ਨਿਵੇਸ਼_Char ;
            }
        }
    }


    /* sets the index to the end of the buffer, together with the step point to -1 */
    i = ਸਰੋਤ_ਲੰਮਾਈ - 1;
    iend = -1;
    ਕਦਮ = -1;

    /*
     * This function resolves the link between the characters .
     * Arabic characters have four forms :
     * Isolated Form, Initial Form, Middle Form and Final Form
     */
    currLink = getLink(dest[i]);

    ਆਖਰੀ_Pos = i;
    Nx = -2, Nw = 0;

    ਜਦੋਂ (i != iend) {
        /* If high byte of currLink > 0 then more than one shape */
        ਜੇ ((currLink & 0xFF00) > 0 || (getLink(dest[i]) & IRRELEVANT) != 0) {
            Nw = i + ਕਦਮ;
            ਜਦੋਂ (Nx < 0) {         /* we need to know about next char */
                ਜੇ(Nw == iend) {
                    ਅਗਲਾ_Link = 0;
                    Nx = 3000;
                } ਨਹੀਂਤਾਂ {
                    ਅਗਲਾ_Link = getLink(dest[Nw]);
                    ਜੇ((ਅਗਲਾ_Link & IRRELEVANT) == 0) {
                        Nx = Nw;
                    } ਨਹੀਂਤਾਂ {
                        Nw = Nw + ਕਦਮ;
                    }
                }
            }

            ਜੇ ( ((currLink & ALEFTYPE) > 0)  &&  ((ਆਖਰੀ_Link & LAMTYPE) > 0) ) {
                lamalef_ਮਿਲਿਆ = 1;
                wLamalef = changeLamAlef(dest[i]); /*get from 0x065C-0x065f */
                ਜੇ ( wLamalef != 0) {
                    dest[i] = LAMALEF_SPACE_SUB;            /* The default case is to drop the Alef and replace */
                    dest[ਆਖਰੀ_Pos] =wLamalef;     /* it by LAMALEF_SPACE_SUB which is the last character in the  */
                    i=ਆਖਰੀ_Pos;                   /* unicode private use area, this is done to make   */
                }                                /* sure that removeLamAlefSpaces() handles only the */
                ਆਖਰੀ_Link = prevLink;             /* spaces generated during lamalef generation.      */
                currLink = getLink(wLamalef);    /* LAMALEF_SPACE_SUB is added here and is replaced by spaces   */
            }                                    /* in removeLamAlefSpaces()                         */

            ਜੇ ((i > 0) && (dest[i-1] == SPACE_CHAR)){
                ਜੇ ( is_ਵੇਖਿਆ_Family_Char(dest[i])) {
                    seenfam_ਮਿਲਿਆ = 1;
                } ਨਹੀਂਤਾਂ ਜੇ (dest[i] == YEH_HAMZA_CHAR) {
                    yehhamza_ਮਿਲਿਆ = 1;
                }
            }
            ਨਹੀਂਤਾਂ ਜੇ(i==0){
                ਜੇ ( is_ਵੇਖਿਆ_Family_Char(dest[i])){
                    seenfam_ਮਿਲਿਆ = 1;
                } ਨਹੀਂਤਾਂ ਜੇ (dest[i] == YEH_HAMZA_CHAR) {
                    yehhamza_ਮਿਲਿਆ = 1;
                }
            }

            /*
             * get the proper shape according to link ability of neighbors
             * and of character; depends on the order of the shapes
             * (isolated, initial, middle, final) in the compatibility area
             */
            Shape = shape_ਸਾਰਣੀ[ਅਗਲਾ_Link & (LINKR + LINKL)]
                              [ਆਖਰੀ_Link & (LINKR + LINKL)]
                              [currLink & (LINKR + LINKL)];

            ਜੇ ((currLink & (LINKR+LINKL)) == 1) {
                Shape &= 1;
            } ਨਹੀਂਤਾਂ ਜੇ(isTashkeelChar(dest[i])) {
                ਜੇ( (ਆਖਰੀ_Link & LINKL) && (ਅਗਲਾ_Link & LINKR) && (tashkeelFlag == 1) &&
                     dest[i] != 0x064C && dest[i] != 0x064D )
                {
                    Shape = 1;
                    ਜੇ( (ਅਗਲਾ_Link&ALEFTYPE) == ALEFTYPE && (ਆਖਰੀ_Link&LAMTYPE) == LAMTYPE ) {
                        Shape = 0;
                    }
                } ਨਹੀਂਤਾਂ ਜੇ(tashkeelFlag == 2 && dest[i] == SHADDA06_CHAR){
                    Shape = 1;
                } ਨਹੀਂਤਾਂ {
                    Shape = 0;
                }
            }
            ਜੇ ((dest[i] ^ 0x0600) < 0x100) {
                ਜੇ ( isTashkeelChar(dest[i]) ){
                    ਜੇ (tashkeelFlag == 2  && dest[i] != SHADDA06_CHAR){
                        dest[i] = TASHKEEL_SPACE_SUB;
                        tashkeel_ਮਿਲਿਆ  = 1;
                    } ਨਹੀਂਤਾਂ {
                        /* to ensure the array index is within the range */
                        U_ASSERT(dest[i] >= 0x064Bu
                            && dest[i]-0x064Bu < UPRV_LENGTHOF(IrrelevantPos));
                        dest[i] =  0xFE70 + IrrelevantPos[(dest[i] - 0x064B)] + static_cast<char16_t>(Shape);
                    }
                }ਨਹੀਂਤਾਂ ਜੇ ((currLink & APRESENT) > 0) {
                    dest[i] = static_cast<char16_t>(0xFB50 + (currLink >> 8) + Shape);
                }ਨਹੀਂਤਾਂ ਜੇ ((currLink >> 8) > 0 && (currLink & IRRELEVANT) == 0) {
                    dest[i] = static_cast<char16_t>(0xFE70 + (currLink >> 8) + Shape);
                }
            }
        }

        /* move one notch forward */
        ਜੇ ((currLink & IRRELEVANT) == 0) {
            prevLink = ਆਖਰੀ_Link;
            ਆਖਰੀ_Link = currLink;
            ਆਖਰੀ_Pos = i;
        }

        i = i + ਕਦਮ;
        ਜੇ (i == Nx) {
            currLink = ਅਗਲਾ_Link;
            Nx = -2;
        } ਨਹੀਂਤਾਂ ਜੇ(i != iend) {
            currLink = getLink(dest[i]);
        }
    }
    dest_ਮਾਪ = ਸਰੋਤ_ਲੰਮਾਈ;
    ਜੇ ( (lamalef_ਮਿਲਿਆ != 0 ) || (tashkeel_ਮਿਲਿਆ  != 0) ){
        dest_ਮਾਪ = handleGeneratedSpaces(dest,ਸਰੋਤ_ਲੰਮਾਈ,dest_ਮਾਪ,options,p_Error_ਕੋਡ, shapeVars);
    }

    ਜੇ ( (seenfam_ਮਿਲਿਆ != 0) || (yehhamza_ਮਿਲਿਆ != 0) ) {
        dest_ਮਾਪ = expandCompositChar(dest, ਸਰੋਤ_ਲੰਮਾਈ,dest_ਮਾਪ,options,p_Error_ਕੋਡ, SHAPE_ਬਹੁਲਕ,shapeVars);
    }
    ਵਾਪਸ dest_ਮਾਪ;
}

/*
 *Name     : deShapeUnicode
 *Function : Converts an Arabic Unicode buffer in FExx Range into unshaped
 *           arabic Unicode buffer in 06xx Range
 */
ਸਥਿਰ int32_t
deShapeUnicode(char16_t *dest, int32_t ਸਰੋਤ_ਲੰਮਾਈ,
               int32_t dest_ਮਾਪ,uint32_t options,
               UError_ਕੋਡ *p_Error_ਕੋਡ, ਬਣਤਰ uShapeVariables shapeVars) {
    int32_t i = 0;
    int32_t lamalef_ਮਿਲਿਆ = 0;
    int32_t yehHamzaComposeEnabled = 0;
    int32_t ਵੇਖਿਆ_Compose_Enabled = 0;

    yehHamzaComposeEnabled = ((options&U_SHAPE_YEHHAMZA_ਨਕਾਬ) == U_SHAPE_YEHHAMZA_TWOCELL_NEAR) ? 1 : 0;
    ਵੇਖਿਆ_Compose_Enabled = ((options&U_SHAPE_ਵੇਖਿਆ_ਨਕਾਬ) == U_SHAPE_ਵੇਖਿਆ_TWOCELL_NEAR)? 1 : 0;

    /*
     *This for loop changes the buffer from the Unicode FE range to
     *the Unicode 06 range
     */

    ਲਈ(i = 0; i < ਸਰੋਤ_ਲੰਮਾਈ; i++) {
        char16_t  ਨਿਵੇਸ਼_Char = dest[i];
        ਜੇ ( (ਨਿਵੇਸ਼_Char >= 0xFB50) && (ਨਿਵੇਸ਼_Char <= 0xFBFF)) { /* FBxx Arabic range */
            char16_t c = convertFBto06 [ (ਨਿਵੇਸ਼_Char - 0xFB50) ];
            ਜੇ (c != 0)
                dest[i] = c;
        } ਨਹੀਂਤਾਂ ਜੇ( (yehHamzaComposeEnabled == 1) && ((ਨਿਵੇਸ਼_Char == HAMZA06_CHAR) || (ਨਿਵੇਸ਼_Char == HAMZAFE_CHAR))
                && (i < (ਸਰੋਤ_ਲੰਮਾਈ - 1)) && isAlefMaksouraChar(dest[i+1] )) {
                 dest[i] = SPACE_CHAR;
                 dest[i+1] = YEH_HAMZA_CHAR;
        } ਨਹੀਂਤਾਂ ਜੇ ( (ਵੇਖਿਆ_Compose_Enabled == 1) && (is_ਪੂਛ_Char(ਨਿਵੇਸ਼_Char)) && (i< (ਸਰੋਤ_ਲੰਮਾਈ - 1))
                        && (is_ਵੇਖਿਆ_ਪੂਛ_Family_Char(dest[i+1])) ) {
                dest[i] = SPACE_CHAR;
        } ਨਹੀਂਤਾਂ ਜੇ (( ਨਿਵੇਸ਼_Char >= 0xFE70) && (ਨਿਵੇਸ਼_Char <= 0xFEF4 )) { /* FExx Arabic range */
                dest[i] = convertFEto06 [ (ਨਿਵੇਸ਼_Char - 0xFE70) ];
        } ਨਹੀਂਤਾਂ {
            dest[i] = ਨਿਵੇਸ਼_Char ;
        }

        ਜੇ( isLamAlefChar(dest[i]) )
            lamalef_ਮਿਲਿਆ = 1;
    }

   dest_ਮਾਪ = ਸਰੋਤ_ਲੰਮਾਈ;
   ਜੇ (lamalef_ਮਿਲਿਆ != 0){
          dest_ਮਾਪ = expandCompositChar(dest,ਸਰੋਤ_ਲੰਮਾਈ,dest_ਮਾਪ,options,p_Error_ਕੋਡ,DESHAPE_ਬਹੁਲਕ, shapeVars);
   }
   ਵਾਪਸ dest_ਮਾਪ;
}

/*
 ****************************************
 * u_shapeArabic
 ****************************************
 */

U_CAPI int32_t U_EXPORT2
u_shapeArabic(ਅਚਲ char16_t *ਸਰੋਤ, int32_t ਸਰੋਤ_ਲੰਮਾਈ,
              char16_t *dest, int32_t dest_ਸਮਰੱਥਾ,
              uint32_t options,
              UError_ਕੋਡ *p_Error_ਕੋਡ) {

    int32_t dest_ਲੰਮਾਈ;
    ਬਣਤਰ  uShapeVariables shapeVars = { OLD_ਪੂਛ_CHAR,U_SHAPE_LAMALEF_BEGIN,U_SHAPE_LAMALEF_ਸਿਰਾ,U_SHAPE_TASHKEEL_BEGIN,U_SHAPE_TASHKEEL_ਸਿਰਾ,0};

    /* usual error checking */
    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }

    /* make sure that no reserved options values are used; allow dest==nullptr only for preflighting */
    ਜੇ( ਸਰੋਤ==ਖਾਲੀ_ਸੂਚਕ || ਸਰੋਤ_ਲੰਮਾਈ<-1 || (dest==ਖਾਲੀ_ਸੂਚਕ && dest_ਸਮਰੱਥਾ!=0) || dest_ਸਮਰੱਥਾ<0 ||
                (((options&U_SHAPE_TASHKEEL_ਨਕਾਬ) > 0) &&
                 ((options&U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED) == U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED) ) ||
                (((options&U_SHAPE_TASHKEEL_ਨਕਾਬ) > 0) &&
                 ((options&U_SHAPE_LETTERS_ਨਕਾਬ) == U_SHAPE_LETTERS_UNSHAPE)) ||
                (options&U_SHAPE_DIGIT_TYPE_RESERVED)==U_SHAPE_DIGIT_TYPE_RESERVED ||
                (options&U_SHAPE_DIGITS_ਨਕਾਬ)==U_SHAPE_DIGITS_RESERVED ||
                ((options&U_SHAPE_LAMALEF_ਨਕਾਬ) != U_SHAPE_LAMALEF_RESIZE  &&
                (options&U_SHAPE_AGGREGATE_TASHKEEL_ਨਕਾਬ) != 0) ||
                ((options&U_SHAPE_AGGREGATE_TASHKEEL_ਨਕਾਬ) == U_SHAPE_AGGREGATE_TASHKEEL &&
                (options&U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED) != U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED)
    )
    {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }
    /* Validate  lamalef options */
    ਜੇ(((options&U_SHAPE_LAMALEF_ਨਕਾਬ) > 0)&&
              !(((options & U_SHAPE_LAMALEF_ਨਕਾਬ)==U_SHAPE_LAMALEF_BEGIN) ||
                ((options & U_SHAPE_LAMALEF_ਨਕਾਬ)==U_SHAPE_LAMALEF_ਸਿਰਾ ) ||
                ((options & U_SHAPE_LAMALEF_ਨਕਾਬ)==U_SHAPE_LAMALEF_RESIZE )||
                 ((options & U_SHAPE_LAMALEF_ਨਕਾਬ)==U_SHAPE_LAMALEF_AUTO) ||
                 ((options & U_SHAPE_LAMALEF_ਨਕਾਬ)==U_SHAPE_LAMALEF_NEAR)))
    {
         *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }
    /* Validate  Tashkeel options */
    ਜੇ(((options&U_SHAPE_TASHKEEL_ਨਕਾਬ) > 0)&&
                   !(((options & U_SHAPE_TASHKEEL_ਨਕਾਬ)==U_SHAPE_TASHKEEL_BEGIN) ||
                     ((options & U_SHAPE_TASHKEEL_ਨਕਾਬ)==U_SHAPE_TASHKEEL_ਸਿਰਾ )
                    ||((options & U_SHAPE_TASHKEEL_ਨਕਾਬ)==U_SHAPE_TASHKEEL_RESIZE )||
                    ((options & U_SHAPE_TASHKEEL_ਨਕਾਬ)==U_SHAPE_TASHKEEL_REPLACE_BY_TATWEEL)))
    {
         *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }
    /* determine the source length */
    ਜੇ(ਸਰੋਤ_ਲੰਮਾਈ==-1) {
        ਸਰੋਤ_ਲੰਮਾਈ=u_strlen(ਸਰੋਤ);
    }
    ਜੇ(ਸਰੋਤ_ਲੰਮਾਈ<=0) {
        ਵਾਪਸ u_terminateUChars(dest, dest_ਸਮਰੱਥਾ, 0, p_Error_ਕੋਡ);
    }

    /* check that source and destination do not overlap */
    ਜੇ( dest!=ਖਾਲੀ_ਸੂਚਕ &&
        ((ਸਰੋਤ<=dest && dest<ਸਰੋਤ+ਸਰੋਤ_ਲੰਮਾਈ) ||
         (dest<=ਸਰੋਤ && ਸਰੋਤ<dest+dest_ਸਮਰੱਥਾ))) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    /* Does Options contain the new Seen Tail Unicode code point option */
    ਜੇ ( (options&U_SHAPE_ਪੂਛ_TYPE_ਨਕਾਬ) == U_SHAPE_ਪੂਛ_NEW_UNICODE){
        shapeVars.ਪੂਛ_Char = NEW_ਪੂਛ_CHAR;
    }ਨਹੀਂਤਾਂ {
        shapeVars.ਪੂਛ_Char = OLD_ਪੂਛ_CHAR;
    }

    ਜੇ((options&U_SHAPE_LETTERS_ਨਕਾਬ)!=U_SHAPE_LETTERS_NOOP) {
        char16_t ਬਫਰ[300];
        char16_t *tempbuffer, *tempsource = ਖਾਲੀ_ਸੂਚਕ;
        int32_t output_ਮਾਪ, spacesCountl=0, spacesCountr=0;

        ਜੇ((options&U_SHAPE_AGGREGATE_TASHKEEL_ਨਕਾਬ)>0) {
            int32_t logical_ਕ੍ਰਮ = (options&U_SHAPE_ਪਾਠ_DIRECTION_ਨਕਾਬ) == U_SHAPE_ਪਾਠ_DIRECTION_LOGICAL;
            int32_t aggregate_tashkeel =
                        (options&(U_SHAPE_AGGREGATE_TASHKEEL_ਨਕਾਬ+U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED)) ==
                        (U_SHAPE_AGGREGATE_TASHKEEL+U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED);
            ਪੂਰਨ ਕਦਮ=logical_ਕ੍ਰਮ?1:-1;
            ਪੂਰਨ j=logical_ਕ੍ਰਮ?-1:2*ਸਰੋਤ_ਲੰਮਾਈ;
            ਪੂਰਨ i=logical_ਕ੍ਰਮ?-1:ਸਰੋਤ_ਲੰਮਾਈ;
            ਪੂਰਨ ਸਿਰਾ=logical_ਕ੍ਰਮ?ਸਰੋਤ_ਲੰਮਾਈ:-1;
            ਪੂਰਨ aggregation_possible = 1;
            char16_t prev = 0;
            char16_t prevLink, currLink = 0;
            ਪੂਰਨ new_ਸਰੋਤ_ਲੰਮਾਈ = 0;
            tempsource = (char16_t *)uprv_malloc(2*ਸਰੋਤ_ਲੰਮਾਈ*U_SIZEOF_UCHAR);
            ਜੇ(tempsource == ਖਾਲੀ_ਸੂਚਕ) {
                *p_Error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                ਵਾਪਸ 0;
            }

            ਜਦੋਂ ((i+=ਕਦਮ) != ਸਿਰਾ) {
                prevLink = currLink;
                currLink = getLink(ਸਰੋਤ[i]);
                ਜੇ (aggregate_tashkeel && ((prevLink|currLink)&COMBINE) == COMBINE && aggregation_possible) {
                    ਜੇ (j<0 || j>=2*ਸਰੋਤ_ਲੰਮਾਈ) {
                        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
                        ਜੇ (tempsource != ਖਾਲੀ_ਸੂਚਕ) uprv_free(tempsource);
                        ਵਾਪਸ 0;
                    }
                    aggregation_possible = 0;
                    tempsource[j] = (prev<ਸਰੋਤ[i]?prev:ਸਰੋਤ[i])-0x064C+0xFC5E;
                    currLink = getLink(tempsource[j]);
                } ਨਹੀਂਤਾਂ {
                    aggregation_possible = 1;
                    tempsource[j+=ਕਦਮ] = ਸਰੋਤ[i];
                    prev = ਸਰੋਤ[i];
                    new_ਸਰੋਤ_ਲੰਮਾਈ++;
                }
            }
            ਸਰੋਤ = tempsource+(logical_ਕ੍ਰਮ?0:j);
            ਸਰੋਤ_ਲੰਮਾਈ = new_ਸਰੋਤ_ਲੰਮਾਈ;
        }

        /* calculate destination size */
        /* TODO: do we ever need to do this pure preflighting? */
        ਜੇ(((options&U_SHAPE_LAMALEF_ਨਕਾਬ)==U_SHAPE_LAMALEF_RESIZE) ||
           ((options&U_SHAPE_TASHKEEL_ਨਕਾਬ)==U_SHAPE_TASHKEEL_RESIZE)) {
            output_ਮਾਪ=calculate_ਮਾਪ(ਸਰੋਤ,ਸਰੋਤ_ਲੰਮਾਈ,dest_ਸਮਰੱਥਾ,options);
        } ਨਹੀਂਤਾਂ {
            output_ਮਾਪ=ਸਰੋਤ_ਲੰਮਾਈ;
        }

        ਜੇ(output_ਮਾਪ>dest_ਸਮਰੱਥਾ) {
            *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                ਜੇ (tempsource != ਖਾਲੀ_ਸੂਚਕ) uprv_free(tempsource);
            ਵਾਪਸ output_ਮਾਪ;
        }

        /*
         * need a temporary buffer of size max(outputSize, sourceLength)
         * because at first we copy source->temp
         */
        ਜੇ(ਸਰੋਤ_ਲੰਮਾਈ>output_ਮਾਪ) {
            output_ਮਾਪ=ਸਰੋਤ_ਲੰਮਾਈ;
        }

        /* Start of Arabic letter shaping part */
        ਜੇ(output_ਮਾਪ<=UPRV_LENGTHOF(ਬਫਰ)) {
            output_ਮਾਪ=UPRV_LENGTHOF(ਬਫਰ);
            tempbuffer=ਬਫਰ;
        } ਨਹੀਂਤਾਂ {
            tempbuffer = (char16_t *)uprv_malloc(output_ਮਾਪ*U_SIZEOF_UCHAR);

            /*Test for nullptr*/
            ਜੇ(tempbuffer == ਖਾਲੀ_ਸੂਚਕ) {
                *p_Error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                ਜੇ (tempsource != ਖਾਲੀ_ਸੂਚਕ) uprv_free(tempsource);
                ਵਾਪਸ 0;
            }
        }
        u_memcpy(tempbuffer, ਸਰੋਤ, ਸਰੋਤ_ਲੰਮਾਈ);
        ਜੇ (tempsource != ਖਾਲੀ_ਸੂਚਕ){
            uprv_free(tempsource);
        }

        ਜੇ(ਸਰੋਤ_ਲੰਮਾਈ<output_ਮਾਪ) {
            uprv_memset(tempbuffer+ਸਰੋਤ_ਲੰਮਾਈ, 0, (output_ਮਾਪ-ਸਰੋਤ_ਲੰਮਾਈ)*U_SIZEOF_UCHAR);
        }

        ਜੇ((options&U_SHAPE_ਪਾਠ_DIRECTION_ਨਕਾਬ) == U_SHAPE_ਪਾਠ_DIRECTION_LOGICAL) {
            ਗਿਣਤੀ_Spaces(tempbuffer,ਸਰੋਤ_ਲੰਮਾਈ,options,&spacesCountl,&spacesCountr);
            invert_ਬਫਰ(tempbuffer,ਸਰੋਤ_ਲੰਮਾਈ,options,spacesCountl,spacesCountr);
        }

        ਜੇ((options&U_SHAPE_ਪਾਠ_DIRECTION_ਨਕਾਬ) == U_SHAPE_ਪਾਠ_DIRECTION_VISUAL_LTR) {
            ਜੇ((options&U_SHAPE_SPACES_RELATIVE_ਮੰਜ਼ਿਲ_ਪਾਠ_ਨਕਾਬ) == U_SHAPE_SPACES_RELATIVE_ਮੰਜ਼ਿਲ_ਪਾਠ_BEGIN_ਸਿਰਾ) {
                shapeVars.spaces_Relative_ਮੰਜ਼ਿਲ_ਪਾਠ_Begin_ਸਿਰਾ = 1;
                shapeVars.uShapeLamalefBegin = U_SHAPE_LAMALEF_ਸਿਰਾ;
                shapeVars.u_Shape_Lamalef_ਸਿਰਾ = U_SHAPE_LAMALEF_BEGIN;
                shapeVars.uShapeTashkeelBegin = U_SHAPE_TASHKEEL_ਸਿਰਾ;
                shapeVars.u_Shape_Tashkeel_ਸਿਰਾ = U_SHAPE_TASHKEEL_BEGIN;
            }
        }

        ਚੋਣ(options&U_SHAPE_LETTERS_ਨਕਾਬ) {
        ਹਾਲਤ U_SHAPE_LETTERS_SHAPE :
             ਜੇ( (options&U_SHAPE_TASHKEEL_ਨਕਾਬ)> 0
                 && ((options&U_SHAPE_TASHKEEL_ਨਕਾਬ) !=U_SHAPE_TASHKEEL_REPLACE_BY_TATWEEL)) {
                /* Call the shaping function with tashkeel flag == 2 for removal of tashkeel */
                dest_ਲੰਮਾਈ = shapeUnicode(tempbuffer,ਸਰੋਤ_ਲੰਮਾਈ,dest_ਸਮਰੱਥਾ,options,p_Error_ਕੋਡ,2,shapeVars);
             }ਨਹੀਂਤਾਂ {
                /* default Call the shaping function with tashkeel flag == 1 */
                dest_ਲੰਮਾਈ = shapeUnicode(tempbuffer,ਸਰੋਤ_ਲੰਮਾਈ,dest_ਸਮਰੱਥਾ,options,p_Error_ਕੋਡ,1,shapeVars);

                /*After shaping text check if user wants to remove tashkeel and replace it with tatweel*/
                ਜੇ( (options&U_SHAPE_TASHKEEL_ਨਕਾਬ) == U_SHAPE_TASHKEEL_REPLACE_BY_TATWEEL){
                  dest_ਲੰਮਾਈ = handleTashkeelWithTatweel(tempbuffer,dest_ਲੰਮਾਈ,dest_ਸਮਰੱਥਾ,options,p_Error_ਕੋਡ);
                }
            }
            ਰੋਕੋ;
        ਹਾਲਤ U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED :
            /* Call the shaping function with tashkeel flag == 0 */
            dest_ਲੰਮਾਈ = shapeUnicode(tempbuffer,ਸਰੋਤ_ਲੰਮਾਈ,dest_ਸਮਰੱਥਾ,options,p_Error_ਕੋਡ,0,shapeVars);
            ਰੋਕੋ;

        ਹਾਲਤ U_SHAPE_LETTERS_UNSHAPE :
            /* Call the deshaping function */
            dest_ਲੰਮਾਈ = deShapeUnicode(tempbuffer,ਸਰੋਤ_ਲੰਮਾਈ,dest_ਸਮਰੱਥਾ,options,p_Error_ਕੋਡ,shapeVars);
            ਰੋਕੋ;
        ਮੂਲ :
            /* will never occur because of validity checks above */
            dest_ਲੰਮਾਈ = 0;
            ਰੋਕੋ;
        }

        /*
         * TODO: (markus 2002aug01)
         * For as long as we always preflight the outputSize above
         * we should U_ASSERT(outputSize==destLength)
         * except for the adjustment above before the tempbuffer allocation
         */

        ਜੇ((options&U_SHAPE_ਪਾਠ_DIRECTION_ਨਕਾਬ) == U_SHAPE_ਪਾਠ_DIRECTION_LOGICAL) {
            ਗਿਣਤੀ_Spaces(tempbuffer,dest_ਲੰਮਾਈ,options,&spacesCountl,&spacesCountr);
            invert_ਬਫਰ(tempbuffer,dest_ਲੰਮਾਈ,options,spacesCountl,spacesCountr);
        }
        u_memcpy(dest, tempbuffer, uprv_min(dest_ਲੰਮਾਈ, dest_ਸਮਰੱਥਾ));

        ਜੇ(tempbuffer!=ਬਫਰ) {
            uprv_free(tempbuffer);
        }

        ਜੇ(dest_ਲੰਮਾਈ>dest_ਸਮਰੱਥਾ) {
            *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
            ਵਾਪਸ dest_ਲੰਮਾਈ;
        }

        /* End of Arabic letter shaping part */
    } ਨਹੀਂਤਾਂ {
        /*
         * No letter shaping:
         * just make sure the destination is large enough and copy the string.
         */
        ਜੇ(dest_ਸਮਰੱਥਾ<ਸਰੋਤ_ਲੰਮਾਈ) {
            /* this catches preflighting, too */
            *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
            ਵਾਪਸ ਸਰੋਤ_ਲੰਮਾਈ;
        }
        u_memcpy(dest, ਸਰੋਤ, ਸਰੋਤ_ਲੰਮਾਈ);
        dest_ਲੰਮਾਈ=ਸਰੋਤ_ਲੰਮਾਈ;
    }

    /*
     * Perform number shaping.
     * With UTF-16 or UTF-32, the length of the string is constant.
     * The easiest way to do this is to operate on the destination and
     * "shape" the digits in-place.
     */
    ਜੇ((options&U_SHAPE_DIGITS_ਨਕਾਬ)!=U_SHAPE_DIGITS_NOOP) {
        char16_t digit_ਅਧਾਰ;
        int32_t i;

        /* select the requested digit group */
        ਚੋਣ(options&U_SHAPE_DIGIT_TYPE_ਨਕਾਬ) {
        ਹਾਲਤ U_SHAPE_DIGIT_TYPE_AN:
            digit_ਅਧਾਰ=0x660; /* Unicode: "Arabic-Indic digits" */
            ਰੋਕੋ;
        ਹਾਲਤ U_SHAPE_DIGIT_TYPE_AN_EXTENDED:
            digit_ਅਧਾਰ=0x6f0; /* Unicode: "Eastern Arabic-Indic digits (Persian and Urdu)" */
            ਰੋਕੋ;
        ਮੂਲ:
            /* will never occur because of validity checks above */
            digit_ਅਧਾਰ=0;
            ਰੋਕੋ;
        }

        /* perform the requested operation */
        ਚੋਣ(options&U_SHAPE_DIGITS_ਨਕਾਬ) {
        ਹਾਲਤ U_SHAPE_DIGITS_EN2AN:
            /* add (digitBase-'0') to each European (ASCII) digit code point */
            digit_ਅਧਾਰ-=0x30;
            ਲਈ(i=0; i<dest_ਲੰਮਾਈ; ++i) {
                ਜੇ(((uint32_t)dest[i]-0x30)<10) {
                    dest[i]+=digit_ਅਧਾਰ;
                }
            }
            ਰੋਕੋ;
        ਹਾਲਤ U_SHAPE_DIGITS_AN2EN:
            /* subtract (digitBase-'0') from each Arabic digit code point */
            ਲਈ(i=0; i<dest_ਲੰਮਾਈ; ++i) {
                ਜੇ(((uint32_t)dest[i]-(uint32_t)digit_ਅਧਾਰ)<10) {
                    dest[i]-=digit_ਅਧਾਰ-0x30;
                }
            }
            ਰੋਕੋ;
        ਹਾਲਤ U_SHAPE_DIGITS_ALEN2AN_INIT_LR:
            shape_ਮੰਜ਼ਿਲ_Arabic_Digits_With_Context(dest, dest_ਲੰਮਾਈ,
                                            digit_ਅਧਾਰ,
                                            (options & U_SHAPE_ਪਾਠ_DIRECTION_ਨਕਾਬ) == U_SHAPE_ਪਾਠ_DIRECTION_LOGICAL,
                                            ਝੂਠ);
            ਰੋਕੋ;
        ਹਾਲਤ U_SHAPE_DIGITS_ALEN2AN_INIT_AL:
            shape_ਮੰਜ਼ਿਲ_Arabic_Digits_With_Context(dest, dest_ਲੰਮਾਈ,
                                            digit_ਅਧਾਰ,
                                            (options & U_SHAPE_ਪਾਠ_DIRECTION_ਨਕਾਬ) == U_SHAPE_ਪਾਠ_DIRECTION_LOGICAL,
                                            ਸੱਚ);
            ਰੋਕੋ;
        ਮੂਲ:
            /* will never occur because of validity checks above */
            ਰੋਕੋ;
        }
    }

    ਵਾਪਸ u_terminateUChars(dest, dest_ਸਮਰੱਥਾ, dest_ਲੰਮਾਈ, p_Error_ਕੋਡ);
}
