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
UnicodeString::doCaseCompare(int32_t شروعات,
                             int32_t length,
                             ثابت char16_t *srcChars,
                             int32_t src_شروعات,
                             int32_t src_ڊيگهائي,
                             uint32_t options) ثابت
{
  // compare illegal string values
  // treat const char16_t *srcChars==nullptr as an empty string
  إذا(isBogus()) {
    أرجع -1;
  }

  // pin indices to legal values
  pinIndices(شروعات, length);

  إذا(srcChars == خالي_اشارو) {
    src_شروعات = src_ڊيگهائي = 0;
  }

  // get the correct pointer
  ثابت char16_t *chars = get_Array_شروعات();

  chars += شروعات;
  إذا(src_شروعات!=0) {
    srcChars += src_شروعات;
  }

  إذا(chars != srcChars) {
    UError_ڪوڊ error_ڪوڊ=U_ZERO_ERROR;
    int32_t نتيجو=u_strcmpFold(chars, length, srcChars, src_ڊيگهائي,
                                options|U_COMPARE_IGNORE_CASE, &error_ڪوڊ);
    إذا(نتيجو!=0) {
      أرجع static_cast<int8_t>(نتيجو >> 24 | 1);
    }
  } وإلا {
    // get the srcLength if necessary
    إذا(src_ڊيگهائي < 0) {
      src_ڊيگهائي = u_strlen(srcChars + src_شروعات);
    }
    إذا(length != src_ڊيگهائي) {
      أرجع static_cast<int8_t>((length - src_ڊيگهائي) >> 24 | 1);
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
  إذا(is_خالي_جايون() || !isWritable()) {
    // nothing to do
    أرجع *هذا;
  }

  char16_t old_بفر[2 * US_STACKBUF_سائيز];
  char16_t *oldArray;
  int32_t old_ڊيگهائي = length();
  int32_t new_ڊيگهائي;
  UBool writable = is_بفر_Writable();
  UError_ڪوڊ error_ڪوڊ = U_ZERO_ERROR;

#إذا !UCONFIG_NO_BREAK_ITERATION
  // Read-only alias to the original string contents for the titlecasing BreakIterator.
  // We cannot set the iterator simply to *this because *this is being modified.
  UnicodeString oldString;
#جيڪڏهن_پورو

  // Try to avoid heap-allocating a new character array for this string.
  إذا (writable ? old_ڊيگهائي <= UPRV_LENGTHOF(old_بفر) : old_ڊيگهائي < US_STACKBUF_سائيز) {
    // Short string: Copy the contents into a temporary buffer and
    // case-map back into the current array, or into the stack buffer.
    char16_t *بفر = get_Array_شروعات();
    int32_t capacity;
    oldArray = old_بفر;
    u_memcpy(old_بفر, بفر, old_ڊيگهائي);
    إذا (writable) {
      capacity = get_صلاحيت();
    } وإلا {
      // Switch from the read-only alias or shared heap buffer to the stack buffer.
      إذا (!cloneArrayIfNeeded(US_STACKBUF_سائيز, US_STACKBUF_سائيز, /* doCopyArray= */ خطأ)) {
        أرجع *هذا;
      }
      U_ASSERT(fUnion.fFields.f_ڊيگهائي_And_Flags & k_Using_ڍير_بفر);
      بفر = fUnion.f_ڍير_Fields.f_بفر;
      capacity = US_STACKBUF_سائيز;
    }
#إذا !UCONFIG_NO_BREAK_ITERATION
    إذا (iter != خالي_اشارو) {
      oldString.سيٽ_منزل(خطأ, oldArray, old_ڊيگهائي);
      iter->سيٽ_متن(oldString);
    }
#جيڪڏهن_پورو
    new_ڊيگهائي = stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                 بفر, capacity,
                                 oldArray, old_ڊيگهائي, خالي_اشارو, error_ڪوڊ);
    إذا (U_SUCCESS(error_ڪوڊ)) {
      سيٽ_ڊيگهائي(new_ڊيگهائي);
      أرجع *هذا;
    } وإلا إذا (error_ڪوڊ == U_بفر_حد_کان_مٿي_ERROR) {
      // common overflow handling below
    } وإلا {
      سيٽ_منزل_Bogus();
      أرجع *هذا;
    }
  } وإلا {
    // Longer string or read-only buffer:
    // Collect only changes and then apply them to this string.
    // Case mapping often changes only small parts of a string,
    // and often does not change its length.
    oldArray = get_Array_شروعات();
    Edits edits;
    char16_t replacementChars[200];
#إذا !UCONFIG_NO_BREAK_ITERATION
    إذا (iter != خالي_اشارو) {
      oldString.سيٽ_منزل(خطأ, oldArray, old_ڊيگهائي);
      iter->سيٽ_متن(oldString);
    }
#جيڪڏهن_پورو
    stringCaseMapper(caseLocale, options | U_OMIT_UNCHANGED_متن, UCASEMAP_BREAK_ITERATOR
                     replacementChars, UPRV_LENGTHOF(replacementChars),
                     oldArray, old_ڊيگهائي, &edits, error_ڪوڊ);
    إذا (U_SUCCESS(error_ڪوڊ)) {
      // Grow the buffer at most once, not for multiple doReplace() calls.
      new_ڊيگهائي = old_ڊيگهائي + edits.ڊيگهائي_Delta();
      إذا (new_ڊيگهائي > old_ڊيگهائي && !cloneArrayIfNeeded(new_ڊيگهائي, new_ڊيگهائي)) {
        أرجع *هذا;
      }
      لكل (Edits::Iterator ei = edits.getCoarseChangesIterator(); ei.next(error_ڪوڊ);) {
        doReplace(ei.destination_اشاريو(), ei.old_ڊيگهائي(),
                  replacementChars, ei.replacement_اشاريو(), ei.new_ڊيگهائي());
      }
      إذا (U_FAILURE(error_ڪوڊ)) {
        سيٽ_منزل_Bogus();
      }
      أرجع *هذا;
    } وإلا إذا (error_ڪوڊ == U_بفر_حد_کان_مٿي_ERROR) {
      // common overflow handling below
      new_ڊيگهائي = old_ڊيگهائي + edits.ڊيگهائي_Delta();
    } وإلا {
      سيٽ_منزل_Bogus();
      أرجع *هذا;
    }
  }

  // Handle buffer overflow, newLength is known.
  // We need to allocate a new buffer for the internal string case mapping function.
  // This is very similar to how doReplace() keeps the old array pointer
  // and deletes the old array itself after it is done.
  // In addition, we are forcing cloneArrayIfNeeded() to always allocate a new array.
  int32_t *بفر_منزل_Delete = خالي_اشارو;
  إذا (!cloneArrayIfNeeded(new_ڊيگهائي, new_ڊيگهائي, خطأ, &بفر_منزل_Delete, سچ)) {
    أرجع *هذا;
  }
  error_ڪوڊ = U_ZERO_ERROR;
  // No need to iter->setText() again: The case mapper restarts via iter->first().
  new_ڊيگهائي = stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                               get_Array_شروعات(), get_صلاحيت(),
                               oldArray, old_ڊيگهائي, خالي_اشارو, error_ڪوڊ);
  إذا (بفر_منزل_Delete) {
    uprv_free(بفر_منزل_Delete);
  }
  إذا (U_SUCCESS(error_ڪوڊ)) {
    سيٽ_ڊيگهائي(new_ڊيگهائي);
  } وإلا {
    سيٽ_منزل_Bogus();
  }
  أرجع *هذا;
}

UnicodeString&
UnicodeString::foldCase(uint32_t options) U_LIFETIME_BOUND {
  أرجع caseMap(UCASE_LOC_پاڙ, options, UCASEMAP_BREAK_ITERATOR_NULL ustrcase_اندروني_Fold);
}

U_NAMESPACE_پڇاڙي

// Defined here to reduce dependencies on break iterator
U_CAPI int32_t U_EXPORT2
uhash_hashCaselessUnicodeString(ثابت UElement ڪنجي) {
    U_NAMESPACE_USE
    ثابت UnicodeString *str = (ثابت UnicodeString*) ڪنجي.اشارو;
    إذا (str == خالي_اشارو) {
        أرجع 0;
    }
    // Inefficient; a better way would be to have a hash function in
    // UnicodeString that does case folding on the fly.
    UnicodeString copy(*str);
    أرجع copy.foldCase().hash_ڪوڊ();
}

// Defined here to reduce dependencies on break iterator
U_CAPI UBool U_EXPORT2
uhash_compareCaselessUnicodeString(ثابت UElement key1, ثابت UElement key2) {
    U_NAMESPACE_USE
    ثابت UnicodeString *str1 = (ثابت UnicodeString*) key1.اشارو;
    ثابت UnicodeString *str2 = (ثابت UnicodeString*) key2.اشارو;
    إذا (str1 == str2) {
        أرجع سچ;
    }
    إذا (str1 == خالي_اشارو || str2 == خالي_اشارو) {
        أرجع خطأ;
    }
    أرجع str1->caseCompare(*str2, U_FOLD_CASE_DEFAULT) == 0;
}
