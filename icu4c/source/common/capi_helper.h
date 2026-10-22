// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ifndef __CAPI_HELPER_H__
#define __CAPI_HELPER_H__

#ضمن "unicode/utypes.h"

U_NAMESPACE_BEGIN

/**
 * An internal helper class to help convert between C and C++ APIs.
 */
template<typename CType, typename CPPType, int32_t kMagic>
class IcuCApiHelper {
  عام:
    /**
     * Convert from the C type to the C++ type (const version).
     */
    ساكن ثابت CPPType* validate(ثابت CType* input, UErrorCode& status);

    /**
     * Convert from the C type to the C++ type (non-const version).
     */
    ساكن CPPType* validate(CType* input, UErrorCode& status);

    /**
     * Convert from the C++ type to the C type (const version).
     */
    ثابت CType* exportConstForC() ثابت;

    /**
     * Convert from the C++ type to the C type (non-const version).
     */
    CType* exportForC();

    /**
     * Invalidates the object.
     */
    ~IcuCApiHelper();

  خاص:
    /**
     * While the object is valid, fMagic equals kMagic.
     */
    int32_t fMagic = kMagic;
};


template<typename CType, typename CPPType, int32_t kMagic>
ثابت CPPType*
IcuCApiHelper<CType, CPPType, kMagic>::validate(ثابت CType* input, UErrorCode& status) {
    𐤀𐤌 (U_FAILURE(status)) {
        أرجع nullptr;
    }
    𐤀𐤌 (input == nullptr) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع nullptr;
    }
    auto* impl = reinterpret_cast<ثابت CPPType*>(input);
    𐤀𐤌 (static_cast<ثابت IcuCApiHelper<CType, CPPType, kMagic>*>(impl)->fMagic != kMagic) {
        status = U_INVALID_FORMAT_ERROR;
        أرجع nullptr;
    }
    أرجع impl;
}

template<typename CType, typename CPPType, int32_t kMagic>
CPPType*
IcuCApiHelper<CType, CPPType, kMagic>::validate(CType* input, UErrorCode& status) {
    auto* constInput = static_cast<ثابت CType*>(input);
    auto* validated = validate(constInput, status);
    أرجع const_cast<CPPType*>(validated);
}

template<typename CType, typename CPPType, int32_t kMagic>
ثابت CType*
IcuCApiHelper<CType, CPPType, kMagic>::exportConstForC() ثابت {
    أرجع reinterpret_cast<ثابت CType*>(static_cast<ثابت CPPType*>(this));
}

template<typename CType, typename CPPType, int32_t kMagic>
CType*
IcuCApiHelper<CType, CPPType, kMagic>::exportForC() {
    أرجع reinterpret_cast<CType*>(static_cast<CPPType*>(this));
}

template<typename CType, typename CPPType, int32_t kMagic>
IcuCApiHelper<CType, CPPType, kMagic>::~IcuCApiHelper() {
    // head off application errors by preventing use of of deleted objects.
    fMagic = 0;
}


U_NAMESPACE_END

#endif // __CAPI_HELPER_H__
