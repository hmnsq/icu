// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2012, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି !UCONFIG_NO_SERVICE

#ଅନ୍ତର୍ଭାଵ "servnotf.h"
#ifdef NOTIFIER_DEBUG
#ଅନ୍ତର୍ଭାଵ <stdio.h>
#endif

U_NAMESPACE_BEGIN

EventListener::~EventListener() {}
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(EventListener)

ସ୍ଥିର UMutex notifyLock;

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


ଶୂନ୍ୟ 
ICUNotifier::addListener(ଅଚଳ EventListener* l, UErrorCode& status) 
{
    ଯଦି (U_SUCCESS(status)) {
        ଯଦି (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ଫେରନ୍ତୁ;
        }

        ଯଦି (acceptsListener(*l)) {
            Mutex lmx(&notifyLock);
            ଯଦି (listeners == nullptr) {
                LocalPointer<UVector> lpListeners(ନୂତନ UVector(5, status), status);
                ଯଦି (U_FAILURE(status)) {
                    ଫେରନ୍ତୁ;
                }
                listeners = lpListeners.orphan();
            } ଅନ୍ୟଥା {
                ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0, e = listeners->size(); i < e; ++i) {
                    ଅଚଳ EventListener* el = static_cast<ଅଚଳ EventListener*>(listeners->elementAt(i));
                    ଯଦି (l == el) {
                        ଫେରନ୍ତୁ;
                    }
                }
            }

            listeners->addElement((ଶୂନ୍ୟ*)l, status); // cast away const
        }
#ifdef NOTIFIER_DEBUG
        ଅନ୍ୟଥା {
            fprintf(stderr, "Listener invalid for this notifier.");
            exit(1);
        }
#endif
    }
}

ଶୂନ୍ୟ 
ICUNotifier::removeListener(ଅଚଳ EventListener *l, UErrorCode& status) 
{
    ଯଦି (U_SUCCESS(status)) {
        ଯଦି (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ଫେରନ୍ତୁ;
        }

        {
            Mutex lmx(&notifyLock);
            ଯଦି (listeners != nullptr) {
                // identity equality check
                ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0, e = listeners->size(); i < e; ++i) {
                    ଅଚଳ EventListener* el = static_cast<ଅଚଳ EventListener*>(listeners->elementAt(i));
                    ଯଦି (l == el) {
                        listeners->removeElementAt(i);
                        ଯଦି (listeners->size() == 0) {
                            delete listeners;
                            listeners = nullptr;
                        }
                        ଫେରନ୍ତୁ;
                    }
                }
            }
        }
    }
}

ଶୂନ୍ୟ 
ICUNotifier::notifyChanged() 
{
    Mutex lmx(&notifyLock);
    ଯଦି (listeners != nullptr) {
        ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0, e = listeners->size(); i < e; ++i) {
            EventListener* el = static_cast<EventListener*>(listeners->elementAt(i));
            notifyListener(*el);
        }
    }
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif

