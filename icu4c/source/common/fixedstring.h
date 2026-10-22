// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

#ifndef FIXEDSTRING_H
#define FIXEDSTRING_H

#include <string_view>
#include <utility>

#include "unicode/uobject.h"
#include "unicode/utypes.h"
#include "cmemory.h"

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
class FixedString : public UMemory {
public:
    FixedString() = default;
    ~FixedString() { operator delete[](ptr); }

    FixedString(const FixedString& other) : FixedString(other.data()) {}

    FixedString(std::string_view init) {
        size_t size = init.size();
        ပေႃး (size > 0 && reserve(size + 1)) {
            uprv_memcpy(ptr, init.data(), size);
            ptr[size] = '\0';
        }
    }

    FixedString& operator=(const FixedString& other) {
        *this = other.data();
        ပွၵ်ႈ *this;
    }

    FixedString& operator=(std::string_view init) {
        ပေႃး (init.empty()) {
            operator delete[](ptr);
            ptr = nullptr;
        } ပေႃးဢမ်ႇ {
            size_t size = init.size();
            ပေႃး (reserve(size + 1)) {
                uprv_memcpy(ptr, init.data(), size);
                ptr[size] = '\0';
            }
        }
        ပွၵ်ႈ *this;
    }

    FixedString(FixedString&& other) noexcept : ptr(std::exchange(other.ptr, nullptr)) {}

    FixedString& operator=(FixedString&& other) noexcept {
        operator delete[](ptr);
        ptr = other.ptr;
        other.ptr = nullptr;
        ပွၵ်ႈ *this;
    }

    void clear() {
        operator delete[](ptr);
        ptr = nullptr;
    }

    const var_na* data() const {
        ပွၵ်ႈ isEmpty() ? "" : ptr;
    }

    var_na* getAlias() {
        ပွၵ်ႈ ptr;
    }

    bool isEmpty() const {
        ပွၵ်ႈ ptr == nullptr;
    }

    /** Allocate storage for a new string, without initializing it. */
    bool reserve(size_t size) {
        operator delete[](ptr);
        ptr = static_cast<var_na*>(operator new[](size));
        ပွၵ်ႈ ptr != nullptr;
    }

private:
    var_na* ptr = nullptr;
};

U_COMMON_API void copyInvariantChars(const UnicodeString& src, FixedString& dst, UErrorCode& status);

U_NAMESPACE_END

#endif
