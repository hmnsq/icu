// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

#यदि_अपरिभाषितम् FIXEDSTRING_H
#परिभाषय FIXEDSTRING_H

#अन्तर्भाव <string_view>
#अन्तर्भाव <utility>

#अन्तर्भाव "unicode/uobject.h"
#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "cmemory.h"

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
class FixedString : सार्वजनिक UMemory {
सार्वजनिक:
    FixedString() = स्वभाव;
    ~FixedString() { संक्रियकः delete[](ptr); }

    FixedString(ध्रुव FixedString& other) : FixedString(other.दत्त()) {}

    FixedString(std::string_view init) {
        size_t आकार_2 = init.आकार_2();
        यदि (आकार_2 > 0 && reserve(आकार_2 + 1)) {
            uprv_memcpy(ptr, init.दत्त(), आकार_2);
            ptr[आकार_2] = '\0';
        }
    }

    FixedString& संक्रियकः=(ध्रुव FixedString& other) {
        *this = other.दत्त();
        प्रत्यागम *this;
    }

    FixedString& संक्रियकः=(std::string_view init) {
        यदि (init.रिक्तानि()) {
            संक्रियकः delete[](ptr);
            ptr = nullptr;
        } अन्यथा {
            size_t आकार_2 = init.आकार_2();
            यदि (reserve(आकार_2 + 1)) {
                uprv_memcpy(ptr, init.दत्त(), आकार_2);
                ptr[आकार_2] = '\0';
            }
        }
        प्रत्यागम *this;
    }

    FixedString(FixedString&& other) noexcept : ptr(std::exchange(other.ptr, nullptr)) {}

    FixedString& संक्रियकः=(FixedString&& other) noexcept {
        संक्रियकः delete[](ptr);
        ptr = other.ptr;
        other.ptr = nullptr;
        प्रत्यागम *this;
    }

    शून्य clear() {
        संक्रियकः delete[](ptr);
        ptr = nullptr;
    }

    ध्रुव वर्ण* दत्त() ध्रुव {
        प्रत्यागम is_रिक्तानि() ? "" : ptr;
    }

    वर्ण* getAlias() {
        प्रत्यागम ptr;
    }

    bool is_रिक्तानि() ध्रुव {
        प्रत्यागम ptr == nullptr;
    }

    /** Allocate storage for a new string, without initializing it. */
    bool reserve(size_t आकार_2) {
        संक्रियकः delete[](ptr);
        ptr = static_cast<वर्ण*>(संक्रियकः new[](आकार_2));
        प्रत्यागम ptr != nullptr;
    }

गुप्त:
    वर्ण* ptr = nullptr;
};

U_COMMON_API शून्य copyInvariantChars(ध्रुव UnicodeString& src, FixedString& dst, UError_संहिता& स्थितिः);

U_NAMESPACE_अन्तः

#यदि_अन्तः
