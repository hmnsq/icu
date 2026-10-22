// © 2020 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// uniquecharstr.h
// created: 2020sep01 Frank Yung-Fong Tang

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __UNIQUECHARSTR_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __UNIQUECHARSTR_H__

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
class Unique_Char_ಅಕ್ಷರಸರಣಿಗಳು {
ಸಾರ್ವಜನಿಕ:
    Unique_Char_ಅಕ್ಷರಸರಣಿಗಳು(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) : ಅಕ್ಷರಸರಣಿಗಳು(nullptr) {
        // Note: We hash on string contents but store stable char16_t * pointers.
        // If the strings are stored in resource bundles which should be built with
        // duplicate elimination, then we should be able to hash on just the pointer values.
        uhash_init(&map, uhash_hashUChars, uhash_compareUChars, uhash_compareLong, &error_ಸಂಕೇತ);
        ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ; }
        ಅಕ್ಷರಸರಣಿಗಳು = new CharString();
        ಆದರೆ (ಅಕ್ಷರಸರಣಿಗಳು == nullptr) {
            error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        }
    }
    ~Unique_Char_ಅಕ್ಷರಸರಣಿಗಳು() {
        uhash_close(&map);
        delete ಅಕ್ಷರಸರಣಿಗಳು;
    }

    /** Returns/orphans the CharString that contains all strings. */
    CharString *orphan_Char_ಅಕ್ಷರಸರಣಿಗಳು() {
        CharString *ಫಲಿತಾಂಶ = ಅಕ್ಷರಸರಣಿಗಳು;
        ಅಕ್ಷರಸರಣಿಗಳು = nullptr;
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

    /**
     * Adds a NUL-terminated string and returns a unique number for it.
     * The string must not change, nor move around in memory,
     * while this UniqueCharStrings is in use.
     *
     * Best used with string data in a stable storage, such as strings returned
     * by resource bundle functions.
     */
    int32_t ಕೂಡಿಸು(ಸ್ಥಿರಾಂಕ char16_t*p, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
        ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ -1; }
        ಆದರೆ (isFrozen) {
            error_ಸಂಕೇತ = U_NO_WRITE_PERMISSION;
            ಹಿಂತಿರುಗಿ -1;
        }
        // The string points into the resource bundle.
        int32_t old_ಸೂಚ್ಯಂಕ = uhash_geti(&map, p);
        ಆದರೆ (old_ಸೂಚ್ಯಂಕ != 0) {  // found duplicate
            ಹಿಂತಿರುಗಿ old_ಸೂಚ್ಯಂಕ;
        }
        // Explicit NUL terminator for the previous string.
        // The strings object is also terminated with one implicit NUL.
        ಅಕ್ಷರಸರಣಿಗಳು->append(0, error_ಸಂಕೇತ);
        int32_t new_ಸೂಚ್ಯಂಕ = ಅಕ್ಷರಸರಣಿಗಳು->ಉದ್ದಳತೆ();
        ಅಕ್ಷರಸರಣಿಗಳು->appendInvariantChars(p, u_strlen(p), error_ಸಂಕೇತ);
        uhash_puti(&map, const_cast<char16_t *>(p), new_ಸೂಚ್ಯಂಕ, &error_ಸಂಕೇತ);
        ಹಿಂತಿರುಗಿ new_ಸೂಚ್ಯಂಕ;
    }

    /**
     * Adds a unicode string by value and returns a unique number for it.
     */
    int32_t ಕೂಡಿಸು_By_ಮೌಲ್ಯ(UnicodeString s, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
        ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ -1; }
        ಆದರೆ (isFrozen) {
            error_ಸಂಕೇತ = U_NO_WRITE_PERMISSION;
            ಹಿಂತಿರುಗಿ -1;
        }
        int32_t old_ಸೂಚ್ಯಂಕ = uhash_geti(&map, s.get_Terminated_ಬಫರ್());
        ಆದರೆ (old_ಸೂಚ್ಯಂಕ != 0) {  // found duplicate
            ಹಿಂತಿರುಗಿ old_ಸೂಚ್ಯಂಕ;
        }
        // We need to store the string content of the UnicodeString.
        UnicodeString *ಕೀಲಿ = ಕೀಲಿ_Store.create(s);
        ಆದರೆ (ಕೀಲಿ == nullptr) {
            error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ -1;
        }
        ಹಿಂತಿರುಗಿ ಕೂಡಿಸು(ಕೀಲಿ->get_Terminated_ಬಫರ್(), error_ಸಂಕೇತ);
    }

    ಶೂನ್ಯ freeze() { isFrozen = true; }

    /**
     * Returns a string pointer for its unique number, if this object is frozen.
     * Otherwise nullptr.
     */
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *get(int32_t i) ಸ್ಥಿರಾಂಕ {
        U_ASSERT(isFrozen);
        ಹಿಂತಿರುಗಿ isFrozen && i > 0 ? ಅಕ್ಷರಸರಣಿಗಳು->ದತ್ತಾಂಶ() + i : nullptr;
    }

ಖಾಸಗಿ:
    UHashtable map;
    CharString *ಅಕ್ಷರಸರಣಿಗಳು;
    ಸ್ಮರಣೆ_Pool<UnicodeString> ಕೀಲಿ_Store;
    bool isFrozen = false;
};

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ  // __UNIQUECHARSTR_H__
