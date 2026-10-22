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

Locale_চাবি_Factory::Locale_চাবি_Factory(int32_t coverage, ধ্রুবক UnicodeString& name)
  : _name(name)
  , _coverage(coverage)
{
}

Locale_চাবি_Factory::~Locale_চাবি_Factory() {
}

UObject*
Locale_চাবি_Factory::create(ধ্রুবক ICUService_চাবি& চাবি, ধ্রুবক ICUService* service, UErrorCode& status) ধ্রুবক {
    যদি (handles_চাবি(চাবি, status)) {
        ধ্রুবক Locale_চাবি& lkey = static_cast<ধ্রুবক Locale_চাবি&>(চাবি);
        int32_t kind = lkey.kind();
        Locale loc;
        lkey.currentLocale(loc);

        ফেরত handleCreate(loc, kind, service, status);
    }
    ফেরত nullptr;
}

UBool
Locale_চাবি_Factory::handles_চাবি(ধ্রুবক ICUService_চাবি& চাবি, UErrorCode& status) ধ্রুবক {
    ধ্রুবক Hashtable* supported = getSupportedIDs(status);
    যদি (supported) {
        UnicodeString id;
        চাবি.currentID(id);
        ফেরত supported->get(id) != nullptr;
    }
    ফেরত মিথ্যা;
}

শূন্য
Locale_চাবি_Factory::updateVisibleIDs(Hashtable& ফলাফল, UErrorCode& status) ধ্রুবক {
    ধ্রুবক Hashtable* supported = getSupportedIDs(status);
    যদি (supported) {
        UBool visible = (_coverage & 0x1) == 0;
        ধ্রুবক UHashElement* elem = nullptr;
        int32_t pos = UHASH_FIRST;
        যখন ((elem = supported->পরবর্তী_Element(pos)) != nullptr) {
            ধ্রুবক UnicodeString& id = *static_cast<ধ্রুবক UnicodeString*>(elem->চাবি.pointer);
            যদি (!visible) {
                ফলাফল.remove(id);
            } অন্যথা {
                ফলাফল.put(id, (শূন্য*)এই, status); // this is dummy non-void marker used for set semantics
                যদি (U_FAILURE(status)) {
                    বিরতি;
                }
            }
        }
    }
}

UnicodeString&
Locale_চাবি_Factory::getDisplayName(ধ্রুবক UnicodeString& id, ধ্রুবক Locale& locale, UnicodeString& ফলাফল) ধ্রুবক {
    যদি ((_coverage & 0x1) == 0) {
        //UErrorCode status = U_ZERO_ERROR;
        // assume if this is called on us, we support some fallback of this id
        // if (isSupportedID(id, status)) {
            Locale loc;
            LocaleUtility::initLocaleFromName(id, loc);
            ফেরত loc.getDisplayName(locale, ফলাফল);
        // }
    }
    ফলাফল.setToBogus();
    ফেরত ফলাফল;
}

UObject*
Locale_চাবি_Factory::handleCreate(ধ্রুবক Locale& /* loc */, 
                   int32_t /* kind */, 
                   ধ্রুবক ICUService* /* service */, 
                   UErrorCode& /* status */) ধ্রুবক {
    ফেরত nullptr;
}

//UBool
//LocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& status) const {
//    const Hashtable* ids = getSupportedIDs(status);
//    return ids && ids->get(id);
//}

ধ্রুবক Hashtable*
Locale_চাবি_Factory::getSupportedIDs(UErrorCode& /* status */) ধ্রুবক {
    ফেরত nullptr;
}

#ifdef SERVICE_DEBUG
UnicodeString&
Locale_চাবি_Factory::debug(UnicodeString& ফলাফল) ধ্রুবক
{
    debugClass(ফলাফল);
    ফলাফল.append((UnicodeString)", name: ");
    ফলাফল.append(_name);
    ফলাফল.append((UnicodeString)", coverage: ");
    ফলাফল.append(_coverage);
    ফেরত ফলাফল;
}

UnicodeString&
Locale_চাবি_Factory::debugClass(UnicodeString& ফলাফল) ধ্রুবক
{
  ফেরত ফলাফল.append((UnicodeString)"LocaleKeyFactory");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_চাবি_Factory)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


