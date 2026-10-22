// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2014, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */
#अगर_अघोषित ICUNOTIF_H
#घोषणा ICUNOTIF_H

#समावेश "unicode/utypes.h"

#अगर UCONFIG_NO_SERVICE

U_NAMESPACE_BEGIN

/*
 * Allow the declaration of APIs with pointers to BreakIterator
 * even when break iteration is removed from the build.
 */
class ICUNotifier;

U_NAMESPACE_END

#अन्यथा

#समावेश "unicode/uobject.h"
#समावेश "unicode/unistr.h"

#समावेश "mutex.h"
#समावेश "uvector.h"

U_NAMESPACE_BEGIN

class U_COMMON_API EventListener : खुला UObject {
खुला: 
    भव ~EventListener();

खुला:
    अटल UClassID U_EXPORT2 getStaticClassID();

    भव UClassID getDynamicClassID() स्थिर override;

खुला:
#अगर_घोषित SERVICE_DEBUG
    भव UnicodeString& debug(UnicodeString& परिणाम) स्थिर {
      वापस debugClass(परिणाम);
    }

    भव UnicodeString& debugClass(UnicodeString& परिणाम) स्थिर {
      वापस परिणाम.append((UnicodeString)"Key");
    }
#पूर्ण_अगर
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

class U_COMMON_API ICUNotifier : खुला UMemory  {
निजी: UVector* listeners;
         
खुला: 
    ICUNotifier();
    
    भव ~ICUNotifier();
    
    /**
     * Add a listener to be notified when notifyChanged is called.
     * The listener must not be null. AcceptsListener must return
     * true for the listener. Attempts to concurrently
     * register the identical listener more than once will be
     * silently ignored.  
     */
    भव व्योम addListener(स्थिर EventListener* l, UErrorCode& status);
    
    /**
     * Stop notifying this listener.  The listener must
     * not be null. Attempts to remove a listener that is
     * not registered will be silently ignored.
     */
    भव व्योम removeListener(स्थिर EventListener* l, UErrorCode& status);
    
    /**
     * ICU doesn't spawn its own threads.  All listeners are notified in
     * the thread of the caller.  Misbehaved listeners can therefore
     * indefinitely block the calling thread.  Callers should beware of
     * deadlock situations.  
     */
    भव व्योम notifyChanged();
    
रक्षित: 
    /**
     * Subclasses implement this to return true if the listener is
     * of the appropriate type.
     */
    भव UBool acceptsListener(स्थिर EventListener& l) स्थिर = 0;
    
    /**
     * Subclasses implement this to notify the listener.
     */
    भव व्योम notifyListener(EventListener& l) स्थिर = 0;
};

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#पूर्ण_अगर

/* ICUNOTIF_H */
#पूर्ण_अगर
