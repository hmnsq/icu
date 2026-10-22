// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

#جيڪڏهن_وصف_نه_ٿيل FIXEDSTRING_H
#وصف_ڪريو FIXEDSTRING_H

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

    FixedString(ثابت FixedString& other) : FixedString(other.معلومات()) {}

    FixedString(std::string_view init) {
        size_t سائيز_2 = init.سائيز_2();
        إذا (سائيز_2 > 0 && reserve(سائيز_2 + 1)) {
            uprv_memcpy(ptr, init.معلومات(), سائيز_2);
            ptr[سائيز_2] = '\0';
        }
    }

    FixedString& عامل=(ثابت FixedString& other) {
        *this = other.معلومات();
        أرجع *this;
    }

    FixedString& عامل=(std::string_view init) {
        إذا (init.خالي_جايون()) {
            عامل delete[](ptr);
            ptr = nullptr;
        } وإلا {
            size_t سائيز_2 = init.سائيز_2();
            إذا (reserve(سائيز_2 + 1)) {
                uprv_memcpy(ptr, init.معلومات(), سائيز_2);
                ptr[سائيز_2] = '\0';
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

    ثابت حرف* معلومات() ثابت {
        أرجع is_خالي_جايون() ? "" : ptr;
    }

    حرف* getAlias() {
        أرجع ptr;
    }

    bool is_خالي_جايون() ثابت {
        أرجع ptr == nullptr;
    }

    /** Allocate storage for a new string, without initializing it. */
    bool reserve(size_t سائيز_2) {
        عامل delete[](ptr);
        ptr = static_cast<حرف*>(عامل new[](سائيز_2));
        أرجع ptr != nullptr;
    }

خاص:
    حرف* ptr = nullptr;
};

U_COMMON_API فراغ copyInvariantChars(ثابت UnicodeString& src, FixedString& dst, UError_ڪوڊ& حالت);

U_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو
