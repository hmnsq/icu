// © 2020 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// uniquecharstr.h
// created: 2020sep01 Frank Yung-Fong Tang

#ifndef __UNIQUECHARSTR_H__
#define __UNIQUECHARSTR_H__

#অন্তর্ভাব "charstr.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "uhash.h"
#অন্তর্ভাব "cmemory.h"

U_NAMESPACE_BEGIN

/**
 * Stores NUL-terminated strings with duplicate elimination.
 * Checks for unique UTF-16 string pointers and converts to invariant characters.
 *
 * Intended to be stack-allocated. Add strings, get a unique number for each,
 * freeze the object, get a char * pointer for each string,
 * call orphanCharStrings() to capture the string storage, and let this object go out of scope.
 */
class UniqueCharStrings {
সর্বজনীন:
    UniqueCharStrings(UErrorCode &errorCode) : strings(nullptr) {
        // Note: We hash on string contents but store stable char16_t * pointers.
        // If the strings are stored in resource bundles which should be built with
        // duplicate elimination, then we should be able to hash on just the pointer values.
        uhash_init(&map, uhash_hashUChars, uhash_compareUChars, uhash_compareLong, &errorCode);
        যদি (U_FAILURE(errorCode)) { ফেরত; }
        strings = new CharString();
        যদি (strings == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
        }
    }
    ~UniqueCharStrings() {
        uhash_close(&map);
        delete strings;
    }

    /** Returns/orphans the CharString that contains all strings. */
    CharString *orphanCharStrings() {
        CharString *ফলাফল = strings;
        strings = nullptr;
        ফেরত ফলাফল;
    }

    /**
     * Adds a NUL-terminated string and returns a unique number for it.
     * The string must not change, nor move around in memory,
     * while this UniqueCharStrings is in use.
     *
     * Best used with string data in a stable storage, such as strings returned
     * by resource bundle functions.
     */
    int32_t add(ধ্রুবক char16_t*p, UErrorCode &errorCode) {
        যদি (U_FAILURE(errorCode)) { ফেরত -1; }
        যদি (isFrozen) {
            errorCode = U_NO_WRITE_PERMISSION;
            ফেরত -1;
        }
        // The string points into the resource bundle.
        int32_t old_সূচক = uhash_geti(&map, p);
        যদি (old_সূচক != 0) {  // found duplicate
            ফেরত old_সূচক;
        }
        // Explicit NUL terminator for the previous string.
        // The strings object is also terminated with one implicit NUL.
        strings->append(0, errorCode);
        int32_t new_সূচক = strings->দৈর্ঘ্য();
        strings->appendInvariantChars(p, u_strlen(p), errorCode);
        uhash_puti(&map, const_cast<char16_t *>(p), new_সূচক, &errorCode);
        ফেরত new_সূচক;
    }

    /**
     * Adds a unicode string by value and returns a unique number for it.
     */
    int32_t add_By_মান(UnicodeString s, UErrorCode &errorCode) {
        যদি (U_FAILURE(errorCode)) { ফেরত -1; }
        যদি (isFrozen) {
            errorCode = U_NO_WRITE_PERMISSION;
            ফেরত -1;
        }
        int32_t old_সূচক = uhash_geti(&map, s.get_Terminated_বাফার());
        যদি (old_সূচক != 0) {  // found duplicate
            ফেরত old_সূচক;
        }
        // We need to store the string content of the UnicodeString.
        UnicodeString *চাবি = চাবি_Store.create(s);
        যদি (চাবি == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ফেরত -1;
        }
        ফেরত add(চাবি->get_Terminated_বাফার(), errorCode);
    }

    শূন্য freeze() { isFrozen = true; }

    /**
     * Returns a string pointer for its unique number, if this object is frozen.
     * Otherwise nullptr.
     */
    ধ্রুবক অক্ষর *get(int32_t i) ধ্রুবক {
        U_ASSERT(isFrozen);
        ফেরত isFrozen && i > 0 ? strings->তথ্য() + i : nullptr;
    }

ব্যক্তিগত:
    UHashtable map;
    CharString *strings;
    MemoryPool<UnicodeString> চাবি_Store;
    bool isFrozen = false;
};

U_NAMESPACE_END

#endif  // __UNIQUECHARSTR_H__
