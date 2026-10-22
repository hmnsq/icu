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
#அந்தர்பாவ "unicode/utypes.h"

#எனில் !UCONFIG_NO_SERVICE

#அந்தர்பாவ "unicode/resbund.h"
#அந்தர்பாவ "uresimp.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "servloc.h"
#அந்தர்பாவ "ustrfmt.h"
#அந்தர்பாவ "uhash.h"
#அந்தர்பாவ "charstr.h"
#அந்தர்பாவ "ucln_cmn.h"
#அந்தர்பாவ "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)


U_NAMESPACE_BEGIN

Locale_சாவி_Factory::Locale_சாவி_Factory(int32_t coverage)
  : _name()
  , _coverage(coverage)
{
}

Locale_சாவி_Factory::Locale_சாவி_Factory(int32_t coverage, மாறிலி UnicodeString& name)
  : _name(name)
  , _coverage(coverage)
{
}

Locale_சாவி_Factory::~Locale_சாவி_Factory() {
}

UObject*
Locale_சாவி_Factory::create(மாறிலி ICUService_சாவி& சாவி, மாறிலி ICUService* service, UErrorCode& status) மாறிலி {
    எனில் (handles_சாவி(சாவி, status)) {
        மாறிலி Locale_சாவி& lkey = static_cast<மாறிலி Locale_சாவி&>(சாவி);
        int32_t kind = lkey.kind();
        Locale loc;
        lkey.currentLocale(loc);

        திரும்பு handleCreate(loc, kind, service, status);
    }
    திரும்பு nullptr;
}

UBool
Locale_சாவி_Factory::handles_சாவி(மாறிலி ICUService_சாவி& சாவி, UErrorCode& status) மாறிலி {
    மாறிலி Hashtable* supported = getSupportedIDs(status);
    எனில் (supported) {
        UnicodeString id;
        சாவி.currentID(id);
        திரும்பு supported->get(id) != nullptr;
    }
    திரும்பு பொய்;
}

வெறுமை
Locale_சாவி_Factory::updateVisibleIDs(Hashtable& result, UErrorCode& status) மாறிலி {
    மாறிலி Hashtable* supported = getSupportedIDs(status);
    எனில் (supported) {
        UBool visible = (_coverage & 0x1) == 0;
        மாறிலி UHashElement* elem = nullptr;
        int32_t pos = UHASH_FIRST;
        போது ((elem = supported->nextElement(pos)) != nullptr) {
            மாறிலி UnicodeString& id = *static_cast<மாறிலி UnicodeString*>(elem->சாவி.pointer);
            எனில் (!visible) {
                result.remove(id);
            } இல்லையெனில் {
                result.put(id, (வெறுமை*)இது, status); // this is dummy non-void marker used for set semantics
                எனில் (U_FAILURE(status)) {
                    நிறுத்து;
                }
            }
        }
    }
}

UnicodeString&
Locale_சாவி_Factory::getDisplayName(மாறிலி UnicodeString& id, மாறிலி Locale& locale, UnicodeString& result) மாறிலி {
    எனில் ((_coverage & 0x1) == 0) {
        //UErrorCode status = U_ZERO_ERROR;
        // assume if this is called on us, we support some fallback of this id
        // if (isSupportedID(id, status)) {
            Locale loc;
            LocaleUtility::initLocaleFromName(id, loc);
            திரும்பு loc.getDisplayName(locale, result);
        // }
    }
    result.setToBogus();
    திரும்பு result;
}

UObject*
Locale_சாவி_Factory::handleCreate(மாறிலி Locale& /* loc */, 
                   int32_t /* kind */, 
                   மாறிலி ICUService* /* service */, 
                   UErrorCode& /* status */) மாறிலி {
    திரும்பு nullptr;
}

//UBool
//LocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& status) const {
//    const Hashtable* ids = getSupportedIDs(status);
//    return ids && ids->get(id);
//}

மாறிலி Hashtable*
Locale_சாவி_Factory::getSupportedIDs(UErrorCode& /* status */) மாறிலி {
    திரும்பு nullptr;
}

#ifdef SERVICE_DEBUG
UnicodeString&
Locale_சாவி_Factory::debug(UnicodeString& result) மாறிலி
{
    debugClass(result);
    result.append((UnicodeString)", name: ");
    result.append(_name);
    result.append((UnicodeString)", coverage: ");
    result.append(_coverage);
    திரும்பு result;
}

UnicodeString&
Locale_சாவி_Factory::debugClass(UnicodeString& result) மாறிலி
{
  திரும்பு result.append((UnicodeString)"LocaleKeyFactory");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_சாவி_Factory)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


