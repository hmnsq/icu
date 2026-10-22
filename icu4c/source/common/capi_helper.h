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
    स्थिर स्थिरांक CPPType* validate(स्थिरांक CType* आदान, UErrorCode& status);

    /**
     * Convert from the C type to the C++ type (non-const version).
     */
    स्थिर CPPType* validate(CType* आदान, UErrorCode& status);

    /**
     * Convert from the C++ type to the C type (const version).
     */
    स्थिरांक CType* exportConstForC() स्थिरांक;

    /**
     * Convert from the C++ type to the C type (non-const version).
     */
    CType* exportForC();

    /**
     * Invalidates the object.
     */
    ~IcuCApiHelper();

  खाजगी:
    /**
     * While the object is valid, fMagic equals kMagic.
     */
    int32_t fMagic = kMagic;
};


ढाँचा<typename CType, typename CPPType, int32_t kMagic>
स्थिरांक CPPType*
IcuCApiHelper<CType, CPPType, kMagic>::validate(स्थिरांक CType* आदान, UErrorCode& status) {
    जर (U_FAILURE(status)) {
        परत nullptr;
    }
    जर (आदान == nullptr) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        परत nullptr;
    }
    स्वतः* impl = reinterpret_cast<स्थिरांक CPPType*>(आदान);
    जर (static_cast<स्थिरांक IcuCApiHelper<CType, CPPType, kMagic>*>(impl)->fMagic != kMagic) {
        status = U_INVALID_FORMAT_ERROR;
        परत nullptr;
    }
    परत impl;
}

ढाँचा<typename CType, typename CPPType, int32_t kMagic>
CPPType*
IcuCApiHelper<CType, CPPType, kMagic>::validate(CType* आदान, UErrorCode& status) {
    स्वतः* const_आदान = static_cast<स्थिरांक CType*>(आदान);
    स्वतः* validated = validate(const_आदान, status);
    परत const_cast<CPPType*>(validated);
}

ढाँचा<typename CType, typename CPPType, int32_t kMagic>
स्थिरांक CType*
IcuCApiHelper<CType, CPPType, kMagic>::exportConstForC() स्थिरांक {
    परत reinterpret_cast<स्थिरांक CType*>(static_cast<स्थिरांक CPPType*>(this));
}

ढाँचा<typename CType, typename CPPType, int32_t kMagic>
CType*
IcuCApiHelper<CType, CPPType, kMagic>::exportForC() {
    परत reinterpret_cast<CType*>(static_cast<CPPType*>(this));
}

ढाँचा<typename CType, typename CPPType, int32_t kMagic>
IcuCApiHelper<CType, CPPType, kMagic>::~IcuCApiHelper() {
    // head off application errors by preventing use of of deleted objects.
    fMagic = 0;
}


U_NAMESPACE_END

#endif // __CAPI_HELPER_H__
