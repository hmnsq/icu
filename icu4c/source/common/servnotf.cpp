// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2012, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */

#अन्तर्भाव "unicode/utypes.h"

#जइ !UCONFIG_NO_SERVICE

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
ICUNotifier::addListener(ध्रुव EventListener* l, UErrorCode& status) 
{
    जइ (U_SUCCESS(status)) {
        जइ (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            प्रत्यागम;
        }

        जइ (acceptsListener(*l)) {
            Mutex lmx(&notifyLock);
            जइ (listeners == nullptr) {
                LocalPointer<UVector> lpListeners(नूतन UVector(5, status), status);
                जइ (U_FAILURE(status)) {
                    प्रत्यागम;
                }
                listeners = lpListeners.orphan();
            } अण्णहा {
                प्रति (पूर्णाङ्क i = 0, e = listeners->size(); i < e; ++i) {
                    ध्रुव EventListener* el = static_cast<ध्रुव EventListener*>(listeners->elementAt(i));
                    जइ (l == el) {
                        प्रत्यागम;
                    }
                }
            }

            listeners->addElement((शून्य*)l, status); // cast away const
        }
#ifdef NOTIFIER_DEBUG
        अण्णहा {
            fprintf(stderr, "Listener invalid for this notifier.");
            exit(1);
        }
#endif
    }
}

शून्य 
ICUNotifier::removeListener(ध्रुव EventListener *l, UErrorCode& status) 
{
    जइ (U_SUCCESS(status)) {
        जइ (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            प्रत्यागम;
        }

        {
            Mutex lmx(&notifyLock);
            जइ (listeners != nullptr) {
                // identity equality check
                प्रति (पूर्णाङ्क i = 0, e = listeners->size(); i < e; ++i) {
                    ध्रुव EventListener* el = static_cast<ध्रुव EventListener*>(listeners->elementAt(i));
                    जइ (l == el) {
                        listeners->removeElementAt(i);
                        जइ (listeners->size() == 0) {
                            delete listeners;
                            listeners = nullptr;
                        }
                        प्रत्यागम;
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
    जइ (listeners != nullptr) {
        प्रति (पूर्णाङ्क i = 0, e = listeners->size(); i < e; ++i) {
            EventListener* el = static_cast<EventListener*>(listeners->elementAt(i));
            notifyListener(*el);
        }
    }
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif

