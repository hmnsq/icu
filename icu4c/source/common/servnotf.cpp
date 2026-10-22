// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2012, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_SERVICE

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
ICUNotifier::addListener(अचर EventListener* l, UErrorCode& status) 
{
    यदि (U_SUCCESS(status)) {
        यदि (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            फर्काऊ;
        }

        यदि (acceptsListener(*l)) {
            Mutex lmx(&notifyLock);
            यदि (listeners == nullptr) {
                LocalPointer<UVector> lpListeners(नूतन UVector(5, status), status);
                यदि (U_FAILURE(status)) {
                    फर्काऊ;
                }
                listeners = lpListeners.orphan();
            } अन्यथा {
                लागि (पूर्णाङ्क i = 0, e = listeners->size(); i < e; ++i) {
                    अचर EventListener* el = static_cast<अचर EventListener*>(listeners->elementAt(i));
                    यदि (l == el) {
                        फर्काऊ;
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
ICUNotifier::removeListener(अचर EventListener *l, UErrorCode& status) 
{
    यदि (U_SUCCESS(status)) {
        यदि (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            फर्काऊ;
        }

        {
            Mutex lmx(&notifyLock);
            यदि (listeners != nullptr) {
                // identity equality check
                लागि (पूर्णाङ्क i = 0, e = listeners->size(); i < e; ++i) {
                    अचर EventListener* el = static_cast<अचर EventListener*>(listeners->elementAt(i));
                    यदि (l == el) {
                        listeners->removeElementAt(i);
                        यदि (listeners->size() == 0) {
                            delete listeners;
                            listeners = nullptr;
                        }
                        फर्काऊ;
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
    यदि (listeners != nullptr) {
        लागि (पूर्णाङ्क i = 0, e = listeners->size(); i < e; ++i) {
            EventListener* el = static_cast<EventListener*>(listeners->elementAt(i));
            notifyListener(*el);
        }
    }
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif

