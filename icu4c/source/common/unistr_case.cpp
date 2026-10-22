// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 1999-2014, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  unistr_case.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:2
*
*   created on: 2004aug19
*   created by: Markus W. Scherer
*
*   Case-mapping functions moved here from unistr.cpp
*/

#شامل "unicode/utypes.h"
#شامل "unicode/brkiter.h"
#شامل "unicode/casemap.h"
#شامل "unicode/edits.h"
#شامل "unicode/putil.h"
#شامل "cstring.h"
#شامل "cmemory.h"
#شامل "unicode/ustring.h"
#شامل "unicode/unistr.h"
#شامل "unicode/uchar.h"
#شامل "uassert.h"
#شامل "ucasemap_imp.h"
#شامل "uelement.h"

U_NAMESPACE_BEGIN

//========================================
// Read-only implementation
//========================================

int8_t
UnicodeString::doCaseCompare(int32_t آغاز,
                             int32_t length,
                             ثابت char16_t *srcChars,
                             int32_t src_آغاز,
                             int32_t src_لمبائی,
                             uint32_t options) ثابت
{
  // compare illegal string values
  // treat const char16_t *srcChars==nullptr as an empty string
  اگر(isBogus()) {
    واپس -1;
  }

  // pin indices to legal values
  pinIndices(آغاز, length);

  اگر(srcChars == خالی_اشاریہ) {
    src_آغاز = src_لمبائی = 0;
  }

  // get the correct pointer
  ثابت char16_t *chars = get_Array_آغاز();

  chars += آغاز;
  اگر(src_آغاز!=0) {
    srcChars += src_آغاز;
  }

  اگر(chars != srcChars) {
    UError_رمز error_رمز=U_ZERO_ERROR;
    int32_t نتیجہ=u_strcmpFold(chars, length, srcChars, src_لمبائی,
                                options|U_COMPARE_IGNORE_CASE, &error_رمز);
    اگر(نتیجہ!=0) {
      واپس static_cast<int8_t>(نتیجہ >> 24 | 1);
    }
  } ورنہ {
    // get the srcLength if necessary
    اگر(src_لمبائی < 0) {
      src_لمبائی = u_strlen(srcChars + src_آغاز);
    }
    اگر(length != src_لمبائی) {
      واپس static_cast<int8_t>((length - src_لمبائی) >> 24 | 1);
    }
  }
  واپس 0;
}

//========================================
// Write implementation
//========================================

UnicodeString&
UnicodeString::caseMap(
    int32_t caseLocale,
    uint32_t options,
    UCASEMAP_BREAK_ITERATOR_PARAM UStringCaseMapper* stringCaseMapper) U_LIFETIME_BOUND {
  اگر(is_خالی_خانے() || !isWritable()) {
    // nothing to do
    واپس *یہ;
  }

  char16_t old_بفر[2 * US_STACKBUF_سائز];
  char16_t *oldArray;
  int32_t old_لمبائی = length();
  int32_t new_لمبائی;
  UBool writable = is_بفر_Writable();
  UError_رمز error_رمز = U_ZERO_ERROR;

#اگر !UCONFIG_NO_BREAK_ITERATION
  // Read-only alias to the original string contents for the titlecasing BreakIterator.
  // We cannot set the iterator simply to *this because *this is being modified.
  UnicodeString oldString;
#اگر_ختم

  // Try to avoid heap-allocating a new character array for this string.
  اگر (writable ? old_لمبائی <= UPRV_LENGTHOF(old_بفر) : old_لمبائی < US_STACKBUF_سائز) {
    // Short string: Copy the contents into a temporary buffer and
    // case-map back into the current array, or into the stack buffer.
    char16_t *بفر = get_Array_آغاز();
    int32_t capacity;
    oldArray = old_بفر;
    u_memcpy(old_بفر, بفر, old_لمبائی);
    اگر (writable) {
      capacity = get_استعداد();
    } ورنہ {
      // Switch from the read-only alias or shared heap buffer to the stack buffer.
      اگر (!cloneArrayIfNeeded(US_STACKBUF_سائز, US_STACKBUF_سائز, /* doCopyArray= */ جھوٹ)) {
        واپس *یہ;
      }
      U_ASSERT(fUnion.fFields.f_لمبائی_And_Flags & k_Using_ڈھیر_بفر);
      بفر = fUnion.f_ڈھیر_Fields.f_بفر;
      capacity = US_STACKBUF_سائز;
    }
#اگر !UCONFIG_NO_BREAK_ITERATION
    اگر (iter != خالی_اشاریہ) {
      oldString.سیٹ_مقصد(جھوٹ, oldArray, old_لمبائی);
      iter->سیٹ_متن(oldString);
    }
#اگر_ختم
    new_لمبائی = stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                 بفر, capacity,
                                 oldArray, old_لمبائی, خالی_اشاریہ, error_رمز);
    اگر (U_SUCCESS(error_رمز)) {
      سیٹ_لمبائی(new_لمبائی);
      واپس *یہ;
    } ورنہ اگر (error_رمز == U_بفر_حد_سے_تجاوز_ERROR) {
      // common overflow handling below
    } ورنہ {
      سیٹ_مقصد_Bogus();
      واپس *یہ;
    }
  } ورنہ {
    // Longer string or read-only buffer:
    // Collect only changes and then apply them to this string.
    // Case mapping often changes only small parts of a string,
    // and often does not change its length.
    oldArray = get_Array_آغاز();
    Edits edits;
    char16_t replacementChars[200];
#اگر !UCONFIG_NO_BREAK_ITERATION
    اگر (iter != خالی_اشاریہ) {
      oldString.سیٹ_مقصد(جھوٹ, oldArray, old_لمبائی);
      iter->سیٹ_متن(oldString);
    }
#اگر_ختم
    stringCaseMapper(caseLocale, options | U_OMIT_UNCHANGED_متن, UCASEMAP_BREAK_ITERATOR
                     replacementChars, UPRV_LENGTHOF(replacementChars),
                     oldArray, old_لمبائی, &edits, error_رمز);
    اگر (U_SUCCESS(error_رمز)) {
      // Grow the buffer at most once, not for multiple doReplace() calls.
      new_لمبائی = old_لمبائی + edits.لمبائی_Delta();
      اگر (new_لمبائی > old_لمبائی && !cloneArrayIfNeeded(new_لمبائی, new_لمبائی)) {
        واپس *یہ;
      }
      برائے (Edits::Iterator ei = edits.getCoarseChangesIterator(); ei.next(error_رمز);) {
        doReplace(ei.destination_اشاریہ(), ei.old_لمبائی(),
                  replacementChars, ei.replacement_اشاریہ(), ei.new_لمبائی());
      }
      اگر (U_FAILURE(error_رمز)) {
        سیٹ_مقصد_Bogus();
      }
      واپس *یہ;
    } ورنہ اگر (error_رمز == U_بفر_حد_سے_تجاوز_ERROR) {
      // common overflow handling below
      new_لمبائی = old_لمبائی + edits.لمبائی_Delta();
    } ورنہ {
      سیٹ_مقصد_Bogus();
      واپس *یہ;
    }
  }

  // Handle buffer overflow, newLength is known.
  // We need to allocate a new buffer for the internal string case mapping function.
  // This is very similar to how doReplace() keeps the old array pointer
  // and deletes the old array itself after it is done.
  // In addition, we are forcing cloneArrayIfNeeded() to always allocate a new array.
  int32_t *بفر_مقصد_Delete = خالی_اشاریہ;
  اگر (!cloneArrayIfNeeded(new_لمبائی, new_لمبائی, جھوٹ, &بفر_مقصد_Delete, سچ)) {
    واپس *یہ;
  }
  error_رمز = U_ZERO_ERROR;
  // No need to iter->setText() again: The case mapper restarts via iter->first().
  new_لمبائی = stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                               get_Array_آغاز(), get_استعداد(),
                               oldArray, old_لمبائی, خالی_اشاریہ, error_رمز);
  اگر (بفر_مقصد_Delete) {
    uprv_free(بفر_مقصد_Delete);
  }
  اگر (U_SUCCESS(error_رمز)) {
    سیٹ_لمبائی(new_لمبائی);
  } ورنہ {
    سیٹ_مقصد_Bogus();
  }
  واپس *یہ;
}

UnicodeString&
UnicodeString::foldCase(uint32_t options) U_LIFETIME_BOUND {
  واپس caseMap(UCASE_LOC_جڑ, options, UCASEMAP_BREAK_ITERATOR_NULL ustrcase_اندرونی_Fold);
}

U_NAMESPACE_انتہا

// Defined here to reduce dependencies on break iterator
U_CAPI int32_t U_EXPORT2
uhash_hashCaselessUnicodeString(ثابت UElement کلید) {
    U_NAMESPACE_USE
    ثابت UnicodeString *str = (ثابت UnicodeString*) کلید.اشارہ_گر;
    اگر (str == خالی_اشاریہ) {
        واپس 0;
    }
    // Inefficient; a better way would be to have a hash function in
    // UnicodeString that does case folding on the fly.
    UnicodeString copy(*str);
    واپس copy.foldCase().hash_رمز();
}

// Defined here to reduce dependencies on break iterator
U_CAPI UBool U_EXPORT2
uhash_compareCaselessUnicodeString(ثابت UElement key1, ثابت UElement key2) {
    U_NAMESPACE_USE
    ثابت UnicodeString *str1 = (ثابت UnicodeString*) key1.اشارہ_گر;
    ثابت UnicodeString *str2 = (ثابت UnicodeString*) key2.اشارہ_گر;
    اگر (str1 == str2) {
        واپس سچ;
    }
    اگر (str1 == خالی_اشاریہ || str2 == خالی_اشاریہ) {
        واپس جھوٹ;
    }
    واپس str1->caseCompare(*str2, U_FOLD_CASE_DEFAULT) == 0;
}
