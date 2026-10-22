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

#تعریف_کن مقدار_STRING_درازی 48
/*Magic # 32 = 4(number of char in value string) * 8(max number of bytes per char for any converter) */
#تعریف_کن UNICODE_PERCENT_SIGN_CODEPOINT  0x0025
#تعریف_کن UNICODE_U_CODEPOINT             0x0055
#تعریف_کن UNICODE_X_CODEPOINT             0x0058
#تعریف_کن UNICODE_RS_CODEPOINT            0x005C
#تعریف_کن UNICODE_U_کم_CODEPOINT         0x0075
#تعریف_کن UNICODE_X_کم_CODEPOINT         0x0078
#تعریف_کن UNICODE_AMP_CODEPOINT           0x0026
#تعریف_کن UNICODE_HASH_CODEPOINT          0x0023
#تعریف_کن UNICODE_SEMICOLON_CODEPOINT     0x003B
#تعریف_کن UNICODE_PLUS_CODEPOINT          0x002B
#تعریف_کن UNICODE_چپ_CURLY_CODEPOINT    0x007B
#تعریف_کن UNICODE_راست_CURLY_CODEPOINT   0x007D
#تعریف_کن UNICODE_SPACE_CODEPOINT         0x0020
#تعریف_کن UCNV_PRV_ESCAPE_ICU         0
#تعریف_کن UCNV_PRV_ESCAPE_C           'C'
#تعریف_کن UCNV_PRV_ESCAPE_XML_DEC     'D'
#تعریف_کن UCNV_PRV_ESCAPE_XML_HEX     'X'
#تعریف_کن UCNV_PRV_ESCAPE_JAVA        'J'
#تعریف_کن UCNV_PRV_ESCAPE_UNICODE     'U'
#تعریف_کن UCNV_PRV_ESCAPE_CSS2        'S'
#تعریف_کن UCNV_PRV_STOP_ON_ILLEGAL    'i'

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
#تعریف_کن IS_DEFAULT_IGNORABLE_کد_نقطه(c) ( \
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
                  UConverter_From_Unicode_آرگومانها *fromUArgs,
                  ثابت char16_t* کد_Units,
                  int32_t length,
                  UChar32 کد_نقطه,
                  UConverterCallbackReason reason,
                  UError_کد * err)
{
    (فراغ)context;
    (فراغ)fromUArgs;
    (فراغ)کد_Units;
    (فراغ)length;
    إذا (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_کد_نقطه(کد_نقطه))
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
UCNV_مقصد_U_CALLBACK_STOP (
                   ثابت فراغ *context,
                   UConverter_مقصد_Unicode_آرگومانها *مقصد_UArgs,
                   ثابت حرف* کد_Points,
                   int32_t length,
                   UConverterCallbackReason reason,
                   UError_کد * err)
{
    /* the caller must have set the error code accordingly */
    (فراغ)context; (فراغ)مقصد_UArgs; (فراغ)کد_Points; (فراغ)length; (فراغ)reason; (فراغ)err;
}

U_CAPI فراغ    U_EXPORT2
UCNV_FROM_U_CALLBACK_SKIP (                  
                  ثابت فراغ *context,
                  UConverter_From_Unicode_آرگومانها *fromUArgs,
                  ثابت char16_t* کد_Units,
                  int32_t length,
                  UChar32 کد_نقطه,
                  UConverterCallbackReason reason,
                  UError_کد * err)
{
    (فراغ)fromUArgs;
    (فراغ)کد_Units;
    (فراغ)length;
    إذا (reason <= UCNV_IRREGULAR)
    {
        إذا (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_کد_نقطه(کد_نقطه))
        {
            /*
             * Skip if the codepoint has unicode property of default ignorable.
             */
            *err = U_ZERO_ERROR;
        }
        وإلا إذا (context == اشاره_تهی || (*((حرف*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
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
                  UConverter_From_Unicode_آرگومانها *from_آرگومانها,
                  ثابت char16_t* کد_Units,
                  int32_t length,
                  UChar32 کد_نقطه,
                  UConverterCallbackReason reason,
                  UError_کد * err)
{
    (فراغ)کد_Units;
    (فراغ)length;
    إذا (reason <= UCNV_IRREGULAR)
    {
        إذا (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_کد_نقطه(کد_نقطه))
        {
            /*
             * Skip if the codepoint has unicode property of default ignorable.
             */
            *err = U_ZERO_ERROR;
        }
        وإلا إذا (context == اشاره_تهی || (*((حرف*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
        {
            *err = U_ZERO_ERROR;
            ucnv_cbFromUWriteSub(from_آرگومانها, 0, err);
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
                         UConverter_From_Unicode_آرگومانها *from_آرگومانها,
                         ثابت char16_t *کد_Units,
                         int32_t length,
                         UChar32 کد_نقطه,
                         UConverterCallbackReason reason,
                         UError_کد * err)
{

  char16_t مقدار_String[مقدار_STRING_درازی];
  int32_t مقدار_String_درازی = 0;
  int32_t i = 0;

  ثابت char16_t *my_مقدار_منبع = اشاره_تهی;
  UError_کد err2 = U_ZERO_ERROR;
  UConverterFromUCallback original = اشاره_تهی;
  ثابت فراغ *originalContext;

  UConverterFromUCallback ignoredCallback = اشاره_تهی;
  ثابت فراغ *ignoredContext;
  
  إذا (reason > UCNV_IRREGULAR)
  {
      أرجع;
  }
  وإلا إذا (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_کد_نقطه(کد_نقطه))
  {
      /*
       * Skip if the codepoint has unicode property of default ignorable.
       */
      *err = U_ZERO_ERROR;
      أرجع;
  }

  ucnv_مجموعه_From_UCall_پشت (from_آرگومانها->converter,
                     (UConverterFromUCallback) UCNV_FROM_U_CALLBACK_SUBSTITUTE,
                     اشاره_تهی,
                     &original,
                     &originalContext,
                     &err2);
  
  إذا (U_FAILURE (err2))
  {
    *err = err2;
    أرجع;
  } 
  إذا(context==اشاره_تهی)
  { 
      طالما (i < length)
      {
        مقدار_String[مقدار_String_درازی++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT;  /* adding % */
        مقدار_String[مقدار_String_درازی++] = (char16_t) UNICODE_U_CODEPOINT; /* adding U */
        مقدار_String_درازی += uprv_itou (مقدار_String + مقدار_String_درازی, مقدار_STRING_درازی - مقدار_String_درازی, (uint16_t)کد_Units[i++], 16, 4);
      }
  }
  وإلا
  {
      بدل(*((حرف*)context))
      {
      حالة UCNV_PRV_ESCAPE_JAVA:
          طالما (i < length)
          {
              مقدار_String[مقدار_String_درازی++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */
              مقدار_String[مقدار_String_درازی++] = (char16_t) UNICODE_U_کم_CODEPOINT; /* adding u */
              مقدار_String_درازی += uprv_itou (مقدار_String + مقدار_String_درازی, مقدار_STRING_درازی - مقدار_String_درازی, (uint16_t)کد_Units[i++], 16, 4);
          }
          اكسر;

      حالة UCNV_PRV_ESCAPE_C:
          مقدار_String[مقدار_String_درازی++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */

          إذا(length==2){
              مقدار_String[مقدار_String_درازی++] = (char16_t) UNICODE_U_CODEPOINT; /* adding U */
              مقدار_String_درازی += uprv_itou (مقدار_String + مقدار_String_درازی, مقدار_STRING_درازی - مقدار_String_درازی, کد_نقطه, 16, 8);

          }
          وإلا{
              مقدار_String[مقدار_String_درازی++] = (char16_t) UNICODE_U_کم_CODEPOINT; /* adding u */
              مقدار_String_درازی += uprv_itou (مقدار_String + مقدار_String_درازی, مقدار_STRING_درازی - مقدار_String_درازی, (uint16_t)کد_Units[0], 16, 4);
          }
          اكسر;

      حالة UCNV_PRV_ESCAPE_XML_DEC:

          مقدار_String[مقدار_String_درازی++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
          مقدار_String[مقدار_String_درازی++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
          إذا(length==2){
              مقدار_String_درازی += uprv_itou (مقدار_String + مقدار_String_درازی, مقدار_STRING_درازی - مقدار_String_درازی, کد_نقطه, 10, 0);
          }
          وإلا{
              مقدار_String_درازی += uprv_itou (مقدار_String + مقدار_String_درازی, مقدار_STRING_درازی - مقدار_String_درازی, (uint16_t)کد_Units[0], 10, 0);
          }
          مقدار_String[مقدار_String_درازی++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
          اكسر;

      حالة UCNV_PRV_ESCAPE_XML_HEX:

          مقدار_String[مقدار_String_درازی++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
          مقدار_String[مقدار_String_درازی++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
          مقدار_String[مقدار_String_درازی++] = (char16_t) UNICODE_X_کم_CODEPOINT; /* adding x */
          إذا(length==2){
              مقدار_String_درازی += uprv_itou (مقدار_String + مقدار_String_درازی, مقدار_STRING_درازی - مقدار_String_درازی, کد_نقطه, 16, 0);
          }
          وإلا{
              مقدار_String_درازی += uprv_itou (مقدار_String + مقدار_String_درازی, مقدار_STRING_درازی - مقدار_String_درازی, (uint16_t)کد_Units[0], 16, 0);
          }
          مقدار_String[مقدار_String_درازی++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
          اكسر;

      حالة UCNV_PRV_ESCAPE_UNICODE:
          مقدار_String[مقدار_String_درازی++] = (char16_t) UNICODE_چپ_CURLY_CODEPOINT;    /* adding { */
          مقدار_String[مقدار_String_درازی++] = (char16_t) UNICODE_U_CODEPOINT;    /* adding U */
          مقدار_String[مقدار_String_درازی++] = (char16_t) UNICODE_PLUS_CODEPOINT; /* adding + */
          إذا (length == 2) {
              مقدار_String_درازی += uprv_itou (مقدار_String + مقدار_String_درازی, مقدار_STRING_درازی - مقدار_String_درازی, کد_نقطه, 16, 4);
          } وإلا {
              مقدار_String_درازی += uprv_itou (مقدار_String + مقدار_String_درازی, مقدار_STRING_درازی - مقدار_String_درازی, (uint16_t)کد_Units[0], 16, 4);
          }
          مقدار_String[مقدار_String_درازی++] = (char16_t) UNICODE_راست_CURLY_CODEPOINT;    /* adding } */
          اكسر;

      حالة UCNV_PRV_ESCAPE_CSS2:
          مقدار_String[مقدار_String_درازی++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */
          مقدار_String_درازی += uprv_itou (مقدار_String + مقدار_String_درازی, مقدار_STRING_درازی - مقدار_String_درازی, کد_نقطه, 16, 0);
          /* Always add space character, because the next character might be whitespace,
             which would erroneously be considered the termination of the escape sequence. */
          مقدار_String[مقدار_String_درازی++] = (char16_t) UNICODE_SPACE_CODEPOINT;
          اكسر;

      افتراضي:
          طالما (i < length)
          {
              مقدار_String[مقدار_String_درازی++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT;  /* adding % */
              مقدار_String[مقدار_String_درازی++] = (char16_t) UNICODE_U_CODEPOINT;             /* adding U */
              مقدار_String_درازی += uprv_itou (مقدار_String + مقدار_String_درازی, مقدار_STRING_درازی - مقدار_String_درازی, (uint16_t)کد_Units[i++], 16, 4);
          }
      }
  }  
  my_مقدار_منبع = مقدار_String;

  /* reset the error */
  *err = U_ZERO_ERROR;

  ucnv_cbFromUWriteUChars(from_آرگومانها, &my_مقدار_منبع, my_مقدار_منبع+مقدار_String_درازی, 0, err);

  ucnv_مجموعه_From_UCall_پشت (from_آرگومانها->converter,
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
UCNV_مقصد_U_CALLBACK_SKIP (
                 ثابت فراغ *context,
                 UConverter_مقصد_Unicode_آرگومانها *مقصد_آرگومانها,
                 ثابت حرف* کد_Units,
                 int32_t length,
                 UConverterCallbackReason reason,
                 UError_کد * err)
{
    (فراغ)مقصد_آرگومانها;
    (فراغ)کد_Units;
    (فراغ)length;
    إذا (reason <= UCNV_IRREGULAR)
    {
        إذا (context == اشاره_تهی || (*((حرف*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
        {
            *err = U_ZERO_ERROR;
        }
        /* else the caller must have set the error code accordingly. */
    }
    /* else ignore the reset, close and clone calls. */
}

U_CAPI فراغ    U_EXPORT2
UCNV_مقصد_U_CALLBACK_SUBSTITUTE (
                 ثابت فراغ *context,
                 UConverter_مقصد_Unicode_آرگومانها *مقصد_آرگومانها,
                 ثابت حرف* کد_Units,
                 int32_t length,
                 UConverterCallbackReason reason,
                 UError_کد * err)
{
    (فراغ)کد_Units;
    (فراغ)length;
    إذا (reason <= UCNV_IRREGULAR)
    {
        إذا (context == اشاره_تهی || (*((حرف*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
        {
            *err = U_ZERO_ERROR;
            ucnv_cb_مقصد_UWrite_Sub(مقصد_آرگومانها,0,err);
        }
        /* else the caller must have set the error code accordingly. */
    }
    /* else ignore the reset, close and clone calls. */
}

/*uses uprv_itou to get a unicode escape sequence of the offensive sequence,
 *and uses that as the substitution sequence
 */
U_CAPI فراغ   U_EXPORT2
UCNV_مقصد_U_CALLBACK_ESCAPE (
                 ثابت فراغ *context,
                 UConverter_مقصد_Unicode_آرگومانها *مقصد_آرگومانها,
                 ثابت حرف* کد_Units,
                 int32_t length,
                 UConverterCallbackReason reason,
                 UError_کد * err)
{
    char16_t uni_مقدار_String[مقدار_STRING_درازی];
    int32_t مقدار_String_درازی = 0;
    int32_t i = 0;

    إذا (reason > UCNV_IRREGULAR)
    {
        أرجع;
    }

    إذا(context==اشاره_تهی)
    {    
        طالما (i < length)
        {
            uni_مقدار_String[مقدار_String_درازی++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT; /* adding % */
            uni_مقدار_String[مقدار_String_درازی++] = (char16_t) UNICODE_X_CODEPOINT;    /* adding X */
            مقدار_String_درازی += uprv_itou (uni_مقدار_String + مقدار_String_درازی, مقدار_STRING_درازی - مقدار_String_درازی, (uint8_t) کد_Units[i++], 16, 2);
        }
    }
    وإلا
    {
        بدل(*((حرف*)context))
        {
        حالة UCNV_PRV_ESCAPE_XML_DEC:
            طالما (i < length)
            {
                uni_مقدار_String[مقدار_String_درازی++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
                uni_مقدار_String[مقدار_String_درازی++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
                مقدار_String_درازی += uprv_itou (uni_مقدار_String + مقدار_String_درازی, مقدار_STRING_درازی - مقدار_String_درازی, (uint8_t)کد_Units[i++], 10, 0);
                uni_مقدار_String[مقدار_String_درازی++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
            }
            اكسر;

        حالة UCNV_PRV_ESCAPE_XML_HEX:
            طالما (i < length)
            {
                uni_مقدار_String[مقدار_String_درازی++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
                uni_مقدار_String[مقدار_String_درازی++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
                uni_مقدار_String[مقدار_String_درازی++] = (char16_t) UNICODE_X_کم_CODEPOINT; /* adding x */
                مقدار_String_درازی += uprv_itou (uni_مقدار_String + مقدار_String_درازی, مقدار_STRING_درازی - مقدار_String_درازی, (uint8_t)کد_Units[i++], 16, 0);
                uni_مقدار_String[مقدار_String_درازی++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
            }
            اكسر;
        حالة UCNV_PRV_ESCAPE_C:
            طالما (i < length)
            {
                uni_مقدار_String[مقدار_String_درازی++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */
                uni_مقدار_String[مقدار_String_درازی++] = (char16_t) UNICODE_X_کم_CODEPOINT; /* adding x */
                مقدار_String_درازی += uprv_itou (uni_مقدار_String + مقدار_String_درازی, مقدار_STRING_درازی - مقدار_String_درازی, (uint8_t)کد_Units[i++], 16, 2);
            }
            اكسر;
        افتراضي:
            طالما (i < length)
            {
                uni_مقدار_String[مقدار_String_درازی++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT; /* adding % */
                uni_مقدار_String[مقدار_String_درازی++] = (char16_t) UNICODE_X_CODEPOINT;    /* adding X */
                uprv_itou (uni_مقدار_String + مقدار_String_درازی, مقدار_STRING_درازی - مقدار_String_درازی, (uint8_t) کد_Units[i++], 16, 2);
                مقدار_String_درازی += 2;
            }
        }
    }
    /* reset the error */
    *err = U_ZERO_ERROR;

    ucnv_cb_مقصد_UWrite_UChars(مقصد_آرگومانها, uni_مقدار_String, مقدار_String_درازی, 0, err);
}

#پایان_اگر
