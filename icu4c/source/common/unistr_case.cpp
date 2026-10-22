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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/brkiter.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/casemap.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/edits.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/putil.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unistr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uchar.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucasemap_imp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uelement.h"

U_NAMESPACE_BEGIN

//========================================
// Read-only implementation
//========================================

int8_t
UnicodeString::doCaseCompare(int32_t ਸ਼ੁਰੂ,
                             int32_t length,
                             ਅਚਲ char16_t *srcChars,
                             int32_t src_ਸ਼ੁਰੂ,
                             int32_t src_ਲੰਮਾਈ,
                             uint32_t options) ਅਚਲ
{
  // compare illegal string values
  // treat const char16_t *srcChars==nullptr as an empty string
  ਜੇ(isBogus()) {
    ਵਾਪਸ -1;
  }

  // pin indices to legal values
  pinIndices(ਸ਼ੁਰੂ, length);

  ਜੇ(srcChars == ਖਾਲੀ_ਸੂਚਕ) {
    src_ਸ਼ੁਰੂ = src_ਲੰਮਾਈ = 0;
  }

  // get the correct pointer
  ਅਚਲ char16_t *chars = get_Array_ਸ਼ੁਰੂ();

  chars += ਸ਼ੁਰੂ;
  ਜੇ(src_ਸ਼ੁਰੂ!=0) {
    srcChars += src_ਸ਼ੁਰੂ;
  }

  ਜੇ(chars != srcChars) {
    UError_ਕੋਡ error_ਕੋਡ=U_ZERO_ERROR;
    int32_t ਨਤੀਜਾ=u_strcmpFold(chars, length, srcChars, src_ਲੰਮਾਈ,
                                options|U_COMPARE_IGNORE_CASE, &error_ਕੋਡ);
    ਜੇ(ਨਤੀਜਾ!=0) {
      ਵਾਪਸ static_cast<int8_t>(ਨਤੀਜਾ >> 24 | 1);
    }
  } ਨਹੀਂਤਾਂ {
    // get the srcLength if necessary
    ਜੇ(src_ਲੰਮਾਈ < 0) {
      src_ਲੰਮਾਈ = u_strlen(srcChars + src_ਸ਼ੁਰੂ);
    }
    ਜੇ(length != src_ਲੰਮਾਈ) {
      ਵਾਪਸ static_cast<int8_t>((length - src_ਲੰਮਾਈ) >> 24 | 1);
    }
  }
  ਵਾਪਸ 0;
}

//========================================
// Write implementation
//========================================

