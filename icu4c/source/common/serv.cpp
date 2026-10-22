// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
*******************************************************************************
* Copyright (C) 2001-2014, International Business Machines Corporation.
* All Rights Reserved.
*******************************************************************************
*/

#شامل "unicode/utypes.h"
#شامل "unicode/localpointer.h"

#اگر !UCONFIG_NO_SERVICE

#شامل "serv.h"
#شامل "umutex.h"

#تعریف_منسوخ SERVICE_REFCOUNT

// in case we use the refcount stuff

U_NAMESPACE_BEGIN

/*
******************************************************************
*/

ثابت char16_t ICUService_کلید::سابقہ_DELIMITER = 0x002F;   /* '/' */

ICUService_کلید::ICUService_کلید(ثابت UnicodeString& id) 
: _id(id) {
}

ICUService_کلید::~ICUService_کلید() 
{
}

ثابت UnicodeString& 
ICUService_کلید::getID() ثابت 
{
    واپس _id;
}

UnicodeString& 
ICUService_کلید::canonicalID(UnicodeString& نتیجہ) ثابت 
{
    واپس نتیجہ.append(_id);
}

UnicodeString& 
ICUService_کلید::currentID(UnicodeString& نتیجہ) ثابت 
{
    واپس canonicalID(نتیجہ);
}

UnicodeString& 
ICUService_کلید::currentDescriptor(UnicodeString& نتیجہ) ثابت 
{
    سابقہ(نتیجہ);
    نتیجہ.append(سابقہ_DELIMITER);
    واپس currentID(نتیجہ);
}

UBool 
ICUService_کلید::fallback() 
{
    واپس جھوٹ;
}

UBool 
ICUService_کلید::is_Fallback_کا(ثابت UnicodeString& id) ثابت 
{
    واپس id == _id;
}

UnicodeString& 
ICUService_کلید::سابقہ(UnicodeString& نتیجہ) ثابت 
{
    واپس نتیجہ;
}

UnicodeString& 
ICUService_کلید::parse_سابقہ(UnicodeString& نتیجہ) 
{
    int32_t n = نتیجہ.اشاریہ_کا(سابقہ_DELIMITER);
    اگر (n < 0) {
        n = 0;
    }
    نتیجہ.remove(n);
    واپس نتیجہ;
}

UnicodeString& 
ICUService_کلید::parseSuffix(UnicodeString& نتیجہ) 
{
    int32_t n = نتیجہ.اشاریہ_کا(سابقہ_DELIMITER);
    اگر (n >= 0) {
        نتیجہ.remove(0, n+1);
    }
    واپس نتیجہ;
}

#اگر_تعریف_شدہ SERVICE_DEBUG
UnicodeString& 
ICUService_کلید::debug(UnicodeString& نتیجہ) ثابت 
{
    debugClass(نتیجہ);
    نتیجہ.append((UnicodeString)" id: ");
    نتیجہ.append(_id);
    واپس نتیجہ;
}

UnicodeString& 
ICUService_کلید::debugClass(UnicodeString& نتیجہ) ثابت 
{
    واپس نتیجہ.append((UnicodeString)"ICUServiceKey");
}
#اگر_ختم

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
SimpleFactory::create(ثابت ICUService_کلید& کلید, ثابت ICUService* service, UError_رمز& حالت_2) ثابت 
{
    اگر (U_SUCCESS(حالت_2)) {
        UnicodeString عارضی;
        اگر (_id == کلید.currentID(عارضی)) {
            واپس service->cloneInstance(_instance); 
        }
    }
    واپس خالی_اشاریہ;
}

خالی 
SimpleFactory::updateVisibleIDs(Hashtable& نتیجہ, UError_رمز& حالت_2) ثابت 
{
    اگر (_visible) {
        نتیجہ.put(_id, (خالی*)یہ, حالت_2); // cast away const
    } ورنہ {
        نتیجہ.remove(_id);
    }
}

