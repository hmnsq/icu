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

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_CONVERSION

#شامل "unicode/ucnv_err.h"
#شامل "unicode/ucnv_cb.h"
#شامل "ucnv_cnv.h"
#شامل "cmemory.h"
#شامل "unicode/ucnv.h"
#شامل "ustrfmt.h"

#تعریف_کرو قدر_STRING_لمبائی 48
/*Magic # 32 = 4(number of char in value string) * 8(max number of bytes per char for any converter) */
#تعریف_کرو UNICODE_PERCENT_SIGN_CODEPOINT  0x0025
#تعریف_کرو UNICODE_U_CODEPOINT             0x0055
#تعریف_کرو UNICODE_X_CODEPOINT             0x0058
#تعریف_کرو UNICODE_RS_CODEPOINT            0x005C
#تعریف_کرو UNICODE_U_کم_CODEPOINT         0x0075
#تعریف_کرو UNICODE_X_کم_CODEPOINT         0x0078
#تعریف_کرو UNICODE_AMP_CODEPOINT           0x0026
#تعریف_کرو UNICODE_HASH_CODEPOINT          0x0023
#تعریف_کرو UNICODE_SEMICOLON_CODEPOINT     0x003B
#تعریف_کرو UNICODE_PLUS_CODEPOINT          0x002B
#تعریف_کرو UNICODE_بایاں_CURLY_CODEPOINT    0x007B
#تعریف_کرو UNICODE_دایاں_CURLY_CODEPOINT   0x007D
#تعریف_کرو UNICODE_SPACE_CODEPOINT         0x0020
#تعریف_کرو UCNV_PRV_ESCAPE_ICU         0
#تعریف_کرو UCNV_PRV_ESCAPE_C           'C'
#تعریف_کرو UCNV_PRV_ESCAPE_XML_DEC     'D'
#تعریف_کرو UCNV_PRV_ESCAPE_XML_HEX     'X'
#تعریف_کرو UCNV_PRV_ESCAPE_JAVA        'J'
#تعریف_کرو UCNV_PRV_ESCAPE_UNICODE     'U'
#تعریف_کرو UCNV_PRV_ESCAPE_CSS2        'S'
#تعریف_کرو UCNV_PRV_STOP_ON_ILLEGAL    'i'

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
#تعریف_کرو IS_DEFAULT_IGNORABLE_رمز_نقطہ(c) ( \
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
U_CAPI خالی    U_EXPORT2
UCNV_FROM_U_CALLBACK_STOP (
                  ثابت خالی *context,
                  UConverter_From_Unicode_دلائل *fromUArgs,
                  ثابت char16_t* رمز_Units,
                  int32_t length,
                  UChar32 رمز_نقطہ,
                  UConverterCallbackReason reason,
                  UError_رمز * err)
{
    (خالی)context;
    (خالی)fromUArgs;
    (خالی)رمز_Units;
    (خالی)length;
    اگر (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_رمز_نقطہ(رمز_نقطہ))
    {
        /*
         * Skip if the codepoint has unicode property of default ignorable.
         */
        *err = U_ZERO_ERROR;
    }
    /* the caller must have set the error code accordingly */
}


/*Function Pointer STOPS at the ILLEGAL_SEQUENCE */
U_CAPI خالی    U_EXPORT2
UCNV_مقصد_U_CALLBACK_STOP (
                   ثابت خالی *context,
                   UConverter_مقصد_Unicode_دلائل *مقصد_UArgs,
                   ثابت حرف* رمز_Points,
                   int32_t length,
                   UConverterCallbackReason reason,
                   UError_رمز * err)
{
    /* the caller must have set the error code accordingly */
    (خالی)context; (خالی)مقصد_UArgs; (خالی)رمز_Points; (خالی)length; (خالی)reason; (خالی)err;
}

U_CAPI خالی    U_EXPORT2
UCNV_FROM_U_CALLBACK_SKIP (                  
                  ثابت خالی *context,
                  UConverter_From_Unicode_دلائل *fromUArgs,
                  ثابت char16_t* رمز_Units,
                  int32_t length,
                  UChar32 رمز_نقطہ,
                  UConverterCallbackReason reason,
                  UError_رمز * err)
{
    (خالی)fromUArgs;
    (خالی)رمز_Units;
    (خالی)length;
    اگر (reason <= UCNV_IRREGULAR)
    {
        اگر (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_رمز_نقطہ(رمز_نقطہ))
        {
            /*
             * Skip if the codepoint has unicode property of default ignorable.
             */
            *err = U_ZERO_ERROR;
        }
        ورنہ اگر (context == خالی_اشاریہ || (*((حرف*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
        {
            *err = U_ZERO_ERROR;
        }
        /* else the caller must have set the error code accordingly. */
    }
    /* else ignore the reset, close and clone calls. */
}

U_CAPI خالی    U_EXPORT2
UCNV_FROM_U_CALLBACK_SUBSTITUTE (
                  ثابت خالی *context,
                  UConverter_From_Unicode_دلائل *from_دلائل,
                  ثابت char16_t* رمز_Units,
                  int32_t length,
                  UChar32 رمز_نقطہ,
                  UConverterCallbackReason reason,
                  UError_رمز * err)
{
    (خالی)رمز_Units;
    (خالی)length;
    اگر (reason <= UCNV_IRREGULAR)
    {
        اگر (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_رمز_نقطہ(رمز_نقطہ))
        {
            /*
             * Skip if the codepoint has unicode property of default ignorable.
             */
            *err = U_ZERO_ERROR;
        }
        ورنہ اگر (context == خالی_اشاریہ || (*((حرف*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
        {
            *err = U_ZERO_ERROR;
            ucnv_cbFromUWriteSub(from_دلائل, 0, err);
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
U_CAPI خالی    U_EXPORT2
UCNV_FROM_U_CALLBACK_ESCAPE (
                         ثابت خالی *context,
                         UConverter_From_Unicode_دلائل *from_دلائل,
                         ثابت char16_t *رمز_Units,
                         int32_t length,
                         UChar32 رمز_نقطہ,
                         UConverterCallbackReason reason,
                         UError_رمز * err)
{

  char16_t قدر_String[قدر_STRING_لمبائی];
  int32_t قدر_String_لمبائی = 0;
  int32_t i = 0;

  ثابت char16_t *my_قدر_ماخذ = خالی_اشاریہ;
  UError_رمز err2 = U_ZERO_ERROR;
  UConverterFromUCallback original = خالی_اشاریہ;
  ثابت خالی *originalContext;

  UConverterFromUCallback ignoredCallback = خالی_اشاریہ;
  ثابت خالی *ignoredContext;
  
  اگر (reason > UCNV_IRREGULAR)
  {
      واپس;
  }
  ورنہ اگر (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_رمز_نقطہ(رمز_نقطہ))
  {
      /*
       * Skip if the codepoint has unicode property of default ignorable.
       */
      *err = U_ZERO_ERROR;
      واپس;
  }

  ucnv_سیٹ_From_UCall_عقب (from_دلائل->converter,
                     (UConverterFromUCallback) UCNV_FROM_U_CALLBACK_SUBSTITUTE,
                     خالی_اشاریہ,
                     &original,
                     &originalContext,
                     &err2);
  
  اگر (U_FAILURE (err2))
  {
    *err = err2;
    واپس;
  } 
  اگر(context==خالی_اشاریہ)
  { 
      جب (i < length)
      {
        قدر_String[قدر_String_لمبائی++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT;  /* adding % */
        قدر_String[قدر_String_لمبائی++] = (char16_t) UNICODE_U_CODEPOINT; /* adding U */
        قدر_String_لمبائی += uprv_itou (قدر_String + قدر_String_لمبائی, قدر_STRING_لمبائی - قدر_String_لمبائی, (uint16_t)رمز_Units[i++], 16, 4);
      }
  }
  ورنہ
  {
      انتخاب(*((حرف*)context))
      {
      حالت UCNV_PRV_ESCAPE_JAVA:
          جب (i < length)
          {
              قدر_String[قدر_String_لمبائی++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */
              قدر_String[قدر_String_لمبائی++] = (char16_t) UNICODE_U_کم_CODEPOINT; /* adding u */
              قدر_String_لمبائی += uprv_itou (قدر_String + قدر_String_لمبائی, قدر_STRING_لمبائی - قدر_String_لمبائی, (uint16_t)رمز_Units[i++], 16, 4);
          }
          توڑو;

      حالت UCNV_PRV_ESCAPE_C:
          قدر_String[قدر_String_لمبائی++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */

          اگر(length==2){
              قدر_String[قدر_String_لمبائی++] = (char16_t) UNICODE_U_CODEPOINT; /* adding U */
              قدر_String_لمبائی += uprv_itou (قدر_String + قدر_String_لمبائی, قدر_STRING_لمبائی - قدر_String_لمبائی, رمز_نقطہ, 16, 8);

          }
          ورنہ{
              قدر_String[قدر_String_لمبائی++] = (char16_t) UNICODE_U_کم_CODEPOINT; /* adding u */
              قدر_String_لمبائی += uprv_itou (قدر_String + قدر_String_لمبائی, قدر_STRING_لمبائی - قدر_String_لمبائی, (uint16_t)رمز_Units[0], 16, 4);
          }
          توڑو;

      حالت UCNV_PRV_ESCAPE_XML_DEC:

          قدر_String[قدر_String_لمبائی++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
          قدر_String[قدر_String_لمبائی++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
          اگر(length==2){
              قدر_String_لمبائی += uprv_itou (قدر_String + قدر_String_لمبائی, قدر_STRING_لمبائی - قدر_String_لمبائی, رمز_نقطہ, 10, 0);
          }
          ورنہ{
              قدر_String_لمبائی += uprv_itou (قدر_String + قدر_String_لمبائی, قدر_STRING_لمبائی - قدر_String_لمبائی, (uint16_t)رمز_Units[0], 10, 0);
          }
          قدر_String[قدر_String_لمبائی++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
          توڑو;

      حالت UCNV_PRV_ESCAPE_XML_HEX:

          قدر_String[قدر_String_لمبائی++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
          قدر_String[قدر_String_لمبائی++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
          قدر_String[قدر_String_لمبائی++] = (char16_t) UNICODE_X_کم_CODEPOINT; /* adding x */
          اگر(length==2){
              قدر_String_لمبائی += uprv_itou (قدر_String + قدر_String_لمبائی, قدر_STRING_لمبائی - قدر_String_لمبائی, رمز_نقطہ, 16, 0);
          }
          ورنہ{
              قدر_String_لمبائی += uprv_itou (قدر_String + قدر_String_لمبائی, قدر_STRING_لمبائی - قدر_String_لمبائی, (uint16_t)رمز_Units[0], 16, 0);
          }
          قدر_String[قدر_String_لمبائی++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
          توڑو;

      حالت UCNV_PRV_ESCAPE_UNICODE:
          قدر_String[قدر_String_لمبائی++] = (char16_t) UNICODE_بایاں_CURLY_CODEPOINT;    /* adding { */
          قدر_String[قدر_String_لمبائی++] = (char16_t) UNICODE_U_CODEPOINT;    /* adding U */
          قدر_String[قدر_String_لمبائی++] = (char16_t) UNICODE_PLUS_CODEPOINT; /* adding + */
          اگر (length == 2) {
              قدر_String_لمبائی += uprv_itou (قدر_String + قدر_String_لمبائی, قدر_STRING_لمبائی - قدر_String_لمبائی, رمز_نقطہ, 16, 4);
          } ورنہ {
              قدر_String_لمبائی += uprv_itou (قدر_String + قدر_String_لمبائی, قدر_STRING_لمبائی - قدر_String_لمبائی, (uint16_t)رمز_Units[0], 16, 4);
          }
          قدر_String[قدر_String_لمبائی++] = (char16_t) UNICODE_دایاں_CURLY_CODEPOINT;    /* adding } */
          توڑو;

      حالت UCNV_PRV_ESCAPE_CSS2:
          قدر_String[قدر_String_لمبائی++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */
          قدر_String_لمبائی += uprv_itou (قدر_String + قدر_String_لمبائی, قدر_STRING_لمبائی - قدر_String_لمبائی, رمز_نقطہ, 16, 0);
          /* Always add space character, because the next character might be whitespace,
             which would erroneously be considered the termination of the escape sequence. */
          قدر_String[قدر_String_لمبائی++] = (char16_t) UNICODE_SPACE_CODEPOINT;
          توڑو;

      طےشدہ:
          جب (i < length)
          {
              قدر_String[قدر_String_لمبائی++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT;  /* adding % */
              قدر_String[قدر_String_لمبائی++] = (char16_t) UNICODE_U_CODEPOINT;             /* adding U */
              قدر_String_لمبائی += uprv_itou (قدر_String + قدر_String_لمبائی, قدر_STRING_لمبائی - قدر_String_لمبائی, (uint16_t)رمز_Units[i++], 16, 4);
          }
      }
  }  
  my_قدر_ماخذ = قدر_String;

  /* reset the error */
  *err = U_ZERO_ERROR;

  ucnv_cbFromUWriteUChars(from_دلائل, &my_قدر_ماخذ, my_قدر_ماخذ+قدر_String_لمبائی, 0, err);

  ucnv_سیٹ_From_UCall_عقب (from_دلائل->converter,
                         original,
                         originalContext,
                         &ignoredCallback,
                         &ignoredContext,
                         &err2);
  اگر (U_FAILURE (err2))
  {
      *err = err2;
      واپس;
  }
}



U_CAPI خالی  U_EXPORT2
UCNV_مقصد_U_CALLBACK_SKIP (
                 ثابت خالی *context,
                 UConverter_مقصد_Unicode_دلائل *مقصد_دلائل,
                 ثابت حرف* رمز_Units,
                 int32_t length,
                 UConverterCallbackReason reason,
                 UError_رمز * err)
{
    (خالی)مقصد_دلائل;
    (خالی)رمز_Units;
    (خالی)length;
    اگر (reason <= UCNV_IRREGULAR)
    {
        اگر (context == خالی_اشاریہ || (*((حرف*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
        {
            *err = U_ZERO_ERROR;
        }
        /* else the caller must have set the error code accordingly. */
    }
    /* else ignore the reset, close and clone calls. */
}

U_CAPI خالی    U_EXPORT2
UCNV_مقصد_U_CALLBACK_SUBSTITUTE (
                 ثابت خالی *context,
                 UConverter_مقصد_Unicode_دلائل *مقصد_دلائل,
                 ثابت حرف* رمز_Units,
                 int32_t length,
                 UConverterCallbackReason reason,
                 UError_رمز * err)
{
    (خالی)رمز_Units;
    (خالی)length;
    اگر (reason <= UCNV_IRREGULAR)
    {
        اگر (context == خالی_اشاریہ || (*((حرف*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
        {
            *err = U_ZERO_ERROR;
            ucnv_cb_مقصد_UWrite_Sub(مقصد_دلائل,0,err);
        }
        /* else the caller must have set the error code accordingly. */
    }
    /* else ignore the reset, close and clone calls. */
}

/*uses uprv_itou to get a unicode escape sequence of the offensive sequence,
 *and uses that as the substitution sequence
 */
U_CAPI خالی   U_EXPORT2
UCNV_مقصد_U_CALLBACK_ESCAPE (
                 ثابت خالی *context,
                 UConverter_مقصد_Unicode_دلائل *مقصد_دلائل,
                 ثابت حرف* رمز_Units,
                 int32_t length,
                 UConverterCallbackReason reason,
                 UError_رمز * err)
{
    char16_t uni_قدر_String[قدر_STRING_لمبائی];
    int32_t قدر_String_لمبائی = 0;
    int32_t i = 0;

    اگر (reason > UCNV_IRREGULAR)
    {
        واپس;
    }

    اگر(context==خالی_اشاریہ)
    {    
        جب (i < length)
        {
            uni_قدر_String[قدر_String_لمبائی++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT; /* adding % */
            uni_قدر_String[قدر_String_لمبائی++] = (char16_t) UNICODE_X_CODEPOINT;    /* adding X */
            قدر_String_لمبائی += uprv_itou (uni_قدر_String + قدر_String_لمبائی, قدر_STRING_لمبائی - قدر_String_لمبائی, (uint8_t) رمز_Units[i++], 16, 2);
        }
    }
    ورنہ
    {
        انتخاب(*((حرف*)context))
        {
        حالت UCNV_PRV_ESCAPE_XML_DEC:
            جب (i < length)
            {
                uni_قدر_String[قدر_String_لمبائی++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
                uni_قدر_String[قدر_String_لمبائی++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
                قدر_String_لمبائی += uprv_itou (uni_قدر_String + قدر_String_لمبائی, قدر_STRING_لمبائی - قدر_String_لمبائی, (uint8_t)رمز_Units[i++], 10, 0);
                uni_قدر_String[قدر_String_لمبائی++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
            }
            توڑو;

        حالت UCNV_PRV_ESCAPE_XML_HEX:
            جب (i < length)
            {
                uni_قدر_String[قدر_String_لمبائی++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
                uni_قدر_String[قدر_String_لمبائی++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
                uni_قدر_String[قدر_String_لمبائی++] = (char16_t) UNICODE_X_کم_CODEPOINT; /* adding x */
                قدر_String_لمبائی += uprv_itou (uni_قدر_String + قدر_String_لمبائی, قدر_STRING_لمبائی - قدر_String_لمبائی, (uint8_t)رمز_Units[i++], 16, 0);
                uni_قدر_String[قدر_String_لمبائی++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
            }
            توڑو;
        حالت UCNV_PRV_ESCAPE_C:
            جب (i < length)
            {
                uni_قدر_String[قدر_String_لمبائی++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */
                uni_قدر_String[قدر_String_لمبائی++] = (char16_t) UNICODE_X_کم_CODEPOINT; /* adding x */
                قدر_String_لمبائی += uprv_itou (uni_قدر_String + قدر_String_لمبائی, قدر_STRING_لمبائی - قدر_String_لمبائی, (uint8_t)رمز_Units[i++], 16, 2);
            }
            توڑو;
        طےشدہ:
            جب (i < length)
            {
                uni_قدر_String[قدر_String_لمبائی++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT; /* adding % */
                uni_قدر_String[قدر_String_لمبائی++] = (char16_t) UNICODE_X_CODEPOINT;    /* adding X */
                uprv_itou (uni_قدر_String + قدر_String_لمبائی, قدر_STRING_لمبائی - قدر_String_لمبائی, (uint8_t) رمز_Units[i++], 16, 2);
                قدر_String_لمبائی += 2;
            }
        }
    }
    /* reset the error */
    *err = U_ZERO_ERROR;

    ucnv_cb_مقصد_UWrite_UChars(مقصد_دلائل, uni_قدر_String, قدر_String_لمبائی, 0, err);
}

#اگر_ختم
