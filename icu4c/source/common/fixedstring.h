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
class FixedString : সর্বজনীন UMemory {
সর্বজনীন:
    FixedString() = পূর্বনির্ধারিত;
    ~FixedString() { operator delete[](ptr); }

    FixedString(ধ্রুবক FixedString& other) : FixedString(other.তথ্য()) {}

    FixedString(std::string_view init) {
        size_t আকার_3 = init.আকার_3();
        যদি (আকার_3 > 0 && reserve(আকার_3 + 1)) {
            uprv_memcpy(ptr, init.তথ্য(), আকার_3);
            ptr[আকার_3] = '\0';
        }
    }

    FixedString& operator=(ধ্রুবক FixedString& other) {
        *this = other.তথ্য();
        ফেরত *this;
    }

    FixedString& operator=(std::string_view init) {
        যদি (init.empty()) {
            operator delete[](ptr);
            ptr = nullptr;
        } অন্যথা {
            size_t আকার_3 = init.আকার_3();
            যদি (reserve(আকার_3 + 1)) {
                uprv_memcpy(ptr, init.তথ্য(), আকার_3);
                ptr[আকার_3] = '\0';
            }
        }
        ফেরত *this;
    }

    FixedString(FixedString&& other) noexcept : ptr(std::exchange(other.ptr, nullptr)) {}

    FixedString& operator=(FixedString&& other) noexcept {
        operator delete[](ptr);
        ptr = other.ptr;
        other.ptr = nullptr;
        ফেরত *this;
    }

    শূন্য clear() {
        operator delete[](ptr);
        ptr = nullptr;
    }

    ধ্রুবক অক্ষর* তথ্য() ধ্রুবক {
        ফেরত isEmpty() ? "" : ptr;
    }

    অক্ষর* getAlias() {
        ফেরত ptr;
    }

    bool isEmpty() ধ্রুবক {
        ফেরত ptr == nullptr;
    }

    /** Allocate storage for a new string, without initializing it. */
    bool reserve(size_t আকার_3) {
        operator delete[](ptr);
        ptr = static_cast<অক্ষর*>(operator new[](আকার_3));
        ফেরত ptr != nullptr;
    }

ব্যক্তিগত:
    অক্ষর* ptr = nullptr;
};

U_COMMON_API শূন্য copyInvariantChars(ধ্রুবক UnicodeString& src, FixedString& dst, UErrorCode& status);

U_NAMESPACE_END

#endif
