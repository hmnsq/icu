// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ifndef __CAPI_HELPER_H__
#define __CAPI_HELPER_H__

#अन्तर्भाव "unicode/utypes.h"

U_NAMESPACE_BEGIN

/**
 * An internal helper class to help convert between C and C++ APIs.
 */
ढाँचा<typename CType, typename CPPType, int32_t kMagic>
class IcuCApiHelper {
  सार्वजनिक:
    /**
     * Convert from the C type to the C++ type (const version).
     */
    स्थिर अचर CPPType* validate(अचर CType* आगत, UErrorCode& status);

    /**
     * Convert from the C type to the C++ type (non-const version).
     */
    स्थिर CPPType* validate(CType* आगत, UErrorCode& status);

    /**
     * Convert from the C++ type to the C type (const version).
     */
    अचर CType* exportConstForC() अचर;

    /**
     * Convert from the C++ type to the C type (non-const version).
     */
    CType* exportForC();

    /**
     * Invalidates the object.
     */
    ~IcuCApiHelper();

  गुप्त:
    /**
     * While the object is valid, fMagic equals kMagic.
     */
    int32_t fMagic = kMagic;
};


ढाँचा<typename CType, typename CPPType, int32_t kMagic>
अचर CPPType*
IcuCApiHelper<CType, CPPType, kMagic>::validate(अचर CType* आगत, UErrorCode& status) {
    यदि (U_FAILURE(status)) {
        फर्काऊ nullptr;
    }
    यदि (आगत == nullptr) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ nullptr;
    }
    स्वतः* impl = reinterpret_cast<अचर CPPType*>(आगत);
    यदि (static_cast<अचर IcuCApiHelper<CType, CPPType, kMagic>*>(impl)->fMagic != kMagic) {
        status = U_INVALID_FORMAT_ERROR;
        फर्काऊ nullptr;
    }
    फर्काऊ impl;
}

ढाँचा<typename CType, typename CPPType, int32_t kMagic>
CPPType*
IcuCApiHelper<CType, CPPType, kMagic>::validate(CType* आगत, UErrorCode& status) {
    स्वतः* const_आगत = static_cast<अचर CType*>(आगत);
    स्वतः* validated = validate(const_आगत, status);
    फर्काऊ const_cast<CPPType*>(validated);
}

ढाँचा<typename CType, typename CPPType, int32_t kMagic>
अचर CType*
IcuCApiHelper<CType, CPPType, kMagic>::exportConstForC() अचर {
    फर्काऊ reinterpret_cast<अचर CType*>(static_cast<अचर CPPType*>(this));
}

ढाँचा<typename CType, typename CPPType, int32_t kMagic>
CType*
IcuCApiHelper<CType, CPPType, kMagic>::exportForC() {
    फर्काऊ reinterpret_cast<CType*>(static_cast<CPPType*>(this));
}

ढाँचा<typename CType, typename CPPType, int32_t kMagic>
IcuCApiHelper<CType, CPPType, kMagic>::~IcuCApiHelper() {
    // head off application errors by preventing use of of deleted objects.
    fMagic = 0;
}


U_NAMESPACE_END

#endif // __CAPI_HELPER_H__
