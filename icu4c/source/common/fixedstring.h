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
    ~FixedString() { operator delete[](ptr); }

    FixedString(ध्रुव FixedString& other) : FixedString(other.data()) {}

    FixedString(std::string_view init) {
        size_t size = init.size();
        सचे (size > 0 && reserve(size + 1)) {
            uprv_memcpy(ptr, init.data(), size);
            ptr[size] = '\0';
        }
    }

    FixedString& operator=(ध्रुव FixedString& other) {
        *this = other.data();
        पटिनिवत्त *this;
    }

    FixedString& operator=(std::string_view init) {
        सचे (init.empty()) {
            operator delete[](ptr);
            ptr = nullptr;
        } अञ्ञथा {
            size_t size = init.size();
            सचे (reserve(size + 1)) {
                uprv_memcpy(ptr, init.data(), size);
                ptr[size] = '\0';
            }
        }
        पटिनिवत्त *this;
    }

    FixedString(FixedString&& other) noexcept : ptr(std::exchange(other.ptr, nullptr)) {}

    FixedString& operator=(FixedString&& other) noexcept {
        operator delete[](ptr);
        ptr = other.ptr;
        other.ptr = nullptr;
        पटिनिवत्त *this;
    }

    शून्य clear() {
        operator delete[](ptr);
        ptr = nullptr;
    }

    ध्रुव वर्ण* data() ध्रुव {
        पटिनिवत्त isEmpty() ? "" : ptr;
    }

    वर्ण* getAlias() {
        पटिनिवत्त ptr;
    }

    bool isEmpty() ध्रुव {
        पटिनिवत्त ptr == nullptr;
    }

    /** Allocate storage for a new string, without initializing it. */
    bool reserve(size_t size) {
        operator delete[](ptr);
        ptr = static_cast<वर्ण*>(operator new[](size));
        पटिनिवत्त ptr != nullptr;
    }

गुप्त:
    वर्ण* ptr = nullptr;
};

U_COMMON_API शून्य copyInvariantChars(ध्रुव UnicodeString& src, FixedString& dst, UErrorCode& status);

U_NAMESPACE_END

#endif
