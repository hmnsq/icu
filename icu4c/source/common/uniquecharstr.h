// © 2020 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// uniquecharstr.h
// created: 2020sep01 Frank Yung-Fong Tang

#جے_بنا_تعریف __UNIQUECHARSTR_H__
#تعریف_کرو __UNIQUECHARSTR_H__

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
class Unique_Char_لڑیاں {
عام:
    Unique_Char_لڑیاں(UError_کوڈ &error_کوڈ) : لڑیاں(nullptr) {
        // Note: We hash on string contents but store stable char16_t * pointers.
        // If the strings are stored in resource bundles which should be built with
        // duplicate elimination, then we should be able to hash on just the pointer values.
        uhash_init(&map, uhash_hashUChars, uhash_compareUChars, uhash_compareLong, &error_کوڈ);
        إذا (U_FAILURE(error_کوڈ)) { أرجع; }
        لڑیاں = new CharString();
        إذا (لڑیاں == nullptr) {
            error_کوڈ = U_حافظہ_ونڈ_ERROR;
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
        أرجع نتیجہ;
    }

    /**
     * Adds a NUL-terminated string and returns a unique number for it.
     * The string must not change, nor move around in memory,
     * while this UniqueCharStrings is in use.
     *
     * Best used with string data in a stable storage, such as strings returned
     * by resource bundle functions.
     */
    int32_t جوڑو(ثابت char16_t*p, UError_کوڈ &error_کوڈ) {
        إذا (U_FAILURE(error_کوڈ)) { أرجع -1; }
        إذا (isFrozen) {
            error_کوڈ = U_NO_WRITE_PERMISSION;
            أرجع -1;
        }
        // The string points into the resource bundle.
        int32_t old_اشاریہ = uhash_geti(&map, p);
        إذا (old_اشاریہ != 0) {  // found duplicate
            أرجع old_اشاریہ;
        }
        // Explicit NUL terminator for the previous string.
        // The strings object is also terminated with one implicit NUL.
        لڑیاں->append(0, error_کوڈ);
        int32_t new_اشاریہ = لڑیاں->لمائی();
        لڑیاں->appendInvariantChars(p, u_strlen(p), error_کوڈ);
        uhash_puti(&map, const_cast<char16_t *>(p), new_اشاریہ, &error_کوڈ);
        أرجع new_اشاریہ;
    }

    /**
     * Adds a unicode string by value and returns a unique number for it.
     */
    int32_t جوڑو_By_قدر(UnicodeString s, UError_کوڈ &error_کوڈ) {
        إذا (U_FAILURE(error_کوڈ)) { أرجع -1; }
        إذا (isFrozen) {
            error_کوڈ = U_NO_WRITE_PERMISSION;
            أرجع -1;
        }
        int32_t old_اشاریہ = uhash_geti(&map, s.get_Terminated_بفر());
        إذا (old_اشاریہ != 0) {  // found duplicate
            أرجع old_اشاریہ;
        }
        // We need to store the string content of the UnicodeString.
        UnicodeString *کلید = کلید_Store.create(s);
        إذا (کلید == nullptr) {
            error_کوڈ = U_حافظہ_ونڈ_ERROR;
            أرجع -1;
        }
        أرجع جوڑو(کلید->get_Terminated_بفر(), error_کوڈ);
    }

    فراغ freeze() { isFrozen = true; }

    /**
     * Returns a string pointer for its unique number, if this object is frozen.
     * Otherwise nullptr.
     */
    ثابت حرف *get(int32_t i) ثابت {
        U_ASSERT(isFrozen);
        أرجع isFrozen && i > 0 ? لڑیاں->انکڑے() + i : nullptr;
    }

خاص:
    UHashtable map;
    CharString *لڑیاں;
    حافظہ_Pool<UnicodeString> کلید_Store;
    bool isFrozen = false;
};

U_NAMESPACE_سرا

#جے_انت  // __UNIQUECHARSTR_H__
