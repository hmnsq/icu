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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಯದಿ !UCONFIG_NO_CONVERSION

#ಅನ್ತರ್ಭಾವ "unicode/ucnv_err.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucnv_cb.h"
#ಅನ್ತರ್ಭಾವ "ucnv_cnv.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucnv.h"
#ಅನ್ತರ್ಭಾವ "ustrfmt.h"

#define ಮೌಲ್ಯ_STRING_LENGTH 48
/*Magic # 32 = 4(number of char in value string) * 8(max number of bytes per char for any converter) */
#define UNICODE_PERCENT_SIGN_CODEPOINT  0x0025
#define UNICODE_U_CODEPOINT             0x0055
#define UNICODE_X_CODEPOINT             0x0058
#define UNICODE_RS_CODEPOINT            0x005C
#define UNICODE_U_ಕೆಳಗಿನ_CODEPOINT         0x0075
#define UNICODE_X_ಕೆಳಗಿನ_CODEPOINT         0x0078
#define UNICODE_AMP_CODEPOINT           0x0026
#define UNICODE_HASH_CODEPOINT          0x0023
#define UNICODE_SEMICOLON_CODEPOINT     0x003B
#define UNICODE_PLUS_CODEPOINT          0x002B
#define UNICODE_ಎಡ_CURLY_CODEPOINT    0x007B
#define UNICODE_ಬಲ_CURLY_CODEPOINT   0x007D
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
U_CAPI ಶೂನ್ಯ    U_EXPORT2
UCNV_FROM_U_CALLBACK_STOP (
                  ಧ್ರುವ ಶೂನ್ಯ *context,
                  UConverterFromUnicodeArgs *fromUArgs,
                  ಧ್ರುವ char16_t* codeUnits,
                  int32_t length,
                  UChar32 codePoint,
                  UConverterCallbackReason reason,
                  UErrorCode * err)
{
    (ಶೂನ್ಯ)context;
    (ಶೂನ್ಯ)fromUArgs;
    (ಶೂನ್ಯ)codeUnits;
    (ಶೂನ್ಯ)length;
    ಯದಿ (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_CODE_POINT(codePoint))
    {
        /*
         * Skip if the codepoint has unicode property of default ignorable.
         */
        *err = U_ZERO_ERROR;
    }
    /* the caller must have set the error code accordingly */
}


/*Function Pointer STOPS at the ILLEGAL_SEQUENCE */
U_CAPI ಶೂನ್ಯ    U_EXPORT2
UCNV_TO_U_CALLBACK_STOP (
                   ಧ್ರುವ ಶೂನ್ಯ *context,
                   UConverterToUnicodeArgs *toUArgs,
                   ಧ್ರುವ ವರ್ಣ* codePoints,
                   int32_t length,
                   UConverterCallbackReason reason,
                   UErrorCode * err)
{
    /* the caller must have set the error code accordingly */
    (ಶೂನ್ಯ)context; (ಶೂನ್ಯ)toUArgs; (ಶೂನ್ಯ)codePoints; (ಶೂನ್ಯ)length; (ಶೂನ್ಯ)reason; (ಶೂನ್ಯ)err;
}

U_CAPI ಶೂನ್ಯ    U_EXPORT2
UCNV_FROM_U_CALLBACK_SKIP (                  
                  ಧ್ರುವ ಶೂನ್ಯ *context,
                  UConverterFromUnicodeArgs *fromUArgs,
                  ಧ್ರುವ char16_t* codeUnits,
                  int32_t length,
                  UChar32 codePoint,
                  UConverterCallbackReason reason,
                  UErrorCode * err)
{
    (ಶೂನ್ಯ)fromUArgs;
    (ಶೂನ್ಯ)codeUnits;
    (ಶೂನ್ಯ)length;
    ಯದಿ (reason <= UCNV_IRREGULAR)
    {
        ಯದಿ (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_CODE_POINT(codePoint))
        {
            /*
             * Skip if the codepoint has unicode property of default ignorable.
             */
            *err = U_ZERO_ERROR;
        }
        ಅನ್ಯಥಾ ಯದಿ (context == nullptr || (*((ವರ್ಣ*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
        {
            *err = U_ZERO_ERROR;
        }
        /* else the caller must have set the error code accordingly. */
    }
    /* else ignore the reset, close and clone calls. */
}

U_CAPI ಶೂನ್ಯ    U_EXPORT2
UCNV_FROM_U_CALLBACK_SUBSTITUTE (
                  ಧ್ರುವ ಶೂನ್ಯ *context,
                  UConverterFromUnicodeArgs *fromArgs,
                  ಧ್ರುವ char16_t* codeUnits,
                  int32_t length,
                  UChar32 codePoint,
                  UConverterCallbackReason reason,
                  UErrorCode * err)
{
    (ಶೂನ್ಯ)codeUnits;
    (ಶೂನ್ಯ)length;
    ಯದಿ (reason <= UCNV_IRREGULAR)
    {
        ಯದಿ (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_CODE_POINT(codePoint))
        {
            /*
             * Skip if the codepoint has unicode property of default ignorable.
             */
            *err = U_ZERO_ERROR;
        }
        ಅನ್ಯಥಾ ಯದಿ (context == nullptr || (*((ವರ್ಣ*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
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
U_CAPI ಶೂನ್ಯ    U_EXPORT2
UCNV_FROM_U_CALLBACK_ESCAPE (
                         ಧ್ರುವ ಶೂನ್ಯ *context,
                         UConverterFromUnicodeArgs *fromArgs,
                         ಧ್ರುವ char16_t *codeUnits,
                         int32_t length,
                         UChar32 codePoint,
                         UConverterCallbackReason reason,
                         UErrorCode * err)
{

  char16_t ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_STRING_LENGTH];
  int32_t ಮೌಲ್ಯ_String_Length = 0;
  int32_t i = 0;

  ಧ್ರುವ char16_t *my_ಮೌಲ್ಯ_Source = nullptr;
  UErrorCode err2 = U_ZERO_ERROR;
  UConverterFromUCallback original = nullptr;
  ಧ್ರುವ ಶೂನ್ಯ *originalContext;

  UConverterFromUCallback ignoredCallback = nullptr;
  ಧ್ರುವ ಶೂನ್ಯ *ignoredContext;
  
  ಯದಿ (reason > UCNV_IRREGULAR)
  {
      ಪ್ರತ್ಯಾಗಮ;
  }
  ಅನ್ಯಥಾ ಯದಿ (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_CODE_POINT(codePoint))
  {
      /*
       * Skip if the codepoint has unicode property of default ignorable.
       */
      *err = U_ZERO_ERROR;
      ಪ್ರತ್ಯಾಗಮ;
  }

  ucnv_setFromUCallBack (fromArgs->converter,
                     (UConverterFromUCallback) UCNV_FROM_U_CALLBACK_SUBSTITUTE,
                     nullptr,
                     &original,
                     &originalContext,
                     &err2);
  
  ಯದಿ (U_FAILURE (err2))
  {
    *err = err2;
    ಪ್ರತ್ಯಾಗಮ;
  } 
  ಯದಿ(context==nullptr)
  { 
      ಯಾವತ್ (i < length)
      {
        ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_Length++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT;  /* adding % */
        ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_Length++] = (char16_t) UNICODE_U_CODEPOINT; /* adding U */
        ಮೌಲ್ಯ_String_Length += uprv_itou (ಮೌಲ್ಯ_String + ಮೌಲ್ಯ_String_Length, ಮೌಲ್ಯ_STRING_LENGTH - ಮೌಲ್ಯ_String_Length, (uint16_t)codeUnits[i++], 16, 4);
      }
  }
  ಅನ್ಯಥಾ
  {
      ವಿಕಲ್ಪ(*((ವರ್ಣ*)context))
      {
      ಪಕ್ಷ UCNV_PRV_ESCAPE_JAVA:
          ಯಾವತ್ (i < length)
          {
              ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_Length++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */
              ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_Length++] = (char16_t) UNICODE_U_ಕೆಳಗಿನ_CODEPOINT; /* adding u */
              ಮೌಲ್ಯ_String_Length += uprv_itou (ಮೌಲ್ಯ_String + ಮೌಲ್ಯ_String_Length, ಮೌಲ್ಯ_STRING_LENGTH - ಮೌಲ್ಯ_String_Length, (uint16_t)codeUnits[i++], 16, 4);
          }
          ಭಙ್ಗ;

      ಪಕ್ಷ UCNV_PRV_ESCAPE_C:
          ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_Length++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */

          ಯದಿ(length==2){
              ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_Length++] = (char16_t) UNICODE_U_CODEPOINT; /* adding U */
              ಮೌಲ್ಯ_String_Length += uprv_itou (ಮೌಲ್ಯ_String + ಮೌಲ್ಯ_String_Length, ಮೌಲ್ಯ_STRING_LENGTH - ಮೌಲ್ಯ_String_Length, codePoint, 16, 8);

          }
          ಅನ್ಯಥಾ{
              ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_Length++] = (char16_t) UNICODE_U_ಕೆಳಗಿನ_CODEPOINT; /* adding u */
              ಮೌಲ್ಯ_String_Length += uprv_itou (ಮೌಲ್ಯ_String + ಮೌಲ್ಯ_String_Length, ಮೌಲ್ಯ_STRING_LENGTH - ಮೌಲ್ಯ_String_Length, (uint16_t)codeUnits[0], 16, 4);
          }
          ಭಙ್ಗ;

      ಪಕ್ಷ UCNV_PRV_ESCAPE_XML_DEC:

          ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_Length++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
          ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_Length++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
          ಯದಿ(length==2){
              ಮೌಲ್ಯ_String_Length += uprv_itou (ಮೌಲ್ಯ_String + ಮೌಲ್ಯ_String_Length, ಮೌಲ್ಯ_STRING_LENGTH - ಮೌಲ್ಯ_String_Length, codePoint, 10, 0);
          }
          ಅನ್ಯಥಾ{
              ಮೌಲ್ಯ_String_Length += uprv_itou (ಮೌಲ್ಯ_String + ಮೌಲ್ಯ_String_Length, ಮೌಲ್ಯ_STRING_LENGTH - ಮೌಲ್ಯ_String_Length, (uint16_t)codeUnits[0], 10, 0);
          }
          ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_Length++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
          ಭಙ್ಗ;

      ಪಕ್ಷ UCNV_PRV_ESCAPE_XML_HEX:

          ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_Length++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
          ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_Length++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
          ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_Length++] = (char16_t) UNICODE_X_ಕೆಳಗಿನ_CODEPOINT; /* adding x */
          ಯದಿ(length==2){
              ಮೌಲ್ಯ_String_Length += uprv_itou (ಮೌಲ್ಯ_String + ಮೌಲ್ಯ_String_Length, ಮೌಲ್ಯ_STRING_LENGTH - ಮೌಲ್ಯ_String_Length, codePoint, 16, 0);
          }
          ಅನ್ಯಥಾ{
              ಮೌಲ್ಯ_String_Length += uprv_itou (ಮೌಲ್ಯ_String + ಮೌಲ್ಯ_String_Length, ಮೌಲ್ಯ_STRING_LENGTH - ಮೌಲ್ಯ_String_Length, (uint16_t)codeUnits[0], 16, 0);
          }
          ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_Length++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
          ಭಙ್ಗ;

      ಪಕ್ಷ UCNV_PRV_ESCAPE_UNICODE:
          ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_Length++] = (char16_t) UNICODE_ಎಡ_CURLY_CODEPOINT;    /* adding { */
          ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_Length++] = (char16_t) UNICODE_U_CODEPOINT;    /* adding U */
          ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_Length++] = (char16_t) UNICODE_PLUS_CODEPOINT; /* adding + */
          ಯದಿ (length == 2) {
              ಮೌಲ್ಯ_String_Length += uprv_itou (ಮೌಲ್ಯ_String + ಮೌಲ್ಯ_String_Length, ಮೌಲ್ಯ_STRING_LENGTH - ಮೌಲ್ಯ_String_Length, codePoint, 16, 4);
          } ಅನ್ಯಥಾ {
              ಮೌಲ್ಯ_String_Length += uprv_itou (ಮೌಲ್ಯ_String + ಮೌಲ್ಯ_String_Length, ಮೌಲ್ಯ_STRING_LENGTH - ಮೌಲ್ಯ_String_Length, (uint16_t)codeUnits[0], 16, 4);
          }
          ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_Length++] = (char16_t) UNICODE_ಬಲ_CURLY_CODEPOINT;    /* adding } */
          ಭಙ್ಗ;

      ಪಕ್ಷ UCNV_PRV_ESCAPE_CSS2:
          ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_Length++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */
          ಮೌಲ್ಯ_String_Length += uprv_itou (ಮೌಲ್ಯ_String + ಮೌಲ್ಯ_String_Length, ಮೌಲ್ಯ_STRING_LENGTH - ಮೌಲ್ಯ_String_Length, codePoint, 16, 0);
          /* Always add space character, because the next character might be whitespace,
             which would erroneously be considered the termination of the escape sequence. */
          ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_Length++] = (char16_t) UNICODE_SPACE_CODEPOINT;
          ಭಙ್ಗ;

      ಸ್ವಭಾವ:
          ಯಾವತ್ (i < length)
          {
              ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_Length++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT;  /* adding % */
              ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_Length++] = (char16_t) UNICODE_U_CODEPOINT;             /* adding U */
              ಮೌಲ್ಯ_String_Length += uprv_itou (ಮೌಲ್ಯ_String + ಮೌಲ್ಯ_String_Length, ಮೌಲ್ಯ_STRING_LENGTH - ಮೌಲ್ಯ_String_Length, (uint16_t)codeUnits[i++], 16, 4);
          }
      }
  }  
  my_ಮೌಲ್ಯ_Source = ಮೌಲ್ಯ_String;

  /* reset the error */
  *err = U_ZERO_ERROR;

  ucnv_cbFromUWriteUChars(fromArgs, &my_ಮೌಲ್ಯ_Source, my_ಮೌಲ್ಯ_Source+ಮೌಲ್ಯ_String_Length, 0, err);

  ucnv_setFromUCallBack (fromArgs->converter,
                         original,
                         originalContext,
                         &ignoredCallback,
                         &ignoredContext,
                         &err2);
  ಯದಿ (U_FAILURE (err2))
  {
      *err = err2;
      ಪ್ರತ್ಯಾಗಮ;
  }
}



U_CAPI ಶೂನ್ಯ  U_EXPORT2
UCNV_TO_U_CALLBACK_SKIP (
                 ಧ್ರುವ ಶೂನ್ಯ *context,
                 UConverterToUnicodeArgs *toArgs,
                 ಧ್ರುವ ವರ್ಣ* codeUnits,
                 int32_t length,
                 UConverterCallbackReason reason,
                 UErrorCode * err)
{
    (ಶೂನ್ಯ)toArgs;
    (ಶೂನ್ಯ)codeUnits;
    (ಶೂನ್ಯ)length;
    ಯದಿ (reason <= UCNV_IRREGULAR)
    {
        ಯದಿ (context == nullptr || (*((ವರ್ಣ*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
        {
            *err = U_ZERO_ERROR;
        }
        /* else the caller must have set the error code accordingly. */
    }
    /* else ignore the reset, close and clone calls. */
}

U_CAPI ಶೂನ್ಯ    U_EXPORT2
UCNV_TO_U_CALLBACK_SUBSTITUTE (
                 ಧ್ರುವ ಶೂನ್ಯ *context,
                 UConverterToUnicodeArgs *toArgs,
                 ಧ್ರುವ ವರ್ಣ* codeUnits,
                 int32_t length,
                 UConverterCallbackReason reason,
                 UErrorCode * err)
{
    (ಶೂನ್ಯ)codeUnits;
    (ಶೂನ್ಯ)length;
    ಯದಿ (reason <= UCNV_IRREGULAR)
    {
        ಯದಿ (context == nullptr || (*((ವರ್ಣ*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
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
U_CAPI ಶೂನ್ಯ   U_EXPORT2
UCNV_TO_U_CALLBACK_ESCAPE (
                 ಧ್ರುವ ಶೂನ್ಯ *context,
                 UConverterToUnicodeArgs *toArgs,
                 ಧ್ರುವ ವರ್ಣ* codeUnits,
                 int32_t length,
                 UConverterCallbackReason reason,
                 UErrorCode * err)
{
    char16_t uni_ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_STRING_LENGTH];
    int32_t ಮೌಲ್ಯ_String_Length = 0;
    int32_t i = 0;

    ಯದಿ (reason > UCNV_IRREGULAR)
    {
        ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ(context==nullptr)
    {    
        ಯಾವತ್ (i < length)
        {
            uni_ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_Length++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT; /* adding % */
            uni_ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_Length++] = (char16_t) UNICODE_X_CODEPOINT;    /* adding X */
            ಮೌಲ್ಯ_String_Length += uprv_itou (uni_ಮೌಲ್ಯ_String + ಮೌಲ್ಯ_String_Length, ಮೌಲ್ಯ_STRING_LENGTH - ಮೌಲ್ಯ_String_Length, (uint8_t) codeUnits[i++], 16, 2);
        }
    }
    ಅನ್ಯಥಾ
    {
        ವಿಕಲ್ಪ(*((ವರ್ಣ*)context))
        {
        ಪಕ್ಷ UCNV_PRV_ESCAPE_XML_DEC:
            ಯಾವತ್ (i < length)
            {
                uni_ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_Length++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
                uni_ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_Length++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
                ಮೌಲ್ಯ_String_Length += uprv_itou (uni_ಮೌಲ್ಯ_String + ಮೌಲ್ಯ_String_Length, ಮೌಲ್ಯ_STRING_LENGTH - ಮೌಲ್ಯ_String_Length, (uint8_t)codeUnits[i++], 10, 0);
                uni_ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_Length++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
            }
            ಭಙ್ಗ;

        ಪಕ್ಷ UCNV_PRV_ESCAPE_XML_HEX:
            ಯಾವತ್ (i < length)
            {
                uni_ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_Length++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
                uni_ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_Length++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
                uni_ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_Length++] = (char16_t) UNICODE_X_ಕೆಳಗಿನ_CODEPOINT; /* adding x */
                ಮೌಲ್ಯ_String_Length += uprv_itou (uni_ಮೌಲ್ಯ_String + ಮೌಲ್ಯ_String_Length, ಮೌಲ್ಯ_STRING_LENGTH - ಮೌಲ್ಯ_String_Length, (uint8_t)codeUnits[i++], 16, 0);
                uni_ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_Length++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
            }
            ಭಙ್ಗ;
        ಪಕ್ಷ UCNV_PRV_ESCAPE_C:
            ಯಾವತ್ (i < length)
            {
                uni_ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_Length++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */
                uni_ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_Length++] = (char16_t) UNICODE_X_ಕೆಳಗಿನ_CODEPOINT; /* adding x */
                ಮೌಲ್ಯ_String_Length += uprv_itou (uni_ಮೌಲ್ಯ_String + ಮೌಲ್ಯ_String_Length, ಮೌಲ್ಯ_STRING_LENGTH - ಮೌಲ್ಯ_String_Length, (uint8_t)codeUnits[i++], 16, 2);
            }
            ಭಙ್ಗ;
        ಸ್ವಭಾವ:
            ಯಾವತ್ (i < length)
            {
                uni_ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_Length++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT; /* adding % */
                uni_ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_Length++] = (char16_t) UNICODE_X_CODEPOINT;    /* adding X */
                uprv_itou (uni_ಮೌಲ್ಯ_String + ಮೌಲ್ಯ_String_Length, ಮೌಲ್ಯ_STRING_LENGTH - ಮೌಲ್ಯ_String_Length, (uint8_t) codeUnits[i++], 16, 2);
                ಮೌಲ್ಯ_String_Length += 2;
            }
        }
    }
    /* reset the error */
    *err = U_ZERO_ERROR;

    ucnv_cbToUWriteUChars(toArgs, uni_ಮೌಲ್ಯ_String, ಮೌಲ್ಯ_String_Length, 0, err);
}

#endif
