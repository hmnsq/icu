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

#અન્તર્ભાવ "unicode/utypes.h"

#જો !UCONFIG_NO_CONVERSION

#અન્તર્ભાવ "unicode/ucnv_err.h"
#અન્તર્ભાવ "unicode/ucnv_cb.h"
#અન્તર્ભાવ "ucnv_cnv.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "unicode/ucnv.h"
#અન્તર્ભાવ "ustrfmt.h"

#define કિંમત_STRING_LENGTH 48
/*Magic # 32 = 4(number of char in value string) * 8(max number of bytes per char for any converter) */
#define UNICODE_PERCENT_SIGN_CODEPOINT  0x0025
#define UNICODE_U_CODEPOINT             0x0055
#define UNICODE_X_CODEPOINT             0x0058
#define UNICODE_RS_CODEPOINT            0x005C
#define UNICODE_U_નીચલું_CODEPOINT         0x0075
#define UNICODE_X_નીચલું_CODEPOINT         0x0078
#define UNICODE_AMP_CODEPOINT           0x0026
#define UNICODE_HASH_CODEPOINT          0x0023
#define UNICODE_SEMICOLON_CODEPOINT     0x003B
#define UNICODE_PLUS_CODEPOINT          0x002B
#define UNICODE_ડાબું_CURLY_CODEPOINT    0x007B
#define UNICODE_જમણું_CURLY_CODEPOINT   0x007D
#define UNICODE_SPACE_CODEPOINT         0x0020
#define UCNV_PRV_ESCAPE_ICU         0
#define UCNV_PRV_ESCAPE_C           'C'
#define UCNV_PRV_ESCAPE_XML_DEC     'D'
#define UCNV_PRV_ESCAPE_XML_HEX     'X'
#define UCNV_PRV_ESCAPE_JAVA        'J'
#define UCNV_PRV_ESCAPE_UNICODE     'U'
#define UCNV_PRV_ESCAPE_CSS2        'S'
#define UCNV_PRV_STOP_ON_ILLEGAL    'i'

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
#define IS_DEFAULT_IGNORABLE_CODE_POINT(c) ( \
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
U_CAPI શૂન્ય    U_EXPORT2
UCNV_FROM_U_CALLBACK_STOP (
                  અચલ શૂન્ય *context,
                  UConverterFromUnicodeArgs *fromUArgs,
                  અચલ char16_t* codeUnits,
                  int32_t length,
                  UChar32 codePoint,
                  UConverterCallbackReason reason,
                  UErrorCode * err)
{
    (શૂન્ય)context;
    (શૂન્ય)fromUArgs;
    (શૂન્ય)codeUnits;
    (શૂન્ય)length;
    જો (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_CODE_POINT(codePoint))
    {
        /*
         * Skip if the codepoint has unicode property of default ignorable.
         */
        *err = U_ZERO_ERROR;
    }
    /* the caller must have set the error code accordingly */
}


/*Function Pointer STOPS at the ILLEGAL_SEQUENCE */
U_CAPI શૂન્ય    U_EXPORT2
UCNV_TO_U_CALLBACK_STOP (
                   અચલ શૂન્ય *context,
                   UConverterToUnicodeArgs *toUArgs,
                   અચલ અક્ષર* codePoints,
                   int32_t length,
                   UConverterCallbackReason reason,
                   UErrorCode * err)
{
    /* the caller must have set the error code accordingly */
    (શૂન્ય)context; (શૂન્ય)toUArgs; (શૂન્ય)codePoints; (શૂન્ય)length; (શૂન્ય)reason; (શૂન્ય)err;
}

