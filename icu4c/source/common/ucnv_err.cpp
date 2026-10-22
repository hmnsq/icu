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

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_CONVERSION

#अन्तर्भाव "unicode/ucnv_err.h"
#अन्तर्भाव "unicode/ucnv_cb.h"
#अन्तर्भाव "ucnv_cnv.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "unicode/ucnv.h"
#अन्तर्भाव "ustrfmt.h"

#परिभाषय मानम्_STRING_दीर्घता 48
/*Magic # 32 = 4(number of char in value string) * 8(max number of bytes per char for any converter) */
#परिभाषय UNICODE_PERCENT_SIGN_CODEPOINT  0x0025
#परिभाषय UNICODE_U_CODEPOINT             0x0055
#परिभाषय UNICODE_X_CODEPOINT             0x0058
#परिभाषय UNICODE_RS_CODEPOINT            0x005C
#परिभाषय UNICODE_U_नीचः_CODEPOINT         0x0075
#परिभाषय UNICODE_X_नीचः_CODEPOINT         0x0078
#परिभाषय UNICODE_AMP_CODEPOINT           0x0026
#परिभाषय UNICODE_HASH_CODEPOINT          0x0023
#परिभाषय UNICODE_SEMICOLON_CODEPOINT     0x003B
#परिभाषय UNICODE_PLUS_CODEPOINT          0x002B
#परिभाषय UNICODE_वामः_CURLY_CODEPOINT    0x007B
#परिभाषय UNICODE_दक्षिणः_CURLY_CODEPOINT   0x007D
#परिभाषय UNICODE_SPACE_CODEPOINT         0x0020
#परिभाषय UCNV_PRV_ESCAPE_ICU         0
#परिभाषय UCNV_PRV_ESCAPE_C           'C'
#परिभाषय UCNV_PRV_ESCAPE_XML_DEC     'D'
#परिभाषय UCNV_PRV_ESCAPE_XML_HEX     'X'
#परिभाषय UCNV_PRV_ESCAPE_JAVA        'J'
#परिभाषय UCNV_PRV_ESCAPE_UNICODE     'U'
#परिभाषय UCNV_PRV_ESCAPE_CSS2        'S'
#परिभाषय UCNV_PRV_STOP_ON_ILLEGAL    'i'

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
#परिभाषय IS_DEFAULT_IGNORABLE_संहिता_POINT(c) ( \
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
U_CAPI शून्य    U_EXPORT2
UCNV_FROM_U_CALLBACK_STOP (
                  ध्रुव शून्य *context,
                  UConverter_From_Unicode_तर्काः *fromUArgs,
                  ध्रुव char16_t* संहिता_Units,
                  int32_t length,
                  UChar32 संहिता_Point,
                  UConverterCallbackReason reason,
                  UError_संहिता * err)
{
    (शून्य)context;
    (शून्य)fromUArgs;
    (शून्य)संहिता_Units;
    (शून्य)length;
    यदि (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_संहिता_POINT(संहिता_Point))
    {
        /*
         * Skip if the codepoint has unicode property of default ignorable.
         */
        *err = U_ZERO_ERROR;
    }
    /* the caller must have set the error code accordingly */
}


/*Function Pointer STOPS at the ILLEGAL_SEQUENCE */
U_CAPI शून्य    U_EXPORT2
UCNV_गन्तव्यम्_U_CALLBACK_STOP (
                   ध्रुव शून्य *context,
                   UConverter_गन्तव्यम्_Unicode_तर्काः *गन्तव्यम्_UArgs,
                   ध्रुव वर्ण* संहिता_Points,
                   int32_t length,
                   UConverterCallbackReason reason,
                   UError_संहिता * err)
{
    /* the caller must have set the error code accordingly */
    (शून्य)context; (शून्य)गन्तव्यम्_UArgs; (शून्य)संहिता_Points; (शून्य)length; (शून्य)reason; (शून्य)err;
}

U_CAPI शून्य    U_EXPORT2
UCNV_FROM_U_CALLBACK_SKIP (                  
                  ध्रुव शून्य *context,
                  UConverter_From_Unicode_तर्काः *fromUArgs,
                  ध्रुव char16_t* संहिता_Units,
                  int32_t length,
                  UChar32 संहिता_Point,
                  UConverterCallbackReason reason,
                  UError_संहिता * err)
{
    (शून्य)fromUArgs;
    (शून्य)संहिता_Units;
    (शून्य)length;
    यदि (reason <= UCNV_IRREGULAR)
    {
        यदि (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_संहिता_POINT(संहिता_Point))
        {
            /*
             * Skip if the codepoint has unicode property of default ignorable.
             */
            *err = U_ZERO_ERROR;
        }
        अन्यथा यदि (context == शून्यसूचकः || (*((वर्ण*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
        {
            *err = U_ZERO_ERROR;
        }
        /* else the caller must have set the error code accordingly. */
    }
    /* else ignore the reset, close and clone calls. */
}

U_CAPI शून्य    U_EXPORT2
UCNV_FROM_U_CALLBACK_SUBSTITUTE (
                  ध्रुव शून्य *context,
                  UConverter_From_Unicode_तर्काः *from_तर्काः,
                  ध्रुव char16_t* संहिता_Units,
                  int32_t length,
                  UChar32 संहिता_Point,
                  UConverterCallbackReason reason,
                  UError_संहिता * err)
{
    (शून्य)संहिता_Units;
    (शून्य)length;
    यदि (reason <= UCNV_IRREGULAR)
    {
        यदि (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_संहिता_POINT(संहिता_Point))
        {
            /*
             * Skip if the codepoint has unicode property of default ignorable.
             */
            *err = U_ZERO_ERROR;
        }
        अन्यथा यदि (context == शून्यसूचकः || (*((वर्ण*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
        {
            *err = U_ZERO_ERROR;
            ucnv_cbFromUWriteSub(from_तर्काः, 0, err);
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
U_CAPI शून्य    U_EXPORT2
UCNV_FROM_U_CALLBACK_ESCAPE (
                         ध्रुव शून्य *context,
                         UConverter_From_Unicode_तर्काः *from_तर्काः,
                         ध्रुव char16_t *संहिता_Units,
                         int32_t length,
                         UChar32 संहिता_Point,
                         UConverterCallbackReason reason,
                         UError_संहिता * err)
{

  char16_t मानम्_String[मानम्_STRING_दीर्घता];
  int32_t मानम्_String_दीर्घता = 0;
  int32_t i = 0;

  ध्रुव char16_t *my_मानम्_Source = शून्यसूचकः;
  UError_संहिता err2 = U_ZERO_ERROR;
  UConverterFromUCallback original = शून्यसूचकः;
  ध्रुव शून्य *originalContext;

  UConverterFromUCallback ignoredCallback = शून्यसूचकः;
  ध्रुव शून्य *ignoredContext;
  
  यदि (reason > UCNV_IRREGULAR)
  {
      प्रत्यागम;
  }
  अन्यथा यदि (reason == UCNV_UNASSIGNED && IS_DEFAULT_IGNORABLE_संहिता_POINT(संहिता_Point))
  {
      /*
       * Skip if the codepoint has unicode property of default ignorable.
       */
      *err = U_ZERO_ERROR;
      प्रत्यागम;
  }

  ucnv_समुच्चयः_From_UCall_पृष्ठभागः (from_तर्काः->converter,
                     (UConverterFromUCallback) UCNV_FROM_U_CALLBACK_SUBSTITUTE,
                     शून्यसूचकः,
                     &original,
                     &originalContext,
                     &err2);
  
  यदि (U_FAILURE (err2))
  {
    *err = err2;
    प्रत्यागम;
  } 
  यदि(context==शून्यसूचकः)
  { 
      यावत् (i < length)
      {
        मानम्_String[मानम्_String_दीर्घता++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT;  /* adding % */
        मानम्_String[मानम्_String_दीर्घता++] = (char16_t) UNICODE_U_CODEPOINT; /* adding U */
        मानम्_String_दीर्घता += uprv_itou (मानम्_String + मानम्_String_दीर्घता, मानम्_STRING_दीर्घता - मानम्_String_दीर्घता, (uint16_t)संहिता_Units[i++], 16, 4);
      }
  }
  अन्यथा
  {
      विकल्प(*((वर्ण*)context))
      {
      पक्ष UCNV_PRV_ESCAPE_JAVA:
          यावत् (i < length)
          {
              मानम्_String[मानम्_String_दीर्घता++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */
              मानम्_String[मानम्_String_दीर्घता++] = (char16_t) UNICODE_U_नीचः_CODEPOINT; /* adding u */
              मानम्_String_दीर्घता += uprv_itou (मानम्_String + मानम्_String_दीर्घता, मानम्_STRING_दीर्घता - मानम्_String_दीर्घता, (uint16_t)संहिता_Units[i++], 16, 4);
          }
          भङ्ग;

      पक्ष UCNV_PRV_ESCAPE_C:
          मानम्_String[मानम्_String_दीर्घता++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */

          यदि(length==2){
              मानम्_String[मानम्_String_दीर्घता++] = (char16_t) UNICODE_U_CODEPOINT; /* adding U */
              मानम्_String_दीर्घता += uprv_itou (मानम्_String + मानम्_String_दीर्घता, मानम्_STRING_दीर्घता - मानम्_String_दीर्घता, संहिता_Point, 16, 8);

          }
          अन्यथा{
              मानम्_String[मानम्_String_दीर्घता++] = (char16_t) UNICODE_U_नीचः_CODEPOINT; /* adding u */
              मानम्_String_दीर्घता += uprv_itou (मानम्_String + मानम्_String_दीर्घता, मानम्_STRING_दीर्घता - मानम्_String_दीर्घता, (uint16_t)संहिता_Units[0], 16, 4);
          }
          भङ्ग;

      पक्ष UCNV_PRV_ESCAPE_XML_DEC:

          मानम्_String[मानम्_String_दीर्घता++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
          मानम्_String[मानम्_String_दीर्घता++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
          यदि(length==2){
              मानम्_String_दीर्घता += uprv_itou (मानम्_String + मानम्_String_दीर्घता, मानम्_STRING_दीर्घता - मानम्_String_दीर्घता, संहिता_Point, 10, 0);
          }
          अन्यथा{
              मानम्_String_दीर्घता += uprv_itou (मानम्_String + मानम्_String_दीर्घता, मानम्_STRING_दीर्घता - मानम्_String_दीर्घता, (uint16_t)संहिता_Units[0], 10, 0);
          }
          मानम्_String[मानम्_String_दीर्घता++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
          भङ्ग;

      पक्ष UCNV_PRV_ESCAPE_XML_HEX:

          मानम्_String[मानम्_String_दीर्घता++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
          मानम्_String[मानम्_String_दीर्घता++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
          मानम्_String[मानम्_String_दीर्घता++] = (char16_t) UNICODE_X_नीचः_CODEPOINT; /* adding x */
          यदि(length==2){
              मानम्_String_दीर्घता += uprv_itou (मानम्_String + मानम्_String_दीर्घता, मानम्_STRING_दीर्घता - मानम्_String_दीर्घता, संहिता_Point, 16, 0);
          }
          अन्यथा{
              मानम्_String_दीर्घता += uprv_itou (मानम्_String + मानम्_String_दीर्घता, मानम्_STRING_दीर्घता - मानम्_String_दीर्घता, (uint16_t)संहिता_Units[0], 16, 0);
          }
          मानम्_String[मानम्_String_दीर्घता++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
          भङ्ग;

      पक्ष UCNV_PRV_ESCAPE_UNICODE:
          मानम्_String[मानम्_String_दीर्घता++] = (char16_t) UNICODE_वामः_CURLY_CODEPOINT;    /* adding { */
          मानम्_String[मानम्_String_दीर्घता++] = (char16_t) UNICODE_U_CODEPOINT;    /* adding U */
          मानम्_String[मानम्_String_दीर्घता++] = (char16_t) UNICODE_PLUS_CODEPOINT; /* adding + */
          यदि (length == 2) {
              मानम्_String_दीर्घता += uprv_itou (मानम्_String + मानम्_String_दीर्घता, मानम्_STRING_दीर्घता - मानम्_String_दीर्घता, संहिता_Point, 16, 4);
          } अन्यथा {
              मानम्_String_दीर्घता += uprv_itou (मानम्_String + मानम्_String_दीर्घता, मानम्_STRING_दीर्घता - मानम्_String_दीर्घता, (uint16_t)संहिता_Units[0], 16, 4);
          }
          मानम्_String[मानम्_String_दीर्घता++] = (char16_t) UNICODE_दक्षिणः_CURLY_CODEPOINT;    /* adding } */
          भङ्ग;

      पक्ष UCNV_PRV_ESCAPE_CSS2:
          मानम्_String[मानम्_String_दीर्घता++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */
          मानम्_String_दीर्घता += uprv_itou (मानम्_String + मानम्_String_दीर्घता, मानम्_STRING_दीर्घता - मानम्_String_दीर्घता, संहिता_Point, 16, 0);
          /* Always add space character, because the next character might be whitespace,
             which would erroneously be considered the termination of the escape sequence. */
          मानम्_String[मानम्_String_दीर्घता++] = (char16_t) UNICODE_SPACE_CODEPOINT;
          भङ्ग;

      स्वभाव:
          यावत् (i < length)
          {
              मानम्_String[मानम्_String_दीर्घता++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT;  /* adding % */
              मानम्_String[मानम्_String_दीर्घता++] = (char16_t) UNICODE_U_CODEPOINT;             /* adding U */
              मानम्_String_दीर्घता += uprv_itou (मानम्_String + मानम्_String_दीर्घता, मानम्_STRING_दीर्घता - मानम्_String_दीर्घता, (uint16_t)संहिता_Units[i++], 16, 4);
          }
      }
  }  
  my_मानम्_Source = मानम्_String;

  /* reset the error */
  *err = U_ZERO_ERROR;

  ucnv_cbFromUWriteUChars(from_तर्काः, &my_मानम्_Source, my_मानम्_Source+मानम्_String_दीर्घता, 0, err);

  ucnv_समुच्चयः_From_UCall_पृष्ठभागः (from_तर्काः->converter,
                         original,
                         originalContext,
                         &ignoredCallback,
                         &ignoredContext,
                         &err2);
  यदि (U_FAILURE (err2))
  {
      *err = err2;
      प्रत्यागम;
  }
}



U_CAPI शून्य  U_EXPORT2
UCNV_गन्तव्यम्_U_CALLBACK_SKIP (
                 ध्रुव शून्य *context,
                 UConverter_गन्तव्यम्_Unicode_तर्काः *गन्तव्यम्_तर्काः,
                 ध्रुव वर्ण* संहिता_Units,
                 int32_t length,
                 UConverterCallbackReason reason,
                 UError_संहिता * err)
{
    (शून्य)गन्तव्यम्_तर्काः;
    (शून्य)संहिता_Units;
    (शून्य)length;
    यदि (reason <= UCNV_IRREGULAR)
    {
        यदि (context == शून्यसूचकः || (*((वर्ण*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
        {
            *err = U_ZERO_ERROR;
        }
        /* else the caller must have set the error code accordingly. */
    }
    /* else ignore the reset, close and clone calls. */
}

U_CAPI शून्य    U_EXPORT2
UCNV_गन्तव्यम्_U_CALLBACK_SUBSTITUTE (
                 ध्रुव शून्य *context,
                 UConverter_गन्तव्यम्_Unicode_तर्काः *गन्तव्यम्_तर्काः,
                 ध्रुव वर्ण* संहिता_Units,
                 int32_t length,
                 UConverterCallbackReason reason,
                 UError_संहिता * err)
{
    (शून्य)संहिता_Units;
    (शून्य)length;
    यदि (reason <= UCNV_IRREGULAR)
    {
        यदि (context == शून्यसूचकः || (*((वर्ण*)context) == UCNV_PRV_STOP_ON_ILLEGAL && reason == UCNV_UNASSIGNED))
        {
            *err = U_ZERO_ERROR;
            ucnv_cb_गन्तव्यम्_UWrite_Sub(गन्तव्यम्_तर्काः,0,err);
        }
        /* else the caller must have set the error code accordingly. */
    }
    /* else ignore the reset, close and clone calls. */
}

/*uses uprv_itou to get a unicode escape sequence of the offensive sequence,
 *and uses that as the substitution sequence
 */
U_CAPI शून्य   U_EXPORT2
UCNV_गन्तव्यम्_U_CALLBACK_ESCAPE (
                 ध्रुव शून्य *context,
                 UConverter_गन्तव्यम्_Unicode_तर्काः *गन्तव्यम्_तर्काः,
                 ध्रुव वर्ण* संहिता_Units,
                 int32_t length,
                 UConverterCallbackReason reason,
                 UError_संहिता * err)
{
    char16_t uni_मानम्_String[मानम्_STRING_दीर्घता];
    int32_t मानम्_String_दीर्घता = 0;
    int32_t i = 0;

    यदि (reason > UCNV_IRREGULAR)
    {
        प्रत्यागम;
    }

    यदि(context==शून्यसूचकः)
    {    
        यावत् (i < length)
        {
            uni_मानम्_String[मानम्_String_दीर्घता++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT; /* adding % */
            uni_मानम्_String[मानम्_String_दीर्घता++] = (char16_t) UNICODE_X_CODEPOINT;    /* adding X */
            मानम्_String_दीर्घता += uprv_itou (uni_मानम्_String + मानम्_String_दीर्घता, मानम्_STRING_दीर्घता - मानम्_String_दीर्घता, (uint8_t) संहिता_Units[i++], 16, 2);
        }
    }
    अन्यथा
    {
        विकल्प(*((वर्ण*)context))
        {
        पक्ष UCNV_PRV_ESCAPE_XML_DEC:
            यावत् (i < length)
            {
                uni_मानम्_String[मानम्_String_दीर्घता++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
                uni_मानम्_String[मानम्_String_दीर्घता++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
                मानम्_String_दीर्घता += uprv_itou (uni_मानम्_String + मानम्_String_दीर्घता, मानम्_STRING_दीर्घता - मानम्_String_दीर्घता, (uint8_t)संहिता_Units[i++], 10, 0);
                uni_मानम्_String[मानम्_String_दीर्घता++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
            }
            भङ्ग;

        पक्ष UCNV_PRV_ESCAPE_XML_HEX:
            यावत् (i < length)
            {
                uni_मानम्_String[मानम्_String_दीर्घता++] = (char16_t) UNICODE_AMP_CODEPOINT;   /* adding & */
                uni_मानम्_String[मानम्_String_दीर्घता++] = (char16_t) UNICODE_HASH_CODEPOINT;  /* adding # */
                uni_मानम्_String[मानम्_String_दीर्घता++] = (char16_t) UNICODE_X_नीचः_CODEPOINT; /* adding x */
                मानम्_String_दीर्घता += uprv_itou (uni_मानम्_String + मानम्_String_दीर्घता, मानम्_STRING_दीर्घता - मानम्_String_दीर्घता, (uint8_t)संहिता_Units[i++], 16, 0);
                uni_मानम्_String[मानम्_String_दीर्घता++] = (char16_t) UNICODE_SEMICOLON_CODEPOINT; /* adding ; */
            }
            भङ्ग;
        पक्ष UCNV_PRV_ESCAPE_C:
            यावत् (i < length)
            {
                uni_मानम्_String[मानम्_String_दीर्घता++] = (char16_t) UNICODE_RS_CODEPOINT;    /* adding \ */
                uni_मानम्_String[मानम्_String_दीर्घता++] = (char16_t) UNICODE_X_नीचः_CODEPOINT; /* adding x */
                मानम्_String_दीर्घता += uprv_itou (uni_मानम्_String + मानम्_String_दीर्घता, मानम्_STRING_दीर्घता - मानम्_String_दीर्घता, (uint8_t)संहिता_Units[i++], 16, 2);
            }
            भङ्ग;
        स्वभाव:
            यावत् (i < length)
            {
                uni_मानम्_String[मानम्_String_दीर्घता++] = (char16_t) UNICODE_PERCENT_SIGN_CODEPOINT; /* adding % */
                uni_मानम्_String[मानम्_String_दीर्घता++] = (char16_t) UNICODE_X_CODEPOINT;    /* adding X */
                uprv_itou (uni_मानम्_String + मानम्_String_दीर्घता, मानम्_STRING_दीर्घता - मानम्_String_दीर्घता, (uint8_t) संहिता_Units[i++], 16, 2);
                मानम्_String_दीर्घता += 2;
            }
        }
    }
    /* reset the error */
    *err = U_ZERO_ERROR;

    ucnv_cb_गन्तव्यम्_UWrite_UChars(गन्तव्यम्_तर्काः, uni_मानम्_String, मानम्_String_दीर्घता, 0, err);
}

#यदि_अन्तः
