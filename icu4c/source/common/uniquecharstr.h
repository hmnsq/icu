// © 2020 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// uniquecharstr.h
// created: 2020sep01 Frank Yung-Fong Tang

#که_نه_تعریف_شوی __UNIQUECHARSTR_H__
#تعریف_کړه __UNIQUECHARSTR_H__

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
class Unique_Char_مزي_ګانې {
عام:
    Unique_Char_مزي_ګانې(UError_کوډ &error_کوډ) : مزي_ګانې(nullptr) {
        // Note: We hash on string contents but store stable char16_t * pointers.
        // If the strings are stored in resource bundles which should be built with
        // duplicate elimination, then we should be able to hash on just the pointer values.
        uhash_init(&map, uhash_hashUChars, uhash_compareUChars, uhash_compareLong, &error_کوډ);
        إذا (U_FAILURE(error_کوډ)) { أرجع; }
        مزي_ګانې = new CharString();
        إذا (مزي_ګانې == nullptr) {
            error_کوډ = U_حافظه_ویش_ERROR;
        }
    }
    ~Unique_Char_مزي_ګانې() {
        uhash_close(&map);
        delete مزي_ګانې;
    }

    /** Returns/orphans the CharString that contains all strings. */
    CharString *orphan_Char_مزي_ګانې() {
        CharString *پایله = مزي_ګانې;
        مزي_ګانې = nullptr;
        أرجع پایله;
    }

    /**
     * Adds a NUL-terminated string and returns a unique number for it.
     * The string must not change, nor move around in memory,
     * while this UniqueCharStrings is in use.
     *
     * Best used with string data in a stable storage, such as strings returned
     * by resource bundle functions.
     */
    int32_t جمع_کړه(ثابت char16_t*p, UError_کوډ &error_کوډ) {
        إذا (U_FAILURE(error_کوډ)) { أرجع -1; }
        إذا (isFrozen) {
            error_کوډ = U_NO_WRITE_PERMISSION;
            أرجع -1;
        }
        // The string points into the resource bundle.
        int32_t old_شاخص = uhash_geti(&map, p);
        إذا (old_شاخص != 0) {  // found duplicate
            أرجع old_شاخص;
        }
        // Explicit NUL terminator for the previous string.
        // The strings object is also terminated with one implicit NUL.
        مزي_ګانې->append(0, error_کوډ);
        int32_t new_شاخص = مزي_ګانې->اوږدتیا();
        مزي_ګانې->appendInvariantChars(p, u_strlen(p), error_کوډ);
        uhash_puti(&map, const_cast<char16_t *>(p), new_شاخص, &error_کوډ);
        أرجع new_شاخص;
    }

    /**
     * Adds a unicode string by value and returns a unique number for it.
     */
    int32_t جمع_کړه_By_ارزښت(UnicodeString s, UError_کوډ &error_کوډ) {
        إذا (U_FAILURE(error_کوډ)) { أرجع -1; }
        إذا (isFrozen) {
            error_کوډ = U_NO_WRITE_PERMISSION;
            أرجع -1;
        }
        int32_t old_شاخص = uhash_geti(&map, s.get_Terminated_بفر());
        إذا (old_شاخص != 0) {  // found duplicate
            أرجع old_شاخص;
        }
        // We need to store the string content of the UnicodeString.
        UnicodeString *کلی = کلی_Store.create(s);
        إذا (کلی == nullptr) {
            error_کوډ = U_حافظه_ویش_ERROR;
            أرجع -1;
        }
        أرجع جمع_کړه(کلی->get_Terminated_بفر(), error_کوډ);
    }

    فراغ freeze() { isFrozen = true; }

    /**
     * Returns a string pointer for its unique number, if this object is frozen.
     * Otherwise nullptr.
     */
    ثابت حرف *get(int32_t i) ثابت {
        U_ASSERT(isFrozen);
        أرجع isFrozen && i > 0 ? مزي_ګانې->معلومات() + i : nullptr;
    }

خاص:
    UHashtable map;
    CharString *مزي_ګانې;
    حافظه_Pool<UnicodeString> کلی_Store;
    bool isFrozen = false;
};

U_NAMESPACE_پای_ټکی

#د_که_پای  // __UNIQUECHARSTR_H__