UnicodeString& 
SimpleFactory::getDisplayName(ثابت UnicodeString& id, ثابت Locale& /* locale */, UnicodeString& نتیجہ) ثابت 
{
    اگر (_visible && _id == id) {
        نتیجہ = _id;
    } ورنہ {
        نتیجہ.سیٹ_مقصد_Bogus();
    }
    واپس نتیجہ;
}

#اگر_تعریف_شدہ SERVICE_DEBUG
UnicodeString& 
SimpleFactory::debug(UnicodeString& مقصد_Append_مقصد) ثابت 
{
    debugClass(مقصد_Append_مقصد);
    مقصد_Append_مقصد.append((UnicodeString)" id: ");
    مقصد_Append_مقصد.append(_id);
    مقصد_Append_مقصد.append((UnicodeString)", visible: ");
    مقصد_Append_مقصد.append(_visible ? (UnicodeString)"T" : (UnicodeString)"F");
    واپس مقصد_Append_مقصد;
}

UnicodeString& 
SimpleFactory::debugClass(UnicodeString& مقصد_Append_مقصد) ثابت 
{
    واپس مقصد_Append_مقصد.append((UnicodeString)"SimpleFactory");
}
#اگر_ختم

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
جماعت Cache_اندراج : عوامی UMemory {
نجی:
    int32_t refcount;

عوامی:
    UnicodeString actualDescriptor;
    UObject* service;

    /**
    * Releases a reference to the shared resource.
    */
    ~Cache_اندراج() {
        حذف service;
    }

    Cache_اندراج(ثابت UnicodeString& _actualDescriptor, UObject* _service) 
        : refcount(1), actualDescriptor(_actualDescriptor), service(_service) {
    }

    /**
    * Instantiation creates an initial reference, so don't call this
    * unless you're creating a new pointer to this.  Management of
    * that pointer will have to know how to deal with refcounts.  
    * Return true if the resource has not already been released.
    */
    Cache_اندراج* ref() {
        ++refcount;
        واپس یہ;
    }

    /**
    * Destructions removes a reference, so don't call this unless
    * you're removing pointer to this somewhere.  Management of that
    * pointer will have to know how to deal with refcounts.  Once
    * the refcount drops to zero, the resource is released.  Return
    * false if the resource has been released.
    */
    Cache_اندراج* unref() {
        اگر ((--refcount) == 0) {
            حذف یہ;
            واپس خالی_اشاریہ;
        }
        واپس یہ;
    }

    /**
    * Return true if there is at least one reference to this and the
    * resource has not been released.
    */
    UBool isShared() ثابت {
        واپس refcount > 1;
    }
};

// Deleter for serviceCache
U_CDECL_BEGIN
ساکن خالی U_CALLCONV
cacheDeleter(خالی* obj) {
    U_NAMESPACE_USE ((Cache_اندراج*)obj)->unref();
}

U_CDECL_انتہا

/*
******************************************************************
*/

جماعت DNCache : عوامی UMemory {
عوامی:
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
                   UError_رمز& حالت_2)
{
    اگر (U_SUCCESS(حالت_2)) {
        StringPair* sp = نیا StringPair(displayName, id);
        اگر (sp == خالی_اشاریہ || sp->isBogus()) {
            حالت_2 = U_حافظہ_تخصیص_ERROR;
            حذف sp;
            واپس خالی_اشاریہ;
        }
        واپس sp;
    }
    واپس خالی_اشاریہ;
}

UBool 
StringPair::isBogus() ثابت {
    واپس displayName.isBogus() || id.isBogus();
}

StringPair::StringPair(ثابت UnicodeString& _displayName, 
                       ثابت UnicodeString& _id)
: displayName(_displayName)
, id(_id)
{
}

U_CDECL_BEGIN
ساکن خالی U_CALLCONV
userv_deleteStringPair(خالی *obj) {
    U_NAMESPACE_USE حذف (StringPair*) obj;
}
U_CDECL_انتہا

/*
******************************************************************
*/

ساکن UMutex تالا;

