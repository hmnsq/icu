// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ifndef __CAPI_HELPER_H__
#define __CAPI_HELPER_H__

#அந்தர்பாவ "unicode/utypes.h"

U_NAMESPACE_BEGIN

/**
 * An internal helper class to help convert between C and C++ APIs.
 */
template<typename CType, typename CPPType, int32_t kMagic>
class IcuCApiHelper {
  பொது:
    /**
     * Convert from the C type to the C++ type (const version).
     */
    நிலையான மாறிலி CPPType* validate(மாறிலி CType* உள்ளீடு, UErrorCode& status);

    /**
     * Convert from the C type to the C++ type (non-const version).
     */
    நிலையான CPPType* validate(CType* உள்ளீடு, UErrorCode& status);

    /**
     * Convert from the C++ type to the C type (const version).
     */
    மாறிலி CType* exportConstForC() மாறிலி;

    /**
     * Convert from the C++ type to the C type (non-const version).
     */
    CType* exportForC();

    /**
     * Invalidates the object.
     */
    ~IcuCApiHelper();

  தனிப்பட்ட:
    /**
     * While the object is valid, fMagic equals kMagic.
     */
    int32_t fMagic = kMagic;
};


template<typename CType, typename CPPType, int32_t kMagic>
மாறிலி CPPType*
IcuCApiHelper<CType, CPPType, kMagic>::validate(மாறிலி CType* உள்ளீடு, UErrorCode& status) {
    எனில் (U_FAILURE(status)) {
        திரும்பு nullptr;
    }
    எனில் (உள்ளீடு == nullptr) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }
    auto* impl = reinterpret_cast<மாறிலி CPPType*>(உள்ளீடு);
    எனில் (static_cast<மாறிலி IcuCApiHelper<CType, CPPType, kMagic>*>(impl)->fMagic != kMagic) {
        status = U_INVALID_FORMAT_ERROR;
        திரும்பு nullptr;
    }
    திரும்பு impl;
}

template<typename CType, typename CPPType, int32_t kMagic>
CPPType*
IcuCApiHelper<CType, CPPType, kMagic>::validate(CType* உள்ளீடு, UErrorCode& status) {
    auto* const_உள்ளீடு = static_cast<மாறிலி CType*>(உள்ளீடு);
    auto* validated = validate(const_உள்ளீடு, status);
    திரும்பு const_cast<CPPType*>(validated);
}

template<typename CType, typename CPPType, int32_t kMagic>
மாறிலி CType*
IcuCApiHelper<CType, CPPType, kMagic>::exportConstForC() மாறிலி {
    திரும்பு reinterpret_cast<மாறிலி CType*>(static_cast<மாறிலி CPPType*>(this));
}

template<typename CType, typename CPPType, int32_t kMagic>
CType*
IcuCApiHelper<CType, CPPType, kMagic>::exportForC() {
    திரும்பு reinterpret_cast<CType*>(static_cast<CPPType*>(this));
}

template<typename CType, typename CPPType, int32_t kMagic>
IcuCApiHelper<CType, CPPType, kMagic>::~IcuCApiHelper() {
    // head off application errors by preventing use of of deleted objects.
    fMagic = 0;
}


U_NAMESPACE_END

#endif // __CAPI_HELPER_H__
