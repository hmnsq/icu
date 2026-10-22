// © 2020 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// uniquecharstr.h
// created: 2020sep01 Frank Yung-Fong Tang

#ifndef __UNIQUECHARSTR_H__
#define __UNIQUECHARSTR_H__

#अन्तर्भाव "charstr.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "uhash.h"
#अन्तर्भाव "cmemory.h"

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
सार्वजनिक:
    UniqueCharStrings(UErrorCode &errorCode) : strings(nullptr) {
        // Note: We hash on string contents but store stable char16_t * pointers.
        // If the strings are stored in resource bundles which should be built with
        // duplicate elimination, then we should be able to hash on just the pointer values.
        uhash_init(&map, uhash_hashUChars, uhash_compareUChars, uhash_compareLong, &errorCode);
        जर (U_FAILURE(errorCode)) { परत; }
        strings = new CharString();
        जर (strings == nullptr) {
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
        परत result;
    }

    /**
     * Adds a NUL-terminated string and returns a unique number for it.
     * The string must not change, nor move around in memory,
     * while this UniqueCharStrings is in use.
     *
     * Best used with string data in a stable storage, such as strings returned
     * by resource bundle functions.
     */
    int32_t add(स्थिरांक char16_t*p, UErrorCode &errorCode) {
        जर (U_FAILURE(errorCode)) { परत -1; }
        जर (isFrozen) {
            errorCode = U_NO_WRITE_PERMISSION;
            परत -1;
        }
        // The string points into the resource bundle.
        int32_t oldIndex = uhash_geti(&map, p);
        जर (oldIndex != 0) {  // found duplicate
            परत oldIndex;
        }
        // Explicit NUL terminator for the previous string.
        // The strings object is also terminated with one implicit NUL.
        strings->append(0, errorCode);
        int32_t newIndex = strings->length();
        strings->appendInvariantChars(p, u_strlen(p), errorCode);
        uhash_puti(&map, const_cast<char16_t *>(p), newIndex, &errorCode);
        परत newIndex;
    }

    /**
     * Adds a unicode string by value and returns a unique number for it.
     */
    int32_t add_By_मूल्य(UnicodeString s, UErrorCode &errorCode) {
        जर (U_FAILURE(errorCode)) { परत -1; }
        जर (isFrozen) {
            errorCode = U_NO_WRITE_PERMISSION;
            परत -1;
        }
        int32_t oldIndex = uhash_geti(&map, s.getTerminatedBuffer());
        जर (oldIndex != 0) {  // found duplicate
            परत oldIndex;
        }
        // We need to store the string content of the UnicodeString.
        UnicodeString *किल्ली = किल्ली_Store.create(s);
        जर (किल्ली == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            परत -1;
        }
        परत add(किल्ली->getTerminatedBuffer(), errorCode);
    }

    शून्य freeze() { isFrozen = true; }

    /**
     * Returns a string pointer for its unique number, if this object is frozen.
     * Otherwise nullptr.
     */
    स्थिरांक अक्षर *get(int32_t i) स्थिरांक {
        U_ASSERT(isFrozen);
        परत isFrozen && i > 0 ? strings->data() + i : nullptr;
    }

खाजगी:
    UHashtable map;
    CharString *strings;
    MemoryPool<UnicodeString> किल्ली_Store;
    bool isFrozen = false;
};

U_NAMESPACE_END

#endif  // __UNIQUECHARSTR_H__
