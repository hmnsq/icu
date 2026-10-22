// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

#ifndef FIXEDSTRING_H
#define FIXEDSTRING_H

#അന്തര്ഭാവ <string_view>
#അന്തര്ഭാവ <utility>

#അന്തര്ഭാവ "unicode/uobject.h"
#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "cmemory.h"

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
class FixedString : പൊതു UMemory {
പൊതു:
    FixedString() = സ്ഥിരസ്ഥിതി;
    ~FixedString() { operator delete[](ptr); }

    FixedString(സ്ഥിരം FixedString& other) : FixedString(other.data()) {}

    FixedString(std::string_view init) {
        size_t size = init.size();
        എങ്കിൽ (size > 0 && reserve(size + 1)) {
            uprv_memcpy(ptr, init.data(), size);
            ptr[size] = '\0';
        }
    }

    FixedString& operator=(സ്ഥിരം FixedString& other) {
        *this = other.data();
        തിരികെ *this;
    }

    FixedString& operator=(std::string_view init) {
        എങ്കിൽ (init.empty()) {
            operator delete[](ptr);
            ptr = nullptr;
        } അല്ലെങ്കിൽ {
            size_t size = init.size();
            എങ്കിൽ (reserve(size + 1)) {
                uprv_memcpy(ptr, init.data(), size);
                ptr[size] = '\0';
            }
        }
        തിരികെ *this;
    }

    FixedString(FixedString&& other) noexcept : ptr(std::exchange(other.ptr, nullptr)) {}

    FixedString& operator=(FixedString&& other) noexcept {
        operator delete[](ptr);
        ptr = other.ptr;
        other.ptr = nullptr;
        തിരികെ *this;
    }

    ശൂന്യം clear() {
        operator delete[](ptr);
        ptr = nullptr;
    }

    സ്ഥിരം അക്ഷരം* data() സ്ഥിരം {
        തിരികെ isEmpty() ? "" : ptr;
    }

    അക്ഷരം* getAlias() {
        തിരികെ ptr;
    }

    bool isEmpty() സ്ഥിരം {
        തിരികെ ptr == nullptr;
    }

    /** Allocate storage for a new string, without initializing it. */
    bool reserve(size_t size) {
        operator delete[](ptr);
        ptr = static_cast<അക്ഷരം*>(operator new[](size));
        തിരികെ ptr != nullptr;
    }

സ്വകാര്യ:
    അക്ഷരം* ptr = nullptr;
};

U_COMMON_API ശൂന്യം copyInvariantChars(സ്ഥിരം UnicodeString& src, FixedString& dst, UErrorCode& status);

U_NAMESPACE_END

#endif
