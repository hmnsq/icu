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

#undef SERVICE_REFCOUNT

// in case we use the refcount stuff

U_NAMESPACE_BEGIN

/*
******************************************************************
*/

ثابت char16_t ICUService_کلید::PREFIX_DELIMITER = 0x002F;   /* '/' */

ICUService_کلید::ICUService_کلید(ثابت UnicodeString& id) 
: _id(id) {
}

ICUService_کلید::~ICUService_کلید() 
{
}

ثابت UnicodeString& 
ICUService_کلید::getID() ثابت 
{
    بازگشت _id;
}

UnicodeString& 
ICUService_کلید::canonicalID(UnicodeString& result) ثابت 
{
    بازگشت result.append(_id);
}

UnicodeString& 
ICUService_کلید::currentID(UnicodeString& result) ثابت 
{
    بازگشت canonicalID(result);
}

UnicodeString& 
ICUService_کلید::currentDescriptor(UnicodeString& result) ثابت 
{
    prefix(result);
    result.append(PREFIX_DELIMITER);
    بازگشت currentID(result);
}

UBool 
ICUService_کلید::fallback() 
{
    بازگشت نادرست;
}

UBool 
ICUService_کلید::isFallbackOf(ثابت UnicodeString& id) ثابت 
{
    بازگشت id == _id;
}

UnicodeString& 
ICUService_کلید::prefix(UnicodeString& result) ثابت 
{
    بازگشت result;
}

UnicodeString& 
ICUService_کلید::parsePrefix(UnicodeString& result) 
{
    int32_t n = result.indexOf(PREFIX_DELIMITER);
    اگر (n < 0) {
        n = 0;
    }
    result.remove(n);
    بازگشت result;
}

UnicodeString& 
ICUService_کلید::parseSuffix(UnicodeString& result) 
{
    int32_t n = result.indexOf(PREFIX_DELIMITER);
    اگر (n >= 0) {
        result.remove(0, n+1);
    }
    بازگشت result;
}

#ifdef SERVICE_DEBUG
UnicodeString& 
ICUService_کلید::debug(UnicodeString& result) ثابت 
{
    debugClass(result);
    result.append((UnicodeString)" id: ");
    result.append(_id);
    بازگشت result;
}

UnicodeString& 
ICUService_کلید::debugClass(UnicodeString& result) ثابت 
{
    بازگشت result.append((UnicodeString)"ICUServiceKey");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ICUService_کلید)

/*
******************************************************************
*/

ICUServiceFactory::~ICUServiceFactory() {}

SimpleFactory::SimpleFactory(UObject* instanceToAdopt, ثابت UnicodeString& id, UBool visible) 
: _instance(instanceToAdopt), _id(id), _visible(visible)
{
}

SimpleFactory::~SimpleFactory() 
{
    delete _instance;
}

UObject* 
SimpleFactory::create(ثابت ICUService_کلید& کلید, ثابت ICUService* service, UErrorCode& status) ثابت 
{
    اگر (U_SUCCESS(status)) {
        UnicodeString temp;
        اگر (_id == کلید.currentID(temp)) {
            بازگشت service->cloneInstance(_instance); 
        }
    }
    بازگشت nullptr;
}

تهی 
SimpleFactory::updateVisibleIDs(Hashtable& result, UErrorCode& status) ثابت 
{
    اگر (_visible) {
        result.put(_id, (تهی*)این, status); // cast away const
    } وگرنه {
        result.remove(_id);
    }
}

UnicodeString& 
SimpleFactory::getDisplayName(ثابت UnicodeString& id, ثابت Locale& /* locale */, UnicodeString& result) ثابت 
{
    اگر (_visible && _id == id) {
        result = _id;
    } وگرنه {
        result.setToBogus();
    }
    بازگشت result;
}

#ifdef SERVICE_DEBUG
UnicodeString& 
SimpleFactory::debug(UnicodeString& toAppendTo) ثابت 
{
    debugClass(toAppendTo);
    toAppendTo.append((UnicodeString)" id: ");
    toAppendTo.append(_id);
    toAppendTo.append((UnicodeString)", visible: ");
    toAppendTo.append(_visible ? (UnicodeString)"T" : (UnicodeString)"F");
    بازگشت toAppendTo;
}

UnicodeString& 
SimpleFactory::debugClass(UnicodeString& toAppendTo) ثابت 
{
    بازگشت toAppendTo.append((UnicodeString)"SimpleFactory");
}
#endif

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
رده CacheEntry : عمومی UMemory {
خصوصی:
    int32_t refcount;

عمومی:
    UnicodeString actualDescriptor;
    UObject* service;

    /**
    * Releases a reference to the shared resource.
    */
    ~CacheEntry() {
        delete service;
    }

    CacheEntry(ثابت UnicodeString& _actualDescriptor, UObject* _service) 
        : refcount(1), actualDescriptor(_actualDescriptor), service(_service) {
    }

    /**
    * Instantiation creates an initial reference, so don't call this
    * unless you're creating a new pointer to this.  Management of
    * that pointer will have to know how to deal with refcounts.  
    * Return true if the resource has not already been released.
    */
    CacheEntry* ref() {
        ++refcount;
        بازگشت این;
    }

    /**
    * Destructions removes a reference, so don't call this unless
    * you're removing pointer to this somewhere.  Management of that
    * pointer will have to know how to deal with refcounts.  Once
    * the refcount drops to zero, the resource is released.  Return
    * false if the resource has been released.
    */
    CacheEntry* unref() {
        اگر ((--refcount) == 0) {
            delete این;
            بازگشت nullptr;
        }
        بازگشت این;
    }

    /**
    * Return true if there is at least one reference to this and the
    * resource has not been released.
    */
    UBool isShared() ثابت {
        بازگشت refcount > 1;
    }
};

// Deleter for serviceCache
U_CDECL_BEGIN
ایستا تهی U_CALLCONV
cacheDeleter(تهی* obj) {
    U_NAMESPACE_USE ((CacheEntry*)obj)->unref();
}

U_CDECL_END

/*
******************************************************************
*/

رده DNCache : عمومی UMemory {
عمومی:
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
                   UErrorCode& status)
{
    اگر (U_SUCCESS(status)) {
        StringPair* sp = جدید StringPair(displayName, id);
        اگر (sp == nullptr || sp->isBogus()) {
            status = U_MEMORY_ALLOCATION_ERROR;
            delete sp;
            بازگشت nullptr;
        }
        بازگشت sp;
    }
    بازگشت nullptr;
}

UBool 
StringPair::isBogus() ثابت {
    بازگشت displayName.isBogus() || id.isBogus();
}

StringPair::StringPair(ثابت UnicodeString& _displayName, 
                       ثابت UnicodeString& _id)
: displayName(_displayName)
, id(_id)
{
}

U_CDECL_BEGIN
ایستا تهی U_CALLCONV
userv_deleteStringPair(تهی *obj) {
    U_NAMESPACE_USE delete (StringPair*) obj;
}
U_CDECL_END

/*
******************************************************************
*/

ایستا UMutex lock;

ICUService::ICUService()
: name()
, timestamp(0)
, factories(nullptr)
, serviceCache(nullptr)
, idCache(nullptr)
, dnCache(nullptr)
{
}

ICUService::ICUService(ثابت UnicodeString& newName) 
: name(newName)
, timestamp(0)
, factories(nullptr)
, serviceCache(nullptr)
, idCache(nullptr)
, dnCache(nullptr)
{
}

ICUService::~ICUService()
{
    {
        Mutex mutex(&lock);
        clearCaches();
        delete factories;
        factories = nullptr;
    }
}

UObject* 
ICUService::get(ثابت UnicodeString& descriptor, UErrorCode& status) ثابت 
{
    بازگشت get(descriptor, nullptr, status);
}

UObject* 
ICUService::get(ثابت UnicodeString& descriptor, UnicodeString* actualReturn, UErrorCode& status) ثابت 
{
    UObject* result = nullptr;
    ICUService_کلید* کلید = create_کلید(&descriptor, status);
    اگر (کلید) {
        result = get_کلید(*کلید, actualReturn, status);
        delete کلید;
    }
    بازگشت result;
}

UObject* 
ICUService::get_کلید(ICUService_کلید& کلید, UErrorCode& status) ثابت 
{
    بازگشت get_کلید(کلید, nullptr, status);
}

// this is a vector that subclasses of ICUService can override to further customize the result object
// before returning it.  All other public get functions should call this one.

UObject* 
ICUService::get_کلید(ICUService_کلید& کلید, UnicodeString* actualReturn, UErrorCode& status) ثابت 
{
    بازگشت get_کلید(کلید, actualReturn, nullptr, status);
}

// make it possible to call reentrantly on systems that don't have reentrant mutexes.
// we can use this simple approach since we know the situation where we're calling
// reentrantly even without knowing the thread.
رده XMutex : عمومی UMemory {
عمومی:
    درونخطی XMutex(UMutex *mutex, UBool reentering) 
        : fMutex(mutex)
        , fActive(!reentering) 
    {
        اگر (fActive) umtx_lock(fMutex);
    }
    درونخطی ~XMutex() {
        اگر (fActive) umtx_unlock(fMutex);
    }

خصوصی:
    UMutex  *fMutex;
    UBool fActive;
};

// called only by factories, treat as private
UObject* 
ICUService::get_کلید(ICUService_کلید& کلید, UnicodeString* actualReturn, ثابت ICUServiceFactory* factory, UErrorCode& status) ثابت 
{
    اگر (U_FAILURE(status)) {
        بازگشت nullptr;
    }

    اگر (isDefault()) {
        بازگشت handleDefault(کلید, actualReturn, status);
    }

    ICUService* ncthis = const_cast<ICUService*>(این); // cast away semantic const

    CacheEntry* result = nullptr;
    {
        // The factory list can't be modified until we're done, 
        // otherwise we might update the cache with an invalid result.
        // The cache has to stay in synch with the factory list.
        // ICU doesn't have monitors so we can't use rw locks, so 
        // we single-thread everything using this service, for now.

        // if factory is not null, we're calling from within the mutex,
        // and since some unix machines don't have reentrant mutexes we
        // need to make sure not to try to lock it again.
        XMutex mutex(&lock, factory != nullptr);

        اگر (serviceCache == nullptr) {
            ncthis->serviceCache = جدید Hashtable(status);
            اگر (ncthis->serviceCache == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                بازگشت nullptr;
            }
            اگر (U_FAILURE(status)) {
                delete serviceCache;
                بازگشت nullptr;
            }
            serviceCache->set_مقدار_Deleter(cacheDeleter);
        }

        UnicodeString currentDescriptor;
        LocalPointer<UVector> cacheDescriptorList;
        UBool putInCache = نادرست;

        int32_t startIndex = 0;
        int32_t limit = factories->size();
        UBool cacheResult = درست;

        اگر (factory != nullptr) {
            برای (int32_t i = 0; i < limit; ++i) {
                اگر (factory == static_cast<ثابت ICUServiceFactory*>(factories->elementAt(i))) {
                    startIndex = i + 1;
                    بشکن;
                }
            }
            اگر (startIndex == 0) {
                // throw new InternalError("Factory " + factory + "not registered with service: " + this);
                status = U_ILLEGAL_ARGUMENT_ERROR;
                بازگشت nullptr;
            }
            cacheResult = نادرست;
        }

        انجامبده {
            currentDescriptor.remove();
            کلید.currentDescriptor(currentDescriptor);
            result = static_cast<CacheEntry*>(serviceCache->get(currentDescriptor));
            اگر (result != nullptr) {
                بشکن;
            }

            // first test of cache failed, so we'll have to update
            // the cache if we eventually succeed-- that is, if we're 
            // going to update the cache at all.
            putInCache = درست;

            int32_t index = startIndex;
            وقتی (index < limit) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(index++));
                LocalPointer<UObject> service(f->create(کلید, این, status));
                اگر (U_FAILURE(status)) {
                    بازگشت nullptr;
                }
                اگر (service.isValid()) {
                    result = جدید CacheEntry(currentDescriptor, service.getAlias());
                    اگر (result == nullptr) {
                        status = U_MEMORY_ALLOCATION_ERROR;
                        بازگشت nullptr;
                    }
                    service.orphan(); // result now owns service.

                    برو outerEnd;
                }
            }

            // prepare to load the cache with all additional ids that 
            // will resolve to result, assuming we'll succeed.  We
            // don't want to keep querying on an id that's going to
            // fallback to the one that succeeded, we want to hit the
            // cache the first time next goaround.
            اگر (cacheDescriptorList.isNull()) {
                cacheDescriptorList.adoptInsteadAndCheckErrorCode(جدید UVector(uprv_deleteUObject, nullptr, 5, status), status);
                اگر (U_FAILURE(status)) {
                    بازگشت nullptr;
                }
            }

            LocalPointer<UnicodeString> idToCache(جدید UnicodeString(currentDescriptor), status);
            اگر (U_FAILURE(status)) {
                بازگشت nullptr;
            }
            اگر (idToCache->isBogus()) {
                status = U_MEMORY_ALLOCATION_ERROR;
                بازگشت nullptr;
            }
            cacheDescriptorList->adoptElement(idToCache.orphan(), status);
            اگر (U_FAILURE(status)) {
                بازگشت nullptr;
            }
        } وقتی (کلید.fallback());
outerEnd:

        اگر (result != nullptr) {
            اگر (putInCache && cacheResult) {
                serviceCache->put(result->actualDescriptor, result, status);
                اگر (U_FAILURE(status)) {
                    بازگشت nullptr;
                }

                اگر (cacheDescriptorList.isValid()) {
                    برای (int32_t i = cacheDescriptorList->size(); --i >= 0;) {
                        UnicodeString* desc = static_cast<UnicodeString*>(cacheDescriptorList->elementAt(i));

                        serviceCache->put(*desc, result, status);
                        اگر (U_FAILURE(status)) {
                            بازگشت nullptr;
                        }

                        result->ref();
                        cacheDescriptorList->removeElementAt(i);
                    }
                }
            }

            اگر (actualReturn != nullptr) {
                // strip null prefix
                اگر (result->actualDescriptor.indexOf(static_cast<char16_t>(0x2f)) == 0) { // U+002f=slash (/)
                    actualReturn->remove();
                    actualReturn->append(result->actualDescriptor, 
                        1, 
                        result->actualDescriptor.length() - 1);
                } وگرنه {
                    *actualReturn = result->actualDescriptor;
                }

                اگر (actualReturn->isBogus()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    delete result;
                    بازگشت nullptr;
                }
            }

            UObject* service = cloneInstance(result->service);
            اگر (putInCache && !cacheResult) {
                delete result;
            }
            بازگشت service;
        }
    }

    بازگشت handleDefault(کلید, actualReturn, status);
}

UObject* 
ICUService::handleDefault(ثابت ICUService_کلید& /*کلید*/, UnicodeString* /* actualIDReturn */, UErrorCode& /* status */) ثابت 
{
    بازگشت nullptr;
}

UVector& 
ICUService::getVisibleIDs(UVector& result, UErrorCode& status) ثابت {
    بازگشت getVisibleIDs(result, nullptr, status);
}

UVector& 
ICUService::getVisibleIDs(UVector& result, ثابت UnicodeString* matchID, UErrorCode& status) ثابت 
{
    result.removeAllElements();

    اگر (U_FAILURE(status)) {
        بازگشت result;
    }
    UObjectDeleter *savedDeleter = result.setDeleter(uprv_deleteUObject);

    {
        Mutex mutex(&lock);
        ثابت Hashtable* map = getVisibleIDMap(status);
        اگر (map != nullptr) {
            ICUService_کلید* fallback_کلید = create_کلید(matchID, status);

            برای (int32_t pos = UHASH_FIRST; U_SUCCESS(status); ) {
                ثابت UHashElement* e = map->nextElement(pos);
                اگر (e == nullptr) {
                    بشکن;
                }

                ثابت UnicodeString* id = static_cast<ثابت UnicodeString*>(e->کلید.pointer);
                اگر (fallback_کلید != nullptr) {
                    اگر (!fallback_کلید->isFallbackOf(*id)) {
                        ادامه;
                    }
                }

                LocalPointer<UnicodeString> idClone(id->clone(), status);
                result.adoptElement(idClone.orphan(), status);
            }
            delete fallback_کلید;
        }
    }
    اگر (U_FAILURE(status)) {
        result.removeAllElements();
    }
    result.setDeleter(savedDeleter);
    بازگشت result;
}

ثابت Hashtable* 
ICUService::getVisibleIDMap(UErrorCode& status) ثابت {
    اگر (U_FAILURE(status)) بازگشت nullptr;

    // must only be called when lock is already held

    ICUService* ncthis = const_cast<ICUService*>(این); // cast away semantic const
    اگر (idCache == nullptr) {
        ncthis->idCache = جدید Hashtable(status);
        اگر (idCache == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        } وگرنه اگر (factories != nullptr) {
            برای (int32_t pos = factories->size(); --pos >= 0;) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(pos));
                f->updateVisibleIDs(*idCache, status);
            }
            اگر (U_FAILURE(status)) {
                delete idCache;
                ncthis->idCache = nullptr;
            }
        }
    }

    بازگشت idCache;
}


UnicodeString& 
ICUService::getDisplayName(ثابت UnicodeString& id, UnicodeString& result) ثابت 
{
    بازگشت getDisplayName(id, result, Locale::getDefault());
}

