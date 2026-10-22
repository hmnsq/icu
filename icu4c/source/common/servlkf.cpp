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
#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_SERVICE

#অন্তর্ভাব "unicode/resbund.h"
#অন্তর্ভাব "uresimp.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "servloc.h"
#অন্তর্ভাব "ustrfmt.h"
#অন্তর্ভাব "uhash.h"
#অন্তর্ভাব "charstr.h"
#অন্তর্ভাব "ucln_cmn.h"
#অন্তর্ভাব "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)


U_NAMESPACE_BEGIN

Locale_চাবি_Factory::Locale_চাবি_Factory(int32_t coverage)
  : _name()
  , _coverage(coverage)
{
}

Locale_চাবি_Factory::Locale_চাবি_Factory(int32_t coverage, ধ্রুব UnicodeString& name)
  : _name(name)
  , _coverage(coverage)
{
}

Locale_চাবি_Factory::~Locale_চাবি_Factory() {
}

UObject*
Locale_চাবি_Factory::create(ধ্রুব ICUService_চাবি& চাবি, ধ্রুব ICUService* service, UErrorCode& status) ধ্রুব {
    যদি (handles_চাবি(চাবি, status)) {
        ধ্রুব Locale_চাবি& lkey = static_cast<ধ্রুব Locale_চাবি&>(চাবি);
        int32_t kind = lkey.kind();
        Locale loc;
        lkey.currentLocale(loc);

        প্রত্যাগম handleCreate(loc, kind, service, status);
    }
    প্রত্যাগম nullptr;
}

UBool
Locale_চাবি_Factory::handles_চাবি(ধ্রুব ICUService_চাবি& চাবি, UErrorCode& status) ধ্রুব {
    ধ্রুব Hashtable* supported = getSupportedIDs(status);
    যদি (supported) {
        UnicodeString id;
        চাবি.currentID(id);
        প্রত্যাগম supported->get(id) != nullptr;
    }
    প্রত্যাগম অসত্য;
}

শূন্য
Locale_চাবি_Factory::updateVisibleIDs(Hashtable& result, UErrorCode& status) ধ্রুব {
    ধ্রুব Hashtable* supported = getSupportedIDs(status);
    যদি (supported) {
        UBool visible = (_coverage & 0x1) == 0;
        ধ্রুব UHashElement* elem = nullptr;
        int32_t pos = UHASH_FIRST;
        যাবত্ ((elem = supported->nextElement(pos)) != nullptr) {
            ধ্রুব UnicodeString& id = *static_cast<ধ্রুব UnicodeString*>(elem->চাবি.pointer);
            যদি (!visible) {
                result.remove(id);
            } অন্যথা {
                result.put(id, (শূন্য*)এতত্, status); // this is dummy non-void marker used for set semantics
                যদি (U_FAILURE(status)) {
                    ভঙ্গ;
                }
            }
        }
    }
}

UnicodeString&
Locale_চাবি_Factory::getDisplayName(ধ্রুব UnicodeString& id, ধ্রুব Locale& locale, UnicodeString& result) ধ্রুব {
    যদি ((_coverage & 0x1) == 0) {
        //UErrorCode status = U_ZERO_ERROR;
        // assume if this is called on us, we support some fallback of this id
        // if (isSupportedID(id, status)) {
            Locale loc;
            LocaleUtility::initLocaleFromName(id, loc);
            প্রত্যাগম loc.getDisplayName(locale, result);
        // }
    }
    result.setToBogus();
    প্রত্যাগম result;
}

UObject*
Locale_চাবি_Factory::handleCreate(ধ্রুব Locale& /* loc */, 
                   int32_t /* kind */, 
                   ধ্রুব ICUService* /* service */, 
                   UErrorCode& /* status */) ধ্রুব {
    প্রত্যাগম nullptr;
}

//UBool
//LocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& status) const {
//    const Hashtable* ids = getSupportedIDs(status);
//    return ids && ids->get(id);
//}

ধ্রুব Hashtable*
Locale_চাবি_Factory::getSupportedIDs(UErrorCode& /* status */) ধ্রুব {
    প্রত্যাগম nullptr;
}

#ifdef SERVICE_DEBUG
UnicodeString&
Locale_চাবি_Factory::debug(UnicodeString& result) ধ্রুব
{
    debugClass(result);
    result.append((UnicodeString)", name: ");
    result.append(_name);
    result.append((UnicodeString)", coverage: ");
    result.append(_coverage);
    প্রত্যাগম result;
}

UnicodeString&
Locale_চাবি_Factory::debugClass(UnicodeString& result) ধ্রুব
{
  প্রত্যাগম result.append((UnicodeString)"LocaleKeyFactory");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_চাবি_Factory)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


