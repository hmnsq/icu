// © 2020 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// uniquecharstr.h
// created: 2020sep01 Frank Yung-Fong Tang

#جيڪڏهن_وصف_نه_ٿيل __UNIQUECHARSTR_H__
#وصف_ڪريو __UNIQUECHARSTR_H__

#ضمن "charstr.h"
#ضمن "uassert.h"
#ضمن "uhash.h"
#ضمن "cmemory.h"

U_NAMESPACE_BEGIN

/**
 * Stores NUL-terminated strings with duplicate elimination.
 * Checks for unique UTF-16 string pointers and converts to invariant characters.
 *
 * Intended to be stack-allocated. Add strings, get a unique number for each,
 * freeze the object, get a char * pointer for each string,
 * call orphanCharStrings() to capture the string storage, and let this object go out of scope.
 */
class Unique_Char_لڙيون {
عام:
    Unique_Char_لڙيون(UError_ڪوڊ &error_ڪوڊ) : لڙيون(nullptr) {
        // Note: We hash on string contents but store stable char16_t * pointers.
        // If the strings are stored in resource bundles which should be built with
        // duplicate elimination, then we should be able to hash on just the pointer values.
        uhash_init(&map, uhash_hashUChars, uhash_compareUChars, uhash_compareLong, &error_ڪوڊ);
        إذا (U_FAILURE(error_ڪوڊ)) { أرجع; }
        لڙيون = new CharString();
        إذا (لڙيون == nullptr) {
            error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
        }
    }
    ~Unique_Char_لڙيون() {
        uhash_close(&map);
        delete لڙيون;
    }

    /** Returns/orphans the CharString that contains all strings. */
    CharString *orphan_Char_لڙيون() {
        CharString *نتيجو = لڙيون;
        لڙيون = nullptr;
        أرجع نتيجو;
    }

    /**
     * Adds a NUL-terminated string and returns a unique number for it.
     * The string must not change, nor move around in memory,
     * while this UniqueCharStrings is in use.
     *
     * Best used with string data in a stable storage, such as strings returned
     * by resource bundle functions.
     */
    int32_t جوڙيو(ثابت char16_t*p, UError_ڪوڊ &error_ڪوڊ) {
        إذا (U_FAILURE(error_ڪوڊ)) { أرجع -1; }
        إذا (isFrozen) {
            error_ڪوڊ = U_NO_WRITE_PERMISSION;
            أرجع -1;
        }
        // The string points into the resource bundle.
        int32_t old_اشاريو = uhash_geti(&map, p);
        إذا (old_اشاريو != 0) {  // found duplicate
            أرجع old_اشاريو;
        }
        // Explicit NUL terminator for the previous string.
        // The strings object is also terminated with one implicit NUL.
        لڙيون->append(0, error_ڪوڊ);
        int32_t new_اشاريو = لڙيون->ڊيگهائي();
        لڙيون->appendInvariantChars(p, u_strlen(p), error_ڪوڊ);
        uhash_puti(&map, const_cast<char16_t *>(p), new_اشاريو, &error_ڪوڊ);
        أرجع new_اشاريو;
    }

    /**
     * Adds a unicode string by value and returns a unique number for it.
     */
    int32_t جوڙيو_By_قدر(UnicodeString s, UError_ڪوڊ &error_ڪوڊ) {
        إذا (U_FAILURE(error_ڪوڊ)) { أرجع -1; }
        إذا (isFrozen) {
            error_ڪوڊ = U_NO_WRITE_PERMISSION;
            أرجع -1;
        }
        int32_t old_اشاريو = uhash_geti(&map, s.get_Terminated_بفر());
        إذا (old_اشاريو != 0) {  // found duplicate
            أرجع old_اشاريو;
        }
        // We need to store the string content of the UnicodeString.
        UnicodeString *ڪنجي = ڪنجي_Store.create(s);
        إذا (ڪنجي == nullptr) {
            error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
            أرجع -1;
        }
        أرجع جوڙيو(ڪنجي->get_Terminated_بفر(), error_ڪوڊ);
    }

    فراغ freeze() { isFrozen = true; }

    /**
     * Returns a string pointer for its unique number, if this object is frozen.
     * Otherwise nullptr.
     */
    ثابت حرف *get(int32_t i) ثابت {
        U_ASSERT(isFrozen);
        أرجع isFrozen && i > 0 ? لڙيون->معلومات() + i : nullptr;
    }

خاص:
    UHashtable map;
    CharString *لڙيون;
    يادگيري_Pool<UnicodeString> ڪنجي_Store;
    bool isFrozen = false;
};

U_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو  // __UNIQUECHARSTR_H__
