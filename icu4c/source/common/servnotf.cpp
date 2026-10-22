// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2012, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */

#अन्तर्भाव "unicode/utypes.h"

#जर !UCONFIG_NO_SERVICE

#अन्तर्भाव "servnotf.h"
#ifdef NOTIFIER_DEBUG
#अन्तर्भाव <stdio.h>
#endif

U_NAMESPACE_BEGIN

EventListener::~EventListener() {}
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(EventListener)

स्थिर UMutex notifyLock;

ICUNotifier::ICUNotifier() 
: listeners(nullptr) 
{
}

ICUNotifier::~ICUNotifier() {
    {
        Mutex lmx(&notifyLock);
        delete listeners;
        listeners = nullptr;
    }
}


शून्य 
ICUNotifier::addListener(स्थिरांक EventListener* l, UErrorCode& status) 
{
    जर (U_SUCCESS(status)) {
        जर (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            परत;
        }

        जर (acceptsListener(*l)) {
            Mutex lmx(&notifyLock);
            जर (listeners == nullptr) {
                LocalPointer<UVector> lpListeners(नवीन UVector(5, status), status);
                जर (U_FAILURE(status)) {
                    परत;
                }
                listeners = lpListeners.orphan();
            } अन्यथा {
                साठी (पूर्णांक i = 0, e = listeners->size(); i < e; ++i) {
                    स्थिरांक EventListener* el = static_cast<स्थिरांक EventListener*>(listeners->elementAt(i));
                    जर (l == el) {
                        परत;
                    }
                }
            }

            listeners->addElement((शून्य*)l, status); // cast away const
        }
#ifdef NOTIFIER_DEBUG
        अन्यथा {
            ख_लिखो(मानक_त्रुटि, "Listener invalid for this notifier.");
            निकास(1);
        }
#endif
    }
}

शून्य 
ICUNotifier::removeListener(स्थिरांक EventListener *l, UErrorCode& status) 
{
    जर (U_SUCCESS(status)) {
        जर (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            परत;
        }

        {
            Mutex lmx(&notifyLock);
            जर (listeners != nullptr) {
                // identity equality check
                साठी (पूर्णांक i = 0, e = listeners->size(); i < e; ++i) {
                    स्थिरांक EventListener* el = static_cast<स्थिरांक EventListener*>(listeners->elementAt(i));
                    जर (l == el) {
                        listeners->removeElementAt(i);
                        जर (listeners->size() == 0) {
                            delete listeners;
                            listeners = nullptr;
                        }
                        परत;
                    }
                }
            }
        }
    }
}

शून्य 
ICUNotifier::notifyChanged() 
{
    Mutex lmx(&notifyLock);
    जर (listeners != nullptr) {
        साठी (पूर्णांक i = 0, e = listeners->size(); i < e; ++i) {
            EventListener* el = static_cast<EventListener*>(listeners->elementAt(i));
            notifyListener(*el);
        }
    }
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif

