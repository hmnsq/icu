// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2012, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */

#அந்தர்பாவ "unicode/utypes.h"

#எனில் !UCONFIG_NO_SERVICE

#அந்தர்பாவ "servnotf.h"
#ifdef NOTIFIER_DEBUG
#அந்தர்பாவ <stdio.h>
#endif

U_NAMESPACE_BEGIN

EventListener::~EventListener() {}
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(EventListener)

நிலையான UMutex notifyLock;

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


வெறுமை 
ICUNotifier::addListener(மாறிலி EventListener* l, UErrorCode& status) 
{
    எனில் (U_SUCCESS(status)) {
        எனில் (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            திரும்பு;
        }

        எனில் (acceptsListener(*l)) {
            Mutex lmx(&notifyLock);
            எனில் (listeners == nullptr) {
                LocalPointer<UVector> lpListeners(புதிய UVector(5, status), status);
                எனில் (U_FAILURE(status)) {
                    திரும்பு;
                }
                listeners = lpListeners.orphan();
            } இல்லையெனில் {
                க்கு (முழு i = 0, e = listeners->size(); i < e; ++i) {
                    மாறிலி EventListener* el = static_cast<மாறிலி EventListener*>(listeners->elementAt(i));
                    எனில் (l == el) {
                        திரும்பு;
                    }
                }
            }

            listeners->addElement((வெறுமை*)l, status); // cast away const
        }
#ifdef NOTIFIER_DEBUG
        இல்லையெனில் {
            fprintf(stderr, "Listener invalid for this notifier.");
            exit(1);
        }
#endif
    }
}

வெறுமை 
ICUNotifier::removeListener(மாறிலி EventListener *l, UErrorCode& status) 
{
    எனில் (U_SUCCESS(status)) {
        எனில் (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            திரும்பு;
        }

        {
            Mutex lmx(&notifyLock);
            எனில் (listeners != nullptr) {
                // identity equality check
                க்கு (முழு i = 0, e = listeners->size(); i < e; ++i) {
                    மாறிலி EventListener* el = static_cast<மாறிலி EventListener*>(listeners->elementAt(i));
                    எனில் (l == el) {
                        listeners->removeElementAt(i);
                        எனில் (listeners->size() == 0) {
                            delete listeners;
                            listeners = nullptr;
                        }
                        திரும்பு;
                    }
                }
            }
        }
    }
}

வெறுமை 
ICUNotifier::notifyChanged() 
{
    Mutex lmx(&notifyLock);
    எனில் (listeners != nullptr) {
        க்கு (முழு i = 0, e = listeners->size(); i < e; ++i) {
            EventListener* el = static_cast<EventListener*>(listeners->elementAt(i));
            notifyListener(*el);
        }
    }
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif

