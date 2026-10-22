// © 2020 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// uniquecharstr.h
// created: 2020sep01 Frank Yung-Fong Tang

#اگر_غیر_تعریف_شدہ __UNIQUECHARSTR_H__
#تعریف_کرو __UNIQUECHARSTR_H__

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
class Unique_Char_لڑیاں {
عوامی:
    Unique_Char_لڑیاں(UError_رمز &error_رمز) : لڑیاں(nullptr) {
        // Note: We hash on string contents but store stable char16_t * pointers.
        // If the strings are stored in resource bundles which should be built with
        // duplicate elimination, then we should be able to hash on just the pointer values.
        uhash_init(&map, uhash_hashUChars, uhash_compareUChars, uhash_compareLong, &error_رمز);
        اگر (U_FAILURE(error_رمز)) { واپس; }
        لڑیاں = new CharString();
        اگر (لڑیاں == nullptr) {
            error_رمز = U_حافظہ_تخصیص_ERROR;
        }
    }
    ~Unique_Char_لڑیاں() {
        uhash_close(&map);
        delete لڑیاں;
    }

    /** Returns/orphans the CharString that contains all strings. */
    CharString *orphan_Char_لڑیاں() {
        CharString *نتیجہ = لڑیاں;
        لڑیاں = nullptr;
        واپس نتیجہ;
    }

    /**
     * Adds a NUL-terminated string and returns a unique number for it.
     * The string must not change, nor move around in memory,
     * while this UniqueCharStrings is in use.
     *
     * Best used with string data in a stable storage, such as strings returned
     * by resource bundle functions.
     */
    int32_t جوڑو(ثابت char16_t*p, UError_رمز &error_رمز) {
        اگر (U_FAILURE(error_رمز)) { واپس -1; }
        اگر (isFrozen) {
            error_رمز = U_NO_WRITE_PERMISSION;
            واپس -1;
        }
        // The string points into the resource bundle.
        int32_t old_اشاریہ = uhash_geti(&map, p);
        اگر (old_اشاریہ != 0) {  // found duplicate
            واپس old_اشاریہ;
        }
        // Explicit NUL terminator for the previous string.
        // The strings object is also terminated with one implicit NUL.
        لڑیاں->append(0, error_رمز);
        int32_t new_اشاریہ = لڑیاں->لمبائی();
        لڑیاں->appendInvariantChars(p, u_strlen(p), error_رمز);
        uhash_puti(&map, const_cast<char16_t *>(p), new_اشاریہ, &error_رمز);
        واپس new_اشاریہ;
    }

    /**
     * Adds a unicode string by value and returns a unique number for it.
     */
    int32_t جوڑو_By_قدر(UnicodeString s, UError_رمز &error_رمز) {
        اگر (U_FAILURE(error_رمز)) { واپس -1; }
        اگر (isFrozen) {
            error_رمز = U_NO_WRITE_PERMISSION;
            واپس -1;
        }
        int32_t old_اشاریہ = uhash_geti(&map, s.get_Terminated_بفر());
        اگر (old_اشاریہ != 0) {  // found duplicate
            واپس old_اشاریہ;
        }
        // We need to store the string content of the UnicodeString.
        UnicodeString *کلید = کلید_Store.create(s);
        اگر (کلید == nullptr) {
            error_رمز = U_حافظہ_تخصیص_ERROR;
            واپس -1;
        }
        واپس جوڑو(کلید->get_Terminated_بفر(), error_رمز);
    }

    خالی freeze() { isFrozen = true; }

    /**
     * Returns a string pointer for its unique number, if this object is frozen.
     * Otherwise nullptr.
     */
    ثابت حرف *get(int32_t i) ثابت {
        U_ASSERT(isFrozen);
        واپس isFrozen && i > 0 ? لڑیاں->ڈیٹا() + i : nullptr;
    }

نجی:
    UHashtable map;
    CharString *لڑیاں;
    حافظہ_Pool<UnicodeString> کلید_Store;
    bool isFrozen = false;
};

U_NAMESPACE_انتہا

#اگر_ختم  // __UNIQUECHARSTR_H__
