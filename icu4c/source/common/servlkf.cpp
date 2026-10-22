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

#यदि !UCONFIG_NO_SERVICE

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

Locale_कुंजी_Factory::Locale_कुंजी_Factory(int32_t coverage)
  : _name()
  , _coverage(coverage)
{
}

Locale_कुंजी_Factory::Locale_कुंजी_Factory(int32_t coverage, ध्रुव UnicodeString& name)
  : _name(name)
  , _coverage(coverage)
{
}

Locale_कुंजी_Factory::~Locale_कुंजी_Factory() {
}

UObject*
Locale_कुंजी_Factory::create(ध्रुव ICUService_कुंजी& कुंजी, ध्रुव ICUService* service, UErrorCode& status) ध्रुव {
    यदि (handles_कुंजी(कुंजी, status)) {
        ध्रुव Locale_कुंजी& lkey = static_cast<ध्रुव Locale_कुंजी&>(कुंजी);
        int32_t kind = lkey.kind();
        Locale loc;
        lkey.currentLocale(loc);

        प्रत्यागम handleCreate(loc, kind, service, status);
    }
    प्रत्यागम nullptr;
}

UBool
Locale_कुंजी_Factory::handles_कुंजी(ध्रुव ICUService_कुंजी& कुंजी, UErrorCode& status) ध्रुव {
    ध्रुव Hashtable* supported = getSupportedIDs(status);
    यदि (supported) {
        UnicodeString id;
        कुंजी.currentID(id);
        प्रत्यागम supported->get(id) != nullptr;
    }
    प्रत्यागम असत्य;
}

शून्य
Locale_कुंजी_Factory::updateVisibleIDs(Hashtable& result, UErrorCode& status) ध्रुव {
    ध्रुव Hashtable* supported = getSupportedIDs(status);
    यदि (supported) {
        UBool visible = (_coverage & 0x1) == 0;
        ध्रुव UHashElement* elem = nullptr;
        int32_t pos = UHASH_FIRST;
        यावत् ((elem = supported->nextElement(pos)) != nullptr) {
            ध्रुव UnicodeString& id = *static_cast<ध्रुव UnicodeString*>(elem->कुंजी.pointer);
            यदि (!visible) {
                result.remove(id);
            } अन्यथा {
                result.put(id, (शून्य*)एतत्, status); // this is dummy non-void marker used for set semantics
                यदि (U_FAILURE(status)) {
                    भङ्ग;
                }
            }
        }
    }
}

UnicodeString&
Locale_कुंजी_Factory::getDisplayName(ध्रुव UnicodeString& id, ध्रुव Locale& locale, UnicodeString& result) ध्रुव {
    यदि ((_coverage & 0x1) == 0) {
        //UErrorCode status = U_ZERO_ERROR;
        // assume if this is called on us, we support some fallback of this id
        // if (isSupportedID(id, status)) {
            Locale loc;
            LocaleUtility::initLocaleFromName(id, loc);
            प्रत्यागम loc.getDisplayName(locale, result);
        // }
    }
    result.setToBogus();
    प्रत्यागम result;
}

UObject*
Locale_कुंजी_Factory::handleCreate(ध्रुव Locale& /* loc */, 
                   int32_t /* kind */, 
                   ध्रुव ICUService* /* service */, 
                   UErrorCode& /* status */) ध्रुव {
    प्रत्यागम nullptr;
}

//UBool
//LocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& status) const {
//    const Hashtable* ids = getSupportedIDs(status);
//    return ids && ids->get(id);
//}

ध्रुव Hashtable*
Locale_कुंजी_Factory::getSupportedIDs(UErrorCode& /* status */) ध्रुव {
    प्रत्यागम nullptr;
}

#ifdef SERVICE_DEBUG
UnicodeString&
Locale_कुंजी_Factory::debug(UnicodeString& result) ध्रुव
{
    debugClass(result);
    result.append((UnicodeString)", name: ");
    result.append(_name);
    result.append((UnicodeString)", coverage: ");
    result.append(_coverage);
    प्रत्यागम result;
}

UnicodeString&
Locale_कुंजी_Factory::debugClass(UnicodeString& result) ध्रुव
{
  प्रत्यागम result.append((UnicodeString)"LocaleKeyFactory");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_कुंजी_Factory)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


