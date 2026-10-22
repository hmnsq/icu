// © 2020 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// charstrmap.h
// created: 2020sep01 Frank Yung-Fong Tang

#अगर_अघोषित __CHARSTRMAP_H__
#घोषणा __CHARSTRMAP_H__

#समावेश <utility>
#समावेश "unicode/utypes.h"
#समावेश "unicode/uobject.h"
#समावेश "uhash.h"

U_NAMESPACE_BEGIN

/**
 * Map of const char * keys & values.
 * Stores pointers as is: Does not own/copy/adopt/release strings.
 */
class CharStringMap final : खुला UMemory {
खुला:
    /** Constructs an unusable non-map. */
    CharStringMap() : map(nullptr) {}
    CharStringMap(int32_t आकार_2, UErrorCode &errorCode) {
        map = uhash_open_आकार(uhash_hashChars, uhash_compareChars, uhash_compareChars,
                             आकार_2, &errorCode);
    }
    CharStringMap(CharStringMap &&other) noexcept : map(other.map) {
        other.map = nullptr;
    }
    CharStringMap(स्थिर CharStringMap &other) = delete;
    ~CharStringMap() {
        uhash_close(map);
    }

    CharStringMap &चालक=(CharStringMap &&other) noexcept {
        map = other.map;
        other.map = nullptr;
        वापस *this;
    }
    CharStringMap &चालक=(स्थिर CharStringMap &other) = delete;

    स्थिर अक्षर *get(स्थिर अक्षर *कुंजी) स्थिर { वापस static_cast<स्थिर अक्षर *>(uhash_get(map, कुंजी)); }
    व्योम put(स्थिर अक्षर *कुंजी, स्थिर अक्षर *मूल्य_2, UErrorCode &errorCode) {
        uhash_put(map, const_cast<अक्षर *>(कुंजी), const_cast<अक्षर *>(मूल्य_2), &errorCode);
    }

निजी:
    UHashtable *map;
};

U_NAMESPACE_END

#पूर्ण_अगर  //  __CHARSTRMAP_H__
