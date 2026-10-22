// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ifndef __CAPI_HELPER_H__
#define __CAPI_HELPER_H__

#અન્તર્ભાવ "unicode/utypes.h"

U_NAMESPACE_BEGIN

/**
 * An internal helper class to help convert between C and C++ APIs.
 */
template<typename CType, typename CPPType, int32_t kMagic>
class IcuCApiHelper {
  જાહેર:
    /**
     * Convert from the C type to the C++ type (const version).
     */
    સ્થિર અચલ CPPType* validate(અચલ CType* આગત, UErrorCode& status);

    /**
     * Convert from the C type to the C++ type (non-const version).
     */
    સ્થિર CPPType* validate(CType* આગત, UErrorCode& status);

    /**
     * Convert from the C++ type to the C type (const version).
     */
    અચલ CType* exportConstForC() અચલ;

    /**
     * Convert from the C++ type to the C type (non-const version).
     */
    CType* exportForC();

    /**
     * Invalidates the object.
     */
    ~IcuCApiHelper();

  ખાનગી:
    /**
     * While the object is valid, fMagic equals kMagic.
     */
    int32_t fMagic = kMagic;
};


template<typename CType, typename CPPType, int32_t kMagic>
અચલ CPPType*
IcuCApiHelper<CType, CPPType, kMagic>::validate(અચલ CType* આગત, UErrorCode& status) {
    જો (U_FAILURE(status)) {
        પાછા nullptr;
    }
    જો (આગત == nullptr) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા nullptr;
    }
    auto* impl = reinterpret_cast<અચલ CPPType*>(આગત);
    જો (static_cast<અચલ IcuCApiHelper<CType, CPPType, kMagic>*>(impl)->fMagic != kMagic) {
        status = U_INVALID_FORMAT_ERROR;
        પાછા nullptr;
    }
    પાછા impl;
}

template<typename CType, typename CPPType, int32_t kMagic>
CPPType*
IcuCApiHelper<CType, CPPType, kMagic>::validate(CType* આગત, UErrorCode& status) {
    auto* const_આગત = static_cast<અચલ CType*>(આગત);
    auto* validated = validate(const_આગત, status);
    પાછા const_cast<CPPType*>(validated);
}

template<typename CType, typename CPPType, int32_t kMagic>
અચલ CType*
IcuCApiHelper<CType, CPPType, kMagic>::exportConstForC() અચલ {
    પાછા reinterpret_cast<અચલ CType*>(static_cast<અચલ CPPType*>(this));
}

template<typename CType, typename CPPType, int32_t kMagic>
CType*
IcuCApiHelper<CType, CPPType, kMagic>::exportForC() {
    પાછા reinterpret_cast<CType*>(static_cast<CPPType*>(this));
}

template<typename CType, typename CPPType, int32_t kMagic>
IcuCApiHelper<CType, CPPType, kMagic>::~IcuCApiHelper() {
    // head off application errors by preventing use of of deleted objects.
    fMagic = 0;
}


U_NAMESPACE_END

#endif // __CAPI_HELPER_H__
