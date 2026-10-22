// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2012, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಯದಿ !UCONFIG_NO_SERVICE

#ಅನ್ತರ್ಭಾವ "servnotf.h"
#ifdef NOTIFIER_DEBUG
#ಅನ್ತರ್ಭಾವ <stdio.h>
#endif

U_NAMESPACE_BEGIN

EventListener::~EventListener() {}
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(EventListener)

ಸ್ಥಿರ UMutex notifyLock;

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


ಶೂನ್ಯ 
ICUNotifier::addListener(ಧ್ರುವ EventListener* l, UErrorCode& status) 
{
    ಯದಿ (U_SUCCESS(status)) {
        ಯದಿ (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ಪ್ರತ್ಯಾಗಮ;
        }

        ಯದಿ (acceptsListener(*l)) {
            Mutex lmx(&notifyLock);
            ಯದಿ (listeners == nullptr) {
                LocalPointer<UVector> lpListeners(ನೂತನ UVector(5, status), status);
                ಯದಿ (U_FAILURE(status)) {
                    ಪ್ರತ್ಯಾಗಮ;
                }
                listeners = lpListeners.orphan();
            } ಅನ್ಯಥಾ {
                ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0, e = listeners->size(); i < e; ++i) {
                    ಧ್ರುವ EventListener* el = static_cast<ಧ್ರುವ EventListener*>(listeners->elementAt(i));
                    ಯದಿ (l == el) {
                        ಪ್ರತ್ಯಾಗಮ;
                    }
                }
            }

            listeners->addElement((ಶೂನ್ಯ*)l, status); // cast away const
        }
#ifdef NOTIFIER_DEBUG
        ಅನ್ಯಥಾ {
            fprintf(stderr, "Listener invalid for this notifier.");
            exit(1);
        }
#endif
    }
}

ಶೂನ್ಯ 
ICUNotifier::removeListener(ಧ್ರುವ EventListener *l, UErrorCode& status) 
{
    ಯದಿ (U_SUCCESS(status)) {
        ಯದಿ (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ಪ್ರತ್ಯಾಗಮ;
        }

        {
            Mutex lmx(&notifyLock);
            ಯದಿ (listeners != nullptr) {
                // identity equality check
                ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0, e = listeners->size(); i < e; ++i) {
                    ಧ್ರುವ EventListener* el = static_cast<ಧ್ರುವ EventListener*>(listeners->elementAt(i));
                    ಯದಿ (l == el) {
                        listeners->removeElementAt(i);
                        ಯದಿ (listeners->size() == 0) {
                            delete listeners;
                            listeners = nullptr;
                        }
                        ಪ್ರತ್ಯಾಗಮ;
                    }
                }
            }
        }
    }
}

ಶೂನ್ಯ 
ICUNotifier::notifyChanged() 
{
    Mutex lmx(&notifyLock);
    ಯದಿ (listeners != nullptr) {
        ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0, e = listeners->size(); i < e; ++i) {
            EventListener* el = static_cast<EventListener*>(listeners->elementAt(i));
            notifyListener(*el);
        }
    }
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif

