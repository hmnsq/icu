// © 2020 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// uniquecharstr.h
// created: 2020sep01 Frank Yung-Fong Tang

#यदि_अपरिभाषितम् __UNIQUECHARSTR_H__
#परिभाषय __UNIQUECHARSTR_H__

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
class Unique_Char_वाक्यानि {
सार्वजनिक:
    Unique_Char_वाक्यानि(UError_संहिता &error_संहिता) : वाक्यानि(nullptr) {
        // Note: We hash on string contents but store stable char16_t * pointers.
        // If the strings are stored in resource bundles which should be built with
        // duplicate elimination, then we should be able to hash on just the pointer values.
        uhash_init(&map, uhash_hashUChars, uhash_compareUChars, uhash_compareLong, &error_संहिता);
        यदि (U_FAILURE(error_संहिता)) { प्रत्यागम; }
        वाक्यानि = new CharString();
        यदि (वाक्यानि == nullptr) {
            error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
        }
    }
    ~Unique_Char_वाक्यानि() {
        uhash_close(&map);
        delete वाक्यानि;
    }

    /** Returns/orphans the CharString that contains all strings. */
    CharString *orphan_Char_वाक्यानि() {
        CharString *परिणाम = वाक्यानि;
        वाक्यानि = nullptr;
        प्रत्यागम परिणाम;
    }

    /**
     * Adds a NUL-terminated string and returns a unique number for it.
     * The string must not change, nor move around in memory,
     * while this UniqueCharStrings is in use.
     *
     * Best used with string data in a stable storage, such as strings returned
     * by resource bundle functions.
     */
    int32_t सङ्कलय(ध्रुव char16_t*p, UError_संहिता &error_संहिता) {
        यदि (U_FAILURE(error_संहिता)) { प्रत्यागम -1; }
        यदि (isFrozen) {
            error_संहिता = U_NO_WRITE_PERMISSION;
            प्रत्यागम -1;
        }
        // The string points into the resource bundle.
        int32_t old_सूचकांक = uhash_geti(&map, p);
        यदि (old_सूचकांक != 0) {  // found duplicate
            प्रत्यागम old_सूचकांक;
        }
        // Explicit NUL terminator for the previous string.
        // The strings object is also terminated with one implicit NUL.
        वाक्यानि->append(0, error_संहिता);
        int32_t new_सूचकांक = वाक्यानि->दीर्घता();
        वाक्यानि->appendInvariantChars(p, u_strlen(p), error_संहिता);
        uhash_puti(&map, const_cast<char16_t *>(p), new_सूचकांक, &error_संहिता);
        प्रत्यागम new_सूचकांक;
    }

    /**
     * Adds a unicode string by value and returns a unique number for it.
     */
    int32_t सङ्कलय_By_मानम्(UnicodeString s, UError_संहिता &error_संहिता) {
        यदि (U_FAILURE(error_संहिता)) { प्रत्यागम -1; }
        यदि (isFrozen) {
            error_संहिता = U_NO_WRITE_PERMISSION;
            प्रत्यागम -1;
        }
        int32_t old_सूचकांक = uhash_geti(&map, s.get_Terminated_बफर());
        यदि (old_सूचकांक != 0) {  // found duplicate
            प्रत्यागम old_सूचकांक;
        }
        // We need to store the string content of the UnicodeString.
        UnicodeString *कुञ्चिका = कुञ्चिका_Store.create(s);
        यदि (कुञ्चिका == nullptr) {
            error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम -1;
        }
        प्रत्यागम सङ्कलय(कुञ्चिका->get_Terminated_बफर(), error_संहिता);
    }

    शून्य freeze() { isFrozen = true; }

    /**
     * Returns a string pointer for its unique number, if this object is frozen.
     * Otherwise nullptr.
     */
    ध्रुव वर्ण *get(int32_t i) ध्रुव {
        U_ASSERT(isFrozen);
        प्रत्यागम isFrozen && i > 0 ? वाक्यानि->दत्त() + i : nullptr;
    }

गुप्त:
    UHashtable map;
    CharString *वाक्यानि;
    स्मृतिः_Pool<UnicodeString> कुञ्चिका_Store;
    bool isFrozen = false;
};

U_NAMESPACE_अन्तः

#यदि_अन्तः  // __UNIQUECHARSTR_H__
