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

#وصف_رد SERVICE_REFCOUNT

// in case we use the refcount stuff

U_NAMESPACE_BEGIN

/*
******************************************************************
*/

ثابت char16_t ICUService_ڪنجي::اڳياڙي_DELIMITER = 0x002F;   /* '/' */

ICUService_ڪنجي::ICUService_ڪنجي(ثابت UnicodeString& id) 
: _id(id) {
}

ICUService_ڪنجي::~ICUService_ڪنجي() 
{
}

ثابت UnicodeString& 
ICUService_ڪنجي::getID() ثابت 
{
    أرجع _id;
}

UnicodeString& 
ICUService_ڪنجي::canonicalID(UnicodeString& نتيجو) ثابت 
{
    أرجع نتيجو.append(_id);
}

UnicodeString& 
ICUService_ڪنجي::currentID(UnicodeString& نتيجو) ثابت 
{
    أرجع canonicalID(نتيجو);
}

UnicodeString& 
ICUService_ڪنجي::currentDescriptor(UnicodeString& نتيجو) ثابت 
{
    اڳياڙي(نتيجو);
    نتيجو.append(اڳياڙي_DELIMITER);
    أرجع currentID(نتيجو);
}

UBool 
ICUService_ڪنجي::fallback() 
{
    أرجع خطأ;
}

UBool 
ICUService_ڪنجي::is_Fallback_جو(ثابت UnicodeString& id) ثابت 
{
    أرجع id == _id;
}

UnicodeString& 
ICUService_ڪنجي::اڳياڙي(UnicodeString& نتيجو) ثابت 
{
    أرجع نتيجو;
}

UnicodeString& 
ICUService_ڪنجي::parse_اڳياڙي(UnicodeString& نتيجو) 
{
    int32_t n = نتيجو.اشاريو_جو(اڳياڙي_DELIMITER);
    إذا (n < 0) {
        n = 0;
    }
    نتيجو.remove(n);
    أرجع نتيجو;
}

UnicodeString& 
ICUService_ڪنجي::parseSuffix(UnicodeString& نتيجو) 
{
    int32_t n = نتيجو.اشاريو_جو(اڳياڙي_DELIMITER);
    إذا (n >= 0) {
        نتيجو.remove(0, n+1);
    }
    أرجع نتيجو;
}

#جيڪڏهن_وصف_ٿيل SERVICE_DEBUG
UnicodeString& 
ICUService_ڪنجي::debug(UnicodeString& نتيجو) ثابت 
{
    debugClass(نتيجو);
    نتيجو.append((UnicodeString)" id: ");
    نتيجو.append(_id);
    أرجع نتيجو;
}

UnicodeString& 
ICUService_ڪنجي::debugClass(UnicodeString& نتيجو) ثابت 
{
    أرجع نتيجو.append((UnicodeString)"ICUServiceKey");
}
#جيڪڏهن_پورو

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ICUService_ڪنجي)

/*
******************************************************************
*/

ICUServiceFactory::~ICUServiceFactory() {}

SimpleFactory::SimpleFactory(UObject* instance_منزل_Adopt, ثابت UnicodeString& id, UBool visible) 
: _instance(instance_منزل_Adopt), _id(id), _visible(visible)
{
}

SimpleFactory::~SimpleFactory() 
{
    ميٽيو _instance;
}

UObject* 
SimpleFactory::create(ثابت ICUService_ڪنجي& ڪنجي, ثابت ICUService* service, UError_ڪوڊ& حالت) ثابت 
{
    إذا (U_SUCCESS(حالت)) {
        UnicodeString عارضي;
        إذا (_id == ڪنجي.currentID(عارضي)) {
            أرجع service->cloneInstance(_instance); 
        }
    }
    أرجع خالي_اشارو;
}

فراغ 
SimpleFactory::updateVisibleIDs(Hashtable& نتيجو, UError_ڪوڊ& حالت) ثابت 
{
    إذا (_visible) {
        نتيجو.put(_id, (فراغ*)هذا, حالت); // cast away const
    } وإلا {
        نتيجو.remove(_id);
    }
}

UnicodeString& 
SimpleFactory::getDisplayName(ثابت UnicodeString& id, ثابت Locale& /* locale */, UnicodeString& نتيجو) ثابت 
{
    إذا (_visible && _id == id) {
        نتيجو = _id;
    } وإلا {
        نتيجو.سيٽ_منزل_Bogus();
    }
    أرجع نتيجو;
}

#جيڪڏهن_وصف_ٿيل SERVICE_DEBUG
UnicodeString& 
SimpleFactory::debug(UnicodeString& منزل_Append_منزل) ثابت 
{
    debugClass(منزل_Append_منزل);
    منزل_Append_منزل.append((UnicodeString)" id: ");
    منزل_Append_منزل.append(_id);
    منزل_Append_منزل.append((UnicodeString)", visible: ");
    منزل_Append_منزل.append(_visible ? (UnicodeString)"T" : (UnicodeString)"F");
    أرجع منزل_Append_منزل;
}

UnicodeString& 
SimpleFactory::debugClass(UnicodeString& منزل_Append_منزل) ثابت 
{
    أرجع منزل_Append_منزل.append((UnicodeString)"SimpleFactory");
}
#جيڪڏهن_پورو

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
صنف Cache_داخلا : عام UMemory {
خاص:
    int32_t refcount;

عام:
    UnicodeString actualDescriptor;
    UObject* service;

    /**
    * Releases a reference to the shared resource.
    */
    ~Cache_داخلا() {
        ميٽيو service;
    }

    Cache_داخلا(ثابت UnicodeString& _actualDescriptor, UObject* _service) 
        : refcount(1), actualDescriptor(_actualDescriptor), service(_service) {
    }

    /**
    * Instantiation creates an initial reference, so don't call this
    * unless you're creating a new pointer to this.  Management of
    * that pointer will have to know how to deal with refcounts.  
    * Return true if the resource has not already been released.
    */
    Cache_داخلا* ref() {
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
    Cache_داخلا* unref() {
        إذا ((--refcount) == 0) {
            ميٽيو هذا;
            أرجع خالي_اشارو;
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
    U_NAMESPACE_USE ((Cache_داخلا*)obj)->unref();
}

U_CDECL_پڇاڙي

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
                   UError_ڪوڊ& حالت)
{
    إذا (U_SUCCESS(حالت)) {
        StringPair* sp = جديد StringPair(displayName, id);
        إذا (sp == خالي_اشارو || sp->isBogus()) {
            حالت = U_يادگيري_ورهاست_ERROR;
            ميٽيو sp;
            أرجع خالي_اشارو;
        }
        أرجع sp;
    }
    أرجع خالي_اشارو;
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
    U_NAMESPACE_USE ميٽيو (StringPair*) obj;
}
U_CDECL_پڇاڙي

/*
******************************************************************
*/

ساكن UMutex تالو;

ICUService::ICUService()
: name()
, timestamp(0)
, factories(خالي_اشارو)
, serviceCache(خالي_اشارو)
, idCache(خالي_اشارو)
, dnCache(خالي_اشارو)
{
}

ICUService::ICUService(ثابت UnicodeString& newName) 
: name(newName)
, timestamp(0)
, factories(خالي_اشارو)
, serviceCache(خالي_اشارو)
, idCache(خالي_اشارو)
, dnCache(خالي_اشارو)
{
}

ICUService::~ICUService()
{
    {
        Mutex mutex(&تالو);
        clearCaches();
        ميٽيو factories;
        factories = خالي_اشارو;
    }
}

UObject* 
ICUService::get(ثابت UnicodeString& descriptor, UError_ڪوڊ& حالت) ثابت 
{
    أرجع get(descriptor, خالي_اشارو, حالت);
}

UObject* 
ICUService::get(ثابت UnicodeString& descriptor, UnicodeString* actualReturn, UError_ڪوڊ& حالت) ثابت 
{
    UObject* نتيجو = خالي_اشارو;
    ICUService_ڪنجي* ڪنجي = create_ڪنجي(&descriptor, حالت);
    إذا (ڪنجي) {
        نتيجو = get_ڪنجي(*ڪنجي, actualReturn, حالت);
        ميٽيو ڪنجي;
    }
    أرجع نتيجو;
}

UObject* 
ICUService::get_ڪنجي(ICUService_ڪنجي& ڪنجي, UError_ڪوڊ& حالت) ثابت 
{
    أرجع get_ڪنجي(ڪنجي, خالي_اشارو, حالت);
}

// this is a vector that subclasses of ICUService can override to further customize the result object
// before returning it.  All other public get functions should call this one.

UObject* 
ICUService::get_ڪنجي(ICUService_ڪنجي& ڪنجي, UnicodeString* actualReturn, UError_ڪوڊ& حالت) ثابت 
{
    أرجع get_ڪنجي(ڪنجي, actualReturn, خالي_اشارو, حالت);
}

// make it possible to call reentrantly on systems that don't have reentrant mutexes.
// we can use this simple approach since we know the situation where we're calling
// reentrantly even without knowing the thread.
صنف XMutex : عام UMemory {
عام:
    مضمن XMutex(UMutex *mutex, UBool reentering) 
        : fMutex(mutex)
        , f_سرگرم(!reentering) 
    {
        إذا (f_سرگرم) umtx_تالو(fMutex);
    }
    مضمن ~XMutex() {
        إذا (f_سرگرم) umtx_unlock(fMutex);
    }

خاص:
    UMutex  *fMutex;
    UBool f_سرگرم;
};

// called only by factories, treat as private
UObject* 
ICUService::get_ڪنجي(ICUService_ڪنجي& ڪنجي, UnicodeString* actualReturn, ثابت ICUServiceFactory* factory, UError_ڪوڊ& حالت) ثابت 
{
    إذا (U_FAILURE(حالت)) {
        أرجع خالي_اشارو;
    }

    إذا (isDefault()) {
        أرجع handleDefault(ڪنجي, actualReturn, حالت);
    }

    ICUService* ncthis = const_cast<ICUService*>(هذا); // cast away semantic const

    Cache_داخلا* نتيجو = خالي_اشارو;
    {
        // The factory list can't be modified until we're done, 
        // otherwise we might update the cache with an invalid result.
        // The cache has to stay in synch with the factory list.
        // ICU doesn't have monitors so we can't use rw locks, so 
        // we single-thread everything using this service, for now.

        // if factory is not null, we're calling from within the mutex,
        // and since some unix machines don't have reentrant mutexes we
        // need to make sure not to try to lock it again.
        XMutex mutex(&تالو, factory != خالي_اشارو);

        إذا (serviceCache == خالي_اشارو) {
            ncthis->serviceCache = جديد Hashtable(حالت);
            إذا (ncthis->serviceCache == خالي_اشارو) {
                حالت = U_يادگيري_ورهاست_ERROR;
                أرجع خالي_اشارو;
            }
            إذا (U_FAILURE(حالت)) {
                ميٽيو serviceCache;
                أرجع خالي_اشارو;
            }
            serviceCache->سيٽ_قدر_Deleter(cacheDeleter);
        }

        UnicodeString currentDescriptor;
        Local_اشارو<UVector> cacheDescriptorList;
        UBool put_اندريون_Cache = خطأ;

        int32_t شروعات_اشاريو = 0;
        int32_t حد_بندي = factories->size();
        UBool cache_نتيجو = سچ;

        إذا (factory != خالي_اشارو) {
            لكل (int32_t i = 0; i < حد_بندي; ++i) {
                إذا (factory == static_cast<ثابت ICUServiceFactory*>(factories->elementAt(i))) {
                    شروعات_اشاريو = i + 1;
                    اكسر;
                }
            }
            إذا (شروعات_اشاريو == 0) {
                // throw new InternalError("Factory " + factory + "not registered with service: " + this);
                حالت = U_ILLEGAL_دليل_ERROR;
                أرجع خالي_اشارو;
            }
            cache_نتيجو = خطأ;
        }

        افعل {
            currentDescriptor.remove();
            ڪنجي.currentDescriptor(currentDescriptor);
            نتيجو = static_cast<Cache_داخلا*>(serviceCache->get(currentDescriptor));
            إذا (نتيجو != خالي_اشارو) {
                اكسر;
            }

            // first test of cache failed, so we'll have to update
            // the cache if we eventually succeed-- that is, if we're 
            // going to update the cache at all.
            put_اندريون_Cache = سچ;

            int32_t اشاريو = شروعات_اشاريو;
            طالما (اشاريو < حد_بندي) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(اشاريو++));
                Local_اشارو<UObject> service(f->create(ڪنجي, هذا, حالت));
                إذا (U_FAILURE(حالت)) {
                    أرجع خالي_اشارو;
                }
                إذا (service.isValid()) {
                    نتيجو = جديد Cache_داخلا(currentDescriptor, service.getAlias());
                    إذا (نتيجو == خالي_اشارو) {
                        حالت = U_يادگيري_ورهاست_ERROR;
                        أرجع خالي_اشارو;
                    }
                    service.orphan(); // result now owns service.

                    اذهب outer_پڇاڙي;
                }
            }

            // prepare to load the cache with all additional ids that 
            // will resolve to result, assuming we'll succeed.  We
            // don't want to keep querying on an id that's going to
            // fallback to the one that succeeded, we want to hit the
            // cache the first time next goaround.
            إذا (cacheDescriptorList.isNull()) {
                cacheDescriptorList.adopt_Instead_And_Check_Error_ڪوڊ(جديد UVector(uprv_deleteUObject, خالي_اشارو, 5, حالت), حالت);
                إذا (U_FAILURE(حالت)) {
                    أرجع خالي_اشارو;
                }
            }

            Local_اشارو<UnicodeString> id_منزل_Cache(جديد UnicodeString(currentDescriptor), حالت);
            إذا (U_FAILURE(حالت)) {
                أرجع خالي_اشارو;
            }
            إذا (id_منزل_Cache->isBogus()) {
                حالت = U_يادگيري_ورهاست_ERROR;
                أرجع خالي_اشارو;
            }
            cacheDescriptorList->adoptElement(id_منزل_Cache.orphan(), حالت);
            إذا (U_FAILURE(حالت)) {
                أرجع خالي_اشارو;
            }
        } طالما (ڪنجي.fallback());
outer_پڇاڙي:

        إذا (نتيجو != خالي_اشارو) {
            إذا (put_اندريون_Cache && cache_نتيجو) {
                serviceCache->put(نتيجو->actualDescriptor, نتيجو, حالت);
                إذا (U_FAILURE(حالت)) {
                    أرجع خالي_اشارو;
                }

                إذا (cacheDescriptorList.isValid()) {
                    لكل (int32_t i = cacheDescriptorList->size(); --i >= 0;) {
                        UnicodeString* desc = static_cast<UnicodeString*>(cacheDescriptorList->elementAt(i));

                        serviceCache->put(*desc, نتيجو, حالت);
                        إذا (U_FAILURE(حالت)) {
                            أرجع خالي_اشارو;
                        }

                        نتيجو->ref();
                        cacheDescriptorList->removeElementAt(i);
                    }
                }
            }

            إذا (actualReturn != خالي_اشارو) {
                // strip null prefix
                إذا (نتيجو->actualDescriptor.اشاريو_جو(static_cast<char16_t>(0x2f)) == 0) { // U+002f=slash (/)
                    actualReturn->remove();
                    actualReturn->append(نتيجو->actualDescriptor, 
                        1, 
                        نتيجو->actualDescriptor.length() - 1);
                } وإلا {
                    *actualReturn = نتيجو->actualDescriptor;
                }

                إذا (actualReturn->isBogus()) {
                    حالت = U_يادگيري_ورهاست_ERROR;
                    ميٽيو نتيجو;
                    أرجع خالي_اشارو;
                }
            }

            UObject* service = cloneInstance(نتيجو->service);
            إذا (put_اندريون_Cache && !cache_نتيجو) {
                ميٽيو نتيجو;
            }
            أرجع service;
        }
    }

    أرجع handleDefault(ڪنجي, actualReturn, حالت);
}

UObject* 
ICUService::handleDefault(ثابت ICUService_ڪنجي& /*ڪنجي*/, UnicodeString* /* actualIDReturn */, UError_ڪوڊ& /* status */) ثابت 
{
    أرجع خالي_اشارو;
}

UVector& 
ICUService::getVisibleIDs(UVector& نتيجو, UError_ڪوڊ& حالت) ثابت {
    أرجع getVisibleIDs(نتيجو, خالي_اشارو, حالت);
}

UVector& 
ICUService::getVisibleIDs(UVector& نتيجو, ثابت UnicodeString* ميلاپ_ID, UError_ڪوڊ& حالت) ثابت 
{
    نتيجو.removeAllElements();

    إذا (U_FAILURE(حالت)) {
        أرجع نتيجو;
    }
    UObjectDeleter *savedDeleter = نتيجو.سيٽ_Deleter(uprv_deleteUObject);

    {
        Mutex mutex(&تالو);
        ثابت Hashtable* map = getVisibleIDMap(حالت);
        إذا (map != خالي_اشارو) {
            ICUService_ڪنجي* fallback_ڪنجي = create_ڪنجي(ميلاپ_ID, حالت);

            لكل (int32_t pos = UHASH_پهريون; U_SUCCESS(حالت); ) {
                ثابت UHashElement* e = map->ايندڙ_Element(pos);
                إذا (e == خالي_اشارو) {
                    اكسر;
                }

                ثابت UnicodeString* id = static_cast<ثابت UnicodeString*>(e->ڪنجي.اشارو);
                إذا (fallback_ڪنجي != خالي_اشارو) {
                    إذا (!fallback_ڪنجي->is_Fallback_جو(*id)) {
                        استمر;
                    }
                }

                Local_اشارو<UnicodeString> idClone(id->clone(), حالت);
                نتيجو.adoptElement(idClone.orphan(), حالت);
            }
            ميٽيو fallback_ڪنجي;
        }
    }
    إذا (U_FAILURE(حالت)) {
        نتيجو.removeAllElements();
    }
    نتيجو.سيٽ_Deleter(savedDeleter);
    أرجع نتيجو;
}

ثابت Hashtable* 
ICUService::getVisibleIDMap(UError_ڪوڊ& حالت) ثابت {
    إذا (U_FAILURE(حالت)) أرجع خالي_اشارو;

    // must only be called when lock is already held

    ICUService* ncthis = const_cast<ICUService*>(هذا); // cast away semantic const
    إذا (idCache == خالي_اشارو) {
        ncthis->idCache = جديد Hashtable(حالت);
        إذا (idCache == خالي_اشارو) {
            حالت = U_يادگيري_ورهاست_ERROR;
        } وإلا إذا (factories != خالي_اشارو) {
            لكل (int32_t pos = factories->size(); --pos >= 0;) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(pos));
                f->updateVisibleIDs(*idCache, حالت);
            }
            إذا (U_FAILURE(حالت)) {
                ميٽيو idCache;
                ncthis->idCache = خالي_اشارو;
            }
        }
    }

    أرجع idCache;
}


UnicodeString& 
ICUService::getDisplayName(ثابت UnicodeString& id, UnicodeString& نتيجو) ثابت 
{
    أرجع getDisplayName(id, نتيجو, Locale::getDefault());
}

UnicodeString& 
ICUService::getDisplayName(ثابت UnicodeString& id, UnicodeString& نتيجو, ثابت Locale& locale) ثابت 
{
    {
        UError_ڪوڊ حالت = U_ZERO_ERROR;
        Mutex mutex(&تالو);
        ثابت Hashtable* map = getVisibleIDMap(حالت);
        إذا (map != خالي_اشارو) {
            ICUServiceFactory* f = static_cast<ICUServiceFactory*>(map->get(id));
            إذا (f != خالي_اشارو) {
                f->getDisplayName(id, locale, نتيجو);
                أرجع نتيجو;
            }

            // fallback
            حالت = U_ZERO_ERROR;
            ICUService_ڪنجي* fallback_ڪنجي = create_ڪنجي(&id, حالت);
            طالما (fallback_ڪنجي != خالي_اشارو && fallback_ڪنجي->fallback()) {
                UnicodeString us;
                fallback_ڪنجي->currentID(us);
                f = static_cast<ICUServiceFactory*>(map->get(us));
                إذا (f != خالي_اشارو) {
                    f->getDisplayName(id, locale, نتيجو);
                    ميٽيو fallback_ڪنجي;
                    أرجع نتيجو;
                }
            }
            ميٽيو fallback_ڪنجي;
        }
    }
    نتيجو.سيٽ_منزل_Bogus();
    أرجع نتيجو;
}

