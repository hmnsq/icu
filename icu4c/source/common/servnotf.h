// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2014, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */
#यदि_अपरिभाषितम् ICUNOTIF_H
#परिभाषय ICUNOTIF_H

#अन्तर्भाव "unicode/utypes.h"

#यदि UCONFIG_NO_SERVICE

U_NAMESPACE_BEGIN

/*
 * Allow the declaration of APIs with pointers to BreakIterator
 * even when break iteration is removed from the build.
 */
class ICUNotifier;

U_NAMESPACE_अन्तः

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

    आभास UClassID getDynamicClassID() ध्रुव override;

सार्वजनिक:
#यदि_परिभाषितम् SERVICE_DEBUG
    आभास UnicodeString& debug(UnicodeString& परिणाम) ध्रुव {
      प्रत्यागम debugClass(परिणाम);
    }

    आभास UnicodeString& debugClass(UnicodeString& परिणाम) ध्रुव {
      प्रत्यागम परिणाम.append((UnicodeString)"Key");
    }
#यदि_अन्तः
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
    आभास शून्य सङ्कलय_Listener(ध्रुव EventListener* l, UError_संहिता& स्थितिः);
    
    /**
     * Stop notifying this listener.  The listener must
     * not be null. Attempts to remove a listener that is
     * not registered will be silently ignored.
     */
    आभास शून्य removeListener(ध्रुव EventListener* l, UError_संहिता& स्थितिः);
    
    /**
     * ICU doesn't spawn its own threads.  All listeners are notified in
     * the thread of the caller.  Misbehaved listeners can therefore
     * indefinitely block the calling thread.  Callers should beware of
     * deadlock situations.  
     */
    आभास शून्य notify_परिवर्तितम्();
    
रक्षित: 
    /**
     * Subclasses implement this to return true if the listener is
     * of the appropriate type.
     */
    आभास UBool acceptsListener(ध्रुव EventListener& l) ध्रुव = 0;
    
    /**
     * Subclasses implement this to notify the listener.
     */
    आभास शून्य notifyListener(EventListener& l) ध्रुव = 0;
};

U_NAMESPACE_अन्तः

/* UCONFIG_NO_SERVICE */
#यदि_अन्तः

/* ICUNOTIF_H */
#यदि_अन्तः
