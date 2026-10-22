// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

#ifndef FIXEDSTRING_H
#define FIXEDSTRING_H

#ضمن <string_view>
#ضمن <utility>

#ضمن "unicode/uobject.h"
#ضمن "unicode/utypes.h"
#ضمن "cmemory.h"

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
class FixedString : عام UMemory {
عام:
    FixedString() = افتراضي;
    ~FixedString() { operator delete[](ptr); }

    FixedString(ثابت FixedString& other) : FixedString(other.data()) {}

    FixedString(std::string_view init) {
        size_t size = init.size();
        𐤀𐤌 (size > 0 && reserve(size + 1)) {
            uprv_memcpy(ptr, init.data(), size);
            ptr[size] = '\0';
        }
    }

    FixedString& operator=(ثابت FixedString& other) {
        *this = other.data();
        أرجع *this;
    }

    FixedString& operator=(std::string_view init) {
        𐤀𐤌 (init.empty()) {
            operator delete[](ptr);
            ptr = nullptr;
        } وإلا {
            size_t size = init.size();
            𐤀𐤌 (reserve(size + 1)) {
                uprv_memcpy(ptr, init.data(), size);
                ptr[size] = '\0';
            }
        }
        أرجع *this;
    }

    FixedString(FixedString&& other) noexcept : ptr(std::exchange(other.ptr, nullptr)) {}

    FixedString& operator=(FixedString&& other) noexcept {
        operator delete[](ptr);
        ptr = other.ptr;
        other.ptr = nullptr;
        أرجع *this;
    }

    فراغ clear() {
        operator delete[](ptr);
        ptr = nullptr;
    }

    ثابت حرف* data() ثابت {
        أرجع isEmpty() ? "" : ptr;
    }

    حرف* getAlias() {
        أرجع ptr;
    }

    bool isEmpty() ثابت {
        أرجع ptr == nullptr;
    }

    /** Allocate storage for a new string, without initializing it. */
    bool reserve(size_t size) {
        operator delete[](ptr);
        ptr = static_cast<حرف*>(operator new[](size));
        أرجع ptr != nullptr;
    }

خاص:
    حرف* ptr = nullptr;
};

U_COMMON_API فراغ copyInvariantChars(ثابت UnicodeString& src, FixedString& dst, UErrorCode& status);

U_NAMESPACE_END

#endif
