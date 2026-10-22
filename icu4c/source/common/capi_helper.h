// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#که_نه_تعریف_شوی __CAPI_HELPER_H__
#تعریف_کړه __CAPI_HELPER_H__

#ضمن "unicode/utypes.h"

U_NAMESPACE_BEGIN

/**
 * An internal helper class to help convert between C and C++ APIs.
 */
کينډۍ<typename CType, typename CPPType, int32_t kMagic>
class IcuCApiHelper {
  عام:
    /**
     * Convert from the C type to the C++ type (const version).
     */
    ساكن ثابت CPPType* validate(ثابت CType* ننوت, UError_کوډ& حالت);

    /**
     * Convert from the C type to the C++ type (non-const version).
     */
    ساكن CPPType* validate(CType* ننوت, UError_کوډ& حالت);

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


کينډۍ<typename CType, typename CPPType, int32_t kMagic>
ثابت CPPType*
IcuCApiHelper<CType, CPPType, kMagic>::validate(ثابت CType* ننوت, UError_کوډ& حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع nullptr;
    }
    إذا (ننوت == nullptr) {
        حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع nullptr;
    }
    خپلکاری* impl = reinterpret_cast<ثابت CPPType*>(ننوت);
    إذا (static_cast<ثابت IcuCApiHelper<CType, CPPType, kMagic>*>(impl)->fMagic != kMagic) {
        حالت = U_INVALID_FORMAT_ERROR;
        أرجع nullptr;
    }
    أرجع impl;
}

کينډۍ<typename CType, typename CPPType, int32_t kMagic>
CPPType*
IcuCApiHelper<CType, CPPType, kMagic>::validate(CType* ننوت, UError_کوډ& حالت) {
    خپلکاری* const_ننوت = static_cast<ثابت CType*>(ننوت);
    خپلکاری* validated = validate(const_ننوت, حالت);
    أرجع const_cast<CPPType*>(validated);
}

کينډۍ<typename CType, typename CPPType, int32_t kMagic>
ثابت CType*
IcuCApiHelper<CType, CPPType, kMagic>::exportConstForC() ثابت {
    أرجع reinterpret_cast<ثابت CType*>(static_cast<ثابت CPPType*>(this));
}

کينډۍ<typename CType, typename CPPType, int32_t kMagic>
CType*
IcuCApiHelper<CType, CPPType, kMagic>::exportForC() {
    أرجع reinterpret_cast<CType*>(static_cast<CPPType*>(this));
}

کينډۍ<typename CType, typename CPPType, int32_t kMagic>
IcuCApiHelper<CType, CPPType, kMagic>::~IcuCApiHelper() {
    // head off application errors by preventing use of of deleted objects.
    fMagic = 0;
}


U_NAMESPACE_پای_ټکی

#د_که_پای // __CAPI_HELPER_H__
