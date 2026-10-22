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

#وصف_ڪريو قدر_STRING_ڊيگهائي 48
/*Magic # 32 = 4(number of char in value string) * 8(max number of bytes per char for any converter) */
#وصف_ڪريو UNICODE_PERCENT_SIGN_CODEPOINT  0x0025
#وصف_ڪريو UNICODE_U_CODEPOINT             0x0055
#وصف_ڪريو UNICODE_X_CODEPOINT             0x0058
#وصف_ڪريو UNICODE_RS_CODEPOINT            0x005C
#وصف_ڪريو UNICODE_U_هيٺيون_CODEPOINT         0x0075
#وصف_ڪريو UNICODE_X_هيٺيون_CODEPOINT         0x0078
#وصف_ڪريو UNICODE_AMP_CODEPOINT           0x0026
#وصف_ڪريو UNICODE_HASH_CODEPOINT          0x0023
#وصف_ڪريو UNICODE_SEMICOLON_CODEPOINT     0x003B
#وصف_ڪريو UNICODE_PLUS_CODEPOINT          0x002B
#وصف_ڪريو UNICODE_کاٻو_CURLY_CODEPOINT    0x007B
#وصف_ڪريو UNICODE_ساڄو_CURLY_CODEPOINT   0x007D
#وصف_ڪريو UNICODE_SPACE_CODEPOINT         0x0020
#وصف_ڪريو UCNV_PRV_ESCAPE_ICU         0
#وصف_ڪريو UCNV_PRV_ESCAPE_C           'C'
#وصف_ڪريو UCNV_PRV_ESCAPE_XML_DEC     'D'
#وصف_ڪريو UCNV_PRV_ESCAPE_XML_HEX     'X'
#وصف_ڪريو UCNV_PRV_ESCAPE_JAVA        'J'
#وصف_ڪريو UCNV_PRV_ESCAPE_UNICODE     'U'
#وصف_ڪريو UCNV_PRV_ESCAPE_CSS2        'S'
#وصف_ڪريو UCNV_PRV_STOP_ON_ILLEGAL    'i'

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
#وصف_ڪريو IS_DEFAULT_IGNORABLE_ڪوڊ_نقطو(c) ( \
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
                  UConverter_From_Unicode_دليلون *fromUArgs,
                  ثابت char16_t* ڪوڊ_Units,
                  int32_t length,
                  UChar32 ڪوڊ_نقطو,
                  UConverterCallbackReason reason,
                  UError_ڪوڊ * err)
{
    (فراغ)context;
    (فراغ)fromUArgs;
    (فراغ)ڪوڊ_Units;
    (فراغ)length;
    إذا (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_ڪوڊ_نقطو(ڪوڊ_نقطو))
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
UCNV_منزل_U_CALLBACK_STOP (
                   ثابت فراغ *context,
                   UConverter_منزل_Unicode_دليلون *منزل_UArgs,
                   ثابت حرف* ڪوڊ_Points,
                   int32_t length,
                   UConverterCallbackReason reason,
                   UError_ڪوڊ * err)
{
    /* the caller must have set the error code accordingly */
    (فراغ)context; (فراغ)منزل_UArgs; (فراغ)ڪوڊ_Points; (فراغ)length; (فراغ)reason; (فراغ)err;
}

U_CAPI فراغ    U_EXPORT2
UCNV_FROM_U_CALLBACK_SKIP (                  
                  ثابت فراغ *context,
                  UConverter_From_Unicode_دليلون *fromUArgs,
                  ثابت char16_t* ڪوڊ_Units,
                  int32_t length,
                  UChar32 ڪوڊ_نقطو,
                  UConverterCallbackReason reason,
                  UError_ڪوڊ * err)
{
    (فراغ)fromUArgs;
    (فراغ)ڪوڊ_Units;
    (فراغ)length;
    إذا (reason <= UCNV_IRREGULAR)
    {
        إذا (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_ڪوڊ_نقطو(ڪوڊ_نقطو))
        {
            /*
             * Skip if the codepoint has unicode property of default ignorable.
             */
            *err = U_ZERO_ERROR;
        }
        وإلا إذا (context == خالي_اشارو || (*((حرف*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
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
                  UConverter_From_Unicode_دليلون *from_دليلون,
                  ثابت char16_t* ڪوڊ_Units,
                  int32_t length,
                  UChar32 ڪوڊ_نقطو,
                  UConverterCallbackReason reason,
                  UError_ڪوڊ * err)
{
    (فراغ)ڪوڊ_Units;
    (فراغ)length;
    إذا (reason <= UCNV_IRREGULAR)
    {
        إذا (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_ڪوڊ_نقطو(ڪوڊ_نقطو))
        {
            /*
             * Skip if the codepoint has unicode property of default ignorable.
             */
            *err = U_ZERO_ERROR;
        }
        وإلا إذا (context == خالي_اشارو || (*((حرف*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
        {
            *err = U_ZERO_ERROR;
            ucnv_cbFromUWriteSub(from_دليلون, 0, err);
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
                         UConverter_From_Unicode_دليلون *from_دليلون,
                         ثابت char16_t *ڪوڊ_Units,
                         int32_t length,
                         UChar32 ڪوڊ_نقطو,
                         UConverterCallbackReason reason,
                         UError_ڪوڊ * err)
{

  char16_t قدر_String[قدر_STRING_ڊيگهائي];
  int32_t قدر_String_ڊيگهائي = 0;
  int32_t i = 0;

  ثابت char16_t *my_قدر_ذريعو = خالي_اشارو;
  UError_ڪوڊ err2 = U_ZERO_ERROR;
  UConverterFromUCallback original = خالي_اشارو;
  ثابت فراغ *originalContext;

  UConverterFromUCallback ignoredCallback = خالي_اشارو;
  ثابت فراغ *ignoredContext;
  
  إذا (reason > UCNV_IRREGULAR)
  {
      أرجع;
  }
  وإلا إذا (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_ڪوڊ_نقطو(ڪوڊ_نقطو))
  {
      /*
       * Skip if the codepoint has unicode property of default ignorable.
       */
      *err = U_ZERO_ERROR;
      أرجع;
  }

  ucnv_سيٽ_From_UCall_پٺ (from_دليلون->converter,
                     (UConverterFromUCallback) UCNV_FROM_U_CALLBACK_SUBSTITUTE,
                     خالي_اشارو,
                     &original,
                     &originalContext,
                     &err2);
  
  إذا (U_FAILURE (err2))
  {
    *err = err2;
    أرجع;
  } 
  إذا(context==خالي_اشارو)
  { 
      طالما (i < length)
      {
        قدر_String[قدر_String_ڊيگهائي++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT;  /* adding % */
        قدر_String[قدر_String_ڊيگهائي++] = (char16_t) UNICODE_U_CODEPOINT; /* adding U */
        قدر_String_ڊيگهائي += uprv_itou (قدر_String + قدر_String_ڊيگهائي, قدر_STRING_ڊيگهائي - قدر_String_ڊيگهائي, (uint16_t)ڪوڊ_Units[i++], 16, 4);
      }
  }
  وإلا
  {
      بدل(*((حرف*)context))
      {
      حالة UCNV_PRV_ESCAPE_JAVA:
          طالما (i < length)
          {
              قدر_String[قدر_String_ڊيگهائي++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */
              قدر_String[قدر_String_ڊيگهائي++] = (char16_t) UNICODE_U_هيٺيون_CODEPOINT; /* adding u */
              قدر_String_ڊيگهائي += uprv_itou (قدر_String + قدر_String_ڊيگهائي, قدر_STRING_ڊيگهائي - قدر_String_ڊيگهائي, (uint16_t)ڪوڊ_Units[i++], 16, 4);
          }
          اكسر;

      حالة UCNV_PRV_ESCAPE_C:
          قدر_String[قدر_String_ڊيگهائي++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */

          إذا(length==2){
              قدر_String[قدر_String_ڊيگهائي++] = (char16_t) UNICODE_U_CODEPOINT; /* adding U */
              قدر_String_ڊيگهائي += uprv_itou (قدر_String + قدر_String_ڊيگهائي, قدر_STRING_ڊيگهائي - قدر_String_ڊيگهائي, ڪوڊ_نقطو, 16, 8);

          }
          وإلا{
              قدر_String[قدر_String_ڊيگهائي++] = (char16_t) UNICODE_U_هيٺيون_CODEPOINT; /* adding u */
              قدر_String_ڊيگهائي += uprv_itou (قدر_String + قدر_String_ڊيگهائي, قدر_STRING_ڊيگهائي - قدر_String_ڊيگهائي, (uint16_t)ڪوڊ_Units[0], 16, 4);
          }
          اكسر;

      حالة UCNV_PRV_ESCAPE_XML_DEC:

          قدر_String[قدر_String_ڊيگهائي++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
          قدر_String[قدر_String_ڊيگهائي++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
          إذا(length==2){
              قدر_String_ڊيگهائي += uprv_itou (قدر_String + قدر_String_ڊيگهائي, قدر_STRING_ڊيگهائي - قدر_String_ڊيگهائي, ڪوڊ_نقطو, 10, 0);
          }
          وإلا{
              قدر_String_ڊيگهائي += uprv_itou (قدر_String + قدر_String_ڊيگهائي, قدر_STRING_ڊيگهائي - قدر_String_ڊيگهائي, (uint16_t)ڪوڊ_Units[0], 10, 0);
          }
          قدر_String[قدر_String_ڊيگهائي++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
          اكسر;

      حالة UCNV_PRV_ESCAPE_XML_HEX:

          قدر_String[قدر_String_ڊيگهائي++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
          قدر_String[قدر_String_ڊيگهائي++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
          قدر_String[قدر_String_ڊيگهائي++] = (char16_t) UNICODE_X_هيٺيون_CODEPOINT; /* adding x */
          إذا(length==2){
              قدر_String_ڊيگهائي += uprv_itou (قدر_String + قدر_String_ڊيگهائي, قدر_STRING_ڊيگهائي - قدر_String_ڊيگهائي, ڪوڊ_نقطو, 16, 0);
          }
          وإلا{
              قدر_String_ڊيگهائي += uprv_itou (قدر_String + قدر_String_ڊيگهائي, قدر_STRING_ڊيگهائي - قدر_String_ڊيگهائي, (uint16_t)ڪوڊ_Units[0], 16, 0);
          }
          قدر_String[قدر_String_ڊيگهائي++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
          اكسر;

      حالة UCNV_PRV_ESCAPE_UNICODE:
          قدر_String[قدر_String_ڊيگهائي++] = (char16_t) UNICODE_کاٻو_CURLY_CODEPOINT;    /* adding { */
          قدر_String[قدر_String_ڊيگهائي++] = (char16_t) UNICODE_U_CODEPOINT;    /* adding U */
          قدر_String[قدر_String_ڊيگهائي++] = (char16_t) UNICODE_PLUS_CODEPOINT; /* adding + */
          إذا (length == 2) {
              قدر_String_ڊيگهائي += uprv_itou (قدر_String + قدر_String_ڊيگهائي, قدر_STRING_ڊيگهائي - قدر_String_ڊيگهائي, ڪوڊ_نقطو, 16, 4);
          } وإلا {
              قدر_String_ڊيگهائي += uprv_itou (قدر_String + قدر_String_ڊيگهائي, قدر_STRING_ڊيگهائي - قدر_String_ڊيگهائي, (uint16_t)ڪوڊ_Units[0], 16, 4);
          }
          قدر_String[قدر_String_ڊيگهائي++] = (char16_t) UNICODE_ساڄو_CURLY_CODEPOINT;    /* adding } */
          اكسر;

      حالة UCNV_PRV_ESCAPE_CSS2:
          قدر_String[قدر_String_ڊيگهائي++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */
          قدر_String_ڊيگهائي += uprv_itou (قدر_String + قدر_String_ڊيگهائي, قدر_STRING_ڊيگهائي - قدر_String_ڊيگهائي, ڪوڊ_نقطو, 16, 0);
          /* Always add space character, because the next character might be whitespace,
             which would erroneously be considered the termination of the escape sequence. */
          قدر_String[قدر_String_ڊيگهائي++] = (char16_t) UNICODE_SPACE_CODEPOINT;
          اكسر;

      افتراضي:
          طالما (i < length)
          {
              قدر_String[قدر_String_ڊيگهائي++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT;  /* adding % */
              قدر_String[قدر_String_ڊيگهائي++] = (char16_t) UNICODE_U_CODEPOINT;             /* adding U */
              قدر_String_ڊيگهائي += uprv_itou (قدر_String + قدر_String_ڊيگهائي, قدر_STRING_ڊيگهائي - قدر_String_ڊيگهائي, (uint16_t)ڪوڊ_Units[i++], 16, 4);
          }
      }
  }  
  my_قدر_ذريعو = قدر_String;

  /* reset the error */
  *err = U_ZERO_ERROR;

  ucnv_cbFromUWriteUChars(from_دليلون, &my_قدر_ذريعو, my_قدر_ذريعو+قدر_String_ڊيگهائي, 0, err);

  ucnv_سيٽ_From_UCall_پٺ (from_دليلون->converter,
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
UCNV_منزل_U_CALLBACK_SKIP (
                 ثابت فراغ *context,
                 UConverter_منزل_Unicode_دليلون *منزل_دليلون,
                 ثابت حرف* ڪوڊ_Units,
                 int32_t length,
                 UConverterCallbackReason reason,
                 UError_ڪوڊ * err)
{
    (فراغ)منزل_دليلون;
    (فراغ)ڪوڊ_Units;
    (فراغ)length;
    إذا (reason <= UCNV_IRREGULAR)
    {
        إذا (context == خالي_اشارو || (*((حرف*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
        {
            *err = U_ZERO_ERROR;
        }
        /* else the caller must have set the error code accordingly. */
    }
    /* else ignore the reset, close and clone calls. */
}

U_CAPI فراغ    U_EXPORT2
UCNV_منزل_U_CALLBACK_SUBSTITUTE (
                 ثابت فراغ *context,
                 UConverter_منزل_Unicode_دليلون *منزل_دليلون,
                 ثابت حرف* ڪوڊ_Units,
                 int32_t length,
                 UConverterCallbackReason reason,
                 UError_ڪوڊ * err)
{
    (فراغ)ڪوڊ_Units;
    (فراغ)length;
    إذا (reason <= UCNV_IRREGULAR)
    {
        إذا (context == خالي_اشارو || (*((حرف*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
        {
            *err = U_ZERO_ERROR;
            ucnv_cb_منزل_UWrite_Sub(منزل_دليلون,0,err);
        }
        /* else the caller must have set the error code accordingly. */
    }
    /* else ignore the reset, close and clone calls. */
}

/*uses uprv_itou to get a unicode escape sequence of the offensive sequence,
 *and uses that as the substitution sequence
 */
U_CAPI فراغ   U_EXPORT2
UCNV_منزل_U_CALLBACK_ESCAPE (
                 ثابت فراغ *context,
                 UConverter_منزل_Unicode_دليلون *منزل_دليلون,
                 ثابت حرف* ڪوڊ_Units,
                 int32_t length,
                 UConverterCallbackReason reason,
                 UError_ڪوڊ * err)
{
    char16_t uni_قدر_String[قدر_STRING_ڊيگهائي];
    int32_t قدر_String_ڊيگهائي = 0;
    int32_t i = 0;

    إذا (reason > UCNV_IRREGULAR)
    {
        أرجع;
    }

    إذا(context==خالي_اشارو)
    {    
        طالما (i < length)
        {
            uni_قدر_String[قدر_String_ڊيگهائي++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT; /* adding % */
            uni_قدر_String[قدر_String_ڊيگهائي++] = (char16_t) UNICODE_X_CODEPOINT;    /* adding X */
            قدر_String_ڊيگهائي += uprv_itou (uni_قدر_String + قدر_String_ڊيگهائي, قدر_STRING_ڊيگهائي - قدر_String_ڊيگهائي, (uint8_t) ڪوڊ_Units[i++], 16, 2);
        }
    }
    وإلا
    {
        بدل(*((حرف*)context))
        {
        حالة UCNV_PRV_ESCAPE_XML_DEC:
            طالما (i < length)
            {
                uni_قدر_String[قدر_String_ڊيگهائي++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
                uni_قدر_String[قدر_String_ڊيگهائي++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
                قدر_String_ڊيگهائي += uprv_itou (uni_قدر_String + قدر_String_ڊيگهائي, قدر_STRING_ڊيگهائي - قدر_String_ڊيگهائي, (uint8_t)ڪوڊ_Units[i++], 10, 0);
                uni_قدر_String[قدر_String_ڊيگهائي++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
            }
            اكسر;

        حالة UCNV_PRV_ESCAPE_XML_HEX:
            طالما (i < length)
            {
                uni_قدر_String[قدر_String_ڊيگهائي++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
                uni_قدر_String[قدر_String_ڊيگهائي++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
                uni_قدر_String[قدر_String_ڊيگهائي++] = (char16_t) UNICODE_X_هيٺيون_CODEPOINT; /* adding x */
                قدر_String_ڊيگهائي += uprv_itou (uni_قدر_String + قدر_String_ڊيگهائي, قدر_STRING_ڊيگهائي - قدر_String_ڊيگهائي, (uint8_t)ڪوڊ_Units[i++], 16, 0);
                uni_قدر_String[قدر_String_ڊيگهائي++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
            }
            اكسر;
        حالة UCNV_PRV_ESCAPE_C:
            طالما (i < length)
            {
                uni_قدر_String[قدر_String_ڊيگهائي++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */
                uni_قدر_String[قدر_String_ڊيگهائي++] = (char16_t) UNICODE_X_هيٺيون_CODEPOINT; /* adding x */
                قدر_String_ڊيگهائي += uprv_itou (uni_قدر_String + قدر_String_ڊيگهائي, قدر_STRING_ڊيگهائي - قدر_String_ڊيگهائي, (uint8_t)ڪوڊ_Units[i++], 16, 2);
            }
            اكسر;
        افتراضي:
            طالما (i < length)
            {
                uni_قدر_String[قدر_String_ڊيگهائي++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT; /* adding % */
                uni_قدر_String[قدر_String_ڊيگهائي++] = (char16_t) UNICODE_X_CODEPOINT;    /* adding X */
                uprv_itou (uni_قدر_String + قدر_String_ڊيگهائي, قدر_STRING_ڊيگهائي - قدر_String_ڊيگهائي, (uint8_t) ڪوڊ_Units[i++], 16, 2);
                قدر_String_ڊيگهائي += 2;
            }
        }
    }
    /* reset the error */
    *err = U_ZERO_ERROR;

    ucnv_cb_منزل_UWrite_UChars(منزل_دليلون, uni_قدر_String, قدر_String_ڊيگهائي, 0, err);
}

#جيڪڏهن_پورو
