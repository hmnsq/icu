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

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/uchar.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "unicode/ushape.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "putilimp.h"
#అన్తర్భావ "ustr_imp.h"
#అన్తర్భావ "ubidi_props.h"
#అన్తర్భావ "uassert.h"

#అన్తర్భావ <limits>
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

#నిర్వచించు IRRELEVANT 4
#నిర్వచించు LAMTYPE    16
#నిర్వచించు ALEFTYPE   32
#నిర్వచించు LINKR      1
#నిర్వచించు LINKL      2
#నిర్వచించు APRESENT   8
#నిర్వచించు SHADDA     64
#నిర్వచించు CSHADDA    128
#నిర్వచించు COMBINE    (SHADDA+CSHADDA)

#నిర్వచించు HAMZAFE_CHAR       0xfe80
#నిర్వచించు HAMZA06_CHAR       0x0621
#నిర్వచించు YEH_HAMZA_CHAR     0x0626
#నిర్వచించు YEH_HAMZAFE_CHAR   0xFE89
#నిర్వచించు LAMALEF_SPACE_SUB  0xFFFF
#నిర్వచించు TASHKEEL_SPACE_SUB 0xFFFE
#నిర్వచించు NEW_తోక_CHAR      0xFE73
#నిర్వచించు OLD_తోక_CHAR      0x200B
#నిర్వచించు LAM_CHAR           0x0644
#నిర్వచించు SPACE_CHAR         0x0020
#నిర్వచించు SHADDA_CHAR        0xFE7C
#నిర్వచించు TATWEEL_CHAR       0x0640
#నిర్వచించు SHADDA_TATWEEL_CHAR  0xFE7D
#నిర్వచించు SHADDA06_CHAR      0x0651

#నిర్వచించు SHAPE_బహుళకం   0
#నిర్వచించు DESHAPE_బహుళకం 1

నిర్మాణం uShapeVariables {
     char16_t తోక_Char;
     uint32_t uShapeLamalefBegin;
     uint32_t u_Shape_Lamalef_చివర;
     uint32_t uShapeTashkeelBegin;
     uint32_t u_Shape_Tashkeel_చివర;
     పూర్ణాంకం spaces_Relative_గమ్యం_పాఠ్యం_Begin_చివర;
};

స్థిర స్థిరరాశి uint8_t తోక_Family_Isolated_Final[] = {
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

స్థిర స్థిరరాశి uint8_t tashkeelMedial[] = {
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

స్థిర స్థిరరాశి char16_t yeh_Hamza_గమ్యం_Yeh[] =
{
/* isolated*/ 0xFEEF,
/* final   */ 0xFEF0
};

స్థిర స్థిరరాశి uint8_t IrrelevantPos[] = {
    0x0, 0x2, 0x4, 0x6,
    0x8, 0xA, 0xC, 0xE
};


స్థిర స్థిరరాశి char16_t convertLamAlef[] =
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

స్థిర స్థిరరాశి char16_t araLink[178]=
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

స్థిర స్థిరరాశి uint8_t presALink[] = {
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

స్థిర స్థిరరాశి uint8_t presBLink[]=
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

స్థిర స్థిరరాశి char16_t convertFBto06[] =
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

స్థిర స్థిరరాశి char16_t convertFEto06[] =
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

స్థిర స్థిరరాశి uint8_t shape_పట్టిక[4][4][4]=
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
స్థిర శూన్య
shape_గమ్యం_Arabic_Digits_With_Context(char16_t *s, int32_t length,
                                char16_t digit_పునాది,
                                UBool isLogical, UBool చివరి_Strong_Was_AL) {
    int32_t i;
    char16_t c;

    digit_పునాది-=0x30;

    /* the iteration direction depends on the type of input */
    ఒకవేళ(isLogical) {
        కొరకు(i=0; i<length; ++i) {
            c=s[i];
            ఎంపిక(ubidi_getClass(c)) {
            స్థితి U_ఎడమ_గమ్యం_కుడి: /* L */
            స్థితి U_కుడి_గమ్యం_ఎడమ: /* R */
                చివరి_Strong_Was_AL=అబద్ధం;
                ఆపు;
            స్థితి U_కుడి_గమ్యం_ఎడమ_ARABIC: /* AL */
                చివరి_Strong_Was_AL=నిజం;
                ఆపు;
            స్థితి U_EUROPEAN_సంఖ్య: /* EN */
                ఒకవేళ (చివరి_Strong_Was_AL && static_cast<uint32_t>(c - 0x30) < 10) {
                    s[i] = static_cast<char16_t>(digit_పునాది + c); /* digitBase+(c-0x30) - digitBase was modified above */
                }
                ఆపు;
            అప్రమేయ :
                ఆపు;
            }
        }
    } లేకపోతే {
        కొరకు(i=length; i>0; /* pre-decrement in the body */) {
            c=s[--i];
            ఎంపిక(ubidi_getClass(c)) {
            స్థితి U_ఎడమ_గమ్యం_కుడి: /* L */
            స్థితి U_కుడి_గమ్యం_ఎడమ: /* R */
                చివరి_Strong_Was_AL=అబద్ధం;
                ఆపు;
            స్థితి U_కుడి_గమ్యం_ఎడమ_ARABIC: /* AL */
                చివరి_Strong_Was_AL=నిజం;
                ఆపు;
            స్థితి U_EUROPEAN_సంఖ్య: /* EN */
                ఒకవేళ (చివరి_Strong_Was_AL && static_cast<uint32_t>(c - 0x30) < 10) {
                    s[i] = static_cast<char16_t>(digit_పునాది + c); /* digitBase+(c-0x30) - digitBase was modified above */
                }
                ఆపు;
            అప్రమేయ :
                ఆపు;
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
స్థిర శూన్య
invert_బఫరు(char16_t *బఫరు, int32_t size, uint32_t /*options*/, int32_t lowlimit, int32_t highlimit) {
    char16_t తాత్కాలిక;
    int32_t i=0,j=0;
    కొరకు(i=lowlimit,j=size-highlimit-1;i<j;i++,j--) {
        తాత్కాలిక = బఫరు[i];
        బఫరు[i] = బఫరు[j];
        బఫరు[j] = తాత్కాలిక;
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
స్థిర అంతఃవరుస char16_t
changeLamAlef(char16_t ch) {
    ఎంపిక(ch) {
    స్థితి 0x0622 :
        తిరిగి 0x065C;
    స్థితి 0x0623 :
        తిరిగి 0x065D;
    స్థితి 0x0625 :
        తిరిగి 0x065E;
    స్థితి 0x0627 :
        తిరిగి 0x065F;
    }
    తిరిగి 0;
}

/*
 *Name     : getLink
 *Function : Resolves the link between the characters as
 *           Arabic characters have four forms :
 *           Isolated, Initial, Middle and Final Form
 */
స్థిర char16_t
getLink(char16_t ch) {
    ఒకవేళ(ch >= 0x0622 && ch <= 0x06D3) {
        తిరిగి(araLink[ch-0x0622]);
    } లేకపోతే ఒకవేళ(ch == 0x200D) {
        తిరిగి(3);
    } లేకపోతే ఒకవేళ(ch >= 0x206D && ch <= 0x206F) {
        తిరిగి(4);
    }లేకపోతే ఒకవేళ(ch >= 0xFB50 && ch <= 0xFC62) {
        తిరిగి(presALink[ch-0xFB50]);
    } లేకపోతే ఒకవేళ(ch >= 0xFE70 && ch <= 0xFEFC) {
        తిరిగి(presBLink[ch-0xFE70]);
    }లేకపోతే {
        తిరిగి(0);
    }
}

/*
 *Name     : countSpaces
 *Function : Counts the number of spaces
 *           at each end of the logical buffer
 */
స్థిర శూన్య
లెక్క_Spaces(char16_t *dest, int32_t size, uint32_t /*options*/, int32_t *spacesCountl, int32_t *spacesCountr) {
    int32_t i = 0;
    int32_t countl = 0,countr = 0;
    అయితే((dest[i] == SPACE_CHAR) && (countl < size)) {
       countl++;
       i++;
    }
    ఒకవేళ (countl < size) {  /* the entire buffer is not all space */
        అయితే(dest[size-1] == SPACE_CHAR) {
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
స్థిర అంతఃవరుస int32_t
isTashkeelChar(char16_t ch) {
    తిరిగి static_cast<int32_t>(ch >= 0x064B && ch <= 0x0652);
}

/*
 *Name     : isTashkeelCharFE
 *Function : Returns 1 for Tashkeel characters in FE range else return 0
 */
స్థిర అంతఃవరుస int32_t
isTashkeelCharFE(char16_t ch) {
    తిరిగి static_cast<int32_t>(ch >= 0xFE70 && ch <= 0xFE7F);
}

/*
 *Name     : isAlefChar
 *Function : Returns 1 for Alef characters else return 0
 */
స్థిర అంతఃవరుస int32_t
isAlefChar(char16_t ch) {
    తిరిగి static_cast<int32_t>(ch == 0x0622 || ch == 0x0623 || ch == 0x0625 || ch == 0x0627);
}

/*
 *Name     : isLamAlefChar
 *Function : Returns 1 for LamAlef characters else return 0
 */
స్థిర అంతఃవరుస int32_t
isLamAlefChar(char16_t ch) {
    తిరిగి static_cast<int32_t>(ch >= 0xFEF5 && ch <= 0xFEFC);
}

/*BIDI
 *Name     : isTailChar
 *Function : returns 1 if the character matches one of the tail characters (0xfe73 or 0x200b) otherwise returns 0
 */

స్థిర అంతఃవరుస int32_t
is_తోక_Char(char16_t ch) {
    ఒకవేళ(ch == OLD_తోక_CHAR || ch == NEW_తోక_CHAR){
            తిరిగి 1;
    }లేకపోతే{
            తిరిగి 0;
    }
}

/*BIDI
 *Name     : isSeenTailFamilyChar
 *Function : returns 1 if the character is a seen family isolated character
 *           in the FE range otherwise returns 0
 */

స్థిర అంతఃవరుస int32_t
is_చూసిన_తోక_Family_Char(char16_t ch) {
    ఒకవేళ(ch >= 0xfeb1 && ch < 0xfebf){
            తిరిగి తోక_Family_Isolated_Final [ch - 0xFEB1];
    }లేకపోతే{
            తిరిగి 0;
    }
}

 /* Name     : isSeenFamilyChar
  * Function : returns 1 if the character is a seen family character in the Unicode
  *            06 range otherwise returns 0
 */

స్థిర అంతఃవరుస int32_t
is_చూసిన_Family_Char(char16_t  ch){
    ఒకవేళ(ch >= 0x633 && ch <= 0x636){
        తిరిగి 1;
    }లేకపోతే {
        తిరిగి 0;
    }
}

/*Start of BIDI*/
/*
 *Name     : isAlefMaksouraChar
 *Function : returns 1 if the character is a Alef Maksoura Final or isolated
 *           otherwise returns 0
 */
స్థిర అంతఃవరుస int32_t
isAlefMaksouraChar(char16_t ch) {
    తిరిగి static_cast<int32_t>(ch == 0xFEEF || ch == 0xFEF0 || ch == 0x0649);
}

/*
 * Name     : isYehHamzaChar
 * Function : returns 1 if the character is a yehHamza isolated or yehhamza
 *            final is found otherwise returns 0
 */
స్థిర అంతఃవరుస int32_t
isYehHamzaChar(char16_t ch) {
    ఒకవేళ((ch==0xFE89)||(ch==0xFE8A)){
        తిరిగి 1;
    }లేకపోతే{
        తిరిగి 0;
    }
}

 /*
 * Name: isTashkeelOnTatweelChar
 * Function: Checks if the Tashkeel Character is on Tatweel or not,if the
 *           Tashkeel on tatweel (FE range), it returns 1 else if the
 *           Tashkeel with shadda on tatweel (FC range)return 2 otherwise
 *           returns 0
 */
స్థిర అంతఃవరుస int32_t
isTashkeelOnTatweelChar(char16_t ch){
    ఒకవేళ(ch >= 0xfe70 && ch <= 0xfe7f && ch != NEW_తోక_CHAR && ch != 0xFE75 && ch != SHADDA_TATWEEL_CHAR)
    {
        తిరిగి tashkeelMedial [ch - 0xFE70];
    }లేకపోతే ఒకవేళ( (ch >= 0xfcf2 && ch <= 0xfcf4) || (ch == SHADDA_TATWEEL_CHAR)) {
        తిరిగి 2;
    }లేకపోతే{
        తిరిగి 0;
    }
}

/*
 * Name: isIsolatedTashkeelChar
 * Function: Checks if the Tashkeel Character is in the isolated form
 *           (i.e. Unicode FE range) returns 1 else if the Tashkeel
 *           with shadda is in the isolated form (i.e. Unicode FC range)
 *           returns 2 otherwise returns 0
 */
స్థిర అంతఃవరుస int32_t
isIsolatedTashkeelChar(char16_t ch){
    ఒకవేళ(ch >= 0xfe70 && ch <= 0xfe7f && ch != NEW_తోక_CHAR && ch != 0xFE75){
        తిరిగి (1 - tashkeelMedial [ch - 0xFE70]);
    }లేకపోతే ఒకవేళ(ch >= 0xfc5e && ch <= 0xfc63){
        తిరిగి 1;
    }లేకపోతే{
        తిరిగి 0;
    }
}




/*
 *Name     : calculateSize
 *Function : This function calculates the destSize to be used in preflighting
 *           when the destSize is equal to 0
 *           It is used also to calculate the new destsize in case the
 *           destination buffer will be resized.
 */

స్థిర int32_t
calculate_కొలత(స్థిరరాశి char16_t *ఆధారం, int32_t ఆధారం_నిడివి,
int32_t dest_కొలత,uint32_t options) {
    int32_t i = 0;

    పూర్ణాంకం lamAlefOption = 0;
    పూర్ణాంకం tashkeelOption = 0;

    dest_కొలత = ఆధారం_నిడివి;

    ఒకవేళ (((options&U_SHAPE_LETTERS_ముసుగు) == U_SHAPE_LETTERS_SHAPE ||
        ((options&U_SHAPE_LETTERS_ముసుగు) == U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED )) &&
        ((options&U_SHAPE_LAMALEF_ముసుగు) == U_SHAPE_LAMALEF_RESIZE )){
            lamAlefOption = 1;
    }
    ఒకవేళ((options&U_SHAPE_LETTERS_ముసుగు) == U_SHAPE_LETTERS_SHAPE &&
       ((options&U_SHAPE_TASHKEEL_ముసుగు) == U_SHAPE_TASHKEEL_RESIZE ) ){
            tashkeelOption = 1;
        }

    ఒకవేళ(lamAlefOption || tashkeelOption){
        ఒకవేళ((options&U_SHAPE_పాఠ్యం_DIRECTION_ముసుగు)==U_SHAPE_పాఠ్యం_DIRECTION_VISUAL_LTR) {
            కొరకు(i=0;i<ఆధారం_నిడివి;i++) {
                ఒకవేళ( ((isAlefChar(ఆధారం[i]))&& (i<(ఆధారం_నిడివి-1)) &&(ఆధారం[i+1] == LAM_CHAR)) || (isTashkeelCharFE(ఆధారం[i])) ) {
                        dest_కొలత--;
                    }
                }
            }లేకపోతే ఒకవేళ((options&U_SHAPE_పాఠ్యం_DIRECTION_ముసుగు)==U_SHAPE_పాఠ్యం_DIRECTION_LOGICAL) {
                కొరకు(i=0;i<ఆధారం_నిడివి;i++) {
                    ఒకవేళ( ( (ఆధారం[i] == LAM_CHAR) && (i<(ఆధారం_నిడివి-1)) && (isAlefChar(ఆధారం[i+1]))) || (isTashkeelCharFE(ఆధారం[i])) ) {
                        dest_కొలత--;
                    }
                }
            }
        }

    ఒకవేళ ((options&U_SHAPE_LETTERS_ముసుగు) == U_SHAPE_LETTERS_UNSHAPE){
        ఒకవేళ ( (options&U_SHAPE_LAMALEF_ముసుగు) == U_SHAPE_LAMALEF_RESIZE){
            కొరకు(i=0;i<ఆధారం_నిడివి;i++) {
                ఒకవేళ(isLamAlefChar(ఆధారం[i]))
                dest_కొలత++;
            }
        }
    }

    తిరిగి dest_కొలత;
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
స్థిర int32_t
handleTashkeelWithTatweel(char16_t *dest, int32_t ఆధారం_నిడివి,
             int32_t /*destSize*/, uint32_t /*options*/,
             UError_సంకేతం * /*pErrorCode*/) {
                 పూర్ణాంకం i;
                 కొరకు(i = 0; i < ఆధారం_నిడివి; i++){
                     ఒకవేళ((isTashkeelOnTatweelChar(dest[i]) == 1)){
                         dest[i] = TATWEEL_CHAR;
                    }లేకపోతే ఒకవేళ((isTashkeelOnTatweelChar(dest[i]) == 2)){
                         dest[i] = SHADDA_TATWEEL_CHAR;
                    }లేకపోతే ఒకవేళ(isIsolatedTashkeelChar(dest[i]) && dest[i] != SHADDA_CHAR){
                         dest[i] = SPACE_CHAR;
                    }
                 }
                 తిరిగి ఆధారం_నిడివి;
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

స్థిర int32_t
handleGeneratedSpaces(char16_t *dest, int32_t ఆధారం_నిడివి,
                    int32_t dest_కొలత,
                    uint32_t options,
                    UError_సంకేతం *p_Error_సంకేతం,నిర్మాణం uShapeVariables shapeVars ) {

    int32_t i = 0, j = 0;
    int32_t count = 0;
    char16_t *tempbuffer=శూన్యసూచిక;

    పూర్ణాంకం lamAlefOption = 0;
    పూర్ణాంకం tashkeelOption = 0;
    పూర్ణాంకం shaping_బహుళకం = SHAPE_బహుళకం;

    ఒకవేళ (shaping_బహుళకం == 0){
        ఒకవేళ ( (options&U_SHAPE_LAMALEF_ముసుగు) == U_SHAPE_LAMALEF_RESIZE ){
            lamAlefOption = 1;
        }
        ఒకవేళ ( (options&U_SHAPE_TASHKEEL_ముసుగు) == U_SHAPE_TASHKEEL_RESIZE ){
            tashkeelOption = 1;
        }
    }

    ఒకవేళ (static_cast<size_t>(ఆధారం_నిడివి) + 1 > std::numeric_limits<size_t>::max() / U_SIZEOF_UCHAR) {
        *p_Error_సంకేతం = U_సూచీ_OUTOFBOUNDS_ERROR;
        తిరిగి 0;
    }
    tempbuffer = static_cast<char16_t*>(uprv_malloc((ఆధారం_నిడివి + 1) * U_SIZEOF_UCHAR));
    /* Test for nullptr */
    ఒకవేళ(tempbuffer == శూన్యసూచిక) {
        *p_Error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి 0;
    }


    ఒకవేళ (lamAlefOption || tashkeelOption){
        uprv_memset(tempbuffer, 0, (ఆధారం_నిడివి+1)*U_SIZEOF_UCHAR);

        i = j = 0; count = 0;
        అయితే(i < ఆధారం_నిడివి) {
            ఒకవేళ ( (lamAlefOption && dest[i] == LAMALEF_SPACE_SUB) ||
               (tashkeelOption && dest[i] == TASHKEEL_SPACE_SUB) ){
                j--;
                count++;
            } లేకపోతే {
                tempbuffer[j] = dest[i];
            }
            i++;
            j++;
        }

        అయితే(count >= 0) {
            tempbuffer[i] = 0x0000;
            i--;
            count--;
        }

        u_memcpy(dest, tempbuffer, ఆధారం_నిడివి);
        dest_కొలత = u_strlen(dest);
    }

      lamAlefOption = 0;

    ఒకవేళ (shaping_బహుళకం == 0){
        ఒకవేళ ( (options&U_SHAPE_LAMALEF_ముసుగు) == U_SHAPE_LAMALEF_NEAR ){
            lamAlefOption = 1;
        }
    }

    ఒకవేళ (lamAlefOption){
        /* Lam+Alef is already shaped into LamAlef + FFFF */
        i = 0;
        అయితే(i < ఆధారం_నిడివి) {
            ఒకవేళ(lamAlefOption&&dest[i] == LAMALEF_SPACE_SUB){
                dest[i] = SPACE_CHAR;
            }
            i++;
        }
        dest_కొలత = ఆధారం_నిడివి;
    }
    lamAlefOption = 0;
    tashkeelOption = 0;

    ఒకవేళ (shaping_బహుళకం == 0) {
        ఒకవేళ ( ((options&U_SHAPE_LAMALEF_ముసుగు) == shapeVars.uShapeLamalefBegin) ||
              (((options&U_SHAPE_LAMALEF_ముసుగు) == U_SHAPE_LAMALEF_AUTO )
              && (shapeVars.spaces_Relative_గమ్యం_పాఠ్యం_Begin_చివర==1)) ) {
            lamAlefOption = 1;
        }
        ఒకవేళ ( (options&U_SHAPE_TASHKEEL_ముసుగు) == shapeVars.uShapeTashkeelBegin ) {
            tashkeelOption = 1;
        }
    }

    ఒకవేళ(lamAlefOption || tashkeelOption){
        uprv_memset(tempbuffer, 0, (ఆధారం_నిడివి+1)*U_SIZEOF_UCHAR);
        
        i = j = ఆధారం_నిడివి; count = 0;
        
        అయితే(i >= 0) {
            ఒకవేళ ( (lamAlefOption && dest[i] == LAMALEF_SPACE_SUB) ||
                 (tashkeelOption && dest[i] == TASHKEEL_SPACE_SUB) ){
                j++;
                count++;
            }లేకపోతే {
                tempbuffer[j] = dest[i];
            }
            i--;
            j--;
        }

        కొరకు(i=0 ;i < count; i++){
                tempbuffer[i] = SPACE_CHAR;
        }

        u_memcpy(dest, tempbuffer, ఆధారం_నిడివి);
        dest_కొలత = ఆధారం_నిడివి;
    }



    lamAlefOption = 0;
    tashkeelOption = 0;

    ఒకవేళ (shaping_బహుళకం == 0) {
        ఒకవేళ ( ((options&U_SHAPE_LAMALEF_ముసుగు) == shapeVars.u_Shape_Lamalef_చివర) ||
              (((options&U_SHAPE_LAMALEF_ముసుగు) == U_SHAPE_LAMALEF_AUTO )
              && (shapeVars.spaces_Relative_గమ్యం_పాఠ్యం_Begin_చివర==0)) ) {
            lamAlefOption = 1;
        }
        ఒకవేళ ( (options&U_SHAPE_TASHKEEL_ముసుగు) == shapeVars.u_Shape_Tashkeel_చివర ){
            tashkeelOption = 1;
        }
    }

    ఒకవేళ(lamAlefOption || tashkeelOption){
        uprv_memset(tempbuffer, 0, (ఆధారం_నిడివి+1)*U_SIZEOF_UCHAR);

        i = j = 0; count = 0;
        అయితే(i < ఆధారం_నిడివి) {
            ఒకవేళ ( (lamAlefOption && dest[i] == LAMALEF_SPACE_SUB) ||
                 (tashkeelOption && dest[i] == TASHKEEL_SPACE_SUB) ){
                j--;
                count++;
            }లేకపోతే {
                tempbuffer[j] = dest[i];
            }
            i++;
            j++;
        }

        అయితే(count >= 0) {
            tempbuffer[i] = SPACE_CHAR;
            i--;
            count--;
        }

        u_memcpy(dest, tempbuffer, ఆధారం_నిడివి);
        dest_కొలత = ఆధారం_నిడివి;
    }


    ఒకవేళ(tempbuffer){
        uprv_free(tempbuffer);
    }

    తిరిగి dest_కొలత;
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

స్థిర int32_t
expandCompositCharAtBegin(char16_t *dest, int32_t ఆధారం_నిడివి, int32_t dest_కొలత,UError_సంకేతం *p_Error_సంకేతం) {
    int32_t      i = 0,j = 0;
    int32_t      countl = 0;
    char16_t *tempbuffer=శూన్యసూచిక;

    tempbuffer = static_cast<char16_t*>(uprv_malloc((ఆధారం_నిడివి + 1) * U_SIZEOF_UCHAR));

    /* Test for nullptr */
    ఒకవేళ(tempbuffer == శూన్యసూచిక) {
        *p_Error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి 0;
    }

        uprv_memset(tempbuffer, 0, (ఆధారం_నిడివి+1)*U_SIZEOF_UCHAR);

        i = 0;
        అయితే(dest[i] == SPACE_CHAR) {
            countl++;
            i++;
        }

        i = j = ఆధారం_నిడివి-1;

        అయితే(i >= 0 && j >= 0) {
            ఒకవేళ( countl>0 && isLamAlefChar(dest[i])) {
                tempbuffer[j] = LAM_CHAR;
                /* to ensure the array index is within the range */
                U_ASSERT(dest[i] >= 0xFEF5u
                    && dest[i]-0xFEF5u < UPRV_LENGTHOF(convertLamAlef));
                tempbuffer[j-1] = convertLamAlef[ dest[i] - 0xFEF5 ];
                j--;
                countl--;
            }లేకపోతే {
                 ఒకవేళ( countl == 0 && isLamAlefChar(dest[i]) ) {
                     *p_Error_సంకేతం=U_NO_SPACE_అందుబాటు;
                     }
                 tempbuffer[j] = dest[i];
            }
            i--;
            j--;
        }
        u_memcpy(dest, tempbuffer, ఆధారం_నిడివి);

        uprv_free(tempbuffer);

        dest_కొలత = ఆధారం_నిడివి;
        తిరిగి dest_కొలత;
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

స్థిర int32_t
expand_Composit_Char_At_చివర(char16_t *dest, int32_t ఆధారం_నిడివి, int32_t dest_కొలత,UError_సంకేతం *p_Error_సంకేతం) {
    int32_t      i = 0,j = 0;

    int32_t      countr = 0;
    int32_t  inpsize = ఆధారం_నిడివి;

    char16_t *tempbuffer=శూన్యసూచిక;
    tempbuffer = static_cast<char16_t*>(uprv_malloc((ఆధారం_నిడివి + 1) * U_SIZEOF_UCHAR));

    /* Test for nullptr */
    ఒకవేళ(tempbuffer == శూన్యసూచిక) {
        *p_Error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
         తిరిగి 0;
    }

    uprv_memset(tempbuffer, 0, (ఆధారం_నిడివి+1)*U_SIZEOF_UCHAR);

    అయితే(inpsize>0 && dest[inpsize-1] == SPACE_CHAR) {
        countr++;
        inpsize--;
    }

    i = ఆధారం_నిడివి - countr - 1;
    j = ఆధారం_నిడివి - 1;

    అయితే(i >= 0 && j >= 0) {
        ఒకవేళ( countr>0 && isLamAlefChar(dest[i]) ) {
            tempbuffer[j] = LAM_CHAR;
            tempbuffer[j-1] = convertLamAlef[ dest[i] - 0xFEF5 ];
            j--;
            countr--;
        }లేకపోతే {
            ఒకవేళ ((countr == 0) && isLamAlefChar(dest[i]) ) {
                *p_Error_సంకేతం=U_NO_SPACE_అందుబాటు;
            }
            tempbuffer[j] = dest[i];
        }
        i--;
        j--;
    }

    ఒకవేళ(countr > 0) {
        u_memmove(tempbuffer, tempbuffer+countr, ఆధారం_నిడివి);
        ఒకవేళ(u_strlen(tempbuffer) < ఆధారం_నిడివి) {
            కొరకు(i=ఆధారం_నిడివి-1;i>=ఆధారం_నిడివి-countr;i--) {
                tempbuffer[i] = SPACE_CHAR;
            }
        }
    }
    u_memcpy(dest, tempbuffer, ఆధారం_నిడివి);

    uprv_free(tempbuffer);

    dest_కొలత = ఆధారం_నిడివి;
    తిరిగి dest_కొలత;
}

/*
 *Name     : expandCompositCharAtNear
 *Function : Expands the LamAlef character into Lam + Alef, YehHamza character
 *           into Yeh + Hamza, SeenFamily character into SeenFamily character
 *           + Tail, while consuming the space next to the character.
 *           If there are no spaces next to the character, an error
 *           will be set to U_NO_SPACE_AVAILABLE as defined in utypes.h
 */

స్థిర int32_t
expandCompositCharAtNear(char16_t *dest, int32_t ఆధారం_నిడివి, int32_t dest_కొలత,UError_సంకేతం *p_Error_సంకేతం,
                         పూర్ణాంకం yehHamzaOption, పూర్ణాంకం చూసిన_తోక_Option, పూర్ణాంకం lamAlefOption, నిర్మాణం uShapeVariables shapeVars) {
    int32_t      i = 0;


    char16_t lamalefChar, yehhamzaChar;

    కొరకు(i = 0 ;i<=ఆధారం_నిడివి-1;i++) {
            ఒకవేళ (చూసిన_తోక_Option && is_చూసిన_తోక_Family_Char(dest[i])) {
                ఒకవేళ ((i>0) && (dest[i-1] == SPACE_CHAR) ) {
                    dest[i-1] = shapeVars.తోక_Char;
                }లేకపోతే {
                    *p_Error_సంకేతం=U_NO_SPACE_అందుబాటు;
                }
            }లేకపోతే ఒకవేళ(yehHamzaOption && (isYehHamzaChar(dest[i])) ) {
                ఒకవేళ ((i>0) && (dest[i-1] == SPACE_CHAR) ) {
                    yehhamzaChar = dest[i];
                    dest[i] = yeh_Hamza_గమ్యం_Yeh[yehhamzaChar - YEH_HAMZAFE_CHAR];
                    dest[i-1] = HAMZAFE_CHAR;
                }లేకపోతే {

                    *p_Error_సంకేతం=U_NO_SPACE_అందుబాటు;
                }
            }లేకపోతే ఒకవేళ(lamAlefOption && isLamAlefChar(dest[i+1])) {
                ఒకవేళ(dest[i] == SPACE_CHAR){
                    lamalefChar = dest[i+1];
                    dest[i+1] = LAM_CHAR;
                    dest[i] = convertLamAlef[ lamalefChar - 0xFEF5 ];
                }లేకపోతే {
                    *p_Error_సంకేతం=U_NO_SPACE_అందుబాటు;
                }
            }
       }
       dest_కొలత = ఆధారం_నిడివి;
       తిరిగి dest_కొలత;
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

స్థిర int32_t
expandCompositChar(char16_t *dest, int32_t ఆధారం_నిడివి,
              int32_t dest_కొలత,uint32_t options,
              UError_సంకేతం *p_Error_సంకేతం, పూర్ణాంకం shaping_బహుళకం,నిర్మాణం uShapeVariables shapeVars) {

    int32_t      i = 0,j = 0;

    char16_t *tempbuffer=శూన్యసూచిక;
    పూర్ణాంకం yehHamzaOption = 0;
    పూర్ణాంకం చూసిన_తోక_Option = 0;
    పూర్ణాంకం lamAlefOption = 0;

    ఒకవేళ (shaping_బహుళకం == 1){
        ఒకవేళ ( (options&U_SHAPE_LAMALEF_ముసుగు) == U_SHAPE_LAMALEF_AUTO){

            ఒకవేళ(shapeVars.spaces_Relative_గమ్యం_పాఠ్యం_Begin_చివర == 0) {
                dest_కొలత = expand_Composit_Char_At_చివర(dest, ఆధారం_నిడివి, dest_కొలత, p_Error_సంకేతం);

                ఒకవేళ(*p_Error_సంకేతం == U_NO_SPACE_అందుబాటు) {
                    *p_Error_సంకేతం = U_ZERO_ERROR;
                    dest_కొలత = expandCompositCharAtBegin(dest, ఆధారం_నిడివి, dest_కొలత, p_Error_సంకేతం);
                }
            }లేకపోతే {
                dest_కొలత = expandCompositCharAtBegin(dest, ఆధారం_నిడివి, dest_కొలత, p_Error_సంకేతం);

                ఒకవేళ(*p_Error_సంకేతం == U_NO_SPACE_అందుబాటు) {
                    *p_Error_సంకేతం = U_ZERO_ERROR;
                    dest_కొలత = expand_Composit_Char_At_చివర(dest, ఆధారం_నిడివి, dest_కొలత, p_Error_సంకేతం);
                }
            }

            ఒకవేళ(*p_Error_సంకేతం == U_NO_SPACE_అందుబాటు) {
                *p_Error_సంకేతం = U_ZERO_ERROR;
                dest_కొలత = expandCompositCharAtNear(dest, ఆధారం_నిడివి, dest_కొలత, p_Error_సంకేతం, yehHamzaOption,
                                                చూసిన_తోక_Option, 1,shapeVars);
            }
        }
    }

    ఒకవేళ (shaping_బహుళకం == 1){
        ఒకవేళ ( (options&U_SHAPE_LAMALEF_ముసుగు) == shapeVars.u_Shape_Lamalef_చివర){
            dest_కొలత = expand_Composit_Char_At_చివర(dest, ఆధారం_నిడివి, dest_కొలత, p_Error_సంకేతం);
        }
    }

    ఒకవేళ (shaping_బహుళకం == 1){
        ఒకవేళ ( (options&U_SHAPE_LAMALEF_ముసుగు) == shapeVars.uShapeLamalefBegin){
            dest_కొలత = expandCompositCharAtBegin(dest, ఆధారం_నిడివి, dest_కొలత, p_Error_సంకేతం);
        }
    }

    ఒకవేళ (shaping_బహుళకం == 0){
         ఒకవేళ ((options&U_SHAPE_YEHHAMZA_ముసుగు) == U_SHAPE_YEHHAMZA_TWOCELL_NEAR){
             yehHamzaOption = 1;
         }
         ఒకవేళ ((options&U_SHAPE_చూసిన_ముసుగు) == U_SHAPE_చూసిన_TWOCELL_NEAR){
            చూసిన_తోక_Option = 1;
         }
    }
    ఒకవేళ (shaping_బహుళకం == 1) {
        ఒకవేళ ( (options&U_SHAPE_LAMALEF_ముసుగు) == U_SHAPE_LAMALEF_NEAR) {
            lamAlefOption = 1;
        }
    }


    ఒకవేళ (yehHamzaOption || చూసిన_తోక_Option || lamAlefOption){
        dest_కొలత = expandCompositCharAtNear(dest, ఆధారం_నిడివి, dest_కొలత, p_Error_సంకేతం, yehHamzaOption,
                                            చూసిన_తోక_Option,lamAlefOption,shapeVars);
    }


    ఒకవేళ (shaping_బహుళకం == 1){
        ఒకవేళ ( (options&U_SHAPE_LAMALEF_ముసుగు) == U_SHAPE_LAMALEF_RESIZE){
            dest_కొలత = calculate_కొలత(dest,ఆధారం_నిడివి,dest_కొలత,options);
            tempbuffer = static_cast<char16_t*>(uprv_malloc((dest_కొలత + 1) * U_SIZEOF_UCHAR));

            /* Test for nullptr */
            ఒకవేళ(tempbuffer == శూన్యసూచిక) {
                *p_Error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                తిరిగి 0;
            }

            uprv_memset(tempbuffer, 0, (dest_కొలత+1)*U_SIZEOF_UCHAR);

            i = j = 0;
            అయితే(i < dest_కొలత && j < dest_కొలత) {
                ఒకవేళ(isLamAlefChar(dest[i]) ) {
                    tempbuffer[j] = convertLamAlef[ dest[i] - 0xFEF5 ];
                    tempbuffer[j+1] = LAM_CHAR;
                    j++;
                }లేకపోతే {
                    tempbuffer[j] = dest[i];
                }
                i++;
                j++;
            }

            u_memcpy(dest, tempbuffer, dest_కొలత);
        }
    }

    ఒకవేళ(tempbuffer) {
        uprv_free(tempbuffer);
    }
    తిరిగి dest_కొలత;
}

/*
 *Name     : shapeUnicode
 *Function : Converts an Arabic Unicode buffer in 06xx Range into a shaped
 *           arabic Unicode buffer in FExx Range
 */
స్థిర int32_t
shapeUnicode(char16_t *dest, int32_t ఆధారం_నిడివి,
             int32_t dest_కొలత,uint32_t options,
             UError_సంకేతం *p_Error_సంకేతం,
             పూర్ణాంకం tashkeelFlag, నిర్మాణం uShapeVariables shapeVars) {

    int32_t          i, iend;
    int32_t          అడుగు;
    int32_t          చివరి_Pos,Nx, Nw;
    గుర్తులేని పూర్ణాంకం     Shape;
    int32_t          lamalef_దొరికింది = 0;
    int32_t seenfam_దొరికింది = 0, yehhamza_దొరికింది =0, tashkeel_దొరికింది  = 0;
    char16_t         prevLink = 0, చివరి_Link = 0, currLink, తదుపరి_Link = 0;
    char16_t         wLamalef;

    /*
     * Converts the input buffer from FExx Range into 06xx Range
     * to make sure that all characters are in the 06xx range
     * even the lamalef is converted to the special region in
     * the 06xx range
     */
    ఒకవేళ ((options & U_SHAPE_PRESERVE_PRESENTATION_ముసుగు)  == U_SHAPE_PRESERVE_PRESENTATION_NOOP) {
        కొరకు (i = 0; i < ఆధారం_నిడివి; i++) {
            char16_t ఆదానం_Char  = dest[i];
            ఒకవేళ ( (ఆదానం_Char >= 0xFB50) && (ఆదానం_Char <= 0xFBFF)) {
                char16_t c = convertFBto06 [ (ఆదానం_Char - 0xFB50) ];
                ఒకవేళ (c != 0)
                    dest[i] = c;
            } లేకపోతే ఒకవేళ ( (ఆదానం_Char >= 0xFE70) && (ఆదానం_Char <= 0xFEFC)) {
                dest[i] = convertFEto06 [ (ఆదానం_Char - 0xFE70) ] ;
            } లేకపోతే {
                dest[i] = ఆదానం_Char ;
            }
        }
    }


    /* sets the index to the end of the buffer, together with the step point to -1 */
    i = ఆధారం_నిడివి - 1;
    iend = -1;
    అడుగు = -1;

    /*
     * This function resolves the link between the characters .
     * Arabic characters have four forms :
     * Isolated Form, Initial Form, Middle Form and Final Form
     */
    currLink = getLink(dest[i]);

    చివరి_Pos = i;
    Nx = -2, Nw = 0;

    అయితే (i != iend) {
        /* If high byte of currLink > 0 then more than one shape */
        ఒకవేళ ((currLink & 0xFF00) > 0 || (getLink(dest[i]) & IRRELEVANT) != 0) {
            Nw = i + అడుగు;
            అయితే (Nx < 0) {         /* we need to know about next char */
                ఒకవేళ(Nw == iend) {
                    తదుపరి_Link = 0;
                    Nx = 3000;
                } లేకపోతే {
                    తదుపరి_Link = getLink(dest[Nw]);
                    ఒకవేళ((తదుపరి_Link & IRRELEVANT) == 0) {
                        Nx = Nw;
                    } లేకపోతే {
                        Nw = Nw + అడుగు;
                    }
                }
            }

            ఒకవేళ ( ((currLink & ALEFTYPE) > 0)  &&  ((చివరి_Link & LAMTYPE) > 0) ) {
                lamalef_దొరికింది = 1;
                wLamalef = changeLamAlef(dest[i]); /*get from 0x065C-0x065f */
                ఒకవేళ ( wLamalef != 0) {
                    dest[i] = LAMALEF_SPACE_SUB;            /* The default case is to drop the Alef and replace */
                    dest[చివరి_Pos] =wLamalef;     /* it by LAMALEF_SPACE_SUB which is the last character in the  */
                    i=చివరి_Pos;                   /* unicode private use area, this is done to make   */
                }                                /* sure that removeLamAlefSpaces() handles only the */
                చివరి_Link = prevLink;             /* spaces generated during lamalef generation.      */
                currLink = getLink(wLamalef);    /* LAMALEF_SPACE_SUB is added here and is replaced by spaces   */
            }                                    /* in removeLamAlefSpaces()                         */

            ఒకవేళ ((i > 0) && (dest[i-1] == SPACE_CHAR)){
                ఒకవేళ ( is_చూసిన_Family_Char(dest[i])) {
                    seenfam_దొరికింది = 1;
                } లేకపోతే ఒకవేళ (dest[i] == YEH_HAMZA_CHAR) {
                    yehhamza_దొరికింది = 1;
                }
            }
            లేకపోతే ఒకవేళ(i==0){
                ఒకవేళ ( is_చూసిన_Family_Char(dest[i])){
                    seenfam_దొరికింది = 1;
                } లేకపోతే ఒకవేళ (dest[i] == YEH_HAMZA_CHAR) {
                    yehhamza_దొరికింది = 1;
                }
            }

            /*
             * get the proper shape according to link ability of neighbors
             * and of character; depends on the order of the shapes
             * (isolated, initial, middle, final) in the compatibility area
             */
            Shape = shape_పట్టిక[తదుపరి_Link & (LINKR + LINKL)]
                              [చివరి_Link & (LINKR + LINKL)]
                              [currLink & (LINKR + LINKL)];

            ఒకవేళ ((currLink & (LINKR+LINKL)) == 1) {
                Shape &= 1;
            } లేకపోతే ఒకవేళ(isTashkeelChar(dest[i])) {
                ఒకవేళ( (చివరి_Link & LINKL) && (తదుపరి_Link & LINKR) && (tashkeelFlag == 1) &&
                     dest[i] != 0x064C && dest[i] != 0x064D )
                {
                    Shape = 1;
                    ఒకవేళ( (తదుపరి_Link&ALEFTYPE) == ALEFTYPE && (చివరి_Link&LAMTYPE) == LAMTYPE ) {
                        Shape = 0;
                    }
                } లేకపోతే ఒకవేళ(tashkeelFlag == 2 && dest[i] == SHADDA06_CHAR){
                    Shape = 1;
                } లేకపోతే {
                    Shape = 0;
                }
            }
            ఒకవేళ ((dest[i] ^ 0x0600) < 0x100) {
                ఒకవేళ ( isTashkeelChar(dest[i]) ){
                    ఒకవేళ (tashkeelFlag == 2  && dest[i] != SHADDA06_CHAR){
                        dest[i] = TASHKEEL_SPACE_SUB;
                        tashkeel_దొరికింది  = 1;
                    } లేకపోతే {
                        /* to ensure the array index is within the range */
                        U_ASSERT(dest[i] >= 0x064Bu
                            && dest[i]-0x064Bu < UPRV_LENGTHOF(IrrelevantPos));
                        dest[i] =  0xFE70 + IrrelevantPos[(dest[i] - 0x064B)] + static_cast<char16_t>(Shape);
                    }
                }లేకపోతే ఒకవేళ ((currLink & APRESENT) > 0) {
                    dest[i] = static_cast<char16_t>(0xFB50 + (currLink >> 8) + Shape);
                }లేకపోతే ఒకవేళ ((currLink >> 8) > 0 && (currLink & IRRELEVANT) == 0) {
                    dest[i] = static_cast<char16_t>(0xFE70 + (currLink >> 8) + Shape);
                }
            }
        }

        /* move one notch forward */
        ఒకవేళ ((currLink & IRRELEVANT) == 0) {
            prevLink = చివరి_Link;
            చివరి_Link = currLink;
            చివరి_Pos = i;
        }

        i = i + అడుగు;
        ఒకవేళ (i == Nx) {
            currLink = తదుపరి_Link;
            Nx = -2;
        } లేకపోతే ఒకవేళ(i != iend) {
            currLink = getLink(dest[i]);
        }
    }
    dest_కొలత = ఆధారం_నిడివి;
    ఒకవేళ ( (lamalef_దొరికింది != 0 ) || (tashkeel_దొరికింది  != 0) ){
        dest_కొలత = handleGeneratedSpaces(dest,ఆధారం_నిడివి,dest_కొలత,options,p_Error_సంకేతం, shapeVars);
    }

    ఒకవేళ ( (seenfam_దొరికింది != 0) || (yehhamza_దొరికింది != 0) ) {
        dest_కొలత = expandCompositChar(dest, ఆధారం_నిడివి,dest_కొలత,options,p_Error_సంకేతం, SHAPE_బహుళకం,shapeVars);
    }
    తిరిగి dest_కొలత;
}

/*
 *Name     : deShapeUnicode
 *Function : Converts an Arabic Unicode buffer in FExx Range into unshaped
 *           arabic Unicode buffer in 06xx Range
 */
స్థిర int32_t
deShapeUnicode(char16_t *dest, int32_t ఆధారం_నిడివి,
               int32_t dest_కొలత,uint32_t options,
               UError_సంకేతం *p_Error_సంకేతం, నిర్మాణం uShapeVariables shapeVars) {
    int32_t i = 0;
    int32_t lamalef_దొరికింది = 0;
    int32_t yehHamzaComposeEnabled = 0;
    int32_t చూసిన_Compose_Enabled = 0;

    yehHamzaComposeEnabled = ((options&U_SHAPE_YEHHAMZA_ముసుగు) == U_SHAPE_YEHHAMZA_TWOCELL_NEAR) ? 1 : 0;
    చూసిన_Compose_Enabled = ((options&U_SHAPE_చూసిన_ముసుగు) == U_SHAPE_చూసిన_TWOCELL_NEAR)? 1 : 0;

    /*
     *This for loop changes the buffer from the Unicode FE range to
     *the Unicode 06 range
     */

    కొరకు(i = 0; i < ఆధారం_నిడివి; i++) {
        char16_t  ఆదానం_Char = dest[i];
        ఒకవేళ ( (ఆదానం_Char >= 0xFB50) && (ఆదానం_Char <= 0xFBFF)) { /* FBxx Arabic range */
            char16_t c = convertFBto06 [ (ఆదానం_Char - 0xFB50) ];
            ఒకవేళ (c != 0)
                dest[i] = c;
        } లేకపోతే ఒకవేళ( (yehHamzaComposeEnabled == 1) && ((ఆదానం_Char == HAMZA06_CHAR) || (ఆదానం_Char == HAMZAFE_CHAR))
                && (i < (ఆధారం_నిడివి - 1)) && isAlefMaksouraChar(dest[i+1] )) {
                 dest[i] = SPACE_CHAR;
                 dest[i+1] = YEH_HAMZA_CHAR;
        } లేకపోతే ఒకవేళ ( (చూసిన_Compose_Enabled == 1) && (is_తోక_Char(ఆదానం_Char)) && (i< (ఆధారం_నిడివి - 1))
                        && (is_చూసిన_తోక_Family_Char(dest[i+1])) ) {
                dest[i] = SPACE_CHAR;
        } లేకపోతే ఒకవేళ (( ఆదానం_Char >= 0xFE70) && (ఆదానం_Char <= 0xFEF4 )) { /* FExx Arabic range */
                dest[i] = convertFEto06 [ (ఆదానం_Char - 0xFE70) ];
        } లేకపోతే {
            dest[i] = ఆదానం_Char ;
        }

        ఒకవేళ( isLamAlefChar(dest[i]) )
            lamalef_దొరికింది = 1;
    }

   dest_కొలత = ఆధారం_నిడివి;
   ఒకవేళ (lamalef_దొరికింది != 0){
          dest_కొలత = expandCompositChar(dest,ఆధారం_నిడివి,dest_కొలత,options,p_Error_సంకేతం,DESHAPE_బహుళకం, shapeVars);
   }
   తిరిగి dest_కొలత;
}

/*
 ****************************************
 * u_shapeArabic
 ****************************************
 */

U_CAPI int32_t U_EXPORT2
u_shapeArabic(స్థిరరాశి char16_t *ఆధారం, int32_t ఆధారం_నిడివి,
              char16_t *dest, int32_t dest_సామర్థ్యం,
              uint32_t options,
              UError_సంకేతం *p_Error_సంకేతం) {

    int32_t dest_నిడివి;
    నిర్మాణం  uShapeVariables shapeVars = { OLD_తోక_CHAR,U_SHAPE_LAMALEF_BEGIN,U_SHAPE_LAMALEF_చివర,U_SHAPE_TASHKEEL_BEGIN,U_SHAPE_TASHKEEL_చివర,0};

    /* usual error checking */
    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }

    /* make sure that no reserved options values are used; allow dest==nullptr only for preflighting */
    ఒకవేళ( ఆధారం==శూన్యసూచిక || ఆధారం_నిడివి<-1 || (dest==శూన్యసూచిక && dest_సామర్థ్యం!=0) || dest_సామర్థ్యం<0 ||
                (((options&U_SHAPE_TASHKEEL_ముసుగు) > 0) &&
                 ((options&U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED) == U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED) ) ||
                (((options&U_SHAPE_TASHKEEL_ముసుగు) > 0) &&
                 ((options&U_SHAPE_LETTERS_ముసుగు) == U_SHAPE_LETTERS_UNSHAPE)) ||
                (options&U_SHAPE_DIGIT_TYPE_RESERVED)==U_SHAPE_DIGIT_TYPE_RESERVED ||
                (options&U_SHAPE_DIGITS_ముసుగు)==U_SHAPE_DIGITS_RESERVED ||
                ((options&U_SHAPE_LAMALEF_ముసుగు) != U_SHAPE_LAMALEF_RESIZE  &&
                (options&U_SHAPE_AGGREGATE_TASHKEEL_ముసుగు) != 0) ||
                ((options&U_SHAPE_AGGREGATE_TASHKEEL_ముసుగు) == U_SHAPE_AGGREGATE_TASHKEEL &&
                (options&U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED) != U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED)
    )
    {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }
    /* Validate  lamalef options */
    ఒకవేళ(((options&U_SHAPE_LAMALEF_ముసుగు) > 0)&&
              !(((options & U_SHAPE_LAMALEF_ముసుగు)==U_SHAPE_LAMALEF_BEGIN) ||
                ((options & U_SHAPE_LAMALEF_ముసుగు)==U_SHAPE_LAMALEF_చివర ) ||
                ((options & U_SHAPE_LAMALEF_ముసుగు)==U_SHAPE_LAMALEF_RESIZE )||
                 ((options & U_SHAPE_LAMALEF_ముసుగు)==U_SHAPE_LAMALEF_AUTO) ||
                 ((options & U_SHAPE_LAMALEF_ముసుగు)==U_SHAPE_LAMALEF_NEAR)))
    {
         *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }
    /* Validate  Tashkeel options */
    ఒకవేళ(((options&U_SHAPE_TASHKEEL_ముసుగు) > 0)&&
                   !(((options & U_SHAPE_TASHKEEL_ముసుగు)==U_SHAPE_TASHKEEL_BEGIN) ||
                     ((options & U_SHAPE_TASHKEEL_ముసుగు)==U_SHAPE_TASHKEEL_చివర )
                    ||((options & U_SHAPE_TASHKEEL_ముసుగు)==U_SHAPE_TASHKEEL_RESIZE )||
                    ((options & U_SHAPE_TASHKEEL_ముసుగు)==U_SHAPE_TASHKEEL_REPLACE_BY_TATWEEL)))
    {
         *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }
    /* determine the source length */
    ఒకవేళ(ఆధారం_నిడివి==-1) {
        ఆధారం_నిడివి=u_strlen(ఆధారం);
    }
    ఒకవేళ(ఆధారం_నిడివి<=0) {
        తిరిగి u_terminateUChars(dest, dest_సామర్థ్యం, 0, p_Error_సంకేతం);
    }

    /* check that source and destination do not overlap */
    ఒకవేళ( dest!=శూన్యసూచిక &&
        ((ఆధారం<=dest && dest<ఆధారం+ఆధారం_నిడివి) ||
         (dest<=ఆధారం && ఆధారం<dest+dest_సామర్థ్యం))) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    /* Does Options contain the new Seen Tail Unicode code point option */
    ఒకవేళ ( (options&U_SHAPE_తోక_TYPE_ముసుగు) == U_SHAPE_తోక_NEW_UNICODE){
        shapeVars.తోక_Char = NEW_తోక_CHAR;
    }లేకపోతే {
        shapeVars.తోక_Char = OLD_తోక_CHAR;
    }

    ఒకవేళ((options&U_SHAPE_LETTERS_ముసుగు)!=U_SHAPE_LETTERS_NOOP) {
        char16_t బఫరు[300];
        char16_t *tempbuffer, *tempsource = శూన్యసూచిక;
        int32_t output_కొలత, spacesCountl=0, spacesCountr=0;

        ఒకవేళ((options&U_SHAPE_AGGREGATE_TASHKEEL_ముసుగు)>0) {
            int32_t logical_క్రమం = (options&U_SHAPE_పాఠ్యం_DIRECTION_ముసుగు) == U_SHAPE_పాఠ్యం_DIRECTION_LOGICAL;
            int32_t aggregate_tashkeel =
                        (options&(U_SHAPE_AGGREGATE_TASHKEEL_ముసుగు+U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED)) ==
                        (U_SHAPE_AGGREGATE_TASHKEEL+U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED);
            పూర్ణాంకం అడుగు=logical_క్రమం?1:-1;
            పూర్ణాంకం j=logical_క్రమం?-1:2*ఆధారం_నిడివి;
            పూర్ణాంకం i=logical_క్రమం?-1:ఆధారం_నిడివి;
            పూర్ణాంకం చివర=logical_క్రమం?ఆధారం_నిడివి:-1;
            పూర్ణాంకం aggregation_possible = 1;
            char16_t prev = 0;
            char16_t prevLink, currLink = 0;
            పూర్ణాంకం new_ఆధారం_నిడివి = 0;
            tempsource = (char16_t *)uprv_malloc(2*ఆధారం_నిడివి*U_SIZEOF_UCHAR);
            ఒకవేళ(tempsource == శూన్యసూచిక) {
                *p_Error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                తిరిగి 0;
            }

            అయితే ((i+=అడుగు) != చివర) {
                prevLink = currLink;
                currLink = getLink(ఆధారం[i]);
                ఒకవేళ (aggregate_tashkeel && ((prevLink|currLink)&COMBINE) == COMBINE && aggregation_possible) {
                    ఒకవేళ (j<0 || j>=2*ఆధారం_నిడివి) {
                        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
                        ఒకవేళ (tempsource != శూన్యసూచిక) uprv_free(tempsource);
                        తిరిగి 0;
                    }
                    aggregation_possible = 0;
                    tempsource[j] = (prev<ఆధారం[i]?prev:ఆధారం[i])-0x064C+0xFC5E;
                    currLink = getLink(tempsource[j]);
                } లేకపోతే {
                    aggregation_possible = 1;
                    tempsource[j+=అడుగు] = ఆధారం[i];
                    prev = ఆధారం[i];
                    new_ఆధారం_నిడివి++;
                }
            }
            ఆధారం = tempsource+(logical_క్రమం?0:j);
            ఆధారం_నిడివి = new_ఆధారం_నిడివి;
        }

        /* calculate destination size */
        /* TODO: do we ever need to do this pure preflighting? */
        ఒకవేళ(((options&U_SHAPE_LAMALEF_ముసుగు)==U_SHAPE_LAMALEF_RESIZE) ||
           ((options&U_SHAPE_TASHKEEL_ముసుగు)==U_SHAPE_TASHKEEL_RESIZE)) {
            output_కొలత=calculate_కొలత(ఆధారం,ఆధారం_నిడివి,dest_సామర్థ్యం,options);
        } లేకపోతే {
            output_కొలత=ఆధారం_నిడివి;
        }

        ఒకవేళ(output_కొలత>dest_సామర్థ్యం) {
            *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                ఒకవేళ (tempsource != శూన్యసూచిక) uprv_free(tempsource);
            తిరిగి output_కొలత;
        }

        /*
         * need a temporary buffer of size max(outputSize, sourceLength)
         * because at first we copy source->temp
         */
        ఒకవేళ(ఆధారం_నిడివి>output_కొలత) {
            output_కొలత=ఆధారం_నిడివి;
        }

        /* Start of Arabic letter shaping part */
        ఒకవేళ(output_కొలత<=UPRV_LENGTHOF(బఫరు)) {
            output_కొలత=UPRV_LENGTHOF(బఫరు);
            tempbuffer=బఫరు;
        } లేకపోతే {
            tempbuffer = (char16_t *)uprv_malloc(output_కొలత*U_SIZEOF_UCHAR);

            /*Test for nullptr*/
            ఒకవేళ(tempbuffer == శూన్యసూచిక) {
                *p_Error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                ఒకవేళ (tempsource != శూన్యసూచిక) uprv_free(tempsource);
                తిరిగి 0;
            }
        }
        u_memcpy(tempbuffer, ఆధారం, ఆధారం_నిడివి);
        ఒకవేళ (tempsource != శూన్యసూచిక){
            uprv_free(tempsource);
        }

        ఒకవేళ(ఆధారం_నిడివి<output_కొలత) {
            uprv_memset(tempbuffer+ఆధారం_నిడివి, 0, (output_కొలత-ఆధారం_నిడివి)*U_SIZEOF_UCHAR);
        }

        ఒకవేళ((options&U_SHAPE_పాఠ్యం_DIRECTION_ముసుగు) == U_SHAPE_పాఠ్యం_DIRECTION_LOGICAL) {
            లెక్క_Spaces(tempbuffer,ఆధారం_నిడివి,options,&spacesCountl,&spacesCountr);
            invert_బఫరు(tempbuffer,ఆధారం_నిడివి,options,spacesCountl,spacesCountr);
        }

        ఒకవేళ((options&U_SHAPE_పాఠ్యం_DIRECTION_ముసుగు) == U_SHAPE_పాఠ్యం_DIRECTION_VISUAL_LTR) {
            ఒకవేళ((options&U_SHAPE_SPACES_RELATIVE_గమ్యం_పాఠ్యం_ముసుగు) == U_SHAPE_SPACES_RELATIVE_గమ్యం_పాఠ్యం_BEGIN_చివర) {
                shapeVars.spaces_Relative_గమ్యం_పాఠ్యం_Begin_చివర = 1;
                shapeVars.uShapeLamalefBegin = U_SHAPE_LAMALEF_చివర;
                shapeVars.u_Shape_Lamalef_చివర = U_SHAPE_LAMALEF_BEGIN;
                shapeVars.uShapeTashkeelBegin = U_SHAPE_TASHKEEL_చివర;
                shapeVars.u_Shape_Tashkeel_చివర = U_SHAPE_TASHKEEL_BEGIN;
            }
        }

        ఎంపిక(options&U_SHAPE_LETTERS_ముసుగు) {
        స్థితి U_SHAPE_LETTERS_SHAPE :
             ఒకవేళ( (options&U_SHAPE_TASHKEEL_ముసుగు)> 0
                 && ((options&U_SHAPE_TASHKEEL_ముసుగు) !=U_SHAPE_TASHKEEL_REPLACE_BY_TATWEEL)) {
                /* Call the shaping function with tashkeel flag == 2 for removal of tashkeel */
                dest_నిడివి = shapeUnicode(tempbuffer,ఆధారం_నిడివి,dest_సామర్థ్యం,options,p_Error_సంకేతం,2,shapeVars);
             }లేకపోతే {
                /* default Call the shaping function with tashkeel flag == 1 */
                dest_నిడివి = shapeUnicode(tempbuffer,ఆధారం_నిడివి,dest_సామర్థ్యం,options,p_Error_సంకేతం,1,shapeVars);

                /*After shaping text check if user wants to remove tashkeel and replace it with tatweel*/
                ఒకవేళ( (options&U_SHAPE_TASHKEEL_ముసుగు) == U_SHAPE_TASHKEEL_REPLACE_BY_TATWEEL){
                  dest_నిడివి = handleTashkeelWithTatweel(tempbuffer,dest_నిడివి,dest_సామర్థ్యం,options,p_Error_సంకేతం);
                }
            }
            ఆపు;
        స్థితి U_SHAPE_LETTERS_SHAPE_TASHKEEL_ISOLATED :
            /* Call the shaping function with tashkeel flag == 0 */
            dest_నిడివి = shapeUnicode(tempbuffer,ఆధారం_నిడివి,dest_సామర్థ్యం,options,p_Error_సంకేతం,0,shapeVars);
            ఆపు;

        స్థితి U_SHAPE_LETTERS_UNSHAPE :
            /* Call the deshaping function */
            dest_నిడివి = deShapeUnicode(tempbuffer,ఆధారం_నిడివి,dest_సామర్థ్యం,options,p_Error_సంకేతం,shapeVars);
            ఆపు;
        అప్రమేయ :
            /* will never occur because of validity checks above */
            dest_నిడివి = 0;
            ఆపు;
        }

        /*
         * TODO: (markus 2002aug01)
         * For as long as we always preflight the outputSize above
         * we should U_ASSERT(outputSize==destLength)
         * except for the adjustment above before the tempbuffer allocation
         */

        ఒకవేళ((options&U_SHAPE_పాఠ్యం_DIRECTION_ముసుగు) == U_SHAPE_పాఠ్యం_DIRECTION_LOGICAL) {
            లెక్క_Spaces(tempbuffer,dest_నిడివి,options,&spacesCountl,&spacesCountr);
            invert_బఫరు(tempbuffer,dest_నిడివి,options,spacesCountl,spacesCountr);
        }
        u_memcpy(dest, tempbuffer, uprv_min(dest_నిడివి, dest_సామర్థ్యం));

        ఒకవేళ(tempbuffer!=బఫరు) {
            uprv_free(tempbuffer);
        }

        ఒకవేళ(dest_నిడివి>dest_సామర్థ్యం) {
            *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
            తిరిగి dest_నిడివి;
        }

        /* End of Arabic letter shaping part */
    } లేకపోతే {
        /*
         * No letter shaping:
         * just make sure the destination is large enough and copy the string.
         */
        ఒకవేళ(dest_సామర్థ్యం<ఆధారం_నిడివి) {
            /* this catches preflighting, too */
            *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
            తిరిగి ఆధారం_నిడివి;
        }
        u_memcpy(dest, ఆధారం, ఆధారం_నిడివి);
        dest_నిడివి=ఆధారం_నిడివి;
    }

    /*
     * Perform number shaping.
     * With UTF-16 or UTF-32, the length of the string is constant.
     * The easiest way to do this is to operate on the destination and
     * "shape" the digits in-place.
     */
    ఒకవేళ((options&U_SHAPE_DIGITS_ముసుగు)!=U_SHAPE_DIGITS_NOOP) {
        char16_t digit_పునాది;
        int32_t i;

        /* select the requested digit group */
        ఎంపిక(options&U_SHAPE_DIGIT_TYPE_ముసుగు) {
        స్థితి U_SHAPE_DIGIT_TYPE_AN:
            digit_పునాది=0x660; /* Unicode: "Arabic-Indic digits" */
            ఆపు;
        స్థితి U_SHAPE_DIGIT_TYPE_AN_EXTENDED:
            digit_పునాది=0x6f0; /* Unicode: "Eastern Arabic-Indic digits (Persian and Urdu)" */
            ఆపు;
        అప్రమేయ:
            /* will never occur because of validity checks above */
            digit_పునాది=0;
            ఆపు;
        }

        /* perform the requested operation */
        ఎంపిక(options&U_SHAPE_DIGITS_ముసుగు) {
        స్థితి U_SHAPE_DIGITS_EN2AN:
            /* add (digitBase-'0') to each European (ASCII) digit code point */
            digit_పునాది-=0x30;
            కొరకు(i=0; i<dest_నిడివి; ++i) {
                ఒకవేళ(((uint32_t)dest[i]-0x30)<10) {
                    dest[i]+=digit_పునాది;
                }
            }
            ఆపు;
        స్థితి U_SHAPE_DIGITS_AN2EN:
            /* subtract (digitBase-'0') from each Arabic digit code point */
            కొరకు(i=0; i<dest_నిడివి; ++i) {
                ఒకవేళ(((uint32_t)dest[i]-(uint32_t)digit_పునాది)<10) {
                    dest[i]-=digit_పునాది-0x30;
                }
            }
            ఆపు;
        స్థితి U_SHAPE_DIGITS_ALEN2AN_INIT_LR:
            shape_గమ్యం_Arabic_Digits_With_Context(dest, dest_నిడివి,
                                            digit_పునాది,
                                            (options & U_SHAPE_పాఠ్యం_DIRECTION_ముసుగు) == U_SHAPE_పాఠ్యం_DIRECTION_LOGICAL,
                                            అబద్ధం);
            ఆపు;
        స్థితి U_SHAPE_DIGITS_ALEN2AN_INIT_AL:
            shape_గమ్యం_Arabic_Digits_With_Context(dest, dest_నిడివి,
                                            digit_పునాది,
                                            (options & U_SHAPE_పాఠ్యం_DIRECTION_ముసుగు) == U_SHAPE_పాఠ్యం_DIRECTION_LOGICAL,
                                            నిజం);
            ఆపు;
        అప్రమేయ:
            /* will never occur because of validity checks above */
            ఆపు;
        }
    }

    తిరిగి u_terminateUChars(dest, dest_సామర్థ్యం, dest_నిడివి, p_Error_సంకేతం);
}
