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

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_CONVERSION

#समावेश "unicode/ucnv_err.h"
#समावेश "unicode/ucnv_cb.h"
#समावेश "ucnv_cnv.h"
#समावेश "cmemory.h"
#समावेश "unicode/ucnv.h"
#समावेश "ustrfmt.h"

#घोषणा मूल्य_STRING_लंबाई 48
/*Magic # 32 = 4(number of char in value string) * 8(max number of bytes per char for any converter) */
#घोषणा UNICODE_PERCENT_SIGN_CODEPOINT  0x0025
#घोषणा UNICODE_U_CODEPOINT             0x0055
#घोषणा UNICODE_X_CODEPOINT             0x0058
#घोषणा UNICODE_RS_CODEPOINT            0x005C
#घोषणा UNICODE_U_निचला_CODEPOINT         0x0075
#घोषणा UNICODE_X_निचला_CODEPOINT         0x0078
#घोषणा UNICODE_AMP_CODEPOINT           0x0026
#घोषणा UNICODE_HASH_CODEPOINT          0x0023
#घोषणा UNICODE_SEMICOLON_CODEPOINT     0x003B
#घोषणा UNICODE_PLUS_CODEPOINT          0x002B
#घोषणा UNICODE_बायाँ_CURLY_CODEPOINT    0x007B
#घोषणा UNICODE_दायाँ_CURLY_CODEPOINT   0x007D
#घोषणा UNICODE_SPACE_CODEPOINT         0x0020
#घोषणा UCNV_PRV_ESCAPE_ICU         0
#घोषणा UCNV_PRV_ESCAPE_C           'C'
#घोषणा UCNV_PRV_ESCAPE_XML_DEC     'D'
#घोषणा UCNV_PRV_ESCAPE_XML_HEX     'X'
#घोषणा UCNV_PRV_ESCAPE_JAVA        'J'
#घोषणा UCNV_PRV_ESCAPE_UNICODE     'U'
#घोषणा UCNV_PRV_ESCAPE_CSS2        'S'
#घोषणा UCNV_PRV_STOP_ON_ILLEGAL    'i'

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
#घोषणा IS_DEFAULT_IGNORABLE_CODE_POINT(c) ( \
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
U_CAPI व्योम    U_EXPORT2
UCNV_FROM_U_CALLBACK_STOP (
                  स्थिर व्योम *context,
                  UConverterFromUnicodeArgs *fromUArgs,
                  स्थिर char16_t* codeUnits,
                  int32_t length,
                  UChar32 codePoint,
                  UConverterCallbackReason reason,
                  UErrorCode * err)
{
    (व्योम)context;
    (व्योम)fromUArgs;
    (व्योम)codeUnits;
    (व्योम)length;
    अगर (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_CODE_POINT(codePoint))
    {
        /*
         * Skip if the codepoint has unicode property of default ignorable.
         */
        *err = U_ZERO_ERROR;
    }
    /* the caller must have set the error code accordingly */
}


/*Function Pointer STOPS at the ILLEGAL_SEQUENCE */
U_CAPI व्योम    U_EXPORT2
UCNV_TO_U_CALLBACK_STOP (
                   स्थिर व्योम *context,
                   UConverterToUnicodeArgs *toUArgs,
                   स्थिर अक्षर* codePoints,
                   int32_t length,
                   UConverterCallbackReason reason,
                   UErrorCode * err)
{
    /* the caller must have set the error code accordingly */
    (व्योम)context; (व्योम)toUArgs; (व्योम)codePoints; (व्योम)length; (व्योम)reason; (व्योम)err;
}

U_CAPI व्योम    U_EXPORT2
UCNV_FROM_U_CALLBACK_SKIP (                  
                  स्थिर व्योम *context,
                  UConverterFromUnicodeArgs *fromUArgs,
                  स्थिर char16_t* codeUnits,
                  int32_t length,
                  UChar32 codePoint,
                  UConverterCallbackReason reason,
                  UErrorCode * err)
{
    (व्योम)fromUArgs;
    (व्योम)codeUnits;
    (व्योम)length;
    अगर (reason <= UCNV_IRREGULAR)
    {
        अगर (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_CODE_POINT(codePoint))
        {
            /*
             * Skip if the codepoint has unicode property of default ignorable.
             */
            *err = U_ZERO_ERROR;
        }
        अन्यथा अगर (context == nullptr || (*((अक्षर*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
        {
            *err = U_ZERO_ERROR;
        }
        /* else the caller must have set the error code accordingly. */
    }
    /* else ignore the reset, close and clone calls. */
}

U_CAPI व्योम    U_EXPORT2
UCNV_FROM_U_CALLBACK_SUBSTITUTE (
                  स्थिर व्योम *context,
                  UConverterFromUnicodeArgs *fromArgs,
                  स्थिर char16_t* codeUnits,
                  int32_t length,
                  UChar32 codePoint,
                  UConverterCallbackReason reason,
                  UErrorCode * err)
{
    (व्योम)codeUnits;
    (व्योम)length;
    अगर (reason <= UCNV_IRREGULAR)
    {
        अगर (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_CODE_POINT(codePoint))
        {
            /*
             * Skip if the codepoint has unicode property of default ignorable.
             */
            *err = U_ZERO_ERROR;
        }
        अन्यथा अगर (context == nullptr || (*((अक्षर*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
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
U_CAPI व्योम    U_EXPORT2
UCNV_FROM_U_CALLBACK_ESCAPE (
                         स्थिर व्योम *context,
                         UConverterFromUnicodeArgs *fromArgs,
                         स्थिर char16_t *codeUnits,
                         int32_t length,
                         UChar32 codePoint,
                         UConverterCallbackReason reason,
                         UErrorCode * err)
{

  char16_t मूल्य_String[मूल्य_STRING_लंबाई];
  int32_t मूल्य_String_लंबाई = 0;
  int32_t i = 0;

  स्थिर char16_t *my_मूल्य_Source = nullptr;
  UErrorCode err2 = U_ZERO_ERROR;
  UConverterFromUCallback original = nullptr;
  स्थिर व्योम *originalContext;

  UConverterFromUCallback ignoredCallback = nullptr;
  स्थिर व्योम *ignoredContext;
  
  अगर (reason > UCNV_IRREGULAR)
  {
      वापस;
  }
  अन्यथा अगर (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_CODE_POINT(codePoint))
  {
      /*
       * Skip if the codepoint has unicode property of default ignorable.
       */
      *err = U_ZERO_ERROR;
      वापस;
  }

  ucnv_setFromUCallBack (fromArgs->converter,
                     (UConverterFromUCallback) UCNV_FROM_U_CALLBACK_SUBSTITUTE,
                     nullptr,
                     &original,
                     &originalContext,
                     &err2);
  
  अगर (U_FAILURE (err2))
  {
    *err = err2;
    वापस;
  } 
  अगर(context==nullptr)
  { 
      जबतक (i < length)
      {
        मूल्य_String[मूल्य_String_लंबाई++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT;  /* adding % */
        मूल्य_String[मूल्य_String_लंबाई++] = (char16_t) UNICODE_U_CODEPOINT; /* adding U */
        मूल्य_String_लंबाई += uprv_itou (मूल्य_String + मूल्य_String_लंबाई, मूल्य_STRING_लंबाई - मूल्य_String_लंबाई, (uint16_t)codeUnits[i++], 16, 4);
      }
  }
  अन्यथा
  {
      चयन(*((अक्षर*)context))
      {
      हाल UCNV_PRV_ESCAPE_JAVA:
          जबतक (i < length)
          {
              मूल्य_String[मूल्य_String_लंबाई++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */
              मूल्य_String[मूल्य_String_लंबाई++] = (char16_t) UNICODE_U_निचला_CODEPOINT; /* adding u */
              मूल्य_String_लंबाई += uprv_itou (मूल्य_String + मूल्य_String_लंबाई, मूल्य_STRING_लंबाई - मूल्य_String_लंबाई, (uint16_t)codeUnits[i++], 16, 4);
          }
          अवरोध;

      हाल UCNV_PRV_ESCAPE_C:
          मूल्य_String[मूल्य_String_लंबाई++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */

          अगर(length==2){
              मूल्य_String[मूल्य_String_लंबाई++] = (char16_t) UNICODE_U_CODEPOINT; /* adding U */
              मूल्य_String_लंबाई += uprv_itou (मूल्य_String + मूल्य_String_लंबाई, मूल्य_STRING_लंबाई - मूल्य_String_लंबाई, codePoint, 16, 8);

          }
          अन्यथा{
              मूल्य_String[मूल्य_String_लंबाई++] = (char16_t) UNICODE_U_निचला_CODEPOINT; /* adding u */
              मूल्य_String_लंबाई += uprv_itou (मूल्य_String + मूल्य_String_लंबाई, मूल्य_STRING_लंबाई - मूल्य_String_लंबाई, (uint16_t)codeUnits[0], 16, 4);
          }
          अवरोध;

      हाल UCNV_PRV_ESCAPE_XML_DEC:

          मूल्य_String[मूल्य_String_लंबाई++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
          मूल्य_String[मूल्य_String_लंबाई++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
          अगर(length==2){
              मूल्य_String_लंबाई += uprv_itou (मूल्य_String + मूल्य_String_लंबाई, मूल्य_STRING_लंबाई - मूल्य_String_लंबाई, codePoint, 10, 0);
          }
          अन्यथा{
              मूल्य_String_लंबाई += uprv_itou (मूल्य_String + मूल्य_String_लंबाई, मूल्य_STRING_लंबाई - मूल्य_String_लंबाई, (uint16_t)codeUnits[0], 10, 0);
          }
          मूल्य_String[मूल्य_String_लंबाई++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
          अवरोध;

      हाल UCNV_PRV_ESCAPE_XML_HEX:

          मूल्य_String[मूल्य_String_लंबाई++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
          मूल्य_String[मूल्य_String_लंबाई++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
          मूल्य_String[मूल्य_String_लंबाई++] = (char16_t) UNICODE_X_निचला_CODEPOINT; /* adding x */
          अगर(length==2){
              मूल्य_String_लंबाई += uprv_itou (मूल्य_String + मूल्य_String_लंबाई, मूल्य_STRING_लंबाई - मूल्य_String_लंबाई, codePoint, 16, 0);
          }
          अन्यथा{
              मूल्य_String_लंबाई += uprv_itou (मूल्य_String + मूल्य_String_लंबाई, मूल्य_STRING_लंबाई - मूल्य_String_लंबाई, (uint16_t)codeUnits[0], 16, 0);
          }
          मूल्य_String[मूल्य_String_लंबाई++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
          अवरोध;

      हाल UCNV_PRV_ESCAPE_UNICODE:
          मूल्य_String[मूल्य_String_लंबाई++] = (char16_t) UNICODE_बायाँ_CURLY_CODEPOINT;    /* adding { */
          मूल्य_String[मूल्य_String_लंबाई++] = (char16_t) UNICODE_U_CODEPOINT;    /* adding U */
          मूल्य_String[मूल्य_String_लंबाई++] = (char16_t) UNICODE_PLUS_CODEPOINT; /* adding + */
          अगर (length == 2) {
              मूल्य_String_लंबाई += uprv_itou (मूल्य_String + मूल्य_String_लंबाई, मूल्य_STRING_लंबाई - मूल्य_String_लंबाई, codePoint, 16, 4);
          } अन्यथा {
              मूल्य_String_लंबाई += uprv_itou (मूल्य_String + मूल्य_String_लंबाई, मूल्य_STRING_लंबाई - मूल्य_String_लंबाई, (uint16_t)codeUnits[0], 16, 4);
          }
          मूल्य_String[मूल्य_String_लंबाई++] = (char16_t) UNICODE_दायाँ_CURLY_CODEPOINT;    /* adding } */
          अवरोध;

      हाल UCNV_PRV_ESCAPE_CSS2:
          मूल्य_String[मूल्य_String_लंबाई++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */
          मूल्य_String_लंबाई += uprv_itou (मूल्य_String + मूल्य_String_लंबाई, मूल्य_STRING_लंबाई - मूल्य_String_लंबाई, codePoint, 16, 0);
          /* Always add space character, because the next character might be whitespace,
             which would erroneously be considered the termination of the escape sequence. */
          मूल्य_String[मूल्य_String_लंबाई++] = (char16_t) UNICODE_SPACE_CODEPOINT;
          अवरोध;

      शेष:
          जबतक (i < length)
          {
              मूल्य_String[मूल्य_String_लंबाई++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT;  /* adding % */
              मूल्य_String[मूल्य_String_लंबाई++] = (char16_t) UNICODE_U_CODEPOINT;             /* adding U */
              मूल्य_String_लंबाई += uprv_itou (मूल्य_String + मूल्य_String_लंबाई, मूल्य_STRING_लंबाई - मूल्य_String_लंबाई, (uint16_t)codeUnits[i++], 16, 4);
          }
      }
  }  
  my_मूल्य_Source = मूल्य_String;

  /* reset the error */
  *err = U_ZERO_ERROR;

  ucnv_cbFromUWriteUChars(fromArgs, &my_मूल्य_Source, my_मूल्य_Source+मूल्य_String_लंबाई, 0, err);

  ucnv_setFromUCallBack (fromArgs->converter,
                         original,
                         originalContext,
                         &ignoredCallback,
                         &ignoredContext,
                         &err2);
  अगर (U_FAILURE (err2))
  {
      *err = err2;
      वापस;
  }
}



U_CAPI व्योम  U_EXPORT2
UCNV_TO_U_CALLBACK_SKIP (
                 स्थिर व्योम *context,
                 UConverterToUnicodeArgs *toArgs,
                 स्थिर अक्षर* codeUnits,
                 int32_t length,
                 UConverterCallbackReason reason,
                 UErrorCode * err)
{
    (व्योम)toArgs;
    (व्योम)codeUnits;
    (व्योम)length;
    अगर (reason <= UCNV_IRREGULAR)
    {
        अगर (context == nullptr || (*((अक्षर*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
        {
            *err = U_ZERO_ERROR;
        }
        /* else the caller must have set the error code accordingly. */
    }
    /* else ignore the reset, close and clone calls. */
}

U_CAPI व्योम    U_EXPORT2
UCNV_TO_U_CALLBACK_SUBSTITUTE (
                 स्थिर व्योम *context,
                 UConverterToUnicodeArgs *toArgs,
                 स्थिर अक्षर* codeUnits,
                 int32_t length,
                 UConverterCallbackReason reason,
                 UErrorCode * err)
{
    (व्योम)codeUnits;
    (व्योम)length;
    अगर (reason <= UCNV_IRREGULAR)
    {
        अगर (context == nullptr || (*((अक्षर*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
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
U_CAPI व्योम   U_EXPORT2
UCNV_TO_U_CALLBACK_ESCAPE (
                 स्थिर व्योम *context,
                 UConverterToUnicodeArgs *toArgs,
                 स्थिर अक्षर* codeUnits,
                 int32_t length,
                 UConverterCallbackReason reason,
                 UErrorCode * err)
{
    char16_t uni_मूल्य_String[मूल्य_STRING_लंबाई];
    int32_t मूल्य_String_लंबाई = 0;
    int32_t i = 0;

    अगर (reason > UCNV_IRREGULAR)
    {
        वापस;
    }

    अगर(context==nullptr)
    {    
        जबतक (i < length)
        {
            uni_मूल्य_String[मूल्य_String_लंबाई++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT; /* adding % */
            uni_मूल्य_String[मूल्य_String_लंबाई++] = (char16_t) UNICODE_X_CODEPOINT;    /* adding X */
            मूल्य_String_लंबाई += uprv_itou (uni_मूल्य_String + मूल्य_String_लंबाई, मूल्य_STRING_लंबाई - मूल्य_String_लंबाई, (uint8_t) codeUnits[i++], 16, 2);
        }
    }
    अन्यथा
    {
        चयन(*((अक्षर*)context))
        {
        हाल UCNV_PRV_ESCAPE_XML_DEC:
            जबतक (i < length)
            {
                uni_मूल्य_String[मूल्य_String_लंबाई++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
                uni_मूल्य_String[मूल्य_String_लंबाई++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
                मूल्य_String_लंबाई += uprv_itou (uni_मूल्य_String + मूल्य_String_लंबाई, मूल्य_STRING_लंबाई - मूल्य_String_लंबाई, (uint8_t)codeUnits[i++], 10, 0);
                uni_मूल्य_String[मूल्य_String_लंबाई++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
            }
            अवरोध;

        हाल UCNV_PRV_ESCAPE_XML_HEX:
            जबतक (i < length)
            {
                uni_मूल्य_String[मूल्य_String_लंबाई++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
                uni_मूल्य_String[मूल्य_String_लंबाई++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
                uni_मूल्य_String[मूल्य_String_लंबाई++] = (char16_t) UNICODE_X_निचला_CODEPOINT; /* adding x */
                मूल्य_String_लंबाई += uprv_itou (uni_मूल्य_String + मूल्य_String_लंबाई, मूल्य_STRING_लंबाई - मूल्य_String_लंबाई, (uint8_t)codeUnits[i++], 16, 0);
                uni_मूल्य_String[मूल्य_String_लंबाई++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
            }
            अवरोध;
        हाल UCNV_PRV_ESCAPE_C:
            जबतक (i < length)
            {
                uni_मूल्य_String[मूल्य_String_लंबाई++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */
                uni_मूल्य_String[मूल्य_String_लंबाई++] = (char16_t) UNICODE_X_निचला_CODEPOINT; /* adding x */
                मूल्य_String_लंबाई += uprv_itou (uni_मूल्य_String + मूल्य_String_लंबाई, मूल्य_STRING_लंबाई - मूल्य_String_लंबाई, (uint8_t)codeUnits[i++], 16, 2);
            }
            अवरोध;
        शेष:
            जबतक (i < length)
            {
                uni_मूल्य_String[मूल्य_String_लंबाई++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT; /* adding % */
                uni_मूल्य_String[मूल्य_String_लंबाई++] = (char16_t) UNICODE_X_CODEPOINT;    /* adding X */
                uprv_itou (uni_मूल्य_String + मूल्य_String_लंबाई, मूल्य_STRING_लंबाई - मूल्य_String_लंबाई, (uint8_t) codeUnits[i++], 16, 2);
                मूल्य_String_लंबाई += 2;
            }
        }
    }
    /* reset the error */
    *err = U_ZERO_ERROR;

    ucnv_cbToUWriteUChars(toArgs, uni_मूल्य_String, मूल्य_String_लंबाई, 0, err);
}

#पूर्ण_अगर
