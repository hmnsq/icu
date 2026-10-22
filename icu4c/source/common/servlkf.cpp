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

#परिभाषय UNDERSCORE_CHAR ((char16_t)0x005f)
#परिभाषय AT_SIGN_CHAR    ((char16_t)64)
#परिभाषय PERIOD_CHAR     ((char16_t)46)


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
Locale_कुञ्चिका_Factory::create(ध्रुव ICUService_कुञ्चिका& कुञ्चिका, ध्रुव ICUService* service, UError_संहिता& स्थितिः) ध्रुव {
    यदि (handles_कुञ्चिका(कुञ्चिका, स्थितिः)) {
        ध्रुव Locale_कुञ्चिका& lkey = static_cast<ध्रुव Locale_कुञ्चिका&>(कुञ्चिका);
        int32_t विधा = lkey.विधा();
        Locale loc;
        lkey.currentLocale(loc);

        प्रत्यागम handleCreate(loc, विधा, service, स्थितिः);
    }
    प्रत्यागम शून्यसूचकः;
}

UBool
Locale_कुञ्चिका_Factory::handles_कुञ्चिका(ध्रुव ICUService_कुञ्चिका& कुञ्चिका, UError_संहिता& स्थितिः) ध्रुव {
    ध्रुव Hashtable* supported = getSupportedIDs(स्थितिः);
    यदि (supported) {
        UnicodeString id;
        कुञ्चिका.currentID(id);
        प्रत्यागम supported->get(id) != शून्यसूचकः;
    }
    प्रत्यागम असत्य;
}

शून्य
Locale_कुञ्चिका_Factory::updateVisibleIDs(Hashtable& परिणाम, UError_संहिता& स्थितिः) ध्रुव {
    ध्रुव Hashtable* supported = getSupportedIDs(स्थितिः);
    यदि (supported) {
        UBool visible = (_coverage & 0x1) == 0;
        ध्रुव UHashElement* elem = शून्यसूचकः;
        int32_t pos = UHASH_FIRST;
        यावत् ((elem = supported->अग्र_Element(pos)) != शून्यसूचकः) {
            ध्रुव UnicodeString& id = *static_cast<ध्रुव UnicodeString*>(elem->कुञ्चिका.pointer);
            यदि (!visible) {
                परिणाम.remove(id);
            } अन्यथा {
                परिणाम.put(id, (शून्य*)एतत्, स्थितिः); // this is dummy non-void marker used for set semantics
                यदि (U_FAILURE(स्थितिः)) {
                    भङ्ग;
                }
            }
        }
    }
}

UnicodeString&
Locale_कुञ्चिका_Factory::getDisplayName(ध्रुव UnicodeString& id, ध्रुव Locale& locale, UnicodeString& परिणाम) ध्रुव {
    यदि ((_coverage & 0x1) == 0) {
        //UErrorCode status = U_ZERO_ERROR;
        // assume if this is called on us, we support some fallback of this id
        // if (isSupportedID(id, status)) {
            Locale loc;
            LocaleUtility::initLocaleFromName(id, loc);
            प्रत्यागम loc.getDisplayName(locale, परिणाम);
        // }
    }
    परिणाम.समुच्चयः_गन्तव्यम्_Bogus();
    प्रत्यागम परिणाम;
}

UObject*
Locale_कुञ्चिका_Factory::handleCreate(ध्रुव Locale& /* loc */, 
                   int32_t /*विधा*/, 
                   ध्रुव ICUService* /* service */, 
                   UError_संहिता& /* status */) ध्रुव {
    प्रत्यागम शून्यसूचकः;
}

//UBool
//LocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& status) const {
//    const Hashtable* ids = getSupportedIDs(status);
//    return ids && ids->get(id);
//}

ध्रुव Hashtable*
Locale_कुञ्चिका_Factory::getSupportedIDs(UError_संहिता& /* status */) ध्रुव {
    प्रत्यागम शून्यसूचकः;
}

#यदि_परिभाषितम् SERVICE_DEBUG
UnicodeString&
Locale_कुञ्चिका_Factory::debug(UnicodeString& परिणाम) ध्रुव
{
    debugClass(परिणाम);
    परिणाम.append((UnicodeString)", name: ");
    परिणाम.append(_name);
    परिणाम.append((UnicodeString)", coverage: ");
    परिणाम.append(_coverage);
    प्रत्यागम परिणाम;
}

UnicodeString&
Locale_कुञ्चिका_Factory::debugClass(UnicodeString& परिणाम) ध्रुव
{
  प्रत्यागम परिणाम.append((UnicodeString)"LocaleKeyFactory");
}
#यदि_अन्तः

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_कुञ्चिका_Factory)

U_NAMESPACE_अन्तः

/* !UCONFIG_NO_SERVICE */
#यदि_अन्तः