UnicodeString& 
ICUService::getDisplayName(ثابت UnicodeString& id, UnicodeString& result, ثابت Locale& locale) ثابت 
{
    {
        UErrorCode status = U_ZERO_ERROR;
        Mutex mutex(&lock);
        ثابت Hashtable* map = getVisibleIDMap(status);
        اگر (map != nullptr) {
            ICUServiceFactory* f = static_cast<ICUServiceFactory*>(map->get(id));
            اگر (f != nullptr) {
                f->getDisplayName(id, locale, result);
                بازگشت result;
            }

            // fallback
            status = U_ZERO_ERROR;
            ICUService_کلید* fallback_کلید = create_کلید(&id, status);
            وقتی (fallback_کلید != nullptr && fallback_کلید->fallback()) {
                UnicodeString us;
                fallback_کلید->currentID(us);
                f = static_cast<ICUServiceFactory*>(map->get(us));
                اگر (f != nullptr) {
                    f->getDisplayName(id, locale, result);
                    delete fallback_کلید;
                    بازگشت result;
                }
            }
            delete fallback_کلید;
        }
    }
    result.setToBogus();
    بازگشت result;
}

UVector& 
ICUService::getDisplayNames(UVector& result, UErrorCode& status) ثابت 
{
    بازگشت getDisplayNames(result, Locale::getDefault(), nullptr, status);
}


UVector& 
ICUService::getDisplayNames(UVector& result, ثابت Locale& locale, UErrorCode& status) ثابت 
{
    بازگشت getDisplayNames(result, locale, nullptr, status);
}

UVector& 
ICUService::getDisplayNames(UVector& result, 
                            ثابت Locale& locale, 
                            ثابت UnicodeString* matchID, 
                            UErrorCode& status) ثابت 
{
    result.removeAllElements();
    result.setDeleter(userv_deleteStringPair);
    اگر (U_SUCCESS(status)) {
        ICUService* ncthis = const_cast<ICUService*>(این); // cast away semantic const
        Mutex mutex(&lock);

        اگر (dnCache != nullptr && dnCache->locale != locale) {
            delete dnCache;
            ncthis->dnCache = nullptr;
        }

        اگر (dnCache == nullptr) {
            ثابت Hashtable* m = getVisibleIDMap(status);
            اگر (U_FAILURE(status)) {
                بازگشت result;
            }
            ncthis->dnCache = جدید DNCache(locale); 
            اگر (dnCache == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                بازگشت result;
            }

            int32_t pos = UHASH_FIRST;
            ثابت UHashElement* entry = nullptr;
            وقتی ((entry = m->nextElement(pos)) != nullptr) {
                ثابت UnicodeString* id = static_cast<ثابت UnicodeString*>(entry->کلید.pointer);
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(entry->value.pointer);
                UnicodeString dname;
                f->getDisplayName(*id, locale, dname);
                اگر (dname.isBogus()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                } وگرنه {
                    dnCache->cache.put(dname, (تهی*)id, status); // share pointer with visibleIDMap
                    اگر (U_SUCCESS(status)) {
                        ادامه;
                    }
                }
                delete dnCache;
                ncthis->dnCache = nullptr;
                بازگشت result;
            }
        }
    }

    ICUService_کلید* match_کلید = create_کلید(matchID, status);
    /* To ensure that all elements in the hashtable are iterated, set pos to -1.
     * nextElement(pos) will skip the position at pos and begin the iteration
     * at the next position, which in this case will be 0.
     */
    int32_t pos = UHASH_FIRST; 
    ثابت UHashElement *entry = nullptr;
    وقتی ((entry = dnCache->cache.nextElement(pos)) != nullptr) {
        ثابت UnicodeString* id = static_cast<ثابت UnicodeString*>(entry->value.pointer);
        اگر (match_کلید != nullptr && !match_کلید->isFallbackOf(*id)) {
            ادامه;
        }
        ثابت UnicodeString* dn = static_cast<ثابت UnicodeString*>(entry->کلید.pointer);
        StringPair* sp = StringPair::create(*id, *dn, status);
        result.adoptElement(sp, status);
        اگر (U_FAILURE(status)) {
            result.removeAllElements();
            بشکن;
        }
    }
    delete match_کلید;

    بازگشت result;
}

URegistry_کلید
ICUService::registerInstance(UObject* objToAdopt, ثابت UnicodeString& id, UErrorCode& status) 
{
    بازگشت registerInstance(objToAdopt, id, درست, status);
}

URegistry_کلید
ICUService::registerInstance(UObject* objToAdopt, ثابت UnicodeString& id, UBool visible, UErrorCode& status) 
{
    ICUService_کلید* کلید = create_کلید(&id, status);
    اگر (کلید != nullptr) {
        UnicodeString canonicalID;
        کلید->canonicalID(canonicalID);
        delete کلید;

        ICUServiceFactory* f = createSimpleFactory(objToAdopt, canonicalID, visible, status);
        اگر (f != nullptr) {
            بازگشت registerFactory(f, status);
        }
    }
    delete objToAdopt;
    بازگشت nullptr;
}

ICUServiceFactory* 
ICUService::createSimpleFactory(UObject* objToAdopt, ثابت UnicodeString& id, UBool visible, UErrorCode& status)
{
    اگر (U_SUCCESS(status)) {
        اگر ((objToAdopt != nullptr) && (!id.isBogus())) {
            بازگشت جدید SimpleFactory(objToAdopt, id, visible);
        }
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    بازگشت nullptr;
}

URegistry_کلید
ICUService::registerFactory(ICUServiceFactory* factoryToAdopt, UErrorCode& status)
{
    LocalPointer<ICUServiceFactory>lpFactoryToAdopt(factoryToAdopt);
    اگر (U_FAILURE(status) || factoryToAdopt == nullptr) {
        بازگشت nullptr;
    }
    {
        Mutex mutex(&lock);

        اگر (factories == nullptr) {
            LocalPointer<UVector> lpFactories(جدید UVector(uprv_deleteUObject, nullptr, status), status);
            اگر (U_FAILURE(status)) {
                بازگشت nullptr;
            }
            factories = lpFactories.orphan();
        }
        factories->درج_Element_At(lpFactoryToAdopt.orphan(), 0, status);
        اگر (U_SUCCESS(status)) {
            clearCaches();
        }
    }   // Close of mutex lock block.

    اگر (U_SUCCESS(status)) {
        notifyChanged();
        بازگشت (URegistry_کلید)factoryToAdopt;
    } وگرنه {
        بازگشت nullptr;
    }
}

UBool 
ICUService::unregister(URegistry_کلید rkey, UErrorCode& status) 
{
    ICUServiceFactory *factory = (ICUServiceFactory*)rkey;
    UBool result = نادرست;
    اگر (factory != nullptr && factories != nullptr) {
        Mutex mutex(&lock);

        اگر (factories->removeElement(factory)) {
            clearCaches();
            result = درست;
        } وگرنه {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            delete factory;
        }
    }
    اگر (result) {
        notifyChanged();
    }
    بازگشت result;
}

تهی 
ICUService::reset() 
{
    {
        Mutex mutex(&lock);
        reInitializeFactories();
        clearCaches();
    }
    notifyChanged();
}

تهی 
ICUService::reInitializeFactories() 
{
    اگر (factories != nullptr) {
        factories->removeAllElements();
    }
}

UBool 
ICUService::isDefault() ثابت 
{
    بازگشت شمار_Factories() == 0;
}

ICUService_کلید* 
ICUService::create_کلید(ثابت UnicodeString* id, UErrorCode& status) ثابت 
{
    بازگشت (U_FAILURE(status) || id == nullptr) ? nullptr : جدید ICUService_کلید(*id);
}

تهی 
ICUService::clearCaches() 
{
    // callers synchronize before use
    ++timestamp;
    delete dnCache;
    dnCache = nullptr;
    delete idCache;
    idCache = nullptr;
    delete serviceCache; serviceCache = nullptr;
}

تهی 
ICUService::clearServiceCache() 
{
    // callers synchronize before use
    delete serviceCache; serviceCache = nullptr;
}

UBool 
ICUService::acceptsListener(ثابت EventListener& l) ثابت 
{
    بازگشت dynamic_cast<ثابت ServiceListener*>(&l) != nullptr;
}

تهی 
ICUService::notifyListener(EventListener& l) ثابت 
{
    (static_cast<ServiceListener&>(l)).serviceChanged(*این);
}

UnicodeString&
ICUService::getName(UnicodeString& result) ثابت 
{
    بازگشت result.append(name);
}

int32_t 
ICUService::شمار_Factories() ثابت 
{
    بازگشت factories == nullptr ? 0 : factories->size();
}

int32_t
ICUService::getTimestamp() ثابت
{
    بازگشت timestamp;
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif
