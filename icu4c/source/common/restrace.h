// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ifndef __RESTRACE_H__
#define __RESTRACE_H__

#அந்தர்பாவ "unicode/utypes.h"

#எனில் U_ENABLE_TRACING

கட்டமைப்பு UResourceBundle;

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
பொது:
    ResourceTracer() :
        fResB(nullptr),
        fParent(nullptr),
        f_சாவி(nullptr),
        fIndex(-1) {}

    ResourceTracer(மாறிலி UResourceBundle* resB) :
        fResB(resB),
        fParent(nullptr),
        f_சாவி(nullptr),
        fIndex(-1) {}

    ResourceTracer(மாறிலி UResourceBundle* resB, மாறிலி எழுத்து* சாவி) :
        fResB(resB),
        fParent(nullptr),
        f_சாவி(சாவி),
        fIndex(-1) {}

    ResourceTracer(மாறிலி UResourceBundle* resB, int32_t index) :
        fResB(resB),
        fParent(nullptr),
        f_சாவி(nullptr),
        fIndex(index) {}

    ResourceTracer(மாறிலி ResourceTracer& parent, மாறிலி எழுத்து* சாவி) :
        fResB(nullptr),
        fParent(&parent),
        f_சாவி(சாவி),
        fIndex(-1) {}

    ResourceTracer(மாறிலி ResourceTracer& parent, int32_t index) :
        fResB(nullptr),
        fParent(&parent),
        f_சாவி(nullptr),
        fIndex(index) {}

    ~ResourceTracer();

    வெறுமை trace(மாறிலி எழுத்து* type) மாறிலி;
    வெறுமை traceOpen() மாறிலி;

    /**
     * Calls trace() if the resB or parent provided to the constructor was
     * non-null; otherwise, does nothing.
     */
    வெறுமை maybeTrace(மாறிலி எழுத்து* type) மாறிலி {
        எனில் (fResB || fParent) {
            trace(type);
        }
    }

தனிப்பட்ட:
    மாறிலி UResourceBundle* fResB;
    மாறிலி ResourceTracer* fParent;
    மாறிலி எழுத்து* f_சாவி;
    int32_t fIndex;

    CharString& getFilePath(CharString& output, UErrorCode& status) மாறிலி;

    CharString& getResPath(CharString& output, UErrorCode& status) மாறிலி;
};

/**
 * This class provides methods to trace data file reads when ICU is built
 * with --enable-tracing.
 */
class FileTracer {
பொது:
    நிலையான வெறுமை traceOpen(மாறிலி எழுத்து* path, மாறிலி எழுத்து* type, மாறிலி எழுத்து* name);

தனிப்பட்ட:
    நிலையான வெறுமை traceOpenDataFile(மாறிலி எழுத்து* path, மாறிலி எழுத்து* type, மாறிலி எழுத்து* name);
    நிலையான வெறுமை traceOpenResFile(மாறிலி எழுத்து* path, மாறிலி எழுத்து* name);
};

U_NAMESPACE_END

#இல்லையெனில் // U_ENABLE_TRACING

U_NAMESPACE_BEGIN

/**
 * Default trivial implementation when --enable-tracing is not used.
 */
class U_COMMON_API ResourceTracer {
பொது:
    ResourceTracer() {}

    ResourceTracer(மாறிலி வெறுமை*) {}

    ResourceTracer(மாறிலி வெறுமை*, மாறிலி எழுத்து*) {}

    ResourceTracer(மாறிலி வெறுமை*, int32_t) {}

    ResourceTracer(மாறிலி ResourceTracer&, மாறிலி எழுத்து*) {}

    ResourceTracer(மாறிலி ResourceTracer&, int32_t) {}

    வெறுமை trace(மாறிலி எழுத்து*) மாறிலி {}

    வெறுமை traceOpen() மாறிலி {}

    வெறுமை maybeTrace(மாறிலி எழுத்து*) மாறிலி {}
};

/**
 * Default trivial implementation when --enable-tracing is not used.
 */
class FileTracer {
பொது:
    நிலையான வெறுமை traceOpen(மாறிலி எழுத்து*, மாறிலி எழுத்து*, மாறிலி எழுத்து*) {}
};

U_NAMESPACE_END

#endif // U_ENABLE_TRACING

#endif //__RESTRACE_H__
