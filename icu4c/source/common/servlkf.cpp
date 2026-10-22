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

#जर !UCONFIG_NO_SERVICE

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

Locale_किल्ली_Factory::Locale_किल्ली_Factory(int32_t coverage)
  : _name()
  , _coverage(coverage)
{
}

Locale_किल्ली_Factory::Locale_किल्ली_Factory(int32_t coverage, स्थिरांक UnicodeString& name)
  : _name(name)
  , _coverage(coverage)
{
}

Locale_किल्ली_Factory::~Locale_किल्ली_Factory() {
}

UObject*
Locale_किल्ली_Factory::create(स्थिरांक ICUService_किल्ली& किल्ली, स्थिरांक ICUService* service, UErrorCode& status) स्थिरांक {
    जर (handles_किल्ली(किल्ली, status)) {
        स्थिरांक Locale_किल्ली& lkey = static_cast<स्थिरांक Locale_किल्ली&>(किल्ली);
        int32_t kind = lkey.kind();
        Locale loc;
        lkey.currentLocale(loc);

        परत handleCreate(loc, kind, service, status);
    }
    परत nullptr;
}

UBool
Locale_किल्ली_Factory::handles_किल्ली(स्थिरांक ICUService_किल्ली& किल्ली, UErrorCode& status) स्थिरांक {
    स्थिरांक Hashtable* supported = getSupportedIDs(status);
    जर (supported) {
        UnicodeString id;
        किल्ली.currentID(id);
        परत supported->get(id) != nullptr;
    }
    परत असत्य;
}

शून्य
Locale_किल्ली_Factory::updateVisibleIDs(Hashtable& result, UErrorCode& status) स्थिरांक {
    स्थिरांक Hashtable* supported = getSupportedIDs(status);
    जर (supported) {
        UBool visible = (_coverage & 0x1) == 0;
        स्थिरांक UHashElement* elem = nullptr;
        int32_t pos = UHASH_FIRST;
        पर्यंत ((elem = supported->nextElement(pos)) != nullptr) {
            स्थिरांक UnicodeString& id = *static_cast<स्थिरांक UnicodeString*>(elem->किल्ली.pointer);
            जर (!visible) {
                result.हटाओ(id);
            } अन्यथा {
                result.put(id, (शून्य*)हे, status); // this is dummy non-void marker used for set semantics
                जर (U_FAILURE(status)) {
                    थांबा;
                }
            }
        }
    }
}

UnicodeString&
Locale_किल्ली_Factory::getDisplayName(स्थिरांक UnicodeString& id, स्थिरांक Locale& locale, UnicodeString& result) स्थिरांक {
    जर ((_coverage & 0x1) == 0) {
        //UErrorCode status = U_ZERO_ERROR;
        // assume if this is called on us, we support some fallback of this id
        // if (isSupportedID(id, status)) {
            Locale loc;
            LocaleUtility::initLocaleFromName(id, loc);
            परत loc.getDisplayName(locale, result);
        // }
    }
    result.setToBogus();
    परत result;
}

UObject*
Locale_किल्ली_Factory::handleCreate(स्थिरांक Locale& /* loc */, 
                   int32_t /* kind */, 
                   स्थिरांक ICUService* /* service */, 
                   UErrorCode& /* status */) स्थिरांक {
    परत nullptr;
}

//UBool
//LocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& status) const {
//    const Hashtable* ids = getSupportedIDs(status);
//    return ids && ids->get(id);
//}

स्थिरांक Hashtable*
Locale_किल्ली_Factory::getSupportedIDs(UErrorCode& /* status */) स्थिरांक {
    परत nullptr;
}

#ifdef SERVICE_DEBUG
UnicodeString&
Locale_किल्ली_Factory::debug(UnicodeString& result) स्थिरांक
{
    debugClass(result);
    result.append((UnicodeString)", name: ");
    result.append(_name);
    result.append((UnicodeString)", coverage: ");
    result.append(_coverage);
    परत result;
}

UnicodeString&
Locale_किल्ली_Factory::debugClass(UnicodeString& result) स्थिरांक
{
  परत result.append((UnicodeString)"LocaleKeyFactory");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_किल्ली_Factory)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


