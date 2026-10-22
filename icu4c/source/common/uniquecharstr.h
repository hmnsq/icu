// © 2020 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// uniquecharstr.h
// created: 2020sep01 Frank Yung-Fong Tang

#నిర్వచితం_కాకపోతే __UNIQUECHARSTR_H__
#నిర్వచించు __UNIQUECHARSTR_H__

#అన్తర్భావ "charstr.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "uhash.h"
#అన్తర్భావ "cmemory.h"

U_NAMESPACE_BEGIN

/**
 * Stores NUL-terminated strings with duplicate elimination.
 * Checks for unique UTF-16 string pointers and converts to invariant characters.
 *
 * Intended to be stack-allocated. Add strings, get a unique number for each,
 * freeze the object, get a char * pointer for each string,
 * call orphanCharStrings() to capture the string storage, and let this object go out of scope.
 */
class Unique_Char_అక్షరశ్రేణులు {
బహిరంగ:
    Unique_Char_అక్షరశ్రేణులు(UError_సంకేతం &error_సంకేతం) : అక్షరశ్రేణులు(nullptr) {
        // Note: We hash on string contents but store stable char16_t * pointers.
        // If the strings are stored in resource bundles which should be built with
        // duplicate elimination, then we should be able to hash on just the pointer values.
        uhash_init(&map, uhash_hashUChars, uhash_compareUChars, uhash_compareLong, &error_సంకేతం);
        ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి; }
        అక్షరశ్రేణులు = new CharString();
        ఒకవేళ (అక్షరశ్రేణులు == nullptr) {
            error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        }
    }
    ~Unique_Char_అక్షరశ్రేణులు() {
        uhash_close(&map);
        delete అక్షరశ్రేణులు;
    }

    /** Returns/orphans the CharString that contains all strings. */
    CharString *orphan_Char_అక్షరశ్రేణులు() {
        CharString *ఫలితం = అక్షరశ్రేణులు;
        అక్షరశ్రేణులు = nullptr;
        తిరిగి ఫలితం;
    }

    /**
     * Adds a NUL-terminated string and returns a unique number for it.
     * The string must not change, nor move around in memory,
     * while this UniqueCharStrings is in use.
     *
     * Best used with string data in a stable storage, such as strings returned
     * by resource bundle functions.
     */
    int32_t కూడు(స్థిరరాశి char16_t*p, UError_సంకేతం &error_సంకేతం) {
        ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి -1; }
        ఒకవేళ (isFrozen) {
            error_సంకేతం = U_NO_WRITE_PERMISSION;
            తిరిగి -1;
        }
        // The string points into the resource bundle.
        int32_t old_సూచీ = uhash_geti(&map, p);
        ఒకవేళ (old_సూచీ != 0) {  // found duplicate
            తిరిగి old_సూచీ;
        }
        // Explicit NUL terminator for the previous string.
        // The strings object is also terminated with one implicit NUL.
        అక్షరశ్రేణులు->append(0, error_సంకేతం);
        int32_t new_సూచీ = అక్షరశ్రేణులు->నిడివి();
        అక్షరశ్రేణులు->appendInvariantChars(p, u_strlen(p), error_సంకేతం);
        uhash_puti(&map, const_cast<char16_t *>(p), new_సూచీ, &error_సంకేతం);
        తిరిగి new_సూచీ;
    }

    /**
     * Adds a unicode string by value and returns a unique number for it.
     */
    int32_t కూడు_By_విలువ(UnicodeString s, UError_సంకేతం &error_సంకేతం) {
        ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి -1; }
        ఒకవేళ (isFrozen) {
            error_సంకేతం = U_NO_WRITE_PERMISSION;
            తిరిగి -1;
        }
        int32_t old_సూచీ = uhash_geti(&map, s.get_Terminated_బఫరు());
        ఒకవేళ (old_సూచీ != 0) {  // found duplicate
            తిరిగి old_సూచీ;
        }
        // We need to store the string content of the UnicodeString.
        UnicodeString *కీలకం = కీలకం_Store.create(s);
        ఒకవేళ (కీలకం == nullptr) {
            error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి -1;
        }
        తిరిగి కూడు(కీలకం->get_Terminated_బఫరు(), error_సంకేతం);
    }

    శూన్య freeze() { isFrozen = true; }

    /**
     * Returns a string pointer for its unique number, if this object is frozen.
     * Otherwise nullptr.
     */
    స్థిరరాశి అక్షరం *get(int32_t i) స్థిరరాశి {
        U_ASSERT(isFrozen);
        తిరిగి isFrozen && i > 0 ? అక్షరశ్రేణులు->దత్తాంశం() + i : nullptr;
    }

అంతరంగ:
    UHashtable map;
    CharString *అక్షరశ్రేణులు;
    జ్ఞాపకం_Pool<UnicodeString> కీలకం_Store;
    bool isFrozen = false;
};

U_NAMESPACE_చివర

#అయితే_ముగింపు  // __UNIQUECHARSTR_H__
