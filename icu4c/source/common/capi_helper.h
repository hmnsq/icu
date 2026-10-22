// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ifndef __CAPI_HELPER_H__
#define __CAPI_HELPER_H__

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

U_NAMESPACE_BEGIN

/**
 * An internal helper class to help convert between C and C++ APIs.
 */
template<typename CType, typename CPPType, int32_t kMagic>
class IcuCApiHelper {
  ಸಾರ್ವಜನಿಕ:
    /**
     * Convert from the C type to the C++ type (const version).
     */
    ಸ್ಥಿರ ಧ್ರುವ CPPType* validate(ಧ್ರುವ CType* ಒಳಹರಿವು, UErrorCode& status);

    /**
     * Convert from the C type to the C++ type (non-const version).
     */
    ಸ್ಥಿರ CPPType* validate(CType* ಒಳಹರಿವು, UErrorCode& status);

    /**
     * Convert from the C++ type to the C type (const version).
     */
    ಧ್ರುವ CType* exportConstForC() ಧ್ರುವ;

    /**
     * Convert from the C++ type to the C type (non-const version).
     */
    CType* exportForC();

    /**
     * Invalidates the object.
     */
    ~IcuCApiHelper();

  ಗುಪ್ತ:
    /**
     * While the object is valid, fMagic equals kMagic.
     */
    int32_t fMagic = kMagic;
};


template<typename CType, typename CPPType, int32_t kMagic>
ಧ್ರುವ CPPType*
IcuCApiHelper<CType, CPPType, kMagic>::validate(ಧ್ರುವ CType* ಒಳಹರಿವು, UErrorCode& status) {
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಯದಿ (ಒಳಹರಿವು == nullptr) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    auto* impl = reinterpret_cast<ಧ್ರುವ CPPType*>(ಒಳಹರಿವು);
    ಯದಿ (static_cast<ಧ್ರುವ IcuCApiHelper<CType, CPPType, kMagic>*>(impl)->fMagic != kMagic) {
        status = U_INVALID_FORMAT_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಪ್ರತ್ಯಾಗಮ impl;
}

template<typename CType, typename CPPType, int32_t kMagic>
CPPType*
IcuCApiHelper<CType, CPPType, kMagic>::validate(CType* ಒಳಹರಿವು, UErrorCode& status) {
    auto* const_ಒಳಹರಿವು = static_cast<ಧ್ರುವ CType*>(ಒಳಹರಿವು);
    auto* validated = validate(const_ಒಳಹರಿವು, status);
    ಪ್ರತ್ಯಾಗಮ const_cast<CPPType*>(validated);
}

template<typename CType, typename CPPType, int32_t kMagic>
ಧ್ರುವ CType*
IcuCApiHelper<CType, CPPType, kMagic>::exportConstForC() ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ reinterpret_cast<ಧ್ರುವ CType*>(static_cast<ಧ್ರುವ CPPType*>(this));
}

template<typename CType, typename CPPType, int32_t kMagic>
CType*
IcuCApiHelper<CType, CPPType, kMagic>::exportForC() {
    ಪ್ರತ್ಯಾಗಮ reinterpret_cast<CType*>(static_cast<CPPType*>(this));
}

template<typename CType, typename CPPType, int32_t kMagic>
IcuCApiHelper<CType, CPPType, kMagic>::~IcuCApiHelper() {
    // head off application errors by preventing use of of deleted objects.
    fMagic = 0;
}


U_NAMESPACE_END

#endif // __CAPI_HELPER_H__
