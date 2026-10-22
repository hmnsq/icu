// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ifndef __CAPI_HELPER_H__
#define __CAPI_HELPER_H__

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

U_NAMESPACE_BEGIN

/**
 * An internal helper class to help convert between C and C++ APIs.
 */
template<typename CType, typename CPPType, int32_t kMagic>
class IcuCApiHelper {
  ସର୍ବସାଧାରଣ:
    /**
     * Convert from the C type to the C++ type (const version).
     */
    ସ୍ଥିର ଅଚଳ CPPType* validate(ଅଚଳ CType* ନିବେଶ, UErrorCode& status);

    /**
     * Convert from the C type to the C++ type (non-const version).
     */
    ସ୍ଥିର CPPType* validate(CType* ନିବେଶ, UErrorCode& status);

    /**
     * Convert from the C++ type to the C type (const version).
     */
    ଅଚଳ CType* exportConstForC() ଅଚଳ;

    /**
     * Convert from the C++ type to the C type (non-const version).
     */
    CType* exportForC();

    /**
     * Invalidates the object.
     */
    ~IcuCApiHelper();

  ବ୍ୟକ୍ତିଗତ:
    /**
     * While the object is valid, fMagic equals kMagic.
     */
    int32_t fMagic = kMagic;
};


template<typename CType, typename CPPType, int32_t kMagic>
ଅଚଳ CPPType*
IcuCApiHelper<CType, CPPType, kMagic>::validate(ଅଚଳ CType* ନିବେଶ, UErrorCode& status) {
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଯଦି (ନିବେଶ == nullptr) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    auto* impl = reinterpret_cast<ଅଚଳ CPPType*>(ନିବେଶ);
    ଯଦି (static_cast<ଅଚଳ IcuCApiHelper<CType, CPPType, kMagic>*>(impl)->fMagic != kMagic) {
        status = U_INVALID_FORMAT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    ଫେରନ୍ତୁ impl;
}

template<typename CType, typename CPPType, int32_t kMagic>
CPPType*
IcuCApiHelper<CType, CPPType, kMagic>::validate(CType* ନିବେଶ, UErrorCode& status) {
    auto* const_ନିବେଶ = static_cast<ଅଚଳ CType*>(ନିବେଶ);
    auto* validated = validate(const_ନିବେଶ, status);
    ଫେରନ୍ତୁ const_cast<CPPType*>(validated);
}

template<typename CType, typename CPPType, int32_t kMagic>
ଅଚଳ CType*
IcuCApiHelper<CType, CPPType, kMagic>::exportConstForC() ଅଚଳ {
    ଫେରନ୍ତୁ reinterpret_cast<ଅଚଳ CType*>(static_cast<ଅଚଳ CPPType*>(this));
}

template<typename CType, typename CPPType, int32_t kMagic>
CType*
IcuCApiHelper<CType, CPPType, kMagic>::exportForC() {
    ଫେରନ୍ତୁ reinterpret_cast<CType*>(static_cast<CPPType*>(this));
}

template<typename CType, typename CPPType, int32_t kMagic>
IcuCApiHelper<CType, CPPType, kMagic>::~IcuCApiHelper() {
    // head off application errors by preventing use of of deleted objects.
    fMagic = 0;
}


U_NAMESPACE_END

#endif // __CAPI_HELPER_H__
