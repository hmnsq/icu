// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

#که_نه_تعریف_شوی FIXEDSTRING_H
#تعریف_کړه FIXEDSTRING_H

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
    ~FixedString() { چلوونکی delete[](ptr); }

    FixedString(ثابت FixedString& other) : FixedString(other.معلومات()) {}

    FixedString(std::string_view init) {
        size_t اندازه_2 = init.اندازه_2();
        إذا (اندازه_2 > 0 && reserve(اندازه_2 + 1)) {
            uprv_memcpy(ptr, init.معلومات(), اندازه_2);
            ptr[اندازه_2] = '\0';
        }
    }

    FixedString& چلوونکی=(ثابت FixedString& other) {
        *this = other.معلومات();
        أرجع *this;
    }

    FixedString& چلوونکی=(std::string_view init) {
        إذا (init.تشه()) {
            چلوونکی delete[](ptr);
            ptr = nullptr;
        } وإلا {
            size_t اندازه_2 = init.اندازه_2();
            إذا (reserve(اندازه_2 + 1)) {
                uprv_memcpy(ptr, init.معلومات(), اندازه_2);
                ptr[اندازه_2] = '\0';
            }
        }
        أرجع *this;
    }

    FixedString(FixedString&& other) noexcept : ptr(std::exchange(other.ptr, nullptr)) {}

    FixedString& چلوونکی=(FixedString&& other) noexcept {
        چلوونکی delete[](ptr);
        ptr = other.ptr;
        other.ptr = nullptr;
        أرجع *this;
    }

    فراغ clear() {
        چلوونکی delete[](ptr);
        ptr = nullptr;
    }

    ثابت حرف* معلومات() ثابت {
        أرجع is_تشه() ? "" : ptr;
    }

    حرف* getAlias() {
        أرجع ptr;
    }

    bool is_تشه() ثابت {
        أرجع ptr == nullptr;
    }

    /** Allocate storage for a new string, without initializing it. */
    bool reserve(size_t اندازه_2) {
        چلوونکی delete[](ptr);
        ptr = static_cast<حرف*>(چلوونکی new[](اندازه_2));
        أرجع ptr != nullptr;
    }

خاص:
    حرف* ptr = nullptr;
};

U_COMMON_API فراغ copyInvariantChars(ثابت UnicodeString& src, FixedString& dst, UError_کوډ& حالت);

U_NAMESPACE_پای_ټکی

#د_که_پای
