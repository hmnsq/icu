// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#నిర్వచితం_కాకపోతే __CAPI_HELPER_H__
#నిర్వచించు __CAPI_HELPER_H__

#అన్తర్భావ "unicode/utypes.h"

U_NAMESPACE_BEGIN

/**
 * An internal helper class to help convert between C and C++ APIs.
 */
మూస<typename CType, typename CPPType, int32_t kMagic>
class IcuCApiHelper {
  బహిరంగ:
    /**
     * Convert from the C type to the C++ type (const version).
     */
    స్థిర స్థిరరాశి CPPType* validate(స్థిరరాశి CType* ఆదానం, UError_సంకేతం& స్థితి_2);

    /**
     * Convert from the C type to the C++ type (non-const version).
     */
    స్థిర CPPType* validate(CType* ఆదానం, UError_సంకేతం& స్థితి_2);

    /**
     * Convert from the C++ type to the C type (const version).
     */
    స్థిరరాశి CType* exportConstForC() స్థిరరాశి;

    /**
     * Convert from the C++ type to the C type (non-const version).
     */
    CType* exportForC();

    /**
     * Invalidates the object.
     */
    ~IcuCApiHelper();

  అంతరంగ:
    /**
     * While the object is valid, fMagic equals kMagic.
     */
    int32_t fMagic = kMagic;
};


మూస<typename CType, typename CPPType, int32_t kMagic>
స్థిరరాశి CPPType*
IcuCApiHelper<CType, CPPType, kMagic>::validate(స్థిరరాశి CType* ఆదానం, UError_సంకేతం& స్థితి_2) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి nullptr;
    }
    ఒకవేళ (ఆదానం == nullptr) {
        స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి nullptr;
    }
    స్వయంచాలక* impl = reinterpret_cast<స్థిరరాశి CPPType*>(ఆదానం);
    ఒకవేళ (static_cast<స్థిరరాశి IcuCApiHelper<CType, CPPType, kMagic>*>(impl)->fMagic != kMagic) {
        స్థితి_2 = U_INVALID_FORMAT_ERROR;
        తిరిగి nullptr;
    }
    తిరిగి impl;
}

మూస<typename CType, typename CPPType, int32_t kMagic>
CPPType*
IcuCApiHelper<CType, CPPType, kMagic>::validate(CType* ఆదానం, UError_సంకేతం& స్థితి_2) {
    స్వయంచాలక* const_ఆదానం = static_cast<స్థిరరాశి CType*>(ఆదానం);
    స్వయంచాలక* validated = validate(const_ఆదానం, స్థితి_2);
    తిరిగి const_cast<CPPType*>(validated);
}

మూస<typename CType, typename CPPType, int32_t kMagic>
స్థిరరాశి CType*
IcuCApiHelper<CType, CPPType, kMagic>::exportConstForC() స్థిరరాశి {
    తిరిగి reinterpret_cast<స్థిరరాశి CType*>(static_cast<స్థిరరాశి CPPType*>(this));
}

మూస<typename CType, typename CPPType, int32_t kMagic>
CType*
IcuCApiHelper<CType, CPPType, kMagic>::exportForC() {
    తిరిగి reinterpret_cast<CType*>(static_cast<CPPType*>(this));
}

మూస<typename CType, typename CPPType, int32_t kMagic>
IcuCApiHelper<CType, CPPType, kMagic>::~IcuCApiHelper() {
    // head off application errors by preventing use of of deleted objects.
    fMagic = 0;
}


U_NAMESPACE_చివర

#అయితే_ముగింపు // __CAPI_HELPER_H__
