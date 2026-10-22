// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

#ifndef FIXEDSTRING_H
#define FIXEDSTRING_H

#அந்தர்பாவ <string_view>
#அந்தர்பாவ <utility>

#அந்தர்பாவ "unicode/uobject.h"
#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "cmemory.h"

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
class FixedString : பொது UMemory {
பொது:
    FixedString() = இயல்பு;
    ~FixedString() { operator delete[](ptr); }

    FixedString(மாறிலி FixedString& other) : FixedString(other.data()) {}

    FixedString(std::string_view init) {
        size_t size = init.size();
        எனில் (size > 0 && reserve(size + 1)) {
            uprv_memcpy(ptr, init.data(), size);
            ptr[size] = '\0';
        }
    }

    FixedString& operator=(மாறிலி FixedString& other) {
        *this = other.data();
        திரும்பு *this;
    }

    FixedString& operator=(std::string_view init) {
        எனில் (init.empty()) {
            operator delete[](ptr);
            ptr = nullptr;
        } இல்லையெனில் {
            size_t size = init.size();
            எனில் (reserve(size + 1)) {
                uprv_memcpy(ptr, init.data(), size);
                ptr[size] = '\0';
            }
        }
        திரும்பு *this;
    }

    FixedString(FixedString&& other) noexcept : ptr(std::exchange(other.ptr, nullptr)) {}

    FixedString& operator=(FixedString&& other) noexcept {
        operator delete[](ptr);
        ptr = other.ptr;
        other.ptr = nullptr;
        திரும்பு *this;
    }

    வெறுமை clear() {
        operator delete[](ptr);
        ptr = nullptr;
    }

    மாறிலி எழுத்து* data() மாறிலி {
        திரும்பு isEmpty() ? "" : ptr;
    }

    எழுத்து* getAlias() {
        திரும்பு ptr;
    }

    bool isEmpty() மாறிலி {
        திரும்பு ptr == nullptr;
    }

    /** Allocate storage for a new string, without initializing it. */
    bool reserve(size_t size) {
        operator delete[](ptr);
        ptr = static_cast<எழுத்து*>(operator new[](size));
        திரும்பு ptr != nullptr;
    }

தனிப்பட்ட:
    எழுத்து* ptr = nullptr;
};

U_COMMON_API வெறுமை copyInvariantChars(மாறிலி UnicodeString& src, FixedString& dst, UErrorCode& status);

U_NAMESPACE_END

#endif
