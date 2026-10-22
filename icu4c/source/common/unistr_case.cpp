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

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/brkiter.h"
#অন্তর্ভাব "unicode/casemap.h"
#অন্তর্ভাব "unicode/edits.h"
#অন্তর্ভাব "unicode/putil.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "unicode/ustring.h"
#অন্তর্ভাব "unicode/unistr.h"
#অন্তর্ভাব "unicode/uchar.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "ucasemap_imp.h"
#অন্তর্ভাব "uelement.h"

U_NAMESPACE_BEGIN

//========================================
// Read-only implementation
//========================================

int8_t
UnicodeString::doCaseCompare(int32_t start,
                             int32_t length,
                             ধ্রুবক char16_t *srcChars,
                             int32_t srcStart,
                             int32_t src_দৈর্ঘ্য,
                             uint32_t options) ধ্রুবক
{
  // compare illegal string values
  // treat const char16_t *srcChars==nullptr as an empty string
  যদি(isBogus()) {
    ফেরত -1;
  }

  // pin indices to legal values
  pinIndices(start, length);

  যদি(srcChars == nullptr) {
    srcStart = src_দৈর্ঘ্য = 0;
  }

  // get the correct pointer
  ধ্রুবক char16_t *chars = getArrayStart();

  chars += start;
  যদি(srcStart!=0) {
    srcChars += srcStart;
  }

  যদি(chars != srcChars) {
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t ফলাফল=u_strcmpFold(chars, length, srcChars, src_দৈর্ঘ্য,
                                options|U_COMPARE_IGNORE_CASE, &errorCode);
    যদি(ফলাফল!=0) {
      ফেরত static_cast<int8_t>(ফলাফল >> 24 | 1);
    }
  } অন্যথা {
    // get the srcLength if necessary
    যদি(src_দৈর্ঘ্য < 0) {
      src_দৈর্ঘ্য = u_strlen(srcChars + srcStart);
    }
    যদি(length != src_দৈর্ঘ্য) {
      ফেরত static_cast<int8_t>((length - src_দৈর্ঘ্য) >> 24 | 1);
    }
  }
  ফেরত 0;
}

//========================================
// Write implementation
//========================================

UnicodeString&
UnicodeString::caseMap(
    int32_t caseLocale,
    uint32_t options,
    UCASEMAP_BREAK_ITERATOR_PARAM UStringCaseMapper* stringCaseMapper) U_LIFETIME_BOUND {
  যদি(isEmpty() || !isWritable()) {
    // nothing to do
    ফেরত *এই;
  }

  char16_t old_বাফার[2 * US_STACKBUF_আকার];
  char16_t *oldArray;
  int32_t old_দৈর্ঘ্য = length();
  int32_t new_দৈর্ঘ্য;
  UBool writable = is_বাফার_Writable();
  UErrorCode errorCode = U_ZERO_ERROR;

#যদি !UCONFIG_NO_BREAK_ITERATION
  // Read-only alias to the original string contents for the titlecasing BreakIterator.
  // We cannot set the iterator simply to *this because *this is being modified.
  UnicodeString oldString;
#endif

  // Try to avoid heap-allocating a new character array for this string.
  যদি (writable ? old_দৈর্ঘ্য <= UPRV_LENGTHOF(old_বাফার) : old_দৈর্ঘ্য < US_STACKBUF_আকার) {
    // Short string: Copy the contents into a temporary buffer and
    // case-map back into the current array, or into the stack buffer.
    char16_t *বাফার = getArrayStart();
    int32_t capacity;
    oldArray = old_বাফার;
    u_memcpy(old_বাফার, বাফার, old_দৈর্ঘ্য);
    যদি (writable) {
      capacity = getCapacity();
    } অন্যথা {
      // Switch from the read-only alias or shared heap buffer to the stack buffer.
      যদি (!cloneArrayIfNeeded(US_STACKBUF_আকার, US_STACKBUF_আকার, /* doCopyArray= */ মিথ্যা)) {
        ফেরত *এই;
      }
      U_ASSERT(fUnion.fFields.f_দৈর্ঘ্য_And_Flags & k_Using_স্তূপ_বাফার);
      বাফার = fUnion.f_স্তূপ_Fields.f_বাফার;
      capacity = US_STACKBUF_আকার;
    }
#যদি !UCONFIG_NO_BREAK_ITERATION
    যদি (iter != nullptr) {
      oldString.setTo(মিথ্যা, oldArray, old_দৈর্ঘ্য);
      iter->setText(oldString);
    }
#endif
    new_দৈর্ঘ্য = stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                 বাফার, capacity,
                                 oldArray, old_দৈর্ঘ্য, nullptr, errorCode);
    যদি (U_SUCCESS(errorCode)) {
      set_দৈর্ঘ্য(new_দৈর্ঘ্য);
      ফেরত *এই;
    } অন্যথা যদি (errorCode == U_বাফার_OVERFLOW_ERROR) {
      // common overflow handling below
    } অন্যথা {
      setToBogus();
      ফেরত *এই;
    }
  } অন্যথা {
    // Longer string or read-only buffer:
    // Collect only changes and then apply them to this string.
    // Case mapping often changes only small parts of a string,
    // and often does not change its length.
    oldArray = getArrayStart();
    Edits edits;
    char16_t replacementChars[200];
#যদি !UCONFIG_NO_BREAK_ITERATION
    যদি (iter != nullptr) {
      oldString.setTo(মিথ্যা, oldArray, old_দৈর্ঘ্য);
      iter->setText(oldString);
    }
#endif
    stringCaseMapper(caseLocale, options | U_OMIT_UNCHANGED_TEXT, UCASEMAP_BREAK_ITERATOR
                     replacementChars, UPRV_LENGTHOF(replacementChars),
                     oldArray, old_দৈর্ঘ্য, &edits, errorCode);
    যদি (U_SUCCESS(errorCode)) {
      // Grow the buffer at most once, not for multiple doReplace() calls.
      new_দৈর্ঘ্য = old_দৈর্ঘ্য + edits.দৈর্ঘ্য_Delta();
      যদি (new_দৈর্ঘ্য > old_দৈর্ঘ্য && !cloneArrayIfNeeded(new_দৈর্ঘ্য, new_দৈর্ঘ্য)) {
        ফেরত *এই;
      }
      জন্য (Edits::Iterator ei = edits.getCoarseChangesIterator(); ei.next(errorCode);) {
        doReplace(ei.destination_সূচক(), ei.old_দৈর্ঘ্য(),
                  replacementChars, ei.replacement_সূচক(), ei.new_দৈর্ঘ্য());
      }
      যদি (U_FAILURE(errorCode)) {
        setToBogus();
      }
      ফেরত *এই;
    } অন্যথা যদি (errorCode == U_বাফার_OVERFLOW_ERROR) {
      // common overflow handling below
      new_দৈর্ঘ্য = old_দৈর্ঘ্য + edits.দৈর্ঘ্য_Delta();
    } অন্যথা {
      setToBogus();
      ফেরত *এই;
    }
  }

  // Handle buffer overflow, newLength is known.
  // We need to allocate a new buffer for the internal string case mapping function.
  // This is very similar to how doReplace() keeps the old array pointer
  // and deletes the old array itself after it is done.
  // In addition, we are forcing cloneArrayIfNeeded() to always allocate a new array.
  int32_t *বাফার_To_Delete = nullptr;
  যদি (!cloneArrayIfNeeded(new_দৈর্ঘ্য, new_দৈর্ঘ্য, মিথ্যা, &বাফার_To_Delete, সত্য)) {
    ফেরত *এই;
  }
  errorCode = U_ZERO_ERROR;
  // No need to iter->setText() again: The case mapper restarts via iter->first().
  new_দৈর্ঘ্য = stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                               getArrayStart(), getCapacity(),
                               oldArray, old_দৈর্ঘ্য, nullptr, errorCode);
  যদি (বাফার_To_Delete) {
    uprv_free(বাফার_To_Delete);
  }
  যদি (U_SUCCESS(errorCode)) {
    set_দৈর্ঘ্য(new_দৈর্ঘ্য);
  } অন্যথা {
    setToBogus();
  }
  ফেরত *এই;
}

UnicodeString&
UnicodeString::foldCase(uint32_t options) U_LIFETIME_BOUND {
  ফেরত caseMap(UCASE_LOC_মূল, options, UCASEMAP_BREAK_ITERATOR_NULL ustrcase_internalFold);
}

U_NAMESPACE_END

// Defined here to reduce dependencies on break iterator
U_CAPI int32_t U_EXPORT2
uhash_hashCaselessUnicodeString(ধ্রুবক UElement চাবি) {
    U_NAMESPACE_USE
    ধ্রুবক UnicodeString *str = (ধ্রুবক UnicodeString*) চাবি.pointer;
    যদি (str == nullptr) {
        ফেরত 0;
    }
    // Inefficient; a better way would be to have a hash function in
    // UnicodeString that does case folding on the fly.
    UnicodeString copy(*str);
    ফেরত copy.foldCase().hashCode();
}

// Defined here to reduce dependencies on break iterator
U_CAPI UBool U_EXPORT2
uhash_compareCaselessUnicodeString(ধ্রুবক UElement key1, ধ্রুবক UElement key2) {
    U_NAMESPACE_USE
    ধ্রুবক UnicodeString *str1 = (ধ্রুবক UnicodeString*) key1.pointer;
    ধ্রুবক UnicodeString *str2 = (ধ্রুবক UnicodeString*) key2.pointer;
    যদি (str1 == str2) {
        ফেরত সত্য;
    }
    যদি (str1 == nullptr || str2 == nullptr) {
        ফেরত মিথ্যা;
    }
    ফেরত str1->caseCompare(*str2, U_FOLD_CASE_DEFAULT) == 0;
}
