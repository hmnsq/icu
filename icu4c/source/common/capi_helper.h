// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#यदि_अपरिभाषितम् __CAPI_HELPER_H__
#परिभाषय __CAPI_HELPER_H__

#अन्तर्भाव "unicode/utypes.h"

U_NAMESPACE_BEGIN

/**
 * An internal helper class to help convert between C and C++ APIs.
 */
प्रतिरूपम्<typename CType, typename CPPType, int32_t kMagic>
class IcuCApiHelper {
  सार्वजनिक:
    /**
     * Convert from the C type to the C++ type (const version).
     */
    स्थिर ध्रुव CPPType* validate(ध्रुव CType* निवेशः, UError_संहिता& स्थितिः);

    /**
     * Convert from the C type to the C++ type (non-const version).
     */
    स्थिर CPPType* validate(CType* निवेशः, UError_संहिता& स्थितिः);

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


प्रतिरूपम्<typename CType, typename CPPType, int32_t kMagic>
ध्रुव CPPType*
IcuCApiHelper<CType, CPPType, kMagic>::validate(ध्रुव CType* निवेशः, UError_संहिता& स्थितिः) {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम nullptr;
    }
    यदि (निवेशः == nullptr) {
        स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम nullptr;
    }
    स्वचालित* impl = reinterpret_cast<ध्रुव CPPType*>(निवेशः);
    यदि (static_cast<ध्रुव IcuCApiHelper<CType, CPPType, kMagic>*>(impl)->fMagic != kMagic) {
        स्थितिः = U_INVALID_FORMAT_ERROR;
        प्रत्यागम nullptr;
    }
    प्रत्यागम impl;
}

प्रतिरूपम्<typename CType, typename CPPType, int32_t kMagic>
CPPType*
IcuCApiHelper<CType, CPPType, kMagic>::validate(CType* निवेशः, UError_संहिता& स्थितिः) {
    स्वचालित* const_निवेशः = static_cast<ध्रुव CType*>(निवेशः);
    स्वचालित* validated = validate(const_निवेशः, स्थितिः);
    प्रत्यागम const_cast<CPPType*>(validated);
}

प्रतिरूपम्<typename CType, typename CPPType, int32_t kMagic>
ध्रुव CType*
IcuCApiHelper<CType, CPPType, kMagic>::exportConstForC() ध्रुव {
    प्रत्यागम reinterpret_cast<ध्रुव CType*>(static_cast<ध्रुव CPPType*>(this));
}

प्रतिरूपम्<typename CType, typename CPPType, int32_t kMagic>
CType*
IcuCApiHelper<CType, CPPType, kMagic>::exportForC() {
    प्रत्यागम reinterpret_cast<CType*>(static_cast<CPPType*>(this));
}

प्रतिरूपम्<typename CType, typename CPPType, int32_t kMagic>
IcuCApiHelper<CType, CPPType, kMagic>::~IcuCApiHelper() {
    // head off application errors by preventing use of of deleted objects.
    fMagic = 0;
}


U_NAMESPACE_अन्तः

#यदि_अन्तः // __CAPI_HELPER_H__
