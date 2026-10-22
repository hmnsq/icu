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

#𐡄𐡍 !UCONFIG_NO_SERVICE

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
    أرجع _id;
}

UnicodeString& 
ICUServiceKey::canonicalID(UnicodeString& result) ثابت 
{
    أرجع result.append(_id);
}

UnicodeString& 
ICUServiceKey::currentID(UnicodeString& result) ثابت 
{
    أرجع canonicalID(result);
}

UnicodeString& 
ICUServiceKey::currentDescriptor(UnicodeString& result) ثابت 
{
    prefix(result);
    result.append(PREFIX_DELIMITER);
    أرجع currentID(result);
}

UBool 
ICUServiceKey::fallback() 
{
    أرجع خطأ;
}

UBool 
ICUServiceKey::isFallbackOf(ثابت UnicodeString& id) ثابت 
{
    أرجع id == _id;
}

UnicodeString& 
ICUServiceKey::prefix(UnicodeString& result) ثابت 
{
    أرجع result;
}

UnicodeString& 
ICUServiceKey::parsePrefix(UnicodeString& result) 
{
    int32_t n = result.indexOf(PREFIX_DELIMITER);
    𐡄𐡍 (n < 0) {
        n = 0;
    }
    result.remove(n);
    أرجع result;
}

UnicodeString& 
ICUServiceKey::parseSuffix(UnicodeString& result) 
{
    int32_t n = result.indexOf(PREFIX_DELIMITER);
    𐡄𐡍 (n >= 0) {
        result.remove(0, n+1);
    }
    أرجع result;
}

#ifdef SERVICE_DEBUG
UnicodeString& 
ICUServiceKey::debug(UnicodeString& result) ثابت 
{
    debugClass(result);
    result.append((UnicodeString)" id: ");
    result.append(_id);
    أرجع result;
}

UnicodeString& 
ICUServiceKey::debugClass(UnicodeString& result) ثابت 
{
    أرجع result.append((UnicodeString)"ICUServiceKey");
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
    𐡄𐡍 (U_SUCCESS(status)) {
        UnicodeString temp;
        𐡄𐡍 (_id == key.currentID(temp)) {
            أرجع service->cloneInstance(_instance); 
        }
    }
    أرجع nullptr;
}

فراغ 
SimpleFactory::updateVisibleIDs(Hashtable& result, UErrorCode& status) ثابت 
{
    𐡄𐡍 (_visible) {
        result.put(_id, (فراغ*)هذا, status); // cast away const
    } وإلا {
        result.remove(_id);
    }
}

UnicodeString& 
SimpleFactory::getDisplayName(ثابت UnicodeString& id, ثابت Locale& /* locale */, UnicodeString& result) ثابت 
{
    𐡄𐡍 (_visible && _id == id) {
        result = _id;
    } وإلا {
        result.setToBogus();
    }
    أرجع result;
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
    أرجع toAppendTo;
}

UnicodeString& 
SimpleFactory::debugClass(UnicodeString& toAppendTo) ثابت 
{
    أرجع toAppendTo.append((UnicodeString)"SimpleFactory");
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
        أرجع هذا;
    }

    /**
    * Destructions removes a reference, so don't call this unless
    * you're removing pointer to this somewhere.  Management of that
    * pointer will have to know how to deal with refcounts.  Once
    * the refcount drops to zero, the resource is released.  Return
    * false if the resource has been released.
    */
    CacheEntry* unref() {
        𐡄𐡍 ((--refcount) == 0) {
            delete هذا;
            أرجع nullptr;
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
    𐡄𐡍 (U_SUCCESS(status)) {
        StringPair* sp = جديد StringPair(displayName, id);
        𐡄𐡍 (sp == nullptr || sp->isBogus()) {
            status = U_MEMORY_ALLOCATION_ERROR;
            delete sp;
            أرجع nullptr;
        }
        أرجع sp;
    }
    أرجع nullptr;
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
    أرجع get(descriptor, nullptr, status);
}

UObject* 
ICUService::get(ثابت UnicodeString& descriptor, UnicodeString* actualReturn, UErrorCode& status) ثابت 
{
    UObject* result = nullptr;
    ICUServiceKey* key = createKey(&descriptor, status);
    𐡄𐡍 (key) {
        result = getKey(*key, actualReturn, status);
        delete key;
    }
    أرجع result;
}

UObject* 
ICUService::getKey(ICUServiceKey& key, UErrorCode& status) ثابت 
{
    أرجع getKey(key, nullptr, status);
}

// this is a vector that subclasses of ICUService can override to further customize the result object
// before returning it.  All other public get functions should call this one.

UObject* 
ICUService::getKey(ICUServiceKey& key, UnicodeString* actualReturn, UErrorCode& status) ثابت 
{
    أرجع getKey(key, actualReturn, nullptr, status);
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
        𐡄𐡍 (fActive) umtx_lock(fMutex);
    }
    مضمن ~XMutex() {
        𐡄𐡍 (fActive) umtx_unlock(fMutex);
    }

خاص:
    UMutex  *fMutex;
    UBool fActive;
};

// called only by factories, treat as private
UObject* 
ICUService::getKey(ICUServiceKey& key, UnicodeString* actualReturn, ثابت ICUServiceFactory* factory, UErrorCode& status) ثابت 
{
    𐡄𐡍 (U_FAILURE(status)) {
        أرجع nullptr;
    }

    𐡄𐡍 (isDefault()) {
        أرجع handleDefault(key, actualReturn, status);
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

        𐡄𐡍 (serviceCache == nullptr) {
            ncthis->serviceCache = جديد Hashtable(status);
            𐡄𐡍 (ncthis->serviceCache == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                أرجع nullptr;
            }
            𐡄𐡍 (U_FAILURE(status)) {
                delete serviceCache;
                أرجع nullptr;
            }
            serviceCache->setValueDeleter(cacheDeleter);
        }

        UnicodeString currentDescriptor;
        LocalPointer<UVector> cacheDescriptorList;
        UBool putInCache = خطأ;

        int32_t startIndex = 0;
        int32_t limit = factories->size();
        UBool cacheResult = 𐡒𐡔𐡈;

        𐡄𐡍 (factory != nullptr) {
            لكل (int32_t i = 0; i < limit; ++i) {
                𐡄𐡍 (factory == static_cast<ثابت ICUServiceFactory*>(factories->elementAt(i))) {
                    startIndex = i + 1;
                    اكسر;
                }
            }
            𐡄𐡍 (startIndex == 0) {
                // throw new InternalError("Factory " + factory + "not registered with service: " + this);
                status = U_ILLEGAL_ARGUMENT_ERROR;
                أرجع nullptr;
            }
            cacheResult = خطأ;
        }

        𐡏𐡁𐡃 {
            currentDescriptor.remove();
            key.currentDescriptor(currentDescriptor);
            result = static_cast<CacheEntry*>(serviceCache->get(currentDescriptor));
            𐡄𐡍 (result != nullptr) {
                اكسر;
            }

            // first test of cache failed, so we'll have to update
            // the cache if we eventually succeed-- that is, if we're 
            // going to update the cache at all.
            putInCache = 𐡒𐡔𐡈;

            int32_t index = startIndex;
            طالما (index < limit) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(index++));
                LocalPointer<UObject> service(f->create(key, هذا, status));
                𐡄𐡍 (U_FAILURE(status)) {
                    أرجع nullptr;
                }
                𐡄𐡍 (service.isValid()) {
                    result = جديد CacheEntry(currentDescriptor, service.getAlias());
                    𐡄𐡍 (result == nullptr) {
                        status = U_MEMORY_ALLOCATION_ERROR;
                        أرجع nullptr;
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
            𐡄𐡍 (cacheDescriptorList.isNull()) {
                cacheDescriptorList.adoptInsteadAndCheckErrorCode(جديد UVector(uprv_deleteUObject, nullptr, 5, status), status);
                𐡄𐡍 (U_FAILURE(status)) {
                    أرجع nullptr;
                }
            }

            LocalPointer<UnicodeString> idToCache(جديد UnicodeString(currentDescriptor), status);
            𐡄𐡍 (U_FAILURE(status)) {
                أرجع nullptr;
            }
            𐡄𐡍 (idToCache->isBogus()) {
                status = U_MEMORY_ALLOCATION_ERROR;
                أرجع nullptr;
            }
            cacheDescriptorList->adoptElement(idToCache.orphan(), status);
            𐡄𐡍 (U_FAILURE(status)) {
                أرجع nullptr;
            }
        } طالما (key.fallback());
outerEnd:

        𐡄𐡍 (result != nullptr) {
            𐡄𐡍 (putInCache && cacheResult) {
                serviceCache->put(result->actualDescriptor, result, status);
                𐡄𐡍 (U_FAILURE(status)) {
                    أرجع nullptr;
                }

                𐡄𐡍 (cacheDescriptorList.isValid()) {
                    لكل (int32_t i = cacheDescriptorList->size(); --i >= 0;) {
                        UnicodeString* desc = static_cast<UnicodeString*>(cacheDescriptorList->elementAt(i));

                        serviceCache->put(*desc, result, status);
                        𐡄𐡍 (U_FAILURE(status)) {
                            أرجع nullptr;
                        }

                        result->ref();
                        cacheDescriptorList->removeElementAt(i);
                    }
                }
            }

            𐡄𐡍 (actualReturn != nullptr) {
                // strip null prefix
                𐡄𐡍 (result->actualDescriptor.indexOf(static_cast<char16_t>(0x2f)) == 0) { // U+002f=slash (/)
                    actualReturn->remove();
                    actualReturn->append(result->actualDescriptor, 
                        1, 
                        result->actualDescriptor.length() - 1);
                } وإلا {
                    *actualReturn = result->actualDescriptor;
                }

                𐡄𐡍 (actualReturn->isBogus()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    delete result;
                    أرجع nullptr;
                }
            }

            UObject* service = cloneInstance(result->service);
            𐡄𐡍 (putInCache && !cacheResult) {
                delete result;
            }
            أرجع service;
        }
    }

    أرجع handleDefault(key, actualReturn, status);
}

UObject* 
ICUService::handleDefault(ثابت ICUServiceKey& /* key */, UnicodeString* /* actualIDReturn */, UErrorCode& /* status */) ثابت 
{
    أرجع nullptr;
}

UVector& 
ICUService::getVisibleIDs(UVector& result, UErrorCode& status) ثابت {
    أرجع getVisibleIDs(result, nullptr, status);
}

UVector& 
ICUService::getVisibleIDs(UVector& result, ثابت UnicodeString* matchID, UErrorCode& status) ثابت 
{
    result.removeAllElements();

    𐡄𐡍 (U_FAILURE(status)) {
        أرجع result;
    }
    UObjectDeleter *savedDeleter = result.setDeleter(uprv_deleteUObject);

    {
        Mutex mutex(&lock);
        ثابت Hashtable* map = getVisibleIDMap(status);
        𐡄𐡍 (map != nullptr) {
            ICUServiceKey* fallbackKey = createKey(matchID, status);

            لكل (int32_t pos = UHASH_FIRST; U_SUCCESS(status); ) {
                ثابت UHashElement* e = map->nextElement(pos);
                𐡄𐡍 (e == nullptr) {
                    اكسر;
                }

                ثابت UnicodeString* id = static_cast<ثابت UnicodeString*>(e->key.pointer);
                𐡄𐡍 (fallbackKey != nullptr) {
                    𐡄𐡍 (!fallbackKey->isFallbackOf(*id)) {
                        استمر;
                    }
                }

                LocalPointer<UnicodeString> idClone(id->clone(), status);
                result.adoptElement(idClone.orphan(), status);
            }
            delete fallbackKey;
        }
    }
    𐡄𐡍 (U_FAILURE(status)) {
        result.removeAllElements();
    }
    result.setDeleter(savedDeleter);
    أرجع result;
}

ثابت Hashtable* 
ICUService::getVisibleIDMap(UErrorCode& status) ثابت {
    𐡄𐡍 (U_FAILURE(status)) أرجع nullptr;

    // must only be called when lock is already held

    ICUService* ncthis = const_cast<ICUService*>(هذا); // cast away semantic const
    𐡄𐡍 (idCache == nullptr) {
        ncthis->idCache = جديد Hashtable(status);
        𐡄𐡍 (idCache == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        } وإلا 𐡄𐡍 (factories != nullptr) {
            لكل (int32_t pos = factories->size(); --pos >= 0;) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(pos));
                f->updateVisibleIDs(*idCache, status);
            }
            𐡄𐡍 (U_FAILURE(status)) {
                delete idCache;
                ncthis->idCache = nullptr;
            }
        }
    }

    أرجع idCache;
}


UnicodeString& 
ICUService::getDisplayName(ثابت UnicodeString& id, UnicodeString& result) ثابت 
{
    أرجع getDisplayName(id, result, Locale::getDefault());
}

UnicodeString& 
ICUService::getDisplayName(ثابت UnicodeString& id, UnicodeString& result, ثابت Locale& locale) ثابت 
{
    {
        UErrorCode status = U_ZERO_ERROR;
        Mutex mutex(&lock);
        ثابت Hashtable* map = getVisibleIDMap(status);
        𐡄𐡍 (map != nullptr) {
            ICUServiceFactory* f = static_cast<ICUServiceFactory*>(map->get(id));
            𐡄𐡍 (f != nullptr) {
                f->getDisplayName(id, locale, result);
                أرجع result;
            }

            // fallback
            status = U_ZERO_ERROR;
            ICUServiceKey* fallbackKey = createKey(&id, status);
            طالما (fallbackKey != nullptr && fallbackKey->fallback()) {
                UnicodeString us;
                fallbackKey->currentID(us);
                f = static_cast<ICUServiceFactory*>(map->get(us));
                𐡄𐡍 (f != nullptr) {
                    f->getDisplayName(id, locale, result);
                    delete fallbackKey;
                    أرجع result;
                }
            }
            delete fallbackKey;
        }
    }
    result.setToBogus();
    أرجع result;
}

