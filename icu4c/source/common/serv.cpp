// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
*******************************************************************************
* Copyright (C) 2001-2014, International Business Machines Corporation.
* All Rights Reserved.
*******************************************************************************
*/

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/localpointer.h"

#எனில் !UCONFIG_NO_SERVICE

#அந்தர்பாவ "serv.h"
#அந்தர்பாவ "umutex.h"

#undef SERVICE_REFCOUNT

// in case we use the refcount stuff

U_NAMESPACE_BEGIN

/*
******************************************************************
*/

மாறிலி char16_t ICUService_சாவி::PREFIX_DELIMITER = 0x002F;   /* '/' */

ICUService_சாவி::ICUService_சாவி(மாறிலி UnicodeString& id) 
: _id(id) {
}

ICUService_சாவி::~ICUService_சாவி() 
{
}

மாறிலி UnicodeString& 
ICUService_சாவி::getID() மாறிலி 
{
    திரும்பு _id;
}

UnicodeString& 
ICUService_சாவி::canonicalID(UnicodeString& result) மாறிலி 
{
    திரும்பு result.append(_id);
}

UnicodeString& 
ICUService_சாவி::currentID(UnicodeString& result) மாறிலி 
{
    திரும்பு canonicalID(result);
}

UnicodeString& 
ICUService_சாவி::currentDescriptor(UnicodeString& result) மாறிலி 
{
    prefix(result);
    result.append(PREFIX_DELIMITER);
    திரும்பு currentID(result);
}

UBool 
ICUService_சாவி::fallback() 
{
    திரும்பு பொய்;
}

UBool 
ICUService_சாவி::isFallbackOf(மாறிலி UnicodeString& id) மாறிலி 
{
    திரும்பு id == _id;
}

UnicodeString& 
ICUService_சாவி::prefix(UnicodeString& result) மாறிலி 
{
    திரும்பு result;
}

UnicodeString& 
ICUService_சாவி::parsePrefix(UnicodeString& result) 
{
    int32_t n = result.indexOf(PREFIX_DELIMITER);
    எனில் (n < 0) {
        n = 0;
    }
    result.remove(n);
    திரும்பு result;
}

UnicodeString& 
ICUService_சாவி::parseSuffix(UnicodeString& result) 
{
    int32_t n = result.indexOf(PREFIX_DELIMITER);
    எனில் (n >= 0) {
        result.remove(0, n+1);
    }
    திரும்பு result;
}

#ifdef SERVICE_DEBUG
UnicodeString& 
ICUService_சாவி::debug(UnicodeString& result) மாறிலி 
{
    debugClass(result);
    result.append((UnicodeString)" id: ");
    result.append(_id);
    திரும்பு result;
}

UnicodeString& 
ICUService_சாவி::debugClass(UnicodeString& result) மாறிலி 
{
    திரும்பு result.append((UnicodeString)"ICUServiceKey");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ICUService_சாவி)

/*
******************************************************************
*/

ICUServiceFactory::~ICUServiceFactory() {}

SimpleFactory::SimpleFactory(UObject* instanceToAdopt, மாறிலி UnicodeString& id, UBool visible) 
: _instance(instanceToAdopt), _id(id), _visible(visible)
{
}

SimpleFactory::~SimpleFactory() 
{
    delete _instance;
}

UObject* 
SimpleFactory::create(மாறிலி ICUService_சாவி& சாவி, மாறிலி ICUService* service, UErrorCode& status) மாறிலி 
{
    எனில் (U_SUCCESS(status)) {
        UnicodeString temp;
        எனில் (_id == சாவி.currentID(temp)) {
            திரும்பு service->cloneInstance(_instance); 
        }
    }
    திரும்பு nullptr;
}

வெறுமை 
SimpleFactory::updateVisibleIDs(Hashtable& result, UErrorCode& status) மாறிலி 
{
    எனில் (_visible) {
        result.put(_id, (வெறுமை*)இது, status); // cast away const
    } இல்லையெனில் {
        result.remove(_id);
    }
}

UnicodeString& 
SimpleFactory::getDisplayName(மாறிலி UnicodeString& id, மாறிலி Locale& /* locale */, UnicodeString& result) மாறிலி 
{
    எனில் (_visible && _id == id) {
        result = _id;
    } இல்லையெனில் {
        result.setToBogus();
    }
    திரும்பு result;
}

#ifdef SERVICE_DEBUG
UnicodeString& 
SimpleFactory::debug(UnicodeString& toAppendTo) மாறிலி 
{
    debugClass(toAppendTo);
    toAppendTo.append((UnicodeString)" id: ");
    toAppendTo.append(_id);
    toAppendTo.append((UnicodeString)", visible: ");
    toAppendTo.append(_visible ? (UnicodeString)"T" : (UnicodeString)"F");
    திரும்பு toAppendTo;
}

UnicodeString& 
SimpleFactory::debugClass(UnicodeString& toAppendTo) மாறிலி 
{
    திரும்பு toAppendTo.append((UnicodeString)"SimpleFactory");
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
வகுப்பு CacheEntry : பொது UMemory {
தனிப்பட்ட:
    int32_t refcount;

பொது:
    UnicodeString actualDescriptor;
    UObject* service;

    /**
    * Releases a reference to the shared resource.
    */
    ~CacheEntry() {
        delete service;
    }

    CacheEntry(மாறிலி UnicodeString& _actualDescriptor, UObject* _service) 
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
        திரும்பு இது;
    }

    /**
    * Destructions removes a reference, so don't call this unless
    * you're removing pointer to this somewhere.  Management of that
    * pointer will have to know how to deal with refcounts.  Once
    * the refcount drops to zero, the resource is released.  Return
    * false if the resource has been released.
    */
    CacheEntry* unref() {
        எனில் ((--refcount) == 0) {
            delete இது;
            திரும்பு nullptr;
        }
        திரும்பு இது;
    }

    /**
    * Return true if there is at least one reference to this and the
    * resource has not been released.
    */
    UBool isShared() மாறிலி {
        திரும்பு refcount > 1;
    }
};

// Deleter for serviceCache
U_CDECL_BEGIN
நிலையான வெறுமை U_CALLCONV
cacheDeleter(வெறுமை* obj) {
    U_NAMESPACE_USE ((CacheEntry*)obj)->unref();
}

U_CDECL_END

/*
******************************************************************
*/

வகுப்பு DNCache : பொது UMemory {
பொது:
    Hashtable cache;
    மாறிலி Locale locale;

    DNCache(மாறிலி Locale& _locale) 
        : cache(), locale(_locale) 
    {
        // cache.setKeyDeleter(uprv_deleteUObject);
    }
};


/*
******************************************************************
*/

StringPair* 
StringPair::create(மாறிலி UnicodeString& displayName, 
                   மாறிலி UnicodeString& id,
                   UErrorCode& status)
{
    எனில் (U_SUCCESS(status)) {
        StringPair* sp = புதிய StringPair(displayName, id);
        எனில் (sp == nullptr || sp->isBogus()) {
            status = U_MEMORY_ALLOCATION_ERROR;
            delete sp;
            திரும்பு nullptr;
        }
        திரும்பு sp;
    }
    திரும்பு nullptr;
}

UBool 
StringPair::isBogus() மாறிலி {
    திரும்பு displayName.isBogus() || id.isBogus();
}

StringPair::StringPair(மாறிலி UnicodeString& _displayName, 
                       மாறிலி UnicodeString& _id)
: displayName(_displayName)
, id(_id)
{
}

U_CDECL_BEGIN
நிலையான வெறுமை U_CALLCONV
userv_deleteStringPair(வெறுமை *obj) {
    U_NAMESPACE_USE delete (StringPair*) obj;
}
U_CDECL_END

/*
******************************************************************
*/

