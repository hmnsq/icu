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

#ܐܢ !UCONFIG_NO_SERVICE

#ضمن "serv.h"
#ضمن "umutex.h"

#undef SERVICE_REFCOUNT

// in case we use the refcount stuff

U_NAMESPACE_BEGIN

/*
******************************************************************
*/

ثابت char16_t ICUServiceKey::PREFIX_DELIMITER = 0x002F;   /* '/' */

ICUServiceKey::ICUServiceKey(ثابت UnicodeString& id) 
: _id(id) {
}

ICUServiceKey::~ICUServiceKey() 
{
}

ثابت UnicodeString& 
ICUServiceKey::getID() ثابت 
{
    ܦܢܐ _id;
}

UnicodeString& 
ICUServiceKey::canonicalID(UnicodeString& result) ثابت 
{
    ܦܢܐ result.append(_id);
}

UnicodeString& 
ICUServiceKey::currentID(UnicodeString& result) ثابت 
{
    ܦܢܐ canonicalID(result);
}

UnicodeString& 
ICUServiceKey::currentDescriptor(UnicodeString& result) ثابت 
{
    prefix(result);
    result.append(PREFIX_DELIMITER);
    ܦܢܐ currentID(result);
}

UBool 
ICUServiceKey::fallback() 
{
    ܦܢܐ ܕܓܠܐ;
}

UBool 
ICUServiceKey::isFallbackOf(ثابت UnicodeString& id) ثابت 
{
    ܦܢܐ id == _id;
}

UnicodeString& 
ICUServiceKey::prefix(UnicodeString& result) ثابت 
{
    ܦܢܐ result;
}

UnicodeString& 
ICUServiceKey::parsePrefix(UnicodeString& result) 
{
    int32_t n = result.indexOf(PREFIX_DELIMITER);
    ܐܢ (n < 0) {
        n = 0;
    }
    result.remove(n);
    ܦܢܐ result;
}

UnicodeString& 
ICUServiceKey::parseSuffix(UnicodeString& result) 
{
    int32_t n = result.indexOf(PREFIX_DELIMITER);
    ܐܢ (n >= 0) {
        result.remove(0, n+1);
    }
    ܦܢܐ result;
}

#ifdef SERVICE_DEBUG
UnicodeString& 
ICUServiceKey::debug(UnicodeString& result) ثابت 
{
    debugClass(result);
    result.append((UnicodeString)" id: ");
    result.append(_id);
    ܦܢܐ result;
}

UnicodeString& 
ICUServiceKey::debugClass(UnicodeString& result) ثابت 
{
    ܦܢܐ result.append((UnicodeString)"ICUServiceKey");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ICUServiceKey)

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
SimpleFactory::create(ثابت ICUServiceKey& key, ثابت ICUService* service, UErrorCode& status) ثابت 
{
    ܐܢ (U_SUCCESS(status)) {
        UnicodeString temp;
        ܐܢ (_id == key.currentID(temp)) {
            ܦܢܐ service->cloneInstance(_instance); 
        }
    }
    ܦܢܐ nullptr;
}

فراغ 
SimpleFactory::updateVisibleIDs(Hashtable& result, UErrorCode& status) ثابت 
{
    ܐܢ (_visible) {
        result.put(_id, (فراغ*)هذا, status); // cast away const
    } ܐܚܪܢܐ {
        result.remove(_id);
    }
}

UnicodeString& 
SimpleFactory::getDisplayName(ثابت UnicodeString& id, ثابت Locale& /* locale */, UnicodeString& result) ثابت 
{
    ܐܢ (_visible && _id == id) {
        result = _id;
    } ܐܚܪܢܐ {
        result.setToBogus();
    }
    ܦܢܐ result;
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
    ܦܢܐ toAppendTo;
}

UnicodeString& 
SimpleFactory::debugClass(UnicodeString& toAppendTo) ثابت 
{
    ܦܢܐ toAppendTo.append((UnicodeString)"SimpleFactory");
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
صنف CacheEntry : عام UMemory {
خاص:
    int32_t refcount;

عام:
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
        ܦܢܐ هذا;
    }

    /**
    * Destructions removes a reference, so don't call this unless
    * you're removing pointer to this somewhere.  Management of that
    * pointer will have to know how to deal with refcounts.  Once
    * the refcount drops to zero, the resource is released.  Return
    * false if the resource has been released.
    */
    CacheEntry* unref() {
        ܐܢ ((--refcount) == 0) {
            delete هذا;
            ܦܢܐ nullptr;
        }
        ܦܢܐ هذا;
    }

    /**
    * Return true if there is at least one reference to this and the
    * resource has not been released.
    */
    UBool isShared() ثابت {
        ܦܢܐ refcount > 1;
    }
};

// Deleter for serviceCache
U_CDECL_BEGIN
ساكن فراغ U_CALLCONV
cacheDeleter(فراغ* obj) {
    U_NAMESPACE_USE ((CacheEntry*)obj)->unref();
}

U_CDECL_END

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
                   UErrorCode& status)
{
    ܐܢ (U_SUCCESS(status)) {
        StringPair* sp = جديد StringPair(displayName, id);
        ܐܢ (sp == nullptr || sp->isBogus()) {
            status = U_MEMORY_ALLOCATION_ERROR;
            delete sp;
            ܦܢܐ nullptr;
        }
        ܦܢܐ sp;
    }
    ܦܢܐ nullptr;
}

UBool 
StringPair::isBogus() ثابت {
    ܦܢܐ displayName.isBogus() || id.isBogus();
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
    U_NAMESPACE_USE delete (StringPair*) obj;
}
U_CDECL_END

/*
******************************************************************
*/

ساكن UMutex lock;

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
    ܦܢܐ get(descriptor, nullptr, status);
}

UObject* 
ICUService::get(ثابت UnicodeString& descriptor, UnicodeString* actualReturn, UErrorCode& status) ثابت 
{
    UObject* result = nullptr;
    ICUServiceKey* key = createKey(&descriptor, status);
    ܐܢ (key) {
        result = getKey(*key, actualReturn, status);
        delete key;
    }
    ܦܢܐ result;
}

UObject* 
ICUService::getKey(ICUServiceKey& key, UErrorCode& status) ثابت 
{
    ܦܢܐ getKey(key, nullptr, status);
}

// this is a vector that subclasses of ICUService can override to further customize the result object
// before returning it.  All other public get functions should call this one.

UObject* 
ICUService::getKey(ICUServiceKey& key, UnicodeString* actualReturn, UErrorCode& status) ثابت 
{
    ܦܢܐ getKey(key, actualReturn, nullptr, status);
}

// make it possible to call reentrantly on systems that don't have reentrant mutexes.
// we can use this simple approach since we know the situation where we're calling
// reentrantly even without knowing the thread.
صنف XMutex : عام UMemory {
عام:
    مضمن XMutex(UMutex *mutex, UBool reentering) 
        : fMutex(mutex)
        , fActive(!reentering) 
    {
        ܐܢ (fActive) umtx_lock(fMutex);
    }
    مضمن ~XMutex() {
        ܐܢ (fActive) umtx_unlock(fMutex);
    }

خاص:
    UMutex  *fMutex;
    UBool fActive;
};