UnicodeString&
UnicodeString::caseMap(
    int32_t caseLocale,
    uint32_t options,
    UCASEMAP_BREAK_ITERATOR_PARAM UStringCaseMapper* stringCaseMapper) U_LIFETIME_BOUND {
  ਜੇ(is_ਖਾਲੀ_ਥਾਵਾਂ() || !isWritable()) {
    // nothing to do
    ਵਾਪਸ *ਇਹ;
  }

  char16_t old_ਬਫਰ[2 * US_STACKBUF_ਮਾਪ];
  char16_t *oldArray;
  int32_t old_ਲੰਮਾਈ = length();
  int32_t new_ਲੰਮਾਈ;
  UBool writable = is_ਬਫਰ_Writable();
  UError_ਕੋਡ error_ਕੋਡ = U_ZERO_ERROR;

#ਜੇ !UCONFIG_NO_BREAK_ITERATION
  // Read-only alias to the original string contents for the titlecasing BreakIterator.
  // We cannot set the iterator simply to *this because *this is being modified.
  UnicodeString oldString;
#ਜੇ_ਅੰਤ

  // Try to avoid heap-allocating a new character array for this string.
  ਜੇ (writable ? old_ਲੰਮਾਈ <= UPRV_LENGTHOF(old_ਬਫਰ) : old_ਲੰਮਾਈ < US_STACKBUF_ਮਾਪ) {
    // Short string: Copy the contents into a temporary buffer and
    // case-map back into the current array, or into the stack buffer.
    char16_t *ਬਫਰ = get_Array_ਸ਼ੁਰੂ();
    int32_t capacity;
    oldArray = old_ਬਫਰ;
    u_memcpy(old_ਬਫਰ, ਬਫਰ, old_ਲੰਮਾਈ);
    ਜੇ (writable) {
      capacity = get_ਸਮਰੱਥਾ();
    } ਨਹੀਂਤਾਂ {
      // Switch from the read-only alias or shared heap buffer to the stack buffer.
      ਜੇ (!cloneArrayIfNeeded(US_STACKBUF_ਮਾਪ, US_STACKBUF_ਮਾਪ, /* doCopyArray= */ ਝੂਠ)) {
        ਵਾਪਸ *ਇਹ;
      }
      U_ASSERT(fUnion.fFields.f_ਲੰਮਾਈ_And_Flags & k_Using_ਢੇਰ_ਬਫਰ);
      ਬਫਰ = fUnion.f_ਢੇਰ_Fields.f_ਬਫਰ;
      capacity = US_STACKBUF_ਮਾਪ;
    }
#ਜੇ !UCONFIG_NO_BREAK_ITERATION
    ਜੇ (iter != ਖਾਲੀ_ਸੂਚਕ) {
      oldString.ਸੈੱਟ_ਮੰਜ਼ਿਲ(ਝੂਠ, oldArray, old_ਲੰਮਾਈ);
      iter->ਸੈੱਟ_ਪਾਠ(oldString);
    }
#ਜੇ_ਅੰਤ
    new_ਲੰਮਾਈ = stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                 ਬਫਰ, capacity,
                                 oldArray, old_ਲੰਮਾਈ, ਖਾਲੀ_ਸੂਚਕ, error_ਕੋਡ);
    ਜੇ (U_SUCCESS(error_ਕੋਡ)) {
      ਸੈੱਟ_ਲੰਮਾਈ(new_ਲੰਮਾਈ);
      ਵਾਪਸ *ਇਹ;
    } ਨਹੀਂਤਾਂ ਜੇ (error_ਕੋਡ == U_ਬਫਰ_ਉਛਾਲ_ERROR) {
      // common overflow handling below
    } ਨਹੀਂਤਾਂ {
      ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
      ਵਾਪਸ *ਇਹ;
    }
  } ਨਹੀਂਤਾਂ {
    // Longer string or read-only buffer:
    // Collect only changes and then apply them to this string.
    // Case mapping often changes only small parts of a string,
    // and often does not change its length.
    oldArray = get_Array_ਸ਼ੁਰੂ();
    Edits edits;
    char16_t replacementChars[200];
#ਜੇ !UCONFIG_NO_BREAK_ITERATION
    ਜੇ (iter != ਖਾਲੀ_ਸੂਚਕ) {
      oldString.ਸੈੱਟ_ਮੰਜ਼ਿਲ(ਝੂਠ, oldArray, old_ਲੰਮਾਈ);
      iter->ਸੈੱਟ_ਪਾਠ(oldString);
    }
#ਜੇ_ਅੰਤ
    stringCaseMapper(caseLocale, options | U_OMIT_UNCHANGED_ਪਾਠ, UCASEMAP_BREAK_ITERATOR
                     replacementChars, UPRV_LENGTHOF(replacementChars),
                     oldArray, old_ਲੰਮਾਈ, &edits, error_ਕੋਡ);
    ਜੇ (U_SUCCESS(error_ਕੋਡ)) {
      // Grow the buffer at most once, not for multiple doReplace() calls.
      new_ਲੰਮਾਈ = old_ਲੰਮਾਈ + edits.ਲੰਮਾਈ_Delta();
      ਜੇ (new_ਲੰਮਾਈ > old_ਲੰਮਾਈ && !cloneArrayIfNeeded(new_ਲੰਮਾਈ, new_ਲੰਮਾਈ)) {
        ਵਾਪਸ *ਇਹ;
      }
      ਲਈ (Edits::Iterator ei = edits.getCoarseChangesIterator(); ei.next(error_ਕੋਡ);) {
        doReplace(ei.destination_ਸੂਚਕਾਂਕ(), ei.old_ਲੰਮਾਈ(),
                  replacementChars, ei.replacement_ਸੂਚਕਾਂਕ(), ei.new_ਲੰਮਾਈ());
      }
      ਜੇ (U_FAILURE(error_ਕੋਡ)) {
        ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
      }
      ਵਾਪਸ *ਇਹ;
    } ਨਹੀਂਤਾਂ ਜੇ (error_ਕੋਡ == U_ਬਫਰ_ਉਛਾਲ_ERROR) {
      // common overflow handling below
      new_ਲੰਮਾਈ = old_ਲੰਮਾਈ + edits.ਲੰਮਾਈ_Delta();
    } ਨਹੀਂਤਾਂ {
      ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
      ਵਾਪਸ *ਇਹ;
    }
  }

  // Handle buffer overflow, newLength is known.
  // We need to allocate a new buffer for the internal string case mapping function.
  // This is very similar to how doReplace() keeps the old array pointer
  // and deletes the old array itself after it is done.
  // In addition, we are forcing cloneArrayIfNeeded() to always allocate a new array.
  int32_t *ਬਫਰ_ਮੰਜ਼ਿਲ_Delete = ਖਾਲੀ_ਸੂਚਕ;
  ਜੇ (!cloneArrayIfNeeded(new_ਲੰਮਾਈ, new_ਲੰਮਾਈ, ਝੂਠ, &ਬਫਰ_ਮੰਜ਼ਿਲ_Delete, ਸੱਚ)) {
    ਵਾਪਸ *ਇਹ;
  }
  error_ਕੋਡ = U_ZERO_ERROR;
  // No need to iter->setText() again: The case mapper restarts via iter->first().
  new_ਲੰਮਾਈ = stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                               get_Array_ਸ਼ੁਰੂ(), get_ਸਮਰੱਥਾ(),
                               oldArray, old_ਲੰਮਾਈ, ਖਾਲੀ_ਸੂਚਕ, error_ਕੋਡ);
  ਜੇ (ਬਫਰ_ਮੰਜ਼ਿਲ_Delete) {
    uprv_free(ਬਫਰ_ਮੰਜ਼ਿਲ_Delete);
  }
  ਜੇ (U_SUCCESS(error_ਕੋਡ)) {
    ਸੈੱਟ_ਲੰਮਾਈ(new_ਲੰਮਾਈ);
  } ਨਹੀਂਤਾਂ {
    ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
  }
  ਵਾਪਸ *ਇਹ;
}

UnicodeString&
UnicodeString::foldCase(uint32_t options) U_LIFETIME_BOUND {
  ਵਾਪਸ caseMap(UCASE_LOC_ਜੜ੍ਹ, options, UCASEMAP_BREAK_ITERATOR_NULL ustrcase_ਅੰਦਰੂਨੀ_Fold);
}

U_NAMESPACE_ਸਿਰਾ

// Defined here to reduce dependencies on break iterator
U_CAPI int32_t U_EXPORT2
uhash_hashCaselessUnicodeString(ਅਚਲ UElement ਕੁੰਜੀ) {
    U_NAMESPACE_USE
    ਅਚਲ UnicodeString *str = (ਅਚਲ UnicodeString*) ਕੁੰਜੀ.ਸੂਚਕ;
    ਜੇ (str == ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ 0;
    }
    // Inefficient; a better way would be to have a hash function in
    // UnicodeString that does case folding on the fly.
    UnicodeString copy(*str);
    ਵਾਪਸ copy.foldCase().hash_ਕੋਡ();
}

// Defined here to reduce dependencies on break iterator
U_CAPI UBool U_EXPORT2
uhash_compareCaselessUnicodeString(ਅਚਲ UElement key1, ਅਚਲ UElement key2) {
    U_NAMESPACE_USE
    ਅਚਲ UnicodeString *str1 = (ਅਚਲ UnicodeString*) key1.ਸੂਚਕ;
    ਅਚਲ UnicodeString *str2 = (ਅਚਲ UnicodeString*) key2.ਸੂਚਕ;
    ਜੇ (str1 == str2) {
        ਵਾਪਸ ਸੱਚ;
    }
    ਜੇ (str1 == ਖਾਲੀ_ਸੂਚਕ || str2 == ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਵਾਪਸ str1->caseCompare(*str2, U_FOLD_CASE_DEFAULT) == 0;
}
