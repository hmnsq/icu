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
  সর্বজনীন:
    /**
     * Convert from the C type to the C++ type (const version).
     */
    স্থির ধ্রুবক CPPType* validate(ধ্রুবক CType* প্রবেশ, UErrorCode& status);

    /**
     * Convert from the C type to the C++ type (non-const version).
     */
    স্থির CPPType* validate(CType* প্রবেশ, UErrorCode& status);

    /**
     * Convert from the C++ type to the C type (const version).
     */
    ধ্রুবক CType* exportConstForC() ধ্রুবক;

    /**
     * Convert from the C++ type to the C type (non-const version).
     */
    CType* exportForC();

    /**
     * Invalidates the object.
     */
    ~IcuCApiHelper();

  ব্যক্তিগত:
    /**
     * While the object is valid, fMagic equals kMagic.
     */
    int32_t fMagic = kMagic;
};


template<typename CType, typename CPPType, int32_t kMagic>
ধ্রুবক CPPType*
IcuCApiHelper<CType, CPPType, kMagic>::validate(ধ্রুবক CType* প্রবেশ, UErrorCode& status) {
    যদি (U_FAILURE(status)) {
        ফেরত nullptr;
    }
    যদি (প্রবেশ == nullptr) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }
    auto* impl = reinterpret_cast<ধ্রুবক CPPType*>(প্রবেশ);
    যদি (static_cast<ধ্রুবক IcuCApiHelper<CType, CPPType, kMagic>*>(impl)->fMagic != kMagic) {
        status = U_INVALID_FORMAT_ERROR;
        ফেরত nullptr;
    }
    ফেরত impl;
}

template<typename CType, typename CPPType, int32_t kMagic>
CPPType*
IcuCApiHelper<CType, CPPType, kMagic>::validate(CType* প্রবেশ, UErrorCode& status) {
    auto* const_প্রবেশ = static_cast<ধ্রুবক CType*>(প্রবেশ);
    auto* validated = validate(const_প্রবেশ, status);
    ফেরত const_cast<CPPType*>(validated);
}

template<typename CType, typename CPPType, int32_t kMagic>
ধ্রুবক CType*
IcuCApiHelper<CType, CPPType, kMagic>::exportConstForC() ধ্রুবক {
    ফেরত reinterpret_cast<ধ্রুবক CType*>(static_cast<ধ্রুবক CPPType*>(this));
}

template<typename CType, typename CPPType, int32_t kMagic>
CType*
IcuCApiHelper<CType, CPPType, kMagic>::exportForC() {
    ফেরত reinterpret_cast<CType*>(static_cast<CPPType*>(this));
}

template<typename CType, typename CPPType, int32_t kMagic>
IcuCApiHelper<CType, CPPType, kMagic>::~IcuCApiHelper() {
    // head off application errors by preventing use of of deleted objects.
    fMagic = 0;
}


U_NAMESPACE_END

#endif // __CAPI_HELPER_H__
