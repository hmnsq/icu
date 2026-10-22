// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#اگر_غیر_تعریف_شدہ __CAPI_HELPER_H__
#تعریف_کرو __CAPI_HELPER_H__

#شامل "unicode/utypes.h"

U_NAMESPACE_BEGIN

/**
 * An internal helper class to help convert between C and C++ APIs.
 */
سانچہ<typename CType, typename CPPType, int32_t kMagic>
class IcuCApiHelper {
  عوامی:
    /**
     * Convert from the C type to the C++ type (const version).
     */
    ساکن ثابت CPPType* validate(ثابت CType* داخلہ, UError_رمز& حالت_2);

    /**
     * Convert from the C type to the C++ type (non-const version).
     */
    ساکن CPPType* validate(CType* داخلہ, UError_رمز& حالت_2);

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

  نجی:
    /**
     * While the object is valid, fMagic equals kMagic.
     */
    int32_t fMagic = kMagic;
};


سانچہ<typename CType, typename CPPType, int32_t kMagic>
ثابت CPPType*
IcuCApiHelper<CType, CPPType, kMagic>::validate(ثابت CType* داخلہ, UError_رمز& حالت_2) {
    اگر (U_FAILURE(حالت_2)) {
        واپس nullptr;
    }
    اگر (داخلہ == nullptr) {
        حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس nullptr;
    }
    خودکار* impl = reinterpret_cast<ثابت CPPType*>(داخلہ);
    اگر (static_cast<ثابت IcuCApiHelper<CType, CPPType, kMagic>*>(impl)->fMagic != kMagic) {
        حالت_2 = U_INVALID_FORMAT_ERROR;
        واپس nullptr;
    }
    واپس impl;
}

سانچہ<typename CType, typename CPPType, int32_t kMagic>
CPPType*
IcuCApiHelper<CType, CPPType, kMagic>::validate(CType* داخلہ, UError_رمز& حالت_2) {
    خودکار* const_داخلہ = static_cast<ثابت CType*>(داخلہ);
    خودکار* validated = validate(const_داخلہ, حالت_2);
    واپس const_cast<CPPType*>(validated);
}

سانچہ<typename CType, typename CPPType, int32_t kMagic>
ثابت CType*
IcuCApiHelper<CType, CPPType, kMagic>::exportConstForC() ثابت {
    واپس reinterpret_cast<ثابت CType*>(static_cast<ثابت CPPType*>(this));
}

سانچہ<typename CType, typename CPPType, int32_t kMagic>
CType*
IcuCApiHelper<CType, CPPType, kMagic>::exportForC() {
    واپس reinterpret_cast<CType*>(static_cast<CPPType*>(this));
}

سانچہ<typename CType, typename CPPType, int32_t kMagic>
IcuCApiHelper<CType, CPPType, kMagic>::~IcuCApiHelper() {
    // head off application errors by preventing use of of deleted objects.
    fMagic = 0;
}


U_NAMESPACE_انتہا

#اگر_ختم // __CAPI_HELPER_H__
