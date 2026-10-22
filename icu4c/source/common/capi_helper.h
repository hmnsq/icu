// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#جے_بنا_تعریف __CAPI_HELPER_H__
#تعریف_کرو __CAPI_HELPER_H__

#ضمن "unicode/utypes.h"

U_NAMESPACE_BEGIN

/**
 * An internal helper class to help convert between C and C++ APIs.
 */
سانچا<typename CType, typename CPPType, int32_t kMagic>
class IcuCApiHelper {
  عام:
    /**
     * Convert from the C type to the C++ type (const version).
     */
    ساكن ثابت CPPType* validate(ثابت CType* داخلہ, UError_کوڈ& حالت);

    /**
     * Convert from the C type to the C++ type (non-const version).
     */
    ساكن CPPType* validate(CType* داخلہ, UError_کوڈ& حالت);

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


سانچا<typename CType, typename CPPType, int32_t kMagic>
ثابت CPPType*
IcuCApiHelper<CType, CPPType, kMagic>::validate(ثابت CType* داخلہ, UError_کوڈ& حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع nullptr;
    }
    إذا (داخلہ == nullptr) {
        حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع nullptr;
    }
    خودکار* impl = reinterpret_cast<ثابت CPPType*>(داخلہ);
    إذا (static_cast<ثابت IcuCApiHelper<CType, CPPType, kMagic>*>(impl)->fMagic != kMagic) {
        حالت = U_INVALID_FORMAT_ERROR;
        أرجع nullptr;
    }
    أرجع impl;
}

سانچا<typename CType, typename CPPType, int32_t kMagic>
CPPType*
IcuCApiHelper<CType, CPPType, kMagic>::validate(CType* داخلہ, UError_کوڈ& حالت) {
    خودکار* const_داخلہ = static_cast<ثابت CType*>(داخلہ);
    خودکار* validated = validate(const_داخلہ, حالت);
    أرجع const_cast<CPPType*>(validated);
}

سانچا<typename CType, typename CPPType, int32_t kMagic>
ثابت CType*
IcuCApiHelper<CType, CPPType, kMagic>::exportConstForC() ثابت {
    أرجع reinterpret_cast<ثابت CType*>(static_cast<ثابت CPPType*>(this));
}

سانچا<typename CType, typename CPPType, int32_t kMagic>
CType*
IcuCApiHelper<CType, CPPType, kMagic>::exportForC() {
    أرجع reinterpret_cast<CType*>(static_cast<CPPType*>(this));
}

سانچا<typename CType, typename CPPType, int32_t kMagic>
IcuCApiHelper<CType, CPPType, kMagic>::~IcuCApiHelper() {
    // head off application errors by preventing use of of deleted objects.
    fMagic = 0;
}


U_NAMESPACE_سرا

#جے_انت // __CAPI_HELPER_H__
