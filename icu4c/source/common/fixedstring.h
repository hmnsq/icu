// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

#ifndef FIXEDSTRING_H
#define FIXEDSTRING_H

#ಅನ್ತರ್ಭಾವ <string_view>
#ಅನ್ತರ್ಭಾವ <utility>

#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"

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
class FixedString : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:
    FixedString() = ಸ್ವಭಾವ;
    ~FixedString() { operator delete[](ptr); }

    FixedString(ಧ್ರುವ FixedString& other) : FixedString(other.data()) {}

    FixedString(std::string_view init) {
        size_t size = init.size();
        ಯದಿ (size > 0 && reserve(size + 1)) {
            uprv_memcpy(ptr, init.data(), size);
            ptr[size] = '\0';
        }
    }

    FixedString& operator=(ಧ್ರುವ FixedString& other) {
        *this = other.data();
        ಪ್ರತ್ಯಾಗಮ *this;
    }

    FixedString& operator=(std::string_view init) {
        ಯದಿ (init.empty()) {
            operator delete[](ptr);
            ptr = nullptr;
        } ಅನ್ಯಥಾ {
            size_t size = init.size();
            ಯದಿ (reserve(size + 1)) {
                uprv_memcpy(ptr, init.data(), size);
                ptr[size] = '\0';
            }
        }
        ಪ್ರತ್ಯಾಗಮ *this;
    }

    FixedString(FixedString&& other) noexcept : ptr(std::exchange(other.ptr, nullptr)) {}

    FixedString& operator=(FixedString&& other) noexcept {
        operator delete[](ptr);
        ptr = other.ptr;
        other.ptr = nullptr;
        ಪ್ರತ್ಯಾಗಮ *this;
    }

    ಶೂನ್ಯ clear() {
        operator delete[](ptr);
        ptr = nullptr;
    }

    ಧ್ರುವ ವರ್ಣ* data() ಧ್ರುವ {
        ಪ್ರತ್ಯಾಗಮ isEmpty() ? "" : ptr;
    }

    ವರ್ಣ* getAlias() {
        ಪ್ರತ್ಯಾಗಮ ptr;
    }

    bool isEmpty() ಧ್ರುವ {
        ಪ್ರತ್ಯಾಗಮ ptr == nullptr;
    }

    /** Allocate storage for a new string, without initializing it. */
    bool reserve(size_t size) {
        operator delete[](ptr);
        ptr = static_cast<ವರ್ಣ*>(operator new[](size));
        ಪ್ರತ್ಯಾಗಮ ptr != nullptr;
    }

ಗುಪ್ತ:
    ವರ್ಣ* ptr = nullptr;
};

U_COMMON_API ಶೂನ್ಯ copyInvariantChars(ಧ್ರುವ UnicodeString& src, FixedString& dst, UErrorCode& status);

U_NAMESPACE_END

#endif
