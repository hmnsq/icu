// © 2020 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// uniquecharstr.h
// created: 2020sep01 Frank Yung-Fong Tang

#अगर_अघोषित __UNIQUECHARSTR_H__
#घोषणा __UNIQUECHARSTR_H__

#समावेश "charstr.h"
#समावेश "uassert.h"
#समावेश "uhash.h"
#समावेश "cmemory.h"

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
खुला:
    UniqueCharStrings(UErrorCode &errorCode) : strings(nullptr) {
        // Note: We hash on string contents but store stable char16_t * pointers.
        // If the strings are stored in resource bundles which should be built with
        // duplicate elimination, then we should be able to hash on just the pointer values.
        uhash_init(&map, uhash_hashUChars, uhash_compareUChars, uhash_compareLong, &errorCode);
        अगर (U_FAILURE(errorCode)) { वापस; }
        strings = new CharString();
        अगर (strings == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
        }
    }
    ~UniqueCharStrings() {
        uhash_close(&map);
        delete strings;
    }

    /** Returns/orphans the CharString that contains all strings. */
    CharString *orphanCharStrings() {
        CharString *परिणाम = strings;
        strings = nullptr;
        वापस परिणाम;
    }

    /**
     * Adds a NUL-terminated string and returns a unique number for it.
     * The string must not change, nor move around in memory,
     * while this UniqueCharStrings is in use.
     *
     * Best used with string data in a stable storage, such as strings returned
     * by resource bundle functions.
     */
    int32_t add(स्थिर char16_t*p, UErrorCode &errorCode) {
        अगर (U_FAILURE(errorCode)) { वापस -1; }
        अगर (isFrozen) {
            errorCode = U_NO_WRITE_PERMISSION;
            वापस -1;
        }
        // The string points into the resource bundle.
        int32_t old_सूचकांक = uhash_geti(&map, p);
        अगर (old_सूचकांक != 0) {  // found duplicate
            वापस old_सूचकांक;
        }
        // Explicit NUL terminator for the previous string.
        // The strings object is also terminated with one implicit NUL.
        strings->append(0, errorCode);
        int32_t new_सूचकांक = strings->लंबाई();
        strings->appendInvariantChars(p, u_strlen(p), errorCode);
        uhash_puti(&map, const_cast<char16_t *>(p), new_सूचकांक, &errorCode);
        वापस new_सूचकांक;
    }

    /**
     * Adds a unicode string by value and returns a unique number for it.
     */
    int32_t add_By_मूल्य(UnicodeString s, UErrorCode &errorCode) {
        अगर (U_FAILURE(errorCode)) { वापस -1; }
        अगर (isFrozen) {
            errorCode = U_NO_WRITE_PERMISSION;
            वापस -1;
        }
        int32_t old_सूचकांक = uhash_geti(&map, s.get_Terminated_बफर());
        अगर (old_सूचकांक != 0) {  // found duplicate
            वापस old_सूचकांक;
        }
        // We need to store the string content of the UnicodeString.
        UnicodeString *कुंजी = कुंजी_Store.create(s);
        अगर (कुंजी == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            वापस -1;
        }
        वापस add(कुंजी->get_Terminated_बफर(), errorCode);
    }

    व्योम freeze() { isFrozen = true; }

    /**
     * Returns a string pointer for its unique number, if this object is frozen.
     * Otherwise nullptr.
     */
    स्थिर अक्षर *get(int32_t i) स्थिर {
        U_ASSERT(isFrozen);
        वापस isFrozen && i > 0 ? strings->तथ्य() + i : nullptr;
    }

निजी:
    UHashtable map;
    CharString *strings;
    MemoryPool<UnicodeString> कुंजी_Store;
    bool isFrozen = false;
};

U_NAMESPACE_END

#पूर्ण_अगर  // __UNIQUECHARSTR_H__
