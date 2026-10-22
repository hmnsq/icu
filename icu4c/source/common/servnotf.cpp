// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2012, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_SERVICE

#অন্তর্ভাব "servnotf.h"
#ifdef NOTIFIER_DEBUG
#অন্তর্ভাব <stdio.h>
#endif

U_NAMESPACE_BEGIN

EventListener::~EventListener() {}
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(EventListener)

স্থির UMutex notifyLock;

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


শূন্য 
ICUNotifier::addListener(ধ্রুব EventListener* l, UErrorCode& status) 
{
    যদি (U_SUCCESS(status)) {
        যদি (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            প্রত্যাগম;
        }

        যদি (acceptsListener(*l)) {
            Mutex lmx(&notifyLock);
            যদি (listeners == nullptr) {
                LocalPointer<UVector> lpListeners(নূতন UVector(5, status), status);
                যদি (U_FAILURE(status)) {
                    প্রত্যাগম;
                }
                listeners = lpListeners.orphan();
            } অন্যথা {
                প্রতি (পূর্ণাঙ্ক i = 0, e = listeners->size(); i < e; ++i) {
                    ধ্রুব EventListener* el = static_cast<ধ্রুব EventListener*>(listeners->elementAt(i));
                    যদি (l == el) {
                        প্রত্যাগম;
                    }
                }
            }

            listeners->addElement((শূন্য*)l, status); // cast away const
        }
#ifdef NOTIFIER_DEBUG
        অন্যথা {
            fprintf(stderr, "Listener invalid for this notifier.");
            exit(1);
        }
#endif
    }
}

শূন্য 
ICUNotifier::removeListener(ধ্রুব EventListener *l, UErrorCode& status) 
{
    যদি (U_SUCCESS(status)) {
        যদি (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            প্রত্যাগম;
        }

        {
            Mutex lmx(&notifyLock);
            যদি (listeners != nullptr) {
                // identity equality check
                প্রতি (পূর্ণাঙ্ক i = 0, e = listeners->size(); i < e; ++i) {
                    ধ্রুব EventListener* el = static_cast<ধ্রুব EventListener*>(listeners->elementAt(i));
                    যদি (l == el) {
                        listeners->removeElementAt(i);
                        যদি (listeners->size() == 0) {
                            delete listeners;
                            listeners = nullptr;
                        }
                        প্রত্যাগম;
                    }
                }
            }
        }
    }
}

শূন্য 
ICUNotifier::notifyChanged() 
{
    Mutex lmx(&notifyLock);
    যদি (listeners != nullptr) {
        প্রতি (পূর্ণাঙ্ক i = 0, e = listeners->size(); i < e; ++i) {
            EventListener* el = static_cast<EventListener*>(listeners->elementAt(i));
            notifyListener(*el);
        }
    }
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif

