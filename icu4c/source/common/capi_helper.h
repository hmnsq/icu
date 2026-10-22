// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __CAPI_HELPER_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __CAPI_HELPER_H__

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

U_NAMESPACE_BEGIN

/**
 * An internal helper class to help convert between C and C++ APIs.
 */
ਸਾਂਚਾ<typename CType, typename CPPType, int32_t kMagic>
class IcuCApiHelper {
  ਜਨਤਕ:
    /**
     * Convert from the C type to the C++ type (const version).
     */
    ਸਥਿਰ ਅਚਲ CPPType* validate(ਅਚਲ CType* ਨਿਵੇਸ਼, UError_ਕੋਡ& ਸਥਿਤੀ);

    /**
     * Convert from the C type to the C++ type (non-const version).
     */
    ਸਥਿਰ CPPType* validate(CType* ਨਿਵੇਸ਼, UError_ਕੋਡ& ਸਥਿਤੀ);

    /**
     * Convert from the C++ type to the C type (const version).
     */
    ਅਚਲ CType* exportConstForC() ਅਚਲ;

    /**
     * Convert from the C++ type to the C type (non-const version).
     */
    CType* exportForC();

    /**
     * Invalidates the object.
     */
    ~IcuCApiHelper();

  ਨਿਜੀ:
    /**
     * While the object is valid, fMagic equals kMagic.
     */
    int32_t fMagic = kMagic;
};


ਸਾਂਚਾ<typename CType, typename CPPType, int32_t kMagic>
ਅਚਲ CPPType*
IcuCApiHelper<CType, CPPType, kMagic>::validate(ਅਚਲ CType* ਨਿਵੇਸ਼, UError_ਕੋਡ& ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ nullptr;
    }
    ਜੇ (ਨਿਵੇਸ਼ == nullptr) {
        ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ nullptr;
    }
    ਸਵੈਚਾਲਿਤ* impl = reinterpret_cast<ਅਚਲ CPPType*>(ਨਿਵੇਸ਼);
    ਜੇ (static_cast<ਅਚਲ IcuCApiHelper<CType, CPPType, kMagic>*>(impl)->fMagic != kMagic) {
        ਸਥਿਤੀ = U_INVALID_FORMAT_ERROR;
        ਵਾਪਸ nullptr;
    }
    ਵਾਪਸ impl;
}

ਸਾਂਚਾ<typename CType, typename CPPType, int32_t kMagic>
CPPType*
IcuCApiHelper<CType, CPPType, kMagic>::validate(CType* ਨਿਵੇਸ਼, UError_ਕੋਡ& ਸਥਿਤੀ) {
    ਸਵੈਚਾਲਿਤ* const_ਨਿਵੇਸ਼ = static_cast<ਅਚਲ CType*>(ਨਿਵੇਸ਼);
    ਸਵੈਚਾਲਿਤ* validated = validate(const_ਨਿਵੇਸ਼, ਸਥਿਤੀ);
    ਵਾਪਸ const_cast<CPPType*>(validated);
}

ਸਾਂਚਾ<typename CType, typename CPPType, int32_t kMagic>
ਅਚਲ CType*
IcuCApiHelper<CType, CPPType, kMagic>::exportConstForC() ਅਚਲ {
    ਵਾਪਸ reinterpret_cast<ਅਚਲ CType*>(static_cast<ਅਚਲ CPPType*>(this));
}

ਸਾਂਚਾ<typename CType, typename CPPType, int32_t kMagic>
CType*
IcuCApiHelper<CType, CPPType, kMagic>::exportForC() {
    ਵਾਪਸ reinterpret_cast<CType*>(static_cast<CPPType*>(this));
}

ਸਾਂਚਾ<typename CType, typename CPPType, int32_t kMagic>
IcuCApiHelper<CType, CPPType, kMagic>::~IcuCApiHelper() {
    // head off application errors by preventing use of of deleted objects.
    fMagic = 0;
}


U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ // __CAPI_HELPER_H__
