// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

#ifndef FIXEDSTRING_H
#define FIXEDSTRING_H

#অন্তর্ভাব <string_view>
#অন্তর্ভাব <utility>

#অন্তর্ভাব "unicode/uobject.h"
#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "cmemory.h"

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
class FixedString : সার্বজনিক UMemory {
সার্বজনিক:
    FixedString() = স্বভাব;
    ~FixedString() { operator delete[](ptr); }

    FixedString(ধ্রুব FixedString& other) : FixedString(other.data()) {}

    FixedString(std::string_view init) {
        size_t size = init.size();
        যদি (size > 0 && reserve(size + 1)) {
            uprv_memcpy(ptr, init.data(), size);
            ptr[size] = '\0';
        }
    }

    FixedString& operator=(ধ্রুব FixedString& other) {
        *this = other.data();
        প্রত্যাগম *this;
    }

    FixedString& operator=(std::string_view init) {
        যদি (init.empty()) {
            operator delete[](ptr);
            ptr = nullptr;
        } অন্যথা {
            size_t size = init.size();
            যদি (reserve(size + 1)) {
                uprv_memcpy(ptr, init.data(), size);
                ptr[size] = '\0';
            }
        }
        প্রত্যাগম *this;
    }

    FixedString(FixedString&& other) noexcept : ptr(std::exchange(other.ptr, nullptr)) {}

    FixedString& operator=(FixedString&& other) noexcept {
        operator delete[](ptr);
        ptr = other.ptr;
        other.ptr = nullptr;
        প্রত্যাগম *this;
    }

    শূন্য clear() {
        operator delete[](ptr);
        ptr = nullptr;
    }

    ধ্রুব বর্ণ* data() ধ্রুব {
        প্রত্যাগম isEmpty() ? "" : ptr;
    }

    বর্ণ* getAlias() {
        প্রত্যাগম ptr;
    }

    bool isEmpty() ধ্রুব {
        প্রত্যাগম ptr == nullptr;
    }

    /** Allocate storage for a new string, without initializing it. */
    bool reserve(size_t size) {
        operator delete[](ptr);
        ptr = static_cast<বর্ণ*>(operator new[](size));
        প্রত্যাগম ptr != nullptr;
    }

গুপ্ত:
    বর্ণ* ptr = nullptr;
};

U_COMMON_API শূন্য copyInvariantChars(ধ্রুব UnicodeString& src, FixedString& dst, UErrorCode& status);

U_NAMESPACE_END

#endif
