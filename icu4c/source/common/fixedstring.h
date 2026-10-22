// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

#ifndef FIXEDSTRING_H
#define FIXEDSTRING_H

#अन्तर्भाव <string_view>
#अन्तर्भाव <utility>

#अन्तर्भाव "unicode/uobject.h"
#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "cmemory.h"

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
class FixedString : सार्वजनिक UMemory {
सार्वजनिक:
    FixedString() = पूर्वनिर्धारित;
    ~FixedString() { चालक delete[](ptr); }

    FixedString(स्थिरांक FixedString& other) : FixedString(other.data()) {}

    FixedString(std::string_view init) {
        माप_प्रकार size = init.size();
        जर (size > 0 && reserve(size + 1)) {
            uprv_memcpy(ptr, init.data(), size);
            ptr[size] = '\0';
        }
    }

    FixedString& चालक=(स्थिरांक FixedString& other) {
        *this = other.data();
        परत *this;
    }

    FixedString& चालक=(std::string_view init) {
        जर (init.empty()) {
            चालक delete[](ptr);
            ptr = nullptr;
        } अन्यथा {
            माप_प्रकार size = init.size();
            जर (reserve(size + 1)) {
                uprv_memcpy(ptr, init.data(), size);
                ptr[size] = '\0';
            }
        }
        परत *this;
    }

    FixedString(FixedString&& other) noexcept : ptr(std::exchange(other.ptr, nullptr)) {}

    FixedString& चालक=(FixedString&& other) noexcept {
        चालक delete[](ptr);
        ptr = other.ptr;
        other.ptr = nullptr;
        परत *this;
    }

    शून्य clear() {
        चालक delete[](ptr);
        ptr = nullptr;
    }

    स्थिरांक अक्षर* data() स्थिरांक {
        परत isEmpty() ? "" : ptr;
    }

    अक्षर* getAlias() {
        परत ptr;
    }

    bool isEmpty() स्थिरांक {
        परत ptr == nullptr;
    }

    /** Allocate storage for a new string, without initializing it. */
    bool reserve(माप_प्रकार size) {
        चालक delete[](ptr);
        ptr = static_cast<अक्षर*>(चालक new[](size));
        परत ptr != nullptr;
    }

खाजगी:
    अक्षर* ptr = nullptr;
};

U_COMMON_API शून्य copyInvariantChars(स्थिरांक UnicodeString& src, FixedString& dst, UErrorCode& status);

U_NAMESPACE_END

#endif