// called only by factories, treat as private
UObject* 
ICUService::getKey(ICUServiceKey& key, UnicodeString* actualReturn, ثابت ICUServiceFactory* factory, UErrorCode& status) ثابت 
{
    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ nullptr;
    }

    ܐܢ (isDefault()) {
        ܦܢܐ handleDefault(key, actualReturn, status);
    }

    ICUService* ncthis = const_cast<ICUService*>(هذا); // cast away semantic const

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

        ܐܢ (serviceCache == nullptr) {
            ncthis->serviceCache = جديد Hashtable(status);
            ܐܢ (ncthis->serviceCache == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                ܦܢܐ nullptr;
            }
            ܐܢ (U_FAILURE(status)) {
                delete serviceCache;
                ܦܢܐ nullptr;
            }
            serviceCache->setValueDeleter(cacheDeleter);
        }

        UnicodeString currentDescriptor;
        LocalPointer<UVector> cacheDescriptorList;
        UBool putInCache = ܕܓܠܐ;

        int32_t startIndex = 0;
        int32_t limit = factories->size();
        UBool cacheResult = ܫܪܪܐ;

        ܐܢ (factory != nullptr) {
            لكل (int32_t i = 0; i < limit; ++i) {
                ܐܢ (factory == static_cast<ثابت ICUServiceFactory*>(factories->elementAt(i))) {
                    startIndex = i + 1;
                    اكسر;
                }
            }
            ܐܢ (startIndex == 0) {
                // throw new InternalError("Factory " + factory + "not registered with service: " + this);
                status = U_ILLEGAL_ARGUMENT_ERROR;
                ܦܢܐ nullptr;
            }
            cacheResult = ܕܓܠܐ;
        }

        ܥܒܕ {
            currentDescriptor.remove();
            key.currentDescriptor(currentDescriptor);
            result = static_cast<CacheEntry*>(serviceCache->get(currentDescriptor));
            ܐܢ (result != nullptr) {
                اكسر;
            }

            // first test of cache failed, so we'll have to update
            // the cache if we eventually succeed-- that is, if we're 
            // going to update the cache at all.
            putInCache = ܫܪܪܐ;

            int32_t index = startIndex;
            ܟܡܐ (index < limit) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(index++));
                LocalPointer<UObject> service(f->create(key, هذا, status));
                ܐܢ (U_FAILURE(status)) {
                    ܦܢܐ nullptr;
                }
                ܐܢ (service.isValid()) {
                    result = جديد CacheEntry(currentDescriptor, service.getAlias());
                    ܐܢ (result == nullptr) {
                        status = U_MEMORY_ALLOCATION_ERROR;
                        ܦܢܐ nullptr;
                    }
                    service.orphan(); // result now owns service.

                    اذهب outerEnd;
                }
            }

            // prepare to load the cache with all additional ids that 
            // will resolve to result, assuming we'll succeed.  We
            // don't want to keep querying on an id that's going to
            // fallback to the one that succeeded, we want to hit the
            // cache the first time next goaround.
            ܐܢ (cacheDescriptorList.isNull()) {
                cacheDescriptorList.adoptInsteadAndCheckErrorCode(جديد UVector(uprv_deleteUObject, nullptr, 5, status), status);
                ܐܢ (U_FAILURE(status)) {
                    ܦܢܐ nullptr;
                }
            }

            LocalPointer<UnicodeString> idToCache(جديد UnicodeString(currentDescriptor), status);
            ܐܢ (U_FAILURE(status)) {
                ܦܢܐ nullptr;
            }
            ܐܢ (idToCache->isBogus()) {
                status = U_MEMORY_ALLOCATION_ERROR;
                ܦܢܐ nullptr;
            }
            cacheDescriptorList->adoptElement(idToCache.orphan(), status);
            ܐܢ (U_FAILURE(status)) {
                ܦܢܐ nullptr;
            }
        } ܟܡܐ (key.fallback());
outerEnd:

        ܐܢ (result != nullptr) {
            ܐܢ (putInCache && cacheResult) {
                serviceCache->put(result->actualDescriptor, result, status);
                ܐܢ (U_FAILURE(status)) {
                    ܦܢܐ nullptr;
                }

                ܐܢ (cacheDescriptorList.isValid()) {
                    لكل (int32_t i = cacheDescriptorList->size(); --i >= 0;) {
                        UnicodeString* desc = static_cast<UnicodeString*>(cacheDescriptorList->elementAt(i));

                        serviceCache->put(*desc, result, status);
                        ܐܢ (U_FAILURE(status)) {
                            ܦܢܐ nullptr;
                        }

                        result->ref();
                        cacheDescriptorList->removeElementAt(i);
                    }
                }
            }

            ܐܢ (actualReturn != nullptr) {
                // strip null prefix
                ܐܢ (result->actualDescriptor.indexOf(static_cast<char16_t>(0x2f)) == 0) { // U+002f=slash (/)
                    actualReturn->remove();
                    actualReturn->append(result->actualDescriptor, 
                        1, 
                        result->actualDescriptor.length() - 1);
                } ܐܚܪܢܐ {
                    *actualReturn = result->actualDescriptor;
                }

                ܐܢ (actualReturn->isBogus()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    delete result;
                    ܦܢܐ nullptr;
                }
            }

            UObject* service = cloneInstance(result->service);
            ܐܢ (putInCache && !cacheResult) {
                delete result;
            }
            ܦܢܐ service;
        }
    }

    ܦܢܐ handleDefault(key, actualReturn, status);
}

UObject* 
ICUService::handleDefault(ثابت ICUServiceKey& /* key */, UnicodeString* /* actualIDReturn */, UErrorCode& /* status */) ثابت 
{
    ܦܢܐ nullptr;
}

UVector& 
ICUService::getVisibleIDs(UVector& result, UErrorCode& status) ثابت {
    ܦܢܐ getVisibleIDs(result, nullptr, status);
}

UVector& 
ICUService::getVisibleIDs(UVector& result, ثابت UnicodeString* matchID, UErrorCode& status) ثابت 
{
    result.removeAllElements();

    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ result;
    }
    UObjectDeleter *savedDeleter = result.setDeleter(uprv_deleteUObject);

    {
        Mutex mutex(&lock);
        ثابت Hashtable* map = getVisibleIDMap(status);
        ܐܢ (map != nullptr) {
            ICUServiceKey* fallbackKey = createKey(matchID, status);

            لكل (int32_t pos = UHASH_FIRST; U_SUCCESS(status); ) {
                ثابت UHashElement* e = map->nextElement(pos);
                ܐܢ (e == nullptr) {
                    اكسر;
                }

                ثابت UnicodeString* id = static_cast<ثابت UnicodeString*>(e->key.pointer);
                ܐܢ (fallbackKey != nullptr) {
                    ܐܢ (!fallbackKey->isFallbackOf(*id)) {
                        استمر;
                    }
                }

                LocalPointer<UnicodeString> idClone(id->clone(), status);
                result.adoptElement(idClone.orphan(), status);
            }
            delete fallbackKey;
        }
    }
    ܐܢ (U_FAILURE(status)) {
        result.removeAllElements();
    }
    result.setDeleter(savedDeleter);
    ܦܢܐ result;
}

ثابت Hashtable* 
ICUService::getVisibleIDMap(UErrorCode& status) ثابت {
    ܐܢ (U_FAILURE(status)) ܦܢܐ nullptr;

    // must only be called when lock is already held

    ICUService* ncthis = const_cast<ICUService*>(هذا); // cast away semantic const
    ܐܢ (idCache == nullptr) {
        ncthis->idCache = جديد Hashtable(status);
        ܐܢ (idCache == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        } ܐܚܪܢܐ ܐܢ (factories != nullptr) {
            لكل (int32_t pos = factories->size(); --pos >= 0;) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(pos));
                f->updateVisibleIDs(*idCache, status);
            }
            ܐܢ (U_FAILURE(status)) {
                delete idCache;
                ncthis->idCache = nullptr;
            }
        }
    }

    ܦܢܐ idCache;
}


UnicodeString& 
ICUService::getDisplayName(ثابت UnicodeString& id, UnicodeString& result) ثابت 
{
    ܦܢܐ getDisplayName(id, result, Locale::getDefault());
}

UnicodeString& 
ICUService::getDisplayName(ثابت UnicodeString& id, UnicodeString& result, ثابت Locale& locale) ثابت 
{
    {
        UErrorCode status = U_ZERO_ERROR;
        Mutex mutex(&lock);
        ثابت Hashtable* map = getVisibleIDMap(status);
        ܐܢ (map != nullptr) {
            ICUServiceFactory* f = static_cast<ICUServiceFactory*>(map->get(id));
            ܐܢ (f != nullptr) {
                f->getDisplayName(id, locale, result);
                ܦܢܐ result;
            }

            // fallback
            status = U_ZERO_ERROR;
            ICUServiceKey* fallbackKey = createKey(&id, status);
            ܟܡܐ (fallbackKey != nullptr && fallbackKey->fallback()) {
                UnicodeString us;
                fallbackKey->currentID(us);
                f = static_cast<ICUServiceFactory*>(map->get(us));
                ܐܢ (f != nullptr) {
                    f->getDisplayName(id, locale, result);
                    delete fallbackKey;
                    ܦܢܐ result;
                }
            }
            delete fallbackKey;
        }
    }
    result.setToBogus();
    ܦܢܐ result;
}

UVector& 
ICUService::getDisplayNames(UVector& result, UErrorCode& status) ثابت 
{
    ܦܢܐ getDisplayNames(result, Locale::getDefault(), nullptr, status);
}


UVector& 
ICUService::getDisplayNames(UVector& result, ثابت Locale& locale, UErrorCode& status) ثابت 
{
    ܦܢܐ getDisplayNames(result, locale, nullptr, status);
}

UVector& 
ICUService::getDisplayNames(UVector& result, 
                            ثابت Locale& locale, 
                            ثابت UnicodeString* matchID, 
                            UErrorCode& status) ثابت 
{
    result.removeAllElements();
    result.setDeleter(userv_deleteStringPair);
    ܐܢ (U_SUCCESS(status)) {
        ICUService* ncthis = const_cast<ICUService*>(هذا); // cast away semantic const
        Mutex mutex(&lock);

        ܐܢ (dnCache != nullptr && dnCache->locale != locale) {
            delete dnCache;
            ncthis->dnCache = nullptr;
        }

        ܐܢ (dnCache == nullptr) {
            ثابت Hashtable* m = getVisibleIDMap(status);
            ܐܢ (U_FAILURE(status)) {
                ܦܢܐ result;
            }
            ncthis->dnCache = جديد DNCache(locale); 
            ܐܢ (dnCache == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                ܦܢܐ result;
            }

            int32_t pos = UHASH_FIRST;
            ثابت UHashElement* entry = nullptr;
            ܟܡܐ ((entry = m->nextElement(pos)) != nullptr) {
                ثابت UnicodeString* id = static_cast<ثابت UnicodeString*>(entry->key.pointer);
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(entry->value.pointer);
                UnicodeString dname;
                f->getDisplayName(*id, locale, dname);
                ܐܢ (dname.isBogus()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                } ܐܚܪܢܐ {
                    dnCache->cache.put(dname, (فراغ*)id, status); // share pointer with visibleIDMap
                    ܐܢ (U_SUCCESS(status)) {
                        استمر;
                    }
                }
                delete dnCache;
                ncthis->dnCache = nullptr;
                ܦܢܐ result;
            }
        }
    }

    ICUServiceKey* matchKey = createKey(matchID, status);
    /* To ensure that all elements in the hashtable are iterated, set pos to -1.
     * nextElement(pos) will skip the position at pos and begin the iteration
     * at the next position, which in this case will be 0.
     */
    int32_t pos = UHASH_FIRST; 
    ثابت UHashElement *entry = nullptr;
    ܟܡܐ ((entry = dnCache->cache.nextElement(pos)) != nullptr) {
        ثابت UnicodeString* id = static_cast<ثابت UnicodeString*>(entry->value.pointer);
        ܐܢ (matchKey != nullptr && !matchKey->isFallbackOf(*id)) {
            استمر;
        }
        ثابت UnicodeString* dn = static_cast<ثابت UnicodeString*>(entry->key.pointer);
        StringPair* sp = StringPair::create(*id, *dn, status);
        result.adoptElement(sp, status);
        ܐܢ (U_FAILURE(status)) {
            result.removeAllElements();
            اكسر;
        }
    }
    delete matchKey;

    ܦܢܐ result;
}

URegistryKey
ICUService::registerInstance(UObject* objToAdopt, ثابت UnicodeString& id, UErrorCode& status) 
{
    ܦܢܐ registerInstance(objToAdopt, id, ܫܪܪܐ, status);
}

URegistryKey
ICUService::registerInstance(UObject* objToAdopt, ثابت UnicodeString& id, UBool visible, UErrorCode& status) 
{
    ICUServiceKey* key = createKey(&id, status);
    ܐܢ (key != nullptr) {
        UnicodeString canonicalID;
        key->canonicalID(canonicalID);
        delete key;

        ICUServiceFactory* f = createSimpleFactory(objToAdopt, canonicalID, visible, status);
        ܐܢ (f != nullptr) {
            ܦܢܐ registerFactory(f, status);
        }
    }
    delete objToAdopt;
    ܦܢܐ nullptr;
}

ICUServiceFactory* 
ICUService::createSimpleFactory(UObject* objToAdopt, ثابت UnicodeString& id, UBool visible, UErrorCode& status)
{
    ܐܢ (U_SUCCESS(status)) {
        ܐܢ ((objToAdopt != nullptr) && (!id.isBogus())) {
            ܦܢܐ جديد SimpleFactory(objToAdopt, id, visible);
        }
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    ܦܢܐ nullptr;
}

URegistryKey
ICUService::registerFactory(ICUServiceFactory* factoryToAdopt, UErrorCode& status)
{
    LocalPointer<ICUServiceFactory>lpFactoryToAdopt(factoryToAdopt);
    ܐܢ (U_FAILURE(status) || factoryToAdopt == nullptr) {
        ܦܢܐ nullptr;
    }
    {
        Mutex mutex(&lock);

        ܐܢ (factories == nullptr) {
            LocalPointer<UVector> lpFactories(جديد UVector(uprv_deleteUObject, nullptr, status), status);
            ܐܢ (U_FAILURE(status)) {
                ܦܢܐ nullptr;
            }
            factories = lpFactories.orphan();
        }
        factories->insertElementAt(lpFactoryToAdopt.orphan(), 0, status);
        ܐܢ (U_SUCCESS(status)) {
            clearCaches();
        }
    }   // Close of mutex lock block.

    ܐܢ (U_SUCCESS(status)) {
        notifyChanged();
        ܦܢܐ (URegistryKey)factoryToAdopt;
    } ܐܚܪܢܐ {
        ܦܢܐ nullptr;
    }
}

UBool 
ICUService::unregister(URegistryKey rkey, UErrorCode& status) 
{
    ICUServiceFactory *factory = (ICUServiceFactory*)rkey;
    UBool result = ܕܓܠܐ;
    ܐܢ (factory != nullptr && factories != nullptr) {
        Mutex mutex(&lock);

        ܐܢ (factories->removeElement(factory)) {
            clearCaches();
            result = ܫܪܪܐ;
        } ܐܚܪܢܐ {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            delete factory;
        }
    }
    ܐܢ (result) {
        notifyChanged();
    }
    ܦܢܐ result;
}

فراغ 
ICUService::reset() 
{
    {
        Mutex mutex(&lock);
        reInitializeFactories();
        clearCaches();
    }
    notifyChanged();
}

فراغ 
ICUService::reInitializeFactories() 
{
    ܐܢ (factories != nullptr) {
        factories->removeAllElements();
    }
}

UBool 
ICUService::isDefault() ثابت 
{
    ܦܢܐ countFactories() == 0;
}

ICUServiceKey* 
ICUService::createKey(ثابت UnicodeString* id, UErrorCode& status) ثابت 
{
    ܦܢܐ (U_FAILURE(status) || id == nullptr) ? nullptr : جديد ICUServiceKey(*id);
}

فراغ 
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

فراغ 
ICUService::clearServiceCache() 
{
    // callers synchronize before use
    delete serviceCache; serviceCache = nullptr;
}

UBool 
ICUService::acceptsListener(ثابت EventListener& l) ثابت 
{
    ܦܢܐ dynamic_cast<ثابت ServiceListener*>(&l) != nullptr;
}

فراغ 
ICUService::notifyListener(EventListener& l) ثابت 
{
    (static_cast<ServiceListener&>(l)).serviceChanged(*هذا);
}

UnicodeString&
ICUService::getName(UnicodeString& result) ثابت 
{
    ܦܢܐ result.append(name);
}

int32_t 
ICUService::countFactories() ثابت 
{
    ܦܢܐ factories == nullptr ? 0 : factories->size();
}

int32_t
ICUService::getTimestamp() ثابت
{
    ܦܢܐ timestamp;
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif
