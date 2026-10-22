// © 2020 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// uniquecharstr.h
// created: 2020sep01 Frank Yung-Fong Tang

#ifndef __UNIQUECHARSTR_H__
#define __UNIQUECHARSTR_H__

#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "uhash.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"

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
ಸಾರ್ವಜನಿಕ:
    UniqueCharStrings(UErrorCode &errorCode) : strings(nullptr) {
        // Note: We hash on string contents but store stable char16_t * pointers.
        // If the strings are stored in resource bundles which should be built with
        // duplicate elimination, then we should be able to hash on just the pointer values.
        uhash_init(&map, uhash_hashUChars, uhash_compareUChars, uhash_compareLong, &errorCode);
        ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ; }
        strings = new CharString();
        ಯದಿ (strings == nullptr) {
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
        ಪ್ರತ್ಯಾಗಮ result;
    }

    /**
     * Adds a NUL-terminated string and returns a unique number for it.
     * The string must not change, nor move around in memory,
     * while this UniqueCharStrings is in use.
     *
     * Best used with string data in a stable storage, such as strings returned
     * by resource bundle functions.
     */
    int32_t add(ಧ್ರುವ char16_t*p, UErrorCode &errorCode) {
        ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ -1; }
        ಯದಿ (isFrozen) {
            errorCode = U_NO_WRITE_PERMISSION;
            ಪ್ರತ್ಯಾಗಮ -1;
        }
        // The string points into the resource bundle.
        int32_t oldIndex = uhash_geti(&map, p);
        ಯದಿ (oldIndex != 0) {  // found duplicate
            ಪ್ರತ್ಯಾಗಮ oldIndex;
        }
        // Explicit NUL terminator for the previous string.
        // The strings object is also terminated with one implicit NUL.
        strings->append(0, errorCode);
        int32_t newIndex = strings->length();
        strings->appendInvariantChars(p, u_strlen(p), errorCode);
        uhash_puti(&map, const_cast<char16_t *>(p), newIndex, &errorCode);
        ಪ್ರತ್ಯಾಗಮ newIndex;
    }

    /**
     * Adds a unicode string by value and returns a unique number for it.
     */
    int32_t add_By_ಮೌಲ್ಯ(UnicodeString s, UErrorCode &errorCode) {
        ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ -1; }
        ಯದಿ (isFrozen) {
            errorCode = U_NO_WRITE_PERMISSION;
            ಪ್ರತ್ಯಾಗಮ -1;
        }
        int32_t oldIndex = uhash_geti(&map, s.getTerminatedBuffer());
        ಯದಿ (oldIndex != 0) {  // found duplicate
            ಪ್ರತ್ಯಾಗಮ oldIndex;
        }
        // We need to store the string content of the UnicodeString.
        UnicodeString *ಕೀಲಿ = ಕೀಲಿ_Store.create(s);
        ಯದಿ (ಕೀಲಿ == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ಪ್ರತ್ಯಾಗಮ -1;
        }
        ಪ್ರತ್ಯಾಗಮ add(ಕೀಲಿ->getTerminatedBuffer(), errorCode);
    }

    ಶೂನ್ಯ freeze() { isFrozen = true; }

    /**
     * Returns a string pointer for its unique number, if this object is frozen.
     * Otherwise nullptr.
     */
    ಧ್ರುವ ವರ್ಣ *get(int32_t i) ಧ್ರುವ {
        U_ASSERT(isFrozen);
        ಪ್ರತ್ಯಾಗಮ isFrozen && i > 0 ? strings->data() + i : nullptr;
    }

ಗುಪ್ತ:
    UHashtable map;
    CharString *strings;
    MemoryPool<UnicodeString> ಕೀಲಿ_Store;
    bool isFrozen = false;
};

U_NAMESPACE_END

#endif  // __UNIQUECHARSTR_H__
