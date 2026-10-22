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

#অন্তর্ভাৱ "unicode/utypes.h"

#যদি !UCONFIG_NO_CONVERSION

#অন্তর্ভাৱ "unicode/ucnv_err.h"
#অন্তর্ভাৱ "unicode/ucnv_cb.h"
#অন্তর্ভাৱ "ucnv_cnv.h"
#অন্তর্ভাৱ "cmemory.h"
#অন্তর্ভাৱ "unicode/ucnv.h"
#অন্তর্ভাৱ "ustrfmt.h"

#define মান_STRING_LENGTH 48
/*Magic # 32 = 4(number of char in value string) * 8(max number of bytes per char for any converter) */
#define UNICODE_PERCENT_SIGN_CODEPOINT  0x0025
#define UNICODE_U_CODEPOINT             0x0055
#define UNICODE_X_CODEPOINT             0x0058
#define UNICODE_RS_CODEPOINT            0x005C
#define UNICODE_U_নিচেৰ_CODEPOINT         0x0075
#define UNICODE_X_নিচেৰ_CODEPOINT         0x0078
#define UNICODE_AMP_CODEPOINT           0x0026
#define UNICODE_HASH_CODEPOINT          0x0023
#define UNICODE_SEMICOLON_CODEPOINT     0x003B
#define UNICODE_PLUS_CODEPOINT          0x002B
#define UNICODE_বাম_CURLY_CODEPOINT    0x007B
#define UNICODE_ডান_CURLY_CODEPOINT   0x007D
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
U_CAPI শূন্য    U_EXPORT2
UCNV_FROM_U_CALLBACK_STOP (
                  ধ্রুৱ শূন্য *context,
                  UConverterFromUnicodeArgs *fromUArgs,
                  ধ্রুৱ char16_t* codeUnits,
                  int32_t length,
                  UChar32 codePoint,
                  UConverterCallbackReason reason,
                  UErrorCode * err)
{
    (শূন্য)context;
    (শূন্য)fromUArgs;
    (শূন্য)codeUnits;
    (শূন্য)length;
    যদি (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_CODE_POINT(codePoint))
    {
        /*
         * Skip if the codepoint has unicode property of default ignorable.
         */
        *err = U_ZERO_ERROR;
    }
    /* the caller must have set the error code accordingly */
}


/*Function Pointer STOPS at the ILLEGAL_SEQUENCE */
U_CAPI শূন্য    U_EXPORT2
UCNV_TO_U_CALLBACK_STOP (
                   ধ্রুৱ শূন্য *context,
                   UConverterToUnicodeArgs *toUArgs,
                   ধ্রুৱ ৱর্ণ* codePoints,
                   int32_t length,
                   UConverterCallbackReason reason,
                   UErrorCode * err)
{
    /* the caller must have set the error code accordingly */
    (শূন্য)context; (শূন্য)toUArgs; (শূন্য)codePoints; (শূন্য)length; (শূন্য)reason; (শূন্য)err;
}

U_CAPI শূন্য    U_EXPORT2
UCNV_FROM_U_CALLBACK_SKIP (                  
                  ধ্রুৱ শূন্য *context,
                  UConverterFromUnicodeArgs *fromUArgs,
                  ধ্রুৱ char16_t* codeUnits,
                  int32_t length,
                  UChar32 codePoint,
                  UConverterCallbackReason reason,
                  UErrorCode * err)
{
    (শূন্য)fromUArgs;
    (শূন্য)codeUnits;
    (শূন্য)length;
    যদি (reason <= UCNV_IRREGULAR)
    {
        যদি (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_CODE_POINT(codePoint))
        {
            /*
             * Skip if the codepoint has unicode property of default ignorable.
             */
            *err = U_ZERO_ERROR;
        }
        অন্যথা যদি (context == nullptr || (*((ৱর্ণ*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
        {
            *err = U_ZERO_ERROR;
        }
        /* else the caller must have set the error code accordingly. */
    }
    /* else ignore the reset, close and clone calls. */
}

U_CAPI শূন্য    U_EXPORT2
UCNV_FROM_U_CALLBACK_SUBSTITUTE (
                  ধ্রুৱ শূন্য *context,
                  UConverterFromUnicodeArgs *fromArgs,
                  ধ্রুৱ char16_t* codeUnits,
                  int32_t length,
                  UChar32 codePoint,
                  UConverterCallbackReason reason,
                  UErrorCode * err)
{
    (শূন্য)codeUnits;
    (শূন্য)length;
    যদি (reason <= UCNV_IRREGULAR)
    {
        যদি (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_CODE_POINT(codePoint))
        {
            /*
             * Skip if the codepoint has unicode property of default ignorable.
             */
            *err = U_ZERO_ERROR;
        }
        অন্যথা যদি (context == nullptr || (*((ৱর্ণ*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
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
U_CAPI শূন্য    U_EXPORT2
UCNV_FROM_U_CALLBACK_ESCAPE (
                         ধ্রুৱ শূন্য *context,
                         UConverterFromUnicodeArgs *fromArgs,
                         ধ্রুৱ char16_t *codeUnits,
                         int32_t length,
                         UChar32 codePoint,
                         UConverterCallbackReason reason,
                         UErrorCode * err)
{

  char16_t মান_String[মান_STRING_LENGTH];
  int32_t মান_String_Length = 0;
  int32_t i = 0;

  ধ্রুৱ char16_t *my_মান_Source = nullptr;
  UErrorCode err2 = U_ZERO_ERROR;
  UConverterFromUCallback original = nullptr;
  ধ্রুৱ শূন্য *originalContext;

  UConverterFromUCallback ignoredCallback = nullptr;
  ধ্রুৱ শূন্য *ignoredContext;
  
  যদি (reason > UCNV_IRREGULAR)
  {
      ঘূৰাই;
  }
  অন্যথা যদি (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_CODE_POINT(codePoint))
  {
      /*
       * Skip if the codepoint has unicode property of default ignorable.
       */
      *err = U_ZERO_ERROR;
      ঘূৰাই;
  }

  ucnv_setFromUCallBack (fromArgs->converter,
                     (UConverterFromUCallback) UCNV_FROM_U_CALLBACK_SUBSTITUTE,
                     nullptr,
                     &original,
                     &originalContext,
                     &err2);
  
  যদি (U_FAILURE (err2))
  {
    *err = err2;
    ঘূৰাই;
  } 
  যদি(context==nullptr)
  { 
      যেতিয়ালৈ (i < length)
      {
        মান_String[মান_String_Length++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT;  /* adding % */
        মান_String[মান_String_Length++] = (char16_t) UNICODE_U_CODEPOINT; /* adding U */
        মান_String_Length += uprv_itou (মান_String + মান_String_Length, মান_STRING_LENGTH - মান_String_Length, (uint16_t)codeUnits[i++], 16, 4);
      }
  }
  অন্যথা
  {
      ৱিকল্প(*((ৱর্ণ*)context))
      {
      পক্ষ UCNV_PRV_ESCAPE_JAVA:
          যেতিয়ালৈ (i < length)
          {
              মান_String[মান_String_Length++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */
              মান_String[মান_String_Length++] = (char16_t) UNICODE_U_নিচেৰ_CODEPOINT; /* adding u */
              মান_String_Length += uprv_itou (মান_String + মান_String_Length, মান_STRING_LENGTH - মান_String_Length, (uint16_t)codeUnits[i++], 16, 4);
          }
          ভঙ্গ;

      পক্ষ UCNV_PRV_ESCAPE_C:
          মান_String[মান_String_Length++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */

          যদি(length==2){
              মান_String[মান_String_Length++] = (char16_t) UNICODE_U_CODEPOINT; /* adding U */
              মান_String_Length += uprv_itou (মান_String + মান_String_Length, মান_STRING_LENGTH - মান_String_Length, codePoint, 16, 8);

          }
          অন্যথা{
              মান_String[মান_String_Length++] = (char16_t) UNICODE_U_নিচেৰ_CODEPOINT; /* adding u */
              মান_String_Length += uprv_itou (মান_String + মান_String_Length, মান_STRING_LENGTH - মান_String_Length, (uint16_t)codeUnits[0], 16, 4);
          }
          ভঙ্গ;

      পক্ষ UCNV_PRV_ESCAPE_XML_DEC:

          মান_String[মান_String_Length++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
          মান_String[মান_String_Length++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
          যদি(length==2){
              মান_String_Length += uprv_itou (মান_String + মান_String_Length, মান_STRING_LENGTH - মান_String_Length, codePoint, 10, 0);
          }
          অন্যথা{
              মান_String_Length += uprv_itou (মান_String + মান_String_Length, মান_STRING_LENGTH - মান_String_Length, (uint16_t)codeUnits[0], 10, 0);
          }
          মান_String[মান_String_Length++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
          ভঙ্গ;

      পক্ষ UCNV_PRV_ESCAPE_XML_HEX:

          মান_String[মান_String_Length++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
          মান_String[মান_String_Length++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
          মান_String[মান_String_Length++] = (char16_t) UNICODE_X_নিচেৰ_CODEPOINT; /* adding x */
          যদি(length==2){
              মান_String_Length += uprv_itou (মান_String + মান_String_Length, মান_STRING_LENGTH - মান_String_Length, codePoint, 16, 0);
          }
          অন্যথা{
              মান_String_Length += uprv_itou (মান_String + মান_String_Length, মান_STRING_LENGTH - মান_String_Length, (uint16_t)codeUnits[0], 16, 0);
          }
          মান_String[মান_String_Length++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
          ভঙ্গ;

      পক্ষ UCNV_PRV_ESCAPE_UNICODE:
          মান_String[মান_String_Length++] = (char16_t) UNICODE_বাম_CURLY_CODEPOINT;    /* adding { */
          মান_String[মান_String_Length++] = (char16_t) UNICODE_U_CODEPOINT;    /* adding U */
          মান_String[মান_String_Length++] = (char16_t) UNICODE_PLUS_CODEPOINT; /* adding + */
          যদি (length == 2) {
              মান_String_Length += uprv_itou (মান_String + মান_String_Length, মান_STRING_LENGTH - মান_String_Length, codePoint, 16, 4);
          } অন্যথা {
              মান_String_Length += uprv_itou (মান_String + মান_String_Length, মান_STRING_LENGTH - মান_String_Length, (uint16_t)codeUnits[0], 16, 4);
          }
          মান_String[মান_String_Length++] = (char16_t) UNICODE_ডান_CURLY_CODEPOINT;    /* adding } */
          ভঙ্গ;

      পক্ষ UCNV_PRV_ESCAPE_CSS2:
          মান_String[মান_String_Length++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */
          মান_String_Length += uprv_itou (মান_String + মান_String_Length, মান_STRING_LENGTH - মান_String_Length, codePoint, 16, 0);
          /* Always add space character, because the next character might be whitespace,
             which would erroneously be considered the termination of the escape sequence. */
          মান_String[মান_String_Length++] = (char16_t) UNICODE_SPACE_CODEPOINT;
          ভঙ্গ;

      স্ৱভাৱ:
          যেতিয়ালৈ (i < length)
          {
              মান_String[মান_String_Length++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT;  /* adding % */
              মান_String[মান_String_Length++] = (char16_t) UNICODE_U_CODEPOINT;             /* adding U */
              মান_String_Length += uprv_itou (মান_String + মান_String_Length, মান_STRING_LENGTH - মান_String_Length, (uint16_t)codeUnits[i++], 16, 4);
          }
      }
  }  
  my_মান_Source = মান_String;

  /* reset the error */
  *err = U_ZERO_ERROR;

  ucnv_cbFromUWriteUChars(fromArgs, &my_মান_Source, my_মান_Source+মান_String_Length, 0, err);

  ucnv_setFromUCallBack (fromArgs->converter,
                         original,
                         originalContext,
                         &ignoredCallback,
                         &ignoredContext,
                         &err2);
  যদি (U_FAILURE (err2))
  {
      *err = err2;
      ঘূৰাই;
  }
}



U_CAPI শূন্য  U_EXPORT2
UCNV_TO_U_CALLBACK_SKIP (
                 ধ্রুৱ শূন্য *context,
                 UConverterToUnicodeArgs *toArgs,
                 ধ্রুৱ ৱর্ণ* codeUnits,
                 int32_t length,
                 UConverterCallbackReason reason,
                 UErrorCode * err)
{
    (শূন্য)toArgs;
    (শূন্য)codeUnits;
    (শূন্য)length;
    যদি (reason <= UCNV_IRREGULAR)
    {
        যদি (context == nullptr || (*((ৱর্ণ*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
        {
            *err = U_ZERO_ERROR;
        }
        /* else the caller must have set the error code accordingly. */
    }
    /* else ignore the reset, close and clone calls. */
}

U_CAPI শূন্য    U_EXPORT2
UCNV_TO_U_CALLBACK_SUBSTITUTE (
                 ধ্রুৱ শূন্য *context,
                 UConverterToUnicodeArgs *toArgs,
                 ধ্রুৱ ৱর্ণ* codeUnits,
                 int32_t length,
                 UConverterCallbackReason reason,
                 UErrorCode * err)
{
    (শূন্য)codeUnits;
    (শূন্য)length;
    যদি (reason <= UCNV_IRREGULAR)
    {
        যদি (context == nullptr || (*((ৱর্ণ*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
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
U_CAPI শূন্য   U_EXPORT2
UCNV_TO_U_CALLBACK_ESCAPE (
                 ধ্রুৱ শূন্য *context,
                 UConverterToUnicodeArgs *toArgs,
                 ধ্রুৱ ৱর্ণ* codeUnits,
                 int32_t length,
                 UConverterCallbackReason reason,
                 UErrorCode * err)
{
    char16_t uni_মান_String[মান_STRING_LENGTH];
    int32_t মান_String_Length = 0;
    int32_t i = 0;

    যদি (reason > UCNV_IRREGULAR)
    {
        ঘূৰাই;
    }

    যদি(context==nullptr)
    {    
        যেতিয়ালৈ (i < length)
        {
            uni_মান_String[মান_String_Length++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT; /* adding % */
            uni_মান_String[মান_String_Length++] = (char16_t) UNICODE_X_CODEPOINT;    /* adding X */
            মান_String_Length += uprv_itou (uni_মান_String + মান_String_Length, মান_STRING_LENGTH - মান_String_Length, (uint8_t) codeUnits[i++], 16, 2);
        }
    }
    অন্যথা
    {
        ৱিকল্প(*((ৱর্ণ*)context))
        {
        পক্ষ UCNV_PRV_ESCAPE_XML_DEC:
            যেতিয়ালৈ (i < length)
            {
                uni_মান_String[মান_String_Length++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
                uni_মান_String[মান_String_Length++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
                মান_String_Length += uprv_itou (uni_মান_String + মান_String_Length, মান_STRING_LENGTH - মান_String_Length, (uint8_t)codeUnits[i++], 10, 0);
                uni_মান_String[মান_String_Length++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
            }
            ভঙ্গ;

        পক্ষ UCNV_PRV_ESCAPE_XML_HEX:
            যেতিয়ালৈ (i < length)
            {
                uni_মান_String[মান_String_Length++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
                uni_মান_String[মান_String_Length++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
                uni_মান_String[মান_String_Length++] = (char16_t) UNICODE_X_নিচেৰ_CODEPOINT; /* adding x */
                মান_String_Length += uprv_itou (uni_মান_String + মান_String_Length, মান_STRING_LENGTH - মান_String_Length, (uint8_t)codeUnits[i++], 16, 0);
                uni_মান_String[মান_String_Length++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
            }
            ভঙ্গ;
        পক্ষ UCNV_PRV_ESCAPE_C:
            যেতিয়ালৈ (i < length)
            {
                uni_মান_String[মান_String_Length++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */
                uni_মান_String[মান_String_Length++] = (char16_t) UNICODE_X_নিচেৰ_CODEPOINT; /* adding x */
                মান_String_Length += uprv_itou (uni_মান_String + মান_String_Length, মান_STRING_LENGTH - মান_String_Length, (uint8_t)codeUnits[i++], 16, 2);
            }
            ভঙ্গ;
        স্ৱভাৱ:
            যেতিয়ালৈ (i < length)
            {
                uni_মান_String[মান_String_Length++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT; /* adding % */
                uni_মান_String[মান_String_Length++] = (char16_t) UNICODE_X_CODEPOINT;    /* adding X */
                uprv_itou (uni_মান_String + মান_String_Length, মান_STRING_LENGTH - মান_String_Length, (uint8_t) codeUnits[i++], 16, 2);
                মান_String_Length += 2;
            }
        }
    }
    /* reset the error */
    *err = U_ZERO_ERROR;

    ucnv_cbToUWriteUChars(toArgs, uni_মান_String, মান_String_Length, 0, err);
}

#endif
