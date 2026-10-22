// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

#ifndef FIXEDSTRING_H
#define FIXEDSTRING_H

#شامل <string_view>
#شامل <utility>

#شامل "unicode/uobject.h"
#شامل "unicode/utypes.h"
#شامل "cmemory.h"

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
class FixedString : عمومی UMemory {
عمومی:
    FixedString() = پیشفرض;
    ~FixedString() { operator delete[](ptr); }

    FixedString(ثابت FixedString& other) : FixedString(other.data()) {}

    FixedString(std::string_view init) {
        size_t size = init.size();
        اگر (size > 0 && reserve(size + 1)) {
            uprv_memcpy(ptr, init.data(), size);
            ptr[size] = '\0';
        }
    }

    FixedString& operator=(ثابت FixedString& other) {
        *this = other.data();
        بازگشت *this;
    }

    FixedString& operator=(std::string_view init) {
        اگر (init.empty()) {
            operator delete[](ptr);
            ptr = nullptr;
        } وگرنه {
            size_t size = init.size();
            اگر (reserve(size + 1)) {
                uprv_memcpy(ptr, init.data(), size);
                ptr[size] = '\0';
            }
        }
        بازگشت *this;
    }

    FixedString(FixedString&& other) noexcept : ptr(std::exchange(other.ptr, nullptr)) {}

    FixedString& operator=(FixedString&& other) noexcept {
        operator delete[](ptr);
        ptr = other.ptr;
        other.ptr = nullptr;
        بازگشت *this;
    }

    تهی clear() {
        operator delete[](ptr);
        ptr = nullptr;
    }

    ثابت نویسه* data() ثابت {
        بازگشت isEmpty() ? "" : ptr;
    }

    نویسه* getAlias() {
        بازگشت ptr;
    }

    bool isEmpty() ثابت {
        بازگشت ptr == nullptr;
    }

    /** Allocate storage for a new string, without initializing it. */
    bool reserve(size_t size) {
        operator delete[](ptr);
        ptr = static_cast<نویسه*>(operator new[](size));
        بازگشت ptr != nullptr;
    }

خصوصی:
    نویسه* ptr = nullptr;
};

U_COMMON_API تهی copyInvariantChars(ثابت UnicodeString& src, FixedString& dst, UErrorCode& status);

U_NAMESPACE_END

#endif
