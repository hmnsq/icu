// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
*******************************************************************************
* Copyright (C) 2001-2014, International Business Machines Corporation.
* All Rights Reserved.
*******************************************************************************
*/

#include "unicode/utypes.h"
#include "unicode/localpointer.h"

#ᬬᬾᬦ᭄ !UCONFIG_NO_SERVICE

#include "serv.h"
#include "umutex.h"

#undef SERVICE_REFCOUNT

// in case we use the refcount stuff

U_NAMESPACE_BEGIN

/*
******************************************************************
*/

const char16_t ICUServiceKey::PREFIX_DELIMITER = 0x002F;   /* '/' */

ICUServiceKey::ICUServiceKey(const UnicodeString& id) 
: _id(id) {
}

ICUServiceKey::~ICUServiceKey() 
{
}

const UnicodeString& 
ICUServiceKey::getID() const 
{
    ᬯᬮᬶ _id;
}

UnicodeString& 
ICUServiceKey::canonicalID(UnicodeString& result) const 
{
    ᬯᬮᬶ result.append(_id);
}

UnicodeString& 
ICUServiceKey::currentID(UnicodeString& result) const 
{
    ᬯᬮᬶ canonicalID(result);
}

UnicodeString& 
ICUServiceKey::currentDescriptor(UnicodeString& result) const 
{
    prefix(result);
    result.append(PREFIX_DELIMITER);
    ᬯᬮᬶ currentID(result);
}

UBool 
ICUServiceKey::fallback() 
{
    ᬯᬮᬶ false;
}

UBool 
ICUServiceKey::isFallbackOf(const UnicodeString& id) const 
{
    ᬯᬮᬶ id == _id;
}

UnicodeString& 
ICUServiceKey::prefix(UnicodeString& result) const 
{
    ᬯᬮᬶ result;
}

UnicodeString& 
ICUServiceKey::parsePrefix(UnicodeString& result) 
{
    int32_t n = result.indexOf(PREFIX_DELIMITER);
    ᬬᬾᬦ᭄ (n < 0) {
        n = 0;
    }
    result.remove(n);
    ᬯᬮᬶ result;
}

UnicodeString& 
ICUServiceKey::parseSuffix(UnicodeString& result) 
{
    int32_t n = result.indexOf(PREFIX_DELIMITER);
    ᬬᬾᬦ᭄ (n >= 0) {
        result.remove(0, n+1);
    }
    ᬯᬮᬶ result;
}

#ifdef SERVICE_DEBUG
UnicodeString& 
ICUServiceKey::debug(UnicodeString& result) const 
{
    debugClass(result);
    result.append((UnicodeString)" id: ");
    result.append(_id);
    ᬯᬮᬶ result;
}

UnicodeString& 
ICUServiceKey::debugClass(UnicodeString& result) const 
{
    ᬯᬮᬶ result.append((UnicodeString)"ICUServiceKey");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ICUServiceKey)

/*
******************************************************************
*/

ICUServiceFactory::~ICUServiceFactory() {}

SimpleFactory::SimpleFactory(UObject* instanceToAdopt, const UnicodeString& id, UBool visible) 
: _instance(instanceToAdopt), _id(id), _visible(visible)
{
}

SimpleFactory::~SimpleFactory() 
{
    delete _instance;
}

UObject* 
SimpleFactory::create(const ICUServiceKey& key, const ICUService* service, UErrorCode& status) const 
{
    ᬬᬾᬦ᭄ (U_SUCCESS(status)) {
        UnicodeString temp;
        ᬬᬾᬦ᭄ (_id == key.currentID(temp)) {
            ᬯᬮᬶ service->cloneInstance(_instance); 
        }
    }
    ᬯᬮᬶ nullptr;
}

void 
SimpleFactory::updateVisibleIDs(Hashtable& result, UErrorCode& status) const 
{
    ᬬᬾᬦ᭄ (_visible) {
        result.put(_id, (void*)this, status); // cast away const
    } ᬮᬶᬬ {
        result.remove(_id);
    }
}