UVector& 
ICUService::get_Display_نالا(UVector& نتيجو, UError_ڪوڊ& حالت) ثابت 
{
    أرجع get_Display_نالا(نتيجو, Locale::getDefault(), خالي_اشارو, حالت);
}


UVector& 
ICUService::get_Display_نالا(UVector& نتيجو, ثابت Locale& locale, UError_ڪوڊ& حالت) ثابت 
{
    أرجع get_Display_نالا(نتيجو, locale, خالي_اشارو, حالت);
}

UVector& 
ICUService::get_Display_نالا(UVector& نتيجو, 
                            ثابت Locale& locale, 
                            ثابت UnicodeString* ميلاپ_ID, 
                            UError_ڪوڊ& حالت) ثابت 
{
    نتيجو.removeAllElements();
    نتيجو.سيٽ_Deleter(userv_deleteStringPair);
    إذا (U_SUCCESS(حالت)) {
        ICUService* ncthis = const_cast<ICUService*>(هذا); // cast away semantic const
        Mutex mutex(&تالو);

        إذا (dnCache != خالي_اشارو && dnCache->locale != locale) {
            ميٽيو dnCache;
            ncthis->dnCache = خالي_اشارو;
        }

        إذا (dnCache == خالي_اشارو) {
            ثابت Hashtable* m = getVisibleIDMap(حالت);
            إذا (U_FAILURE(حالت)) {
                أرجع نتيجو;
            }
            ncthis->dnCache = جديد DNCache(locale); 
            إذا (dnCache == خالي_اشارو) {
                حالت = U_يادگيري_ورهاست_ERROR;
                أرجع نتيجو;
            }

            int32_t pos = UHASH_پهريون;
            ثابت UHashElement* داخلا_2 = خالي_اشارو;
            طالما ((داخلا_2 = m->ايندڙ_Element(pos)) != خالي_اشارو) {
                ثابت UnicodeString* id = static_cast<ثابت UnicodeString*>(داخلا_2->ڪنجي.اشارو);
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(داخلا_2->value.اشارو);
                UnicodeString dname;
                f->getDisplayName(*id, locale, dname);
                إذا (dname.isBogus()) {
                    حالت = U_يادگيري_ورهاست_ERROR;
                } وإلا {
                    dnCache->cache.put(dname, (فراغ*)id, حالت); // share pointer with visibleIDMap
                    إذا (U_SUCCESS(حالت)) {
                        استمر;
                    }
                }
                ميٽيو dnCache;
                ncthis->dnCache = خالي_اشارو;
                أرجع نتيجو;
            }
        }
    }

    ICUService_ڪنجي* ميلاپ_ڪنجي = create_ڪنجي(ميلاپ_ID, حالت);
    /* To ensure that all elements in the hashtable are iterated, set pos to -1.
     * nextElement(pos) will skip the position at pos and begin the iteration
     * at the next position, which in this case will be 0.
     */
    int32_t pos = UHASH_پهريون; 
    ثابت UHashElement *داخلا_2 = خالي_اشارو;
    طالما ((داخلا_2 = dnCache->cache.ايندڙ_Element(pos)) != خالي_اشارو) {
        ثابت UnicodeString* id = static_cast<ثابت UnicodeString*>(داخلا_2->value.اشارو);
        إذا (ميلاپ_ڪنجي != خالي_اشارو && !ميلاپ_ڪنجي->is_Fallback_جو(*id)) {
            استمر;
        }
        ثابت UnicodeString* dn = static_cast<ثابت UnicodeString*>(داخلا_2->ڪنجي.اشارو);
        StringPair* sp = StringPair::create(*id, *dn, حالت);
        نتيجو.adoptElement(sp, حالت);
        إذا (U_FAILURE(حالت)) {
            نتيجو.removeAllElements();
            اكسر;
        }
    }
    ميٽيو ميلاپ_ڪنجي;

    أرجع نتيجو;
}

URegistry_ڪنجي
ICUService::رجسٽر_Instance(UObject* obj_منزل_Adopt, ثابت UnicodeString& id, UError_ڪوڊ& حالت) 
{
    أرجع رجسٽر_Instance(obj_منزل_Adopt, id, سچ, حالت);
}

URegistry_ڪنجي
ICUService::رجسٽر_Instance(UObject* obj_منزل_Adopt, ثابت UnicodeString& id, UBool visible, UError_ڪوڊ& حالت) 
{
    ICUService_ڪنجي* ڪنجي = create_ڪنجي(&id, حالت);
    إذا (ڪنجي != خالي_اشارو) {
        UnicodeString canonicalID;
        ڪنجي->canonicalID(canonicalID);
        ميٽيو ڪنجي;

        ICUServiceFactory* f = createSimpleFactory(obj_منزل_Adopt, canonicalID, visible, حالت);
        إذا (f != خالي_اشارو) {
            أرجع رجسٽر_Factory(f, حالت);
        }
    }
    ميٽيو obj_منزل_Adopt;
    أرجع خالي_اشارو;
}

ICUServiceFactory* 
ICUService::createSimpleFactory(UObject* obj_منزل_Adopt, ثابت UnicodeString& id, UBool visible, UError_ڪوڊ& حالت)
{
    إذا (U_SUCCESS(حالت)) {
        إذا ((obj_منزل_Adopt != خالي_اشارو) && (!id.isBogus())) {
            أرجع جديد SimpleFactory(obj_منزل_Adopt, id, visible);
        }
        حالت = U_ILLEGAL_دليل_ERROR;
    }
    أرجع خالي_اشارو;
}

URegistry_ڪنجي
ICUService::رجسٽر_Factory(ICUServiceFactory* factory_منزل_Adopt, UError_ڪوڊ& حالت)
{
    Local_اشارو<ICUServiceFactory>lp_Factory_منزل_Adopt(factory_منزل_Adopt);
    إذا (U_FAILURE(حالت) || factory_منزل_Adopt == خالي_اشارو) {
        أرجع خالي_اشارو;
    }
    {
        Mutex mutex(&تالو);

        إذا (factories == خالي_اشارو) {
            Local_اشارو<UVector> lpFactories(جديد UVector(uprv_deleteUObject, خالي_اشارو, حالت), حالت);
            إذا (U_FAILURE(حالت)) {
                أرجع خالي_اشارو;
            }
            factories = lpFactories.orphan();
        }
        factories->وجهو_Element_At(lp_Factory_منزل_Adopt.orphan(), 0, حالت);
        إذا (U_SUCCESS(حالت)) {
            clearCaches();
        }
    }   // Close of mutex lock block.

    إذا (U_SUCCESS(حالت)) {
        notify_بدليو();
        أرجع (URegistry_ڪنجي)factory_منزل_Adopt;
    } وإلا {
        أرجع خالي_اشارو;
    }
}

UBool 
ICUService::unregister(URegistry_ڪنجي rkey, UError_ڪوڊ& حالت) 
{
    ICUServiceFactory *factory = (ICUServiceFactory*)rkey;
    UBool نتيجو = خطأ;
    إذا (factory != خالي_اشارو && factories != خالي_اشارو) {
        Mutex mutex(&تالو);

        إذا (factories->removeElement(factory)) {
            clearCaches();
            نتيجو = سچ;
        } وإلا {
            حالت = U_ILLEGAL_دليل_ERROR;
            ميٽيو factory;
        }
    }
    إذا (نتيجو) {
        notify_بدليو();
    }
    أرجع نتيجو;
}

فراغ 
ICUService::reset() 
{
    {
        Mutex mutex(&تالو);
        reInitializeFactories();
        clearCaches();
    }
    notify_بدليو();
}

فراغ 
ICUService::reInitializeFactories() 
{
    إذا (factories != خالي_اشارو) {
        factories->removeAllElements();
    }
}

UBool 
ICUService::isDefault() ثابت 
{
    أرجع ڳڻپ_Factories() == 0;
}

ICUService_ڪنجي* 
ICUService::create_ڪنجي(ثابت UnicodeString* id, UError_ڪوڊ& حالت) ثابت 
{
    أرجع (U_FAILURE(حالت) || id == خالي_اشارو) ? خالي_اشارو : جديد ICUService_ڪنجي(*id);
}

فراغ 
ICUService::clearCaches() 
{
    // callers synchronize before use
    ++timestamp;
    ميٽيو dnCache;
    dnCache = خالي_اشارو;
    ميٽيو idCache;
    idCache = خالي_اشارو;
    ميٽيو serviceCache; serviceCache = خالي_اشارو;
}

فراغ 
ICUService::clearServiceCache() 
{
    // callers synchronize before use
    ميٽيو serviceCache; serviceCache = خالي_اشارو;
}

UBool 
ICUService::قبولي_Listener(ثابت EventListener& l) ثابت 
{
    أرجع dynamic_cast<ثابت ServiceListener*>(&l) != خالي_اشارو;
}

فراغ 
ICUService::notifyListener(EventListener& l) ثابت 
{
    (static_cast<ServiceListener&>(l)).service_بدليو(*هذا);
}

UnicodeString&
ICUService::getName(UnicodeString& نتيجو) ثابت 
{
    أرجع نتيجو.append(name);
}

int32_t 
ICUService::ڳڻپ_Factories() ثابت 
{
    أرجع factories == خالي_اشارو ? 0 : factories->size();
}

int32_t
ICUService::getTimestamp() ثابت
{
    أرجع timestamp;
}

U_NAMESPACE_پڇاڙي

/* UCONFIG_NO_SERVICE */
#جيڪڏهن_پورو
