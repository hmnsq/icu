// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ifndef __RESTRACE_H__
#define __RESTRACE_H__

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಯದಿ U_ENABLE_TRACING

ಸಂರಚನಾ UResourceBundle;

U_NAMESPACE_BEGIN

class CharString;

/**
 * Instances of this class store information used to trace reads from resource
 * bundles when ICU is built with --enable-tracing.
 *
 * All arguments of type const UResourceBundle*, const char*, and
 * const ResourceTracer& are stored as pointers. The caller must retain
 * ownership for the lifetime of this ResourceTracer.
 *
 * Exported as U_COMMON_API for Windows because it is a value field
 * in other exported types.
 */
class U_COMMON_API ResourceTracer {
ಸಾರ್ವಜನಿಕ:
    ResourceTracer() :
        fResB(nullptr),
        fParent(nullptr),
        f_ಕೀಲಿ(nullptr),
        fIndex(-1) {}

    ResourceTracer(ಧ್ರುವ UResourceBundle* resB) :
        fResB(resB),
        fParent(nullptr),
        f_ಕೀಲಿ(nullptr),
        fIndex(-1) {}

    ResourceTracer(ಧ್ರುವ UResourceBundle* resB, ಧ್ರುವ ವರ್ಣ* ಕೀಲಿ) :
        fResB(resB),
        fParent(nullptr),
        f_ಕೀಲಿ(ಕೀಲಿ),
        fIndex(-1) {}

    ResourceTracer(ಧ್ರುವ UResourceBundle* resB, int32_t index) :
        fResB(resB),
        fParent(nullptr),
        f_ಕೀಲಿ(nullptr),
        fIndex(index) {}

    ResourceTracer(ಧ್ರುವ ResourceTracer& parent, ಧ್ರುವ ವರ್ಣ* ಕೀಲಿ) :
        fResB(nullptr),
        fParent(&parent),
        f_ಕೀಲಿ(ಕೀಲಿ),
        fIndex(-1) {}

    ResourceTracer(ಧ್ರುವ ResourceTracer& parent, int32_t index) :
        fResB(nullptr),
        fParent(&parent),
        f_ಕೀಲಿ(nullptr),
        fIndex(index) {}

    ~ResourceTracer();

    ಶೂನ್ಯ trace(ಧ್ರುವ ವರ್ಣ* type) ಧ್ರುವ;
    ಶೂನ್ಯ traceOpen() ಧ್ರುವ;

    /**
     * Calls trace() if the resB or parent provided to the constructor was
     * non-null; otherwise, does nothing.
     */
    ಶೂನ್ಯ maybeTrace(ಧ್ರುವ ವರ್ಣ* type) ಧ್ರುವ {
        ಯದಿ (fResB || fParent) {
            trace(type);
        }
    }

ಗುಪ್ತ:
    ಧ್ರುವ UResourceBundle* fResB;
    ಧ್ರುವ ResourceTracer* fParent;
    ಧ್ರುವ ವರ್ಣ* f_ಕೀಲಿ;
    int32_t fIndex;

    CharString& getFilePath(CharString& output, UErrorCode& status) ಧ್ರುವ;

    CharString& getResPath(CharString& output, UErrorCode& status) ಧ್ರುವ;
};

/**
 * This class provides methods to trace data file reads when ICU is built
 * with --enable-tracing.
 */
class FileTracer {
ಸಾರ್ವಜನಿಕ:
    ಸ್ಥಿರ ಶೂನ್ಯ traceOpen(ಧ್ರುವ ವರ್ಣ* path, ಧ್ರುವ ವರ್ಣ* type, ಧ್ರುವ ವರ್ಣ* name);

ಗುಪ್ತ:
    ಸ್ಥಿರ ಶೂನ್ಯ traceOpenDataFile(ಧ್ರುವ ವರ್ಣ* path, ಧ್ರುವ ವರ್ಣ* type, ಧ್ರುವ ವರ್ಣ* name);
    ಸ್ಥಿರ ಶೂನ್ಯ traceOpenResFile(ಧ್ರುವ ವರ್ಣ* path, ಧ್ರುವ ವರ್ಣ* name);
};

U_NAMESPACE_END

#ಅನ್ಯಥಾ // U_ENABLE_TRACING

U_NAMESPACE_BEGIN

/**
 * Default trivial implementation when --enable-tracing is not used.
 */
class U_COMMON_API ResourceTracer {
ಸಾರ್ವಜನಿಕ:
    ResourceTracer() {}

    ResourceTracer(ಧ್ರುವ ಶೂನ್ಯ*) {}

    ResourceTracer(ಧ್ರುವ ಶೂನ್ಯ*, ಧ್ರುವ ವರ್ಣ*) {}

    ResourceTracer(ಧ್ರುವ ಶೂನ್ಯ*, int32_t) {}

    ResourceTracer(ಧ್ರುವ ResourceTracer&, ಧ್ರುವ ವರ್ಣ*) {}

    ResourceTracer(ಧ್ರುವ ResourceTracer&, int32_t) {}

    ಶೂನ್ಯ trace(ಧ್ರುವ ವರ್ಣ*) ಧ್ರುವ {}

    ಶೂನ್ಯ traceOpen() ಧ್ರುವ {}

    ಶೂನ್ಯ maybeTrace(ಧ್ರುವ ವರ್ಣ*) ಧ್ರುವ {}
};

/**
 * Default trivial implementation when --enable-tracing is not used.
 */
class FileTracer {
ಸಾರ್ವಜನಿಕ:
    ಸ್ಥಿರ ಶೂನ್ಯ traceOpen(ಧ್ರುವ ವರ್ಣ*, ಧ್ರುವ ವರ್ಣ*, ಧ್ರುವ ವರ್ಣ*) {}
};

U_NAMESPACE_END

#endif // U_ENABLE_TRACING

#endif //__RESTRACE_H__
