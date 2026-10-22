// © 2020 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// uniquecharstr.h
// created: 2020sep01 Frank Yung-Fong Tang

#اگر_تعریف_نشده __UNIQUECHARSTR_H__
#تعریف_کن __UNIQUECHARSTR_H__

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
class Unique_Char_رشتهها {
عام:
    Unique_Char_رشتهها(UError_کد &error_کد) : رشتهها(nullptr) {
        // Note: We hash on string contents but store stable char16_t * pointers.
        // If the strings are stored in resource bundles which should be built with
        // duplicate elimination, then we should be able to hash on just the pointer values.
        uhash_init(&map, uhash_hashUChars, uhash_compareUChars, uhash_compareLong, &error_کد);
        إذا (U_FAILURE(error_کد)) { أرجع; }
        رشتهها = new CharString();
        إذا (رشتهها == nullptr) {
            error_کد = U_حافظه_تخصیص_ERROR;
        }
    }
    ~Unique_Char_رشتهها() {
        uhash_close(&map);
        delete رشتهها;
    }

    /** Returns/orphans the CharString that contains all strings. */
    CharString *orphan_Char_رشتهها() {
        CharString *نتیجه = رشتهها;
        رشتهها = nullptr;
        أرجع نتیجه;
    }

    /**
     * Adds a NUL-terminated string and returns a unique number for it.
     * The string must not change, nor move around in memory,
     * while this UniqueCharStrings is in use.
     *
     * Best used with string data in a stable storage, such as strings returned
     * by resource bundle functions.
     */
    int32_t جمع_کن(ثابت char16_t*p, UError_کد &error_کد) {
        إذا (U_FAILURE(error_کد)) { أرجع -1; }
        إذا (isFrozen) {
            error_کد = U_NO_WRITE_PERMISSION;
            أرجع -1;
        }
        // The string points into the resource bundle.
        int32_t old_شاخص = uhash_geti(&map, p);
        إذا (old_شاخص != 0) {  // found duplicate
            أرجع old_شاخص;
        }
        // Explicit NUL terminator for the previous string.
        // The strings object is also terminated with one implicit NUL.
        رشتهها->append(0, error_کد);
        int32_t new_شاخص = رشتهها->درازی();
        رشتهها->appendInvariantChars(p, u_strlen(p), error_کد);
        uhash_puti(&map, const_cast<char16_t *>(p), new_شاخص, &error_کد);
        أرجع new_شاخص;
    }

    /**
     * Adds a unicode string by value and returns a unique number for it.
     */
    int32_t جمع_کن_By_مقدار(UnicodeString s, UError_کد &error_کد) {
        إذا (U_FAILURE(error_کد)) { أرجع -1; }
        إذا (isFrozen) {
            error_کد = U_NO_WRITE_PERMISSION;
            أرجع -1;
        }
        int32_t old_شاخص = uhash_geti(&map, s.get_Terminated_میانگیر());
        إذا (old_شاخص != 0) {  // found duplicate
            أرجع old_شاخص;
        }
        // We need to store the string content of the UnicodeString.
        UnicodeString *کلید = کلید_Store.create(s);
        إذا (کلید == nullptr) {
            error_کد = U_حافظه_تخصیص_ERROR;
            أرجع -1;
        }
        أرجع جمع_کن(کلید->get_Terminated_میانگیر(), error_کد);
    }

    فراغ freeze() { isFrozen = true; }

    /**
     * Returns a string pointer for its unique number, if this object is frozen.
     * Otherwise nullptr.
     */
    ثابت حرف *get(int32_t i) ثابت {
        U_ASSERT(isFrozen);
        أرجع isFrozen && i > 0 ? رشتهها->داده() + i : nullptr;
    }

خاص:
    UHashtable map;
    CharString *رشتهها;
    حافظه_Pool<UnicodeString> کلید_Store;
    bool isFrozen = false;
};

U_NAMESPACE_انتها

#پایان_اگر  // __UNIQUECHARSTR_H__
