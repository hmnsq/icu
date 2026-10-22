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
#include "unicode/utypes.h"

#if !UCONFIG_NO_SERVICE

#include "unicode/resbund.h"
#include "uresimp.h"
#include "cmemory.h"
#include "servloc.h"
#include "ustrfmt.h"
#include "uhash.h"
#include "charstr.h"
#include "ucln_cmn.h"
#include "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)


U_NAMESPACE_BEGIN

Locale_सुसि_Factory::Locale_सुसि_Factory(int32_t coverage)
  : _name()
  , _coverage(coverage)
{
}

Locale_सुसि_Factory::Locale_सुसि_Factory(int32_t coverage, const UnicodeString& name)
  : _name(name)
  , _coverage(coverage)
{
}

Locale_सुसि_Factory::~Locale_सुसि_Factory() {
}

UObject*
Locale_सुसि_Factory::create(const ICUService_सुसि& सुसि, const ICUService* service, UErrorCode& status) const {
    if (handles_सुसि(सुसि, status)) {
        const Locale_सुसि& lkey = static_cast<const Locale_सुसि&>(सुसि);
        int32_t kind = lkey.kind();
        Locale loc;
        lkey.currentLocale(loc);

        return handleCreate(loc, kind, service, status);
    }
    return nullptr;
}

UBool
Locale_सुसि_Factory::handles_सुसि(const ICUService_सुसि& सुसि, UErrorCode& status) const {
    const Hashtable* supported = getSupportedIDs(status);
    if (supported) {
        UnicodeString id;
        सुसि.currentID(id);
        return supported->get(id) != nullptr;
    }
    return false;
}

void
Locale_सुसि_Factory::updateVisibleIDs(Hashtable& result, UErrorCode& status) const {
    const Hashtable* supported = getSupportedIDs(status);
    if (supported) {
        UBool visible = (_coverage & 0x1) == 0;
        const UHashElement* elem = nullptr;
        int32_t pos = UHASH_FIRST;
        while ((elem = supported->nextElement(pos)) != nullptr) {
            const UnicodeString& id = *static_cast<const UnicodeString*>(elem->सुसि.pointer);
            if (!visible) {
                result.remove(id);
            } else {
                result.put(id, (void*)this, status); // this is dummy non-void marker used for set semantics
                if (U_FAILURE(status)) {
                    break;
                }
            }
        }
    }
}

UnicodeString&
Locale_सुसि_Factory::getDisplayName(const UnicodeString& id, const Locale& locale, UnicodeString& result) const {
    if ((_coverage & 0x1) == 0) {
        //UErrorCode status = U_ZERO_ERROR;
        // assume if this is called on us, we support some fallback of this id
        // if (isSupportedID(id, status)) {
            Locale loc;
            LocaleUtility::initLocaleFromName(id, loc);
            return loc.getDisplayName(locale, result);
        // }
    }
    result.setToBogus();
    return result;
}

UObject*
Locale_सुसि_Factory::handleCreate(const Locale& /* loc */, 
                   int32_t /* kind */, 
                   const ICUService* /* service */, 
                   UErrorCode& /* status */) const {
    return nullptr;
}

//UBool
//LocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& status) const {
//    const Hashtable* ids = getSupportedIDs(status);
//    return ids && ids->get(id);
//}

const Hashtable*
Locale_सुसि_Factory::getSupportedIDs(UErrorCode& /* status */) const {
    return nullptr;
}

#ifdef SERVICE_DEBUG
UnicodeString&
Locale_सुसि_Factory::debug(UnicodeString& result) const
{
    debugClass(result);
    result.append((UnicodeString)", name: ");
    result.append(_name);
    result.append((UnicodeString)", coverage: ");
    result.append(_coverage);
    return result;
}

UnicodeString&
Locale_सुसि_Factory::debugClass(UnicodeString& result) const
{
  return result.append((UnicodeString)"LocaleKeyFactory");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_सुसि_Factory)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


