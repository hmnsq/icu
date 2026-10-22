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

#அந்தர்பாவ "unicode/utypes.h"

#எனில் !UCONFIG_NO_CONVERSION

#அந்தர்பாவ "unicode/ucnv_err.h"
#அந்தர்பாவ "unicode/ucnv_cb.h"
#அந்தர்பாவ "ucnv_cnv.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "unicode/ucnv.h"
#அந்தர்பாவ "ustrfmt.h"

#define மதிப்பு_STRING_LENGTH 48
/*Magic # 32 = 4(number of char in value string) * 8(max number of bytes per char for any converter) */
#define UNICODE_PERCENT_SIGN_CODEPOINT  0x0025
#define UNICODE_U_CODEPOINT             0x0055
#define UNICODE_X_CODEPOINT             0x0058
#define UNICODE_RS_CODEPOINT            0x005C
#define UNICODE_U_கீழ்_CODEPOINT         0x0075
#define UNICODE_X_கீழ்_CODEPOINT         0x0078
#define UNICODE_AMP_CODEPOINT           0x0026
#define UNICODE_HASH_CODEPOINT          0x0023
#define UNICODE_SEMICOLON_CODEPOINT     0x003B
#define UNICODE_PLUS_CODEPOINT          0x002B
#define UNICODE_இடது_CURLY_CODEPOINT    0x007B
#define UNICODE_வலது_CURLY_CODEPOINT   0x007D
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
U_CAPI வெறுமை    U_EXPORT2
UCNV_FROM_U_CALLBACK_STOP (
                  மாறிலி வெறுமை *context,
                  UConverterFromUnicodeArgs *fromUArgs,
                  மாறிலி char16_t* codeUnits,
                  int32_t length,
                  UChar32 codePoint,
                  UConverterCallbackReason reason,
                  UErrorCode * err)
{
    (வெறுமை)context;
    (வெறுமை)fromUArgs;
    (வெறுமை)codeUnits;
    (வெறுமை)length;
    எனில் (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_CODE_POINT(codePoint))
    {
        /*
         * Skip if the codepoint has unicode property of default ignorable.
         */
        *err = U_ZERO_ERROR;
    }
    /* the caller must have set the error code accordingly */
}


/*Function Pointer STOPS at the ILLEGAL_SEQUENCE */
U_CAPI வெறுமை    U_EXPORT2
UCNV_TO_U_CALLBACK_STOP (
                   மாறிலி வெறுமை *context,
                   UConverterToUnicodeArgs *toUArgs,
                   மாறிலி எழுத்து* codePoints,
                   int32_t length,
                   UConverterCallbackReason reason,
                   UErrorCode * err)
{
    /* the caller must have set the error code accordingly */
    (வெறுமை)context; (வெறுமை)toUArgs; (வெறுமை)codePoints; (வெறுமை)length; (வெறுமை)reason; (வெறுமை)err;
}

U_CAPI வெறுமை    U_EXPORT2
UCNV_FROM_U_CALLBACK_SKIP (                  
                  மாறிலி வெறுமை *context,
                  UConverterFromUnicodeArgs *fromUArgs,
                  மாறிலி char16_t* codeUnits,
                  int32_t length,
                  UChar32 codePoint,
                  UConverterCallbackReason reason,
                  UErrorCode * err)
{
    (வெறுமை)fromUArgs;
    (வெறுமை)codeUnits;
    (வெறுமை)length;
    எனில் (reason <= UCNV_IRREGULAR)
    {
        எனில் (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_CODE_POINT(codePoint))
        {
            /*
             * Skip if the codepoint has unicode property of default ignorable.
             */
            *err = U_ZERO_ERROR;
        }
        இல்லையெனில் எனில் (context == nullptr || (*((எழுத்து*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
        {
            *err = U_ZERO_ERROR;
        }
        /* else the caller must have set the error code accordingly. */
    }
    /* else ignore the reset, close and clone calls. */
}

U_CAPI வெறுமை    U_EXPORT2
UCNV_FROM_U_CALLBACK_SUBSTITUTE (
                  மாறிலி வெறுமை *context,
                  UConverterFromUnicodeArgs *fromArgs,
                  மாறிலி char16_t* codeUnits,
                  int32_t length,
                  UChar32 codePoint,
                  UConverterCallbackReason reason,
                  UErrorCode * err)
{
    (வெறுமை)codeUnits;
    (வெறுமை)length;
    எனில் (reason <= UCNV_IRREGULAR)
    {
        எனில் (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_CODE_POINT(codePoint))
        {
            /*
             * Skip if the codepoint has unicode property of default ignorable.
             */
            *err = U_ZERO_ERROR;
        }
        இல்லையெனில் எனில் (context == nullptr || (*((எழுத்து*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
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
U_CAPI வெறுமை    U_EXPORT2
UCNV_FROM_U_CALLBACK_ESCAPE (
                         மாறிலி வெறுமை *context,
                         UConverterFromUnicodeArgs *fromArgs,
                         மாறிலி char16_t *codeUnits,
                         int32_t length,
                         UChar32 codePoint,
                         UConverterCallbackReason reason,
                         UErrorCode * err)
{

  char16_t மதிப்பு_String[மதிப்பு_STRING_LENGTH];
  int32_t மதிப்பு_String_Length = 0;
  int32_t i = 0;

  மாறிலி char16_t *my_மதிப்பு_Source = nullptr;
  UErrorCode err2 = U_ZERO_ERROR;
  UConverterFromUCallback original = nullptr;
  மாறிலி வெறுமை *originalContext;

  UConverterFromUCallback ignoredCallback = nullptr;
  மாறிலி வெறுமை *ignoredContext;
  
  எனில் (reason > UCNV_IRREGULAR)
  {
      திரும்பு;
  }
  இல்லையெனில் எனில் (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_CODE_POINT(codePoint))
  {
      /*
       * Skip if the codepoint has unicode property of default ignorable.
       */
      *err = U_ZERO_ERROR;
      திரும்பு;
  }

  ucnv_setFromUCallBack (fromArgs->converter,
                     (UConverterFromUCallback) UCNV_FROM_U_CALLBACK_SUBSTITUTE,
                     nullptr,
                     &original,
                     &originalContext,
                     &err2);
  
  எனில் (U_FAILURE (err2))
  {
    *err = err2;
    திரும்பு;
  } 
  எனில்(context==nullptr)
  { 
      போது (i < length)
      {
        மதிப்பு_String[மதிப்பு_String_Length++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT;  /* adding % */
        மதிப்பு_String[மதிப்பு_String_Length++] = (char16_t) UNICODE_U_CODEPOINT; /* adding U */
        மதிப்பு_String_Length += uprv_itou (மதிப்பு_String + மதிப்பு_String_Length, மதிப்பு_STRING_LENGTH - மதிப்பு_String_Length, (uint16_t)codeUnits[i++], 16, 4);
      }
  }
  இல்லையெனில்
  {
      தேர்வு(*((எழுத்து*)context))
      {
      நிலை UCNV_PRV_ESCAPE_JAVA:
          போது (i < length)
          {
              மதிப்பு_String[மதிப்பு_String_Length++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */
              மதிப்பு_String[மதிப்பு_String_Length++] = (char16_t) UNICODE_U_கீழ்_CODEPOINT; /* adding u */
              மதிப்பு_String_Length += uprv_itou (மதிப்பு_String + மதிப்பு_String_Length, மதிப்பு_STRING_LENGTH - மதிப்பு_String_Length, (uint16_t)codeUnits[i++], 16, 4);
          }
          நிறுத்து;

      நிலை UCNV_PRV_ESCAPE_C:
          மதிப்பு_String[மதிப்பு_String_Length++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */

          எனில்(length==2){
              மதிப்பு_String[மதிப்பு_String_Length++] = (char16_t) UNICODE_U_CODEPOINT; /* adding U */
              மதிப்பு_String_Length += uprv_itou (மதிப்பு_String + மதிப்பு_String_Length, மதிப்பு_STRING_LENGTH - மதிப்பு_String_Length, codePoint, 16, 8);

          }
          இல்லையெனில்{
              மதிப்பு_String[மதிப்பு_String_Length++] = (char16_t) UNICODE_U_கீழ்_CODEPOINT; /* adding u */
              மதிப்பு_String_Length += uprv_itou (மதிப்பு_String + மதிப்பு_String_Length, மதிப்பு_STRING_LENGTH - மதிப்பு_String_Length, (uint16_t)codeUnits[0], 16, 4);
          }
          நிறுத்து;

      நிலை UCNV_PRV_ESCAPE_XML_DEC:

          மதிப்பு_String[மதிப்பு_String_Length++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
          மதிப்பு_String[மதிப்பு_String_Length++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
          எனில்(length==2){
              மதிப்பு_String_Length += uprv_itou (மதிப்பு_String + மதிப்பு_String_Length, மதிப்பு_STRING_LENGTH - மதிப்பு_String_Length, codePoint, 10, 0);
          }
          இல்லையெனில்{
              மதிப்பு_String_Length += uprv_itou (மதிப்பு_String + மதிப்பு_String_Length, மதிப்பு_STRING_LENGTH - மதிப்பு_String_Length, (uint16_t)codeUnits[0], 10, 0);
          }
          மதிப்பு_String[மதிப்பு_String_Length++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
          நிறுத்து;

      நிலை UCNV_PRV_ESCAPE_XML_HEX:

          மதிப்பு_String[மதிப்பு_String_Length++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
          மதிப்பு_String[மதிப்பு_String_Length++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
          மதிப்பு_String[மதிப்பு_String_Length++] = (char16_t) UNICODE_X_கீழ்_CODEPOINT; /* adding x */
          எனில்(length==2){
              மதிப்பு_String_Length += uprv_itou (மதிப்பு_String + மதிப்பு_String_Length, மதிப்பு_STRING_LENGTH - மதிப்பு_String_Length, codePoint, 16, 0);
          }
          இல்லையெனில்{
              மதிப்பு_String_Length += uprv_itou (மதிப்பு_String + மதிப்பு_String_Length, மதிப்பு_STRING_LENGTH - மதிப்பு_String_Length, (uint16_t)codeUnits[0], 16, 0);
          }
          மதிப்பு_String[மதிப்பு_String_Length++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
          நிறுத்து;

      நிலை UCNV_PRV_ESCAPE_UNICODE:
          மதிப்பு_String[மதிப்பு_String_Length++] = (char16_t) UNICODE_இடது_CURLY_CODEPOINT;    /* adding { */
          மதிப்பு_String[மதிப்பு_String_Length++] = (char16_t) UNICODE_U_CODEPOINT;    /* adding U */
          மதிப்பு_String[மதிப்பு_String_Length++] = (char16_t) UNICODE_PLUS_CODEPOINT; /* adding + */
          எனில் (length == 2) {
              மதிப்பு_String_Length += uprv_itou (மதிப்பு_String + மதிப்பு_String_Length, மதிப்பு_STRING_LENGTH - மதிப்பு_String_Length, codePoint, 16, 4);
          } இல்லையெனில் {
              மதிப்பு_String_Length += uprv_itou (மதிப்பு_String + மதிப்பு_String_Length, மதிப்பு_STRING_LENGTH - மதிப்பு_String_Length, (uint16_t)codeUnits[0], 16, 4);
          }
          மதிப்பு_String[மதிப்பு_String_Length++] = (char16_t) UNICODE_வலது_CURLY_CODEPOINT;    /* adding } */
          நிறுத்து;

      நிலை UCNV_PRV_ESCAPE_CSS2:
          மதிப்பு_String[மதிப்பு_String_Length++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */
          மதிப்பு_String_Length += uprv_itou (மதிப்பு_String + மதிப்பு_String_Length, மதிப்பு_STRING_LENGTH - மதிப்பு_String_Length, codePoint, 16, 0);
          /* Always add space character, because the next character might be whitespace,
             which would erroneously be considered the termination of the escape sequence. */
          மதிப்பு_String[மதிப்பு_String_Length++] = (char16_t) UNICODE_SPACE_CODEPOINT;
          நிறுத்து;

      இயல்பு:
          போது (i < length)
          {
              மதிப்பு_String[மதிப்பு_String_Length++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT;  /* adding % */
              மதிப்பு_String[மதிப்பு_String_Length++] = (char16_t) UNICODE_U_CODEPOINT;             /* adding U */
              மதிப்பு_String_Length += uprv_itou (மதிப்பு_String + மதிப்பு_String_Length, மதிப்பு_STRING_LENGTH - மதிப்பு_String_Length, (uint16_t)codeUnits[i++], 16, 4);
          }
      }
  }  
  my_மதிப்பு_Source = மதிப்பு_String;

  /* reset the error */
  *err = U_ZERO_ERROR;

  ucnv_cbFromUWriteUChars(fromArgs, &my_மதிப்பு_Source, my_மதிப்பு_Source+மதிப்பு_String_Length, 0, err);

  ucnv_setFromUCallBack (fromArgs->converter,
                         original,
                         originalContext,
                         &ignoredCallback,
                         &ignoredContext,
                         &err2);
  எனில் (U_FAILURE (err2))
  {
      *err = err2;
      திரும்பு;
  }
}



U_CAPI வெறுமை  U_EXPORT2
UCNV_TO_U_CALLBACK_SKIP (
                 மாறிலி வெறுமை *context,
                 UConverterToUnicodeArgs *toArgs,
                 மாறிலி எழுத்து* codeUnits,
                 int32_t length,
                 UConverterCallbackReason reason,
                 UErrorCode * err)
{
    (வெறுமை)toArgs;
    (வெறுமை)codeUnits;
    (வெறுமை)length;
    எனில் (reason <= UCNV_IRREGULAR)
    {
        எனில் (context == nullptr || (*((எழுத்து*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
        {
            *err = U_ZERO_ERROR;
        }
        /* else the caller must have set the error code accordingly. */
    }
    /* else ignore the reset, close and clone calls. */
}

U_CAPI வெறுமை    U_EXPORT2
UCNV_TO_U_CALLBACK_SUBSTITUTE (
                 மாறிலி வெறுமை *context,
                 UConverterToUnicodeArgs *toArgs,
                 மாறிலி எழுத்து* codeUnits,
                 int32_t length,
                 UConverterCallbackReason reason,
                 UErrorCode * err)
{
    (வெறுமை)codeUnits;
    (வெறுமை)length;
    எனில் (reason <= UCNV_IRREGULAR)
    {
        எனில் (context == nullptr || (*((எழுத்து*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
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
U_CAPI வெறுமை   U_EXPORT2
UCNV_TO_U_CALLBACK_ESCAPE (
                 மாறிலி வெறுமை *context,
                 UConverterToUnicodeArgs *toArgs,
                 மாறிலி எழுத்து* codeUnits,
                 int32_t length,
                 UConverterCallbackReason reason,
                 UErrorCode * err)
{
    char16_t uni_மதிப்பு_String[மதிப்பு_STRING_LENGTH];
    int32_t மதிப்பு_String_Length = 0;
    int32_t i = 0;

    எனில் (reason > UCNV_IRREGULAR)
    {
        திரும்பு;
    }

    எனில்(context==nullptr)
    {    
        போது (i < length)
        {
            uni_மதிப்பு_String[மதிப்பு_String_Length++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT; /* adding % */
            uni_மதிப்பு_String[மதிப்பு_String_Length++] = (char16_t) UNICODE_X_CODEPOINT;    /* adding X */
            மதிப்பு_String_Length += uprv_itou (uni_மதிப்பு_String + மதிப்பு_String_Length, மதிப்பு_STRING_LENGTH - மதிப்பு_String_Length, (uint8_t) codeUnits[i++], 16, 2);
        }
    }
    இல்லையெனில்
    {
        தேர்வு(*((எழுத்து*)context))
        {
        நிலை UCNV_PRV_ESCAPE_XML_DEC:
            போது (i < length)
            {
                uni_மதிப்பு_String[மதிப்பு_String_Length++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
                uni_மதிப்பு_String[மதிப்பு_String_Length++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
                மதிப்பு_String_Length += uprv_itou (uni_மதிப்பு_String + மதிப்பு_String_Length, மதிப்பு_STRING_LENGTH - மதிப்பு_String_Length, (uint8_t)codeUnits[i++], 10, 0);
                uni_மதிப்பு_String[மதிப்பு_String_Length++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
            }
            நிறுத்து;

        நிலை UCNV_PRV_ESCAPE_XML_HEX:
            போது (i < length)
            {
                uni_மதிப்பு_String[மதிப்பு_String_Length++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
                uni_மதிப்பு_String[மதிப்பு_String_Length++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
                uni_மதிப்பு_String[மதிப்பு_String_Length++] = (char16_t) UNICODE_X_கீழ்_CODEPOINT; /* adding x */
                மதிப்பு_String_Length += uprv_itou (uni_மதிப்பு_String + மதிப்பு_String_Length, மதிப்பு_STRING_LENGTH - மதிப்பு_String_Length, (uint8_t)codeUnits[i++], 16, 0);
                uni_மதிப்பு_String[மதிப்பு_String_Length++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
            }
            நிறுத்து;
        நிலை UCNV_PRV_ESCAPE_C:
            போது (i < length)
            {
                uni_மதிப்பு_String[மதிப்பு_String_Length++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */
                uni_மதிப்பு_String[மதிப்பு_String_Length++] = (char16_t) UNICODE_X_கீழ்_CODEPOINT; /* adding x */
                மதிப்பு_String_Length += uprv_itou (uni_மதிப்பு_String + மதிப்பு_String_Length, மதிப்பு_STRING_LENGTH - மதிப்பு_String_Length, (uint8_t)codeUnits[i++], 16, 2);
            }
            நிறுத்து;
        இயல்பு:
            போது (i < length)
            {
                uni_மதிப்பு_String[மதிப்பு_String_Length++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT; /* adding % */
                uni_மதிப்பு_String[மதிப்பு_String_Length++] = (char16_t) UNICODE_X_CODEPOINT;    /* adding X */
                uprv_itou (uni_மதிப்பு_String + மதிப்பு_String_Length, மதிப்பு_STRING_LENGTH - மதிப்பு_String_Length, (uint8_t) codeUnits[i++], 16, 2);
                மதிப்பு_String_Length += 2;
            }
        }
    }
    /* reset the error */
    *err = U_ZERO_ERROR;

    ucnv_cbToUWriteUChars(toArgs, uni_மதிப்பு_String, மதிப்பு_String_Length, 0, err);
}

#endif
