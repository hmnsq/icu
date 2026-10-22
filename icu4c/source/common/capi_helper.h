// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ifndef __CAPI_HELPER_H__
#define __CAPI_HELPER_H__

#അന്തര്ഭാവ "unicode/utypes.h"

U_NAMESPACE_BEGIN

/**
 * An internal helper class to help convert between C and C++ APIs.
 */
template<typename CType, typename CPPType, int32_t kMagic>
class IcuCApiHelper {
  പൊതു:
    /**
     * Convert from the C type to the C++ type (const version).
     */
    സ്ഥിതം സ്ഥിരം CPPType* validate(സ്ഥിരം CType* നിവേശം, UErrorCode& status);

    /**
     * Convert from the C type to the C++ type (non-const version).
     */
    സ്ഥിതം CPPType* validate(CType* നിവേശം, UErrorCode& status);

    /**
     * Convert from the C++ type to the C type (const version).
     */
    സ്ഥിരം CType* exportConstForC() സ്ഥിരം;

    /**
     * Convert from the C++ type to the C type (non-const version).
     */
    CType* exportForC();

    /**
     * Invalidates the object.
     */
    ~IcuCApiHelper();

  സ്വകാര്യ:
    /**
     * While the object is valid, fMagic equals kMagic.
     */
    int32_t fMagic = kMagic;
};


template<typename CType, typename CPPType, int32_t kMagic>
സ്ഥിരം CPPType*
IcuCApiHelper<CType, CPPType, kMagic>::validate(സ്ഥിരം CType* നിവേശം, UErrorCode& status) {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ nullptr;
    }
    എങ്കിൽ (നിവേശം == nullptr) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ nullptr;
    }
    auto* impl = reinterpret_cast<സ്ഥിരം CPPType*>(നിവേശം);
    എങ്കിൽ (static_cast<സ്ഥിരം IcuCApiHelper<CType, CPPType, kMagic>*>(impl)->fMagic != kMagic) {
        status = U_INVALID_FORMAT_ERROR;
        തിരികെ nullptr;
    }
    തിരികെ impl;
}

template<typename CType, typename CPPType, int32_t kMagic>
CPPType*
IcuCApiHelper<CType, CPPType, kMagic>::validate(CType* നിവേശം, UErrorCode& status) {
    auto* const_നിവേശം = static_cast<സ്ഥിരം CType*>(നിവേശം);
    auto* validated = validate(const_നിവേശം, status);
    തിരികെ const_cast<CPPType*>(validated);
}

template<typename CType, typename CPPType, int32_t kMagic>
സ്ഥിരം CType*
IcuCApiHelper<CType, CPPType, kMagic>::exportConstForC() സ്ഥിരം {
    തിരികെ reinterpret_cast<സ്ഥിരം CType*>(static_cast<സ്ഥിരം CPPType*>(this));
}

template<typename CType, typename CPPType, int32_t kMagic>
CType*
IcuCApiHelper<CType, CPPType, kMagic>::exportForC() {
    തിരികെ reinterpret_cast<CType*>(static_cast<CPPType*>(this));
}

template<typename CType, typename CPPType, int32_t kMagic>
IcuCApiHelper<CType, CPPType, kMagic>::~IcuCApiHelper() {
    // head off application errors by preventing use of of deleted objects.
    fMagic = 0;
}


U_NAMESPACE_END

#endif // __CAPI_HELPER_H__