UnicodeString& 
SimpleFactory::getDisplayName(const UnicodeString& id, const Locale& /* locale */, UnicodeString& result) const 
{
    ᬬᬾᬦ᭄ (_visible && _id == id) {
        result = _id;
    } ᬮᬶᬬ {
        result.setToBogus();
    }
    ᬯᬮᬶ result;
}

#ifdef SERVICE_DEBUG
UnicodeString& 
SimpleFactory::debug(UnicodeString& toAppendTo) const 
{
    debugClass(toAppendTo);
    toAppendTo.append((UnicodeString)" id: ");
    toAppendTo.append(_id);
    toAppendTo.append((UnicodeString)", visible: ");
    toAppendTo.append(_visible ? (UnicodeString)"T" : (UnicodeString)"F");
    ᬯᬮᬶ toAppendTo;
}

UnicodeString& 
SimpleFactory::debugClass(UnicodeString& toAppendTo) const 
{
    ᬯᬮᬶ toAppendTo.append((UnicodeString)"SimpleFactory");
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
class CacheEntry : public UMemory {
private:
    int32_t refcount;

public:
    UnicodeString actualDescriptor;
    UObject* service;

    /**
    * Releases a reference to the shared resource.
    */
    ~CacheEntry() {
        delete service;
    }

    CacheEntry(const UnicodeString& _actualDescriptor, UObject* _service) 
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
        ᬯᬮᬶ this;
    }

    /**
    * Destructions removes a reference, so don't call this unless
    * you're removing pointer to this somewhere.  Management of that
    * pointer will have to know how to deal with refcounts.  Once
    * the refcount drops to zero, the resource is released.  Return
    * false if the resource has been released.
    */
    CacheEntry* unref() {
        ᬬᬾᬦ᭄ ((--refcount) == 0) {
            delete this;
            ᬯᬮᬶ nullptr;
        }
        ᬯᬮᬶ this;
    }

    /**
    * Return true if there is at least one reference to this and the
    * resource has not been released.
    */
    UBool isShared() const {
        ᬯᬮᬶ refcount > 1;
    }
};

// Deleter for serviceCache
U_CDECL_BEGIN
static void U_CALLCONV
cacheDeleter(void* obj) {
    U_NAMESPACE_USE ((CacheEntry*)obj)->unref();
}

U_CDECL_END

/*
******************************************************************
*/

class DNCache : public UMemory {
public:
    Hashtable cache;
    const Locale locale;

    DNCache(const Locale& _locale) 
        : cache(), locale(_locale) 
    {
        // cache.setKeyDeleter(uprv_deleteUObject);
    }
};


/*
******************************************************************
*/

StringPair* 
StringPair::create(const UnicodeString& displayName, 
                   const UnicodeString& id,
                   UErrorCode& status)
{
    ᬬᬾᬦ᭄ (U_SUCCESS(status)) {
        StringPair* sp = new StringPair(displayName, id);
        ᬬᬾᬦ᭄ (sp == nullptr || sp->isBogus()) {
            status = U_MEMORY_ALLOCATION_ERROR;
            delete sp;
            ᬯᬮᬶ nullptr;
        }
        ᬯᬮᬶ sp;
    }
    ᬯᬮᬶ nullptr;
}

UBool 
StringPair::isBogus() const {
    ᬯᬮᬶ displayName.isBogus() || id.isBogus();
}

StringPair::StringPair(const UnicodeString& _displayName, 
                       const UnicodeString& _id)
: displayName(_displayName)
, id(_id)
{
}

U_CDECL_BEGIN
static void U_CALLCONV
userv_deleteStringPair(void *obj) {
    U_NAMESPACE_USE delete (StringPair*) obj;
}
U_CDECL_END

/*
******************************************************************
*/

static UMutex lock;

ICUService::ICUService()
: name()
, timestamp(0)
, factories(nullptr)
, serviceCache(nullptr)
, idCache(nullptr)
, dnCache(nullptr)
{
}

ICUService::ICUService(const UnicodeString& newName) 
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
ICUService::get(const UnicodeString& descriptor, UErrorCode& status) const 
{
    ᬯᬮᬶ get(descriptor, nullptr, status);
}

UObject* 
ICUService::get(const UnicodeString& descriptor, UnicodeString* actualReturn, UErrorCode& status) const 
{
    UObject* result = nullptr;
    ICUServiceKey* key = createKey(&descriptor, status);
    ᬬᬾᬦ᭄ (key) {
        result = getKey(*key, actualReturn, status);
        delete key;
    }
    ᬯᬮᬶ result;
}

UObject* 
ICUService::getKey(ICUServiceKey& key, UErrorCode& status) const 
{
    ᬯᬮᬶ getKey(key, nullptr, status);
}

// this is a vector that subclasses of ICUService can override to further customize the result object
// before returning it.  All other public get functions should call this one.

UObject* 
ICUService::getKey(ICUServiceKey& key, UnicodeString* actualReturn, UErrorCode& status) const 
{
    ᬯᬮᬶ getKey(key, actualReturn, nullptr, status);
}

// make it possible to call reentrantly on systems that don't have reentrant mutexes.
// we can use this simple approach since we know the situation where we're calling
// reentrantly even without knowing the thread.
class XMutex : public UMemory {
public:
    anta_hstha XMutex(UMutex *mutex, UBool reentering) 
        : fMutex(mutex)
        , fActive(!reentering) 
    {
        ᬬᬾᬦ᭄ (fActive) umtx_lock(fMutex);
    }
    anta_hstha ~XMutex() {
        ᬬᬾᬦ᭄ (fActive) umtx_unlock(fMutex);
    }

private:
    UMutex  *fMutex;
    UBool fActive;
};

// called only by factories, treat as private
UObject* 
ICUService::getKey(ICUServiceKey& key, UnicodeString* actualReturn, const ICUServiceFactory* factory, UErrorCode& status) const 
{
    ᬬᬾᬦ᭄ (U_FAILURE(status)) {
        ᬯᬮᬶ nullptr;
    }

    ᬬᬾᬦ᭄ (isDefault()) {
        ᬯᬮᬶ handleDefault(key, actualReturn, status);
    }

    ICUService* ncthis = const_cast<ICUService*>(this); // cast away semantic const

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

        ᬬᬾᬦ᭄ (serviceCache == nullptr) {
            ncthis->serviceCache = new Hashtable(status);
            ᬬᬾᬦ᭄ (ncthis->serviceCache == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                ᬯᬮᬶ nullptr;
            }
            ᬬᬾᬦ᭄ (U_FAILURE(status)) {
                delete serviceCache;
                ᬯᬮᬶ nullptr;
            }
            serviceCache->setValueDeleter(cacheDeleter);
        }

        UnicodeString currentDescriptor;
        LocalPointer<UVector> cacheDescriptorList;
        UBool putInCache = false;

        int32_t startIndex = 0;
        int32_t limit = factories->size();
        UBool cacheResult = true;

        ᬬᬾᬦ᭄ (factory != nullptr) {
            for (int32_t i = 0; i < limit; ++i) {
                ᬬᬾᬦ᭄ (factory == static_cast<const ICUServiceFactory*>(factories->elementAt(i))) {
                    startIndex = i + 1;
                    bha_nga;
                }
            }
            ᬬᬾᬦ᭄ (startIndex == 0) {
                // throw new InternalError("Factory " + factory + "not registered with service: " + this);
                status = U_ILLEGAL_ARGUMENT_ERROR;
                ᬯᬮᬶ nullptr;
            }
            cacheResult = false;
        }

        do {
            currentDescriptor.remove();
            key.currentDescriptor(currentDescriptor);
            result = static_cast<CacheEntry*>(serviceCache->get(currentDescriptor));
            ᬬᬾᬦ᭄ (result != nullptr) {
                bha_nga;
            }

            // first test of cache failed, so we'll have to update
            // the cache if we eventually succeed-- that is, if we're 
            // going to update the cache at all.
            putInCache = true;

            int32_t index = startIndex;
            ᬲᭂᬤᭂᬮ (index < limit) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(index++));
                LocalPointer<UObject> service(f->create(key, this, status));
                ᬬᬾᬦ᭄ (U_FAILURE(status)) {
                    ᬯᬮᬶ nullptr;
                }
                ᬬᬾᬦ᭄ (service.isValid()) {
                    result = new CacheEntry(currentDescriptor, service.getAlias());
                    ᬬᬾᬦ᭄ (result == nullptr) {
                        status = U_MEMORY_ALLOCATION_ERROR;
                        ᬯᬮᬶ nullptr;
                    }
                    service.orphan(); // result now owns service.

                    goto outerEnd;
                }
            }

            // prepare to load the cache with all additional ids that 
            // will resolve to result, assuming we'll succeed.  We
            // don't want to keep querying on an id that's going to
            // fallback to the one that succeeded, we want to hit the
            // cache the first time next goaround.
            ᬬᬾᬦ᭄ (cacheDescriptorList.isNull()) {
                cacheDescriptorList.adoptInsteadAndCheckErrorCode(new UVector(uprv_deleteUObject, nullptr, 5, status), status);
                ᬬᬾᬦ᭄ (U_FAILURE(status)) {
                    ᬯᬮᬶ nullptr;
                }
            }

            LocalPointer<UnicodeString> idToCache(new UnicodeString(currentDescriptor), status);
            ᬬᬾᬦ᭄ (U_FAILURE(status)) {
                ᬯᬮᬶ nullptr;
            }
            ᬬᬾᬦ᭄ (idToCache->isBogus()) {
                status = U_MEMORY_ALLOCATION_ERROR;
                ᬯᬮᬶ nullptr;
            }
            cacheDescriptorList->adoptElement(idToCache.orphan(), status);
            ᬬᬾᬦ᭄ (U_FAILURE(status)) {
                ᬯᬮᬶ nullptr;
            }
        } ᬲᭂᬤᭂᬮ (key.fallback());
outerEnd:

        ᬬᬾᬦ᭄ (result != nullptr) {
            ᬬᬾᬦ᭄ (putInCache && cacheResult) {
                serviceCache->put(result->actualDescriptor, result, status);
                ᬬᬾᬦ᭄ (U_FAILURE(status)) {
                    ᬯᬮᬶ nullptr;
                }

                ᬬᬾᬦ᭄ (cacheDescriptorList.isValid()) {
                    for (int32_t i = cacheDescriptorList->size(); --i >= 0;) {
                        UnicodeString* desc = static_cast<UnicodeString*>(cacheDescriptorList->elementAt(i));

                        serviceCache->put(*desc, result, status);
                        ᬬᬾᬦ᭄ (U_FAILURE(status)) {
                            ᬯᬮᬶ nullptr;
                        }

                        result->ref();
                        cacheDescriptorList->removeElementAt(i);
                    }
                }
            }

            ᬬᬾᬦ᭄ (actualReturn != nullptr) {
                // strip null prefix
                ᬬᬾᬦ᭄ (result->actualDescriptor.indexOf(static_cast<char16_t>(0x2f)) == 0) { // U+002f=slash (/)
                    actualReturn->remove();
                    actualReturn->append(result->actualDescriptor, 
                        1, 
                        result->actualDescriptor.length() - 1);
                } ᬮᬶᬬ {
                    *actualReturn = result->actualDescriptor;
                }

                ᬬᬾᬦ᭄ (actualReturn->isBogus()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    delete result;
                    ᬯᬮᬶ nullptr;
                }
            }

            UObject* service = cloneInstance(result->service);
            ᬬᬾᬦ᭄ (putInCache && !cacheResult) {
                delete result;
            }
            ᬯᬮᬶ service;
        }
    }

    ᬯᬮᬶ handleDefault(key, actualReturn, status);
}

UObject* 
ICUService::handleDefault(const ICUServiceKey& /* key */, UnicodeString* /* actualIDReturn */, UErrorCode& /* status */) const 
{
    ᬯᬮᬶ nullptr;
}

UVector& 
ICUService::getVisibleIDs(UVector& result, UErrorCode& status) const {
    ᬯᬮᬶ getVisibleIDs(result, nullptr, status);
}

UVector& 
ICUService::getVisibleIDs(UVector& result, const UnicodeString* matchID, UErrorCode& status) const 
{
    result.removeAllElements();

    ᬬᬾᬦ᭄ (U_FAILURE(status)) {
        ᬯᬮᬶ result;
    }
    UObjectDeleter *savedDeleter = result.setDeleter(uprv_deleteUObject);

    {
        Mutex mutex(&lock);
        const Hashtable* map = getVisibleIDMap(status);
        ᬬᬾᬦ᭄ (map != nullptr) {
            ICUServiceKey* fallbackKey = createKey(matchID, status);

            for (int32_t pos = UHASH_FIRST; U_SUCCESS(status); ) {
                const UHashElement* e = map->nextElement(pos);
                ᬬᬾᬦ᭄ (e == nullptr) {
                    bha_nga;
                }

                const UnicodeString* id = static_cast<const UnicodeString*>(e->key.pointer);
                ᬬᬾᬦ᭄ (fallbackKey != nullptr) {
                    ᬬᬾᬦ᭄ (!fallbackKey->isFallbackOf(*id)) {
                        continue;
                    }
                }

                LocalPointer<UnicodeString> idClone(id->clone(), status);
                result.adoptElement(idClone.orphan(), status);
            }
            delete fallbackKey;
        }
    }
    ᬬᬾᬦ᭄ (U_FAILURE(status)) {
        result.removeAllElements();
    }
    result.setDeleter(savedDeleter);
    ᬯᬮᬶ result;
}

const Hashtable* 
ICUService::getVisibleIDMap(UErrorCode& status) const {
    ᬬᬾᬦ᭄ (U_FAILURE(status)) ᬯᬮᬶ nullptr;

    // must only be called when lock is already held

    ICUService* ncthis = const_cast<ICUService*>(this); // cast away semantic const
    ᬬᬾᬦ᭄ (idCache == nullptr) {
        ncthis->idCache = new Hashtable(status);
        ᬬᬾᬦ᭄ (idCache == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        } ᬮᬶᬬ ᬬᬾᬦ᭄ (factories != nullptr) {
            for (int32_t pos = factories->size(); --pos >= 0;) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(pos));
                f->updateVisibleIDs(*idCache, status);
            }
            ᬬᬾᬦ᭄ (U_FAILURE(status)) {
                delete idCache;
                ncthis->idCache = nullptr;
            }
        }
    }

    ᬯᬮᬶ idCache;
}


UnicodeString& 
ICUService::getDisplayName(const UnicodeString& id, UnicodeString& result) const 
{
    ᬯᬮᬶ getDisplayName(id, result, Locale::getDefault());
}

UnicodeString& 
ICUService::getDisplayName(const UnicodeString& id, UnicodeString& result, const Locale& locale) const 
{
    {
        UErrorCode status = U_ZERO_ERROR;
        Mutex mutex(&lock);
        const Hashtable* map = getVisibleIDMap(status);
        ᬬᬾᬦ᭄ (map != nullptr) {
            ICUServiceFactory* f = static_cast<ICUServiceFactory*>(map->get(id));
            ᬬᬾᬦ᭄ (f != nullptr) {
                f->getDisplayName(id, locale, result);
                ᬯᬮᬶ result;
            }

            // fallback
            status = U_ZERO_ERROR;
            ICUServiceKey* fallbackKey = createKey(&id, status);
            ᬲᭂᬤᭂᬮ (fallbackKey != nullptr && fallbackKey->fallback()) {
                UnicodeString us;
                fallbackKey->currentID(us);
                f = static_cast<ICUServiceFactory*>(map->get(us));
                ᬬᬾᬦ᭄ (f != nullptr) {
                    f->getDisplayName(id, locale, result);
                    delete fallbackKey;
                    ᬯᬮᬶ result;
                }
            }
            delete fallbackKey;
        }
    }
    result.setToBogus();
    ᬯᬮᬶ result;
}

UVector& 
ICUService::getDisplayNames(UVector& result, UErrorCode& status) const 
{
    ᬯᬮᬶ getDisplayNames(result, Locale::getDefault(), nullptr, status);
}


UVector& 
ICUService::getDisplayNames(UVector& result, const Locale& locale, UErrorCode& status) const 
{
    ᬯᬮᬶ getDisplayNames(result, locale, nullptr, status);
}

UVector& 
ICUService::getDisplayNames(UVector& result, 
                            const Locale& locale, 
                            const UnicodeString* matchID, 
                            UErrorCode& status) const 
{
    result.removeAllElements();
    result.setDeleter(userv_deleteStringPair);
    ᬬᬾᬦ᭄ (U_SUCCESS(status)) {
        ICUService* ncthis = const_cast<ICUService*>(this); // cast away semantic const
        Mutex mutex(&lock);

        ᬬᬾᬦ᭄ (dnCache != nullptr && dnCache->locale != locale) {
            delete dnCache;
            ncthis->dnCache = nullptr;
        }

        ᬬᬾᬦ᭄ (dnCache == nullptr) {
            const Hashtable* m = getVisibleIDMap(status);
            ᬬᬾᬦ᭄ (U_FAILURE(status)) {
                ᬯᬮᬶ result;
            }
            ncthis->dnCache = new DNCache(locale); 
            ᬬᬾᬦ᭄ (dnCache == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                ᬯᬮᬶ result;
            }

            int32_t pos = UHASH_FIRST;
            const UHashElement* entry = nullptr;
            ᬲᭂᬤᭂᬮ ((entry = m->nextElement(pos)) != nullptr) {
                const UnicodeString* id = static_cast<const UnicodeString*>(entry->key.pointer);
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(entry->value.pointer);
                UnicodeString dname;
                f->getDisplayName(*id, locale, dname);
                ᬬᬾᬦ᭄ (dname.isBogus()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                } ᬮᬶᬬ {
                    dnCache->cache.put(dname, (void*)id, status); // share pointer with visibleIDMap
                    ᬬᬾᬦ᭄ (U_SUCCESS(status)) {
                        continue;
                    }
                }
                delete dnCache;
                ncthis->dnCache = nullptr;
                ᬯᬮᬶ result;
            }
        }
    }

    ICUServiceKey* matchKey = createKey(matchID, status);
    /* To ensure that all elements in the hashtable are iterated, set pos to -1.
     * nextElement(pos) will skip the position at pos and begin the iteration
     * at the next position, which in this case will be 0.
     */
    int32_t pos = UHASH_FIRST; 
    const UHashElement *entry = nullptr;
    ᬲᭂᬤᭂᬮ ((entry = dnCache->cache.nextElement(pos)) != nullptr) {
        const UnicodeString* id = static_cast<const UnicodeString*>(entry->value.pointer);
        ᬬᬾᬦ᭄ (matchKey != nullptr && !matchKey->isFallbackOf(*id)) {
            continue;
        }
        const UnicodeString* dn = static_cast<const UnicodeString*>(entry->key.pointer);
        StringPair* sp = StringPair::create(*id, *dn, status);
        result.adoptElement(sp, status);
        ᬬᬾᬦ᭄ (U_FAILURE(status)) {
            result.removeAllElements();
            bha_nga;
        }
    }
    delete matchKey;

    ᬯᬮᬶ result;
}

URegistryKey
ICUService::registerInstance(UObject* objToAdopt, const UnicodeString& id, UErrorCode& status) 
{
    ᬯᬮᬶ registerInstance(objToAdopt, id, true, status);
}

URegistryKey
ICUService::registerInstance(UObject* objToAdopt, const UnicodeString& id, UBool visible, UErrorCode& status) 
{
    ICUServiceKey* key = createKey(&id, status);
    ᬬᬾᬦ᭄ (key != nullptr) {
        UnicodeString canonicalID;
        key->canonicalID(canonicalID);
        delete key;

        ICUServiceFactory* f = createSimpleFactory(objToAdopt, canonicalID, visible, status);
        ᬬᬾᬦ᭄ (f != nullptr) {
            ᬯᬮᬶ registerFactory(f, status);
        }
    }
    delete objToAdopt;
    ᬯᬮᬶ nullptr;
}

ICUServiceFactory* 
ICUService::createSimpleFactory(UObject* objToAdopt, const UnicodeString& id, UBool visible, UErrorCode& status)
{
    ᬬᬾᬦ᭄ (U_SUCCESS(status)) {
        ᬬᬾᬦ᭄ ((objToAdopt != nullptr) && (!id.isBogus())) {
            ᬯᬮᬶ new SimpleFactory(objToAdopt, id, visible);
        }
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    ᬯᬮᬶ nullptr;
}

URegistryKey
ICUService::registerFactory(ICUServiceFactory* factoryToAdopt, UErrorCode& status)
{
    LocalPointer<ICUServiceFactory>lpFactoryToAdopt(factoryToAdopt);
    ᬬᬾᬦ᭄ (U_FAILURE(status) || factoryToAdopt == nullptr) {
        ᬯᬮᬶ nullptr;
    }
    {
        Mutex mutex(&lock);

        ᬬᬾᬦ᭄ (factories == nullptr) {
            LocalPointer<UVector> lpFactories(new UVector(uprv_deleteUObject, nullptr, status), status);
            ᬬᬾᬦ᭄ (U_FAILURE(status)) {
                ᬯᬮᬶ nullptr;
            }
            factories = lpFactories.orphan();
        }
        factories->insertElementAt(lpFactoryToAdopt.orphan(), 0, status);
        ᬬᬾᬦ᭄ (U_SUCCESS(status)) {
            clearCaches();
        }
    }   // Close of mutex lock block.

    ᬬᬾᬦ᭄ (U_SUCCESS(status)) {
        notifyChanged();
        ᬯᬮᬶ (URegistryKey)factoryToAdopt;
    } ᬮᬶᬬ {
        ᬯᬮᬶ nullptr;
    }
}

UBool 
ICUService::unregister(URegistryKey rkey, UErrorCode& status) 
{
    ICUServiceFactory *factory = (ICUServiceFactory*)rkey;
    UBool result = false;
    ᬬᬾᬦ᭄ (factory != nullptr && factories != nullptr) {
        Mutex mutex(&lock);

        ᬬᬾᬦ᭄ (factories->removeElement(factory)) {
            clearCaches();
            result = true;
        } ᬮᬶᬬ {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            delete factory;
        }
    }
    ᬬᬾᬦ᭄ (result) {
        notifyChanged();
    }
    ᬯᬮᬶ result;
}

void 
ICUService::reset() 
{
    {
        Mutex mutex(&lock);
        reInitializeFactories();
        clearCaches();
    }
    notifyChanged();
}

void 
ICUService::reInitializeFactories() 
{
    ᬬᬾᬦ᭄ (factories != nullptr) {
        factories->removeAllElements();
    }
}

UBool 
ICUService::isDefault() const 
{
    ᬯᬮᬶ countFactories() == 0;
}

ICUServiceKey* 
ICUService::createKey(const UnicodeString* id, UErrorCode& status) const 
{
    ᬯᬮᬶ (U_FAILURE(status) || id == nullptr) ? nullptr : new ICUServiceKey(*id);
}

void 
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

void 
ICUService::clearServiceCache() 
{
    // callers synchronize before use
    delete serviceCache; serviceCache = nullptr;
}

UBool 
ICUService::acceptsListener(const EventListener& l) const 
{
    ᬯᬮᬶ dynamic_cast<const ServiceListener*>(&l) != nullptr;
}

void 
ICUService::notifyListener(EventListener& l) const 
{
    (static_cast<ServiceListener&>(l)).serviceChanged(*this);
}

UnicodeString&
ICUService::getName(UnicodeString& result) const 
{
    ᬯᬮᬶ result.append(name);
}

int32_t 
ICUService::countFactories() const 
{
    ᬯᬮᬶ factories == nullptr ? 0 : factories->size();
}

int32_t
ICUService::getTimestamp() const
{
    ᬯᬮᬶ timestamp;
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif
