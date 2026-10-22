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
    FixedString() = स्वभाव;
    ~FixedString() { चालक delete[](ptr); }

    FixedString(अचर FixedString& other) : FixedString(other.data()) {}

    FixedString(std::string_view init) {
        माप_प्रकार size = init.size();
        यदि (size > 0 && reserve(size + 1)) {
            uprv_memcpy(ptr, init.data(), size);
            ptr[size] = '\0';
        }
    }

    FixedString& चालक=(अचर FixedString& other) {
        *this = other.data();
        फर्काऊ *this;
    }

    FixedString& चालक=(std::string_view init) {
        यदि (init.empty()) {
            चालक delete[](ptr);
            ptr = nullptr;
        } अन्यथा {
            माप_प्रकार size = init.size();
            यदि (reserve(size + 1)) {
                uprv_memcpy(ptr, init.data(), size);
                ptr[size] = '\0';
            }
        }
        फर्काऊ *this;
    }

    FixedString(FixedString&& other) noexcept : ptr(std::exchange(other.ptr, nullptr)) {}

    FixedString& चालक=(FixedString&& other) noexcept {
        चालक delete[](ptr);
        ptr = other.ptr;
        other.ptr = nullptr;
        फर्काऊ *this;
    }

    शून्य clear() {
        चालक delete[](ptr);
        ptr = nullptr;
    }

    अचर वर्ण* data() अचर {
        फर्काऊ isEmpty() ? "" : ptr;
    }

    वर्ण* getAlias() {
        फर्काऊ ptr;
    }

    bool isEmpty() अचर {
        फर्काऊ ptr == nullptr;
    }

    /** Allocate storage for a new string, without initializing it. */
    bool reserve(माप_प्रकार size) {
        चालक delete[](ptr);
        ptr = static_cast<वर्ण*>(चालक new[](size));
        फर्काऊ ptr != nullptr;
    }

गुप्त:
    वर्ण* ptr = nullptr;
};

U_COMMON_API शून्य copyInvariantChars(अचर UnicodeString& src, FixedString& dst, UErrorCode& status);

U_NAMESPACE_END

#endif