நிலையான UMutex lock;

ICUService::ICUService()
: name()
, timestamp(0)
, factories(nullptr)
, serviceCache(nullptr)
, idCache(nullptr)
, dnCache(nullptr)
{
}

ICUService::ICUService(மாறிலி UnicodeString& newName) 
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
ICUService::get(மாறிலி UnicodeString& descriptor, UErrorCode& status) மாறிலி 
{
    திரும்பு get(descriptor, nullptr, status);
}

UObject* 
ICUService::get(மாறிலி UnicodeString& descriptor, UnicodeString* actualReturn, UErrorCode& status) மாறிலி 
{
    UObject* result = nullptr;
    ICUService_சாவி* சாவி = create_சாவி(&descriptor, status);
    எனில் (சாவி) {
        result = get_சாவி(*சாவி, actualReturn, status);
        delete சாவி;
    }
    திரும்பு result;
}

UObject* 
ICUService::get_சாவி(ICUService_சாவி& சாவி, UErrorCode& status) மாறிலி 
{
    திரும்பு get_சாவி(சாவி, nullptr, status);
}

// this is a vector that subclasses of ICUService can override to further customize the result object
// before returning it.  All other public get functions should call this one.

UObject* 
ICUService::get_சாவி(ICUService_சாவி& சாவி, UnicodeString* actualReturn, UErrorCode& status) மாறிலி 
{
    திரும்பு get_சாவி(சாவி, actualReturn, nullptr, status);
}

// make it possible to call reentrantly on systems that don't have reentrant mutexes.
// we can use this simple approach since we know the situation where we're calling
// reentrantly even without knowing the thread.
வகுப்பு XMutex : பொது UMemory {
பொது:
    உள்வரி XMutex(UMutex *mutex, UBool reentering) 
        : fMutex(mutex)
        , fActive(!reentering) 
    {
        எனில் (fActive) umtx_lock(fMutex);
    }
    உள்வரி ~XMutex() {
        எனில் (fActive) umtx_unlock(fMutex);
    }

தனிப்பட்ட:
    UMutex  *fMutex;
    UBool fActive;
};

// called only by factories, treat as private
UObject* 
ICUService::get_சாவி(ICUService_சாவி& சாவி, UnicodeString* actualReturn, மாறிலி ICUServiceFactory* factory, UErrorCode& status) மாறிலி 
{
    எனில் (U_FAILURE(status)) {
        திரும்பு nullptr;
    }

    எனில் (isDefault()) {
        திரும்பு handleDefault(சாவி, actualReturn, status);
    }

    ICUService* ncthis = const_cast<ICUService*>(இது); // cast away semantic const

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

        எனில் (serviceCache == nullptr) {
            ncthis->serviceCache = புதிய Hashtable(status);
            எனில் (ncthis->serviceCache == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                திரும்பு nullptr;
            }
            எனில் (U_FAILURE(status)) {
                delete serviceCache;
                திரும்பு nullptr;
            }
            serviceCache->set_மதிப்பு_Deleter(cacheDeleter);
        }

        UnicodeString currentDescriptor;
        LocalPointer<UVector> cacheDescriptorList;
        UBool putInCache = பொய்;

        int32_t startIndex = 0;
        int32_t limit = factories->size();
        UBool cacheResult = உண்மை;

        எனில் (factory != nullptr) {
            க்கு (int32_t i = 0; i < limit; ++i) {
                எனில் (factory == static_cast<மாறிலி ICUServiceFactory*>(factories->elementAt(i))) {
                    startIndex = i + 1;
                    நிறுத்து;
                }
            }
            எனில் (startIndex == 0) {
                // throw new InternalError("Factory " + factory + "not registered with service: " + this);
                status = U_ILLEGAL_ARGUMENT_ERROR;
                திரும்பு nullptr;
            }
            cacheResult = பொய்;
        }

        செய் {
            currentDescriptor.remove();
            சாவி.currentDescriptor(currentDescriptor);
            result = static_cast<CacheEntry*>(serviceCache->get(currentDescriptor));
            எனில் (result != nullptr) {
                நிறுத்து;
            }

            // first test of cache failed, so we'll have to update
            // the cache if we eventually succeed-- that is, if we're 
            // going to update the cache at all.
            putInCache = உண்மை;

            int32_t index = startIndex;
            போது (index < limit) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(index++));
                LocalPointer<UObject> service(f->create(சாவி, இது, status));
                எனில் (U_FAILURE(status)) {
                    திரும்பு nullptr;
                }
                எனில் (service.isValid()) {
                    result = புதிய CacheEntry(currentDescriptor, service.getAlias());
                    எனில் (result == nullptr) {
                        status = U_MEMORY_ALLOCATION_ERROR;
                        திரும்பு nullptr;
                    }
                    service.orphan(); // result now owns service.

                    செல் outerEnd;
                }
            }

            // prepare to load the cache with all additional ids that 
            // will resolve to result, assuming we'll succeed.  We
            // don't want to keep querying on an id that's going to
            // fallback to the one that succeeded, we want to hit the
            // cache the first time next goaround.
            எனில் (cacheDescriptorList.isNull()) {
                cacheDescriptorList.adoptInsteadAndCheckErrorCode(புதிய UVector(uprv_deleteUObject, nullptr, 5, status), status);
                எனில் (U_FAILURE(status)) {
                    திரும்பு nullptr;
                }
            }

            LocalPointer<UnicodeString> idToCache(புதிய UnicodeString(currentDescriptor), status);
            எனில் (U_FAILURE(status)) {
                திரும்பு nullptr;
            }
            எனில் (idToCache->isBogus()) {
                status = U_MEMORY_ALLOCATION_ERROR;
                திரும்பு nullptr;
            }
            cacheDescriptorList->adoptElement(idToCache.orphan(), status);
            எனில் (U_FAILURE(status)) {
                திரும்பு nullptr;
            }
        } போது (சாவி.fallback());
outerEnd:

        எனில் (result != nullptr) {
            எனில் (putInCache && cacheResult) {
                serviceCache->put(result->actualDescriptor, result, status);
                எனில் (U_FAILURE(status)) {
                    திரும்பு nullptr;
                }

                எனில் (cacheDescriptorList.isValid()) {
                    க்கு (int32_t i = cacheDescriptorList->size(); --i >= 0;) {
                        UnicodeString* desc = static_cast<UnicodeString*>(cacheDescriptorList->elementAt(i));

                        serviceCache->put(*desc, result, status);
                        எனில் (U_FAILURE(status)) {
                            திரும்பு nullptr;
                        }

                        result->ref();
                        cacheDescriptorList->removeElementAt(i);
                    }
                }
            }

            எனில் (actualReturn != nullptr) {
                // strip null prefix
                எனில் (result->actualDescriptor.indexOf(static_cast<char16_t>(0x2f)) == 0) { // U+002f=slash (/)
                    actualReturn->remove();
                    actualReturn->append(result->actualDescriptor, 
                        1, 
                        result->actualDescriptor.length() - 1);
                } இல்லையெனில் {
                    *actualReturn = result->actualDescriptor;
                }

                எனில் (actualReturn->isBogus()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    delete result;
                    திரும்பு nullptr;
                }
            }

            UObject* service = cloneInstance(result->service);
            எனில் (putInCache && !cacheResult) {
                delete result;
            }
            திரும்பு service;
        }
    }

    திரும்பு handleDefault(சாவி, actualReturn, status);
}

UObject* 
ICUService::handleDefault(மாறிலி ICUService_சாவி& /*சாவி*/, UnicodeString* /* actualIDReturn */, UErrorCode& /* status */) மாறிலி 
{
    திரும்பு nullptr;
}

