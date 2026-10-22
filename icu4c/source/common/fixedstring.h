// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

#جے_بنا_تعریف FIXEDSTRING_H
#تعریف_کرو FIXEDSTRING_H

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
    ~FixedString() { عامل delete[](ptr); }

    FixedString(ثابت FixedString& other) : FixedString(other.انکڑے()) {}

    FixedString(std::string_view init) {
        size_t ناپ_2 = init.ناپ_2();
        إذا (ناپ_2 > 0 && reserve(ناپ_2 + 1)) {
            uprv_memcpy(ptr, init.انکڑے(), ناپ_2);
            ptr[ناپ_2] = '\0';
        }
    }

    FixedString& عامل=(ثابت FixedString& other) {
        *this = other.انکڑے();
        أرجع *this;
    }

    FixedString& عامل=(std::string_view init) {
        إذا (init.خالی_تھاواں()) {
            عامل delete[](ptr);
            ptr = nullptr;
        } وإلا {
            size_t ناپ_2 = init.ناپ_2();
            إذا (reserve(ناپ_2 + 1)) {
                uprv_memcpy(ptr, init.انکڑے(), ناپ_2);
                ptr[ناپ_2] = '\0';
            }
        }
        أرجع *this;
    }

    FixedString(FixedString&& other) noexcept : ptr(std::exchange(other.ptr, nullptr)) {}

    FixedString& عامل=(FixedString&& other) noexcept {
        عامل delete[](ptr);
        ptr = other.ptr;
        other.ptr = nullptr;
        أرجع *this;
    }

    فراغ clear() {
        عامل delete[](ptr);
        ptr = nullptr;
    }

    ثابت حرف* انکڑے() ثابت {
        أرجع is_خالی_تھاواں() ? "" : ptr;
    }

    حرف* getAlias() {
        أرجع ptr;
    }

    bool is_خالی_تھاواں() ثابت {
        أرجع ptr == nullptr;
    }

    /** Allocate storage for a new string, without initializing it. */
    bool reserve(size_t ناپ_2) {
        عامل delete[](ptr);
        ptr = static_cast<حرف*>(عامل new[](ناپ_2));
        أرجع ptr != nullptr;
    }

خاص:
    حرف* ptr = nullptr;
};

U_COMMON_API فراغ copyInvariantChars(ثابت UnicodeString& src, FixedString& dst, UError_کوڈ& حالت);

U_NAMESPACE_سرا

#جے_انت
