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

#ಆದರೆ !UCONFIG_NO_CONVERSION

#ಅನ್ತರ್ಭಾವ "unicode/ucnv_err.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucnv_cb.h"
#ಅನ್ತರ್ಭಾವ "ucnv_cnv.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucnv.h"
#ಅನ್ತರ್ಭಾವ "ustrfmt.h"

#ವ್ಯಾಖ್ಯಾನಿಸು ಮೌಲ್ಯ_STRING_ಉದ್ದಳತೆ 48
/*Magic # 32 = 4(number of char in value string) * 8(max number of bytes per char for any converter) */
#ವ್ಯಾಖ್ಯಾನಿಸು UNICODE_PERCENT_SIGN_CODEPOINT  0x0025
#ವ್ಯಾಖ್ಯಾನಿಸು UNICODE_U_CODEPOINT             0x0055
#ವ್ಯಾಖ್ಯಾನಿಸು UNICODE_X_CODEPOINT             0x0058
#ವ್ಯಾಖ್ಯಾನಿಸು UNICODE_RS_CODEPOINT            0x005C
#ವ್ಯಾಖ್ಯಾನಿಸು UNICODE_U_ಕೆಳಗಿನ_CODEPOINT         0x0075
#ವ್ಯಾಖ್ಯಾನಿಸು UNICODE_X_ಕೆಳಗಿನ_CODEPOINT         0x0078
#ವ್ಯಾಖ್ಯಾನಿಸು UNICODE_AMP_CODEPOINT           0x0026
#ವ್ಯಾಖ್ಯಾನಿಸು UNICODE_HASH_CODEPOINT          0x0023
#ವ್ಯಾಖ್ಯಾನಿಸು UNICODE_SEMICOLON_CODEPOINT     0x003B
#ವ್ಯಾಖ್ಯಾನಿಸು UNICODE_PLUS_CODEPOINT          0x002B
#ವ್ಯಾಖ್ಯಾನಿಸು UNICODE_ಎಡ_CURLY_CODEPOINT    0x007B
#ವ್ಯಾಖ್ಯಾನಿಸು UNICODE_ಬಲ_CURLY_CODEPOINT   0x007D
#ವ್ಯಾಖ್ಯಾನಿಸು UNICODE_SPACE_CODEPOINT         0x0020
#ವ್ಯಾಖ್ಯಾನಿಸು UCNV_PRV_ESCAPE_ICU         0
#ವ್ಯಾಖ್ಯಾನಿಸು UCNV_PRV_ESCAPE_C           'C'
#ವ್ಯಾಖ್ಯಾನಿಸು UCNV_PRV_ESCAPE_XML_DEC     'D'
#ವ್ಯಾಖ್ಯಾನಿಸು UCNV_PRV_ESCAPE_XML_HEX     'X'
#ವ್ಯಾಖ್ಯಾನಿಸು UCNV_PRV_ESCAPE_JAVA        'J'
#ವ್ಯಾಖ್ಯಾನಿಸು UCNV_PRV_ESCAPE_UNICODE     'U'
#ವ್ಯಾಖ್ಯಾನಿಸು UCNV_PRV_ESCAPE_CSS2        'S'
#ವ್ಯಾಖ್ಯಾನಿಸು UCNV_PRV_STOP_ON_ILLEGAL    'i'

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
#ವ್ಯಾಖ್ಯಾನಿಸು IS_DEFAULT_IGNORABLE_ಸಂಕೇತ_ಬಿಂದು(c) ( \
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
                  ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context,
                  UConverter_From_Unicode_ವಾದಗಳು *fromUArgs,
                  ಸ್ಥಿರಾಂಕ char16_t* ಸಂಕೇತ_Units,
                  int32_t length,
                  UChar32 ಸಂಕೇತ_ಬಿಂದು,
                  UConverterCallbackReason reason,
                  UError_ಸಂಕೇತ * err)
{
    (ಶೂನ್ಯ)context;
    (ಶೂನ್ಯ)fromUArgs;
    (ಶೂನ್ಯ)ಸಂಕೇತ_Units;
    (ಶೂನ್ಯ)length;
    ಆದರೆ (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_ಸಂಕೇತ_ಬಿಂದು(ಸಂಕೇತ_ಬಿಂದು))
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
UCNV_ಗಮ್ಯ_U_CALLBACK_STOP (
                   ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context,
                   UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *ಗಮ್ಯ_UArgs,
                   ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸಂಕೇತ_Points,
                   int32_t length,
                   UConverterCallbackReason reason,
                   UError_ಸಂಕೇತ * err)
{
    /* the caller must have set the error code accordingly */
    (ಶೂನ್ಯ)context; (ಶೂನ್ಯ)ಗಮ್ಯ_UArgs; (ಶೂನ್ಯ)ಸಂಕೇತ_Points; (ಶೂನ್ಯ)length; (ಶೂನ್ಯ)reason; (ಶೂನ್ಯ)err;
}

U_CAPI ಶೂನ್ಯ    U_EXPORT2
UCNV_FROM_U_CALLBACK_SKIP (                  
                  ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context,
                  UConverter_From_Unicode_ವಾದಗಳು *fromUArgs,
                  ಸ್ಥಿರಾಂಕ char16_t* ಸಂಕೇತ_Units,
                  int32_t length,
                  UChar32 ಸಂಕೇತ_ಬಿಂದು,
                  UConverterCallbackReason reason,
                  UError_ಸಂಕೇತ * err)
{
    (ಶೂನ್ಯ)fromUArgs;
    (ಶೂನ್ಯ)ಸಂಕೇತ_Units;
    (ಶೂನ್ಯ)length;
    ಆದರೆ (reason <= UCNV_IRREGULAR)
    {
        ಆದರೆ (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_ಸಂಕೇತ_ಬಿಂದು(ಸಂಕೇತ_ಬಿಂದು))
        {
            /*
             * Skip if the codepoint has unicode property of default ignorable.
             */
            *err = U_ZERO_ERROR;
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (context == ಶೂನ್ಯಸೂಚಕ || (*((ಅಕ್ಷರ*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
        {
            *err = U_ZERO_ERROR;
        }
        /* else the caller must have set the error code accordingly. */
    }
    /* else ignore the reset, close and clone calls. */
}

U_CAPI ಶೂನ್ಯ    U_EXPORT2
UCNV_FROM_U_CALLBACK_SUBSTITUTE (
                  ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context,
                  UConverter_From_Unicode_ವಾದಗಳು *from_ವಾದಗಳು,
                  ಸ್ಥಿರಾಂಕ char16_t* ಸಂಕೇತ_Units,
                  int32_t length,
                  UChar32 ಸಂಕೇತ_ಬಿಂದು,
                  UConverterCallbackReason reason,
                  UError_ಸಂಕೇತ * err)
{
    (ಶೂನ್ಯ)ಸಂಕೇತ_Units;
    (ಶೂನ್ಯ)length;
    ಆದರೆ (reason <= UCNV_IRREGULAR)
    {
        ಆದರೆ (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_ಸಂಕೇತ_ಬಿಂದು(ಸಂಕೇತ_ಬಿಂದು))
        {
            /*
             * Skip if the codepoint has unicode property of default ignorable.
             */
            *err = U_ZERO_ERROR;
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (context == ಶೂನ್ಯಸೂಚಕ || (*((ಅಕ್ಷರ*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
        {
            *err = U_ZERO_ERROR;
            ucnv_cbFromUWriteSub(from_ವಾದಗಳು, 0, err);
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
                         ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context,
                         UConverter_From_Unicode_ವಾದಗಳು *from_ವಾದಗಳು,
                         ಸ್ಥಿರಾಂಕ char16_t *ಸಂಕೇತ_Units,
                         int32_t length,
                         UChar32 ಸಂಕೇತ_ಬಿಂದು,
                         UConverterCallbackReason reason,
                         UError_ಸಂಕೇತ * err)
{

  char16_t ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_STRING_ಉದ್ದಳತೆ];
  int32_t ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ = 0;
  int32_t i = 0;

  ಸ್ಥಿರಾಂಕ char16_t *my_ಮೌಲ್ಯ_ಮೂಲ = ಶೂನ್ಯಸೂಚಕ;
  UError_ಸಂಕೇತ err2 = U_ZERO_ERROR;
  UConverterFromUCallback original = ಶೂನ್ಯಸೂಚಕ;
  ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *originalContext;

  UConverterFromUCallback ignoredCallback = ಶೂನ್ಯಸೂಚಕ;
  ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ignoredContext;
  
  ಆದರೆ (reason > UCNV_IRREGULAR)
  {
      ಹಿಂತಿರುಗಿ;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_ಸಂಕೇತ_ಬಿಂದು(ಸಂಕೇತ_ಬಿಂದು))
  {
      /*
       * Skip if the codepoint has unicode property of default ignorable.
       */
      *err = U_ZERO_ERROR;
      ಹಿಂತಿರುಗಿ;
  }

  ucnv_ಗಣ_From_UCall_ಹಿಂಭಾಗ (from_ವಾದಗಳು->converter,
                     (UConverterFromUCallback) UCNV_FROM_U_CALLBACK_SUBSTITUTE,
                     ಶೂನ್ಯಸೂಚಕ,
                     &original,
                     &originalContext,
                     &err2);
  
  ಆದರೆ (U_FAILURE (err2))
  {
    *err = err2;
    ಹಿಂತಿರುಗಿ;
  } 
  ಆದರೆ(context==ಶೂನ್ಯಸೂಚಕ)
  { 
      ಆಗ (i < length)
      {
        ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT;  /* adding % */
        ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ++] = (char16_t) UNICODE_U_CODEPOINT; /* adding U */
        ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ += uprv_itou (ಮೌಲ್ಯ_String + ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ, ಮೌಲ್ಯ_STRING_ಉದ್ದಳತೆ - ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ, (uint16_t)ಸಂಕೇತ_Units[i++], 16, 4);
      }
  }
  ಇಲ್ಲದಿದ್ದರೆ
  {
      ಆಯ್ಕೆ(*((ಅಕ್ಷರ*)context))
      {
      ಸ್ಥಿತಿ UCNV_PRV_ESCAPE_JAVA:
          ಆಗ (i < length)
          {
              ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */
              ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ++] = (char16_t) UNICODE_U_ಕೆಳಗಿನ_CODEPOINT; /* adding u */
              ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ += uprv_itou (ಮೌಲ್ಯ_String + ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ, ಮೌಲ್ಯ_STRING_ಉದ್ದಳತೆ - ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ, (uint16_t)ಸಂಕೇತ_Units[i++], 16, 4);
          }
          ನಿಲ್ಲಿಸು;

      ಸ್ಥಿತಿ UCNV_PRV_ESCAPE_C:
          ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */

          ಆದರೆ(length==2){
              ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ++] = (char16_t) UNICODE_U_CODEPOINT; /* adding U */
              ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ += uprv_itou (ಮೌಲ್ಯ_String + ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ, ಮೌಲ್ಯ_STRING_ಉದ್ದಳತೆ - ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ, ಸಂಕೇತ_ಬಿಂದು, 16, 8);

          }
          ಇಲ್ಲದಿದ್ದರೆ{
              ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ++] = (char16_t) UNICODE_U_ಕೆಳಗಿನ_CODEPOINT; /* adding u */
              ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ += uprv_itou (ಮೌಲ್ಯ_String + ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ, ಮೌಲ್ಯ_STRING_ಉದ್ದಳತೆ - ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ, (uint16_t)ಸಂಕೇತ_Units[0], 16, 4);
          }
          ನಿಲ್ಲಿಸು;

      ಸ್ಥಿತಿ UCNV_PRV_ESCAPE_XML_DEC:

          ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
          ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
          ಆದರೆ(length==2){
              ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ += uprv_itou (ಮೌಲ್ಯ_String + ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ, ಮೌಲ್ಯ_STRING_ಉದ್ದಳತೆ - ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ, ಸಂಕೇತ_ಬಿಂದು, 10, 0);
          }
          ಇಲ್ಲದಿದ್ದರೆ{
              ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ += uprv_itou (ಮೌಲ್ಯ_String + ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ, ಮೌಲ್ಯ_STRING_ಉದ್ದಳತೆ - ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ, (uint16_t)ಸಂಕೇತ_Units[0], 10, 0);
          }
          ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
          ನಿಲ್ಲಿಸು;

      ಸ್ಥಿತಿ UCNV_PRV_ESCAPE_XML_HEX:

          ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
          ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
          ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ++] = (char16_t) UNICODE_X_ಕೆಳಗಿನ_CODEPOINT; /* adding x */
          ಆದರೆ(length==2){
              ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ += uprv_itou (ಮೌಲ್ಯ_String + ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ, ಮೌಲ್ಯ_STRING_ಉದ್ದಳತೆ - ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ, ಸಂಕೇತ_ಬಿಂದು, 16, 0);
          }
          ಇಲ್ಲದಿದ್ದರೆ{
              ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ += uprv_itou (ಮೌಲ್ಯ_String + ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ, ಮೌಲ್ಯ_STRING_ಉದ್ದಳತೆ - ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ, (uint16_t)ಸಂಕೇತ_Units[0], 16, 0);
          }
          ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
          ನಿಲ್ಲಿಸು;

      ಸ್ಥಿತಿ UCNV_PRV_ESCAPE_UNICODE:
          ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ++] = (char16_t) UNICODE_ಎಡ_CURLY_CODEPOINT;    /* adding { */
          ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ++] = (char16_t) UNICODE_U_CODEPOINT;    /* adding U */
          ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ++] = (char16_t) UNICODE_PLUS_CODEPOINT; /* adding + */
          ಆದರೆ (length == 2) {
              ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ += uprv_itou (ಮೌಲ್ಯ_String + ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ, ಮೌಲ್ಯ_STRING_ಉದ್ದಳತೆ - ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ, ಸಂಕೇತ_ಬಿಂದು, 16, 4);
          } ಇಲ್ಲದಿದ್ದರೆ {
              ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ += uprv_itou (ಮೌಲ್ಯ_String + ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ, ಮೌಲ್ಯ_STRING_ಉದ್ದಳತೆ - ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ, (uint16_t)ಸಂಕೇತ_Units[0], 16, 4);
          }
          ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ++] = (char16_t) UNICODE_ಬಲ_CURLY_CODEPOINT;    /* adding } */
          ನಿಲ್ಲಿಸು;

      ಸ್ಥಿತಿ UCNV_PRV_ESCAPE_CSS2:
          ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */
          ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ += uprv_itou (ಮೌಲ್ಯ_String + ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ, ಮೌಲ್ಯ_STRING_ಉದ್ದಳತೆ - ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ, ಸಂಕೇತ_ಬಿಂದು, 16, 0);
          /* Always add space character, because the next character might be whitespace,
             which would erroneously be considered the termination of the escape sequence. */
          ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ++] = (char16_t) UNICODE_SPACE_CODEPOINT;
          ನಿಲ್ಲಿಸು;

      ಪೂರ್ವನಿಯೋಜಿತ:
          ಆಗ (i < length)
          {
              ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT;  /* adding % */
              ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ++] = (char16_t) UNICODE_U_CODEPOINT;             /* adding U */
              ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ += uprv_itou (ಮೌಲ್ಯ_String + ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ, ಮೌಲ್ಯ_STRING_ಉದ್ದಳತೆ - ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ, (uint16_t)ಸಂಕೇತ_Units[i++], 16, 4);
          }
      }
  }  
  my_ಮೌಲ್ಯ_ಮೂಲ = ಮೌಲ್ಯ_String;

  /* reset the error */
  *err = U_ZERO_ERROR;

  ucnv_cbFromUWriteUChars(from_ವಾದಗಳು, &my_ಮೌಲ್ಯ_ಮೂಲ, my_ಮೌಲ್ಯ_ಮೂಲ+ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ, 0, err);

  ucnv_ಗಣ_From_UCall_ಹಿಂಭಾಗ (from_ವಾದಗಳು->converter,
                         original,
                         originalContext,
                         &ignoredCallback,
                         &ignoredContext,
                         &err2);
  ಆದರೆ (U_FAILURE (err2))
  {
      *err = err2;
      ಹಿಂತಿರುಗಿ;
  }
}



U_CAPI ಶೂನ್ಯ  U_EXPORT2
UCNV_ಗಮ್ಯ_U_CALLBACK_SKIP (
                 ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context,
                 UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *ಗಮ್ಯ_ವಾದಗಳು,
                 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸಂಕೇತ_Units,
                 int32_t length,
                 UConverterCallbackReason reason,
                 UError_ಸಂಕೇತ * err)
{
    (ಶೂನ್ಯ)ಗಮ್ಯ_ವಾದಗಳು;
    (ಶೂನ್ಯ)ಸಂಕೇತ_Units;
    (ಶೂನ್ಯ)length;
    ಆದರೆ (reason <= UCNV_IRREGULAR)
    {
        ಆದರೆ (context == ಶೂನ್ಯಸೂಚಕ || (*((ಅಕ್ಷರ*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
        {
            *err = U_ZERO_ERROR;
        }
        /* else the caller must have set the error code accordingly. */
    }
    /* else ignore the reset, close and clone calls. */
}

U_CAPI ಶೂನ್ಯ    U_EXPORT2
UCNV_ಗಮ್ಯ_U_CALLBACK_SUBSTITUTE (
                 ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context,
                 UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *ಗಮ್ಯ_ವಾದಗಳು,
                 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸಂಕೇತ_Units,
                 int32_t length,
                 UConverterCallbackReason reason,
                 UError_ಸಂಕೇತ * err)
{
    (ಶೂನ್ಯ)ಸಂಕೇತ_Units;
    (ಶೂನ್ಯ)length;
    ಆದರೆ (reason <= UCNV_IRREGULAR)
    {
        ಆದರೆ (context == ಶೂನ್ಯಸೂಚಕ || (*((ಅಕ್ಷರ*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
        {
            *err = U_ZERO_ERROR;
            ucnv_cb_ಗಮ್ಯ_UWrite_Sub(ಗಮ್ಯ_ವಾದಗಳು,0,err);
        }
        /* else the caller must have set the error code accordingly. */
    }
    /* else ignore the reset, close and clone calls. */
}

/*uses uprv_itou to get a unicode escape sequence of the offensive sequence,
 *and uses that as the substitution sequence
 */
U_CAPI ಶೂನ್ಯ   U_EXPORT2
UCNV_ಗಮ್ಯ_U_CALLBACK_ESCAPE (
                 ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context,
                 UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *ಗಮ್ಯ_ವಾದಗಳು,
                 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸಂಕೇತ_Units,
                 int32_t length,
                 UConverterCallbackReason reason,
                 UError_ಸಂಕೇತ * err)
{
    char16_t uni_ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_STRING_ಉದ್ದಳತೆ];
    int32_t ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ = 0;
    int32_t i = 0;

    ಆದರೆ (reason > UCNV_IRREGULAR)
    {
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ(context==ಶೂನ್ಯಸೂಚಕ)
    {    
        ಆಗ (i < length)
        {
            uni_ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT; /* adding % */
            uni_ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ++] = (char16_t) UNICODE_X_CODEPOINT;    /* adding X */
            ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ += uprv_itou (uni_ಮೌಲ್ಯ_String + ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ, ಮೌಲ್ಯ_STRING_ಉದ್ದಳತೆ - ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ, (uint8_t) ಸಂಕೇತ_Units[i++], 16, 2);
        }
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
        ಆಯ್ಕೆ(*((ಅಕ್ಷರ*)context))
        {
        ಸ್ಥಿತಿ UCNV_PRV_ESCAPE_XML_DEC:
            ಆಗ (i < length)
            {
                uni_ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
                uni_ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
                ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ += uprv_itou (uni_ಮೌಲ್ಯ_String + ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ, ಮೌಲ್ಯ_STRING_ಉದ್ದಳತೆ - ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ, (uint8_t)ಸಂಕೇತ_Units[i++], 10, 0);
                uni_ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
            }
            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ UCNV_PRV_ESCAPE_XML_HEX:
            ಆಗ (i < length)
            {
                uni_ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
                uni_ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
                uni_ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ++] = (char16_t) UNICODE_X_ಕೆಳಗಿನ_CODEPOINT; /* adding x */
                ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ += uprv_itou (uni_ಮೌಲ್ಯ_String + ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ, ಮೌಲ್ಯ_STRING_ಉದ್ದಳತೆ - ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ, (uint8_t)ಸಂಕೇತ_Units[i++], 16, 0);
                uni_ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
            }
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ UCNV_PRV_ESCAPE_C:
            ಆಗ (i < length)
            {
                uni_ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */
                uni_ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ++] = (char16_t) UNICODE_X_ಕೆಳಗಿನ_CODEPOINT; /* adding x */
                ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ += uprv_itou (uni_ಮೌಲ್ಯ_String + ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ, ಮೌಲ್ಯ_STRING_ಉದ್ದಳತೆ - ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ, (uint8_t)ಸಂಕೇತ_Units[i++], 16, 2);
            }
            ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
            ಆಗ (i < length)
            {
                uni_ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT; /* adding % */
                uni_ಮೌಲ್ಯ_String[ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ++] = (char16_t) UNICODE_X_CODEPOINT;    /* adding X */
                uprv_itou (uni_ಮೌಲ್ಯ_String + ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ, ಮೌಲ್ಯ_STRING_ಉದ್ದಳತೆ - ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ, (uint8_t) ಸಂಕೇತ_Units[i++], 16, 2);
                ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ += 2;
            }
        }
    }
    /* reset the error */
    *err = U_ZERO_ERROR;

    ucnv_cb_ಗಮ್ಯ_UWrite_UChars(ಗಮ್ಯ_ವಾದಗಳು, uni_ಮೌಲ್ಯ_String, ಮೌಲ್ಯ_String_ಉದ್ದಳತೆ, 0, err);
}

#ಆದರೆ_ಅಂತ್ಯ