UVector& 
ICUService::getVisibleIDs(UVector& result, UErrorCode& status) மாறிலி {
    திரும்பு getVisibleIDs(result, nullptr, status);
}

UVector& 
ICUService::getVisibleIDs(UVector& result, மாறிலி UnicodeString* matchID, UErrorCode& status) மாறிலி 
{
    result.removeAllElements();

    எனில் (U_FAILURE(status)) {
        திரும்பு result;
    }
    UObjectDeleter *savedDeleter = result.setDeleter(uprv_deleteUObject);

    {
        Mutex mutex(&lock);
        மாறிலி Hashtable* map = getVisibleIDMap(status);
        எனில் (map != nullptr) {
            ICUService_சாவி* fallback_சாவி = create_சாவி(matchID, status);

            க்கு (int32_t pos = UHASH_FIRST; U_SUCCESS(status); ) {
                மாறிலி UHashElement* e = map->nextElement(pos);
                எனில் (e == nullptr) {
                    நிறுத்து;
                }

                மாறிலி UnicodeString* id = static_cast<மாறிலி UnicodeString*>(e->சாவி.pointer);
                எனில் (fallback_சாவி != nullptr) {
                    எனில் (!fallback_சாவி->isFallbackOf(*id)) {
                        தொடர்;
                    }
                }

                LocalPointer<UnicodeString> idClone(id->clone(), status);
                result.adoptElement(idClone.orphan(), status);
            }
            delete fallback_சாவி;
        }
    }
    எனில் (U_FAILURE(status)) {
        result.removeAllElements();
    }
    result.setDeleter(savedDeleter);
    திரும்பு result;
}

மாறிலி Hashtable* 
ICUService::getVisibleIDMap(UErrorCode& status) மாறிலி {
    எனில் (U_FAILURE(status)) திரும்பு nullptr;

    // must only be called when lock is already held

    ICUService* ncthis = const_cast<ICUService*>(இது); // cast away semantic const
    எனில் (idCache == nullptr) {
        ncthis->idCache = புதிய Hashtable(status);
        எனில் (idCache == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        } இல்லையெனில் எனில் (factories != nullptr) {
            க்கு (int32_t pos = factories->size(); --pos >= 0;) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(pos));
                f->updateVisibleIDs(*idCache, status);
            }
            எனில் (U_FAILURE(status)) {
                delete idCache;
                ncthis->idCache = nullptr;
            }
        }
    }

    திரும்பு idCache;
}


UnicodeString& 
ICUService::getDisplayName(மாறிலி UnicodeString& id, UnicodeString& result) மாறிலி 
{
    திரும்பு getDisplayName(id, result, Locale::getDefault());
}

UnicodeString& 
ICUService::getDisplayName(மாறிலி UnicodeString& id, UnicodeString& result, மாறிலி Locale& locale) மாறிலி 
{
    {
        UErrorCode status = U_ZERO_ERROR;
        Mutex mutex(&lock);
        மாறிலி Hashtable* map = getVisibleIDMap(status);
        எனில் (map != nullptr) {
            ICUServiceFactory* f = static_cast<ICUServiceFactory*>(map->get(id));
            எனில் (f != nullptr) {
                f->getDisplayName(id, locale, result);
                திரும்பு result;
            }

            // fallback
            status = U_ZERO_ERROR;
            ICUService_சாவி* fallback_சாவி = create_சாவி(&id, status);
            போது (fallback_சாவி != nullptr && fallback_சாவி->fallback()) {
                UnicodeString us;
                fallback_சாவி->currentID(us);
                f = static_cast<ICUServiceFactory*>(map->get(us));
                எனில் (f != nullptr) {
                    f->getDisplayName(id, locale, result);
                    delete fallback_சாவி;
                    திரும்பு result;
                }
            }
            delete fallback_சாவி;
        }
    }
    result.setToBogus();
    திரும்பு result;
}

UVector& 
ICUService::getDisplayNames(UVector& result, UErrorCode& status) மாறிலி 
{
    திரும்பு getDisplayNames(result, Locale::getDefault(), nullptr, status);
}


UVector& 
ICUService::getDisplayNames(UVector& result, மாறிலி Locale& locale, UErrorCode& status) மாறிலி 
{
    திரும்பு getDisplayNames(result, locale, nullptr, status);
}

UVector& 
ICUService::getDisplayNames(UVector& result, 
                            மாறிலி Locale& locale, 
                            மாறிலி UnicodeString* matchID, 
                            UErrorCode& status) மாறிலி 
{
    result.removeAllElements();
    result.setDeleter(userv_deleteStringPair);
    எனில் (U_SUCCESS(status)) {
        ICUService* ncthis = const_cast<ICUService*>(இது); // cast away semantic const
        Mutex mutex(&lock);

        எனில் (dnCache != nullptr && dnCache->locale != locale) {
            delete dnCache;
            ncthis->dnCache = nullptr;
        }

        எனில் (dnCache == nullptr) {
            மாறிலி Hashtable* m = getVisibleIDMap(status);
            எனில் (U_FAILURE(status)) {
                திரும்பு result;
            }
            ncthis->dnCache = புதிய DNCache(locale); 
            எனில் (dnCache == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                திரும்பு result;
            }

            int32_t pos = UHASH_FIRST;
            மாறிலி UHashElement* entry = nullptr;
            போது ((entry = m->nextElement(pos)) != nullptr) {
                மாறிலி UnicodeString* id = static_cast<மாறிலி UnicodeString*>(entry->சாவி.pointer);
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(entry->value.pointer);
                UnicodeString dname;
                f->getDisplayName(*id, locale, dname);
                எனில் (dname.isBogus()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                } இல்லையெனில் {
                    dnCache->cache.put(dname, (வெறுமை*)id, status); // share pointer with visibleIDMap
                    எனில் (U_SUCCESS(status)) {
                        தொடர்;
                    }
                }
                delete dnCache;
                ncthis->dnCache = nullptr;
                திரும்பு result;
            }
        }
    }

    ICUService_சாவி* match_சாவி = create_சாவி(matchID, status);
    /* To ensure that all elements in the hashtable are iterated, set pos to -1.
     * nextElement(pos) will skip the position at pos and begin the iteration
     * at the next position, which in this case will be 0.
     */
    int32_t pos = UHASH_FIRST; 
    மாறிலி UHashElement *entry = nullptr;
    போது ((entry = dnCache->cache.nextElement(pos)) != nullptr) {
        மாறிலி UnicodeString* id = static_cast<மாறிலி UnicodeString*>(entry->value.pointer);
        எனில் (match_சாவி != nullptr && !match_சாவி->isFallbackOf(*id)) {
            தொடர்;
        }
        மாறிலி UnicodeString* dn = static_cast<மாறிலி UnicodeString*>(entry->சாவி.pointer);
        StringPair* sp = StringPair::create(*id, *dn, status);
        result.adoptElement(sp, status);
        எனில் (U_FAILURE(status)) {
            result.removeAllElements();
            நிறுத்து;
        }
    }
    delete match_சாவி;

    திரும்பு result;
}

URegistry_சாவி
ICUService::registerInstance(UObject* objToAdopt, மாறிலி UnicodeString& id, UErrorCode& status) 
{
    திரும்பு registerInstance(objToAdopt, id, உண்மை, status);
}

URegistry_சாவி
ICUService::registerInstance(UObject* objToAdopt, மாறிலி UnicodeString& id, UBool visible, UErrorCode& status) 
{
    ICUService_சாவி* சாவி = create_சாவி(&id, status);
    எனில் (சாவி != nullptr) {
        UnicodeString canonicalID;
        சாவி->canonicalID(canonicalID);
        delete சாவி;

        ICUServiceFactory* f = createSimpleFactory(objToAdopt, canonicalID, visible, status);
        எனில் (f != nullptr) {
            திரும்பு registerFactory(f, status);
        }
    }
    delete objToAdopt;
    திரும்பு nullptr;
}

ICUServiceFactory* 
ICUService::createSimpleFactory(UObject* objToAdopt, மாறிலி UnicodeString& id, UBool visible, UErrorCode& status)
{
    எனில் (U_SUCCESS(status)) {
        எனில் ((objToAdopt != nullptr) && (!id.isBogus())) {
            திரும்பு புதிய SimpleFactory(objToAdopt, id, visible);
        }
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    திரும்பு nullptr;
}

URegistry_சாவி
ICUService::registerFactory(ICUServiceFactory* factoryToAdopt, UErrorCode& status)
{
    LocalPointer<ICUServiceFactory>lpFactoryToAdopt(factoryToAdopt);
    எனில் (U_FAILURE(status) || factoryToAdopt == nullptr) {
        திரும்பு nullptr;
    }
    {
        Mutex mutex(&lock);

        எனில் (factories == nullptr) {
            LocalPointer<UVector> lpFactories(புதிய UVector(uprv_deleteUObject, nullptr, status), status);
            எனில் (U_FAILURE(status)) {
                திரும்பு nullptr;
            }
            factories = lpFactories.orphan();
        }
        factories->செருகு_Element_At(lpFactoryToAdopt.orphan(), 0, status);
        எனில் (U_SUCCESS(status)) {
            clearCaches();
        }
    }   // Close of mutex lock block.

    எனில் (U_SUCCESS(status)) {
        notifyChanged();
        திரும்பு (URegistry_சாவி)factoryToAdopt;
    } இல்லையெனில் {
        திரும்பு nullptr;
    }
}

UBool 
ICUService::unregister(URegistry_சாவி rkey, UErrorCode& status) 
{
    ICUServiceFactory *factory = (ICUServiceFactory*)rkey;
    UBool result = பொய்;
    எனில் (factory != nullptr && factories != nullptr) {
        Mutex mutex(&lock);

        எனில் (factories->removeElement(factory)) {
            clearCaches();
            result = உண்மை;
        } இல்லையெனில் {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            delete factory;
        }
    }
    எனில் (result) {
        notifyChanged();
    }
    திரும்பு result;
}

வெறுமை 
ICUService::reset() 
{
    {
        Mutex mutex(&lock);
        reInitializeFactories();
        clearCaches();
    }
    notifyChanged();
}

வெறுமை 
ICUService::reInitializeFactories() 
{
    எனில் (factories != nullptr) {
        factories->removeAllElements();
    }
}

UBool 
ICUService::isDefault() மாறிலி 
{
    திரும்பு மொத்தம்_Factories() == 0;
}

ICUService_சாவி* 
ICUService::create_சாவி(மாறிலி UnicodeString* id, UErrorCode& status) மாறிலி 
{
    திரும்பு (U_FAILURE(status) || id == nullptr) ? nullptr : புதிய ICUService_சாவி(*id);
}

வெறுமை 
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

வெறுமை 
ICUService::clearServiceCache() 
{
    // callers synchronize before use
    delete serviceCache; serviceCache = nullptr;
}

UBool 
ICUService::acceptsListener(மாறிலி EventListener& l) மாறிலி 
{
    திரும்பு dynamic_cast<மாறிலி ServiceListener*>(&l) != nullptr;
}

வெறுமை 
ICUService::notifyListener(EventListener& l) மாறிலி 
{
    (static_cast<ServiceListener&>(l)).serviceChanged(*இது);
}

UnicodeString&
ICUService::getName(UnicodeString& result) மாறிலி 
{
    திரும்பு result.append(name);
}

int32_t 
ICUService::மொத்தம்_Factories() மாறிலி 
{
    திரும்பு factories == nullptr ? 0 : factories->size();
}

int32_t
ICUService::getTimestamp() மாறிலி
{
    திரும்பு timestamp;
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif
