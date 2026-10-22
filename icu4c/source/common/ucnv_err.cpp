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

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_CONVERSION

#അന്തര്ഭാവ "unicode/ucnv_err.h"
#അന്തര്ഭാവ "unicode/ucnv_cb.h"
#അന്തര്ഭാവ "ucnv_cnv.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "unicode/ucnv.h"
#അന്തര്ഭാവ "ustrfmt.h"

#define മൂല്യം_STRING_LENGTH 48
/*Magic # 32 = 4(number of char in value string) * 8(max number of bytes per char for any converter) */
#define UNICODE_PERCENT_SIGN_CODEPOINT  0x0025
#define UNICODE_U_CODEPOINT             0x0055
#define UNICODE_X_CODEPOINT             0x0058
#define UNICODE_RS_CODEPOINT            0x005C
#define UNICODE_U_താഴെയുള്ള_CODEPOINT         0x0075
#define UNICODE_X_താഴെയുള്ള_CODEPOINT         0x0078
#define UNICODE_AMP_CODEPOINT           0x0026
#define UNICODE_HASH_CODEPOINT          0x0023
#define UNICODE_SEMICOLON_CODEPOINT     0x003B
#define UNICODE_PLUS_CODEPOINT          0x002B
#define UNICODE_ഇടത്_CURLY_CODEPOINT    0x007B
#define UNICODE_വലത്_CURLY_CODEPOINT   0x007D
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
U_CAPI ശൂന്യം    U_EXPORT2
UCNV_FROM_U_CALLBACK_STOP (
                  സ്ഥിരം ശൂന്യം *context,
                  UConverterFromUnicodeArgs *fromUArgs,
                  സ്ഥിരം char16_t* codeUnits,
                  int32_t length,
                  UChar32 codePoint,
                  UConverterCallbackReason reason,
                  UErrorCode * err)
{
    (ശൂന്യം)context;
    (ശൂന്യം)fromUArgs;
    (ശൂന്യം)codeUnits;
    (ശൂന്യം)length;
    എങ്കിൽ (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_CODE_POINT(codePoint))
    {
        /*
         * Skip if the codepoint has unicode property of default ignorable.
         */
        *err = U_ZERO_ERROR;
    }
    /* the caller must have set the error code accordingly */
}


/*Function Pointer STOPS at the ILLEGAL_SEQUENCE */
U_CAPI ശൂന്യം    U_EXPORT2
UCNV_TO_U_CALLBACK_STOP (
                   സ്ഥിരം ശൂന്യം *context,
                   UConverterToUnicodeArgs *toUArgs,
                   സ്ഥിരം അക്ഷരം* codePoints,
                   int32_t length,
                   UConverterCallbackReason reason,
                   UErrorCode * err)
{
    /* the caller must have set the error code accordingly */
    (ശൂന്യം)context; (ശൂന്യം)toUArgs; (ശൂന്യം)codePoints; (ശൂന്യം)length; (ശൂന്യം)reason; (ശൂന്യം)err;
}

U_CAPI ശൂന്യം    U_EXPORT2
UCNV_FROM_U_CALLBACK_SKIP (                  
                  സ്ഥിരം ശൂന്യം *context,
                  UConverterFromUnicodeArgs *fromUArgs,
                  സ്ഥിരം char16_t* codeUnits,
                  int32_t length,
                  UChar32 codePoint,
                  UConverterCallbackReason reason,
                  UErrorCode * err)
{
    (ശൂന്യം)fromUArgs;
    (ശൂന്യം)codeUnits;
    (ശൂന്യം)length;
    എങ്കിൽ (reason <= UCNV_IRREGULAR)
    {
        എങ്കിൽ (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_CODE_POINT(codePoint))
        {
            /*
             * Skip if the codepoint has unicode property of default ignorable.
             */
            *err = U_ZERO_ERROR;
        }
        അല്ലെങ്കിൽ എങ്കിൽ (context == nullptr || (*((അക്ഷരം*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
        {
            *err = U_ZERO_ERROR;
        }
        /* else the caller must have set the error code accordingly. */
    }
    /* else ignore the reset, close and clone calls. */
}

U_CAPI ശൂന്യം    U_EXPORT2
UCNV_FROM_U_CALLBACK_SUBSTITUTE (
                  സ്ഥിരം ശൂന്യം *context,
                  UConverterFromUnicodeArgs *fromArgs,
                  സ്ഥിരം char16_t* codeUnits,
                  int32_t length,
                  UChar32 codePoint,
                  UConverterCallbackReason reason,
                  UErrorCode * err)
{
    (ശൂന്യം)codeUnits;
    (ശൂന്യം)length;
    എങ്കിൽ (reason <= UCNV_IRREGULAR)
    {
        എങ്കിൽ (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_CODE_POINT(codePoint))
        {
            /*
             * Skip if the codepoint has unicode property of default ignorable.
             */
            *err = U_ZERO_ERROR;
        }
        അല്ലെങ്കിൽ എങ്കിൽ (context == nullptr || (*((അക്ഷരം*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
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
U_CAPI ശൂന്യം    U_EXPORT2
UCNV_FROM_U_CALLBACK_ESCAPE (
                         സ്ഥിരം ശൂന്യം *context,
                         UConverterFromUnicodeArgs *fromArgs,
                         സ്ഥിരം char16_t *codeUnits,
                         int32_t length,
                         UChar32 codePoint,
                         UConverterCallbackReason reason,
                         UErrorCode * err)
{

  char16_t മൂല്യം_String[മൂല്യം_STRING_LENGTH];
  int32_t മൂല്യം_String_Length = 0;
  int32_t i = 0;

  സ്ഥിരം char16_t *my_മൂല്യം_Source = nullptr;
  UErrorCode err2 = U_ZERO_ERROR;
  UConverterFromUCallback original = nullptr;
  സ്ഥിരം ശൂന്യം *originalContext;

  UConverterFromUCallback ignoredCallback = nullptr;
  സ്ഥിരം ശൂന്യം *ignoredContext;
  
  എങ്കിൽ (reason > UCNV_IRREGULAR)
  {
      തിരികെ;
  }
  അല്ലെങ്കിൽ എങ്കിൽ (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_CODE_POINT(codePoint))
  {
      /*
       * Skip if the codepoint has unicode property of default ignorable.
       */
      *err = U_ZERO_ERROR;
      തിരികെ;
  }

  ucnv_setFromUCallBack (fromArgs->converter,
                     (UConverterFromUCallback) UCNV_FROM_U_CALLBACK_SUBSTITUTE,
                     nullptr,
                     &original,
                     &originalContext,
                     &err2);
  
  എങ്കിൽ (U_FAILURE (err2))
  {
    *err = err2;
    തിരികെ;
  } 
  എങ്കിൽ(context==nullptr)
  { 
      ആകുമ്പോൾ (i < length)
      {
        മൂല്യം_String[മൂല്യം_String_Length++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT;  /* adding % */
        മൂല്യം_String[മൂല്യം_String_Length++] = (char16_t) UNICODE_U_CODEPOINT; /* adding U */
        മൂല്യം_String_Length += uprv_itou (മൂല്യം_String + മൂല്യം_String_Length, മൂല്യം_STRING_LENGTH - മൂല്യം_String_Length, (uint16_t)codeUnits[i++], 16, 4);
      }
  }
  അല്ലെങ്കിൽ
  {
      തിരഞ്ഞെടുപ്പ്(*((അക്ഷരം*)context))
      {
      അവസ്ഥ UCNV_PRV_ESCAPE_JAVA:
          ആകുമ്പോൾ (i < length)
          {
              മൂല്യം_String[മൂല്യം_String_Length++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */
              മൂല്യം_String[മൂല്യം_String_Length++] = (char16_t) UNICODE_U_താഴെയുള്ള_CODEPOINT; /* adding u */
              മൂല്യം_String_Length += uprv_itou (മൂല്യം_String + മൂല്യം_String_Length, മൂല്യം_STRING_LENGTH - മൂല്യം_String_Length, (uint16_t)codeUnits[i++], 16, 4);
          }
          നിർത്തുക;

      അവസ്ഥ UCNV_PRV_ESCAPE_C:
          മൂല്യം_String[മൂല്യം_String_Length++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */

          എങ്കിൽ(length==2){
              മൂല്യം_String[മൂല്യം_String_Length++] = (char16_t) UNICODE_U_CODEPOINT; /* adding U */
              മൂല്യം_String_Length += uprv_itou (മൂല്യം_String + മൂല്യം_String_Length, മൂല്യം_STRING_LENGTH - മൂല്യം_String_Length, codePoint, 16, 8);

          }
          അല്ലെങ്കിൽ{
              മൂല്യം_String[മൂല്യം_String_Length++] = (char16_t) UNICODE_U_താഴെയുള്ള_CODEPOINT; /* adding u */
              മൂല്യം_String_Length += uprv_itou (മൂല്യം_String + മൂല്യം_String_Length, മൂല്യം_STRING_LENGTH - മൂല്യം_String_Length, (uint16_t)codeUnits[0], 16, 4);
          }
          നിർത്തുക;

      അവസ്ഥ UCNV_PRV_ESCAPE_XML_DEC:

          മൂല്യം_String[മൂല്യം_String_Length++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
          മൂല്യം_String[മൂല്യം_String_Length++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
          എങ്കിൽ(length==2){
              മൂല്യം_String_Length += uprv_itou (മൂല്യം_String + മൂല്യം_String_Length, മൂല്യം_STRING_LENGTH - മൂല്യം_String_Length, codePoint, 10, 0);
          }
          അല്ലെങ്കിൽ{
              മൂല്യം_String_Length += uprv_itou (മൂല്യം_String + മൂല്യം_String_Length, മൂല്യം_STRING_LENGTH - മൂല്യം_String_Length, (uint16_t)codeUnits[0], 10, 0);
          }
          മൂല്യം_String[മൂല്യം_String_Length++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
          നിർത്തുക;

      അവസ്ഥ UCNV_PRV_ESCAPE_XML_HEX:

          മൂല്യം_String[മൂല്യം_String_Length++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
          മൂല്യം_String[മൂല്യം_String_Length++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
          മൂല്യം_String[മൂല്യം_String_Length++] = (char16_t) UNICODE_X_താഴെയുള്ള_CODEPOINT; /* adding x */
          എങ്കിൽ(length==2){
              മൂല്യം_String_Length += uprv_itou (മൂല്യം_String + മൂല്യം_String_Length, മൂല്യം_STRING_LENGTH - മൂല്യം_String_Length, codePoint, 16, 0);
          }
          അല്ലെങ്കിൽ{
              മൂല്യം_String_Length += uprv_itou (മൂല്യം_String + മൂല്യം_String_Length, മൂല്യം_STRING_LENGTH - മൂല്യം_String_Length, (uint16_t)codeUnits[0], 16, 0);
          }
          മൂല്യം_String[മൂല്യം_String_Length++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
          നിർത്തുക;

      അവസ്ഥ UCNV_PRV_ESCAPE_UNICODE:
          മൂല്യം_String[മൂല്യം_String_Length++] = (char16_t) UNICODE_ഇടത്_CURLY_CODEPOINT;    /* adding { */
          മൂല്യം_String[മൂല്യം_String_Length++] = (char16_t) UNICODE_U_CODEPOINT;    /* adding U */
          മൂല്യം_String[മൂല്യം_String_Length++] = (char16_t) UNICODE_PLUS_CODEPOINT; /* adding + */
          എങ്കിൽ (length == 2) {
              മൂല്യം_String_Length += uprv_itou (മൂല്യം_String + മൂല്യം_String_Length, മൂല്യം_STRING_LENGTH - മൂല്യം_String_Length, codePoint, 16, 4);
          } അല്ലെങ്കിൽ {
              മൂല്യം_String_Length += uprv_itou (മൂല്യം_String + മൂല്യം_String_Length, മൂല്യം_STRING_LENGTH - മൂല്യം_String_Length, (uint16_t)codeUnits[0], 16, 4);
          }
          മൂല്യം_String[മൂല്യം_String_Length++] = (char16_t) UNICODE_വലത്_CURLY_CODEPOINT;    /* adding } */
          നിർത്തുക;

      അവസ്ഥ UCNV_PRV_ESCAPE_CSS2:
          മൂല്യം_String[മൂല്യം_String_Length++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */
          മൂല്യം_String_Length += uprv_itou (മൂല്യം_String + മൂല്യം_String_Length, മൂല്യം_STRING_LENGTH - മൂല്യം_String_Length, codePoint, 16, 0);
          /* Always add space character, because the next character might be whitespace,
             which would erroneously be considered the termination of the escape sequence. */
          മൂല്യം_String[മൂല്യം_String_Length++] = (char16_t) UNICODE_SPACE_CODEPOINT;
          നിർത്തുക;

      സ്ഥിരസ്ഥിതി:
          ആകുമ്പോൾ (i < length)
          {
              മൂല്യം_String[മൂല്യം_String_Length++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT;  /* adding % */
              മൂല്യം_String[മൂല്യം_String_Length++] = (char16_t) UNICODE_U_CODEPOINT;             /* adding U */
              മൂല്യം_String_Length += uprv_itou (മൂല്യം_String + മൂല്യം_String_Length, മൂല്യം_STRING_LENGTH - മൂല്യം_String_Length, (uint16_t)codeUnits[i++], 16, 4);
          }
      }
  }  
  my_മൂല്യം_Source = മൂല്യം_String;

  /* reset the error */
  *err = U_ZERO_ERROR;

  ucnv_cbFromUWriteUChars(fromArgs, &my_മൂല്യം_Source, my_മൂല്യം_Source+മൂല്യം_String_Length, 0, err);

  ucnv_setFromUCallBack (fromArgs->converter,
                         original,
                         originalContext,
                         &ignoredCallback,
                         &ignoredContext,
                         &err2);
  എങ്കിൽ (U_FAILURE (err2))
  {
      *err = err2;
      തിരികെ;
  }
}



U_CAPI ശൂന്യം  U_EXPORT2
UCNV_TO_U_CALLBACK_SKIP (
                 സ്ഥിരം ശൂന്യം *context,
                 UConverterToUnicodeArgs *toArgs,
                 സ്ഥിരം അക്ഷരം* codeUnits,
                 int32_t length,
                 UConverterCallbackReason reason,
                 UErrorCode * err)
{
    (ശൂന്യം)toArgs;
    (ശൂന്യം)codeUnits;
    (ശൂന്യം)length;
    എങ്കിൽ (reason <= UCNV_IRREGULAR)
    {
        എങ്കിൽ (context == nullptr || (*((അക്ഷരം*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
        {
            *err = U_ZERO_ERROR;
        }
        /* else the caller must have set the error code accordingly. */
    }
    /* else ignore the reset, close and clone calls. */
}

U_CAPI ശൂന്യം    U_EXPORT2
UCNV_TO_U_CALLBACK_SUBSTITUTE (
                 സ്ഥിരം ശൂന്യം *context,
                 UConverterToUnicodeArgs *toArgs,
                 സ്ഥിരം അക്ഷരം* codeUnits,
                 int32_t length,
                 UConverterCallbackReason reason,
                 UErrorCode * err)
{
    (ശൂന്യം)codeUnits;
    (ശൂന്യം)length;
    എങ്കിൽ (reason <= UCNV_IRREGULAR)
    {
        എങ്കിൽ (context == nullptr || (*((അക്ഷരം*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
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
U_CAPI ശൂന്യം   U_EXPORT2
UCNV_TO_U_CALLBACK_ESCAPE (
                 സ്ഥിരം ശൂന്യം *context,
                 UConverterToUnicodeArgs *toArgs,
                 സ്ഥിരം അക്ഷരം* codeUnits,
                 int32_t length,
                 UConverterCallbackReason reason,
                 UErrorCode * err)
{
    char16_t uni_മൂല്യം_String[മൂല്യം_STRING_LENGTH];
    int32_t മൂല്യം_String_Length = 0;
    int32_t i = 0;

    എങ്കിൽ (reason > UCNV_IRREGULAR)
    {
        തിരികെ;
    }

    എങ്കിൽ(context==nullptr)
    {    
        ആകുമ്പോൾ (i < length)
        {
            uni_മൂല്യം_String[മൂല്യം_String_Length++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT; /* adding % */
            uni_മൂല്യം_String[മൂല്യം_String_Length++] = (char16_t) UNICODE_X_CODEPOINT;    /* adding X */
            മൂല്യം_String_Length += uprv_itou (uni_മൂല്യം_String + മൂല്യം_String_Length, മൂല്യം_STRING_LENGTH - മൂല്യം_String_Length, (uint8_t) codeUnits[i++], 16, 2);
        }
    }
    അല്ലെങ്കിൽ
    {
        തിരഞ്ഞെടുപ്പ്(*((അക്ഷരം*)context))
        {
        അവസ്ഥ UCNV_PRV_ESCAPE_XML_DEC:
            ആകുമ്പോൾ (i < length)
            {
                uni_മൂല്യം_String[മൂല്യം_String_Length++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
                uni_മൂല്യം_String[മൂല്യം_String_Length++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
                മൂല്യം_String_Length += uprv_itou (uni_മൂല്യം_String + മൂല്യം_String_Length, മൂല്യം_STRING_LENGTH - മൂല്യം_String_Length, (uint8_t)codeUnits[i++], 10, 0);
                uni_മൂല്യം_String[മൂല്യം_String_Length++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
            }
            നിർത്തുക;

        അവസ്ഥ UCNV_PRV_ESCAPE_XML_HEX:
            ആകുമ്പോൾ (i < length)
            {
                uni_മൂല്യം_String[മൂല്യം_String_Length++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
                uni_മൂല്യം_String[മൂല്യം_String_Length++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
                uni_മൂല്യം_String[മൂല്യം_String_Length++] = (char16_t) UNICODE_X_താഴെയുള്ള_CODEPOINT; /* adding x */
                മൂല്യം_String_Length += uprv_itou (uni_മൂല്യം_String + മൂല്യം_String_Length, മൂല്യം_STRING_LENGTH - മൂല്യം_String_Length, (uint8_t)codeUnits[i++], 16, 0);
                uni_മൂല്യം_String[മൂല്യം_String_Length++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
            }
            നിർത്തുക;
        അവസ്ഥ UCNV_PRV_ESCAPE_C:
            ആകുമ്പോൾ (i < length)
            {
                uni_മൂല്യം_String[മൂല്യം_String_Length++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */
                uni_മൂല്യം_String[മൂല്യം_String_Length++] = (char16_t) UNICODE_X_താഴെയുള്ള_CODEPOINT; /* adding x */
                മൂല്യം_String_Length += uprv_itou (uni_മൂല്യം_String + മൂല്യം_String_Length, മൂല്യം_STRING_LENGTH - മൂല്യം_String_Length, (uint8_t)codeUnits[i++], 16, 2);
            }
            നിർത്തുക;
        സ്ഥിരസ്ഥിതി:
            ആകുമ്പോൾ (i < length)
            {
                uni_മൂല്യം_String[മൂല്യം_String_Length++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT; /* adding % */
                uni_മൂല്യം_String[മൂല്യം_String_Length++] = (char16_t) UNICODE_X_CODEPOINT;    /* adding X */
                uprv_itou (uni_മൂല്യം_String + മൂല്യം_String_Length, മൂല്യം_STRING_LENGTH - മൂല്യം_String_Length, (uint8_t) codeUnits[i++], 16, 2);
                മൂല്യം_String_Length += 2;
            }
        }
    }
    /* reset the error */
    *err = U_ZERO_ERROR;

    ucnv_cbToUWriteUChars(toArgs, uni_മൂല്യം_String, മൂല്യം_String_Length, 0, err);
}

#endif
