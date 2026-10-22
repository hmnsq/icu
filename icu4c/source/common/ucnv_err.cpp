// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 *****************************************************************************
 *
 *   Copyright (C) 1998-2016, International Business Machines
 *   Corporation and others.  All Rights Reserved.
 *
 *****************************************************************************
 *
 *  ucnv_err.c
 *  Implements error behaviour functions called by T_UConverter_{from,to}Unicode
 *
 *
*   Change history:
*
*   06/29/2000  helena      Major rewrite of the callback APIs.
*/

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_CONVERSION

#అన్తర్భావ "unicode/ucnv_err.h"
#అన్తర్భావ "unicode/ucnv_cb.h"
#అన్తర్భావ "ucnv_cnv.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "unicode/ucnv.h"
#అన్తర్భావ "ustrfmt.h"

#నిర్వచించు విలువ_STRING_నిడివి 48
/*Magic # 32 = 4(number of char in value string) * 8(max number of bytes per char for any converter) */
#నిర్వచించు UNICODE_PERCENT_SIGN_CODEPOINT  0x0025
#నిర్వచించు UNICODE_U_CODEPOINT             0x0055
#నిర్వచించు UNICODE_X_CODEPOINT             0x0058
#నిర్వచించు UNICODE_RS_CODEPOINT            0x005C
#నిర్వచించు UNICODE_U_దిగువ_CODEPOINT         0x0075
#నిర్వచించు UNICODE_X_దిగువ_CODEPOINT         0x0078
#నిర్వచించు UNICODE_AMP_CODEPOINT           0x0026
#నిర్వచించు UNICODE_HASH_CODEPOINT          0x0023
#నిర్వచించు UNICODE_SEMICOLON_CODEPOINT     0x003B
#నిర్వచించు UNICODE_PLUS_CODEPOINT          0x002B
#నిర్వచించు UNICODE_ఎడమ_CURLY_CODEPOINT    0x007B
#నిర్వచించు UNICODE_కుడి_CURLY_CODEPOINT   0x007D
#నిర్వచించు UNICODE_SPACE_CODEPOINT         0x0020
#నిర్వచించు UCNV_PRV_ESCAPE_ICU         0
#నిర్వచించు UCNV_PRV_ESCAPE_C           'C'
#నిర్వచించు UCNV_PRV_ESCAPE_XML_DEC     'D'
#నిర్వచించు UCNV_PRV_ESCAPE_XML_HEX     'X'
#నిర్వచించు UCNV_PRV_ESCAPE_JAVA        'J'
#నిర్వచించు UCNV_PRV_ESCAPE_UNICODE     'U'
#నిర్వచించు UCNV_PRV_ESCAPE_CSS2        'S'
#నిర్వచించు UCNV_PRV_STOP_ON_ILLEGAL    'i'

/*
 * IS_DEFAULT_IGNORABLE_CODE_POINT
 * This is to check if a code point has the default ignorable unicode property.
 * As such, this list needs to be updated if the ignorable code point list ever
 * changes.
 * To avoid dependency on other code, this list is hard coded here.
 * When an ignorable code point is found and is unmappable, the default callbacks
 * will ignore them.
 * For a list of the default ignorable code points, use this link:
 * https://util.unicode.org/UnicodeJsps/list-unicodeset.jsp?a=%5B%3ADI%3A%5D&abb=on&g=&i=
 *
 * This list should be sync with the one in CharsetCallback.java
 */
#నిర్వచించు IS_DEFAULT_IGNORABLE_సంకేతం_బిందువు(c) ( \
    (c == 0x00AD) || \
    (c == 0x034F) || \
    (c == 0x061C) || \
    (c == 0x115F) || \
    (c == 0x1160) || \
    (0x17B4 <= c && c <= 0x17B5) || \
    (0x180B <= c && c <= 0x180F) || \
    (0x200B <= c && c <= 0x200F) || \
    (0x202A <= c && c <= 0x202E) || \
    (0x2060 <= c && c <= 0x206F) || \
    (c == 0x3164) || \
    (0xFE00 <= c && c <= 0xFE0F) || \
    (c == 0xFEFF) || \
    (c == 0xFFA0) || \
    (0xFFF0 <= c && c <= 0xFFF8) || \
    (0x1BCA0 <= c && c <= 0x1BCA3) || \
    (0x1D173 <= c && c <= 0x1D17A) || \
    (0xE0000 <= c && c <= 0xE0FFF))


/*Function Pointer STOPS at the ILLEGAL_SEQUENCE */
U_CAPI శూన్య    U_EXPORT2
UCNV_FROM_U_CALLBACK_STOP (
                  స్థిరరాశి శూన్య *context,
                  UConverter_From_Unicode_వాదనలు *fromUArgs,
                  స్థిరరాశి char16_t* సంకేతం_Units,
                  int32_t length,
                  UChar32 సంకేతం_బిందువు,
                  UConverterCallbackReason reason,
                  UError_సంకేతం * err)
{
    (శూన్య)context;
    (శూన్య)fromUArgs;
    (శూన్య)సంకేతం_Units;
    (శూన్య)length;
    ఒకవేళ (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_సంకేతం_బిందువు(సంకేతం_బిందువు))
    {
        /*
         * Skip if the codepoint has unicode property of default ignorable.
         */
        *err = U_ZERO_ERROR;
    }
    /* the caller must have set the error code accordingly */
}


/*Function Pointer STOPS at the ILLEGAL_SEQUENCE */
U_CAPI శూన్య    U_EXPORT2
UCNV_గమ్యం_U_CALLBACK_STOP (
                   స్థిరరాశి శూన్య *context,
                   UConverter_గమ్యం_Unicode_వాదనలు *గమ్యం_UArgs,
                   స్థిరరాశి అక్షరం* సంకేతం_Points,
                   int32_t length,
                   UConverterCallbackReason reason,
                   UError_సంకేతం * err)
{
    /* the caller must have set the error code accordingly */
    (శూన్య)context; (శూన్య)గమ్యం_UArgs; (శూన్య)సంకేతం_Points; (శూన్య)length; (శూన్య)reason; (శూన్య)err;
}

U_CAPI శూన్య    U_EXPORT2
UCNV_FROM_U_CALLBACK_SKIP (                  
                  స్థిరరాశి శూన్య *context,
                  UConverter_From_Unicode_వాదనలు *fromUArgs,
                  స్థిరరాశి char16_t* సంకేతం_Units,
                  int32_t length,
                  UChar32 సంకేతం_బిందువు,
                  UConverterCallbackReason reason,
                  UError_సంకేతం * err)
{
    (శూన్య)fromUArgs;
    (శూన్య)సంకేతం_Units;
    (శూన్య)length;
    ఒకవేళ (reason <= UCNV_IRREGULAR)
    {
        ఒకవేళ (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_సంకేతం_బిందువు(సంకేతం_బిందువు))
        {
            /*
             * Skip if the codepoint has unicode property of default ignorable.
             */
            *err = U_ZERO_ERROR;
        }
        లేకపోతే ఒకవేళ (context == శూన్యసూచిక || (*((అక్షరం*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
        {
            *err = U_ZERO_ERROR;
        }
        /* else the caller must have set the error code accordingly. */
    }
    /* else ignore the reset, close and clone calls. */
}

U_CAPI శూన్య    U_EXPORT2
UCNV_FROM_U_CALLBACK_SUBSTITUTE (
                  స్థిరరాశి శూన్య *context,
                  UConverter_From_Unicode_వాదనలు *from_వాదనలు,
                  స్థిరరాశి char16_t* సంకేతం_Units,
                  int32_t length,
                  UChar32 సంకేతం_బిందువు,
                  UConverterCallbackReason reason,
                  UError_సంకేతం * err)
{
    (శూన్య)సంకేతం_Units;
    (శూన్య)length;
    ఒకవేళ (reason <= UCNV_IRREGULAR)
    {
        ఒకవేళ (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_సంకేతం_బిందువు(సంకేతం_బిందువు))
        {
            /*
             * Skip if the codepoint has unicode property of default ignorable.
             */
            *err = U_ZERO_ERROR;
        }
        లేకపోతే ఒకవేళ (context == శూన్యసూచిక || (*((అక్షరం*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
        {
            *err = U_ZERO_ERROR;
            ucnv_cbFromUWriteSub(from_వాదనలు, 0, err);
        }
        /* else the caller must have set the error code accordingly. */
    }
    /* else ignore the reset, close and clone calls. */
}

/*uses uprv_itou to get a unicode escape sequence of the offensive sequence,
 *uses a clean copy (resetted) of the converter, to convert that unicode
 *escape sequence to the target codepage (if conversion failure happens then
 *we revert to substituting with subchar)
 */
U_CAPI శూన్య    U_EXPORT2
UCNV_FROM_U_CALLBACK_ESCAPE (
                         స్థిరరాశి శూన్య *context,
                         UConverter_From_Unicode_వాదనలు *from_వాదనలు,
                         స్థిరరాశి char16_t *సంకేతం_Units,
                         int32_t length,
                         UChar32 సంకేతం_బిందువు,
                         UConverterCallbackReason reason,
                         UError_సంకేతం * err)
{

  char16_t విలువ_String[విలువ_STRING_నిడివి];
  int32_t విలువ_String_నిడివి = 0;
  int32_t i = 0;

  స్థిరరాశి char16_t *my_విలువ_ఆధారం = శూన్యసూచిక;
  UError_సంకేతం err2 = U_ZERO_ERROR;
  UConverterFromUCallback original = శూన్యసూచిక;
  స్థిరరాశి శూన్య *originalContext;

  UConverterFromUCallback ignoredCallback = శూన్యసూచిక;
  స్థిరరాశి శూన్య *ignoredContext;
  
  ఒకవేళ (reason > UCNV_IRREGULAR)
  {
      తిరిగి;
  }
  లేకపోతే ఒకవేళ (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_సంకేతం_బిందువు(సంకేతం_బిందువు))
  {
      /*
       * Skip if the codepoint has unicode property of default ignorable.
       */
      *err = U_ZERO_ERROR;
      తిరిగి;
  }

  ucnv_సమితి_From_UCall_వెనుక (from_వాదనలు->converter,
                     (UConverterFromUCallback) UCNV_FROM_U_CALLBACK_SUBSTITUTE,
                     శూన్యసూచిక,
                     &original,
                     &originalContext,
                     &err2);
  
  ఒకవేళ (U_FAILURE (err2))
  {
    *err = err2;
    తిరిగి;
  } 
  ఒకవేళ(context==శూన్యసూచిక)
  { 
      అయితే (i < length)
      {
        విలువ_String[విలువ_String_నిడివి++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT;  /* adding % */
        విలువ_String[విలువ_String_నిడివి++] = (char16_t) UNICODE_U_CODEPOINT; /* adding U */
        విలువ_String_నిడివి += uprv_itou (విలువ_String + విలువ_String_నిడివి, విలువ_STRING_నిడివి - విలువ_String_నిడివి, (uint16_t)సంకేతం_Units[i++], 16, 4);
      }
  }
  లేకపోతే
  {
      ఎంపిక(*((అక్షరం*)context))
      {
      స్థితి UCNV_PRV_ESCAPE_JAVA:
          అయితే (i < length)
          {
              విలువ_String[విలువ_String_నిడివి++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */
              విలువ_String[విలువ_String_నిడివి++] = (char16_t) UNICODE_U_దిగువ_CODEPOINT; /* adding u */
              విలువ_String_నిడివి += uprv_itou (విలువ_String + విలువ_String_నిడివి, విలువ_STRING_నిడివి - విలువ_String_నిడివి, (uint16_t)సంకేతం_Units[i++], 16, 4);
          }
          ఆపు;

      స్థితి UCNV_PRV_ESCAPE_C:
          విలువ_String[విలువ_String_నిడివి++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */

          ఒకవేళ(length==2){
              విలువ_String[విలువ_String_నిడివి++] = (char16_t) UNICODE_U_CODEPOINT; /* adding U */
              విలువ_String_నిడివి += uprv_itou (విలువ_String + విలువ_String_నిడివి, విలువ_STRING_నిడివి - విలువ_String_నిడివి, సంకేతం_బిందువు, 16, 8);

          }
          లేకపోతే{
              విలువ_String[విలువ_String_నిడివి++] = (char16_t) UNICODE_U_దిగువ_CODEPOINT; /* adding u */
              విలువ_String_నిడివి += uprv_itou (విలువ_String + విలువ_String_నిడివి, విలువ_STRING_నిడివి - విలువ_String_నిడివి, (uint16_t)సంకేతం_Units[0], 16, 4);
          }
          ఆపు;

      స్థితి UCNV_PRV_ESCAPE_XML_DEC:

          విలువ_String[విలువ_String_నిడివి++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
          విలువ_String[విలువ_String_నిడివి++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
          ఒకవేళ(length==2){
              విలువ_String_నిడివి += uprv_itou (విలువ_String + విలువ_String_నిడివి, విలువ_STRING_నిడివి - విలువ_String_నిడివి, సంకేతం_బిందువు, 10, 0);
          }
          లేకపోతే{
              విలువ_String_నిడివి += uprv_itou (విలువ_String + విలువ_String_నిడివి, విలువ_STRING_నిడివి - విలువ_String_నిడివి, (uint16_t)సంకేతం_Units[0], 10, 0);
          }
          విలువ_String[విలువ_String_నిడివి++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
          ఆపు;

      స్థితి UCNV_PRV_ESCAPE_XML_HEX:

          విలువ_String[విలువ_String_నిడివి++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
          విలువ_String[విలువ_String_నిడివి++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
          విలువ_String[విలువ_String_నిడివి++] = (char16_t) UNICODE_X_దిగువ_CODEPOINT; /* adding x */
          ఒకవేళ(length==2){
              విలువ_String_నిడివి += uprv_itou (విలువ_String + విలువ_String_నిడివి, విలువ_STRING_నిడివి - విలువ_String_నిడివి, సంకేతం_బిందువు, 16, 0);
          }
          లేకపోతే{
              విలువ_String_నిడివి += uprv_itou (విలువ_String + విలువ_String_నిడివి, విలువ_STRING_నిడివి - విలువ_String_నిడివి, (uint16_t)సంకేతం_Units[0], 16, 0);
          }
          విలువ_String[విలువ_String_నిడివి++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
          ఆపు;

      స్థితి UCNV_PRV_ESCAPE_UNICODE:
          విలువ_String[విలువ_String_నిడివి++] = (char16_t) UNICODE_ఎడమ_CURLY_CODEPOINT;    /* adding { */
          విలువ_String[విలువ_String_నిడివి++] = (char16_t) UNICODE_U_CODEPOINT;    /* adding U */
          విలువ_String[విలువ_String_నిడివి++] = (char16_t) UNICODE_PLUS_CODEPOINT; /* adding + */
          ఒకవేళ (length == 2) {
              విలువ_String_నిడివి += uprv_itou (విలువ_String + విలువ_String_నిడివి, విలువ_STRING_నిడివి - విలువ_String_నిడివి, సంకేతం_బిందువు, 16, 4);
          } లేకపోతే {
              విలువ_String_నిడివి += uprv_itou (విలువ_String + విలువ_String_నిడివి, విలువ_STRING_నిడివి - విలువ_String_నిడివి, (uint16_t)సంకేతం_Units[0], 16, 4);
          }
          విలువ_String[విలువ_String_నిడివి++] = (char16_t) UNICODE_కుడి_CURLY_CODEPOINT;    /* adding } */
          ఆపు;

      స్థితి UCNV_PRV_ESCAPE_CSS2:
          విలువ_String[విలువ_String_నిడివి++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */
          విలువ_String_నిడివి += uprv_itou (విలువ_String + విలువ_String_నిడివి, విలువ_STRING_నిడివి - విలువ_String_నిడివి, సంకేతం_బిందువు, 16, 0);
          /* Always add space character, because the next character might be whitespace,
             which would erroneously be considered the termination of the escape sequence. */
          విలువ_String[విలువ_String_నిడివి++] = (char16_t) UNICODE_SPACE_CODEPOINT;
          ఆపు;

      అప్రమేయ:
          అయితే (i < length)
          {
              విలువ_String[విలువ_String_నిడివి++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT;  /* adding % */
              విలువ_String[విలువ_String_నిడివి++] = (char16_t) UNICODE_U_CODEPOINT;             /* adding U */
              విలువ_String_నిడివి += uprv_itou (విలువ_String + విలువ_String_నిడివి, విలువ_STRING_నిడివి - విలువ_String_నిడివి, (uint16_t)సంకేతం_Units[i++], 16, 4);
          }
      }
  }  
  my_విలువ_ఆధారం = విలువ_String;

  /* reset the error */
  *err = U_ZERO_ERROR;

  ucnv_cbFromUWriteUChars(from_వాదనలు, &my_విలువ_ఆధారం, my_విలువ_ఆధారం+విలువ_String_నిడివి, 0, err);

  ucnv_సమితి_From_UCall_వెనుక (from_వాదనలు->converter,
                         original,
                         originalContext,
                         &ignoredCallback,
                         &ignoredContext,
                         &err2);
  ఒకవేళ (U_FAILURE (err2))
  {
      *err = err2;
      తిరిగి;
  }
}



U_CAPI శూన్య  U_EXPORT2
UCNV_గమ్యం_U_CALLBACK_SKIP (
                 స్థిరరాశి శూన్య *context,
                 UConverter_గమ్యం_Unicode_వాదనలు *గమ్యం_వాదనలు,
                 స్థిరరాశి అక్షరం* సంకేతం_Units,
                 int32_t length,
                 UConverterCallbackReason reason,
                 UError_సంకేతం * err)
{
    (శూన్య)గమ్యం_వాదనలు;
    (శూన్య)సంకేతం_Units;
    (శూన్య)length;
    ఒకవేళ (reason <= UCNV_IRREGULAR)
    {
        ఒకవేళ (context == శూన్యసూచిక || (*((అక్షరం*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
        {
            *err = U_ZERO_ERROR;
        }
        /* else the caller must have set the error code accordingly. */
    }
    /* else ignore the reset, close and clone calls. */
}

U_CAPI శూన్య    U_EXPORT2
UCNV_గమ్యం_U_CALLBACK_SUBSTITUTE (
                 స్థిరరాశి శూన్య *context,
                 UConverter_గమ్యం_Unicode_వాదనలు *గమ్యం_వాదనలు,
                 స్థిరరాశి అక్షరం* సంకేతం_Units,
                 int32_t length,
                 UConverterCallbackReason reason,
                 UError_సంకేతం * err)
{
    (శూన్య)సంకేతం_Units;
    (శూన్య)length;
    ఒకవేళ (reason <= UCNV_IRREGULAR)
    {
        ఒకవేళ (context == శూన్యసూచిక || (*((అక్షరం*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
        {
            *err = U_ZERO_ERROR;
            ucnv_cb_గమ్యం_UWrite_Sub(గమ్యం_వాదనలు,0,err);
        }
        /* else the caller must have set the error code accordingly. */
    }
    /* else ignore the reset, close and clone calls. */
}

/*uses uprv_itou to get a unicode escape sequence of the offensive sequence,
 *and uses that as the substitution sequence
 */
U_CAPI శూన్య   U_EXPORT2
UCNV_గమ్యం_U_CALLBACK_ESCAPE (
                 స్థిరరాశి శూన్య *context,
                 UConverter_గమ్యం_Unicode_వాదనలు *గమ్యం_వాదనలు,
                 స్థిరరాశి అక్షరం* సంకేతం_Units,
                 int32_t length,
                 UConverterCallbackReason reason,
                 UError_సంకేతం * err)
{
    char16_t uni_విలువ_String[విలువ_STRING_నిడివి];
    int32_t విలువ_String_నిడివి = 0;
    int32_t i = 0;

    ఒకవేళ (reason > UCNV_IRREGULAR)
    {
        తిరిగి;
    }

    ఒకవేళ(context==శూన్యసూచిక)
    {    
        అయితే (i < length)
        {
            uni_విలువ_String[విలువ_String_నిడివి++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT; /* adding % */
            uni_విలువ_String[విలువ_String_నిడివి++] = (char16_t) UNICODE_X_CODEPOINT;    /* adding X */
            విలువ_String_నిడివి += uprv_itou (uni_విలువ_String + విలువ_String_నిడివి, విలువ_STRING_నిడివి - విలువ_String_నిడివి, (uint8_t) సంకేతం_Units[i++], 16, 2);
        }
    }
    లేకపోతే
    {
        ఎంపిక(*((అక్షరం*)context))
        {
        స్థితి UCNV_PRV_ESCAPE_XML_DEC:
            అయితే (i < length)
            {
                uni_విలువ_String[విలువ_String_నిడివి++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
                uni_విలువ_String[విలువ_String_నిడివి++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
                విలువ_String_నిడివి += uprv_itou (uni_విలువ_String + విలువ_String_నిడివి, విలువ_STRING_నిడివి - విలువ_String_నిడివి, (uint8_t)సంకేతం_Units[i++], 10, 0);
                uni_విలువ_String[విలువ_String_నిడివి++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
            }
            ఆపు;

        స్థితి UCNV_PRV_ESCAPE_XML_HEX:
            అయితే (i < length)
            {
                uni_విలువ_String[విలువ_String_నిడివి++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
                uni_విలువ_String[విలువ_String_నిడివి++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
                uni_విలువ_String[విలువ_String_నిడివి++] = (char16_t) UNICODE_X_దిగువ_CODEPOINT; /* adding x */
                విలువ_String_నిడివి += uprv_itou (uni_విలువ_String + విలువ_String_నిడివి, విలువ_STRING_నిడివి - విలువ_String_నిడివి, (uint8_t)సంకేతం_Units[i++], 16, 0);
                uni_విలువ_String[విలువ_String_నిడివి++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
            }
            ఆపు;
        స్థితి UCNV_PRV_ESCAPE_C:
            అయితే (i < length)
            {
                uni_విలువ_String[విలువ_String_నిడివి++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */
                uni_విలువ_String[విలువ_String_నిడివి++] = (char16_t) UNICODE_X_దిగువ_CODEPOINT; /* adding x */
                విలువ_String_నిడివి += uprv_itou (uni_విలువ_String + విలువ_String_నిడివి, విలువ_STRING_నిడివి - విలువ_String_నిడివి, (uint8_t)సంకేతం_Units[i++], 16, 2);
            }
            ఆపు;
        అప్రమేయ:
            అయితే (i < length)
            {
                uni_విలువ_String[విలువ_String_నిడివి++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT; /* adding % */
                uni_విలువ_String[విలువ_String_నిడివి++] = (char16_t) UNICODE_X_CODEPOINT;    /* adding X */
                uprv_itou (uni_విలువ_String + విలువ_String_నిడివి, విలువ_STRING_నిడివి - విలువ_String_నిడివి, (uint8_t) సంకేతం_Units[i++], 16, 2);
                విలువ_String_నిడివి += 2;
            }
        }
    }
    /* reset the error */
    *err = U_ZERO_ERROR;

    ucnv_cb_గమ్యం_UWrite_UChars(గమ్యం_వాదనలు, uni_విలువ_String, విలువ_String_నిడివి, 0, err);
}

#అయితే_ముగింపు
