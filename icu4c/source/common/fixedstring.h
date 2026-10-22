// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FIXEDSTRING_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FIXEDSTRING_H

#ਅਨ੍ਤਰ੍ਭਾਵ <string_view>
#ਅਨ੍ਤਰ੍ਭਾਵ <utility>

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uobject.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"

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
class FixedString : ਜਨਤਕ UMemory {
ਜਨਤਕ:
    FixedString() = ਮੂਲ;
    ~FixedString() { ਸੰਚਾਲਕ delete[](ptr); }

    FixedString(ਅਚਲ FixedString& other) : FixedString(other.ਅੰਕੜੇ()) {}

    FixedString(std::string_view init) {
        size_t ਮਾਪ_2 = init.ਮਾਪ_2();
        ਜੇ (ਮਾਪ_2 > 0 && reserve(ਮਾਪ_2 + 1)) {
            uprv_memcpy(ptr, init.ਅੰਕੜੇ(), ਮਾਪ_2);
            ptr[ਮਾਪ_2] = '\0';
        }
    }

    FixedString& ਸੰਚਾਲਕ=(ਅਚਲ FixedString& other) {
        *this = other.ਅੰਕੜੇ();
        ਵਾਪਸ *this;
    }

    FixedString& ਸੰਚਾਲਕ=(std::string_view init) {
        ਜੇ (init.ਖਾਲੀ_ਥਾਵਾਂ()) {
            ਸੰਚਾਲਕ delete[](ptr);
            ptr = nullptr;
        } ਨਹੀਂਤਾਂ {
            size_t ਮਾਪ_2 = init.ਮਾਪ_2();
            ਜੇ (reserve(ਮਾਪ_2 + 1)) {
                uprv_memcpy(ptr, init.ਅੰਕੜੇ(), ਮਾਪ_2);
                ptr[ਮਾਪ_2] = '\0';
            }
        }
        ਵਾਪਸ *this;
    }

    FixedString(FixedString&& other) noexcept : ptr(std::exchange(other.ptr, nullptr)) {}

    FixedString& ਸੰਚਾਲਕ=(FixedString&& other) noexcept {
        ਸੰਚਾਲਕ delete[](ptr);
        ptr = other.ptr;
        other.ptr = nullptr;
        ਵਾਪਸ *this;
    }

    ਖਾਲੀ clear() {
        ਸੰਚਾਲਕ delete[](ptr);
        ptr = nullptr;
    }

    ਅਚਲ ਅੱਖਰ* ਅੰਕੜੇ() ਅਚਲ {
        ਵਾਪਸ is_ਖਾਲੀ_ਥਾਵਾਂ() ? "" : ptr;
    }

    ਅੱਖਰ* getAlias() {
        ਵਾਪਸ ptr;
    }

    bool is_ਖਾਲੀ_ਥਾਵਾਂ() ਅਚਲ {
        ਵਾਪਸ ptr == nullptr;
    }

    /** Allocate storage for a new string, without initializing it. */
    bool reserve(size_t ਮਾਪ_2) {
        ਸੰਚਾਲਕ delete[](ptr);
        ptr = static_cast<ਅੱਖਰ*>(ਸੰਚਾਲਕ new[](ਮਾਪ_2));
        ਵਾਪਸ ptr != nullptr;
    }

ਨਿਜੀ:
    ਅੱਖਰ* ptr = nullptr;
};

U_COMMON_API ਖਾਲੀ copyInvariantChars(ਅਚਲ UnicodeString& src, FixedString& dst, UError_ਕੋਡ& ਸਥਿਤੀ);

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ
