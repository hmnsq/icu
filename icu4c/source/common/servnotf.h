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

#अन्तर्भाव "unicode/utypes.h"

#यदि UCONFIG_NO_SERVICE

U_NAMESPACE_BEGIN

/*
 * Allow the declaration of APIs with pointers to BreakIterator
 * even when break iteration is removed from the build.
 */
class ICUNotifier;

U_NAMESPACE_END

#अन्यथा

#अन्तर्भाव "unicode/uobject.h"
#अन्तर्भाव "unicode/unistr.h"

#अन्तर्भाव "mutex.h"
#अन्तर्भाव "uvector.h"

U_NAMESPACE_BEGIN

class U_COMMON_API EventListener : सार्वजनिक UObject {
सार्वजनिक: 
    आभास ~EventListener();

सार्वजनिक:
    स्थिर UClassID U_EXPORT2 getStaticClassID();

    आभास UClassID getDynamicClassID() अचर override;

सार्वजनिक:
#ifdef SERVICE_DEBUG
    आभास UnicodeString& debug(UnicodeString& result) अचर {
      फर्काऊ debugClass(result);
    }

    आभास UnicodeString& debugClass(UnicodeString& result) अचर {
      फर्काऊ result.append((UnicodeString)"Key");
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

class U_COMMON_API ICUNotifier : सार्वजनिक UMemory  {
गुप्त: UVector* listeners;
         
सार्वजनिक: 
    ICUNotifier();
    
    आभास ~ICUNotifier();
    
    /**
     * Add a listener to be notified when notifyChanged is called.
     * The listener must not be null. AcceptsListener must return
     * true for the listener. Attempts to concurrently
     * register the identical listener more than once will be
     * silently ignored.  
     */
    आभास शून्य addListener(अचर EventListener* l, UErrorCode& status);
    
    /**
     * Stop notifying this listener.  The listener must
     * not be null. Attempts to remove a listener that is
     * not registered will be silently ignored.
     */
    आभास शून्य removeListener(अचर EventListener* l, UErrorCode& status);
    
    /**
     * ICU doesn't spawn its own threads.  All listeners are notified in
     * the thread of the caller.  Misbehaved listeners can therefore
     * indefinitely block the calling thread.  Callers should beware of
     * deadlock situations.  
     */
    आभास शून्य notifyChanged();
    
रक्षित: 
    /**
     * Subclasses implement this to return true if the listener is
     * of the appropriate type.
     */
    आभास UBool acceptsListener(अचर EventListener& l) अचर = 0;
    
    /**
     * Subclasses implement this to notify the listener.
     */
    आभास शून्य notifyListener(EventListener& l) अचर = 0;
};

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif

/* ICUNOTIF_H */
#endif
