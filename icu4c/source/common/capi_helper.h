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
    स्थिर ध्रुव CPPType* validate(ध्रुव CType* निवेश, UErrorCode& status);

    /**
     * Convert from the C type to the C++ type (non-const version).
     */
    स्थिर CPPType* validate(CType* निवेश, UErrorCode& status);

    /**
     * Convert from the C++ type to the C type (const version).
     */
    ध्रुव CType* exportConstForC() ध्रुव;

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
ध्रुव CPPType*
IcuCApiHelper<CType, CPPType, kMagic>::validate(ध्रुव CType* निवेश, UErrorCode& status) {
    यदि (U_FAILURE(status)) {
        प्रत्यागम nullptr;
    }
    यदि (निवेश == nullptr) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम nullptr;
    }
    स्वतः* impl = reinterpret_cast<ध्रुव CPPType*>(निवेश);
    यदि (static_cast<ध्रुव IcuCApiHelper<CType, CPPType, kMagic>*>(impl)->fMagic != kMagic) {
        status = U_INVALID_FORMAT_ERROR;
        प्रत्यागम nullptr;
    }
    प्रत्यागम impl;
}

ढाँचा<typename CType, typename CPPType, int32_t kMagic>
CPPType*
IcuCApiHelper<CType, CPPType, kMagic>::validate(CType* निवेश, UErrorCode& status) {
    स्वतः* const_निवेश = static_cast<ध्रुव CType*>(निवेश);
    स्वतः* validated = validate(const_निवेश, status);
    प्रत्यागम const_cast<CPPType*>(validated);
}

ढाँचा<typename CType, typename CPPType, int32_t kMagic>
ध्रुव CType*
IcuCApiHelper<CType, CPPType, kMagic>::exportConstForC() ध्रुव {
    प्रत्यागम reinterpret_cast<ध्रुव CType*>(static_cast<ध्रुव CPPType*>(this));
}

ढाँचा<typename CType, typename CPPType, int32_t kMagic>
CType*
IcuCApiHelper<CType, CPPType, kMagic>::exportForC() {
    प्रत्यागम reinterpret_cast<CType*>(static_cast<CPPType*>(this));
}

ढाँचा<typename CType, typename CPPType, int32_t kMagic>
IcuCApiHelper<CType, CPPType, kMagic>::~IcuCApiHelper() {
    // head off application errors by preventing use of of deleted objects.
    fMagic = 0;
}


U_NAMESPACE_END

#endif // __CAPI_HELPER_H__
