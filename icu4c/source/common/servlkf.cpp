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
#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_SERVICE

#समावेश "unicode/resbund.h"
#समावेश "uresimp.h"
#समावेश "cmemory.h"
#समावेश "servloc.h"
#समावेश "ustrfmt.h"
#समावेश "uhash.h"
#समावेश "charstr.h"
#समावेश "ucln_cmn.h"
#समावेश "uassert.h"

#घोषणा UNDERSCORE_CHAR ((char16_t)0x005f)
#घोषणा AT_SIGN_CHAR    ((char16_t)64)
#घोषणा PERIOD_CHAR     ((char16_t)46)


U_NAMESPACE_BEGIN

Locale_कुंजी_Factory::Locale_कुंजी_Factory(int32_t coverage)
  : _name()
  , _coverage(coverage)
{
}

Locale_कुंजी_Factory::Locale_कुंजी_Factory(int32_t coverage, स्थिर UnicodeString& name)
  : _name(name)
  , _coverage(coverage)
{
}

Locale_कुंजी_Factory::~Locale_कुंजी_Factory() {
}

UObject*
Locale_कुंजी_Factory::create(स्थिर ICUService_कुंजी& कुंजी, स्थिर ICUService* service, UErrorCode& status) स्थिर {
    अगर (handles_कुंजी(कुंजी, status)) {
        स्थिर Locale_कुंजी& lkey = static_cast<स्थिर Locale_कुंजी&>(कुंजी);
        int32_t kind = lkey.kind();
        Locale loc;
        lkey.currentLocale(loc);

        वापस handleCreate(loc, kind, service, status);
    }
    वापस nullptr;
}

UBool
Locale_कुंजी_Factory::handles_कुंजी(स्थिर ICUService_कुंजी& कुंजी, UErrorCode& status) स्थिर {
    स्थिर Hashtable* supported = getSupportedIDs(status);
    अगर (supported) {
        UnicodeString id;
        कुंजी.currentID(id);
        वापस supported->लो(id) != nullptr;
    }
    वापस false;
}

व्योम
Locale_कुंजी_Factory::updateVisibleIDs(Hashtable& परिणाम, UErrorCode& status) स्थिर {
    स्थिर Hashtable* supported = getSupportedIDs(status);
    अगर (supported) {
        UBool visible = (_coverage & 0x1) == 0;
        स्थिर UHashElement* elem = nullptr;
        int32_t pos = UHASH_FIRST;
        जबतक ((elem = supported->अग्र_Element(pos)) != nullptr) {
            स्थिर UnicodeString& id = *static_cast<स्थिर UnicodeString*>(elem->कुंजी.pointer);
            अगर (!visible) {
                परिणाम.हटाओ(id);
            } अन्यथा {
                परिणाम.दो(id, (व्योम*)यह, status); // this is dummy non-void marker used for set semantics
                अगर (U_FAILURE(status)) {
                    अवरोध;
                }
            }
        }
    }
}

UnicodeString&
Locale_कुंजी_Factory::getDisplayName(स्थिर UnicodeString& id, स्थिर Locale& locale, UnicodeString& परिणाम) स्थिर {
    अगर ((_coverage & 0x1) == 0) {
        //UErrorCode status = U_ZERO_ERROR;
        // assume if this is called on us, we support some fallback of this id
        // if (isSupportedID(id, status)) {
            Locale loc;
            LocaleUtility::initLocaleFromName(id, loc);
            वापस loc.getDisplayName(locale, परिणाम);
        // }
    }
    परिणाम.setToBogus();
    वापस परिणाम;
}

UObject*
Locale_कुंजी_Factory::handleCreate(स्थिर Locale& /* loc */, 
                   int32_t /* kind */, 
                   स्थिर ICUService* /* service */, 
                   UErrorCode& /* status */) स्थिर {
    वापस nullptr;
}

//UBool
//LocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& status) const {
//    const Hashtable* ids = getSupportedIDs(status);
//    return ids && ids->get(id);
//}

स्थिर Hashtable*
Locale_कुंजी_Factory::getSupportedIDs(UErrorCode& /* status */) स्थिर {
    वापस nullptr;
}

#अगर_घोषित SERVICE_DEBUG
UnicodeString&
Locale_कुंजी_Factory::debug(UnicodeString& परिणाम) स्थिर
{
    debugClass(परिणाम);
    परिणाम.append((UnicodeString)", name: ");
    परिणाम.append(_name);
    परिणाम.append((UnicodeString)", coverage: ");
    परिणाम.append(_coverage);
    वापस परिणाम;
}

UnicodeString&
Locale_कुंजी_Factory::debugClass(UnicodeString& परिणाम) स्थिर
{
  वापस परिणाम.append((UnicodeString)"LocaleKeyFactory");
}
#पूर्ण_अगर

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_कुंजी_Factory)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#पूर्ण_अगर


