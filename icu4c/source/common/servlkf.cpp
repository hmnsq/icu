// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2014, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 *
 *******************************************************************************
 */
#अन्तर्भाव "unicode/utypes.h"

#सचे !UCONFIG_NO_SERVICE

#अन्तर्भाव "unicode/resbund.h"
#अन्तर्भाव "uresimp.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "servloc.h"
#अन्तर्भाव "ustrfmt.h"
#अन्तर्भाव "uhash.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "ucln_cmn.h"
#अन्तर्भाव "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)


U_NAMESPACE_BEGIN

Locale_कुञ्चिका_Factory::Locale_कुञ्चिका_Factory(int32_t coverage)
  : _name()
  , _coverage(coverage)
{
}

Locale_कुञ्चिका_Factory::Locale_कुञ्चिका_Factory(int32_t coverage, ध्रुव UnicodeString& name)
  : _name(name)
  , _coverage(coverage)
{
}

Locale_कुञ्चिका_Factory::~Locale_कुञ्चिका_Factory() {
}

UObject*
Locale_कुञ्चिका_Factory::create(ध्रुव ICUService_कुञ्चिका& कुञ्चिका, ध्रुव ICUService* service, UErrorCode& status) ध्रुव {
    सचे (handles_कुञ्चिका(कुञ्चिका, status)) {
        ध्रुव Locale_कुञ्चिका& lkey = static_cast<ध्रुव Locale_कुञ्चिका&>(कुञ्चिका);
        int32_t kind = lkey.kind();
        Locale loc;
        lkey.currentLocale(loc);

        पटिनिवत्त handleCreate(loc, kind, service, status);
    }
    पटिनिवत्त nullptr;
}

UBool
Locale_कुञ्चिका_Factory::handles_कुञ्चिका(ध्रुव ICUService_कुञ्चिका& कुञ्चिका, UErrorCode& status) ध्रुव {
    ध्रुव Hashtable* supported = getSupportedIDs(status);
    सचे (supported) {
        UnicodeString id;
        कुञ्चिका.currentID(id);
        पटिनिवत्त supported->get(id) != nullptr;
    }
    पटिनिवत्त मिच्छा;
}

शून्य
Locale_कुञ्चिका_Factory::updateVisibleIDs(Hashtable& result, UErrorCode& status) ध्रुव {
    ध्रुव Hashtable* supported = getSupportedIDs(status);
    सचे (supported) {
        UBool visible = (_coverage & 0x1) == 0;
        ध्रुव UHashElement* elem = nullptr;
        int32_t pos = UHASH_FIRST;
        याव ((elem = supported->nextElement(pos)) != nullptr) {
            ध्रुव UnicodeString& id = *static_cast<ध्रुव UnicodeString*>(elem->कुञ्चिका.pointer);
            सचे (!visible) {
                result.remove(id);
            } अञ्ञथा {
                result.put(id, (शून्य*)एतत्, status); // this is dummy non-void marker used for set semantics
                सचे (U_FAILURE(status)) {
                    भङ्ग;
                }
            }
        }
    }
}

UnicodeString&
Locale_कुञ्चिका_Factory::getDisplayName(ध्रुव UnicodeString& id, ध्रुव Locale& locale, UnicodeString& result) ध्रुव {
    सचे ((_coverage & 0x1) == 0) {
        //UErrorCode status = U_ZERO_ERROR;
        // assume if this is called on us, we support some fallback of this id
        // if (isSupportedID(id, status)) {
            Locale loc;
            LocaleUtility::initLocaleFromName(id, loc);
            पटिनिवत्त loc.getDisplayName(locale, result);
        // }
    }
    result.setToBogus();
    पटिनिवत्त result;
}

UObject*
Locale_कुञ्चिका_Factory::handleCreate(ध्रुव Locale& /* loc */, 
                   int32_t /* kind */, 
                   ध्रुव ICUService* /* service */, 
                   UErrorCode& /* status */) ध्रुव {
    पटिनिवत्त nullptr;
}

//UBool
//LocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& status) const {
//    const Hashtable* ids = getSupportedIDs(status);
//    return ids && ids->get(id);
//}

ध्रुव Hashtable*
Locale_कुञ्चिका_Factory::getSupportedIDs(UErrorCode& /* status */) ध्रुव {
    पटिनिवत्त nullptr;
}

#ifdef SERVICE_DEBUG
UnicodeString&
Locale_कुञ्चिका_Factory::debug(UnicodeString& result) ध्रुव
{
    debugClass(result);
    result.append((UnicodeString)", name: ");
    result.append(_name);
    result.append((UnicodeString)", coverage: ");
    result.append(_coverage);
    पटिनिवत्त result;
}

UnicodeString&
Locale_कुञ्चिका_Factory::debugClass(UnicodeString& result) ध्रुव
{
  पटिनिवत्त result.append((UnicodeString)"LocaleKeyFactory");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_कुञ्चिका_Factory)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


