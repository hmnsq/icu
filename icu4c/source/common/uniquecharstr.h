// © 2020 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// uniquecharstr.h
// created: 2020sep01 Frank Yung-Fong Tang

#ifndef __UNIQUECHARSTR_H__
#define __UNIQUECHARSTR_H__

#شامل "charstr.h"
#شامل "uassert.h"
#شامل "uhash.h"
#شامل "cmemory.h"

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
عمومی:
    UniqueCharStrings(UErrorCode &errorCode) : strings(nullptr) {
        // Note: We hash on string contents but store stable char16_t * pointers.
        // If the strings are stored in resource bundles which should be built with
        // duplicate elimination, then we should be able to hash on just the pointer values.
        uhash_init(&map, uhash_hashUChars, uhash_compareUChars, uhash_compareLong, &errorCode);
        اگر (U_FAILURE(errorCode)) { بازگشت; }
        strings = new CharString();
        اگر (strings == nullptr) {
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
        بازگشت result;
    }

    /**
     * Adds a NUL-terminated string and returns a unique number for it.
     * The string must not change, nor move around in memory,
     * while this UniqueCharStrings is in use.
     *
     * Best used with string data in a stable storage, such as strings returned
     * by resource bundle functions.
     */
    int32_t add(ثابت char16_t*p, UErrorCode &errorCode) {
        اگر (U_FAILURE(errorCode)) { بازگشت -1; }
        اگر (isFrozen) {
            errorCode = U_NO_WRITE_PERMISSION;
            بازگشت -1;
        }
        // The string points into the resource bundle.
        int32_t oldIndex = uhash_geti(&map, p);
        اگر (oldIndex != 0) {  // found duplicate
            بازگشت oldIndex;
        }
        // Explicit NUL terminator for the previous string.
        // The strings object is also terminated with one implicit NUL.
        strings->append(0, errorCode);
        int32_t newIndex = strings->length();
        strings->appendInvariantChars(p, u_strlen(p), errorCode);
        uhash_puti(&map, const_cast<char16_t *>(p), newIndex, &errorCode);
        بازگشت newIndex;
    }

    /**
     * Adds a unicode string by value and returns a unique number for it.
     */
    int32_t add_By_مقدار(UnicodeString s, UErrorCode &errorCode) {
        اگر (U_FAILURE(errorCode)) { بازگشت -1; }
        اگر (isFrozen) {
            errorCode = U_NO_WRITE_PERMISSION;
            بازگشت -1;
        }
        int32_t oldIndex = uhash_geti(&map, s.getTerminatedBuffer());
        اگر (oldIndex != 0) {  // found duplicate
            بازگشت oldIndex;
        }
        // We need to store the string content of the UnicodeString.
        UnicodeString *کلید = کلید_Store.create(s);
        اگر (کلید == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            بازگشت -1;
        }
        بازگشت add(کلید->getTerminatedBuffer(), errorCode);
    }

    تهی freeze() { isFrozen = true; }

    /**
     * Returns a string pointer for its unique number, if this object is frozen.
     * Otherwise nullptr.
     */
    ثابت نویسه *get(int32_t i) ثابت {
        U_ASSERT(isFrozen);
        بازگشت isFrozen && i > 0 ? strings->data() + i : nullptr;
    }

خصوصی:
    UHashtable map;
    CharString *strings;
    MemoryPool<UnicodeString> کلید_Store;
    bool isFrozen = false;
};

U_NAMESPACE_END

#endif  // __UNIQUECHARSTR_H__
