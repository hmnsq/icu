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

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_CONVERSION

#ضمن "unicode/ucnv_err.h"
#ضمن "unicode/ucnv_cb.h"
#ضمن "ucnv_cnv.h"
#ضمن "cmemory.h"
#ضمن "unicode/ucnv.h"
#ضمن "ustrfmt.h"

#define درج_STRING_LENGTH 48
/*Magic # 32 = 4(number of char in value string) * 8(max number of bytes per char for any converter) */
#define UNICODE_PERCENT_SIGN_CODEPOINT  0x0025
#define UNICODE_U_CODEPOINT             0x0055
#define UNICODE_X_CODEPOINT             0x0058
#define UNICODE_RS_CODEPOINT            0x005C
#define UNICODE_U_كس_CODEPOINT         0x0075
#define UNICODE_X_كس_CODEPOINT         0x0078
#define UNICODE_AMP_CODEPOINT           0x0026
#define UNICODE_HASH_CODEPOINT          0x0023
#define UNICODE_SEMICOLON_CODEPOINT     0x003B
#define UNICODE_PLUS_CODEPOINT          0x002B
#define UNICODE_هغو_CURLY_CODEPOINT    0x007B
#define UNICODE_دام_CURLY_CODEPOINT   0x007D
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
U_CAPI فراغ    U_EXPORT2
UCNV_FROM_U_CALLBACK_STOP (
                  ثابت فراغ *context,
                  UConverter_From_Unicode_حجوجي *fromUArgs,
                  ثابت char16_t* codeUnits,
                  int32_t length,
                  UChar32 codePoint,
                  UConverterCallbackReason reason,
                  UErrorCode * err)
{
    (فراغ)context;
    (فراغ)fromUArgs;
    (فراغ)codeUnits;
    (فراغ)length;
    إذا (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_CODE_POINT(codePoint))
    {
        /*
         * Skip if the codepoint has unicode property of default ignorable.
         */
        *err = U_ZERO_ERROR;
    }
    /* the caller must have set the error code accordingly */
}


/*Function Pointer STOPS at the ILLEGAL_SEQUENCE */
U_CAPI فراغ    U_EXPORT2
UCNV_TO_U_CALLBACK_STOP (
                   ثابت فراغ *context,
                   UConverter_To_Unicode_حجوجي *toUArgs,
                   ثابت حرف* codePoints,
                   int32_t length,
                   UConverterCallbackReason reason,
                   UErrorCode * err)
{
    /* the caller must have set the error code accordingly */
    (فراغ)context; (فراغ)toUArgs; (فراغ)codePoints; (فراغ)length; (فراغ)reason; (فراغ)err;
}

U_CAPI فراغ    U_EXPORT2
UCNV_FROM_U_CALLBACK_SKIP (                  
                  ثابت فراغ *context,
                  UConverter_From_Unicode_حجوجي *fromUArgs,
                  ثابت char16_t* codeUnits,
                  int32_t length,
                  UChar32 codePoint,
                  UConverterCallbackReason reason,
                  UErrorCode * err)
{
    (فراغ)fromUArgs;
    (فراغ)codeUnits;
    (فراغ)length;
    إذا (reason <= UCNV_IRREGULAR)
    {
        إذا (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_CODE_POINT(codePoint))
        {
            /*
             * Skip if the codepoint has unicode property of default ignorable.
             */
            *err = U_ZERO_ERROR;
        }
        وإلا إذا (context == nullptr || (*((حرف*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
        {
            *err = U_ZERO_ERROR;
        }
        /* else the caller must have set the error code accordingly. */
    }
    /* else ignore the reset, close and clone calls. */
}

U_CAPI فراغ    U_EXPORT2
UCNV_FROM_U_CALLBACK_SUBSTITUTE (
                  ثابت فراغ *context,
                  UConverter_From_Unicode_حجوجي *from_حجوجي,
                  ثابت char16_t* codeUnits,
                  int32_t length,
                  UChar32 codePoint,
                  UConverterCallbackReason reason,
                  UErrorCode * err)
{
    (فراغ)codeUnits;
    (فراغ)length;
    إذا (reason <= UCNV_IRREGULAR)
    {
        إذا (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_CODE_POINT(codePoint))
        {
            /*
             * Skip if the codepoint has unicode property of default ignorable.
             */
            *err = U_ZERO_ERROR;
        }
        وإلا إذا (context == nullptr || (*((حرف*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
        {
            *err = U_ZERO_ERROR;
            ucnv_cbFromUWriteSub(from_حجوجي, 0, err);
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
U_CAPI فراغ    U_EXPORT2
UCNV_FROM_U_CALLBACK_ESCAPE (
                         ثابت فراغ *context,
                         UConverter_From_Unicode_حجوجي *from_حجوجي,
                         ثابت char16_t *codeUnits,
                         int32_t length,
                         UChar32 codePoint,
                         UConverterCallbackReason reason,
                         UErrorCode * err)
{

  char16_t درج_String[درج_STRING_LENGTH];
  int32_t درج_String_Length = 0;
  int32_t i = 0;

  ثابت char16_t *my_درج_Source = nullptr;
  UErrorCode err2 = U_ZERO_ERROR;
  UConverterFromUCallback original = nullptr;
  ثابت فراغ *originalContext;

  UConverterFromUCallback ignoredCallback = nullptr;
  ثابت فراغ *ignoredContext;
  
  إذا (reason > UCNV_IRREGULAR)
  {
      أرجع;
  }
  وإلا إذا (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_CODE_POINT(codePoint))
  {
      /*
       * Skip if the codepoint has unicode property of default ignorable.
       */
      *err = U_ZERO_ERROR;
      أرجع;
  }

  ucnv_setFromUCallBack (from_حجوجي->converter,
                     (UConverterFromUCallback) UCNV_FROM_U_CALLBACK_SUBSTITUTE,
                     nullptr,
                     &original,
                     &originalContext,
                     &err2);
  
  إذا (U_FAILURE (err2))
  {
    *err = err2;
    أرجع;
  } 
  إذا(context==nullptr)
  { 
      طالما (i < length)
      {
        درج_String[درج_String_Length++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT;  /* adding % */
        درج_String[درج_String_Length++] = (char16_t) UNICODE_U_CODEPOINT; /* adding U */
        درج_String_Length += uprv_itou (درج_String + درج_String_Length, درج_STRING_LENGTH - درج_String_Length, (uint16_t)codeUnits[i++], 16, 4);
      }
  }
  وإلا
  {
      بدل(*((حرف*)context))
      {
      حالة UCNV_PRV_ESCAPE_JAVA:
          طالما (i < length)
          {
              درج_String[درج_String_Length++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */
              درج_String[درج_String_Length++] = (char16_t) UNICODE_U_كس_CODEPOINT; /* adding u */
              درج_String_Length += uprv_itou (درج_String + درج_String_Length, درج_STRING_LENGTH - درج_String_Length, (uint16_t)codeUnits[i++], 16, 4);
          }
          اكسر;

      حالة UCNV_PRV_ESCAPE_C:
          درج_String[درج_String_Length++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */

          إذا(length==2){
              درج_String[درج_String_Length++] = (char16_t) UNICODE_U_CODEPOINT; /* adding U */
              درج_String_Length += uprv_itou (درج_String + درج_String_Length, درج_STRING_LENGTH - درج_String_Length, codePoint, 16, 8);

          }
          وإلا{
              درج_String[درج_String_Length++] = (char16_t) UNICODE_U_كس_CODEPOINT; /* adding u */
              درج_String_Length += uprv_itou (درج_String + درج_String_Length, درج_STRING_LENGTH - درج_String_Length, (uint16_t)codeUnits[0], 16, 4);
          }
          اكسر;

      حالة UCNV_PRV_ESCAPE_XML_DEC:

          درج_String[درج_String_Length++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
          درج_String[درج_String_Length++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
          إذا(length==2){
              درج_String_Length += uprv_itou (درج_String + درج_String_Length, درج_STRING_LENGTH - درج_String_Length, codePoint, 10, 0);
          }
          وإلا{
              درج_String_Length += uprv_itou (درج_String + درج_String_Length, درج_STRING_LENGTH - درج_String_Length, (uint16_t)codeUnits[0], 10, 0);
          }
          درج_String[درج_String_Length++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
          اكسر;

      حالة UCNV_PRV_ESCAPE_XML_HEX:

          درج_String[درج_String_Length++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
          درج_String[درج_String_Length++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
          درج_String[درج_String_Length++] = (char16_t) UNICODE_X_كس_CODEPOINT; /* adding x */
          إذا(length==2){
              درج_String_Length += uprv_itou (درج_String + درج_String_Length, درج_STRING_LENGTH - درج_String_Length, codePoint, 16, 0);
          }
          وإلا{
              درج_String_Length += uprv_itou (درج_String + درج_String_Length, درج_STRING_LENGTH - درج_String_Length, (uint16_t)codeUnits[0], 16, 0);
          }
          درج_String[درج_String_Length++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
          اكسر;

      حالة UCNV_PRV_ESCAPE_UNICODE:
          درج_String[درج_String_Length++] = (char16_t) UNICODE_هغو_CURLY_CODEPOINT;    /* adding { */
          درج_String[درج_String_Length++] = (char16_t) UNICODE_U_CODEPOINT;    /* adding U */
          درج_String[درج_String_Length++] = (char16_t) UNICODE_PLUS_CODEPOINT; /* adding + */
          إذا (length == 2) {
              درج_String_Length += uprv_itou (درج_String + درج_String_Length, درج_STRING_LENGTH - درج_String_Length, codePoint, 16, 4);
          } وإلا {
              درج_String_Length += uprv_itou (درج_String + درج_String_Length, درج_STRING_LENGTH - درج_String_Length, (uint16_t)codeUnits[0], 16, 4);
          }
          درج_String[درج_String_Length++] = (char16_t) UNICODE_دام_CURLY_CODEPOINT;    /* adding } */
          اكسر;

      حالة UCNV_PRV_ESCAPE_CSS2:
          درج_String[درج_String_Length++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */
          درج_String_Length += uprv_itou (درج_String + درج_String_Length, درج_STRING_LENGTH - درج_String_Length, codePoint, 16, 0);
          /* Always add space character, because the next character might be whitespace,
             which would erroneously be considered the termination of the escape sequence. */
          درج_String[درج_String_Length++] = (char16_t) UNICODE_SPACE_CODEPOINT;
          اكسر;

      افتراضي:
          طالما (i < length)
          {
              درج_String[درج_String_Length++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT;  /* adding % */
              درج_String[درج_String_Length++] = (char16_t) UNICODE_U_CODEPOINT;             /* adding U */
              درج_String_Length += uprv_itou (درج_String + درج_String_Length, درج_STRING_LENGTH - درج_String_Length, (uint16_t)codeUnits[i++], 16, 4);
          }
      }
  }  
  my_درج_Source = درج_String;

  /* reset the error */
  *err = U_ZERO_ERROR;

  ucnv_cbFromUWriteUChars(from_حجوجي, &my_درج_Source, my_درج_Source+درج_String_Length, 0, err);

  ucnv_setFromUCallBack (from_حجوجي->converter,
                         original,
                         originalContext,
                         &ignoredCallback,
                         &ignoredContext,
                         &err2);
  إذا (U_FAILURE (err2))
  {
      *err = err2;
      أرجع;
  }
}



U_CAPI فراغ  U_EXPORT2
UCNV_TO_U_CALLBACK_SKIP (
                 ثابت فراغ *context,
                 UConverter_To_Unicode_حجوجي *to_حجوجي,
                 ثابت حرف* codeUnits,
                 int32_t length,
                 UConverterCallbackReason reason,
                 UErrorCode * err)
{
    (فراغ)to_حجوجي;
    (فراغ)codeUnits;
    (فراغ)length;
    إذا (reason <= UCNV_IRREGULAR)
    {
        إذا (context == nullptr || (*((حرف*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
        {
            *err = U_ZERO_ERROR;
        }
        /* else the caller must have set the error code accordingly. */
    }
    /* else ignore the reset, close and clone calls. */
}

U_CAPI فراغ    U_EXPORT2
UCNV_TO_U_CALLBACK_SUBSTITUTE (
                 ثابت فراغ *context,
                 UConverter_To_Unicode_حجوجي *to_حجوجي,
                 ثابت حرف* codeUnits,
                 int32_t length,
                 UConverterCallbackReason reason,
                 UErrorCode * err)
{
    (فراغ)codeUnits;
    (فراغ)length;
    إذا (reason <= UCNV_IRREGULAR)
    {
        إذا (context == nullptr || (*((حرف*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
        {
            *err = U_ZERO_ERROR;
            ucnv_cbToUWriteSub(to_حجوجي,0,err);
        }
        /* else the caller must have set the error code accordingly. */
    }
    /* else ignore the reset, close and clone calls. */
}

/*uses uprv_itou to get a unicode escape sequence of the offensive sequence,
 *and uses that as the substitution sequence
 */
U_CAPI فراغ   U_EXPORT2
UCNV_TO_U_CALLBACK_ESCAPE (
                 ثابت فراغ *context,
                 UConverter_To_Unicode_حجوجي *to_حجوجي,
                 ثابت حرف* codeUnits,
                 int32_t length,
                 UConverterCallbackReason reason,
                 UErrorCode * err)
{
    char16_t uni_درج_String[درج_STRING_LENGTH];
    int32_t درج_String_Length = 0;
    int32_t i = 0;

    إذا (reason > UCNV_IRREGULAR)
    {
        أرجع;
    }

    إذا(context==nullptr)
    {    
        طالما (i < length)
        {
            uni_درج_String[درج_String_Length++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT; /* adding % */
            uni_درج_String[درج_String_Length++] = (char16_t) UNICODE_X_CODEPOINT;    /* adding X */
            درج_String_Length += uprv_itou (uni_درج_String + درج_String_Length, درج_STRING_LENGTH - درج_String_Length, (uint8_t) codeUnits[i++], 16, 2);
        }
    }
    وإلا
    {
        بدل(*((حرف*)context))
        {
        حالة UCNV_PRV_ESCAPE_XML_DEC:
            طالما (i < length)
            {
                uni_درج_String[درج_String_Length++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
                uni_درج_String[درج_String_Length++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
                درج_String_Length += uprv_itou (uni_درج_String + درج_String_Length, درج_STRING_LENGTH - درج_String_Length, (uint8_t)codeUnits[i++], 10, 0);
                uni_درج_String[درج_String_Length++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
            }
            اكسر;

        حالة UCNV_PRV_ESCAPE_XML_HEX:
            طالما (i < length)
            {
                uni_درج_String[درج_String_Length++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
                uni_درج_String[درج_String_Length++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
                uni_درج_String[درج_String_Length++] = (char16_t) UNICODE_X_كس_CODEPOINT; /* adding x */
                درج_String_Length += uprv_itou (uni_درج_String + درج_String_Length, درج_STRING_LENGTH - درج_String_Length, (uint8_t)codeUnits[i++], 16, 0);
                uni_درج_String[درج_String_Length++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
            }
            اكسر;
        حالة UCNV_PRV_ESCAPE_C:
            طالما (i < length)
            {
                uni_درج_String[درج_String_Length++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */
                uni_درج_String[درج_String_Length++] = (char16_t) UNICODE_X_كس_CODEPOINT; /* adding x */
                درج_String_Length += uprv_itou (uni_درج_String + درج_String_Length, درج_STRING_LENGTH - درج_String_Length, (uint8_t)codeUnits[i++], 16, 2);
            }
            اكسر;
        افتراضي:
            طالما (i < length)
            {
                uni_درج_String[درج_String_Length++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT; /* adding % */
                uni_درج_String[درج_String_Length++] = (char16_t) UNICODE_X_CODEPOINT;    /* adding X */
                uprv_itou (uni_درج_String + درج_String_Length, درج_STRING_LENGTH - درج_String_Length, (uint8_t) codeUnits[i++], 16, 2);
                درج_String_Length += 2;
            }
        }
    }
    /* reset the error */
    *err = U_ZERO_ERROR;

    ucnv_cbToUWriteUChars(to_حجوجي, uni_درج_String, درج_String_Length, 0, err);
}

#endif
