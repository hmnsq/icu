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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି !UCONFIG_NO_CONVERSION

#ଅନ୍ତର୍ଭାଵ "unicode/ucnv_err.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ucnv_cb.h"
#ଅନ୍ତର୍ଭାଵ "ucnv_cnv.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ucnv.h"
#ଅନ୍ତର୍ଭାଵ "ustrfmt.h"

#define ମୂଲ୍ୟ_STRING_LENGTH 48
/*Magic # 32 = 4(number of char in value string) * 8(max number of bytes per char for any converter) */
#define UNICODE_PERCENT_SIGN_CODEPOINT  0x0025
#define UNICODE_U_CODEPOINT             0x0055
#define UNICODE_X_CODEPOINT             0x0058
#define UNICODE_RS_CODEPOINT            0x005C
#define UNICODE_U_ତଳ_CODEPOINT         0x0075
#define UNICODE_X_ତଳ_CODEPOINT         0x0078
#define UNICODE_AMP_CODEPOINT           0x0026
#define UNICODE_HASH_CODEPOINT          0x0023
#define UNICODE_SEMICOLON_CODEPOINT     0x003B
#define UNICODE_PLUS_CODEPOINT          0x002B
#define UNICODE_ବାମ_CURLY_CODEPOINT    0x007B
#define UNICODE_ଡାହାଣ_CURLY_CODEPOINT   0x007D
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
U_CAPI ଶୂନ୍ୟ    U_EXPORT2
UCNV_FROM_U_CALLBACK_STOP (
                  ଅଚଳ ଶୂନ୍ୟ *context,
                  UConverterFromUnicodeArgs *fromUArgs,
                  ଅଚଳ char16_t* codeUnits,
                  int32_t length,
                  UChar32 codePoint,
                  UConverterCallbackReason reason,
                  UErrorCode * err)
{
    (ଶୂନ୍ୟ)context;
    (ଶୂନ୍ୟ)fromUArgs;
    (ଶୂନ୍ୟ)codeUnits;
    (ଶୂନ୍ୟ)length;
    ଯଦି (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_CODE_POINT(codePoint))
    {
        /*
         * Skip if the codepoint has unicode property of default ignorable.
         */
        *err = U_ZERO_ERROR;
    }
    /* the caller must have set the error code accordingly */
}


/*Function Pointer STOPS at the ILLEGAL_SEQUENCE */
U_CAPI ଶୂନ୍ୟ    U_EXPORT2
UCNV_TO_U_CALLBACK_STOP (
                   ଅଚଳ ଶୂନ୍ୟ *context,
                   UConverterToUnicodeArgs *toUArgs,
                   ଅଚଳ ଅକ୍ଷର* codePoints,
                   int32_t length,
                   UConverterCallbackReason reason,
                   UErrorCode * err)
{
    /* the caller must have set the error code accordingly */
    (ଶୂନ୍ୟ)context; (ଶୂନ୍ୟ)toUArgs; (ଶୂନ୍ୟ)codePoints; (ଶୂନ୍ୟ)length; (ଶୂନ୍ୟ)reason; (ଶୂନ୍ୟ)err;
}

U_CAPI ଶୂନ୍ୟ    U_EXPORT2
UCNV_FROM_U_CALLBACK_SKIP (                  
                  ଅଚଳ ଶୂନ୍ୟ *context,
                  UConverterFromUnicodeArgs *fromUArgs,
                  ଅଚଳ char16_t* codeUnits,
                  int32_t length,
                  UChar32 codePoint,
                  UConverterCallbackReason reason,
                  UErrorCode * err)
{
    (ଶୂନ୍ୟ)fromUArgs;
    (ଶୂନ୍ୟ)codeUnits;
    (ଶୂନ୍ୟ)length;
    ଯଦି (reason <= UCNV_IRREGULAR)
    {
        ଯଦି (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_CODE_POINT(codePoint))
        {
            /*
             * Skip if the codepoint has unicode property of default ignorable.
             */
            *err = U_ZERO_ERROR;
        }
        ଅନ୍ୟଥା ଯଦି (context == nullptr || (*((ଅକ୍ଷର*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
        {
            *err = U_ZERO_ERROR;
        }
        /* else the caller must have set the error code accordingly. */
    }
    /* else ignore the reset, close and clone calls. */
}

U_CAPI ଶୂନ୍ୟ    U_EXPORT2
UCNV_FROM_U_CALLBACK_SUBSTITUTE (
                  ଅଚଳ ଶୂନ୍ୟ *context,
                  UConverterFromUnicodeArgs *fromArgs,
                  ଅଚଳ char16_t* codeUnits,
                  int32_t length,
                  UChar32 codePoint,
                  UConverterCallbackReason reason,
                  UErrorCode * err)
{
    (ଶୂନ୍ୟ)codeUnits;
    (ଶୂନ୍ୟ)length;
    ଯଦି (reason <= UCNV_IRREGULAR)
    {
        ଯଦି (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_CODE_POINT(codePoint))
        {
            /*
             * Skip if the codepoint has unicode property of default ignorable.
             */
            *err = U_ZERO_ERROR;
        }
        ଅନ୍ୟଥା ଯଦି (context == nullptr || (*((ଅକ୍ଷର*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
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
U_CAPI ଶୂନ୍ୟ    U_EXPORT2
UCNV_FROM_U_CALLBACK_ESCAPE (
                         ଅଚଳ ଶୂନ୍ୟ *context,
                         UConverterFromUnicodeArgs *fromArgs,
                         ଅଚଳ char16_t *codeUnits,
                         int32_t length,
                         UChar32 codePoint,
                         UConverterCallbackReason reason,
                         UErrorCode * err)
{

  char16_t ମୂଲ୍ୟ_String[ମୂଲ୍ୟ_STRING_LENGTH];
  int32_t ମୂଲ୍ୟ_String_Length = 0;
  int32_t i = 0;

  ଅଚଳ char16_t *my_ମୂଲ୍ୟ_Source = nullptr;
  UErrorCode err2 = U_ZERO_ERROR;
  UConverterFromUCallback original = nullptr;
  ଅଚଳ ଶୂନ୍ୟ *originalContext;

  UConverterFromUCallback ignoredCallback = nullptr;
  ଅଚଳ ଶୂନ୍ୟ *ignoredContext;
  
  ଯଦି (reason > UCNV_IRREGULAR)
  {
      ଫେରନ୍ତୁ;
  }
  ଅନ୍ୟଥା ଯଦି (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_CODE_POINT(codePoint))
  {
      /*
       * Skip if the codepoint has unicode property of default ignorable.
       */
      *err = U_ZERO_ERROR;
      ଫେରନ୍ତୁ;
  }

  ucnv_setFromUCallBack (fromArgs->converter,
                     (UConverterFromUCallback) UCNV_FROM_U_CALLBACK_SUBSTITUTE,
                     nullptr,
                     &original,
                     &originalContext,
                     &err2);
  
  ଯଦି (U_FAILURE (err2))
  {
    *err = err2;
    ଫେରନ୍ତୁ;
  } 
  ଯଦି(context==nullptr)
  { 
      ଯେତେବେଳେ (i < length)
      {
        ମୂଲ୍ୟ_String[ମୂଲ୍ୟ_String_Length++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT;  /* adding % */
        ମୂଲ୍ୟ_String[ମୂଲ୍ୟ_String_Length++] = (char16_t) UNICODE_U_CODEPOINT; /* adding U */
        ମୂଲ୍ୟ_String_Length += uprv_itou (ମୂଲ୍ୟ_String + ମୂଲ୍ୟ_String_Length, ମୂଲ୍ୟ_STRING_LENGTH - ମୂଲ୍ୟ_String_Length, (uint16_t)codeUnits[i++], 16, 4);
      }
  }
  ଅନ୍ୟଥା
  {
      ବାଛ(*((ଅକ୍ଷର*)context))
      {
      ସ୍ଥିତି UCNV_PRV_ESCAPE_JAVA:
          ଯେତେବେଳେ (i < length)
          {
              ମୂଲ୍ୟ_String[ମୂଲ୍ୟ_String_Length++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */
              ମୂଲ୍ୟ_String[ମୂଲ୍ୟ_String_Length++] = (char16_t) UNICODE_U_ତଳ_CODEPOINT; /* adding u */
              ମୂଲ୍ୟ_String_Length += uprv_itou (ମୂଲ୍ୟ_String + ମୂଲ୍ୟ_String_Length, ମୂଲ୍ୟ_STRING_LENGTH - ମୂଲ୍ୟ_String_Length, (uint16_t)codeUnits[i++], 16, 4);
          }
          ଭାଙ୍ଗ;

      ସ୍ଥିତି UCNV_PRV_ESCAPE_C:
          ମୂଲ୍ୟ_String[ମୂଲ୍ୟ_String_Length++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */

          ଯଦି(length==2){
              ମୂଲ୍ୟ_String[ମୂଲ୍ୟ_String_Length++] = (char16_t) UNICODE_U_CODEPOINT; /* adding U */
              ମୂଲ୍ୟ_String_Length += uprv_itou (ମୂଲ୍ୟ_String + ମୂଲ୍ୟ_String_Length, ମୂଲ୍ୟ_STRING_LENGTH - ମୂଲ୍ୟ_String_Length, codePoint, 16, 8);

          }
          ଅନ୍ୟଥା{
              ମୂଲ୍ୟ_String[ମୂଲ୍ୟ_String_Length++] = (char16_t) UNICODE_U_ତଳ_CODEPOINT; /* adding u */
              ମୂଲ୍ୟ_String_Length += uprv_itou (ମୂଲ୍ୟ_String + ମୂଲ୍ୟ_String_Length, ମୂଲ୍ୟ_STRING_LENGTH - ମୂଲ୍ୟ_String_Length, (uint16_t)codeUnits[0], 16, 4);
          }
          ଭାଙ୍ଗ;

      ସ୍ଥିତି UCNV_PRV_ESCAPE_XML_DEC:

          ମୂଲ୍ୟ_String[ମୂଲ୍ୟ_String_Length++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
          ମୂଲ୍ୟ_String[ମୂଲ୍ୟ_String_Length++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
          ଯଦି(length==2){
              ମୂଲ୍ୟ_String_Length += uprv_itou (ମୂଲ୍ୟ_String + ମୂଲ୍ୟ_String_Length, ମୂଲ୍ୟ_STRING_LENGTH - ମୂଲ୍ୟ_String_Length, codePoint, 10, 0);
          }
          ଅନ୍ୟଥା{
              ମୂଲ୍ୟ_String_Length += uprv_itou (ମୂଲ୍ୟ_String + ମୂଲ୍ୟ_String_Length, ମୂଲ୍ୟ_STRING_LENGTH - ମୂଲ୍ୟ_String_Length, (uint16_t)codeUnits[0], 10, 0);
          }
          ମୂଲ୍ୟ_String[ମୂଲ୍ୟ_String_Length++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
          ଭାଙ୍ଗ;

      ସ୍ଥିତି UCNV_PRV_ESCAPE_XML_HEX:

          ମୂଲ୍ୟ_String[ମୂଲ୍ୟ_String_Length++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
          ମୂଲ୍ୟ_String[ମୂଲ୍ୟ_String_Length++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
          ମୂଲ୍ୟ_String[ମୂଲ୍ୟ_String_Length++] = (char16_t) UNICODE_X_ତଳ_CODEPOINT; /* adding x */
          ଯଦି(length==2){
              ମୂଲ୍ୟ_String_Length += uprv_itou (ମୂଲ୍ୟ_String + ମୂଲ୍ୟ_String_Length, ମୂଲ୍ୟ_STRING_LENGTH - ମୂଲ୍ୟ_String_Length, codePoint, 16, 0);
          }
          ଅନ୍ୟଥା{
              ମୂଲ୍ୟ_String_Length += uprv_itou (ମୂଲ୍ୟ_String + ମୂଲ୍ୟ_String_Length, ମୂଲ୍ୟ_STRING_LENGTH - ମୂଲ୍ୟ_String_Length, (uint16_t)codeUnits[0], 16, 0);
          }
          ମୂଲ୍ୟ_String[ମୂଲ୍ୟ_String_Length++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
          ଭାଙ୍ଗ;

      ସ୍ଥିତି UCNV_PRV_ESCAPE_UNICODE:
          ମୂଲ୍ୟ_String[ମୂଲ୍ୟ_String_Length++] = (char16_t) UNICODE_ବାମ_CURLY_CODEPOINT;    /* adding { */
          ମୂଲ୍ୟ_String[ମୂଲ୍ୟ_String_Length++] = (char16_t) UNICODE_U_CODEPOINT;    /* adding U */
          ମୂଲ୍ୟ_String[ମୂଲ୍ୟ_String_Length++] = (char16_t) UNICODE_PLUS_CODEPOINT; /* adding + */
          ଯଦି (length == 2) {
              ମୂଲ୍ୟ_String_Length += uprv_itou (ମୂଲ୍ୟ_String + ମୂଲ୍ୟ_String_Length, ମୂଲ୍ୟ_STRING_LENGTH - ମୂଲ୍ୟ_String_Length, codePoint, 16, 4);
          } ଅନ୍ୟଥା {
              ମୂଲ୍ୟ_String_Length += uprv_itou (ମୂଲ୍ୟ_String + ମୂଲ୍ୟ_String_Length, ମୂଲ୍ୟ_STRING_LENGTH - ମୂଲ୍ୟ_String_Length, (uint16_t)codeUnits[0], 16, 4);
          }
          ମୂଲ୍ୟ_String[ମୂଲ୍ୟ_String_Length++] = (char16_t) UNICODE_ଡାହାଣ_CURLY_CODEPOINT;    /* adding } */
          ଭାଙ୍ଗ;

      ସ୍ଥିତି UCNV_PRV_ESCAPE_CSS2:
          ମୂଲ୍ୟ_String[ମୂଲ୍ୟ_String_Length++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */
          ମୂଲ୍ୟ_String_Length += uprv_itou (ମୂଲ୍ୟ_String + ମୂଲ୍ୟ_String_Length, ମୂଲ୍ୟ_STRING_LENGTH - ମୂଲ୍ୟ_String_Length, codePoint, 16, 0);
          /* Always add space character, because the next character might be whitespace,
             which would erroneously be considered the termination of the escape sequence. */
          ମୂଲ୍ୟ_String[ମୂଲ୍ୟ_String_Length++] = (char16_t) UNICODE_SPACE_CODEPOINT;
          ଭାଙ୍ଗ;

      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
          ଯେତେବେଳେ (i < length)
          {
              ମୂଲ୍ୟ_String[ମୂଲ୍ୟ_String_Length++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT;  /* adding % */
              ମୂଲ୍ୟ_String[ମୂଲ୍ୟ_String_Length++] = (char16_t) UNICODE_U_CODEPOINT;             /* adding U */
              ମୂଲ୍ୟ_String_Length += uprv_itou (ମୂଲ୍ୟ_String + ମୂଲ୍ୟ_String_Length, ମୂଲ୍ୟ_STRING_LENGTH - ମୂଲ୍ୟ_String_Length, (uint16_t)codeUnits[i++], 16, 4);
          }
      }
  }  
  my_ମୂଲ୍ୟ_Source = ମୂଲ୍ୟ_String;

  /* reset the error */
  *err = U_ZERO_ERROR;

  ucnv_cbFromUWriteUChars(fromArgs, &my_ମୂଲ୍ୟ_Source, my_ମୂଲ୍ୟ_Source+ମୂଲ୍ୟ_String_Length, 0, err);

  ucnv_setFromUCallBack (fromArgs->converter,
                         original,
                         originalContext,
                         &ignoredCallback,
                         &ignoredContext,
                         &err2);
  ଯଦି (U_FAILURE (err2))
  {
      *err = err2;
      ଫେରନ୍ତୁ;
  }
}



U_CAPI ଶୂନ୍ୟ  U_EXPORT2
UCNV_TO_U_CALLBACK_SKIP (
                 ଅଚଳ ଶୂନ୍ୟ *context,
                 UConverterToUnicodeArgs *toArgs,
                 ଅଚଳ ଅକ୍ଷର* codeUnits,
                 int32_t length,
                 UConverterCallbackReason reason,
                 UErrorCode * err)
{
    (ଶୂନ୍ୟ)toArgs;
    (ଶୂନ୍ୟ)codeUnits;
    (ଶୂନ୍ୟ)length;
    ଯଦି (reason <= UCNV_IRREGULAR)
    {
        ଯଦି (context == nullptr || (*((ଅକ୍ଷର*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
        {
            *err = U_ZERO_ERROR;
        }
        /* else the caller must have set the error code accordingly. */
    }
    /* else ignore the reset, close and clone calls. */
}

U_CAPI ଶୂନ୍ୟ    U_EXPORT2
UCNV_TO_U_CALLBACK_SUBSTITUTE (
                 ଅଚଳ ଶୂନ୍ୟ *context,
                 UConverterToUnicodeArgs *toArgs,
                 ଅଚଳ ଅକ୍ଷର* codeUnits,
                 int32_t length,
                 UConverterCallbackReason reason,
                 UErrorCode * err)
{
    (ଶୂନ୍ୟ)codeUnits;
    (ଶୂନ୍ୟ)length;
    ଯଦି (reason <= UCNV_IRREGULAR)
    {
        ଯଦି (context == nullptr || (*((ଅକ୍ଷର*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
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
U_CAPI ଶୂନ୍ୟ   U_EXPORT2
UCNV_TO_U_CALLBACK_ESCAPE (
                 ଅଚଳ ଶୂନ୍ୟ *context,
                 UConverterToUnicodeArgs *toArgs,
                 ଅଚଳ ଅକ୍ଷର* codeUnits,
                 int32_t length,
                 UConverterCallbackReason reason,
                 UErrorCode * err)
{
    char16_t uni_ମୂଲ୍ୟ_String[ମୂଲ୍ୟ_STRING_LENGTH];
    int32_t ମୂଲ୍ୟ_String_Length = 0;
    int32_t i = 0;

    ଯଦି (reason > UCNV_IRREGULAR)
    {
        ଫେରନ୍ତୁ;
    }

    ଯଦି(context==nullptr)
    {    
        ଯେତେବେଳେ (i < length)
        {
            uni_ମୂଲ୍ୟ_String[ମୂଲ୍ୟ_String_Length++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT; /* adding % */
            uni_ମୂଲ୍ୟ_String[ମୂଲ୍ୟ_String_Length++] = (char16_t) UNICODE_X_CODEPOINT;    /* adding X */
            ମୂଲ୍ୟ_String_Length += uprv_itou (uni_ମୂଲ୍ୟ_String + ମୂଲ୍ୟ_String_Length, ମୂଲ୍ୟ_STRING_LENGTH - ମୂଲ୍ୟ_String_Length, (uint8_t) codeUnits[i++], 16, 2);
        }
    }
    ଅନ୍ୟଥା
    {
        ବାଛ(*((ଅକ୍ଷର*)context))
        {
        ସ୍ଥିତି UCNV_PRV_ESCAPE_XML_DEC:
            ଯେତେବେଳେ (i < length)
            {
                uni_ମୂଲ୍ୟ_String[ମୂଲ୍ୟ_String_Length++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
                uni_ମୂଲ୍ୟ_String[ମୂଲ୍ୟ_String_Length++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
                ମୂଲ୍ୟ_String_Length += uprv_itou (uni_ମୂଲ୍ୟ_String + ମୂଲ୍ୟ_String_Length, ମୂଲ୍ୟ_STRING_LENGTH - ମୂଲ୍ୟ_String_Length, (uint8_t)codeUnits[i++], 10, 0);
                uni_ମୂଲ୍ୟ_String[ମୂଲ୍ୟ_String_Length++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
            }
            ଭାଙ୍ଗ;

        ସ୍ଥିତି UCNV_PRV_ESCAPE_XML_HEX:
            ଯେତେବେଳେ (i < length)
            {
                uni_ମୂଲ୍ୟ_String[ମୂଲ୍ୟ_String_Length++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
                uni_ମୂଲ୍ୟ_String[ମୂଲ୍ୟ_String_Length++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
                uni_ମୂଲ୍ୟ_String[ମୂଲ୍ୟ_String_Length++] = (char16_t) UNICODE_X_ତଳ_CODEPOINT; /* adding x */
                ମୂଲ୍ୟ_String_Length += uprv_itou (uni_ମୂଲ୍ୟ_String + ମୂଲ୍ୟ_String_Length, ମୂଲ୍ୟ_STRING_LENGTH - ମୂଲ୍ୟ_String_Length, (uint8_t)codeUnits[i++], 16, 0);
                uni_ମୂଲ୍ୟ_String[ମୂଲ୍ୟ_String_Length++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
            }
            ଭାଙ୍ଗ;
        ସ୍ଥିତି UCNV_PRV_ESCAPE_C:
            ଯେତେବେଳେ (i < length)
            {
                uni_ମୂଲ୍ୟ_String[ମୂଲ୍ୟ_String_Length++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */
                uni_ମୂଲ୍ୟ_String[ମୂଲ୍ୟ_String_Length++] = (char16_t) UNICODE_X_ତଳ_CODEPOINT; /* adding x */
                ମୂଲ୍ୟ_String_Length += uprv_itou (uni_ମୂଲ୍ୟ_String + ମୂଲ୍ୟ_String_Length, ମୂଲ୍ୟ_STRING_LENGTH - ମୂଲ୍ୟ_String_Length, (uint8_t)codeUnits[i++], 16, 2);
            }
            ଭାଙ୍ଗ;
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            ଯେତେବେଳେ (i < length)
            {
                uni_ମୂଲ୍ୟ_String[ମୂଲ୍ୟ_String_Length++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT; /* adding % */
                uni_ମୂଲ୍ୟ_String[ମୂଲ୍ୟ_String_Length++] = (char16_t) UNICODE_X_CODEPOINT;    /* adding X */
                uprv_itou (uni_ମୂଲ୍ୟ_String + ମୂଲ୍ୟ_String_Length, ମୂଲ୍ୟ_STRING_LENGTH - ମୂଲ୍ୟ_String_Length, (uint8_t) codeUnits[i++], 16, 2);
                ମୂଲ୍ୟ_String_Length += 2;
            }
        }
    }
    /* reset the error */
    *err = U_ZERO_ERROR;

    ucnv_cbToUWriteUChars(toArgs, uni_ମୂଲ୍ୟ_String, ମୂଲ୍ୟ_String_Length, 0, err);
}

#endif
