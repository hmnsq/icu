// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ifndef __CAPI_HELPER_H__
#define __CAPI_HELPER_H__

#অন্তর্ভাব "unicode/utypes.h"

U_NAMESPACE_BEGIN

/**
 * An internal helper class to help convert between C and C++ APIs.
 */
template<typename CType, typename CPPType, int32_t kMagic>
class IcuCApiHelper {
  সার্বজনিক:
    /**
     * Convert from the C type to the C++ type (const version).
     */
    স্থির ধ্রুব CPPType* validate(ধ্রুব CType* প্রবেশ, UErrorCode& status);

    /**
     * Convert from the C type to the C++ type (non-const version).
     */
    স্থির CPPType* validate(CType* প্রবেশ, UErrorCode& status);

    /**
     * Convert from the C++ type to the C type (const version).
     */
    ধ্রুব CType* exportConstForC() ধ্রুব;

    /**
     * Convert from the C++ type to the C type (non-const version).
     */
    CType* exportForC();

    /**
     * Invalidates the object.
     */
    ~IcuCApiHelper();

  গুপ্ত:
    /**
     * While the object is valid, fMagic equals kMagic.
     */
    int32_t fMagic = kMagic;
};


template<typename CType, typename CPPType, int32_t kMagic>
ধ্রুব CPPType*
IcuCApiHelper<CType, CPPType, kMagic>::validate(ধ্রুব CType* প্রবেশ, UErrorCode& status) {
    যদি (U_FAILURE(status)) {
        প্রত্যাগম nullptr;
    }
    যদি (প্রবেশ == nullptr) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম nullptr;
    }
    auto* impl = reinterpret_cast<ধ্রুব CPPType*>(প্রবেশ);
    যদি (static_cast<ধ্রুব IcuCApiHelper<CType, CPPType, kMagic>*>(impl)->fMagic != kMagic) {
        status = U_INVALID_FORMAT_ERROR;
        প্রত্যাগম nullptr;
    }
    প্রত্যাগম impl;
}

template<typename CType, typename CPPType, int32_t kMagic>
CPPType*
IcuCApiHelper<CType, CPPType, kMagic>::validate(CType* প্রবেশ, UErrorCode& status) {
    auto* const_প্রবেশ = static_cast<ধ্রুব CType*>(প্রবেশ);
    auto* validated = validate(const_প্রবেশ, status);
    প্রত্যাগম const_cast<CPPType*>(validated);
}

template<typename CType, typename CPPType, int32_t kMagic>
ধ্রুব CType*
IcuCApiHelper<CType, CPPType, kMagic>::exportConstForC() ধ্রুব {
    প্রত্যাগম reinterpret_cast<ধ্রুব CType*>(static_cast<ধ্রুব CPPType*>(this));
}

template<typename CType, typename CPPType, int32_t kMagic>
CType*
IcuCApiHelper<CType, CPPType, kMagic>::exportForC() {
    প্রত্যাগম reinterpret_cast<CType*>(static_cast<CPPType*>(this));
}

template<typename CType, typename CPPType, int32_t kMagic>
IcuCApiHelper<CType, CPPType, kMagic>::~IcuCApiHelper() {
    // head off application errors by preventing use of of deleted objects.
    fMagic = 0;
}


U_NAMESPACE_END

#endif // __CAPI_HELPER_H__
