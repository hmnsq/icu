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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_CONVERSION

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucnv_err.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucnv_cb.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_cnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustrfmt.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਮੁੱਲ_STRING_ਲੰਮਾਈ 48
/*Magic # 32 = 4(number of char in value string) * 8(max number of bytes per char for any converter) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UNICODE_PERCENT_SIGN_CODEPOINT  0x0025
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UNICODE_U_CODEPOINT             0x0055
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UNICODE_X_CODEPOINT             0x0058
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UNICODE_RS_CODEPOINT            0x005C
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UNICODE_U_ਨੀਵਾਂ_CODEPOINT         0x0075
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UNICODE_X_ਨੀਵਾਂ_CODEPOINT         0x0078
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UNICODE_AMP_CODEPOINT           0x0026
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UNICODE_HASH_CODEPOINT          0x0023
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UNICODE_SEMICOLON_CODEPOINT     0x003B
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UNICODE_PLUS_CODEPOINT          0x002B
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UNICODE_ਖੱਬਾ_CURLY_CODEPOINT    0x007B
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UNICODE_ਸੱਜਾ_CURLY_CODEPOINT   0x007D
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UNICODE_SPACE_CODEPOINT         0x0020
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCNV_PRV_ESCAPE_ICU         0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCNV_PRV_ESCAPE_C           'C'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCNV_PRV_ESCAPE_XML_DEC     'D'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCNV_PRV_ESCAPE_XML_HEX     'X'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCNV_PRV_ESCAPE_JAVA        'J'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCNV_PRV_ESCAPE_UNICODE     'U'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCNV_PRV_ESCAPE_CSS2        'S'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCNV_PRV_STOP_ON_ILLEGAL    'i'

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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IS_DEFAULT_IGNORABLE_ਕੋਡ_ਬਿੰਦੂ(c) ( \
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
U_CAPI ਖਾਲੀ    U_EXPORT2
UCNV_FROM_U_CALLBACK_STOP (
                  ਅਚਲ ਖਾਲੀ *context,
                  UConverter_From_Unicode_ਦਲੀਲਾਂ *fromUArgs,
                  ਅਚਲ char16_t* ਕੋਡ_Units,
                  int32_t length,
                  UChar32 ਕੋਡ_ਬਿੰਦੂ,
                  UConverterCallbackReason reason,
                  UError_ਕੋਡ * err)
{
    (ਖਾਲੀ)context;
    (ਖਾਲੀ)fromUArgs;
    (ਖਾਲੀ)ਕੋਡ_Units;
    (ਖਾਲੀ)length;
    ਜੇ (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_ਕੋਡ_ਬਿੰਦੂ(ਕੋਡ_ਬਿੰਦੂ))
    {
        /*
         * Skip if the codepoint has unicode property of default ignorable.
         */
        *err = U_ZERO_ERROR;
    }
    /* the caller must have set the error code accordingly */
}


/*Function Pointer STOPS at the ILLEGAL_SEQUENCE */
U_CAPI ਖਾਲੀ    U_EXPORT2
UCNV_ਮੰਜ਼ਿਲ_U_CALLBACK_STOP (
                   ਅਚਲ ਖਾਲੀ *context,
                   UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *ਮੰਜ਼ਿਲ_UArgs,
                   ਅਚਲ ਅੱਖਰ* ਕੋਡ_Points,
                   int32_t length,
                   UConverterCallbackReason reason,
                   UError_ਕੋਡ * err)
{
    /* the caller must have set the error code accordingly */
    (ਖਾਲੀ)context; (ਖਾਲੀ)ਮੰਜ਼ਿਲ_UArgs; (ਖਾਲੀ)ਕੋਡ_Points; (ਖਾਲੀ)length; (ਖਾਲੀ)reason; (ਖਾਲੀ)err;
}

U_CAPI ਖਾਲੀ    U_EXPORT2
UCNV_FROM_U_CALLBACK_SKIP (                  
                  ਅਚਲ ਖਾਲੀ *context,
                  UConverter_From_Unicode_ਦਲੀਲਾਂ *fromUArgs,
                  ਅਚਲ char16_t* ਕੋਡ_Units,
                  int32_t length,
                  UChar32 ਕੋਡ_ਬਿੰਦੂ,
                  UConverterCallbackReason reason,
                  UError_ਕੋਡ * err)
{
    (ਖਾਲੀ)fromUArgs;
    (ਖਾਲੀ)ਕੋਡ_Units;
    (ਖਾਲੀ)length;
    ਜੇ (reason <= UCNV_IRREGULAR)
    {
        ਜੇ (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_ਕੋਡ_ਬਿੰਦੂ(ਕੋਡ_ਬਿੰਦੂ))
        {
            /*
             * Skip if the codepoint has unicode property of default ignorable.
             */
            *err = U_ZERO_ERROR;
        }
        ਨਹੀਂਤਾਂ ਜੇ (context == ਖਾਲੀ_ਸੂਚਕ || (*((ਅੱਖਰ*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
        {
            *err = U_ZERO_ERROR;
        }
        /* else the caller must have set the error code accordingly. */
    }
    /* else ignore the reset, close and clone calls. */
}

U_CAPI ਖਾਲੀ    U_EXPORT2
UCNV_FROM_U_CALLBACK_SUBSTITUTE (
                  ਅਚਲ ਖਾਲੀ *context,
                  UConverter_From_Unicode_ਦਲੀਲਾਂ *from_ਦਲੀਲਾਂ,
                  ਅਚਲ char16_t* ਕੋਡ_Units,
                  int32_t length,
                  UChar32 ਕੋਡ_ਬਿੰਦੂ,
                  UConverterCallbackReason reason,
                  UError_ਕੋਡ * err)
{
    (ਖਾਲੀ)ਕੋਡ_Units;
    (ਖਾਲੀ)length;
    ਜੇ (reason <= UCNV_IRREGULAR)
    {
        ਜੇ (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_ਕੋਡ_ਬਿੰਦੂ(ਕੋਡ_ਬਿੰਦੂ))
        {
            /*
             * Skip if the codepoint has unicode property of default ignorable.
             */
            *err = U_ZERO_ERROR;
        }
        ਨਹੀਂਤਾਂ ਜੇ (context == ਖਾਲੀ_ਸੂਚਕ || (*((ਅੱਖਰ*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
        {
            *err = U_ZERO_ERROR;
            ucnv_cbFromUWriteSub(from_ਦਲੀਲਾਂ, 0, err);
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
U_CAPI ਖਾਲੀ    U_EXPORT2
UCNV_FROM_U_CALLBACK_ESCAPE (
                         ਅਚਲ ਖਾਲੀ *context,
                         UConverter_From_Unicode_ਦਲੀਲਾਂ *from_ਦਲੀਲਾਂ,
                         ਅਚਲ char16_t *ਕੋਡ_Units,
                         int32_t length,
                         UChar32 ਕੋਡ_ਬਿੰਦੂ,
                         UConverterCallbackReason reason,
                         UError_ਕੋਡ * err)
{

  char16_t ਮੁੱਲ_String[ਮੁੱਲ_STRING_ਲੰਮਾਈ];
  int32_t ਮੁੱਲ_String_ਲੰਮਾਈ = 0;
  int32_t i = 0;

  ਅਚਲ char16_t *my_ਮੁੱਲ_ਸਰੋਤ = ਖਾਲੀ_ਸੂਚਕ;
  UError_ਕੋਡ err2 = U_ZERO_ERROR;
  UConverterFromUCallback original = ਖਾਲੀ_ਸੂਚਕ;
  ਅਚਲ ਖਾਲੀ *originalContext;

  UConverterFromUCallback ignoredCallback = ਖਾਲੀ_ਸੂਚਕ;
  ਅਚਲ ਖਾਲੀ *ignoredContext;
  
  ਜੇ (reason > UCNV_IRREGULAR)
  {
      ਵਾਪਸ;
  }
  ਨਹੀਂਤਾਂ ਜੇ (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_ਕੋਡ_ਬਿੰਦੂ(ਕੋਡ_ਬਿੰਦੂ))
  {
      /*
       * Skip if the codepoint has unicode property of default ignorable.
       */
      *err = U_ZERO_ERROR;
      ਵਾਪਸ;
  }

  ucnv_ਸੈੱਟ_From_UCall_ਪਿੱਠ (from_ਦਲੀਲਾਂ->converter,
                     (UConverterFromUCallback) UCNV_FROM_U_CALLBACK_SUBSTITUTE,
                     ਖਾਲੀ_ਸੂਚਕ,
                     &original,
                     &originalContext,
                     &err2);
  
  ਜੇ (U_FAILURE (err2))
  {
    *err = err2;
    ਵਾਪਸ;
  } 
  ਜੇ(context==ਖਾਲੀ_ਸੂਚਕ)
  { 
      ਜਦੋਂ (i < length)
      {
        ਮੁੱਲ_String[ਮੁੱਲ_String_ਲੰਮਾਈ++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT;  /* adding % */
        ਮੁੱਲ_String[ਮੁੱਲ_String_ਲੰਮਾਈ++] = (char16_t) UNICODE_U_CODEPOINT; /* adding U */
        ਮੁੱਲ_String_ਲੰਮਾਈ += uprv_itou (ਮੁੱਲ_String + ਮੁੱਲ_String_ਲੰਮਾਈ, ਮੁੱਲ_STRING_ਲੰਮਾਈ - ਮੁੱਲ_String_ਲੰਮਾਈ, (uint16_t)ਕੋਡ_Units[i++], 16, 4);
      }
  }
  ਨਹੀਂਤਾਂ
  {
      ਚੋਣ(*((ਅੱਖਰ*)context))
      {
      ਹਾਲਤ UCNV_PRV_ESCAPE_JAVA:
          ਜਦੋਂ (i < length)
          {
              ਮੁੱਲ_String[ਮੁੱਲ_String_ਲੰਮਾਈ++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */
              ਮੁੱਲ_String[ਮੁੱਲ_String_ਲੰਮਾਈ++] = (char16_t) UNICODE_U_ਨੀਵਾਂ_CODEPOINT; /* adding u */
              ਮੁੱਲ_String_ਲੰਮਾਈ += uprv_itou (ਮੁੱਲ_String + ਮੁੱਲ_String_ਲੰਮਾਈ, ਮੁੱਲ_STRING_ਲੰਮਾਈ - ਮੁੱਲ_String_ਲੰਮਾਈ, (uint16_t)ਕੋਡ_Units[i++], 16, 4);
          }
          ਰੋਕੋ;

      ਹਾਲਤ UCNV_PRV_ESCAPE_C:
          ਮੁੱਲ_String[ਮੁੱਲ_String_ਲੰਮਾਈ++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */

          ਜੇ(length==2){
              ਮੁੱਲ_String[ਮੁੱਲ_String_ਲੰਮਾਈ++] = (char16_t) UNICODE_U_CODEPOINT; /* adding U */
              ਮੁੱਲ_String_ਲੰਮਾਈ += uprv_itou (ਮੁੱਲ_String + ਮੁੱਲ_String_ਲੰਮਾਈ, ਮੁੱਲ_STRING_ਲੰਮਾਈ - ਮੁੱਲ_String_ਲੰਮਾਈ, ਕੋਡ_ਬਿੰਦੂ, 16, 8);

          }
          ਨਹੀਂਤਾਂ{
              ਮੁੱਲ_String[ਮੁੱਲ_String_ਲੰਮਾਈ++] = (char16_t) UNICODE_U_ਨੀਵਾਂ_CODEPOINT; /* adding u */
              ਮੁੱਲ_String_ਲੰਮਾਈ += uprv_itou (ਮੁੱਲ_String + ਮੁੱਲ_String_ਲੰਮਾਈ, ਮੁੱਲ_STRING_ਲੰਮਾਈ - ਮੁੱਲ_String_ਲੰਮਾਈ, (uint16_t)ਕੋਡ_Units[0], 16, 4);
          }
          ਰੋਕੋ;

      ਹਾਲਤ UCNV_PRV_ESCAPE_XML_DEC:

          ਮੁੱਲ_String[ਮੁੱਲ_String_ਲੰਮਾਈ++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
          ਮੁੱਲ_String[ਮੁੱਲ_String_ਲੰਮਾਈ++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
          ਜੇ(length==2){
              ਮੁੱਲ_String_ਲੰਮਾਈ += uprv_itou (ਮੁੱਲ_String + ਮੁੱਲ_String_ਲੰਮਾਈ, ਮੁੱਲ_STRING_ਲੰਮਾਈ - ਮੁੱਲ_String_ਲੰਮਾਈ, ਕੋਡ_ਬਿੰਦੂ, 10, 0);
          }
          ਨਹੀਂਤਾਂ{
              ਮੁੱਲ_String_ਲੰਮਾਈ += uprv_itou (ਮੁੱਲ_String + ਮੁੱਲ_String_ਲੰਮਾਈ, ਮੁੱਲ_STRING_ਲੰਮਾਈ - ਮੁੱਲ_String_ਲੰਮਾਈ, (uint16_t)ਕੋਡ_Units[0], 10, 0);
          }
          ਮੁੱਲ_String[ਮੁੱਲ_String_ਲੰਮਾਈ++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
          ਰੋਕੋ;

      ਹਾਲਤ UCNV_PRV_ESCAPE_XML_HEX:

          ਮੁੱਲ_String[ਮੁੱਲ_String_ਲੰਮਾਈ++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
          ਮੁੱਲ_String[ਮੁੱਲ_String_ਲੰਮਾਈ++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
          ਮੁੱਲ_String[ਮੁੱਲ_String_ਲੰਮਾਈ++] = (char16_t) UNICODE_X_ਨੀਵਾਂ_CODEPOINT; /* adding x */
          ਜੇ(length==2){
              ਮੁੱਲ_String_ਲੰਮਾਈ += uprv_itou (ਮੁੱਲ_String + ਮੁੱਲ_String_ਲੰਮਾਈ, ਮੁੱਲ_STRING_ਲੰਮਾਈ - ਮੁੱਲ_String_ਲੰਮਾਈ, ਕੋਡ_ਬਿੰਦੂ, 16, 0);
          }
          ਨਹੀਂਤਾਂ{
              ਮੁੱਲ_String_ਲੰਮਾਈ += uprv_itou (ਮੁੱਲ_String + ਮੁੱਲ_String_ਲੰਮਾਈ, ਮੁੱਲ_STRING_ਲੰਮਾਈ - ਮੁੱਲ_String_ਲੰਮਾਈ, (uint16_t)ਕੋਡ_Units[0], 16, 0);
          }
          ਮੁੱਲ_String[ਮੁੱਲ_String_ਲੰਮਾਈ++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
          ਰੋਕੋ;

      ਹਾਲਤ UCNV_PRV_ESCAPE_UNICODE:
          ਮੁੱਲ_String[ਮੁੱਲ_String_ਲੰਮਾਈ++] = (char16_t) UNICODE_ਖੱਬਾ_CURLY_CODEPOINT;    /* adding { */
          ਮੁੱਲ_String[ਮੁੱਲ_String_ਲੰਮਾਈ++] = (char16_t) UNICODE_U_CODEPOINT;    /* adding U */
          ਮੁੱਲ_String[ਮੁੱਲ_String_ਲੰਮਾਈ++] = (char16_t) UNICODE_PLUS_CODEPOINT; /* adding + */
          ਜੇ (length == 2) {
              ਮੁੱਲ_String_ਲੰਮਾਈ += uprv_itou (ਮੁੱਲ_String + ਮੁੱਲ_String_ਲੰਮਾਈ, ਮੁੱਲ_STRING_ਲੰਮਾਈ - ਮੁੱਲ_String_ਲੰਮਾਈ, ਕੋਡ_ਬਿੰਦੂ, 16, 4);
          } ਨਹੀਂਤਾਂ {
              ਮੁੱਲ_String_ਲੰਮਾਈ += uprv_itou (ਮੁੱਲ_String + ਮੁੱਲ_String_ਲੰਮਾਈ, ਮੁੱਲ_STRING_ਲੰਮਾਈ - ਮੁੱਲ_String_ਲੰਮਾਈ, (uint16_t)ਕੋਡ_Units[0], 16, 4);
          }
          ਮੁੱਲ_String[ਮੁੱਲ_String_ਲੰਮਾਈ++] = (char16_t) UNICODE_ਸੱਜਾ_CURLY_CODEPOINT;    /* adding } */
          ਰੋਕੋ;

      ਹਾਲਤ UCNV_PRV_ESCAPE_CSS2:
          ਮੁੱਲ_String[ਮੁੱਲ_String_ਲੰਮਾਈ++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */
          ਮੁੱਲ_String_ਲੰਮਾਈ += uprv_itou (ਮੁੱਲ_String + ਮੁੱਲ_String_ਲੰਮਾਈ, ਮੁੱਲ_STRING_ਲੰਮਾਈ - ਮੁੱਲ_String_ਲੰਮਾਈ, ਕੋਡ_ਬਿੰਦੂ, 16, 0);
          /* Always add space character, because the next character might be whitespace,
             which would erroneously be considered the termination of the escape sequence. */
          ਮੁੱਲ_String[ਮੁੱਲ_String_ਲੰਮਾਈ++] = (char16_t) UNICODE_SPACE_CODEPOINT;
          ਰੋਕੋ;

      ਮੂਲ:
          ਜਦੋਂ (i < length)
          {
              ਮੁੱਲ_String[ਮੁੱਲ_String_ਲੰਮਾਈ++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT;  /* adding % */
              ਮੁੱਲ_String[ਮੁੱਲ_String_ਲੰਮਾਈ++] = (char16_t) UNICODE_U_CODEPOINT;             /* adding U */
              ਮੁੱਲ_String_ਲੰਮਾਈ += uprv_itou (ਮੁੱਲ_String + ਮੁੱਲ_String_ਲੰਮਾਈ, ਮੁੱਲ_STRING_ਲੰਮਾਈ - ਮੁੱਲ_String_ਲੰਮਾਈ, (uint16_t)ਕੋਡ_Units[i++], 16, 4);
          }
      }
  }  
  my_ਮੁੱਲ_ਸਰੋਤ = ਮੁੱਲ_String;

  /* reset the error */
  *err = U_ZERO_ERROR;

  ucnv_cbFromUWriteUChars(from_ਦਲੀਲਾਂ, &my_ਮੁੱਲ_ਸਰੋਤ, my_ਮੁੱਲ_ਸਰੋਤ+ਮੁੱਲ_String_ਲੰਮਾਈ, 0, err);

  ucnv_ਸੈੱਟ_From_UCall_ਪਿੱਠ (from_ਦਲੀਲਾਂ->converter,
                         original,
                         originalContext,
                         &ignoredCallback,
                         &ignoredContext,
                         &err2);
  ਜੇ (U_FAILURE (err2))
  {
      *err = err2;
      ਵਾਪਸ;
  }
}



U_CAPI ਖਾਲੀ  U_EXPORT2
UCNV_ਮੰਜ਼ਿਲ_U_CALLBACK_SKIP (
                 ਅਚਲ ਖਾਲੀ *context,
                 UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *ਮੰਜ਼ਿਲ_ਦਲੀਲਾਂ,
                 ਅਚਲ ਅੱਖਰ* ਕੋਡ_Units,
                 int32_t length,
                 UConverterCallbackReason reason,
                 UError_ਕੋਡ * err)
{
    (ਖਾਲੀ)ਮੰਜ਼ਿਲ_ਦਲੀਲਾਂ;
    (ਖਾਲੀ)ਕੋਡ_Units;
    (ਖਾਲੀ)length;
    ਜੇ (reason <= UCNV_IRREGULAR)
    {
        ਜੇ (context == ਖਾਲੀ_ਸੂਚਕ || (*((ਅੱਖਰ*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
        {
            *err = U_ZERO_ERROR;
        }
        /* else the caller must have set the error code accordingly. */
    }
    /* else ignore the reset, close and clone calls. */
}

U_CAPI ਖਾਲੀ    U_EXPORT2
UCNV_ਮੰਜ਼ਿਲ_U_CALLBACK_SUBSTITUTE (
                 ਅਚਲ ਖਾਲੀ *context,
                 UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *ਮੰਜ਼ਿਲ_ਦਲੀਲਾਂ,
                 ਅਚਲ ਅੱਖਰ* ਕੋਡ_Units,
                 int32_t length,
                 UConverterCallbackReason reason,
                 UError_ਕੋਡ * err)
{
    (ਖਾਲੀ)ਕੋਡ_Units;
    (ਖਾਲੀ)length;
    ਜੇ (reason <= UCNV_IRREGULAR)
    {
        ਜੇ (context == ਖਾਲੀ_ਸੂਚਕ || (*((ਅੱਖਰ*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
        {
            *err = U_ZERO_ERROR;
            ucnv_cb_ਮੰਜ਼ਿਲ_UWrite_Sub(ਮੰਜ਼ਿਲ_ਦਲੀਲਾਂ,0,err);
        }
        /* else the caller must have set the error code accordingly. */
    }
    /* else ignore the reset, close and clone calls. */
}

/*uses uprv_itou to get a unicode escape sequence of the offensive sequence,
 *and uses that as the substitution sequence
 */
U_CAPI ਖਾਲੀ   U_EXPORT2
UCNV_ਮੰਜ਼ਿਲ_U_CALLBACK_ESCAPE (
                 ਅਚਲ ਖਾਲੀ *context,
                 UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *ਮੰਜ਼ਿਲ_ਦਲੀਲਾਂ,
                 ਅਚਲ ਅੱਖਰ* ਕੋਡ_Units,
                 int32_t length,
                 UConverterCallbackReason reason,
                 UError_ਕੋਡ * err)
{
    char16_t uni_ਮੁੱਲ_String[ਮੁੱਲ_STRING_ਲੰਮਾਈ];
    int32_t ਮੁੱਲ_String_ਲੰਮਾਈ = 0;
    int32_t i = 0;

    ਜੇ (reason > UCNV_IRREGULAR)
    {
        ਵਾਪਸ;
    }

    ਜੇ(context==ਖਾਲੀ_ਸੂਚਕ)
    {    
        ਜਦੋਂ (i < length)
        {
            uni_ਮੁੱਲ_String[ਮੁੱਲ_String_ਲੰਮਾਈ++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT; /* adding % */
            uni_ਮੁੱਲ_String[ਮੁੱਲ_String_ਲੰਮਾਈ++] = (char16_t) UNICODE_X_CODEPOINT;    /* adding X */
            ਮੁੱਲ_String_ਲੰਮਾਈ += uprv_itou (uni_ਮੁੱਲ_String + ਮੁੱਲ_String_ਲੰਮਾਈ, ਮੁੱਲ_STRING_ਲੰਮਾਈ - ਮੁੱਲ_String_ਲੰਮਾਈ, (uint8_t) ਕੋਡ_Units[i++], 16, 2);
        }
    }
    ਨਹੀਂਤਾਂ
    {
        ਚੋਣ(*((ਅੱਖਰ*)context))
        {
        ਹਾਲਤ UCNV_PRV_ESCAPE_XML_DEC:
            ਜਦੋਂ (i < length)
            {
                uni_ਮੁੱਲ_String[ਮੁੱਲ_String_ਲੰਮਾਈ++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
                uni_ਮੁੱਲ_String[ਮੁੱਲ_String_ਲੰਮਾਈ++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
                ਮੁੱਲ_String_ਲੰਮਾਈ += uprv_itou (uni_ਮੁੱਲ_String + ਮੁੱਲ_String_ਲੰਮਾਈ, ਮੁੱਲ_STRING_ਲੰਮਾਈ - ਮੁੱਲ_String_ਲੰਮਾਈ, (uint8_t)ਕੋਡ_Units[i++], 10, 0);
                uni_ਮੁੱਲ_String[ਮੁੱਲ_String_ਲੰਮਾਈ++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
            }
            ਰੋਕੋ;

        ਹਾਲਤ UCNV_PRV_ESCAPE_XML_HEX:
            ਜਦੋਂ (i < length)
            {
                uni_ਮੁੱਲ_String[ਮੁੱਲ_String_ਲੰਮਾਈ++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
                uni_ਮੁੱਲ_String[ਮੁੱਲ_String_ਲੰਮਾਈ++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
                uni_ਮੁੱਲ_String[ਮੁੱਲ_String_ਲੰਮਾਈ++] = (char16_t) UNICODE_X_ਨੀਵਾਂ_CODEPOINT; /* adding x */
                ਮੁੱਲ_String_ਲੰਮਾਈ += uprv_itou (uni_ਮੁੱਲ_String + ਮੁੱਲ_String_ਲੰਮਾਈ, ਮੁੱਲ_STRING_ਲੰਮਾਈ - ਮੁੱਲ_String_ਲੰਮਾਈ, (uint8_t)ਕੋਡ_Units[i++], 16, 0);
                uni_ਮੁੱਲ_String[ਮੁੱਲ_String_ਲੰਮਾਈ++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
            }
            ਰੋਕੋ;
        ਹਾਲਤ UCNV_PRV_ESCAPE_C:
            ਜਦੋਂ (i < length)
            {
                uni_ਮੁੱਲ_String[ਮੁੱਲ_String_ਲੰਮਾਈ++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */
                uni_ਮੁੱਲ_String[ਮੁੱਲ_String_ਲੰਮਾਈ++] = (char16_t) UNICODE_X_ਨੀਵਾਂ_CODEPOINT; /* adding x */
                ਮੁੱਲ_String_ਲੰਮਾਈ += uprv_itou (uni_ਮੁੱਲ_String + ਮੁੱਲ_String_ਲੰਮਾਈ, ਮੁੱਲ_STRING_ਲੰਮਾਈ - ਮੁੱਲ_String_ਲੰਮਾਈ, (uint8_t)ਕੋਡ_Units[i++], 16, 2);
            }
            ਰੋਕੋ;
        ਮੂਲ:
            ਜਦੋਂ (i < length)
            {
                uni_ਮੁੱਲ_String[ਮੁੱਲ_String_ਲੰਮਾਈ++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT; /* adding % */
                uni_ਮੁੱਲ_String[ਮੁੱਲ_String_ਲੰਮਾਈ++] = (char16_t) UNICODE_X_CODEPOINT;    /* adding X */
                uprv_itou (uni_ਮੁੱਲ_String + ਮੁੱਲ_String_ਲੰਮਾਈ, ਮੁੱਲ_STRING_ਲੰਮਾਈ - ਮੁੱਲ_String_ਲੰਮਾਈ, (uint8_t) ਕੋਡ_Units[i++], 16, 2);
                ਮੁੱਲ_String_ਲੰਮਾਈ += 2;
            }
        }
    }
    /* reset the error */
    *err = U_ZERO_ERROR;

    ucnv_cb_ਮੰਜ਼ਿਲ_UWrite_UChars(ਮੰਜ਼ਿਲ_ਦਲੀਲਾਂ, uni_ਮੁੱਲ_String, ਮੁੱਲ_String_ਲੰਮਾਈ, 0, err);
}

#ਜੇ_ਅੰਤ
