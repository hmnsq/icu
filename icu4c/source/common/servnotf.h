// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2014, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */
#ifndef ICUNOTIF_H
#define ICUNOTIF_H

#அந்தர்பாவ "unicode/utypes.h"

#எனில் UCONFIG_NO_SERVICE

U_NAMESPACE_BEGIN

/*
 * Allow the declaration of APIs with pointers to BreakIterator
 * even when break iteration is removed from the build.
 */
class ICUNotifier;

U_NAMESPACE_END

#இல்லையெனில்

#அந்தர்பாவ "unicode/uobject.h"
#அந்தர்பாவ "unicode/unistr.h"

#அந்தர்பாவ "mutex.h"
#அந்தர்பாவ "uvector.h"

U_NAMESPACE_BEGIN

class U_COMMON_API EventListener : பொது UObject {
பொது: 
    மெய்நிகர் ~EventListener();

பொது:
    நிலையான UClassID U_EXPORT2 getStaticClassID();

    மெய்நிகர் UClassID getDynamicClassID() மாறிலி override;

பொது:
#ifdef SERVICE_DEBUG
    மெய்நிகர் UnicodeString& debug(UnicodeString& result) மாறிலி {
      திரும்பு debugClass(result);
    }

    மெய்நிகர் UnicodeString& debugClass(UnicodeString& result) மாறிலி {
      திரும்பு result.append((UnicodeString)"Key");
    }
#endif
};

/**
 * <p>Abstract implementation of a notification facility.  Clients add
 * EventListeners with addListener and remove them with removeListener.
 * Notifiers call notifyChanged when they wish to notify listeners.
 * This queues the listener list on the notification thread, which
 * eventually dequeues the list and calls notifyListener on each
 * listener in the list.</p>
 *
 * <p>Subclasses override acceptsListener and notifyListener 
 * to add type-safe notification.  AcceptsListener should return
 * true if the listener is of the appropriate type; ICUNotifier
 * itself will ensure the listener is non-null and that the
 * identical listener is not already registered with the Notifier.
 * NotifyListener should cast the listener to the appropriate 
 * type and call the appropriate method on the listener.
 */

class U_COMMON_API ICUNotifier : பொது UMemory  {
தனிப்பட்ட: UVector* listeners;
         
பொது: 
    ICUNotifier();
    
    மெய்நிகர் ~ICUNotifier();
    
    /**
     * Add a listener to be notified when notifyChanged is called.
     * The listener must not be null. AcceptsListener must return
     * true for the listener. Attempts to concurrently
     * register the identical listener more than once will be
     * silently ignored.  
     */
    மெய்நிகர் வெறுமை addListener(மாறிலி EventListener* l, UErrorCode& status);
    
    /**
     * Stop notifying this listener.  The listener must
     * not be null. Attempts to remove a listener that is
     * not registered will be silently ignored.
     */
    மெய்நிகர் வெறுமை removeListener(மாறிலி EventListener* l, UErrorCode& status);
    
    /**
     * ICU doesn't spawn its own threads.  All listeners are notified in
     * the thread of the caller.  Misbehaved listeners can therefore
     * indefinitely block the calling thread.  Callers should beware of
     * deadlock situations.  
     */
    மெய்நிகர் வெறுமை notifyChanged();
    
பாதுகாக்கப்பட்ட: 
    /**
     * Subclasses implement this to return true if the listener is
     * of the appropriate type.
     */
    மெய்நிகர் UBool acceptsListener(மாறிலி EventListener& l) மாறிலி = 0;
    
    /**
     * Subclasses implement this to notify the listener.
     */
    மெய்நிகர் வெறுமை notifyListener(EventListener& l) மாறிலி = 0;
};

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif

/* ICUNOTIF_H */
#endif
