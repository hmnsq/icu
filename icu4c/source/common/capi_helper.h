// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#جيڪڏهن_وصف_نه_ٿيل __CAPI_HELPER_H__
#وصف_ڪريو __CAPI_HELPER_H__

#ضمن "unicode/utypes.h"

U_NAMESPACE_BEGIN

/**
 * An internal helper class to help convert between C and C++ APIs.
 */
سانچو<typename CType, typename CPPType, int32_t kMagic>
class IcuCApiHelper {
  عام:
    /**
     * Convert from the C type to the C++ type (const version).
     */
    ساكن ثابت CPPType* validate(ثابت CType* داخلا, UError_ڪوڊ& حالت);

    /**
     * Convert from the C type to the C++ type (non-const version).
     */
    ساكن CPPType* validate(CType* داخلا, UError_ڪوڊ& حالت);

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


سانچو<typename CType, typename CPPType, int32_t kMagic>
ثابت CPPType*
IcuCApiHelper<CType, CPPType, kMagic>::validate(ثابت CType* داخلا, UError_ڪوڊ& حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع nullptr;
    }
    إذا (داخلا == nullptr) {
        حالت = U_ILLEGAL_دليل_ERROR;
        أرجع nullptr;
    }
    خودڪار* impl = reinterpret_cast<ثابت CPPType*>(داخلا);
    إذا (static_cast<ثابت IcuCApiHelper<CType, CPPType, kMagic>*>(impl)->fMagic != kMagic) {
        حالت = U_INVALID_FORMAT_ERROR;
        أرجع nullptr;
    }
    أرجع impl;
}

سانچو<typename CType, typename CPPType, int32_t kMagic>
CPPType*
IcuCApiHelper<CType, CPPType, kMagic>::validate(CType* داخلا, UError_ڪوڊ& حالت) {
    خودڪار* const_داخلا = static_cast<ثابت CType*>(داخلا);
    خودڪار* validated = validate(const_داخلا, حالت);
    أرجع const_cast<CPPType*>(validated);
}

سانچو<typename CType, typename CPPType, int32_t kMagic>
ثابت CType*
IcuCApiHelper<CType, CPPType, kMagic>::exportConstForC() ثابت {
    أرجع reinterpret_cast<ثابت CType*>(static_cast<ثابت CPPType*>(this));
}

سانچو<typename CType, typename CPPType, int32_t kMagic>
CType*
IcuCApiHelper<CType, CPPType, kMagic>::exportForC() {
    أرجع reinterpret_cast<CType*>(static_cast<CPPType*>(this));
}

سانچو<typename CType, typename CPPType, int32_t kMagic>
IcuCApiHelper<CType, CPPType, kMagic>::~IcuCApiHelper() {
    // head off application errors by preventing use of of deleted objects.
    fMagic = 0;
}


U_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو // __CAPI_HELPER_H__
