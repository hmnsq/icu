// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __CAPI_HELPER_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __CAPI_HELPER_H__

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

U_NAMESPACE_BEGIN

/**
 * An internal helper class to help convert between C and C++ APIs.
 */
ಮಾದರಿ<typename CType, typename CPPType, int32_t kMagic>
class IcuCApiHelper {
  ಸಾರ್ವಜನಿಕ:
    /**
     * Convert from the C type to the C++ type (const version).
     */
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ CPPType* validate(ಸ್ಥಿರಾಂಕ CType* ಒಳಹರಿವು, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

    /**
     * Convert from the C type to the C++ type (non-const version).
     */
    ಸ್ಥಿರ CPPType* validate(CType* ಒಳಹರಿವು, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

    /**
     * Convert from the C++ type to the C type (const version).
     */
    ಸ್ಥಿರಾಂಕ CType* exportConstForC() ಸ್ಥಿರಾಂಕ;

    /**
     * Convert from the C++ type to the C type (non-const version).
     */
    CType* exportForC();

    /**
     * Invalidates the object.
     */
    ~IcuCApiHelper();

  ಖಾಸಗಿ:
    /**
     * While the object is valid, fMagic equals kMagic.
     */
    int32_t fMagic = kMagic;
};


ಮಾದರಿ<typename CType, typename CPPType, int32_t kMagic>
ಸ್ಥಿರಾಂಕ CPPType*
IcuCApiHelper<CType, CPPType, kMagic>::validate(ಸ್ಥಿರಾಂಕ CType* ಒಳಹರಿವು, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ nullptr;
    }
    ಆದರೆ (ಒಳಹರಿವು == nullptr) {
        ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ nullptr;
    }
    ಸ್ವಯಂಚಾಲಿತ* impl = reinterpret_cast<ಸ್ಥಿರಾಂಕ CPPType*>(ಒಳಹರಿವು);
    ಆದರೆ (static_cast<ಸ್ಥಿರಾಂಕ IcuCApiHelper<CType, CPPType, kMagic>*>(impl)->fMagic != kMagic) {
        ಸ್ಥಿತಿ_2 = U_INVALID_FORMAT_ERROR;
        ಹಿಂತಿರುಗಿ nullptr;
    }
    ಹಿಂತಿರುಗಿ impl;
}

ಮಾದರಿ<typename CType, typename CPPType, int32_t kMagic>
CPPType*
IcuCApiHelper<CType, CPPType, kMagic>::validate(CType* ಒಳಹರಿವು, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    ಸ್ವಯಂಚಾಲಿತ* const_ಒಳಹರಿವು = static_cast<ಸ್ಥಿರಾಂಕ CType*>(ಒಳಹರಿವು);
    ಸ್ವಯಂಚಾಲಿತ* validated = validate(const_ಒಳಹರಿವು, ಸ್ಥಿತಿ_2);
    ಹಿಂತಿರುಗಿ const_cast<CPPType*>(validated);
}

ಮಾದರಿ<typename CType, typename CPPType, int32_t kMagic>
ಸ್ಥಿರಾಂಕ CType*
IcuCApiHelper<CType, CPPType, kMagic>::exportConstForC() ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ reinterpret_cast<ಸ್ಥಿರಾಂಕ CType*>(static_cast<ಸ್ಥಿರಾಂಕ CPPType*>(this));
}

ಮಾದರಿ<typename CType, typename CPPType, int32_t kMagic>
CType*
IcuCApiHelper<CType, CPPType, kMagic>::exportForC() {
    ಹಿಂತಿರುಗಿ reinterpret_cast<CType*>(static_cast<CPPType*>(this));
}

ಮಾದರಿ<typename CType, typename CPPType, int32_t kMagic>
IcuCApiHelper<CType, CPPType, kMagic>::~IcuCApiHelper() {
    // head off application errors by preventing use of of deleted objects.
    fMagic = 0;
}


U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ // __CAPI_HELPER_H__
