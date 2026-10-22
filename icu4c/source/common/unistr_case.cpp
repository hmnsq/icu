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
UnicodeString::doCaseCompare(int32_t پیل,
                             int32_t length,
                             ثابت char16_t *srcChars,
                             int32_t src_پیل,
                             int32_t src_اوږدتیا,
                             uint32_t options) ثابت
{
  // compare illegal string values
  // treat const char16_t *srcChars==nullptr as an empty string
  إذا(isBogus()) {
    أرجع -1;
  }

  // pin indices to legal values
  pinIndices(پیل, length);

  إذا(srcChars == تش_اشاره) {
    src_پیل = src_اوږدتیا = 0;
  }

  // get the correct pointer
  ثابت char16_t *chars = get_Array_پیل();

  chars += پیل;
  إذا(src_پیل!=0) {
    srcChars += src_پیل;
  }

  إذا(chars != srcChars) {
    UError_کوډ error_کوډ=U_ZERO_ERROR;
    int32_t پایله=u_strcmpFold(chars, length, srcChars, src_اوږدتیا,
                                options|U_COMPARE_IGNORE_CASE, &error_کوډ);
    إذا(پایله!=0) {
      أرجع static_cast<int8_t>(پایله >> 24 | 1);
    }
  } وإلا {
    // get the srcLength if necessary
    إذا(src_اوږدتیا < 0) {
      src_اوږدتیا = u_strlen(srcChars + src_پیل);
    }
    إذا(length != src_اوږدتیا) {
      أرجع static_cast<int8_t>((length - src_اوږدتیا) >> 24 | 1);
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
  إذا(is_تشه() || !isWritable()) {
    // nothing to do
    أرجع *هذا;
  }

  char16_t old_بفر[2 * US_STACKBUF_اندازه];
  char16_t *oldArray;
  int32_t old_اوږدتیا = length();
  int32_t new_اوږدتیا;
  UBool writable = is_بفر_Writable();
  UError_کوډ error_کوډ = U_ZERO_ERROR;

#إذا !UCONFIG_NO_BREAK_ITERATION
  // Read-only alias to the original string contents for the titlecasing BreakIterator.
  // We cannot set the iterator simply to *this because *this is being modified.
  UnicodeString oldString;
#د_که_پای

  // Try to avoid heap-allocating a new character array for this string.
  إذا (writable ? old_اوږدتیا <= UPRV_LENGTHOF(old_بفر) : old_اوږدتیا < US_STACKBUF_اندازه) {
    // Short string: Copy the contents into a temporary buffer and
    // case-map back into the current array, or into the stack buffer.
    char16_t *بفر = get_Array_پیل();
    int32_t capacity;
    oldArray = old_بفر;
    u_memcpy(old_بفر, بفر, old_اوږدتیا);
    إذا (writable) {
      capacity = get_ظرفیت();
    } وإلا {
      // Switch from the read-only alias or shared heap buffer to the stack buffer.
      إذا (!cloneArrayIfNeeded(US_STACKBUF_اندازه, US_STACKBUF_اندازه, /* doCopyArray= */ خطأ)) {
        أرجع *هذا;
      }
      U_ASSERT(fUnion.fFields.f_اوږدتیا_And_Flags & k_Using_ډېری_بفر);
      بفر = fUnion.f_ډېری_Fields.f_بفر;
      capacity = US_STACKBUF_اندازه;
    }
#إذا !UCONFIG_NO_BREAK_ITERATION
    إذا (iter != تش_اشاره) {
      oldString.سېټ_منزل(خطأ, oldArray, old_اوږدتیا);
      iter->سېټ_متن(oldString);
    }
#د_که_پای
    new_اوږدتیا = stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                 بفر, capacity,
                                 oldArray, old_اوږدتیا, تش_اشاره, error_کوډ);
    إذا (U_SUCCESS(error_کوډ)) {
      سېټ_اوږدتیا(new_اوږدتیا);
      أرجع *هذا;
    } وإلا إذا (error_کوډ == U_بفر_ډکېدنه_ERROR) {
      // common overflow handling below
    } وإلا {
      سېټ_منزل_Bogus();
      أرجع *هذا;
    }
  } وإلا {
    // Longer string or read-only buffer:
    // Collect only changes and then apply them to this string.
    // Case mapping often changes only small parts of a string,
    // and often does not change its length.
    oldArray = get_Array_پیل();
    Edits edits;
    char16_t replacementChars[200];
#إذا !UCONFIG_NO_BREAK_ITERATION
    إذا (iter != تش_اشاره) {
      oldString.سېټ_منزل(خطأ, oldArray, old_اوږدتیا);
      iter->سېټ_متن(oldString);
    }
#د_که_پای
    stringCaseMapper(caseLocale, options | U_OMIT_UNCHANGED_متن, UCASEMAP_BREAK_ITERATOR
                     replacementChars, UPRV_LENGTHOF(replacementChars),
                     oldArray, old_اوږدتیا, &edits, error_کوډ);
    إذا (U_SUCCESS(error_کوډ)) {
      // Grow the buffer at most once, not for multiple doReplace() calls.
      new_اوږدتیا = old_اوږدتیا + edits.اوږدتیا_Delta();
      إذا (new_اوږدتیا > old_اوږدتیا && !cloneArrayIfNeeded(new_اوږدتیا, new_اوږدتیا)) {
        أرجع *هذا;
      }
      لكل (Edits::Iterator ei = edits.getCoarseChangesIterator(); ei.next(error_کوډ);) {
        doReplace(ei.destination_شاخص(), ei.old_اوږدتیا(),
                  replacementChars, ei.replacement_شاخص(), ei.new_اوږدتیا());
      }
      إذا (U_FAILURE(error_کوډ)) {
        سېټ_منزل_Bogus();
      }
      أرجع *هذا;
    } وإلا إذا (error_کوډ == U_بفر_ډکېدنه_ERROR) {
      // common overflow handling below
      new_اوږدتیا = old_اوږدتیا + edits.اوږدتیا_Delta();
    } وإلا {
      سېټ_منزل_Bogus();
      أرجع *هذا;
    }
  }

  // Handle buffer overflow, newLength is known.
  // We need to allocate a new buffer for the internal string case mapping function.
  // This is very similar to how doReplace() keeps the old array pointer
  // and deletes the old array itself after it is done.
  // In addition, we are forcing cloneArrayIfNeeded() to always allocate a new array.
  int32_t *بفر_منزل_Delete = تش_اشاره;
  إذا (!cloneArrayIfNeeded(new_اوږدتیا, new_اوږدتیا, خطأ, &بفر_منزل_Delete, ریښتیا)) {
    أرجع *هذا;
  }
  error_کوډ = U_ZERO_ERROR;
  // No need to iter->setText() again: The case mapper restarts via iter->first().
  new_اوږدتیا = stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                               get_Array_پیل(), get_ظرفیت(),
                               oldArray, old_اوږدتیا, تش_اشاره, error_کوډ);
  إذا (بفر_منزل_Delete) {
    uprv_free(بفر_منزل_Delete);
  }
  إذا (U_SUCCESS(error_کوډ)) {
    سېټ_اوږدتیا(new_اوږدتیا);
  } وإلا {
    سېټ_منزل_Bogus();
  }
  أرجع *هذا;
}

UnicodeString&
UnicodeString::foldCase(uint32_t options) U_LIFETIME_BOUND {
  أرجع caseMap(UCASE_LOC_ریښه, options, UCASEMAP_BREAK_ITERATOR_NULL ustrcase_دننی_Fold);
}

U_NAMESPACE_پای_ټکی

// Defined here to reduce dependencies on break iterator
U_CAPI int32_t U_EXPORT2
uhash_hashCaselessUnicodeString(ثابت UElement کلی) {
    U_NAMESPACE_USE
    ثابت UnicodeString *str = (ثابت UnicodeString*) کلی.اشاره_ګر;
    إذا (str == تش_اشاره) {
        أرجع 0;
    }
    // Inefficient; a better way would be to have a hash function in
    // UnicodeString that does case folding on the fly.
    UnicodeString copy(*str);
    أرجع copy.foldCase().hash_کوډ();
}

// Defined here to reduce dependencies on break iterator
U_CAPI UBool U_EXPORT2
uhash_compareCaselessUnicodeString(ثابت UElement key1, ثابت UElement key2) {
    U_NAMESPACE_USE
    ثابت UnicodeString *str1 = (ثابت UnicodeString*) key1.اشاره_ګر;
    ثابت UnicodeString *str2 = (ثابت UnicodeString*) key2.اشاره_ګر;
    إذا (str1 == str2) {
        أرجع ریښتیا;
    }
    إذا (str1 == تش_اشاره || str2 == تش_اشاره) {
        أرجع خطأ;
    }
    أرجع str1->caseCompare(*str2, U_FOLD_CASE_DEFAULT) == 0;
}
