// © 2020 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// uniquecharstr.h
// created: 2020sep01 Frank Yung-Fong Tang

#ifndef __UNIQUECHARSTR_H__
#define __UNIQUECHARSTR_H__

#ଅନ୍ତର୍ଭାଵ "charstr.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"
#ଅନ୍ତର୍ଭାଵ "uhash.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"

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
ସର୍ବସାଧାରଣ:
    UniqueCharStrings(UErrorCode &errorCode) : strings(nullptr) {
        // Note: We hash on string contents but store stable char16_t * pointers.
        // If the strings are stored in resource bundles which should be built with
        // duplicate elimination, then we should be able to hash on just the pointer values.
        uhash_init(&map, uhash_hashUChars, uhash_compareUChars, uhash_compareLong, &errorCode);
        ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ; }
        strings = new CharString();
        ଯଦି (strings == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
        }
    }
    ~UniqueCharStrings() {
        uhash_close(&map);
        delete strings;
    }

    /** Returns/orphans the CharString that contains all strings. */
    CharString *orphanCharStrings() {
        CharString *result = strings;
        strings = nullptr;
        ଫେରନ୍ତୁ result;
    }

    /**
     * Adds a NUL-terminated string and returns a unique number for it.
     * The string must not change, nor move around in memory,
     * while this UniqueCharStrings is in use.
     *
     * Best used with string data in a stable storage, such as strings returned
     * by resource bundle functions.
     */
    int32_t add(ଅଚଳ char16_t*p, UErrorCode &errorCode) {
        ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ -1; }
        ଯଦି (isFrozen) {
            errorCode = U_NO_WRITE_PERMISSION;
            ଫେରନ୍ତୁ -1;
        }
        // The string points into the resource bundle.
        int32_t oldIndex = uhash_geti(&map, p);
        ଯଦି (oldIndex != 0) {  // found duplicate
            ଫେରନ୍ତୁ oldIndex;
        }
        // Explicit NUL terminator for the previous string.
        // The strings object is also terminated with one implicit NUL.
        strings->append(0, errorCode);
        int32_t newIndex = strings->length();
        strings->appendInvariantChars(p, u_strlen(p), errorCode);
        uhash_puti(&map, const_cast<char16_t *>(p), newIndex, &errorCode);
        ଫେରନ୍ତୁ newIndex;
    }

    /**
     * Adds a unicode string by value and returns a unique number for it.
     */
    int32_t add_By_ମୂଲ୍ୟ(UnicodeString s, UErrorCode &errorCode) {
        ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ -1; }
        ଯଦି (isFrozen) {
            errorCode = U_NO_WRITE_PERMISSION;
            ଫେରନ୍ତୁ -1;
        }
        int32_t oldIndex = uhash_geti(&map, s.getTerminatedBuffer());
        ଯଦି (oldIndex != 0) {  // found duplicate
            ଫେରନ୍ତୁ oldIndex;
        }
        // We need to store the string content of the UnicodeString.
        UnicodeString *ଚାବି = ଚାବି_Store.create(s);
        ଯଦି (ଚାବି == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ଫେରନ୍ତୁ -1;
        }
        ଫେରନ୍ତୁ add(ଚାବି->getTerminatedBuffer(), errorCode);
    }

    ଶୂନ୍ୟ freeze() { isFrozen = true; }

    /**
     * Returns a string pointer for its unique number, if this object is frozen.
     * Otherwise nullptr.
     */
    ଅଚଳ ଅକ୍ଷର *get(int32_t i) ଅଚଳ {
        U_ASSERT(isFrozen);
        ଫେରନ୍ତୁ isFrozen && i > 0 ? strings->data() + i : nullptr;
    }

ବ୍ୟକ୍ତିଗତ:
    UHashtable map;
    CharString *strings;
    MemoryPool<UnicodeString> ଚାବି_Store;
    bool isFrozen = false;
};

U_NAMESPACE_END

#endif  // __UNIQUECHARSTR_H__
