// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ifndef __CAPI_HELPER_H__
#define __CAPI_HELPER_H__

#include "unicode/utypes.h"

U_NAMESPACE_BEGIN

/**
 * An internal helper class to help convert between C and C++ APIs.
 */
template<typename CType, typename CPPType, int32_t kMagic>
class IcuCApiHelper {
  public:
    /**
     * Convert from the C type to the C++ type (const version).
     */
    static const CPPType* validate(const CType* input, UErrorCode& status);

    /**
     * Convert from the C type to the C++ type (non-const version).
     */
    static CPPType* validate(CType* input, UErrorCode& status);

    /**
     * Convert from the C++ type to the C type (const version).
     */
    const CType* exportConstForC() const;

    /**
     * Convert from the C++ type to the C type (non-const version).
     */
    CType* exportForC();

    /**
     * Invalidates the object.
     */
    ~IcuCApiHelper();

  private:
    /**
     * While the object is valid, fMagic equals kMagic.
     */
    int32_t fMagic = kMagic;
};


template<typename CType, typename CPPType, int32_t kMagic>
const CPPType*
IcuCApiHelper<CType, CPPType, kMagic>::validate(const CType* input, UErrorCode& status) {
    ပေႃး (U_FAILURE(status)) {
        ပွၵ်ႈ nullptr;
    }
    ပေႃး (input == nullptr) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ပွၵ်ႈ nullptr;
    }
    auto* impl = reinterpret_cast<const CPPType*>(input);
    ပေႃး (static_cast<const IcuCApiHelper<CType, CPPType, kMagic>*>(impl)->fMagic != kMagic) {
        status = U_INVALID_FORMAT_ERROR;
        ပွၵ်ႈ nullptr;
    }
    ပွၵ်ႈ impl;
}

template<typename CType, typename CPPType, int32_t kMagic>
CPPType*
IcuCApiHelper<CType, CPPType, kMagic>::validate(CType* input, UErrorCode& status) {
    auto* constInput = static_cast<const CType*>(input);
    auto* validated = validate(constInput, status);
    ပွၵ်ႈ const_cast<CPPType*>(validated);
}

template<typename CType, typename CPPType, int32_t kMagic>
const CType*
IcuCApiHelper<CType, CPPType, kMagic>::exportConstForC() const {
    ပွၵ်ႈ reinterpret_cast<const CType*>(static_cast<const CPPType*>(this));
}

template<typename CType, typename CPPType, int32_t kMagic>
CType*
IcuCApiHelper<CType, CPPType, kMagic>::exportForC() {
    ပွၵ်ႈ reinterpret_cast<CType*>(static_cast<CPPType*>(this));
}

template<typename CType, typename CPPType, int32_t kMagic>
IcuCApiHelper<CType, CPPType, kMagic>::~IcuCApiHelper() {
    // head off application errors by preventing use of of deleted objects.
    fMagic = 0;
}


U_NAMESPACE_END

#endif // __CAPI_HELPER_H__