ICUService::ICUService()
: name()
, timestamp(0)
, factories(خالی_اشاریہ)
, serviceCache(خالی_اشاریہ)
, idCache(خالی_اشاریہ)
, dnCache(خالی_اشاریہ)
{
}

ICUService::ICUService(ثابت UnicodeString& newName) 
: name(newName)
, timestamp(0)
, factories(خالی_اشاریہ)
, serviceCache(خالی_اشاریہ)
, idCache(خالی_اشاریہ)
, dnCache(خالی_اشاریہ)
{
}

ICUService::~ICUService()
{
    {
        Mutex mutex(&تالا);
        clearCaches();
        حذف factories;
        factories = خالی_اشاریہ;
    }
}

UObject* 
ICUService::get(ثابت UnicodeString& descriptor, UError_رمز& حالت_2) ثابت 
{
    واپس get(descriptor, خالی_اشاریہ, حالت_2);
}

UObject* 
ICUService::get(ثابت UnicodeString& descriptor, UnicodeString* actualReturn, UError_رمز& حالت_2) ثابت 
{
    UObject* نتیجہ = خالی_اشاریہ;
    ICUService_کلید* کلید = create_کلید(&descriptor, حالت_2);
    اگر (کلید) {
        نتیجہ = get_کلید(*کلید, actualReturn, حالت_2);
        حذف کلید;
    }
    واپس نتیجہ;
}

UObject* 
ICUService::get_کلید(ICUService_کلید& کلید, UError_رمز& حالت_2) ثابت 
{
    واپس get_کلید(کلید, خالی_اشاریہ, حالت_2);
}

// this is a vector that subclasses of ICUService can override to further customize the result object
// before returning it.  All other public get functions should call this one.

UObject* 
ICUService::get_کلید(ICUService_کلید& کلید, UnicodeString* actualReturn, UError_رمز& حالت_2) ثابت 
{
    واپس get_کلید(کلید, actualReturn, خالی_اشاریہ, حالت_2);
}

// make it possible to call reentrantly on systems that don't have reentrant mutexes.
// we can use this simple approach since we know the situation where we're calling
// reentrantly even without knowing the thread.
جماعت XMutex : عوامی UMemory {
عوامی:
    اندرونی XMutex(UMutex *mutex, UBool reentering) 
        : fMutex(mutex)
        , f_فعال(!reentering) 
    {
        اگر (f_فعال) umtx_تالا(fMutex);
    }
    اندرونی ~XMutex() {
        اگر (f_فعال) umtx_unlock(fMutex);
    }

نجی:
    UMutex  *fMutex;
    UBool f_فعال;
};

// called only by factories, treat as private
UObject* 
ICUService::get_کلید(ICUService_کلید& کلید, UnicodeString* actualReturn, ثابت ICUServiceFactory* factory, UError_رمز& حالت_2) ثابت 
{
    اگر (U_FAILURE(حالت_2)) {
        واپس خالی_اشاریہ;
    }

    اگر (isDefault()) {
        واپس handleDefault(کلید, actualReturn, حالت_2);
    }

    ICUService* ncthis = const_cast<ICUService*>(یہ); // cast away semantic const

    Cache_اندراج* نتیجہ = خالی_اشاریہ;
    {
        // The factory list can't be modified until we're done, 
        // otherwise we might update the cache with an invalid result.
        // The cache has to stay in synch with the factory list.
        // ICU doesn't have monitors so we can't use rw locks, so 
        // we single-thread everything using this service, for now.

        // if factory is not null, we're calling from within the mutex,
        // and since some unix machines don't have reentrant mutexes we
        // need to make sure not to try to lock it again.
        XMutex mutex(&تالا, factory != خالی_اشاریہ);

        اگر (serviceCache == خالی_اشاریہ) {
            ncthis->serviceCache = نیا Hashtable(حالت_2);
            اگر (ncthis->serviceCache == خالی_اشاریہ) {
                حالت_2 = U_حافظہ_تخصیص_ERROR;
                واپس خالی_اشاریہ;
            }
            اگر (U_FAILURE(حالت_2)) {
                حذف serviceCache;
                واپس خالی_اشاریہ;
            }
            serviceCache->سیٹ_قدر_Deleter(cacheDeleter);
        }

        UnicodeString currentDescriptor;
        Local_اشارہ_گر<UVector> cacheDescriptorList;
        UBool put_اندر_Cache = جھوٹ;

        int32_t آغاز_اشاریہ = 0;
        int32_t حد_بندی = factories->size();
        UBool cache_نتیجہ = سچ;

        اگر (factory != خالی_اشاریہ) {
            برائے (int32_t i = 0; i < حد_بندی; ++i) {
                اگر (factory == static_cast<ثابت ICUServiceFactory*>(factories->elementAt(i))) {
                    آغاز_اشاریہ = i + 1;
                    توڑو;
                }
            }
            اگر (آغاز_اشاریہ == 0) {
                // throw new InternalError("Factory " + factory + "not registered with service: " + this);
                حالت_2 = U_ILLEGAL_دلیل_ERROR;
                واپس خالی_اشاریہ;
            }
            cache_نتیجہ = جھوٹ;
        }

        کرو {
            currentDescriptor.remove();
            کلید.currentDescriptor(currentDescriptor);
            نتیجہ = static_cast<Cache_اندراج*>(serviceCache->get(currentDescriptor));
            اگر (نتیجہ != خالی_اشاریہ) {
                توڑو;
            }

            // first test of cache failed, so we'll have to update
            // the cache if we eventually succeed-- that is, if we're 
            // going to update the cache at all.
            put_اندر_Cache = سچ;

            int32_t اشاریہ = آغاز_اشاریہ;
            جب (اشاریہ < حد_بندی) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(اشاریہ++));
                Local_اشارہ_گر<UObject> service(f->create(کلید, یہ, حالت_2));
                اگر (U_FAILURE(حالت_2)) {
                    واپس خالی_اشاریہ;
                }
                اگر (service.isValid()) {
                    نتیجہ = نیا Cache_اندراج(currentDescriptor, service.getAlias());
                    اگر (نتیجہ == خالی_اشاریہ) {
                        حالت_2 = U_حافظہ_تخصیص_ERROR;
                        واپس خالی_اشاریہ;
                    }
                    service.orphan(); // result now owns service.

                    جاؤ outer_انتہا;
                }
            }

            // prepare to load the cache with all additional ids that 
            // will resolve to result, assuming we'll succeed.  We
            // don't want to keep querying on an id that's going to
            // fallback to the one that succeeded, we want to hit the
            // cache the first time next goaround.
            اگر (cacheDescriptorList.isNull()) {
                cacheDescriptorList.adopt_Instead_And_Check_Error_رمز(نیا UVector(uprv_deleteUObject, خالی_اشاریہ, 5, حالت_2), حالت_2);
                اگر (U_FAILURE(حالت_2)) {
                    واپس خالی_اشاریہ;
                }
            }

            Local_اشارہ_گر<UnicodeString> id_مقصد_Cache(نیا UnicodeString(currentDescriptor), حالت_2);
            اگر (U_FAILURE(حالت_2)) {
                واپس خالی_اشاریہ;
            }
            اگر (id_مقصد_Cache->isBogus()) {
                حالت_2 = U_حافظہ_تخصیص_ERROR;
                واپس خالی_اشاریہ;
            }
            cacheDescriptorList->adoptElement(id_مقصد_Cache.orphan(), حالت_2);
            اگر (U_FAILURE(حالت_2)) {
                واپس خالی_اشاریہ;
            }
        } جب (کلید.fallback());
outer_انتہا:

        اگر (نتیجہ != خالی_اشاریہ) {
            اگر (put_اندر_Cache && cache_نتیجہ) {
                serviceCache->put(نتیجہ->actualDescriptor, نتیجہ, حالت_2);
                اگر (U_FAILURE(حالت_2)) {
                    واپس خالی_اشاریہ;
                }

                اگر (cacheDescriptorList.isValid()) {
                    برائے (int32_t i = cacheDescriptorList->size(); --i >= 0;) {
                        UnicodeString* desc = static_cast<UnicodeString*>(cacheDescriptorList->elementAt(i));

                        serviceCache->put(*desc, نتیجہ, حالت_2);
                        اگر (U_FAILURE(حالت_2)) {
                            واپس خالی_اشاریہ;
                        }

                        نتیجہ->ref();
                        cacheDescriptorList->removeElementAt(i);
                    }
                }
            }

            اگر (actualReturn != خالی_اشاریہ) {
                // strip null prefix
                اگر (نتیجہ->actualDescriptor.اشاریہ_کا(static_cast<char16_t>(0x2f)) == 0) { // U+002f=slash (/)
                    actualReturn->remove();
                    actualReturn->append(نتیجہ->actualDescriptor, 
                        1, 
                        نتیجہ->actualDescriptor.length() - 1);
                } ورنہ {
                    *actualReturn = نتیجہ->actualDescriptor;
                }

                اگر (actualReturn->isBogus()) {
                    حالت_2 = U_حافظہ_تخصیص_ERROR;
                    حذف نتیجہ;
                    واپس خالی_اشاریہ;
                }
            }

            UObject* service = cloneInstance(نتیجہ->service);
            اگر (put_اندر_Cache && !cache_نتیجہ) {
                حذف نتیجہ;
            }
            واپس service;
        }
    }

    واپس handleDefault(کلید, actualReturn, حالت_2);
}

UObject* 
ICUService::handleDefault(ثابت ICUService_کلید& /*کلید*/, UnicodeString* /* actualIDReturn */, UError_رمز& /* status */) ثابت 
{
    واپس خالی_اشاریہ;
}

UVector& 
ICUService::getVisibleIDs(UVector& نتیجہ, UError_رمز& حالت_2) ثابت {
    واپس getVisibleIDs(نتیجہ, خالی_اشاریہ, حالت_2);
}

UVector& 
ICUService::getVisibleIDs(UVector& نتیجہ, ثابت UnicodeString* مطابقت_ID, UError_رمز& حالت_2) ثابت 
{
    نتیجہ.removeAllElements();

    اگر (U_FAILURE(حالت_2)) {
        واپس نتیجہ;
    }
    UObjectDeleter *savedDeleter = نتیجہ.سیٹ_Deleter(uprv_deleteUObject);

    {
        Mutex mutex(&تالا);
        ثابت Hashtable* map = getVisibleIDMap(حالت_2);
        اگر (map != خالی_اشاریہ) {
            ICUService_کلید* fallback_کلید = create_کلید(مطابقت_ID, حالت_2);

            برائے (int32_t pos = UHASH_پہلا; U_SUCCESS(حالت_2); ) {
                ثابت UHashElement* e = map->اگلا_Element(pos);
                اگر (e == خالی_اشاریہ) {
                    توڑو;
                }

                ثابت UnicodeString* id = static_cast<ثابت UnicodeString*>(e->کلید.اشارہ_گر);
                اگر (fallback_کلید != خالی_اشاریہ) {
                    اگر (!fallback_کلید->is_Fallback_کا(*id)) {
                        جاری;
                    }
                }

                Local_اشارہ_گر<UnicodeString> idClone(id->clone(), حالت_2);
                نتیجہ.adoptElement(idClone.orphan(), حالت_2);
            }
            حذف fallback_کلید;
        }
    }
    اگر (U_FAILURE(حالت_2)) {
        نتیجہ.removeAllElements();
    }
    نتیجہ.سیٹ_Deleter(savedDeleter);
    واپس نتیجہ;
}

ثابت Hashtable* 
ICUService::getVisibleIDMap(UError_رمز& حالت_2) ثابت {
    اگر (U_FAILURE(حالت_2)) واپس خالی_اشاریہ;

    // must only be called when lock is already held

    ICUService* ncthis = const_cast<ICUService*>(یہ); // cast away semantic const
    اگر (idCache == خالی_اشاریہ) {
        ncthis->idCache = نیا Hashtable(حالت_2);
        اگر (idCache == خالی_اشاریہ) {
            حالت_2 = U_حافظہ_تخصیص_ERROR;
        } ورنہ اگر (factories != خالی_اشاریہ) {
            برائے (int32_t pos = factories->size(); --pos >= 0;) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(pos));
                f->updateVisibleIDs(*idCache, حالت_2);
            }
            اگر (U_FAILURE(حالت_2)) {
                حذف idCache;
                ncthis->idCache = خالی_اشاریہ;
            }
        }
    }

    واپس idCache;
}


UnicodeString& 
ICUService::getDisplayName(ثابت UnicodeString& id, UnicodeString& نتیجہ) ثابت 
{
    واپس getDisplayName(id, نتیجہ, Locale::getDefault());
}

UnicodeString& 
ICUService::getDisplayName(ثابت UnicodeString& id, UnicodeString& نتیجہ, ثابت Locale& locale) ثابت 
{
    {
        UError_رمز حالت_2 = U_ZERO_ERROR;
        Mutex mutex(&تالا);
        ثابت Hashtable* map = getVisibleIDMap(حالت_2);
        اگر (map != خالی_اشاریہ) {
            ICUServiceFactory* f = static_cast<ICUServiceFactory*>(map->get(id));
            اگر (f != خالی_اشاریہ) {
                f->getDisplayName(id, locale, نتیجہ);
                واپس نتیجہ;
            }

            // fallback
            حالت_2 = U_ZERO_ERROR;
            ICUService_کلید* fallback_کلید = create_کلید(&id, حالت_2);
            جب (fallback_کلید != خالی_اشاریہ && fallback_کلید->fallback()) {
                UnicodeString us;
                fallback_کلید->currentID(us);
                f = static_cast<ICUServiceFactory*>(map->get(us));
                اگر (f != خالی_اشاریہ) {
                    f->getDisplayName(id, locale, نتیجہ);
                    حذف fallback_کلید;
                    واپس نتیجہ;
                }
            }
            حذف fallback_کلید;
        }
    }
    نتیجہ.سیٹ_مقصد_Bogus();
    واپس نتیجہ;
}

UVector& 
ICUService::get_Display_نام(UVector& نتیجہ, UError_رمز& حالت_2) ثابت 
{
    واپس get_Display_نام(نتیجہ, Locale::getDefault(), خالی_اشاریہ, حالت_2);
}


UVector& 
ICUService::get_Display_نام(UVector& نتیجہ, ثابت Locale& locale, UError_رمز& حالت_2) ثابت 
{
    واپس get_Display_نام(نتیجہ, locale, خالی_اشاریہ, حالت_2);
}

UVector& 
ICUService::get_Display_نام(UVector& نتیجہ, 
                            ثابت Locale& locale, 
                            ثابت UnicodeString* مطابقت_ID, 
                            UError_رمز& حالت_2) ثابت 
{
    نتیجہ.removeAllElements();
    نتیجہ.سیٹ_Deleter(userv_deleteStringPair);
    اگر (U_SUCCESS(حالت_2)) {
        ICUService* ncthis = const_cast<ICUService*>(یہ); // cast away semantic const
        Mutex mutex(&تالا);

        اگر (dnCache != خالی_اشاریہ && dnCache->locale != locale) {
            حذف dnCache;
            ncthis->dnCache = خالی_اشاریہ;
        }

        اگر (dnCache == خالی_اشاریہ) {
            ثابت Hashtable* m = getVisibleIDMap(حالت_2);
            اگر (U_FAILURE(حالت_2)) {
                واپس نتیجہ;
            }
            ncthis->dnCache = نیا DNCache(locale); 
            اگر (dnCache == خالی_اشاریہ) {
                حالت_2 = U_حافظہ_تخصیص_ERROR;
                واپس نتیجہ;
            }

            int32_t pos = UHASH_پہلا;
            ثابت UHashElement* اندراج_2 = خالی_اشاریہ;
            جب ((اندراج_2 = m->اگلا_Element(pos)) != خالی_اشاریہ) {
                ثابت UnicodeString* id = static_cast<ثابت UnicodeString*>(اندراج_2->کلید.اشارہ_گر);
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(اندراج_2->value.اشارہ_گر);
                UnicodeString dname;
                f->getDisplayName(*id, locale, dname);
                اگر (dname.isBogus()) {
                    حالت_2 = U_حافظہ_تخصیص_ERROR;
                } ورنہ {
                    dnCache->cache.put(dname, (خالی*)id, حالت_2); // share pointer with visibleIDMap
                    اگر (U_SUCCESS(حالت_2)) {
                        جاری;
                    }
                }
                حذف dnCache;
                ncthis->dnCache = خالی_اشاریہ;
                واپس نتیجہ;
            }
        }
    }

    ICUService_کلید* مطابقت_کلید = create_کلید(مطابقت_ID, حالت_2);
    /* To ensure that all elements in the hashtable are iterated, set pos to -1.
     * nextElement(pos) will skip the position at pos and begin the iteration
     * at the next position, which in this case will be 0.
     */
    int32_t pos = UHASH_پہلا; 
    ثابت UHashElement *اندراج_2 = خالی_اشاریہ;
    جب ((اندراج_2 = dnCache->cache.اگلا_Element(pos)) != خالی_اشاریہ) {
        ثابت UnicodeString* id = static_cast<ثابت UnicodeString*>(اندراج_2->value.اشارہ_گر);
        اگر (مطابقت_کلید != خالی_اشاریہ && !مطابقت_کلید->is_Fallback_کا(*id)) {
            جاری;
        }
        ثابت UnicodeString* dn = static_cast<ثابت UnicodeString*>(اندراج_2->کلید.اشارہ_گر);
        StringPair* sp = StringPair::create(*id, *dn, حالت_2);
        نتیجہ.adoptElement(sp, حالت_2);
        اگر (U_FAILURE(حالت_2)) {
            نتیجہ.removeAllElements();
            توڑو;
        }
    }
    حذف مطابقت_کلید;

    واپس نتیجہ;
}

URegistry_کلید
ICUService::رجسٹر_Instance(UObject* obj_مقصد_Adopt, ثابت UnicodeString& id, UError_رمز& حالت_2) 
{
    واپس رجسٹر_Instance(obj_مقصد_Adopt, id, سچ, حالت_2);
}

URegistry_کلید
ICUService::رجسٹر_Instance(UObject* obj_مقصد_Adopt, ثابت UnicodeString& id, UBool visible, UError_رمز& حالت_2) 
{
    ICUService_کلید* کلید = create_کلید(&id, حالت_2);
    اگر (کلید != خالی_اشاریہ) {
        UnicodeString canonicalID;
        کلید->canonicalID(canonicalID);
        حذف کلید;

        ICUServiceFactory* f = createSimpleFactory(obj_مقصد_Adopt, canonicalID, visible, حالت_2);
        اگر (f != خالی_اشاریہ) {
            واپس رجسٹر_Factory(f, حالت_2);
        }
    }
    حذف obj_مقصد_Adopt;
    واپس خالی_اشاریہ;
}

ICUServiceFactory* 
ICUService::createSimpleFactory(UObject* obj_مقصد_Adopt, ثابت UnicodeString& id, UBool visible, UError_رمز& حالت_2)
{
    اگر (U_SUCCESS(حالت_2)) {
        اگر ((obj_مقصد_Adopt != خالی_اشاریہ) && (!id.isBogus())) {
            واپس نیا SimpleFactory(obj_مقصد_Adopt, id, visible);
        }
        حالت_2 = U_ILLEGAL_دلیل_ERROR;
    }
    واپس خالی_اشاریہ;
}

URegistry_کلید
ICUService::رجسٹر_Factory(ICUServiceFactory* factory_مقصد_Adopt, UError_رمز& حالت_2)
{
    Local_اشارہ_گر<ICUServiceFactory>lp_Factory_مقصد_Adopt(factory_مقصد_Adopt);
    اگر (U_FAILURE(حالت_2) || factory_مقصد_Adopt == خالی_اشاریہ) {
        واپس خالی_اشاریہ;
    }
    {
        Mutex mutex(&تالا);

        اگر (factories == خالی_اشاریہ) {
            Local_اشارہ_گر<UVector> lpFactories(نیا UVector(uprv_deleteUObject, خالی_اشاریہ, حالت_2), حالت_2);
            اگر (U_FAILURE(حالت_2)) {
                واپس خالی_اشاریہ;
            }
            factories = lpFactories.orphan();
        }
        factories->داخل_کرو_Element_At(lp_Factory_مقصد_Adopt.orphan(), 0, حالت_2);
        اگر (U_SUCCESS(حالت_2)) {
            clearCaches();
        }
    }   // Close of mutex lock block.

    اگر (U_SUCCESS(حالت_2)) {
        notify_بدلا();
        واپس (URegistry_کلید)factory_مقصد_Adopt;
    } ورنہ {
        واپس خالی_اشاریہ;
    }
}

UBool 
ICUService::unregister(URegistry_کلید rkey, UError_رمز& حالت_2) 
{
    ICUServiceFactory *factory = (ICUServiceFactory*)rkey;
    UBool نتیجہ = جھوٹ;
    اگر (factory != خالی_اشاریہ && factories != خالی_اشاریہ) {
        Mutex mutex(&تالا);

        اگر (factories->removeElement(factory)) {
            clearCaches();
            نتیجہ = سچ;
        } ورنہ {
            حالت_2 = U_ILLEGAL_دلیل_ERROR;
            حذف factory;
        }
    }
    اگر (نتیجہ) {
        notify_بدلا();
    }
    واپس نتیجہ;
}

خالی 
ICUService::reset() 
{
    {
        Mutex mutex(&تالا);
        reInitializeFactories();
        clearCaches();
    }
    notify_بدلا();
}

خالی 
ICUService::reInitializeFactories() 
{
    اگر (factories != خالی_اشاریہ) {
        factories->removeAllElements();
    }
}

UBool 
ICUService::isDefault() ثابت 
{
    واپس گنتی_Factories() == 0;
}

ICUService_کلید* 
ICUService::create_کلید(ثابت UnicodeString* id, UError_رمز& حالت_2) ثابت 
{
    واپس (U_FAILURE(حالت_2) || id == خالی_اشاریہ) ? خالی_اشاریہ : نیا ICUService_کلید(*id);
}

خالی 
ICUService::clearCaches() 
{
    // callers synchronize before use
    ++timestamp;
    حذف dnCache;
    dnCache = خالی_اشاریہ;
    حذف idCache;
    idCache = خالی_اشاریہ;
    حذف serviceCache; serviceCache = خالی_اشاریہ;
}

خالی 
ICUService::clearServiceCache() 
{
    // callers synchronize before use
    حذف serviceCache; serviceCache = خالی_اشاریہ;
}

UBool 
ICUService::قبول_کرتا_Listener(ثابت EventListener& l) ثابت 
{
    واپس dynamic_cast<ثابت ServiceListener*>(&l) != خالی_اشاریہ;
}

خالی 
ICUService::notifyListener(EventListener& l) ثابت 
{
    (static_cast<ServiceListener&>(l)).service_بدلا(*یہ);
}

UnicodeString&
ICUService::getName(UnicodeString& نتیجہ) ثابت 
{
    واپس نتیجہ.append(name);
}

int32_t 
ICUService::گنتی_Factories() ثابت 
{
    واپس factories == خالی_اشاریہ ? 0 : factories->size();
}

int32_t
ICUService::getTimestamp() ثابت
{
    واپس timestamp;
}

U_NAMESPACE_انتہا

/* UCONFIG_NO_SERVICE */
#اگر_ختم
