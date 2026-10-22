// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#अगर_अघोषित __CAPI_HELPER_H__
#घोषणा __CAPI_HELPER_H__

#समावेश "unicode/utypes.h"

U_NAMESPACE_BEGIN

/**
 * An internal helper class to help convert between C and C++ APIs.
 */
ढाँचा<typename CType, typename CPPType, int32_t kMagic>
class IcuCApiHelper {
  खुला:
    /**
     * Convert from the C type to the C++ type (const version).
     */
    अटल स्थिर CPPType* validate(स्थिर CType* निवेश, UErrorCode& status);

    /**
     * Convert from the C type to the C++ type (non-const version).
     */
    अटल CPPType* validate(CType* निवेश, UErrorCode& status);

    /**
     * Convert from the C++ type to the C type (const version).
     */
    स्थिर CType* exportConstForC() स्थिर;

    /**
     * Convert from the C++ type to the C type (non-const version).
     */
    CType* exportForC();

    /**
     * Invalidates the object.
     */
    ~IcuCApiHelper();

  निजी:
    /**
     * While the object is valid, fMagic equals kMagic.
     */
    int32_t fMagic = kMagic;
};


ढाँचा<typename CType, typename CPPType, int32_t kMagic>
स्थिर CPPType*
IcuCApiHelper<CType, CPPType, kMagic>::validate(स्थिर CType* निवेश, UErrorCode& status) {
    अगर (U_FAILURE(status)) {
        वापस nullptr;
    }
    अगर (निवेश == nullptr) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    }
    स्वतः* impl = reinterpret_cast<स्थिर CPPType*>(निवेश);
    अगर (static_cast<स्थिर IcuCApiHelper<CType, CPPType, kMagic>*>(impl)->fMagic != kMagic) {
        status = U_INVALID_FORMAT_ERROR;
        वापस nullptr;
    }
    वापस impl;
}

ढाँचा<typename CType, typename CPPType, int32_t kMagic>
CPPType*
IcuCApiHelper<CType, CPPType, kMagic>::validate(CType* निवेश, UErrorCode& status) {
    स्वतः* const_निवेश = static_cast<स्थिर CType*>(निवेश);
    स्वतः* validated = validate(const_निवेश, status);
    वापस const_cast<CPPType*>(validated);
}

ढाँचा<typename CType, typename CPPType, int32_t kMagic>
स्थिर CType*
IcuCApiHelper<CType, CPPType, kMagic>::exportConstForC() स्थिर {
    वापस reinterpret_cast<स्थिर CType*>(static_cast<स्थिर CPPType*>(this));
}

ढाँचा<typename CType, typename CPPType, int32_t kMagic>
CType*
IcuCApiHelper<CType, CPPType, kMagic>::exportForC() {
    वापस reinterpret_cast<CType*>(static_cast<CPPType*>(this));
}

ढाँचा<typename CType, typename CPPType, int32_t kMagic>
IcuCApiHelper<CType, CPPType, kMagic>::~IcuCApiHelper() {
    // head off application errors by preventing use of of deleted objects.
    fMagic = 0;
}


U_NAMESPACE_END

#पूर्ण_अगर // __CAPI_HELPER_H__