UVector& 
ICUService::getDisplayNames(UVector& result, UErrorCode& status) ثابت 
{
    أرجع getDisplayNames(result, Locale::getDefault(), nullptr, status);
}


UVector& 
ICUService::getDisplayNames(UVector& result, ثابت Locale& locale, UErrorCode& status) ثابت 
{
    أرجع getDisplayNames(result, locale, nullptr, status);
}

UVector& 
ICUService::getDisplayNames(UVector& result, 
                            ثابت Locale& locale, 
                            ثابت UnicodeString* matchID, 
                            UErrorCode& status) ثابت 
{
    result.removeAllElements();
    result.setDeleter(userv_deleteStringPair);
    𐡄𐡍 (U_SUCCESS(status)) {
        ICUService* ncthis = const_cast<ICUService*>(هذا); // cast away semantic const
        Mutex mutex(&lock);

        𐡄𐡍 (dnCache != nullptr && dnCache->locale != locale) {
            delete dnCache;
            ncthis->dnCache = nullptr;
        }

        𐡄𐡍 (dnCache == nullptr) {
            ثابت Hashtable* m = getVisibleIDMap(status);
            𐡄𐡍 (U_FAILURE(status)) {
                أرجع result;
            }
            ncthis->dnCache = جديد DNCache(locale); 
            𐡄𐡍 (dnCache == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                أرجع result;
            }

            int32_t pos = UHASH_FIRST;
            ثابت UHashElement* entry = nullptr;
            طالما ((entry = m->nextElement(pos)) != nullptr) {
                ثابت UnicodeString* id = static_cast<ثابت UnicodeString*>(entry->key.pointer);
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(entry->value.pointer);
                UnicodeString dname;
                f->getDisplayName(*id, locale, dname);
                𐡄𐡍 (dname.isBogus()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                } وإلا {
                    dnCache->cache.put(dname, (فراغ*)id, status); // share pointer with visibleIDMap
                    𐡄𐡍 (U_SUCCESS(status)) {
                        استمر;
                    }
                }
                delete dnCache;
                ncthis->dnCache = nullptr;
                أرجع result;
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
    طالما ((entry = dnCache->cache.nextElement(pos)) != nullptr) {
        ثابت UnicodeString* id = static_cast<ثابت UnicodeString*>(entry->value.pointer);
        𐡄𐡍 (matchKey != nullptr && !matchKey->isFallbackOf(*id)) {
            استمر;
        }
        ثابت UnicodeString* dn = static_cast<ثابت UnicodeString*>(entry->key.pointer);
        StringPair* sp = StringPair::create(*id, *dn, status);
        result.adoptElement(sp, status);
        𐡄𐡍 (U_FAILURE(status)) {
            result.removeAllElements();
            اكسر;
        }
    }
    delete matchKey;

    أرجع result;
}

URegistryKey
ICUService::registerInstance(UObject* objToAdopt, ثابت UnicodeString& id, UErrorCode& status) 
{
    أرجع registerInstance(objToAdopt, id, 𐡒𐡔𐡈, status);
}

URegistryKey
ICUService::registerInstance(UObject* objToAdopt, ثابت UnicodeString& id, UBool visible, UErrorCode& status) 
{
    ICUServiceKey* key = createKey(&id, status);
    𐡄𐡍 (key != nullptr) {
        UnicodeString canonicalID;
        key->canonicalID(canonicalID);
        delete key;

        ICUServiceFactory* f = createSimpleFactory(objToAdopt, canonicalID, visible, status);
        𐡄𐡍 (f != nullptr) {
            أرجع registerFactory(f, status);
        }
    }
    delete objToAdopt;
    أرجع nullptr;
}

ICUServiceFactory* 
ICUService::createSimpleFactory(UObject* objToAdopt, ثابت UnicodeString& id, UBool visible, UErrorCode& status)
{
    𐡄𐡍 (U_SUCCESS(status)) {
        𐡄𐡍 ((objToAdopt != nullptr) && (!id.isBogus())) {
            أرجع جديد SimpleFactory(objToAdopt, id, visible);
        }
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    أرجع nullptr;
}

URegistryKey
ICUService::registerFactory(ICUServiceFactory* factoryToAdopt, UErrorCode& status)
{
    LocalPointer<ICUServiceFactory>lpFactoryToAdopt(factoryToAdopt);
    𐡄𐡍 (U_FAILURE(status) || factoryToAdopt == nullptr) {
        أرجع nullptr;
    }
    {
        Mutex mutex(&lock);

        𐡄𐡍 (factories == nullptr) {
            LocalPointer<UVector> lpFactories(جديد UVector(uprv_deleteUObject, nullptr, status), status);
            𐡄𐡍 (U_FAILURE(status)) {
                أرجع nullptr;
            }
            factories = lpFactories.orphan();
        }
        factories->insertElementAt(lpFactoryToAdopt.orphan(), 0, status);
        𐡄𐡍 (U_SUCCESS(status)) {
            clearCaches();
        }
    }   // Close of mutex lock block.

    𐡄𐡍 (U_SUCCESS(status)) {
        notifyChanged();
        أرجع (URegistryKey)factoryToAdopt;
    } وإلا {
        أرجع nullptr;
    }
}

UBool 
ICUService::unregister(URegistryKey rkey, UErrorCode& status) 
{
    ICUServiceFactory *factory = (ICUServiceFactory*)rkey;
    UBool result = خطأ;
    𐡄𐡍 (factory != nullptr && factories != nullptr) {
        Mutex mutex(&lock);

        𐡄𐡍 (factories->removeElement(factory)) {
            clearCaches();
            result = 𐡒𐡔𐡈;
        } وإلا {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            delete factory;
        }
    }
    𐡄𐡍 (result) {
        notifyChanged();
    }
    أرجع result;
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
    𐡄𐡍 (factories != nullptr) {
        factories->removeAllElements();
    }
}

UBool 
ICUService::isDefault() ثابت 
{
    أرجع countFactories() == 0;
}

ICUServiceKey* 
ICUService::createKey(ثابت UnicodeString* id, UErrorCode& status) ثابت 
{
    أرجع (U_FAILURE(status) || id == nullptr) ? nullptr : جديد ICUServiceKey(*id);
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
    أرجع dynamic_cast<ثابت ServiceListener*>(&l) != nullptr;
}

فراغ 
ICUService::notifyListener(EventListener& l) ثابت 
{
    (static_cast<ServiceListener&>(l)).serviceChanged(*هذا);
}

UnicodeString&
ICUService::getName(UnicodeString& result) ثابت 
{
    أرجع result.append(name);
}

int32_t 
ICUService::countFactories() ثابت 
{
    أرجع factories == nullptr ? 0 : factories->size();
}

int32_t
ICUService::getTimestamp() ثابت
{
    أرجع timestamp;
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif
