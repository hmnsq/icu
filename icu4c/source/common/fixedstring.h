// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

#اگر_تعریف_نشده FIXEDSTRING_H
#تعریف_کن FIXEDSTRING_H

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
    ~FixedString() { عملگر delete[](ptr); }

    FixedString(ثابت FixedString& other) : FixedString(other.داده()) {}

    FixedString(std::string_view init) {
        size_t اندازه_2 = init.اندازه_2();
        إذا (اندازه_2 > 0 && reserve(اندازه_2 + 1)) {
            uprv_memcpy(ptr, init.داده(), اندازه_2);
            ptr[اندازه_2] = '\0';
        }
    }

    FixedString& عملگر=(ثابت FixedString& other) {
        *this = other.داده();
        أرجع *this;
    }

    FixedString& عملگر=(std::string_view init) {
        إذا (init.خالی()) {
            عملگر delete[](ptr);
            ptr = nullptr;
        } وإلا {
            size_t اندازه_2 = init.اندازه_2();
            إذا (reserve(اندازه_2 + 1)) {
                uprv_memcpy(ptr, init.داده(), اندازه_2);
                ptr[اندازه_2] = '\0';
            }
        }
        أرجع *this;
    }

    FixedString(FixedString&& other) noexcept : ptr(std::exchange(other.ptr, nullptr)) {}

    FixedString& عملگر=(FixedString&& other) noexcept {
        عملگر delete[](ptr);
        ptr = other.ptr;
        other.ptr = nullptr;
        أرجع *this;
    }

    فراغ clear() {
        عملگر delete[](ptr);
        ptr = nullptr;
    }

    ثابت حرف* داده() ثابت {
        أرجع is_خالی() ? "" : ptr;
    }

    حرف* getAlias() {
        أرجع ptr;
    }

    bool is_خالی() ثابت {
        أرجع ptr == nullptr;
    }

    /** Allocate storage for a new string, without initializing it. */
    bool reserve(size_t اندازه_2) {
        عملگر delete[](ptr);
        ptr = static_cast<حرف*>(عملگر new[](اندازه_2));
        أرجع ptr != nullptr;
    }

خاص:
    حرف* ptr = nullptr;
};

U_COMMON_API فراغ copyInvariantChars(ثابت UnicodeString& src, FixedString& dst, UError_کد& وضعیت);

U_NAMESPACE_انتها

#پایان_اگر
