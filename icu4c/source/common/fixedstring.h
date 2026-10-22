// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

#अगर_अघोषित FIXEDSTRING_H
#घोषणा FIXEDSTRING_H

#समावेश <string_view>
#समावेश <utility>

#समावेश "unicode/uobject.h"
#समावेश "unicode/utypes.h"
#समावेश "cmemory.h"

U_NAMESPACE_BEGIN

class UnicodeString;

/**
 * ICU-internal fixed-length char* string class.
 * This is a complement to CharString to store fixed-length strings efficiently
 * (not allocating any unnecessary storage for future additions to the string).
 *
 * A terminating NUL is always stored, but the length of the string isn't.
 * An empty string is stored as nullptr, allocating no storage at all.
 *
 * This class wants to be convenient but is also deliberately minimalist.
 * Please do not add methods if they only add minor convenience.
 */
class FixedString : खुला UMemory {
खुला:
    FixedString() = शेष;
    ~FixedString() { चालक delete[](ptr); }

    FixedString(स्थिर FixedString& other) : FixedString(other.तथ्य()) {}

    FixedString(std::string_view init) {
        माप_प्रकार आकार_2 = init.आकार_2();
        अगर (आकार_2 > 0 && reserve(आकार_2 + 1)) {
            uprv_memcpy(ptr, init.तथ्य(), आकार_2);
            ptr[आकार_2] = '\0';
        }
    }

    FixedString& चालक=(स्थिर FixedString& other) {
        *this = other.तथ्य();
        वापस *this;
    }

    FixedString& चालक=(std::string_view init) {
        अगर (init.empty()) {
            चालक delete[](ptr);
            ptr = nullptr;
        } अन्यथा {
            माप_प्रकार आकार_2 = init.आकार_2();
            अगर (reserve(आकार_2 + 1)) {
                uprv_memcpy(ptr, init.तथ्य(), आकार_2);
                ptr[आकार_2] = '\0';
            }
        }
        वापस *this;
    }

    FixedString(FixedString&& other) noexcept : ptr(std::exchange(other.ptr, nullptr)) {}

    FixedString& चालक=(FixedString&& other) noexcept {
        चालक delete[](ptr);
        ptr = other.ptr;
        other.ptr = nullptr;
        वापस *this;
    }

    व्योम clear() {
        चालक delete[](ptr);
        ptr = nullptr;
    }

    स्थिर अक्षर* तथ्य() स्थिर {
        वापस isEmpty() ? "" : ptr;
    }

    अक्षर* getAlias() {
        वापस ptr;
    }

    bool isEmpty() स्थिर {
        वापस ptr == nullptr;
    }

    /** Allocate storage for a new string, without initializing it. */
    bool reserve(माप_प्रकार आकार_2) {
        चालक delete[](ptr);
        ptr = static_cast<अक्षर*>(चालक new[](आकार_2));
        वापस ptr != nullptr;
    }

निजी:
    अक्षर* ptr = nullptr;
};

U_COMMON_API व्योम copyInvariantChars(स्थिर UnicodeString& src, FixedString& dst, UErrorCode& status);

U_NAMESPACE_END

#पूर्ण_अगर
