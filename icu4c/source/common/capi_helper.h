// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#اگر_تعریف_نشده __CAPI_HELPER_H__
#تعریف_کن __CAPI_HELPER_H__

#ضمن "unicode/utypes.h"

U_NAMESPACE_BEGIN

/**
 * An internal helper class to help convert between C and C++ APIs.
 */
قالب<typename CType, typename CPPType, int32_t kMagic>
class IcuCApiHelper {
  عام:
    /**
     * Convert from the C type to the C++ type (const version).
     */
    ساكن ثابت CPPType* validate(ثابت CType* درون_داد, UError_کد& وضعیت);

    /**
     * Convert from the C type to the C++ type (non-const version).
     */
    ساكن CPPType* validate(CType* درون_داد, UError_کد& وضعیت);

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


قالب<typename CType, typename CPPType, int32_t kMagic>
ثابت CPPType*
IcuCApiHelper<CType, CPPType, kMagic>::validate(ثابت CType* درون_داد, UError_کد& وضعیت) {
    إذا (U_FAILURE(وضعیت)) {
        أرجع nullptr;
    }
    إذا (درون_داد == nullptr) {
        وضعیت = U_ILLEGAL_آرگومان_ERROR;
        أرجع nullptr;
    }
    خودکار* impl = reinterpret_cast<ثابت CPPType*>(درون_داد);
    إذا (static_cast<ثابت IcuCApiHelper<CType, CPPType, kMagic>*>(impl)->fMagic != kMagic) {
        وضعیت = U_INVALID_FORMAT_ERROR;
        أرجع nullptr;
    }
    أرجع impl;
}

قالب<typename CType, typename CPPType, int32_t kMagic>
CPPType*
IcuCApiHelper<CType, CPPType, kMagic>::validate(CType* درون_داد, UError_کد& وضعیت) {
    خودکار* const_درون_داد = static_cast<ثابت CType*>(درون_داد);
    خودکار* validated = validate(const_درون_داد, وضعیت);
    أرجع const_cast<CPPType*>(validated);
}

قالب<typename CType, typename CPPType, int32_t kMagic>
ثابت CType*
IcuCApiHelper<CType, CPPType, kMagic>::exportConstForC() ثابت {
    أرجع reinterpret_cast<ثابت CType*>(static_cast<ثابت CPPType*>(this));
}

قالب<typename CType, typename CPPType, int32_t kMagic>
CType*
IcuCApiHelper<CType, CPPType, kMagic>::exportForC() {
    أرجع reinterpret_cast<CType*>(static_cast<CPPType*>(this));
}

قالب<typename CType, typename CPPType, int32_t kMagic>
IcuCApiHelper<CType, CPPType, kMagic>::~IcuCApiHelper() {
    // head off application errors by preventing use of of deleted objects.
    fMagic = 0;
}


U_NAMESPACE_انتها

#پایان_اگر // __CAPI_HELPER_H__
