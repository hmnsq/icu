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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/brkiter.h"
#ಅನ್ತರ್ಭಾವ "unicode/casemap.h"
#ಅನ್ತರ್ಭಾವ "unicode/edits.h"
#ಅನ್ತರ್ಭಾವ "unicode/putil.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "unicode/uchar.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "ucasemap_imp.h"
#ಅನ್ತರ್ಭಾವ "uelement.h"

U_NAMESPACE_BEGIN

//========================================
// Read-only implementation
//========================================

int8_t
UnicodeString::doCaseCompare(int32_t ಆರಂಭ,
                             int32_t length,
                             ಸ್ಥಿರಾಂಕ char16_t *srcChars,
                             int32_t src_ಆರಂಭ,
                             int32_t src_ಉದ್ದಳತೆ,
                             uint32_t options) ಸ್ಥಿರಾಂಕ
{
  // compare illegal string values
  // treat const char16_t *srcChars==nullptr as an empty string
  ಆದರೆ(isBogus()) {
    ಹಿಂತಿರುಗಿ -1;
  }

  // pin indices to legal values
  pinIndices(ಆರಂಭ, length);

  ಆದರೆ(srcChars == ಶೂನ್ಯಸೂಚಕ) {
    src_ಆರಂಭ = src_ಉದ್ದಳತೆ = 0;
  }

  // get the correct pointer
  ಸ್ಥಿರಾಂಕ char16_t *chars = get_Array_ಆರಂಭ();

  chars += ಆರಂಭ;
  ಆದರೆ(src_ಆರಂಭ!=0) {
    srcChars += src_ಆರಂಭ;
  }

  ಆದರೆ(chars != srcChars) {
    UError_ಸಂಕೇತ error_ಸಂಕೇತ=U_ZERO_ERROR;
    int32_t ಫಲಿತಾಂಶ=u_strcmpFold(chars, length, srcChars, src_ಉದ್ದಳತೆ,
                                options|U_COMPARE_IGNORE_CASE, &error_ಸಂಕೇತ);
    ಆದರೆ(ಫಲಿತಾಂಶ!=0) {
      ಹಿಂತಿರುಗಿ static_cast<int8_t>(ಫಲಿತಾಂಶ >> 24 | 1);
    }
  } ಇಲ್ಲದಿದ್ದರೆ {
    // get the srcLength if necessary
    ಆದರೆ(src_ಉದ್ದಳತೆ < 0) {
      src_ಉದ್ದಳತೆ = u_strlen(srcChars + src_ಆರಂಭ);
    }
    ಆದರೆ(length != src_ಉದ್ದಳತೆ) {
      ಹಿಂತಿರುಗಿ static_cast<int8_t>((length - src_ಉದ್ದಳತೆ) >> 24 | 1);
    }
  }
  ಹಿಂತಿರುಗಿ 0;
}

//========================================
// Write implementation
//========================================

UnicodeString&
UnicodeString::caseMap(
    int32_t caseLocale,
    uint32_t options,
    UCASEMAP_BREAK_ITERATOR_PARAM UStringCaseMapper* stringCaseMapper) U_LIFETIME_BOUND {
  ಆದರೆ(is_ಖಾಲಿಗಳು() || !isWritable()) {
    // nothing to do
    ಹಿಂತಿರುಗಿ *ಇದು;
  }

  char16_t old_ಬಫರ್[2 * US_STACKBUF_ಗಾತ್ರ];
  char16_t *oldArray;
  int32_t old_ಉದ್ದಳತೆ = length();
  int32_t new_ಉದ್ದಳತೆ;
  UBool writable = is_ಬಫರ್_Writable();
  UError_ಸಂಕೇತ error_ಸಂಕೇತ = U_ZERO_ERROR;

#ಆದರೆ !UCONFIG_NO_BREAK_ITERATION
  // Read-only alias to the original string contents for the titlecasing BreakIterator.
  // We cannot set the iterator simply to *this because *this is being modified.
  UnicodeString oldString;
#ಆದರೆ_ಅಂತ್ಯ

  // Try to avoid heap-allocating a new character array for this string.
  ಆದರೆ (writable ? old_ಉದ್ದಳತೆ <= UPRV_LENGTHOF(old_ಬಫರ್) : old_ಉದ್ದಳತೆ < US_STACKBUF_ಗಾತ್ರ) {
    // Short string: Copy the contents into a temporary buffer and
    // case-map back into the current array, or into the stack buffer.
    char16_t *ಬಫರ್ = get_Array_ಆರಂಭ();
    int32_t capacity;
    oldArray = old_ಬಫರ್;
    u_memcpy(old_ಬಫರ್, ಬಫರ್, old_ಉದ್ದಳತೆ);
    ಆದರೆ (writable) {
      capacity = get_ಸಾಮರ್ಥ್ಯ();
    } ಇಲ್ಲದಿದ್ದರೆ {
      // Switch from the read-only alias or shared heap buffer to the stack buffer.
      ಆದರೆ (!cloneArrayIfNeeded(US_STACKBUF_ಗಾತ್ರ, US_STACKBUF_ಗಾತ್ರ, /* doCopyArray= */ ಸುಳ್ಳು)) {
        ಹಿಂತಿರುಗಿ *ಇದು;
      }
      U_ASSERT(fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags & k_Using_ರಾಶಿ_ಬಫರ್);
      ಬಫರ್ = fUnion.f_ರಾಶಿ_Fields.f_ಬಫರ್;
      capacity = US_STACKBUF_ಗಾತ್ರ;
    }
#ಆದರೆ !UCONFIG_NO_BREAK_ITERATION
    ಆದರೆ (iter != ಶೂನ್ಯಸೂಚಕ) {
      oldString.ಗಣ_ಗಮ್ಯ(ಸುಳ್ಳು, oldArray, old_ಉದ್ದಳತೆ);
      iter->ಗಣ_ಪಠ್ಯ(oldString);
    }
#ಆದರೆ_ಅಂತ್ಯ
    new_ಉದ್ದಳತೆ = stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                 ಬಫರ್, capacity,
                                 oldArray, old_ಉದ್ದಳತೆ, ಶೂನ್ಯಸೂಚಕ, error_ಸಂಕೇತ);
    ಆದರೆ (U_SUCCESS(error_ಸಂಕೇತ)) {
      ಗಣ_ಉದ್ದಳತೆ(new_ಉದ್ದಳತೆ);
      ಹಿಂತಿರುಗಿ *ಇದು;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (error_ಸಂಕೇತ == U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR) {
      // common overflow handling below
    } ಇಲ್ಲದಿದ್ದರೆ {
      ಗಣ_ಗಮ್ಯ_Bogus();
      ಹಿಂತಿರುಗಿ *ಇದು;
    }
  } ಇಲ್ಲದಿದ್ದರೆ {
    // Longer string or read-only buffer:
    // Collect only changes and then apply them to this string.
    // Case mapping often changes only small parts of a string,
    // and often does not change its length.
    oldArray = get_Array_ಆರಂಭ();
    Edits edits;
    char16_t replacementChars[200];
#ಆದರೆ !UCONFIG_NO_BREAK_ITERATION
    ಆದರೆ (iter != ಶೂನ್ಯಸೂಚಕ) {
      oldString.ಗಣ_ಗಮ್ಯ(ಸುಳ್ಳು, oldArray, old_ಉದ್ದಳತೆ);
      iter->ಗಣ_ಪಠ್ಯ(oldString);
    }
#ಆದರೆ_ಅಂತ್ಯ
    stringCaseMapper(caseLocale, options | U_OMIT_UNCHANGED_ಪಠ್ಯ, UCASEMAP_BREAK_ITERATOR
                     replacementChars, UPRV_LENGTHOF(replacementChars),
                     oldArray, old_ಉದ್ದಳತೆ, &edits, error_ಸಂಕೇತ);
    ಆದರೆ (U_SUCCESS(error_ಸಂಕೇತ)) {
      // Grow the buffer at most once, not for multiple doReplace() calls.
      new_ಉದ್ದಳತೆ = old_ಉದ್ದಳತೆ + edits.ಉದ್ದಳತೆ_Delta();
      ಆದರೆ (new_ಉದ್ದಳತೆ > old_ಉದ್ದಳತೆ && !cloneArrayIfNeeded(new_ಉದ್ದಳತೆ, new_ಉದ್ದಳತೆ)) {
        ಹಿಂತಿರುಗಿ *ಇದು;
      }
      ಗಾಗಿ (Edits::Iterator ei = edits.getCoarseChangesIterator(); ei.next(error_ಸಂಕೇತ);) {
        doReplace(ei.destination_ಸೂಚ್ಯಂಕ(), ei.old_ಉದ್ದಳತೆ(),
                  replacementChars, ei.replacement_ಸೂಚ್ಯಂಕ(), ei.new_ಉದ್ದಳತೆ());
      }
      ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) {
        ಗಣ_ಗಮ್ಯ_Bogus();
      }
      ಹಿಂತಿರುಗಿ *ಇದು;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (error_ಸಂಕೇತ == U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR) {
      // common overflow handling below
      new_ಉದ್ದಳತೆ = old_ಉದ್ದಳತೆ + edits.ಉದ್ದಳತೆ_Delta();
    } ಇಲ್ಲದಿದ್ದರೆ {
      ಗಣ_ಗಮ್ಯ_Bogus();
      ಹಿಂತಿರುಗಿ *ಇದು;
    }
  }

  // Handle buffer overflow, newLength is known.
  // We need to allocate a new buffer for the internal string case mapping function.
  // This is very similar to how doReplace() keeps the old array pointer
  // and deletes the old array itself after it is done.
  // In addition, we are forcing cloneArrayIfNeeded() to always allocate a new array.
  int32_t *ಬಫರ್_ಗಮ್ಯ_Delete = ಶೂನ್ಯಸೂಚಕ;
  ಆದರೆ (!cloneArrayIfNeeded(new_ಉದ್ದಳತೆ, new_ಉದ್ದಳತೆ, ಸುಳ್ಳು, &ಬಫರ್_ಗಮ್ಯ_Delete, ನಿಜ)) {
    ಹಿಂತಿರುಗಿ *ಇದು;
  }
  error_ಸಂಕೇತ = U_ZERO_ERROR;
  // No need to iter->setText() again: The case mapper restarts via iter->first().
  new_ಉದ್ದಳತೆ = stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                               get_Array_ಆರಂಭ(), get_ಸಾಮರ್ಥ್ಯ(),
                               oldArray, old_ಉದ್ದಳತೆ, ಶೂನ್ಯಸೂಚಕ, error_ಸಂಕೇತ);
  ಆದರೆ (ಬಫರ್_ಗಮ್ಯ_Delete) {
    uprv_free(ಬಫರ್_ಗಮ್ಯ_Delete);
  }
  ಆದರೆ (U_SUCCESS(error_ಸಂಕೇತ)) {
    ಗಣ_ಉದ್ದಳತೆ(new_ಉದ್ದಳತೆ);
  } ಇಲ್ಲದಿದ್ದರೆ {
    ಗಣ_ಗಮ್ಯ_Bogus();
  }
  ಹಿಂತಿರುಗಿ *ಇದು;
}

UnicodeString&
UnicodeString::foldCase(uint32_t options) U_LIFETIME_BOUND {
  ಹಿಂತಿರುಗಿ caseMap(UCASE_LOC_ಬೇರು, options, UCASEMAP_BREAK_ITERATOR_NULL ustrcase_ಆಂತರಿಕ_Fold);
}

U_NAMESPACE_ಕೊನೆ

// Defined here to reduce dependencies on break iterator
U_CAPI int32_t U_EXPORT2
uhash_hashCaselessUnicodeString(ಸ್ಥಿರಾಂಕ UElement ಕೀಲಿ) {
    U_NAMESPACE_USE
    ಸ್ಥಿರಾಂಕ UnicodeString *str = (ಸ್ಥಿರಾಂಕ UnicodeString*) ಕೀಲಿ.ಸೂಚಕ;
    ಆದರೆ (str == ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ 0;
    }
    // Inefficient; a better way would be to have a hash function in
    // UnicodeString that does case folding on the fly.
    UnicodeString copy(*str);
    ಹಿಂತಿರುಗಿ copy.foldCase().hash_ಸಂಕೇತ();
}

// Defined here to reduce dependencies on break iterator
U_CAPI UBool U_EXPORT2
uhash_compareCaselessUnicodeString(ಸ್ಥಿರಾಂಕ UElement key1, ಸ್ಥಿರಾಂಕ UElement key2) {
    U_NAMESPACE_USE
    ಸ್ಥಿರಾಂಕ UnicodeString *str1 = (ಸ್ಥಿರಾಂಕ UnicodeString*) key1.ಸೂಚಕ;
    ಸ್ಥಿರಾಂಕ UnicodeString *str2 = (ಸ್ಥಿರಾಂಕ UnicodeString*) key2.ಸೂಚಕ;
    ಆದರೆ (str1 == str2) {
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    ಆದರೆ (str1 == ಶೂನ್ಯಸೂಚಕ || str2 == ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಹಿಂತಿರುಗಿ str1->caseCompare(*str2, U_FOLD_CASE_DEFAULT) == 0;
}
