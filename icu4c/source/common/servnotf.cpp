// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2012, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_SERVICE

#समावेश "servnotf.h"
#अगर_घोषित NOTIFIER_DEBUG
#समावेश <stdio.h>
#पूर्ण_अगर

U_NAMESPACE_BEGIN

EventListener::~EventListener() {}
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(EventListener)

अटल UMutex notifyLock;

ICUNotifier::ICUNotifier() 
: listeners(nullptr) 
{
}

ICUNotifier::~ICUNotifier() {
    {
        Mutex lmx(&notifyLock);
        मिटाओ listeners;
        listeners = nullptr;
    }
}


व्योम 
ICUNotifier::addListener(स्थिर EventListener* l, UErrorCode& status) 
{
    अगर (U_SUCCESS(status)) {
        अगर (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            वापस;
        }

        अगर (acceptsListener(*l)) {
            Mutex lmx(&notifyLock);
            अगर (listeners == nullptr) {
                LocalPointer<UVector> lpListeners(नया UVector(5, status), status);
                अगर (U_FAILURE(status)) {
                    वापस;
                }
                listeners = lpListeners.orphan();
            } अन्यथा {
                क्रम (पूर्णांक i = 0, e = listeners->size(); i < e; ++i) {
                    स्थिर EventListener* el = static_cast<स्थिर EventListener*>(listeners->elementAt(i));
                    अगर (l == el) {
                        वापस;
                    }
                }
            }

            listeners->addElement((व्योम*)l, status); // cast away const
        }
#अगर_घोषित NOTIFIER_DEBUG
        अन्यथा {
            ख_लिखो(मानक_त्रुटि, "Listener invalid for this notifier.");
            निकास(1);
        }
#पूर्ण_अगर
    }
}

व्योम 
ICUNotifier::removeListener(स्थिर EventListener *l, UErrorCode& status) 
{
    अगर (U_SUCCESS(status)) {
        अगर (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            वापस;
        }

        {
            Mutex lmx(&notifyLock);
            अगर (listeners != nullptr) {
                // identity equality check
                क्रम (पूर्णांक i = 0, e = listeners->size(); i < e; ++i) {
                    स्थिर EventListener* el = static_cast<स्थिर EventListener*>(listeners->elementAt(i));
                    अगर (l == el) {
                        listeners->removeElementAt(i);
                        अगर (listeners->size() == 0) {
                            मिटाओ listeners;
                            listeners = nullptr;
                        }
                        वापस;
                    }
                }
            }
        }
    }
}

व्योम 
ICUNotifier::notifyChanged() 
{
    Mutex lmx(&notifyLock);
    अगर (listeners != nullptr) {
        क्रम (पूर्णांक i = 0, e = listeners->size(); i < e; ++i) {
            EventListener* el = static_cast<EventListener*>(listeners->elementAt(i));
            notifyListener(*el);
        }
    }
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#पूर्ण_अगर

