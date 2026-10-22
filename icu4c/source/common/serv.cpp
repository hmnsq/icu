// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
*******************************************************************************
* Copyright (C) 2001-2014, International Business Machines Corporation.
* All Rights Reserved.
*******************************************************************************
*/

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/localpointer.h"

#यदि !UCONFIG_NO_SERVICE

#अन्तर्भाव "serv.h"
#अन्तर्भाव "umutex.h"

#undef SERVICE_REFCOUNT

// in case we use the refcount stuff

U_NAMESPACE_BEGIN

/*
******************************************************************
*/

अचर char16_t ICUService_साँचो::PREFIX_DELIMITER = 0x002F;   /* '/' */

ICUService_साँचो::ICUService_साँचो(अचर UnicodeString& id) 
: _id(id) {
}

ICUService_साँचो::~ICUService_साँचो() 
{
}

अचर UnicodeString& 
ICUService_साँचो::getID() अचर 
{
    फर्काऊ _id;
}

UnicodeString& 
ICUService_साँचो::canonicalID(UnicodeString& result) अचर 
{
    फर्काऊ result.append(_id);
}

UnicodeString& 
ICUService_साँचो::currentID(UnicodeString& result) अचर 
{
    फर्काऊ canonicalID(result);
}

UnicodeString& 
ICUService_साँचो::currentDescriptor(UnicodeString& result) अचर 
{
    prefix(result);
    result.append(PREFIX_DELIMITER);
    फर्काऊ currentID(result);
}

UBool 
ICUService_साँचो::fallback() 
{
    फर्काऊ असत्य;
}

UBool 
ICUService_साँचो::isFallbackOf(अचर UnicodeString& id) अचर 
{
    फर्काऊ id == _id;
}

UnicodeString& 
ICUService_साँचो::prefix(UnicodeString& result) अचर 
{
    फर्काऊ result;
}

UnicodeString& 
ICUService_साँचो::parsePrefix(UnicodeString& result) 
{
    int32_t n = result.indexOf(PREFIX_DELIMITER);
    यदि (n < 0) {
        n = 0;
    }
    result.हटाओ(n);
    फर्काऊ result;
}

UnicodeString& 
ICUService_साँचो::parseSuffix(UnicodeString& result) 
{
    int32_t n = result.indexOf(PREFIX_DELIMITER);
    यदि (n >= 0) {
        result.हटाओ(0, n+1);
    }
    फर्काऊ result;
}

#ifdef SERVICE_DEBUG
UnicodeString& 
ICUService_साँचो::debug(UnicodeString& result) अचर 
{
    debugClass(result);
    result.append((UnicodeString)" id: ");
    result.append(_id);
    फर्काऊ result;
}

UnicodeString& 
ICUService_साँचो::debugClass(UnicodeString& result) अचर 
{
    फर्काऊ result.append((UnicodeString)"ICUServiceKey");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ICUService_साँचो)

/*
******************************************************************
*/

ICUServiceFactory::~ICUServiceFactory() {}

SimpleFactory::SimpleFactory(UObject* instanceToAdopt, अचर UnicodeString& id, UBool visible) 
: _instance(instanceToAdopt), _id(id), _visible(visible)
{
}

SimpleFactory::~SimpleFactory() 
{
    delete _instance;
}

UObject* 
SimpleFactory::create(अचर ICUService_साँचो& साँचो, अचर ICUService* service, UErrorCode& status) अचर 
{
    यदि (U_SUCCESS(status)) {
        UnicodeString temp;
        यदि (_id == साँचो.currentID(temp)) {
            फर्काऊ service->cloneInstance(_instance); 
        }
    }
    फर्काऊ nullptr;
}

शून्य 
SimpleFactory::updateVisibleIDs(Hashtable& result, UErrorCode& status) अचर 
{
    यदि (_visible) {
        result.put(_id, (शून्य*)एतत्, status); // cast away const
    } अन्यथा {
        result.हटाओ(_id);
    }
}

UnicodeString& 
SimpleFactory::getDisplayName(अचर UnicodeString& id, अचर Locale& /* locale */, UnicodeString& result) अचर 
{
    यदि (_visible && _id == id) {
        result = _id;
    } अन्यथा {
        result.setToBogus();
    }
    फर्काऊ result;
}

#ifdef SERVICE_DEBUG
UnicodeString& 
SimpleFactory::debug(UnicodeString& toAppendTo) अचर 
{
    debugClass(toAppendTo);
    toAppendTo.append((UnicodeString)" id: ");
    toAppendTo.append(_id);
    toAppendTo.append((UnicodeString)", visible: ");
    toAppendTo.append(_visible ? (UnicodeString)"T" : (UnicodeString)"F");
    फर्काऊ toAppendTo;
}

UnicodeString& 
SimpleFactory::debugClass(UnicodeString& toAppendTo) अचर 
{
    फर्काऊ toAppendTo.append((UnicodeString)"SimpleFactory");
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
वर्ग CacheEntry : सार्वजनिक UMemory {
गुप्त:
    int32_t refcount;

सार्वजनिक:
    UnicodeString actualDescriptor;
    UObject* service;

    /**
    * Releases a reference to the shared resource.
    */
    ~CacheEntry() {
        delete service;
    }

    CacheEntry(अचर UnicodeString& _actualDescriptor, UObject* _service) 
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
        फर्काऊ एतत्;
    }

    /**
    * Destructions removes a reference, so don't call this unless
    * you're removing pointer to this somewhere.  Management of that
    * pointer will have to know how to deal with refcounts.  Once
    * the refcount drops to zero, the resource is released.  Return
    * false if the resource has been released.
    */
    CacheEntry* unref() {
        यदि ((--refcount) == 0) {
            delete एतत्;
            फर्काऊ nullptr;
        }
        फर्काऊ एतत्;
    }

    /**
    * Return true if there is at least one reference to this and the
    * resource has not been released.
    */
    UBool isShared() अचर {
        फर्काऊ refcount > 1;
    }
};

// Deleter for serviceCache
U_CDECL_BEGIN
स्थिर शून्य U_CALLCONV
cacheDeleter(शून्य* obj) {
    U_NAMESPACE_USE ((CacheEntry*)obj)->unref();
}

U_CDECL_END

/*
******************************************************************
*/

वर्ग DNCache : सार्वजनिक UMemory {
सार्वजनिक:
    Hashtable cache;
    अचर Locale locale;

    DNCache(अचर Locale& _locale) 
        : cache(), locale(_locale) 
    {
        // cache.setKeyDeleter(uprv_deleteUObject);
    }
};


/*
******************************************************************
*/

StringPair* 
StringPair::create(अचर UnicodeString& displayName, 
                   अचर UnicodeString& id,
                   UErrorCode& status)
{
    यदि (U_SUCCESS(status)) {
        StringPair* sp = नूतन StringPair(displayName, id);
        यदि (sp == nullptr || sp->isBogus()) {
            status = U_MEMORY_ALLOCATION_ERROR;
            delete sp;
            फर्काऊ nullptr;
        }
        फर्काऊ sp;
    }
    फर्काऊ nullptr;
}

UBool 
StringPair::isBogus() अचर {
    फर्काऊ displayName.isBogus() || id.isBogus();
}

StringPair::StringPair(अचर UnicodeString& _displayName, 
                       अचर UnicodeString& _id)
: displayName(_displayName)
, id(_id)
{
}

U_CDECL_BEGIN
स्थिर शून्य U_CALLCONV
userv_deleteStringPair(शून्य *obj) {
    U_NAMESPACE_USE delete (StringPair*) obj;
}
U_CDECL_END

/*
******************************************************************
*/

स्थिर UMutex lock;

ICUService::ICUService()
: name()
, timestamp(0)
, factories(nullptr)
, serviceCache(nullptr)
, idCache(nullptr)
, dnCache(nullptr)
{
}

ICUService::ICUService(अचर UnicodeString& newName) 
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
ICUService::get(अचर UnicodeString& descriptor, UErrorCode& status) अचर 
{
    फर्काऊ get(descriptor, nullptr, status);
}

UObject* 
ICUService::get(अचर UnicodeString& descriptor, UnicodeString* actualReturn, UErrorCode& status) अचर 
{
    UObject* result = nullptr;
    ICUService_साँचो* साँचो = create_साँचो(&descriptor, status);
    यदि (साँचो) {
        result = get_साँचो(*साँचो, actualReturn, status);
        delete साँचो;
    }
    फर्काऊ result;
}

UObject* 
ICUService::get_साँचो(ICUService_साँचो& साँचो, UErrorCode& status) अचर 
{
    फर्काऊ get_साँचो(साँचो, nullptr, status);
}

// this is a vector that subclasses of ICUService can override to further customize the result object
// before returning it.  All other public get functions should call this one.

UObject* 
ICUService::get_साँचो(ICUService_साँचो& साँचो, UnicodeString* actualReturn, UErrorCode& status) अचर 
{
    फर्काऊ get_साँचो(साँचो, actualReturn, nullptr, status);
}

// make it possible to call reentrantly on systems that don't have reentrant mutexes.
// we can use this simple approach since we know the situation where we're calling
// reentrantly even without knowing the thread.
वर्ग XMutex : सार्वजनिक UMemory {
सार्वजनिक:
    अन्तःस्थ XMutex(UMutex *mutex, UBool reentering) 
        : fMutex(mutex)
        , fActive(!reentering) 
    {
        यदि (fActive) umtx_lock(fMutex);
    }
    अन्तःस्थ ~XMutex() {
        यदि (fActive) umtx_unlock(fMutex);
    }

गुप्त:
    UMutex  *fMutex;
    UBool fActive;
};

// called only by factories, treat as private
UObject* 
ICUService::get_साँचो(ICUService_साँचो& साँचो, UnicodeString* actualReturn, अचर ICUServiceFactory* factory, UErrorCode& status) अचर 
{
    यदि (U_FAILURE(status)) {
        फर्काऊ nullptr;
    }

    यदि (isDefault()) {
        फर्काऊ handleDefault(साँचो, actualReturn, status);
    }

    ICUService* ncthis = const_cast<ICUService*>(एतत्); // cast away semantic const

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

        यदि (serviceCache == nullptr) {
            ncthis->serviceCache = नूतन Hashtable(status);
            यदि (ncthis->serviceCache == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                फर्काऊ nullptr;
            }
            यदि (U_FAILURE(status)) {
                delete serviceCache;
                फर्काऊ nullptr;
            }
            serviceCache->set_मान_Deleter(cacheDeleter);
        }

        UnicodeString currentDescriptor;
        LocalPointer<UVector> cacheDescriptorList;
        UBool putInCache = असत्य;

        int32_t startIndex = 0;
        int32_t limit = factories->size();
        UBool cacheResult = सत्य;

        यदि (factory != nullptr) {
            लागि (int32_t i = 0; i < limit; ++i) {
                यदि (factory == static_cast<अचर ICUServiceFactory*>(factories->elementAt(i))) {
                    startIndex = i + 1;
                    रोक;
                }
            }
            यदि (startIndex == 0) {
                // throw new InternalError("Factory " + factory + "not registered with service: " + this);
                status = U_ILLEGAL_ARGUMENT_ERROR;
                फर्काऊ nullptr;
            }
            cacheResult = असत्य;
        }

        गर {
            currentDescriptor.हटाओ();
            साँचो.currentDescriptor(currentDescriptor);
            result = static_cast<CacheEntry*>(serviceCache->get(currentDescriptor));
            यदि (result != nullptr) {
                रोक;
            }

            // first test of cache failed, so we'll have to update
            // the cache if we eventually succeed-- that is, if we're 
            // going to update the cache at all.
            putInCache = सत्य;

            int32_t index = startIndex;
            जबसम्म (index < limit) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(index++));
                LocalPointer<UObject> service(f->create(साँचो, एतत्, status));
                यदि (U_FAILURE(status)) {
                    फर्काऊ nullptr;
                }
                यदि (service.isValid()) {
                    result = नूतन CacheEntry(currentDescriptor, service.getAlias());
                    यदि (result == nullptr) {
                        status = U_MEMORY_ALLOCATION_ERROR;
                        फर्काऊ nullptr;
                    }
                    service.orphan(); // result now owns service.

                    गच्छ outerEnd;
                }
            }

            // prepare to load the cache with all additional ids that 
            // will resolve to result, assuming we'll succeed.  We
            // don't want to keep querying on an id that's going to
            // fallback to the one that succeeded, we want to hit the
            // cache the first time next goaround.
            यदि (cacheDescriptorList.isNull()) {
                cacheDescriptorList.adoptInsteadAndCheckErrorCode(नूतन UVector(uprv_deleteUObject, nullptr, 5, status), status);
                यदि (U_FAILURE(status)) {
                    फर्काऊ nullptr;
                }
            }

            LocalPointer<UnicodeString> idToCache(नूतन UnicodeString(currentDescriptor), status);
            यदि (U_FAILURE(status)) {
                फर्काऊ nullptr;
            }
            यदि (idToCache->isBogus()) {
                status = U_MEMORY_ALLOCATION_ERROR;
                फर्काऊ nullptr;
            }
            cacheDescriptorList->adoptElement(idToCache.orphan(), status);
            यदि (U_FAILURE(status)) {
                फर्काऊ nullptr;
            }
        } जबसम्म (साँचो.fallback());
outerEnd:

        यदि (result != nullptr) {
            यदि (putInCache && cacheResult) {
                serviceCache->put(result->actualDescriptor, result, status);
                यदि (U_FAILURE(status)) {
                    फर्काऊ nullptr;
                }

                यदि (cacheDescriptorList.isValid()) {
                    लागि (int32_t i = cacheDescriptorList->size(); --i >= 0;) {
                        UnicodeString* desc = static_cast<UnicodeString*>(cacheDescriptorList->elementAt(i));

                        serviceCache->put(*desc, result, status);
                        यदि (U_FAILURE(status)) {
                            फर्काऊ nullptr;
                        }

                        result->ref();
                        cacheDescriptorList->removeElementAt(i);
                    }
                }
            }

            यदि (actualReturn != nullptr) {
                // strip null prefix
                यदि (result->actualDescriptor.indexOf(static_cast<char16_t>(0x2f)) == 0) { // U+002f=slash (/)
                    actualReturn->हटाओ();
                    actualReturn->append(result->actualDescriptor, 
                        1, 
                        result->actualDescriptor.length() - 1);
                } अन्यथा {
                    *actualReturn = result->actualDescriptor;
                }

                यदि (actualReturn->isBogus()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    delete result;
                    फर्काऊ nullptr;
                }
            }

            UObject* service = cloneInstance(result->service);
            यदि (putInCache && !cacheResult) {
                delete result;
            }
            फर्काऊ service;
        }
    }

    फर्काऊ handleDefault(साँचो, actualReturn, status);
}

UObject* 
ICUService::handleDefault(अचर ICUService_साँचो& /*साँचो*/, UnicodeString* /* actualIDReturn */, UErrorCode& /* status */) अचर 
{
    फर्काऊ nullptr;
}

UVector& 
ICUService::getVisibleIDs(UVector& result, UErrorCode& status) अचर {
    फर्काऊ getVisibleIDs(result, nullptr, status);
}

UVector& 
ICUService::getVisibleIDs(UVector& result, अचर UnicodeString* matchID, UErrorCode& status) अचर 
{
    result.removeAllElements();

    यदि (U_FAILURE(status)) {
        फर्काऊ result;
    }
    UObjectDeleter *savedDeleter = result.setDeleter(uprv_deleteUObject);

    {
        Mutex mutex(&lock);
        अचर Hashtable* map = getVisibleIDMap(status);
        यदि (map != nullptr) {
            ICUService_साँचो* fallback_साँचो = create_साँचो(matchID, status);

            लागि (int32_t pos = UHASH_FIRST; U_SUCCESS(status); ) {
                अचर UHashElement* e = map->nextElement(pos);
                यदि (e == nullptr) {
                    रोक;
                }

                अचर UnicodeString* id = static_cast<अचर UnicodeString*>(e->साँचो.pointer);
                यदि (fallback_साँचो != nullptr) {
                    यदि (!fallback_साँचो->isFallbackOf(*id)) {
                        जारी;
                    }
                }

                LocalPointer<UnicodeString> idClone(id->clone(), status);
                result.adoptElement(idClone.orphan(), status);
            }
            delete fallback_साँचो;
        }
    }
    यदि (U_FAILURE(status)) {
        result.removeAllElements();
    }
    result.setDeleter(savedDeleter);
    फर्काऊ result;
}

अचर Hashtable* 
ICUService::getVisibleIDMap(UErrorCode& status) अचर {
    यदि (U_FAILURE(status)) फर्काऊ nullptr;

    // must only be called when lock is already held

    ICUService* ncthis = const_cast<ICUService*>(एतत्); // cast away semantic const
    यदि (idCache == nullptr) {
        ncthis->idCache = नूतन Hashtable(status);
        यदि (idCache == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        } अन्यथा यदि (factories != nullptr) {
            लागि (int32_t pos = factories->size(); --pos >= 0;) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(pos));
                f->updateVisibleIDs(*idCache, status);
            }
            यदि (U_FAILURE(status)) {
                delete idCache;
                ncthis->idCache = nullptr;
            }
        }
    }

    फर्काऊ idCache;
}


UnicodeString& 
ICUService::getDisplayName(अचर UnicodeString& id, UnicodeString& result) अचर 
{
    फर्काऊ getDisplayName(id, result, Locale::getDefault());
}

UnicodeString& 
ICUService::getDisplayName(अचर UnicodeString& id, UnicodeString& result, अचर Locale& locale) अचर 
{
    {
        UErrorCode status = U_ZERO_ERROR;
        Mutex mutex(&lock);
        अचर Hashtable* map = getVisibleIDMap(status);
        यदि (map != nullptr) {
            ICUServiceFactory* f = static_cast<ICUServiceFactory*>(map->get(id));
            यदि (f != nullptr) {
                f->getDisplayName(id, locale, result);
                फर्काऊ result;
            }

            // fallback
            status = U_ZERO_ERROR;
            ICUService_साँचो* fallback_साँचो = create_साँचो(&id, status);
            जबसम्म (fallback_साँचो != nullptr && fallback_साँचो->fallback()) {
                UnicodeString us;
                fallback_साँचो->currentID(us);
                f = static_cast<ICUServiceFactory*>(map->get(us));
                यदि (f != nullptr) {
                    f->getDisplayName(id, locale, result);
                    delete fallback_साँचो;
                    फर्काऊ result;
                }
            }
            delete fallback_साँचो;
        }
    }
    result.setToBogus();
    फर्काऊ result;
}

UVector& 
ICUService::getDisplayNames(UVector& result, UErrorCode& status) अचर 
{
    फर्काऊ getDisplayNames(result, Locale::getDefault(), nullptr, status);
}


UVector& 
ICUService::getDisplayNames(UVector& result, अचर Locale& locale, UErrorCode& status) अचर 
{
    फर्काऊ getDisplayNames(result, locale, nullptr, status);
}

UVector& 
ICUService::getDisplayNames(UVector& result, 
                            अचर Locale& locale, 
                            अचर UnicodeString* matchID, 
                            UErrorCode& status) अचर 
{
    result.removeAllElements();
    result.setDeleter(userv_deleteStringPair);
    यदि (U_SUCCESS(status)) {
        ICUService* ncthis = const_cast<ICUService*>(एतत्); // cast away semantic const
        Mutex mutex(&lock);

        यदि (dnCache != nullptr && dnCache->locale != locale) {
            delete dnCache;
            ncthis->dnCache = nullptr;
        }

        यदि (dnCache == nullptr) {
            अचर Hashtable* m = getVisibleIDMap(status);
            यदि (U_FAILURE(status)) {
                फर्काऊ result;
            }
            ncthis->dnCache = नूतन DNCache(locale); 
            यदि (dnCache == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                फर्काऊ result;
            }

            int32_t pos = UHASH_FIRST;
            अचर UHashElement* entry = nullptr;
            जबसम्म ((entry = m->nextElement(pos)) != nullptr) {
                अचर UnicodeString* id = static_cast<अचर UnicodeString*>(entry->साँचो.pointer);
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(entry->value.pointer);
                UnicodeString dname;
                f->getDisplayName(*id, locale, dname);
                यदि (dname.isBogus()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                } अन्यथा {
                    dnCache->cache.put(dname, (शून्य*)id, status); // share pointer with visibleIDMap
                    यदि (U_SUCCESS(status)) {
                        जारी;
                    }
                }
                delete dnCache;
                ncthis->dnCache = nullptr;
                फर्काऊ result;
            }
        }
    }

    ICUService_साँचो* match_साँचो = create_साँचो(matchID, status);
    /* To ensure that all elements in the hashtable are iterated, set pos to -1.
     * nextElement(pos) will skip the position at pos and begin the iteration
     * at the next position, which in this case will be 0.
     */
    int32_t pos = UHASH_FIRST; 
    अचर UHashElement *entry = nullptr;
    जबसम्म ((entry = dnCache->cache.nextElement(pos)) != nullptr) {
        अचर UnicodeString* id = static_cast<अचर UnicodeString*>(entry->value.pointer);
        यदि (match_साँचो != nullptr && !match_साँचो->isFallbackOf(*id)) {
            जारी;
        }
        अचर UnicodeString* dn = static_cast<अचर UnicodeString*>(entry->साँचो.pointer);
        StringPair* sp = StringPair::create(*id, *dn, status);
        result.adoptElement(sp, status);
        यदि (U_FAILURE(status)) {
            result.removeAllElements();
            रोक;
        }
    }
    delete match_साँचो;

    फर्काऊ result;
}

URegistry_साँचो
ICUService::registerInstance(UObject* objToAdopt, अचर UnicodeString& id, UErrorCode& status) 
{
    फर्काऊ registerInstance(objToAdopt, id, सत्य, status);
}

URegistry_साँचो
ICUService::registerInstance(UObject* objToAdopt, अचर UnicodeString& id, UBool visible, UErrorCode& status) 
{
    ICUService_साँचो* साँचो = create_साँचो(&id, status);
    यदि (साँचो != nullptr) {
        UnicodeString canonicalID;
        साँचो->canonicalID(canonicalID);
        delete साँचो;

        ICUServiceFactory* f = createSimpleFactory(objToAdopt, canonicalID, visible, status);
        यदि (f != nullptr) {
            फर्काऊ registerFactory(f, status);
        }
    }
    delete objToAdopt;
    फर्काऊ nullptr;
}

ICUServiceFactory* 
ICUService::createSimpleFactory(UObject* objToAdopt, अचर UnicodeString& id, UBool visible, UErrorCode& status)
{
    यदि (U_SUCCESS(status)) {
        यदि ((objToAdopt != nullptr) && (!id.isBogus())) {
            फर्काऊ नूतन SimpleFactory(objToAdopt, id, visible);
        }
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    फर्काऊ nullptr;
}

URegistry_साँचो
ICUService::registerFactory(ICUServiceFactory* factoryToAdopt, UErrorCode& status)
{
    LocalPointer<ICUServiceFactory>lpFactoryToAdopt(factoryToAdopt);
    यदि (U_FAILURE(status) || factoryToAdopt == nullptr) {
        फर्काऊ nullptr;
    }
    {
        Mutex mutex(&lock);

        यदि (factories == nullptr) {
            LocalPointer<UVector> lpFactories(नूतन UVector(uprv_deleteUObject, nullptr, status), status);
            यदि (U_FAILURE(status)) {
                फर्काऊ nullptr;
            }
            factories = lpFactories.orphan();
        }
        factories->घुसाउ_Element_At(lpFactoryToAdopt.orphan(), 0, status);
        यदि (U_SUCCESS(status)) {
            clearCaches();
        }
    }   // Close of mutex lock block.

    यदि (U_SUCCESS(status)) {
        notifyChanged();
        फर्काऊ (URegistry_साँचो)factoryToAdopt;
    } अन्यथा {
        फर्काऊ nullptr;
    }
}

UBool 
ICUService::unregister(URegistry_साँचो rkey, UErrorCode& status) 
{
    ICUServiceFactory *factory = (ICUServiceFactory*)rkey;
    UBool result = असत्य;
    यदि (factory != nullptr && factories != nullptr) {
        Mutex mutex(&lock);

        यदि (factories->removeElement(factory)) {
            clearCaches();
            result = सत्य;
        } अन्यथा {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            delete factory;
        }
    }
    यदि (result) {
        notifyChanged();
    }
    फर्काऊ result;
}

शून्य 
ICUService::reset() 
{
    {
        Mutex mutex(&lock);
        reInitializeFactories();
        clearCaches();
    }
    notifyChanged();
}

शून्य 
ICUService::reInitializeFactories() 
{
    यदि (factories != nullptr) {
        factories->removeAllElements();
    }
}

UBool 
ICUService::isDefault() अचर 
{
    फर्काऊ गन्ती_Factories() == 0;
}

ICUService_साँचो* 
ICUService::create_साँचो(अचर UnicodeString* id, UErrorCode& status) अचर 
{
    फर्काऊ (U_FAILURE(status) || id == nullptr) ? nullptr : नूतन ICUService_साँचो(*id);
}

शून्य 
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

शून्य 
ICUService::clearServiceCache() 
{
    // callers synchronize before use
    delete serviceCache; serviceCache = nullptr;
}

UBool 
ICUService::acceptsListener(अचर EventListener& l) अचर 
{
    फर्काऊ dynamic_cast<अचर ServiceListener*>(&l) != nullptr;
}

शून्य 
ICUService::notifyListener(EventListener& l) अचर 
{
    (static_cast<ServiceListener&>(l)).serviceChanged(*एतत्);
}

UnicodeString&
ICUService::getName(UnicodeString& result) अचर 
{
    फर्काऊ result.append(name);
}

int32_t 
ICUService::गन्ती_Factories() अचर 
{
    फर्काऊ factories == nullptr ? 0 : factories->size();
}

int32_t
ICUService::getTimestamp() अचर
{
    फर्काऊ timestamp;
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif
