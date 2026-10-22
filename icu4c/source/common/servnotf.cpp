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
ICUNotifier::addListener(ধ্রুবক EventListener* l, UErrorCode& status) 
{
    যদি (U_SUCCESS(status)) {
        যদি (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ফেরত;
        }

        যদি (acceptsListener(*l)) {
            Mutex lmx(&notifyLock);
            যদি (listeners == nullptr) {
                LocalPointer<UVector> lpListeners(নতুন UVector(5, status), status);
                যদি (U_FAILURE(status)) {
                    ফেরত;
                }
                listeners = lpListeners.orphan();
            } অন্যথা {
                জন্য (পূর্ণসংখ্যা i = 0, e = listeners->size(); i < e; ++i) {
                    ধ্রুবক EventListener* el = static_cast<ধ্রুবক EventListener*>(listeners->elementAt(i));
                    যদি (l == el) {
                        ফেরত;
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
ICUNotifier::removeListener(ধ্রুবক EventListener *l, UErrorCode& status) 
{
    যদি (U_SUCCESS(status)) {
        যদি (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ফেরত;
        }

        {
            Mutex lmx(&notifyLock);
            যদি (listeners != nullptr) {
                // identity equality check
                জন্য (পূর্ণসংখ্যা i = 0, e = listeners->size(); i < e; ++i) {
                    ধ্রুবক EventListener* el = static_cast<ধ্রুবক EventListener*>(listeners->elementAt(i));
                    যদি (l == el) {
                        listeners->removeElementAt(i);
                        যদি (listeners->size() == 0) {
                            delete listeners;
                            listeners = nullptr;
                        }
                        ফেরত;
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
        জন্য (পূর্ণসংখ্যা i = 0, e = listeners->size(); i < e; ++i) {
            EventListener* el = static_cast<EventListener*>(listeners->elementAt(i));
            notifyListener(*el);
        }
    }
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif

