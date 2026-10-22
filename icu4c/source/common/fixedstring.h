// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

#اگر_غیر_تعریف_شدہ FIXEDSTRING_H
#تعریف_کرو FIXEDSTRING_H

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
class FixedString : عوامی UMemory {
عوامی:
    FixedString() = طےشدہ;
    ~FixedString() { عامل delete[](ptr); }

    FixedString(ثابت FixedString& other) : FixedString(other.ڈیٹا()) {}

    FixedString(std::string_view init) {
        size_t سائز_2 = init.سائز_2();
        اگر (سائز_2 > 0 && reserve(سائز_2 + 1)) {
            uprv_memcpy(ptr, init.ڈیٹا(), سائز_2);
            ptr[سائز_2] = '\0';
        }
    }

    FixedString& عامل=(ثابت FixedString& other) {
        *this = other.ڈیٹا();
        واپس *this;
    }

    FixedString& عامل=(std::string_view init) {
        اگر (init.خالی_خانے()) {
            عامل delete[](ptr);
            ptr = nullptr;
        } ورنہ {
            size_t سائز_2 = init.سائز_2();
            اگر (reserve(سائز_2 + 1)) {
                uprv_memcpy(ptr, init.ڈیٹا(), سائز_2);
                ptr[سائز_2] = '\0';
            }
        }
        واپس *this;
    }

    FixedString(FixedString&& other) noexcept : ptr(std::exchange(other.ptr, nullptr)) {}

    FixedString& عامل=(FixedString&& other) noexcept {
        عامل delete[](ptr);
        ptr = other.ptr;
        other.ptr = nullptr;
        واپس *this;
    }

    خالی clear() {
        عامل delete[](ptr);
        ptr = nullptr;
    }

    ثابت حرف* ڈیٹا() ثابت {
        واپس is_خالی_خانے() ? "" : ptr;
    }

    حرف* getAlias() {
        واپس ptr;
    }

    bool is_خالی_خانے() ثابت {
        واپس ptr == nullptr;
    }

    /** Allocate storage for a new string, without initializing it. */
    bool reserve(size_t سائز_2) {
        عامل delete[](ptr);
        ptr = static_cast<حرف*>(عامل new[](سائز_2));
        واپس ptr != nullptr;
    }

نجی:
    حرف* ptr = nullptr;
};

U_COMMON_API خالی copyInvariantChars(ثابت UnicodeString& src, FixedString& dst, UError_رمز& حالت_2);

U_NAMESPACE_انتہا

#اگر_ختم
