// © 2020 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// uniquecharstr.h
// created: 2020sep01 Frank Yung-Fong Tang

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __UNIQUECHARSTR_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __UNIQUECHARSTR_H__

#ਅਨ੍ਤਰ੍ਭਾਵ "charstr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uhash.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"

U_NAMESPACE_BEGIN

/**
 * Stores NUL-terminated strings with duplicate elimination.
 * Checks for unique UTF-16 string pointers and converts to invariant characters.
 *
 * Intended to be stack-allocated. Add strings, get a unique number for each,
 * freeze the object, get a char * pointer for each string,
 * call orphanCharStrings() to capture the string storage, and let this object go out of scope.
 */
class Unique_Char_ਲੜੀਆਂ {
ਜਨਤਕ:
    Unique_Char_ਲੜੀਆਂ(UError_ਕੋਡ &error_ਕੋਡ) : ਲੜੀਆਂ(nullptr) {
        // Note: We hash on string contents but store stable char16_t * pointers.
        // If the strings are stored in resource bundles which should be built with
        // duplicate elimination, then we should be able to hash on just the pointer values.
        uhash_init(&map, uhash_hashUChars, uhash_compareUChars, uhash_compareLong, &error_ਕੋਡ);
        ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ; }
        ਲੜੀਆਂ = new CharString();
        ਜੇ (ਲੜੀਆਂ == nullptr) {
            error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        }
    }
    ~Unique_Char_ਲੜੀਆਂ() {
        uhash_close(&map);
        delete ਲੜੀਆਂ;
    }

    /** Returns/orphans the CharString that contains all strings. */
    CharString *orphan_Char_ਲੜੀਆਂ() {
        CharString *ਨਤੀਜਾ = ਲੜੀਆਂ;
        ਲੜੀਆਂ = nullptr;
        ਵਾਪਸ ਨਤੀਜਾ;
    }

    /**
     * Adds a NUL-terminated string and returns a unique number for it.
     * The string must not change, nor move around in memory,
     * while this UniqueCharStrings is in use.
     *
     * Best used with string data in a stable storage, such as strings returned
     * by resource bundle functions.
     */
    int32_t ਜੋੜੋ(ਅਚਲ char16_t*p, UError_ਕੋਡ &error_ਕੋਡ) {
        ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ -1; }
        ਜੇ (isFrozen) {
            error_ਕੋਡ = U_NO_WRITE_PERMISSION;
            ਵਾਪਸ -1;
        }
        // The string points into the resource bundle.
        int32_t old_ਸੂਚਕਾਂਕ = uhash_geti(&map, p);
        ਜੇ (old_ਸੂਚਕਾਂਕ != 0) {  // found duplicate
            ਵਾਪਸ old_ਸੂਚਕਾਂਕ;
        }
        // Explicit NUL terminator for the previous string.
        // The strings object is also terminated with one implicit NUL.
        ਲੜੀਆਂ->append(0, error_ਕੋਡ);
        int32_t new_ਸੂਚਕਾਂਕ = ਲੜੀਆਂ->ਲੰਮਾਈ();
        ਲੜੀਆਂ->appendInvariantChars(p, u_strlen(p), error_ਕੋਡ);
        uhash_puti(&map, const_cast<char16_t *>(p), new_ਸੂਚਕਾਂਕ, &error_ਕੋਡ);
        ਵਾਪਸ new_ਸੂਚਕਾਂਕ;
    }

    /**
     * Adds a unicode string by value and returns a unique number for it.
     */
    int32_t ਜੋੜੋ_By_ਮੁੱਲ(UnicodeString s, UError_ਕੋਡ &error_ਕੋਡ) {
        ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ -1; }
        ਜੇ (isFrozen) {
            error_ਕੋਡ = U_NO_WRITE_PERMISSION;
            ਵਾਪਸ -1;
        }
        int32_t old_ਸੂਚਕਾਂਕ = uhash_geti(&map, s.get_Terminated_ਬਫਰ());
        ਜੇ (old_ਸੂਚਕਾਂਕ != 0) {  // found duplicate
            ਵਾਪਸ old_ਸੂਚਕਾਂਕ;
        }
        // We need to store the string content of the UnicodeString.
        UnicodeString *ਕੁੰਜੀ = ਕੁੰਜੀ_Store.create(s);
        ਜੇ (ਕੁੰਜੀ == nullptr) {
            error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ -1;
        }
        ਵਾਪਸ ਜੋੜੋ(ਕੁੰਜੀ->get_Terminated_ਬਫਰ(), error_ਕੋਡ);
    }

    ਖਾਲੀ freeze() { isFrozen = true; }

    /**
     * Returns a string pointer for its unique number, if this object is frozen.
     * Otherwise nullptr.
     */
    ਅਚਲ ਅੱਖਰ *get(int32_t i) ਅਚਲ {
        U_ASSERT(isFrozen);
        ਵਾਪਸ isFrozen && i > 0 ? ਲੜੀਆਂ->ਅੰਕੜੇ() + i : nullptr;
    }

ਨਿਜੀ:
    UHashtable map;
    CharString *ਲੜੀਆਂ;
    ਸਮ੍ਰਿਤੀ_Pool<UnicodeString> ਕੁੰਜੀ_Store;
    bool isFrozen = false;
};

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ  // __UNIQUECHARSTR_H__
