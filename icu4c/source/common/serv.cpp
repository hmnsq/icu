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

#अपरिभाषय SERVICE_REFCOUNT

// in case we use the refcount stuff

U_NAMESPACE_BEGIN

/*
******************************************************************
*/

ध्रुव char16_t ICUService_कुञ्चिका::उपसर्गः_DELIMITER = 0x002F;   /* '/' */

ICUService_कुञ्चिका::ICUService_कुञ्चिका(ध्रुव UnicodeString& id) 
: _id(id) {
}

ICUService_कुञ्चिका::~ICUService_कुञ्चिका() 
{
}

ध्रुव UnicodeString& 
ICUService_कुञ्चिका::getID() ध्रुव 
{
    प्रत्यागम _id;
}

UnicodeString& 
ICUService_कुञ्चिका::canonicalID(UnicodeString& परिणाम) ध्रुव 
{
    प्रत्यागम परिणाम.append(_id);
}

UnicodeString& 
ICUService_कुञ्चिका::currentID(UnicodeString& परिणाम) ध्रुव 
{
    प्रत्यागम canonicalID(परिणाम);
}

UnicodeString& 
ICUService_कुञ्चिका::currentDescriptor(UnicodeString& परिणाम) ध्रुव 
{
    उपसर्गः(परिणाम);
    परिणाम.append(उपसर्गः_DELIMITER);
    प्रत्यागम currentID(परिणाम);
}

UBool 
ICUService_कुञ्चिका::fallback() 
{
    प्रत्यागम असत्य;
}

UBool 
ICUService_कुञ्चिका::is_Fallback_सम्बन्धि(ध्रुव UnicodeString& id) ध्रुव 
{
    प्रत्यागम id == _id;
}

UnicodeString& 
ICUService_कुञ्चिका::उपसर्गः(UnicodeString& परिणाम) ध्रुव 
{
    प्रत्यागम परिणाम;
}

UnicodeString& 
ICUService_कुञ्चिका::parse_उपसर्गः(UnicodeString& परिणाम) 
{
    int32_t n = परिणाम.सूचकांक_सम्बन्धि(उपसर्गः_DELIMITER);
    यदि (n < 0) {
        n = 0;
    }
    परिणाम.remove(n);
    प्रत्यागम परिणाम;
}

UnicodeString& 
ICUService_कुञ्चिका::parseSuffix(UnicodeString& परिणाम) 
{
    int32_t n = परिणाम.सूचकांक_सम्बन्धि(उपसर्गः_DELIMITER);
    यदि (n >= 0) {
        परिणाम.remove(0, n+1);
    }
    प्रत्यागम परिणाम;
}

#यदि_परिभाषितम् SERVICE_DEBUG
UnicodeString& 
ICUService_कुञ्चिका::debug(UnicodeString& परिणाम) ध्रुव 
{
    debugClass(परिणाम);
    परिणाम.append((UnicodeString)" id: ");
    परिणाम.append(_id);
    प्रत्यागम परिणाम;
}

UnicodeString& 
ICUService_कुञ्चिका::debugClass(UnicodeString& परिणाम) ध्रुव 
{
    प्रत्यागम परिणाम.append((UnicodeString)"ICUServiceKey");
}
#यदि_अन्तः

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ICUService_कुञ्चिका)

/*
******************************************************************
*/

ICUServiceFactory::~ICUServiceFactory() {}

SimpleFactory::SimpleFactory(UObject* instance_गन्तव्यम्_Adopt, ध्रुव UnicodeString& id, UBool visible) 
: _instance(instance_गन्तव्यम्_Adopt), _id(id), _visible(visible)
{
}

SimpleFactory::~SimpleFactory() 
{
    विलोपय _instance;
}

UObject* 
SimpleFactory::create(ध्रुव ICUService_कुञ्चिका& कुञ्चिका, ध्रुव ICUService* service, UError_संहिता& स्थितिः) ध्रुव 
{
    यदि (U_SUCCESS(स्थितिः)) {
        UnicodeString अस्थायी;
        यदि (_id == कुञ्चिका.currentID(अस्थायी)) {
            प्रत्यागम service->cloneInstance(_instance); 
        }
    }
    प्रत्यागम शून्यसूचकः;
}

शून्य 
SimpleFactory::updateVisibleIDs(Hashtable& परिणाम, UError_संहिता& स्थितिः) ध्रुव 
{
    यदि (_visible) {
        परिणाम.put(_id, (शून्य*)एतत्, स्थितिः); // cast away const
    } अन्यथा {
        परिणाम.remove(_id);
    }
}

UnicodeString& 
SimpleFactory::getDisplayName(ध्रुव UnicodeString& id, ध्रुव Locale& /* locale */, UnicodeString& परिणाम) ध्रुव 
{
    यदि (_visible && _id == id) {
        परिणाम = _id;
    } अन्यथा {
        परिणाम.समुच्चयः_गन्तव्यम्_Bogus();
    }
    प्रत्यागम परिणाम;
}

#यदि_परिभाषितम् SERVICE_DEBUG
UnicodeString& 
SimpleFactory::debug(UnicodeString& गन्तव्यम्_Append_गन्तव्यम्) ध्रुव 
{
    debugClass(गन्तव्यम्_Append_गन्तव्यम्);
    गन्तव्यम्_Append_गन्तव्यम्.append((UnicodeString)" id: ");
    गन्तव्यम्_Append_गन्तव्यम्.append(_id);
    गन्तव्यम्_Append_गन्तव्यम्.append((UnicodeString)", visible: ");
    गन्तव्यम्_Append_गन्तव्यम्.append(_visible ? (UnicodeString)"T" : (UnicodeString)"F");
    प्रत्यागम गन्तव्यम्_Append_गन्तव्यम्;
}

UnicodeString& 
SimpleFactory::debugClass(UnicodeString& गन्तव्यम्_Append_गन्तव्यम्) ध्रुव 
{
    प्रत्यागम गन्तव्यम्_Append_गन्तव्यम्.append((UnicodeString)"SimpleFactory");
}
#यदि_अन्तः

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
वर्ग Cache_प्रविष्टिः : सार्वजनिक UMemory {
गुप्त:
    int32_t refcount;

सार्वजनिक:
    UnicodeString actualDescriptor;
    UObject* service;

    /**
    * Releases a reference to the shared resource.
    */
    ~Cache_प्रविष्टिः() {
        विलोपय service;
    }

    Cache_प्रविष्टिः(ध्रुव UnicodeString& _actualDescriptor, UObject* _service) 
        : refcount(1), actualDescriptor(_actualDescriptor), service(_service) {
    }

    /**
    * Instantiation creates an initial reference, so don't call this
    * unless you're creating a new pointer to this.  Management of
    * that pointer will have to know how to deal with refcounts.  
    * Return true if the resource has not already been released.
    */
    Cache_प्रविष्टिः* ref() {
        ++refcount;
        प्रत्यागम एतत्;
    }

    /**
    * Destructions removes a reference, so don't call this unless
    * you're removing pointer to this somewhere.  Management of that
    * pointer will have to know how to deal with refcounts.  Once
    * the refcount drops to zero, the resource is released.  Return
    * false if the resource has been released.
    */
    Cache_प्रविष्टिः* unref() {
        यदि ((--refcount) == 0) {
            विलोपय एतत्;
            प्रत्यागम शून्यसूचकः;
        }
        प्रत्यागम एतत्;
    }

    /**
    * Return true if there is at least one reference to this and the
    * resource has not been released.
    */
    UBool isShared() ध्रुव {
        प्रत्यागम refcount > 1;
    }
};

// Deleter for serviceCache
U_CDECL_BEGIN
स्थिर शून्य U_CALLCONV
cacheDeleter(शून्य* obj) {
    U_NAMESPACE_USE ((Cache_प्रविष्टिः*)obj)->unref();
}

U_CDECL_अन्तः

/*
******************************************************************
*/

वर्ग DNCache : सार्वजनिक UMemory {
सार्वजनिक:
    Hashtable cache;
    ध्रुव Locale locale;

    DNCache(ध्रुव Locale& _locale) 
        : cache(), locale(_locale) 
    {
        // cache.setKeyDeleter(uprv_deleteUObject);
    }
};


/*
******************************************************************
*/

StringPair* 
StringPair::create(ध्रुव UnicodeString& displayName, 
                   ध्रुव UnicodeString& id,
                   UError_संहिता& स्थितिः)
{
    यदि (U_SUCCESS(स्थितिः)) {
        StringPair* sp = नूतन StringPair(displayName, id);
        यदि (sp == शून्यसूचकः || sp->isBogus()) {
            स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
            विलोपय sp;
            प्रत्यागम शून्यसूचकः;
        }
        प्रत्यागम sp;
    }
    प्रत्यागम शून्यसूचकः;
}

UBool 
StringPair::isBogus() ध्रुव {
    प्रत्यागम displayName.isBogus() || id.isBogus();
}

StringPair::StringPair(ध्रुव UnicodeString& _displayName, 
                       ध्रुव UnicodeString& _id)
: displayName(_displayName)
, id(_id)
{
}

U_CDECL_BEGIN
स्थिर शून्य U_CALLCONV
userv_deleteStringPair(शून्य *obj) {
    U_NAMESPACE_USE विलोपय (StringPair*) obj;
}
U_CDECL_अन्तः

/*
******************************************************************
*/

स्थिर UMutex तालकम्;

ICUService::ICUService()
: name()
, timestamp(0)
, factories(शून्यसूचकः)
, serviceCache(शून्यसूचकः)
, idCache(शून्यसूचकः)
, dnCache(शून्यसूचकः)
{
}

ICUService::ICUService(ध्रुव UnicodeString& newName) 
: name(newName)
, timestamp(0)
, factories(शून्यसूचकः)
, serviceCache(शून्यसूचकः)
, idCache(शून्यसूचकः)
, dnCache(शून्यसूचकः)
{
}

ICUService::~ICUService()
{
    {
        Mutex mutex(&तालकम्);
        clearCaches();
        विलोपय factories;
        factories = शून्यसूचकः;
    }
}

UObject* 
ICUService::get(ध्रुव UnicodeString& descriptor, UError_संहिता& स्थितिः) ध्रुव 
{
    प्रत्यागम get(descriptor, शून्यसूचकः, स्थितिः);
}

UObject* 
ICUService::get(ध्रुव UnicodeString& descriptor, UnicodeString* actualReturn, UError_संहिता& स्थितिः) ध्रुव 
{
    UObject* परिणाम = शून्यसूचकः;
    ICUService_कुञ्चिका* कुञ्चिका = create_कुञ्चिका(&descriptor, स्थितिः);
    यदि (कुञ्चिका) {
        परिणाम = get_कुञ्चिका(*कुञ्चिका, actualReturn, स्थितिः);
        विलोपय कुञ्चिका;
    }
    प्रत्यागम परिणाम;
}

UObject* 
ICUService::get_कुञ्चिका(ICUService_कुञ्चिका& कुञ्चिका, UError_संहिता& स्थितिः) ध्रुव 
{
    प्रत्यागम get_कुञ्चिका(कुञ्चिका, शून्यसूचकः, स्थितिः);
}

// this is a vector that subclasses of ICUService can override to further customize the result object
// before returning it.  All other public get functions should call this one.

UObject* 
ICUService::get_कुञ्चिका(ICUService_कुञ्चिका& कुञ्चिका, UnicodeString* actualReturn, UError_संहिता& स्थितिः) ध्रुव 
{
    प्रत्यागम get_कुञ्चिका(कुञ्चिका, actualReturn, शून्यसूचकः, स्थितिः);
}

// make it possible to call reentrantly on systems that don't have reentrant mutexes.
// we can use this simple approach since we know the situation where we're calling
// reentrantly even without knowing the thread.
वर्ग XMutex : सार्वजनिक UMemory {
सार्वजनिक:
    अन्तःस्थ XMutex(UMutex *mutex, UBool reentering) 
        : fMutex(mutex)
        , f_सक्रिय(!reentering) 
    {
        यदि (f_सक्रिय) umtx_तालकम्(fMutex);
    }
    अन्तःस्थ ~XMutex() {
        यदि (f_सक्रिय) umtx_unlock(fMutex);
    }

गुप्त:
    UMutex  *fMutex;
    UBool f_सक्रिय;
};

// called only by factories, treat as private
UObject* 
ICUService::get_कुञ्चिका(ICUService_कुञ्चिका& कुञ्चिका, UnicodeString* actualReturn, ध्रुव ICUServiceFactory* factory, UError_संहिता& स्थितिः) ध्रुव 
{
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }

    यदि (isDefault()) {
        प्रत्यागम handleDefault(कुञ्चिका, actualReturn, स्थितिः);
    }

    ICUService* ncthis = const_cast<ICUService*>(एतत्); // cast away semantic const

    Cache_प्रविष्टिः* परिणाम = शून्यसूचकः;
    {
        // The factory list can't be modified until we're done, 
        // otherwise we might update the cache with an invalid result.
        // The cache has to stay in synch with the factory list.
        // ICU doesn't have monitors so we can't use rw locks, so 
        // we single-thread everything using this service, for now.

        // if factory is not null, we're calling from within the mutex,
        // and since some unix machines don't have reentrant mutexes we
        // need to make sure not to try to lock it again.
        XMutex mutex(&तालकम्, factory != शून्यसूचकः);

        यदि (serviceCache == शून्यसूचकः) {
            ncthis->serviceCache = नूतन Hashtable(स्थितिः);
            यदि (ncthis->serviceCache == शून्यसूचकः) {
                स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
                प्रत्यागम शून्यसूचकः;
            }
            यदि (U_FAILURE(स्थितिः)) {
                विलोपय serviceCache;
                प्रत्यागम शून्यसूचकः;
            }
            serviceCache->समुच्चयः_मानम्_Deleter(cacheDeleter);
        }

        UnicodeString currentDescriptor;
        LocalPointer<UVector> cacheDescriptorList;
        UBool put_अन्तर्_Cache = असत्य;

        int32_t start_सूचकांक = 0;
        int32_t सीमा = factories->size();
        UBool cache_परिणाम = सत्य;

        यदि (factory != शून्यसूचकः) {
            प्रति (int32_t i = 0; i < सीमा; ++i) {
                यदि (factory == static_cast<ध्रुव ICUServiceFactory*>(factories->elementAt(i))) {
                    start_सूचकांक = i + 1;
                    भङ्ग;
                }
            }
            यदि (start_सूचकांक == 0) {
                // throw new InternalError("Factory " + factory + "not registered with service: " + this);
                स्थितिः = U_ILLEGAL_तर्कः_ERROR;
                प्रत्यागम शून्यसूचकः;
            }
            cache_परिणाम = असत्य;
        }

        कुरु {
            currentDescriptor.remove();
            कुञ्चिका.currentDescriptor(currentDescriptor);
            परिणाम = static_cast<Cache_प्रविष्टिः*>(serviceCache->get(currentDescriptor));
            यदि (परिणाम != शून्यसूचकः) {
                भङ्ग;
            }

            // first test of cache failed, so we'll have to update
            // the cache if we eventually succeed-- that is, if we're 
            // going to update the cache at all.
            put_अन्तर्_Cache = सत्य;

            int32_t सूचकांक = start_सूचकांक;
            यावत् (सूचकांक < सीमा) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(सूचकांक++));
                LocalPointer<UObject> service(f->create(कुञ्चिका, एतत्, स्थितिः));
                यदि (U_FAILURE(स्थितिः)) {
                    प्रत्यागम शून्यसूचकः;
                }
                यदि (service.isValid()) {
                    परिणाम = नूतन Cache_प्रविष्टिः(currentDescriptor, service.getAlias());
                    यदि (परिणाम == शून्यसूचकः) {
                        स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
                        प्रत्यागम शून्यसूचकः;
                    }
                    service.orphan(); // result now owns service.

                    गच्छ outer_अन्तः;
                }
            }

            // prepare to load the cache with all additional ids that 
            // will resolve to result, assuming we'll succeed.  We
            // don't want to keep querying on an id that's going to
            // fallback to the one that succeeded, we want to hit the
            // cache the first time next goaround.
            यदि (cacheDescriptorList.isNull()) {
                cacheDescriptorList.adopt_Instead_And_Check_Error_संहिता(नूतन UVector(uprv_deleteUObject, शून्यसूचकः, 5, स्थितिः), स्थितिः);
                यदि (U_FAILURE(स्थितिः)) {
                    प्रत्यागम शून्यसूचकः;
                }
            }

            LocalPointer<UnicodeString> id_गन्तव्यम्_Cache(नूतन UnicodeString(currentDescriptor), स्थितिः);
            यदि (U_FAILURE(स्थितिः)) {
                प्रत्यागम शून्यसूचकः;
            }
            यदि (id_गन्तव्यम्_Cache->isBogus()) {
                स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
                प्रत्यागम शून्यसूचकः;
            }
            cacheDescriptorList->adoptElement(id_गन्तव्यम्_Cache.orphan(), स्थितिः);
            यदि (U_FAILURE(स्थितिः)) {
                प्रत्यागम शून्यसूचकः;
            }
        } यावत् (कुञ्चिका.fallback());
outer_अन्तः:

        यदि (परिणाम != शून्यसूचकः) {
            यदि (put_अन्तर्_Cache && cache_परिणाम) {
                serviceCache->put(परिणाम->actualDescriptor, परिणाम, स्थितिः);
                यदि (U_FAILURE(स्थितिः)) {
                    प्रत्यागम शून्यसूचकः;
                }

                यदि (cacheDescriptorList.isValid()) {
                    प्रति (int32_t i = cacheDescriptorList->size(); --i >= 0;) {
                        UnicodeString* desc = static_cast<UnicodeString*>(cacheDescriptorList->elementAt(i));

                        serviceCache->put(*desc, परिणाम, स्थितिः);
                        यदि (U_FAILURE(स्थितिः)) {
                            प्रत्यागम शून्यसूचकः;
                        }

                        परिणाम->ref();
                        cacheDescriptorList->removeElementAt(i);
                    }
                }
            }

            यदि (actualReturn != शून्यसूचकः) {
                // strip null prefix
                यदि (परिणाम->actualDescriptor.सूचकांक_सम्बन्धि(static_cast<char16_t>(0x2f)) == 0) { // U+002f=slash (/)
                    actualReturn->remove();
                    actualReturn->append(परिणाम->actualDescriptor, 
                        1, 
                        परिणाम->actualDescriptor.length() - 1);
                } अन्यथा {
                    *actualReturn = परिणाम->actualDescriptor;
                }

                यदि (actualReturn->isBogus()) {
                    स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
                    विलोपय परिणाम;
                    प्रत्यागम शून्यसूचकः;
                }
            }

            UObject* service = cloneInstance(परिणाम->service);
            यदि (put_अन्तर्_Cache && !cache_परिणाम) {
                विलोपय परिणाम;
            }
            प्रत्यागम service;
        }
    }

    प्रत्यागम handleDefault(कुञ्चिका, actualReturn, स्थितिः);
}

UObject* 
ICUService::handleDefault(ध्रुव ICUService_कुञ्चिका& /*कुञ्चिका*/, UnicodeString* /* actualIDReturn */, UError_संहिता& /* status */) ध्रुव 
{
    प्रत्यागम शून्यसूचकः;
}

UVector& 
ICUService::getVisibleIDs(UVector& परिणाम, UError_संहिता& स्थितिः) ध्रुव {
    प्रत्यागम getVisibleIDs(परिणाम, शून्यसूचकः, स्थितिः);
}

UVector& 
ICUService::getVisibleIDs(UVector& परिणाम, ध्रुव UnicodeString* मेलनम्_ID, UError_संहिता& स्थितिः) ध्रुव 
{
    परिणाम.removeAllElements();

    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम परिणाम;
    }
    UObjectDeleter *savedDeleter = परिणाम.समुच्चयः_Deleter(uprv_deleteUObject);

    {
        Mutex mutex(&तालकम्);
        ध्रुव Hashtable* map = getVisibleIDMap(स्थितिः);
        यदि (map != शून्यसूचकः) {
            ICUService_कुञ्चिका* fallback_कुञ्चिका = create_कुञ्चिका(मेलनम्_ID, स्थितिः);

            प्रति (int32_t pos = UHASH_FIRST; U_SUCCESS(स्थितिः); ) {
                ध्रुव UHashElement* e = map->अग्र_Element(pos);
                यदि (e == शून्यसूचकः) {
                    भङ्ग;
                }

                ध्रुव UnicodeString* id = static_cast<ध्रुव UnicodeString*>(e->कुञ्चिका.pointer);
                यदि (fallback_कुञ्चिका != शून्यसूचकः) {
                    यदि (!fallback_कुञ्चिका->is_Fallback_सम्बन्धि(*id)) {
                        अनुवर्त;
                    }
                }

                LocalPointer<UnicodeString> idClone(id->clone(), स्थितिः);
                परिणाम.adoptElement(idClone.orphan(), स्थितिः);
            }
            विलोपय fallback_कुञ्चिका;
        }
    }
    यदि (U_FAILURE(स्थितिः)) {
        परिणाम.removeAllElements();
    }
    परिणाम.समुच्चयः_Deleter(savedDeleter);
    प्रत्यागम परिणाम;
}

ध्रुव Hashtable* 
ICUService::getVisibleIDMap(UError_संहिता& स्थितिः) ध्रुव {
    यदि (U_FAILURE(स्थितिः)) प्रत्यागम शून्यसूचकः;

    // must only be called when lock is already held

    ICUService* ncthis = const_cast<ICUService*>(एतत्); // cast away semantic const
    यदि (idCache == शून्यसूचकः) {
        ncthis->idCache = नूतन Hashtable(स्थितिः);
        यदि (idCache == शून्यसूचकः) {
            स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        } अन्यथा यदि (factories != शून्यसूचकः) {
            प्रति (int32_t pos = factories->size(); --pos >= 0;) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(pos));
                f->updateVisibleIDs(*idCache, स्थितिः);
            }
            यदि (U_FAILURE(स्थितिः)) {
                विलोपय idCache;
                ncthis->idCache = शून्यसूचकः;
            }
        }
    }

    प्रत्यागम idCache;
}


UnicodeString& 
ICUService::getDisplayName(ध्रुव UnicodeString& id, UnicodeString& परिणाम) ध्रुव 
{
    प्रत्यागम getDisplayName(id, परिणाम, Locale::getDefault());
}

UnicodeString& 
ICUService::getDisplayName(ध्रुव UnicodeString& id, UnicodeString& परिणाम, ध्रुव Locale& locale) ध्रुव 
{
    {
        UError_संहिता स्थितिः = U_ZERO_ERROR;
        Mutex mutex(&तालकम्);
        ध्रुव Hashtable* map = getVisibleIDMap(स्थितिः);
        यदि (map != शून्यसूचकः) {
            ICUServiceFactory* f = static_cast<ICUServiceFactory*>(map->get(id));
            यदि (f != शून्यसूचकः) {
                f->getDisplayName(id, locale, परिणाम);
                प्रत्यागम परिणाम;
            }

            // fallback
            स्थितिः = U_ZERO_ERROR;
            ICUService_कुञ्चिका* fallback_कुञ्चिका = create_कुञ्चिका(&id, स्थितिः);
            यावत् (fallback_कुञ्चिका != शून्यसूचकः && fallback_कुञ्चिका->fallback()) {
                UnicodeString us;
                fallback_कुञ्चिका->currentID(us);
                f = static_cast<ICUServiceFactory*>(map->get(us));
                यदि (f != शून्यसूचकः) {
                    f->getDisplayName(id, locale, परिणाम);
                    विलोपय fallback_कुञ्चिका;
                    प्रत्यागम परिणाम;
                }
            }
            विलोपय fallback_कुञ्चिका;
        }
    }
    परिणाम.समुच्चयः_गन्तव्यम्_Bogus();
    प्रत्यागम परिणाम;
}

UVector& 
ICUService::get_Display_नामानि(UVector& परिणाम, UError_संहिता& स्थितिः) ध्रुव 
{
    प्रत्यागम get_Display_नामानि(परिणाम, Locale::getDefault(), शून्यसूचकः, स्थितिः);
}


UVector& 
ICUService::get_Display_नामानि(UVector& परिणाम, ध्रुव Locale& locale, UError_संहिता& स्थितिः) ध्रुव 
{
    प्रत्यागम get_Display_नामानि(परिणाम, locale, शून्यसूचकः, स्थितिः);
}

UVector& 
ICUService::get_Display_नामानि(UVector& परिणाम, 
                            ध्रुव Locale& locale, 
                            ध्रुव UnicodeString* मेलनम्_ID, 
                            UError_संहिता& स्थितिः) ध्रुव 
{
    परिणाम.removeAllElements();
    परिणाम.समुच्चयः_Deleter(userv_deleteStringPair);
    यदि (U_SUCCESS(स्थितिः)) {
        ICUService* ncthis = const_cast<ICUService*>(एतत्); // cast away semantic const
        Mutex mutex(&तालकम्);

        यदि (dnCache != शून्यसूचकः && dnCache->locale != locale) {
            विलोपय dnCache;
            ncthis->dnCache = शून्यसूचकः;
        }

        यदि (dnCache == शून्यसूचकः) {
            ध्रुव Hashtable* m = getVisibleIDMap(स्थितिः);
            यदि (U_FAILURE(स्थितिः)) {
                प्रत्यागम परिणाम;
            }
            ncthis->dnCache = नूतन DNCache(locale); 
            यदि (dnCache == शून्यसूचकः) {
                स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
                प्रत्यागम परिणाम;
            }

            int32_t pos = UHASH_FIRST;
            ध्रुव UHashElement* प्रविष्टिः = शून्यसूचकः;
            यावत् ((प्रविष्टिः = m->अग्र_Element(pos)) != शून्यसूचकः) {
                ध्रुव UnicodeString* id = static_cast<ध्रुव UnicodeString*>(प्रविष्टिः->कुञ्चिका.pointer);
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(प्रविष्टिः->value.pointer);
                UnicodeString dname;
                f->getDisplayName(*id, locale, dname);
                यदि (dname.isBogus()) {
                    स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
                } अन्यथा {
                    dnCache->cache.put(dname, (शून्य*)id, स्थितिः); // share pointer with visibleIDMap
                    यदि (U_SUCCESS(स्थितिः)) {
                        अनुवर्त;
                    }
                }
                विलोपय dnCache;
                ncthis->dnCache = शून्यसूचकः;
                प्रत्यागम परिणाम;
            }
        }
    }

    ICUService_कुञ्चिका* मेलनम्_कुञ्चिका = create_कुञ्चिका(मेलनम्_ID, स्थितिः);
    /* To ensure that all elements in the hashtable are iterated, set pos to -1.
     * nextElement(pos) will skip the position at pos and begin the iteration
     * at the next position, which in this case will be 0.
     */
    int32_t pos = UHASH_FIRST; 
    ध्रुव UHashElement *प्रविष्टिः = शून्यसूचकः;
    यावत् ((प्रविष्टिः = dnCache->cache.अग्र_Element(pos)) != शून्यसूचकः) {
        ध्रुव UnicodeString* id = static_cast<ध्रुव UnicodeString*>(प्रविष्टिः->value.pointer);
        यदि (मेलनम्_कुञ्चिका != शून्यसूचकः && !मेलनम्_कुञ्चिका->is_Fallback_सम्बन्धि(*id)) {
            अनुवर्त;
        }
        ध्रुव UnicodeString* dn = static_cast<ध्रुव UnicodeString*>(प्रविष्टिः->कुञ्चिका.pointer);
        StringPair* sp = StringPair::create(*id, *dn, स्थितिः);
        परिणाम.adoptElement(sp, स्थितिः);
        यदि (U_FAILURE(स्थितिः)) {
            परिणाम.removeAllElements();
            भङ्ग;
        }
    }
    विलोपय मेलनम्_कुञ्चिका;

    प्रत्यागम परिणाम;
}

URegistry_कुञ्चिका
ICUService::पञ्जिकाकोशः_Instance(UObject* obj_गन्तव्यम्_Adopt, ध्रुव UnicodeString& id, UError_संहिता& स्थितिः) 
{
    प्रत्यागम पञ्जिकाकोशः_Instance(obj_गन्तव्यम्_Adopt, id, सत्य, स्थितिः);
}

URegistry_कुञ्चिका
ICUService::पञ्जिकाकोशः_Instance(UObject* obj_गन्तव्यम्_Adopt, ध्रुव UnicodeString& id, UBool visible, UError_संहिता& स्थितिः) 
{
    ICUService_कुञ्चिका* कुञ्चिका = create_कुञ्चिका(&id, स्थितिः);
    यदि (कुञ्चिका != शून्यसूचकः) {
        UnicodeString canonicalID;
        कुञ्चिका->canonicalID(canonicalID);
        विलोपय कुञ्चिका;

        ICUServiceFactory* f = createSimpleFactory(obj_गन्तव्यम्_Adopt, canonicalID, visible, स्थितिः);
        यदि (f != शून्यसूचकः) {
            प्रत्यागम पञ्जिकाकोशः_Factory(f, स्थितिः);
        }
    }
    विलोपय obj_गन्तव्यम्_Adopt;
    प्रत्यागम शून्यसूचकः;
}

ICUServiceFactory* 
ICUService::createSimpleFactory(UObject* obj_गन्तव्यम्_Adopt, ध्रुव UnicodeString& id, UBool visible, UError_संहिता& स्थितिः)
{
    यदि (U_SUCCESS(स्थितिः)) {
        यदि ((obj_गन्तव्यम्_Adopt != शून्यसूचकः) && (!id.isBogus())) {
            प्रत्यागम नूतन SimpleFactory(obj_गन्तव्यम्_Adopt, id, visible);
        }
        स्थितिः = U_ILLEGAL_तर्कः_ERROR;
    }
    प्रत्यागम शून्यसूचकः;
}

URegistry_कुञ्चिका
ICUService::पञ्जिकाकोशः_Factory(ICUServiceFactory* factory_गन्तव्यम्_Adopt, UError_संहिता& स्थितिः)
{
    LocalPointer<ICUServiceFactory>lp_Factory_गन्तव्यम्_Adopt(factory_गन्तव्यम्_Adopt);
    यदि (U_FAILURE(स्थितिः) || factory_गन्तव्यम्_Adopt == शून्यसूचकः) {
        प्रत्यागम शून्यसूचकः;
    }
    {
        Mutex mutex(&तालकम्);

        यदि (factories == शून्यसूचकः) {
            LocalPointer<UVector> lpFactories(नूतन UVector(uprv_deleteUObject, शून्यसूचकः, स्थितिः), स्थितिः);
            यदि (U_FAILURE(स्थितिः)) {
                प्रत्यागम शून्यसूचकः;
            }
            factories = lpFactories.orphan();
        }
        factories->निवेशय_Element_At(lp_Factory_गन्तव्यम्_Adopt.orphan(), 0, स्थितिः);
        यदि (U_SUCCESS(स्थितिः)) {
            clearCaches();
        }
    }   // Close of mutex lock block.

    यदि (U_SUCCESS(स्थितिः)) {
        notify_परिवर्तितम्();
        प्रत्यागम (URegistry_कुञ्चिका)factory_गन्तव्यम्_Adopt;
    } अन्यथा {
        प्रत्यागम शून्यसूचकः;
    }
}

UBool 
ICUService::unregister(URegistry_कुञ्चिका rkey, UError_संहिता& स्थितिः) 
{
    ICUServiceFactory *factory = (ICUServiceFactory*)rkey;
    UBool परिणाम = असत्य;
    यदि (factory != शून्यसूचकः && factories != शून्यसूचकः) {
        Mutex mutex(&तालकम्);

        यदि (factories->removeElement(factory)) {
            clearCaches();
            परिणाम = सत्य;
        } अन्यथा {
            स्थितिः = U_ILLEGAL_तर्कः_ERROR;
            विलोपय factory;
        }
    }
    यदि (परिणाम) {
        notify_परिवर्तितम्();
    }
    प्रत्यागम परिणाम;
}

शून्य 
ICUService::reset() 
{
    {
        Mutex mutex(&तालकम्);
        reInitializeFactories();
        clearCaches();
    }
    notify_परिवर्तितम्();
}

शून्य 
ICUService::reInitializeFactories() 
{
    यदि (factories != शून्यसूचकः) {
        factories->removeAllElements();
    }
}

UBool 
ICUService::isDefault() ध्रुव 
{
    प्रत्यागम गणनम्_Factories() == 0;
}

ICUService_कुञ्चिका* 
ICUService::create_कुञ्चिका(ध्रुव UnicodeString* id, UError_संहिता& स्थितिः) ध्रुव 
{
    प्रत्यागम (U_FAILURE(स्थितिः) || id == शून्यसूचकः) ? शून्यसूचकः : नूतन ICUService_कुञ्चिका(*id);
}

शून्य 
ICUService::clearCaches() 
{
    // callers synchronize before use
    ++timestamp;
    विलोपय dnCache;
    dnCache = शून्यसूचकः;
    विलोपय idCache;
    idCache = शून्यसूचकः;
    विलोपय serviceCache; serviceCache = शून्यसूचकः;
}

शून्य 
ICUService::clearServiceCache() 
{
    // callers synchronize before use
    विलोपय serviceCache; serviceCache = शून्यसूचकः;
}

UBool 
ICUService::acceptsListener(ध्रुव EventListener& l) ध्रुव 
{
    प्रत्यागम dynamic_cast<ध्रुव ServiceListener*>(&l) != शून्यसूचकः;
}

शून्य 
ICUService::notifyListener(EventListener& l) ध्रुव 
{
    (static_cast<ServiceListener&>(l)).service_परिवर्तितम्(*एतत्);
}

UnicodeString&
ICUService::getName(UnicodeString& परिणाम) ध्रुव 
{
    प्रत्यागम परिणाम.append(name);
}

int32_t 
ICUService::गणनम्_Factories() ध्रुव 
{
    प्रत्यागम factories == शून्यसूचकः ? 0 : factories->size();
}

int32_t
ICUService::getTimestamp() ध्रुव
{
    प्रत्यागम timestamp;
}

U_NAMESPACE_अन्तः

/* UCONFIG_NO_SERVICE */
#यदि_अन्तः
