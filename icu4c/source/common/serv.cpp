// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
*******************************************************************************
* Copyright (C) 2001-2014, International Business Machines Corporation.
* All Rights Reserved.
*******************************************************************************
*/

#ضمن "unicode/utypes.h"
#ضمن "unicode/localpointer.h"

#إذا !UCONFIG_NO_SERVICE

#ضمن "serv.h"
#ضمن "umutex.h"

#لغو_تعریف SERVICE_REFCOUNT

// in case we use the refcount stuff

U_NAMESPACE_BEGIN

/*
******************************************************************
*/

ثابت char16_t ICUService_کلید::پیشوند_DELIMITER = 0x002F;   /* '/' */

ICUService_کلید::ICUService_کلید(ثابت UnicodeString& id) 
: _id(id) {
}

ICUService_کلید::~ICUService_کلید() 
{
}

ثابت UnicodeString& 
ICUService_کلید::getID() ثابت 
{
    أرجع _id;
}

UnicodeString& 
ICUService_کلید::canonicalID(UnicodeString& نتیجه) ثابت 
{
    أرجع نتیجه.append(_id);
}

UnicodeString& 
ICUService_کلید::currentID(UnicodeString& نتیجه) ثابت 
{
    أرجع canonicalID(نتیجه);
}

UnicodeString& 
ICUService_کلید::currentDescriptor(UnicodeString& نتیجه) ثابت 
{
    پیشوند(نتیجه);
    نتیجه.append(پیشوند_DELIMITER);
    أرجع currentID(نتیجه);
}

UBool 
ICUService_کلید::fallback() 
{
    أرجع خطأ;
}

UBool 
ICUService_کلید::is_Fallback_از(ثابت UnicodeString& id) ثابت 
{
    أرجع id == _id;
}

UnicodeString& 
ICUService_کلید::پیشوند(UnicodeString& نتیجه) ثابت 
{
    أرجع نتیجه;
}

UnicodeString& 
ICUService_کلید::parse_پیشوند(UnicodeString& نتیجه) 
{
    int32_t n = نتیجه.شاخص_از(پیشوند_DELIMITER);
    إذا (n < 0) {
        n = 0;
    }
    نتیجه.remove(n);
    أرجع نتیجه;
}

UnicodeString& 
ICUService_کلید::parseSuffix(UnicodeString& نتیجه) 
{
    int32_t n = نتیجه.شاخص_از(پیشوند_DELIMITER);
    إذا (n >= 0) {
        نتیجه.remove(0, n+1);
    }
    أرجع نتیجه;
}

#اگر_تعریف_شده SERVICE_DEBUG
UnicodeString& 
ICUService_کلید::debug(UnicodeString& نتیجه) ثابت 
{
    debugClass(نتیجه);
    نتیجه.append((UnicodeString)" id: ");
    نتیجه.append(_id);
    أرجع نتیجه;
}

UnicodeString& 
ICUService_کلید::debugClass(UnicodeString& نتیجه) ثابت 
{
    أرجع نتیجه.append((UnicodeString)"ICUServiceKey");
}
#پایان_اگر

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ICUService_کلید)

/*
******************************************************************
*/

ICUServiceFactory::~ICUServiceFactory() {}

SimpleFactory::SimpleFactory(UObject* instance_مقصد_Adopt, ثابت UnicodeString& id, UBool visible) 
: _instance(instance_مقصد_Adopt), _id(id), _visible(visible)
{
}

SimpleFactory::~SimpleFactory() 
{
    حذف _instance;
}

UObject* 
SimpleFactory::create(ثابت ICUService_کلید& کلید, ثابت ICUService* service, UError_کد& وضعیت) ثابت 
{
    إذا (U_SUCCESS(وضعیت)) {
        UnicodeString موقت;
        إذا (_id == کلید.currentID(موقت)) {
            أرجع service->cloneInstance(_instance); 
        }
    }
    أرجع اشاره_تهی;
}

فراغ 
SimpleFactory::updateVisibleIDs(Hashtable& نتیجه, UError_کد& وضعیت) ثابت 
{
    إذا (_visible) {
        نتیجه.put(_id, (فراغ*)هذا, وضعیت); // cast away const
    } وإلا {
        نتیجه.remove(_id);
    }
}

UnicodeString& 
SimpleFactory::getDisplayName(ثابت UnicodeString& id, ثابت Locale& /* locale */, UnicodeString& نتیجه) ثابت 
{
    إذا (_visible && _id == id) {
        نتیجه = _id;
    } وإلا {
        نتیجه.مجموعه_مقصد_Bogus();
    }
    أرجع نتیجه;
}

#اگر_تعریف_شده SERVICE_DEBUG
UnicodeString& 
SimpleFactory::debug(UnicodeString& مقصد_Append_مقصد) ثابت 
{
    debugClass(مقصد_Append_مقصد);
    مقصد_Append_مقصد.append((UnicodeString)" id: ");
    مقصد_Append_مقصد.append(_id);
    مقصد_Append_مقصد.append((UnicodeString)", visible: ");
    مقصد_Append_مقصد.append(_visible ? (UnicodeString)"T" : (UnicodeString)"F");
    أرجع مقصد_Append_مقصد;
}

UnicodeString& 
SimpleFactory::debugClass(UnicodeString& مقصد_Append_مقصد) ثابت 
{
    أرجع مقصد_Append_مقصد.append((UnicodeString)"SimpleFactory");
}
#پایان_اگر

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(SimpleFactory)

/*
******************************************************************
*/

ServiceListener::~ServiceListener() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ServiceListener)

/*
******************************************************************
*/

// Record the actual id for this service in the cache, so we can return it
// even if we succeed later with a different id.
صنف Cache_مدخل : عام UMemory {
خاص:
    int32_t refcount;

عام:
    UnicodeString actualDescriptor;
    UObject* service;

    /**
    * Releases a reference to the shared resource.
    */
    ~Cache_مدخل() {
        حذف service;
    }

    Cache_مدخل(ثابت UnicodeString& _actualDescriptor, UObject* _service) 
        : refcount(1), actualDescriptor(_actualDescriptor), service(_service) {
    }

    /**
    * Instantiation creates an initial reference, so don't call this
    * unless you're creating a new pointer to this.  Management of
    * that pointer will have to know how to deal with refcounts.  
    * Return true if the resource has not already been released.
    */
    Cache_مدخل* ref() {
        ++refcount;
        أرجع هذا;
    }

    /**
    * Destructions removes a reference, so don't call this unless
    * you're removing pointer to this somewhere.  Management of that
    * pointer will have to know how to deal with refcounts.  Once
    * the refcount drops to zero, the resource is released.  Return
    * false if the resource has been released.
    */
    Cache_مدخل* unref() {
        إذا ((--refcount) == 0) {
            حذف هذا;
            أرجع اشاره_تهی;
        }
        أرجع هذا;
    }

    /**
    * Return true if there is at least one reference to this and the
    * resource has not been released.
    */
    UBool isShared() ثابت {
        أرجع refcount > 1;
    }
};

// Deleter for serviceCache
U_CDECL_BEGIN
ساكن فراغ U_CALLCONV
cacheDeleter(فراغ* obj) {
    U_NAMESPACE_USE ((Cache_مدخل*)obj)->unref();
}

U_CDECL_انتها

/*
******************************************************************
*/

صنف DNCache : عام UMemory {
عام:
    Hashtable cache;
    ثابت Locale locale;

    DNCache(ثابت Locale& _locale) 
        : cache(), locale(_locale) 
    {
        // cache.setKeyDeleter(uprv_deleteUObject);
    }
};


/*
******************************************************************
*/

StringPair* 
StringPair::create(ثابت UnicodeString& displayName, 
                   ثابت UnicodeString& id,
                   UError_کد& وضعیت)
{
    إذا (U_SUCCESS(وضعیت)) {
        StringPair* sp = جديد StringPair(displayName, id);
        إذا (sp == اشاره_تهی || sp->isBogus()) {
            وضعیت = U_حافظه_تخصیص_ERROR;
            حذف sp;
            أرجع اشاره_تهی;
        }
        أرجع sp;
    }
    أرجع اشاره_تهی;
}

UBool 
StringPair::isBogus() ثابت {
    أرجع displayName.isBogus() || id.isBogus();
}

StringPair::StringPair(ثابت UnicodeString& _displayName, 
                       ثابت UnicodeString& _id)
: displayName(_displayName)
, id(_id)
{
}

U_CDECL_BEGIN
ساكن فراغ U_CALLCONV
userv_deleteStringPair(فراغ *obj) {
    U_NAMESPACE_USE حذف (StringPair*) obj;
}
U_CDECL_انتها

/*
******************************************************************
*/

ساكن UMutex قفل;

ICUService::ICUService()
: name()
, timestamp(0)
, factories(اشاره_تهی)
, serviceCache(اشاره_تهی)
, idCache(اشاره_تهی)
, dnCache(اشاره_تهی)
{
}

ICUService::ICUService(ثابت UnicodeString& newName) 
: name(newName)
, timestamp(0)
, factories(اشاره_تهی)
, serviceCache(اشاره_تهی)
, idCache(اشاره_تهی)
, dnCache(اشاره_تهی)
{
}

ICUService::~ICUService()
{
    {
        Mutex mutex(&قفل);
        clearCaches();
        حذف factories;
        factories = اشاره_تهی;
    }
}

UObject* 
ICUService::get(ثابت UnicodeString& descriptor, UError_کد& وضعیت) ثابت 
{
    أرجع get(descriptor, اشاره_تهی, وضعیت);
}

UObject* 
ICUService::get(ثابت UnicodeString& descriptor, UnicodeString* actualReturn, UError_کد& وضعیت) ثابت 
{
    UObject* نتیجه = اشاره_تهی;
    ICUService_کلید* کلید = create_کلید(&descriptor, وضعیت);
    إذا (کلید) {
        نتیجه = get_کلید(*کلید, actualReturn, وضعیت);
        حذف کلید;
    }
    أرجع نتیجه;
}

UObject* 
ICUService::get_کلید(ICUService_کلید& کلید, UError_کد& وضعیت) ثابت 
{
    أرجع get_کلید(کلید, اشاره_تهی, وضعیت);
}

// this is a vector that subclasses of ICUService can override to further customize the result object
// before returning it.  All other public get functions should call this one.

UObject* 
ICUService::get_کلید(ICUService_کلید& کلید, UnicodeString* actualReturn, UError_کد& وضعیت) ثابت 
{
    أرجع get_کلید(کلید, actualReturn, اشاره_تهی, وضعیت);
}

// make it possible to call reentrantly on systems that don't have reentrant mutexes.
// we can use this simple approach since we know the situation where we're calling
// reentrantly even without knowing the thread.
صنف XMutex : عام UMemory {
عام:
    مضمن XMutex(UMutex *mutex, UBool reentering) 
        : fMutex(mutex)
        , f_فعال(!reentering) 
    {
        إذا (f_فعال) umtx_قفل(fMutex);
    }
    مضمن ~XMutex() {
        إذا (f_فعال) umtx_unlock(fMutex);
    }

خاص:
    UMutex  *fMutex;
    UBool f_فعال;
};

// called only by factories, treat as private
UObject* 
ICUService::get_کلید(ICUService_کلید& کلید, UnicodeString* actualReturn, ثابت ICUServiceFactory* factory, UError_کد& وضعیت) ثابت 
{
    إذا (U_FAILURE(وضعیت)) {
        أرجع اشاره_تهی;
    }

    إذا (isDefault()) {
        أرجع handleDefault(کلید, actualReturn, وضعیت);
    }

    ICUService* ncthis = const_cast<ICUService*>(هذا); // cast away semantic const

    Cache_مدخل* نتیجه = اشاره_تهی;
    {
        // The factory list can't be modified until we're done, 
        // otherwise we might update the cache with an invalid result.
        // The cache has to stay in synch with the factory list.
        // ICU doesn't have monitors so we can't use rw locks, so 
        // we single-thread everything using this service, for now.

        // if factory is not null, we're calling from within the mutex,
        // and since some unix machines don't have reentrant mutexes we
        // need to make sure not to try to lock it again.
        XMutex mutex(&قفل, factory != اشاره_تهی);

        إذا (serviceCache == اشاره_تهی) {
            ncthis->serviceCache = جديد Hashtable(وضعیت);
            إذا (ncthis->serviceCache == اشاره_تهی) {
                وضعیت = U_حافظه_تخصیص_ERROR;
                أرجع اشاره_تهی;
            }
            إذا (U_FAILURE(وضعیت)) {
                حذف serviceCache;
                أرجع اشاره_تهی;
            }
            serviceCache->مجموعه_مقدار_Deleter(cacheDeleter);
        }

        UnicodeString currentDescriptor;
        Local_اشاره_گر<UVector> cacheDescriptorList;
        UBool put_درون_Cache = خطأ;

        int32_t شروع_شاخص = 0;
        int32_t حد = factories->size();
        UBool cache_نتیجه = درست;

        إذا (factory != اشاره_تهی) {
            لكل (int32_t i = 0; i < حد; ++i) {
                إذا (factory == static_cast<ثابت ICUServiceFactory*>(factories->elementAt(i))) {
                    شروع_شاخص = i + 1;
                    اكسر;
                }
            }
            إذا (شروع_شاخص == 0) {
                // throw new InternalError("Factory " + factory + "not registered with service: " + this);
                وضعیت = U_ILLEGAL_آرگومان_ERROR;
                أرجع اشاره_تهی;
            }
            cache_نتیجه = خطأ;
        }

        افعل {
            currentDescriptor.remove();
            کلید.currentDescriptor(currentDescriptor);
            نتیجه = static_cast<Cache_مدخل*>(serviceCache->get(currentDescriptor));
            إذا (نتیجه != اشاره_تهی) {
                اكسر;
            }

            // first test of cache failed, so we'll have to update
            // the cache if we eventually succeed-- that is, if we're 
            // going to update the cache at all.
            put_درون_Cache = درست;

            int32_t شاخص = شروع_شاخص;
            طالما (شاخص < حد) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(شاخص++));
                Local_اشاره_گر<UObject> service(f->create(کلید, هذا, وضعیت));
                إذا (U_FAILURE(وضعیت)) {
                    أرجع اشاره_تهی;
                }
                إذا (service.isValid()) {
                    نتیجه = جديد Cache_مدخل(currentDescriptor, service.getAlias());
                    إذا (نتیجه == اشاره_تهی) {
                        وضعیت = U_حافظه_تخصیص_ERROR;
                        أرجع اشاره_تهی;
                    }
                    service.orphan(); // result now owns service.

                    اذهب outer_انتها;
                }
            }

            // prepare to load the cache with all additional ids that 
            // will resolve to result, assuming we'll succeed.  We
            // don't want to keep querying on an id that's going to
            // fallback to the one that succeeded, we want to hit the
            // cache the first time next goaround.
            إذا (cacheDescriptorList.isNull()) {
                cacheDescriptorList.adopt_Instead_And_Check_Error_کد(جديد UVector(uprv_deleteUObject, اشاره_تهی, 5, وضعیت), وضعیت);
                إذا (U_FAILURE(وضعیت)) {
                    أرجع اشاره_تهی;
                }
            }

            Local_اشاره_گر<UnicodeString> id_مقصد_Cache(جديد UnicodeString(currentDescriptor), وضعیت);
            إذا (U_FAILURE(وضعیت)) {
                أرجع اشاره_تهی;
            }
            إذا (id_مقصد_Cache->isBogus()) {
                وضعیت = U_حافظه_تخصیص_ERROR;
                أرجع اشاره_تهی;
            }
            cacheDescriptorList->adoptElement(id_مقصد_Cache.orphan(), وضعیت);
            إذا (U_FAILURE(وضعیت)) {
                أرجع اشاره_تهی;
            }
        } طالما (کلید.fallback());
outer_انتها:

        إذا (نتیجه != اشاره_تهی) {
            إذا (put_درون_Cache && cache_نتیجه) {
                serviceCache->put(نتیجه->actualDescriptor, نتیجه, وضعیت);
                إذا (U_FAILURE(وضعیت)) {
                    أرجع اشاره_تهی;
                }

                إذا (cacheDescriptorList.isValid()) {
                    لكل (int32_t i = cacheDescriptorList->size(); --i >= 0;) {
                        UnicodeString* desc = static_cast<UnicodeString*>(cacheDescriptorList->elementAt(i));

                        serviceCache->put(*desc, نتیجه, وضعیت);
                        إذا (U_FAILURE(وضعیت)) {
                            أرجع اشاره_تهی;
                        }

                        نتیجه->ref();
                        cacheDescriptorList->removeElementAt(i);
                    }
                }
            }

            إذا (actualReturn != اشاره_تهی) {
                // strip null prefix
                إذا (نتیجه->actualDescriptor.شاخص_از(static_cast<char16_t>(0x2f)) == 0) { // U+002f=slash (/)
                    actualReturn->remove();
                    actualReturn->append(نتیجه->actualDescriptor, 
                        1, 
                        نتیجه->actualDescriptor.length() - 1);
                } وإلا {
                    *actualReturn = نتیجه->actualDescriptor;
                }

                إذا (actualReturn->isBogus()) {
                    وضعیت = U_حافظه_تخصیص_ERROR;
                    حذف نتیجه;
                    أرجع اشاره_تهی;
                }
            }

            UObject* service = cloneInstance(نتیجه->service);
            إذا (put_درون_Cache && !cache_نتیجه) {
                حذف نتیجه;
            }
            أرجع service;
        }
    }

    أرجع handleDefault(کلید, actualReturn, وضعیت);
}

UObject* 
ICUService::handleDefault(ثابت ICUService_کلید& /*کلید*/, UnicodeString* /* actualIDReturn */, UError_کد& /* status */) ثابت 
{
    أرجع اشاره_تهی;
}

UVector& 
ICUService::getVisibleIDs(UVector& نتیجه, UError_کد& وضعیت) ثابت {
    أرجع getVisibleIDs(نتیجه, اشاره_تهی, وضعیت);
}

UVector& 
ICUService::getVisibleIDs(UVector& نتیجه, ثابت UnicodeString* تطابق_ID, UError_کد& وضعیت) ثابت 
{
    نتیجه.removeAllElements();

    إذا (U_FAILURE(وضعیت)) {
        أرجع نتیجه;
    }
    UObjectDeleter *savedDeleter = نتیجه.مجموعه_Deleter(uprv_deleteUObject);

    {
        Mutex mutex(&قفل);
        ثابت Hashtable* map = getVisibleIDMap(وضعیت);
        إذا (map != اشاره_تهی) {
            ICUService_کلید* fallback_کلید = create_کلید(تطابق_ID, وضعیت);

            لكل (int32_t pos = UHASH_اولی; U_SUCCESS(وضعیت); ) {
                ثابت UHashElement* e = map->بعدی_Element(pos);
                إذا (e == اشاره_تهی) {
                    اكسر;
                }

                ثابت UnicodeString* id = static_cast<ثابت UnicodeString*>(e->کلید.اشاره_گر);
                إذا (fallback_کلید != اشاره_تهی) {
                    إذا (!fallback_کلید->is_Fallback_از(*id)) {
                        استمر;
                    }
                }

                Local_اشاره_گر<UnicodeString> idClone(id->clone(), وضعیت);
                نتیجه.adoptElement(idClone.orphan(), وضعیت);
            }
            حذف fallback_کلید;
        }
    }
    إذا (U_FAILURE(وضعیت)) {
        نتیجه.removeAllElements();
    }
    نتیجه.مجموعه_Deleter(savedDeleter);
    أرجع نتیجه;
}

ثابت Hashtable* 
ICUService::getVisibleIDMap(UError_کد& وضعیت) ثابت {
    إذا (U_FAILURE(وضعیت)) أرجع اشاره_تهی;

    // must only be called when lock is already held

    ICUService* ncthis = const_cast<ICUService*>(هذا); // cast away semantic const
    إذا (idCache == اشاره_تهی) {
        ncthis->idCache = جديد Hashtable(وضعیت);
        إذا (idCache == اشاره_تهی) {
            وضعیت = U_حافظه_تخصیص_ERROR;
        } وإلا إذا (factories != اشاره_تهی) {
            لكل (int32_t pos = factories->size(); --pos >= 0;) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(pos));
                f->updateVisibleIDs(*idCache, وضعیت);
            }
            إذا (U_FAILURE(وضعیت)) {
                حذف idCache;
                ncthis->idCache = اشاره_تهی;
            }
        }
    }

    أرجع idCache;
}


UnicodeString& 
ICUService::getDisplayName(ثابت UnicodeString& id, UnicodeString& نتیجه) ثابت 
{
    أرجع getDisplayName(id, نتیجه, Locale::getDefault());
}

UnicodeString& 
ICUService::getDisplayName(ثابت UnicodeString& id, UnicodeString& نتیجه, ثابت Locale& locale) ثابت 
{
    {
        UError_کد وضعیت = U_ZERO_ERROR;
        Mutex mutex(&قفل);
        ثابت Hashtable* map = getVisibleIDMap(وضعیت);
        إذا (map != اشاره_تهی) {
            ICUServiceFactory* f = static_cast<ICUServiceFactory*>(map->get(id));
            إذا (f != اشاره_تهی) {
                f->getDisplayName(id, locale, نتیجه);
                أرجع نتیجه;
            }

            // fallback
            وضعیت = U_ZERO_ERROR;
            ICUService_کلید* fallback_کلید = create_کلید(&id, وضعیت);
            طالما (fallback_کلید != اشاره_تهی && fallback_کلید->fallback()) {
                UnicodeString us;
                fallback_کلید->currentID(us);
                f = static_cast<ICUServiceFactory*>(map->get(us));
                إذا (f != اشاره_تهی) {
                    f->getDisplayName(id, locale, نتیجه);
                    حذف fallback_کلید;
                    أرجع نتیجه;
                }
            }
            حذف fallback_کلید;
        }
    }
    نتیجه.مجموعه_مقصد_Bogus();
    أرجع نتیجه;
}

UVector& 
ICUService::get_Display_نامها(UVector& نتیجه, UError_کد& وضعیت) ثابت 
{
    أرجع get_Display_نامها(نتیجه, Locale::getDefault(), اشاره_تهی, وضعیت);
}


UVector& 
ICUService::get_Display_نامها(UVector& نتیجه, ثابت Locale& locale, UError_کد& وضعیت) ثابت 
{
    أرجع get_Display_نامها(نتیجه, locale, اشاره_تهی, وضعیت);
}

UVector& 
ICUService::get_Display_نامها(UVector& نتیجه, 
                            ثابت Locale& locale, 
                            ثابت UnicodeString* تطابق_ID, 
                            UError_کد& وضعیت) ثابت 
{
    نتیجه.removeAllElements();
    نتیجه.مجموعه_Deleter(userv_deleteStringPair);
    إذا (U_SUCCESS(وضعیت)) {
        ICUService* ncthis = const_cast<ICUService*>(هذا); // cast away semantic const
        Mutex mutex(&قفل);

        إذا (dnCache != اشاره_تهی && dnCache->locale != locale) {
            حذف dnCache;
            ncthis->dnCache = اشاره_تهی;
        }

        إذا (dnCache == اشاره_تهی) {
            ثابت Hashtable* m = getVisibleIDMap(وضعیت);
            إذا (U_FAILURE(وضعیت)) {
                أرجع نتیجه;
            }
            ncthis->dnCache = جديد DNCache(locale); 
            إذا (dnCache == اشاره_تهی) {
                وضعیت = U_حافظه_تخصیص_ERROR;
                أرجع نتیجه;
            }

            int32_t pos = UHASH_اولی;
            ثابت UHashElement* مدخل = اشاره_تهی;
            طالما ((مدخل = m->بعدی_Element(pos)) != اشاره_تهی) {
                ثابت UnicodeString* id = static_cast<ثابت UnicodeString*>(مدخل->کلید.اشاره_گر);
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(مدخل->value.اشاره_گر);
                UnicodeString dname;
                f->getDisplayName(*id, locale, dname);
                إذا (dname.isBogus()) {
                    وضعیت = U_حافظه_تخصیص_ERROR;
                } وإلا {
                    dnCache->cache.put(dname, (فراغ*)id, وضعیت); // share pointer with visibleIDMap
                    إذا (U_SUCCESS(وضعیت)) {
                        استمر;
                    }
                }
                حذف dnCache;
                ncthis->dnCache = اشاره_تهی;
                أرجع نتیجه;
            }
        }
    }

    ICUService_کلید* تطابق_کلید = create_کلید(تطابق_ID, وضعیت);
    /* To ensure that all elements in the hashtable are iterated, set pos to -1.
     * nextElement(pos) will skip the position at pos and begin the iteration
     * at the next position, which in this case will be 0.
     */
    int32_t pos = UHASH_اولی; 
    ثابت UHashElement *مدخل = اشاره_تهی;
    طالما ((مدخل = dnCache->cache.بعدی_Element(pos)) != اشاره_تهی) {
        ثابت UnicodeString* id = static_cast<ثابت UnicodeString*>(مدخل->value.اشاره_گر);
        إذا (تطابق_کلید != اشاره_تهی && !تطابق_کلید->is_Fallback_از(*id)) {
            استمر;
        }
        ثابت UnicodeString* dn = static_cast<ثابت UnicodeString*>(مدخل->کلید.اشاره_گر);
        StringPair* sp = StringPair::create(*id, *dn, وضعیت);
        نتیجه.adoptElement(sp, وضعیت);
        إذا (U_FAILURE(وضعیت)) {
            نتیجه.removeAllElements();
            اكسر;
        }
    }
    حذف تطابق_کلید;

    أرجع نتیجه;
}

URegistry_کلید
ICUService::ثبات_Instance(UObject* obj_مقصد_Adopt, ثابت UnicodeString& id, UError_کد& وضعیت) 
{
    أرجع ثبات_Instance(obj_مقصد_Adopt, id, درست, وضعیت);
}

URegistry_کلید
ICUService::ثبات_Instance(UObject* obj_مقصد_Adopt, ثابت UnicodeString& id, UBool visible, UError_کد& وضعیت) 
{
    ICUService_کلید* کلید = create_کلید(&id, وضعیت);
    إذا (کلید != اشاره_تهی) {
        UnicodeString canonicalID;
        کلید->canonicalID(canonicalID);
        حذف کلید;

        ICUServiceFactory* f = createSimpleFactory(obj_مقصد_Adopt, canonicalID, visible, وضعیت);
        إذا (f != اشاره_تهی) {
            أرجع ثبات_Factory(f, وضعیت);
        }
    }
    حذف obj_مقصد_Adopt;
    أرجع اشاره_تهی;
}

ICUServiceFactory* 
ICUService::createSimpleFactory(UObject* obj_مقصد_Adopt, ثابت UnicodeString& id, UBool visible, UError_کد& وضعیت)
{
    إذا (U_SUCCESS(وضعیت)) {
        إذا ((obj_مقصد_Adopt != اشاره_تهی) && (!id.isBogus())) {
            أرجع جديد SimpleFactory(obj_مقصد_Adopt, id, visible);
        }
        وضعیت = U_ILLEGAL_آرگومان_ERROR;
    }
    أرجع اشاره_تهی;
}

URegistry_کلید
ICUService::ثبات_Factory(ICUServiceFactory* factory_مقصد_Adopt, UError_کد& وضعیت)
{
    Local_اشاره_گر<ICUServiceFactory>lp_Factory_مقصد_Adopt(factory_مقصد_Adopt);
    إذا (U_FAILURE(وضعیت) || factory_مقصد_Adopt == اشاره_تهی) {
        أرجع اشاره_تهی;
    }
    {
        Mutex mutex(&قفل);

        إذا (factories == اشاره_تهی) {
            Local_اشاره_گر<UVector> lpFactories(جديد UVector(uprv_deleteUObject, اشاره_تهی, وضعیت), وضعیت);
            إذا (U_FAILURE(وضعیت)) {
                أرجع اشاره_تهی;
            }
            factories = lpFactories.orphan();
        }
        factories->درج_Element_At(lp_Factory_مقصد_Adopt.orphan(), 0, وضعیت);
        إذا (U_SUCCESS(وضعیت)) {
            clearCaches();
        }
    }   // Close of mutex lock block.

    إذا (U_SUCCESS(وضعیت)) {
        notify_تغییریافته();
        أرجع (URegistry_کلید)factory_مقصد_Adopt;
    } وإلا {
        أرجع اشاره_تهی;
    }
}

UBool 
ICUService::unregister(URegistry_کلید rkey, UError_کد& وضعیت) 
{
    ICUServiceFactory *factory = (ICUServiceFactory*)rkey;
    UBool نتیجه = خطأ;
    إذا (factory != اشاره_تهی && factories != اشاره_تهی) {
        Mutex mutex(&قفل);

        إذا (factories->removeElement(factory)) {
            clearCaches();
            نتیجه = درست;
        } وإلا {
            وضعیت = U_ILLEGAL_آرگومان_ERROR;
            حذف factory;
        }
    }
    إذا (نتیجه) {
        notify_تغییریافته();
    }
    أرجع نتیجه;
}

فراغ 
ICUService::reset() 
{
    {
        Mutex mutex(&قفل);
        reInitializeFactories();
        clearCaches();
    }
    notify_تغییریافته();
}

فراغ 
ICUService::reInitializeFactories() 
{
    إذا (factories != اشاره_تهی) {
        factories->removeAllElements();
    }
}

UBool 
ICUService::isDefault() ثابت 
{
    أرجع شمار_Factories() == 0;
}

ICUService_کلید* 
ICUService::create_کلید(ثابت UnicodeString* id, UError_کد& وضعیت) ثابت 
{
    أرجع (U_FAILURE(وضعیت) || id == اشاره_تهی) ? اشاره_تهی : جديد ICUService_کلید(*id);
}

فراغ 
ICUService::clearCaches() 
{
    // callers synchronize before use
    ++timestamp;
    حذف dnCache;
    dnCache = اشاره_تهی;
    حذف idCache;
    idCache = اشاره_تهی;
    حذف serviceCache; serviceCache = اشاره_تهی;
}

فراغ 
ICUService::clearServiceCache() 
{
    // callers synchronize before use
    حذف serviceCache; serviceCache = اشاره_تهی;
}

UBool 
ICUService::میپذیرد_Listener(ثابت EventListener& l) ثابت 
{
    أرجع dynamic_cast<ثابت ServiceListener*>(&l) != اشاره_تهی;
}

فراغ 
ICUService::notifyListener(EventListener& l) ثابت 
{
    (static_cast<ServiceListener&>(l)).service_تغییریافته(*هذا);
}

UnicodeString&
ICUService::getName(UnicodeString& نتیجه) ثابت 
{
    أرجع نتیجه.append(name);
}

int32_t 
ICUService::شمار_Factories() ثابت 
{
    أرجع factories == اشاره_تهی ? 0 : factories->size();
}

int32_t
ICUService::getTimestamp() ثابت
{
    أرجع timestamp;
}

U_NAMESPACE_انتها

/* UCONFIG_NO_SERVICE */
#پایان_اگر
