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

#ضمن "unicode/utypes.h"
#ضمن "unicode/brkiter.h"
#ضمن "unicode/casemap.h"
#ضمن "unicode/edits.h"
#ضمن "unicode/putil.h"
#ضمن "cstring.h"
#ضمن "cmemory.h"
#ضمن "unicode/ustring.h"
#ضمن "unicode/unistr.h"
#ضمن "unicode/uchar.h"
#ضمن "uassert.h"
#ضمن "ucasemap_imp.h"
#ضمن "uelement.h"

U_NAMESPACE_BEGIN

//========================================
// Read-only implementation
//========================================

int8_t
UnicodeString::doCaseCompare(int32_t شروع,
                             int32_t length,
                             ثابت char16_t *srcChars,
                             int32_t src_شروع,
                             int32_t src_لمائی,
                             uint32_t options) ثابت
{
  // compare illegal string values
  // treat const char16_t *srcChars==nullptr as an empty string
  إذا(isBogus()) {
    أرجع -1;
  }

  // pin indices to legal values
  pinIndices(شروع, length);

  إذا(srcChars == خالی_سوچک) {
    src_شروع = src_لمائی = 0;
  }

  // get the correct pointer
  ثابت char16_t *chars = get_Array_شروع();

  chars += شروع;
  إذا(src_شروع!=0) {
    srcChars += src_شروع;
  }

  إذا(chars != srcChars) {
    UError_کوڈ error_کوڈ=U_ZERO_ERROR;
    int32_t نتیجہ=u_strcmpFold(chars, length, srcChars, src_لمائی,
                                options|U_COMPARE_IGNORE_CASE, &error_کوڈ);
    إذا(نتیجہ!=0) {
      أرجع static_cast<int8_t>(نتیجہ >> 24 | 1);
    }
  } وإلا {
    // get the srcLength if necessary
    إذا(src_لمائی < 0) {
      src_لمائی = u_strlen(srcChars + src_شروع);
    }
    إذا(length != src_لمائی) {
      أرجع static_cast<int8_t>((length - src_لمائی) >> 24 | 1);
    }
  }
  أرجع 0;
}

//========================================
// Write implementation
//========================================

UnicodeString&
UnicodeString::caseMap(
    int32_t caseLocale,
    uint32_t options,
    UCASEMAP_BREAK_ITERATOR_PARAM UStringCaseMapper* stringCaseMapper) U_LIFETIME_BOUND {
  إذا(is_خالی_تھاواں() || !isWritable()) {
    // nothing to do
    أرجع *هذا;
  }

  char16_t old_بفر[2 * US_STACKBUF_ناپ];
  char16_t *oldArray;
  int32_t old_لمائی = length();
  int32_t new_لمائی;
  UBool writable = is_بفر_Writable();
  UError_کوڈ error_کوڈ = U_ZERO_ERROR;

#إذا !UCONFIG_NO_BREAK_ITERATION
  // Read-only alias to the original string contents for the titlecasing BreakIterator.
  // We cannot set the iterator simply to *this because *this is being modified.
  UnicodeString oldString;
#جے_انت

  // Try to avoid heap-allocating a new character array for this string.
  إذا (writable ? old_لمائی <= UPRV_LENGTHOF(old_بفر) : old_لمائی < US_STACKBUF_ناپ) {
    // Short string: Copy the contents into a temporary buffer and
    // case-map back into the current array, or into the stack buffer.
    char16_t *بفر = get_Array_شروع();
    int32_t capacity;
    oldArray = old_بفر;
    u_memcpy(old_بفر, بفر, old_لمائی);
    إذا (writable) {
      capacity = get_سمرتھا();
    } وإلا {
      // Switch from the read-only alias or shared heap buffer to the stack buffer.
      إذا (!cloneArrayIfNeeded(US_STACKBUF_ناپ, US_STACKBUF_ناپ, /* doCopyArray= */ خطأ)) {
        أرجع *هذا;
      }
      U_ASSERT(fUnion.fFields.f_لمائی_And_Flags & k_Using_ڈھیر_بفر);
      بفر = fUnion.f_ڈھیر_Fields.f_بفر;
      capacity = US_STACKBUF_ناپ;
    }
#إذا !UCONFIG_NO_BREAK_ITERATION
    إذا (iter != خالی_سوچک) {
      oldString.سیٹ_منزل(خطأ, oldArray, old_لمائی);
      iter->سیٹ_متن(oldString);
    }
#جے_انت
    new_لمائی = stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                 بفر, capacity,
                                 oldArray, old_لمائی, خالی_سوچک, error_کوڈ);
    إذا (U_SUCCESS(error_کوڈ)) {
      سیٹ_لمائی(new_لمائی);
      أرجع *هذا;
    } وإلا إذا (error_کوڈ == U_بفر_حد_توں_ودھ_ERROR) {
      // common overflow handling below
    } وإلا {
      سیٹ_منزل_Bogus();
      أرجع *هذا;
    }
  } وإلا {
    // Longer string or read-only buffer:
    // Collect only changes and then apply them to this string.
    // Case mapping often changes only small parts of a string,
    // and often does not change its length.
    oldArray = get_Array_شروع();
    Edits edits;
    char16_t replacementChars[200];
#إذا !UCONFIG_NO_BREAK_ITERATION
    إذا (iter != خالی_سوچک) {
      oldString.سیٹ_منزل(خطأ, oldArray, old_لمائی);
      iter->سیٹ_متن(oldString);
    }
#جے_انت
    stringCaseMapper(caseLocale, options | U_OMIT_UNCHANGED_متن, UCASEMAP_BREAK_ITERATOR
                     replacementChars, UPRV_LENGTHOF(replacementChars),
                     oldArray, old_لمائی, &edits, error_کوڈ);
    إذا (U_SUCCESS(error_کوڈ)) {
      // Grow the buffer at most once, not for multiple doReplace() calls.
      new_لمائی = old_لمائی + edits.لمائی_Delta();
      إذا (new_لمائی > old_لمائی && !cloneArrayIfNeeded(new_لمائی, new_لمائی)) {
        أرجع *هذا;
      }
      لكل (Edits::Iterator ei = edits.getCoarseChangesIterator(); ei.next(error_کوڈ);) {
        doReplace(ei.destination_اشاریہ(), ei.old_لمائی(),
                  replacementChars, ei.replacement_اشاریہ(), ei.new_لمائی());
      }
      إذا (U_FAILURE(error_کوڈ)) {
        سیٹ_منزل_Bogus();
      }
      أرجع *هذا;
    } وإلا إذا (error_کوڈ == U_بفر_حد_توں_ودھ_ERROR) {
      // common overflow handling below
      new_لمائی = old_لمائی + edits.لمائی_Delta();
    } وإلا {
      سیٹ_منزل_Bogus();
      أرجع *هذا;
    }
  }

  // Handle buffer overflow, newLength is known.
  // We need to allocate a new buffer for the internal string case mapping function.
  // This is very similar to how doReplace() keeps the old array pointer
  // and deletes the old array itself after it is done.
  // In addition, we are forcing cloneArrayIfNeeded() to always allocate a new array.
  int32_t *بفر_منزل_Delete = خالی_سوچک;
  إذا (!cloneArrayIfNeeded(new_لمائی, new_لمائی, خطأ, &بفر_منزل_Delete, سچ)) {
    أرجع *هذا;
  }
  error_کوڈ = U_ZERO_ERROR;
  // No need to iter->setText() again: The case mapper restarts via iter->first().
  new_لمائی = stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                               get_Array_شروع(), get_سمرتھا(),
                               oldArray, old_لمائی, خالی_سوچک, error_کوڈ);
  إذا (بفر_منزل_Delete) {
    uprv_free(بفر_منزل_Delete);
  }
  إذا (U_SUCCESS(error_کوڈ)) {
    سیٹ_لمائی(new_لمائی);
  } وإلا {
    سیٹ_منزل_Bogus();
  }
  أرجع *هذا;
}

UnicodeString&
UnicodeString::foldCase(uint32_t options) U_LIFETIME_BOUND {
  أرجع caseMap(UCASE_LOC_جڑ, options, UCASEMAP_BREAK_ITERATOR_NULL ustrcase_اندرونی_Fold);
}

U_NAMESPACE_سرا

// Defined here to reduce dependencies on break iterator
U_CAPI int32_t U_EXPORT2
uhash_hashCaselessUnicodeString(ثابت UElement کلید) {
    U_NAMESPACE_USE
    ثابت UnicodeString *str = (ثابت UnicodeString*) کلید.اشارہ_گر;
    إذا (str == خالی_سوچک) {
        أرجع 0;
    }
    // Inefficient; a better way would be to have a hash function in
    // UnicodeString that does case folding on the fly.
    UnicodeString copy(*str);
    أرجع copy.foldCase().hash_کوڈ();
}

// Defined here to reduce dependencies on break iterator
U_CAPI UBool U_EXPORT2
uhash_compareCaselessUnicodeString(ثابت UElement key1, ثابت UElement key2) {
    U_NAMESPACE_USE
    ثابت UnicodeString *str1 = (ثابت UnicodeString*) key1.اشارہ_گر;
    ثابت UnicodeString *str2 = (ثابت UnicodeString*) key2.اشارہ_گر;
    إذا (str1 == str2) {
        أرجع سچ;
    }
    إذا (str1 == خالی_سوچک || str2 == خالی_سوچک) {
        أرجع خطأ;
    }
    أرجع str1->caseCompare(*str2, U_FOLD_CASE_DEFAULT) == 0;
}