U_CAPI શૂન્ય    U_EXPORT2
UCNV_FROM_U_CALLBACK_SKIP (                  
                  અચલ શૂન્ય *context,
                  UConverterFromUnicodeArgs *fromUArgs,
                  અચલ char16_t* codeUnits,
                  int32_t length,
                  UChar32 codePoint,
                  UConverterCallbackReason reason,
                  UErrorCode * err)
{
    (શૂન્ય)fromUArgs;
    (શૂન્ય)codeUnits;
    (શૂન્ય)length;
    જો (reason <= UCNV_IRREGULAR)
    {
        જો (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_CODE_POINT(codePoint))
        {
            /*
             * Skip if the codepoint has unicode property of default ignorable.
             */
            *err = U_ZERO_ERROR;
        }
        નહીંતર જો (context == nullptr || (*((અક્ષર*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
        {
            *err = U_ZERO_ERROR;
        }
        /* else the caller must have set the error code accordingly. */
    }
    /* else ignore the reset, close and clone calls. */
}

U_CAPI શૂન્ય    U_EXPORT2
UCNV_FROM_U_CALLBACK_SUBSTITUTE (
                  અચલ શૂન્ય *context,
                  UConverterFromUnicodeArgs *fromArgs,
                  અચલ char16_t* codeUnits,
                  int32_t length,
                  UChar32 codePoint,
                  UConverterCallbackReason reason,
                  UErrorCode * err)
{
    (શૂન્ય)codeUnits;
    (શૂન્ય)length;
    જો (reason <= UCNV_IRREGULAR)
    {
        જો (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_CODE_POINT(codePoint))
        {
            /*
             * Skip if the codepoint has unicode property of default ignorable.
             */
            *err = U_ZERO_ERROR;
        }
        નહીંતર જો (context == nullptr || (*((અક્ષર*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
        {
            *err = U_ZERO_ERROR;
            ucnv_cbFromUWriteSub(fromArgs, 0, err);
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
U_CAPI શૂન્ય    U_EXPORT2
UCNV_FROM_U_CALLBACK_ESCAPE (
                         અચલ શૂન્ય *context,
                         UConverterFromUnicodeArgs *fromArgs,
                         અચલ char16_t *codeUnits,
                         int32_t length,
                         UChar32 codePoint,
                         UConverterCallbackReason reason,
                         UErrorCode * err)
{

  char16_t કિંમત_String[કિંમત_STRING_LENGTH];
  int32_t કિંમત_String_Length = 0;
  int32_t i = 0;

  અચલ char16_t *my_કિંમત_Source = nullptr;
  UErrorCode err2 = U_ZERO_ERROR;
  UConverterFromUCallback original = nullptr;
  અચલ શૂન્ય *originalContext;

  UConverterFromUCallback ignoredCallback = nullptr;
  અચલ શૂન્ય *ignoredContext;
  
  જો (reason > UCNV_IRREGULAR)
  {
      પાછા;
  }
  નહીંતર જો (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_CODE_POINT(codePoint))
  {
      /*
       * Skip if the codepoint has unicode property of default ignorable.
       */
      *err = U_ZERO_ERROR;
      પાછા;
  }

  ucnv_setFromUCallBack (fromArgs->converter,
                     (UConverterFromUCallback) UCNV_FROM_U_CALLBACK_SUBSTITUTE,
                     nullptr,
                     &original,
                     &originalContext,
                     &err2);
  
  જો (U_FAILURE (err2))
  {
    *err = err2;
    પાછા;
  } 
  જો(context==nullptr)
  { 
      જ્યારે (i < length)
      {
        કિંમત_String[કિંમત_String_Length++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT;  /* adding % */
        કિંમત_String[કિંમત_String_Length++] = (char16_t) UNICODE_U_CODEPOINT; /* adding U */
        કિંમત_String_Length += uprv_itou (કિંમત_String + કિંમત_String_Length, કિંમત_STRING_LENGTH - કિંમત_String_Length, (uint16_t)codeUnits[i++], 16, 4);
      }
  }
  નહીંતર
  {
      પસંદગી(*((અક્ષર*)context))
      {
      સ્થિતિ UCNV_PRV_ESCAPE_JAVA:
          જ્યારે (i < length)
          {
              કિંમત_String[કિંમત_String_Length++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */
              કિંમત_String[કિંમત_String_Length++] = (char16_t) UNICODE_U_નીચલું_CODEPOINT; /* adding u */
              કિંમત_String_Length += uprv_itou (કિંમત_String + કિંમત_String_Length, કિંમત_STRING_LENGTH - કિંમત_String_Length, (uint16_t)codeUnits[i++], 16, 4);
          }
          તોડો;

      સ્થિતિ UCNV_PRV_ESCAPE_C:
          કિંમત_String[કિંમત_String_Length++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */

          જો(length==2){
              કિંમત_String[કિંમત_String_Length++] = (char16_t) UNICODE_U_CODEPOINT; /* adding U */
              કિંમત_String_Length += uprv_itou (કિંમત_String + કિંમત_String_Length, કિંમત_STRING_LENGTH - કિંમત_String_Length, codePoint, 16, 8);

          }
          નહીંતર{
              કિંમત_String[કિંમત_String_Length++] = (char16_t) UNICODE_U_નીચલું_CODEPOINT; /* adding u */
              કિંમત_String_Length += uprv_itou (કિંમત_String + કિંમત_String_Length, કિંમત_STRING_LENGTH - કિંમત_String_Length, (uint16_t)codeUnits[0], 16, 4);
          }
          તોડો;

      સ્થિતિ UCNV_PRV_ESCAPE_XML_DEC:

          કિંમત_String[કિંમત_String_Length++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
          કિંમત_String[કિંમત_String_Length++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
          જો(length==2){
              કિંમત_String_Length += uprv_itou (કિંમત_String + કિંમત_String_Length, કિંમત_STRING_LENGTH - કિંમત_String_Length, codePoint, 10, 0);
          }
          નહીંતર{
              કિંમત_String_Length += uprv_itou (કિંમત_String + કિંમત_String_Length, કિંમત_STRING_LENGTH - કિંમત_String_Length, (uint16_t)codeUnits[0], 10, 0);
          }
          કિંમત_String[કિંમત_String_Length++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
          તોડો;

      સ્થિતિ UCNV_PRV_ESCAPE_XML_HEX:

          કિંમત_String[કિંમત_String_Length++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
          કિંમત_String[કિંમત_String_Length++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
          કિંમત_String[કિંમત_String_Length++] = (char16_t) UNICODE_X_નીચલું_CODEPOINT; /* adding x */
          જો(length==2){
              કિંમત_String_Length += uprv_itou (કિંમત_String + કિંમત_String_Length, કિંમત_STRING_LENGTH - કિંમત_String_Length, codePoint, 16, 0);
          }
          નહીંતર{
              કિંમત_String_Length += uprv_itou (કિંમત_String + કિંમત_String_Length, કિંમત_STRING_LENGTH - કિંમત_String_Length, (uint16_t)codeUnits[0], 16, 0);
          }
          કિંમત_String[કિંમત_String_Length++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
          તોડો;

      સ્થિતિ UCNV_PRV_ESCAPE_UNICODE:
          કિંમત_String[કિંમત_String_Length++] = (char16_t) UNICODE_ડાબું_CURLY_CODEPOINT;    /* adding { */
          કિંમત_String[કિંમત_String_Length++] = (char16_t) UNICODE_U_CODEPOINT;    /* adding U */
          કિંમત_String[કિંમત_String_Length++] = (char16_t) UNICODE_PLUS_CODEPOINT; /* adding + */
          જો (length == 2) {
              કિંમત_String_Length += uprv_itou (કિંમત_String + કિંમત_String_Length, કિંમત_STRING_LENGTH - કિંમત_String_Length, codePoint, 16, 4);
          } નહીંતર {
              કિંમત_String_Length += uprv_itou (કિંમત_String + કિંમત_String_Length, કિંમત_STRING_LENGTH - કિંમત_String_Length, (uint16_t)codeUnits[0], 16, 4);
          }
          કિંમત_String[કિંમત_String_Length++] = (char16_t) UNICODE_જમણું_CURLY_CODEPOINT;    /* adding } */
          તોડો;

      સ્થિતિ UCNV_PRV_ESCAPE_CSS2:
          કિંમત_String[કિંમત_String_Length++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */
          કિંમત_String_Length += uprv_itou (કિંમત_String + કિંમત_String_Length, કિંમત_STRING_LENGTH - કિંમત_String_Length, codePoint, 16, 0);
          /* Always add space character, because the next character might be whitespace,
             which would erroneously be considered the termination of the escape sequence. */
          કિંમત_String[કિંમત_String_Length++] = (char16_t) UNICODE_SPACE_CODEPOINT;
          તોડો;

      મૂળભૂત:
          જ્યારે (i < length)
          {
              કિંમત_String[કિંમત_String_Length++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT;  /* adding % */
              કિંમત_String[કિંમત_String_Length++] = (char16_t) UNICODE_U_CODEPOINT;             /* adding U */
              કિંમત_String_Length += uprv_itou (કિંમત_String + કિંમત_String_Length, કિંમત_STRING_LENGTH - કિંમત_String_Length, (uint16_t)codeUnits[i++], 16, 4);
          }
      }
  }  
  my_કિંમત_Source = કિંમત_String;

  /* reset the error */
  *err = U_ZERO_ERROR;

  ucnv_cbFromUWriteUChars(fromArgs, &my_કિંમત_Source, my_કિંમત_Source+કિંમત_String_Length, 0, err);

  ucnv_setFromUCallBack (fromArgs->converter,
                         original,
                         originalContext,
                         &ignoredCallback,
                         &ignoredContext,
                         &err2);
  જો (U_FAILURE (err2))
  {
      *err = err2;
      પાછા;
  }
}



U_CAPI શૂન્ય  U_EXPORT2
UCNV_TO_U_CALLBACK_SKIP (
                 અચલ શૂન્ય *context,
                 UConverterToUnicodeArgs *toArgs,
                 અચલ અક્ષર* codeUnits,
                 int32_t length,
                 UConverterCallbackReason reason,
                 UErrorCode * err)
{
    (શૂન્ય)toArgs;
    (શૂન્ય)codeUnits;
    (શૂન્ય)length;
    જો (reason <= UCNV_IRREGULAR)
    {
        જો (context == nullptr || (*((અક્ષર*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
        {
            *err = U_ZERO_ERROR;
        }
        /* else the caller must have set the error code accordingly. */
    }
    /* else ignore the reset, close and clone calls. */
}

U_CAPI શૂન્ય    U_EXPORT2
UCNV_TO_U_CALLBACK_SUBSTITUTE (
                 અચલ શૂન્ય *context,
                 UConverterToUnicodeArgs *toArgs,
                 અચલ અક્ષર* codeUnits,
                 int32_t length,
                 UConverterCallbackReason reason,
                 UErrorCode * err)
{
    (શૂન્ય)codeUnits;
    (શૂન્ય)length;
    જો (reason <= UCNV_IRREGULAR)
    {
        જો (context == nullptr || (*((અક્ષર*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
        {
            *err = U_ZERO_ERROR;
            ucnv_cbToUWriteSub(toArgs,0,err);
        }
        /* else the caller must have set the error code accordingly. */
    }
    /* else ignore the reset, close and clone calls. */
}

/*uses uprv_itou to get a unicode escape sequence of the offensive sequence,
 *and uses that as the substitution sequence
 */
U_CAPI શૂન્ય   U_EXPORT2
UCNV_TO_U_CALLBACK_ESCAPE (
                 અચલ શૂન્ય *context,
                 UConverterToUnicodeArgs *toArgs,
                 અચલ અક્ષર* codeUnits,
                 int32_t length,
                 UConverterCallbackReason reason,
                 UErrorCode * err)
{
    char16_t uni_કિંમત_String[કિંમત_STRING_LENGTH];
    int32_t કિંમત_String_Length = 0;
    int32_t i = 0;

    જો (reason > UCNV_IRREGULAR)
    {
        પાછા;
    }

    જો(context==nullptr)
    {    
        જ્યારે (i < length)
        {
            uni_કિંમત_String[કિંમત_String_Length++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT; /* adding % */
            uni_કિંમત_String[કિંમત_String_Length++] = (char16_t) UNICODE_X_CODEPOINT;    /* adding X */
            કિંમત_String_Length += uprv_itou (uni_કિંમત_String + કિંમત_String_Length, કિંમત_STRING_LENGTH - કિંમત_String_Length, (uint8_t) codeUnits[i++], 16, 2);
        }
    }
    નહીંતર
    {
        પસંદગી(*((અક્ષર*)context))
        {
        સ્થિતિ UCNV_PRV_ESCAPE_XML_DEC:
            જ્યારે (i < length)
            {
                uni_કિંમત_String[કિંમત_String_Length++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
                uni_કિંમત_String[કિંમત_String_Length++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
                કિંમત_String_Length += uprv_itou (uni_કિંમત_String + કિંમત_String_Length, કિંમત_STRING_LENGTH - કિંમત_String_Length, (uint8_t)codeUnits[i++], 10, 0);
                uni_કિંમત_String[કિંમત_String_Length++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
            }
            તોડો;

        સ્થિતિ UCNV_PRV_ESCAPE_XML_HEX:
            જ્યારે (i < length)
            {
                uni_કિંમત_String[કિંમત_String_Length++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
                uni_કિંમત_String[કિંમત_String_Length++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
                uni_કિંમત_String[કિંમત_String_Length++] = (char16_t) UNICODE_X_નીચલું_CODEPOINT; /* adding x */
                કિંમત_String_Length += uprv_itou (uni_કિંમત_String + કિંમત_String_Length, કિંમત_STRING_LENGTH - કિંમત_String_Length, (uint8_t)codeUnits[i++], 16, 0);
                uni_કિંમત_String[કિંમત_String_Length++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
            }
            તોડો;
        સ્થિતિ UCNV_PRV_ESCAPE_C:
            જ્યારે (i < length)
            {
                uni_કિંમત_String[કિંમત_String_Length++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */
                uni_કિંમત_String[કિંમત_String_Length++] = (char16_t) UNICODE_X_નીચલું_CODEPOINT; /* adding x */
                કિંમત_String_Length += uprv_itou (uni_કિંમત_String + કિંમત_String_Length, કિંમત_STRING_LENGTH - કિંમત_String_Length, (uint8_t)codeUnits[i++], 16, 2);
            }
            તોડો;
        મૂળભૂત:
            જ્યારે (i < length)
            {
                uni_કિંમત_String[કિંમત_String_Length++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT; /* adding % */
                uni_કિંમત_String[કિંમત_String_Length++] = (char16_t) UNICODE_X_CODEPOINT;    /* adding X */
                uprv_itou (uni_કિંમત_String + કિંમત_String_Length, કિંમત_STRING_LENGTH - કિંમત_String_Length, (uint8_t) codeUnits[i++], 16, 2);
                કિંમત_String_Length += 2;
            }
        }
    }
    /* reset the error */
    *err = U_ZERO_ERROR;

    ucnv_cbToUWriteUChars(toArgs, uni_કિંમત_String, કિંમત_String_Length, 0, err);
}

#endif
