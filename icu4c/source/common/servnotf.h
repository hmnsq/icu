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

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ UCONFIG_NO_SERVICE

U_NAMESPACE_BEGIN

/*
 * Allow the declaration of APIs with pointers to BreakIterator
 * even when break iteration is removed from the build.
 */
class ICUNotifier;

U_NAMESPACE_END

#അല്ലെങ്കിൽ

#അന്തര്ഭാവ "unicode/uobject.h"
#അന്തര്ഭാവ "unicode/unistr.h"

#അന്തര്ഭാവ "mutex.h"
#അന്തര്ഭാവ "uvector.h"

U_NAMESPACE_BEGIN

class U_COMMON_API EventListener : പൊതു UObject {
പൊതു: 
    മായാ ~EventListener();

പൊതു:
    സ്ഥിതം UClassID U_EXPORT2 getStaticClassID();

    മായാ UClassID getDynamicClassID() സ്ഥിരം override;

പൊതു:
#ifdef SERVICE_DEBUG
    മായാ UnicodeString& debug(UnicodeString& result) സ്ഥിരം {
      തിരികെ debugClass(result);
    }

    മായാ UnicodeString& debugClass(UnicodeString& result) സ്ഥിരം {
      തിരികെ result.append((UnicodeString)"Key");
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

class U_COMMON_API ICUNotifier : പൊതു UMemory  {
സ്വകാര്യ: UVector* listeners;
         
പൊതു: 
    ICUNotifier();
    
    മായാ ~ICUNotifier();
    
    /**
     * Add a listener to be notified when notifyChanged is called.
     * The listener must not be null. AcceptsListener must return
     * true for the listener. Attempts to concurrently
     * register the identical listener more than once will be
     * silently ignored.  
     */
    മായാ ശൂന്യം addListener(സ്ഥിരം EventListener* l, UErrorCode& status);
    
    /**
     * Stop notifying this listener.  The listener must
     * not be null. Attempts to remove a listener that is
     * not registered will be silently ignored.
     */
    മായാ ശൂന്യം removeListener(സ്ഥിരം EventListener* l, UErrorCode& status);
    
    /**
     * ICU doesn't spawn its own threads.  All listeners are notified in
     * the thread of the caller.  Misbehaved listeners can therefore
     * indefinitely block the calling thread.  Callers should beware of
     * deadlock situations.  
     */
    മായാ ശൂന്യം notifyChanged();
    
സംരക്ഷിത: 
    /**
     * Subclasses implement this to return true if the listener is
     * of the appropriate type.
     */
    മായാ UBool acceptsListener(സ്ഥിരം EventListener& l) സ്ഥിരം = 0;
    
    /**
     * Subclasses implement this to notify the listener.
     */
    മായാ ശൂന്യം notifyListener(EventListener& l) സ്ഥിരം = 0;
};

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif

/* ICUNOTIF_H */
#endif
