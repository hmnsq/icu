// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#جے_بنا_تعریف __RESTRACE_H__
#تعریف_کرو __RESTRACE_H__

#ضمن "unicode/utypes.h"

#إذا U_ENABLE_TRACING

بنية UResourceBundle;

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
عام:
    ResourceTracer() :
        fResB(nullptr),
        f_ماپا(nullptr),
        f_کلید(nullptr),
        f_اشاریہ(-1) {}

    ResourceTracer(ثابت UResourceBundle* resB) :
        fResB(resB),
        f_ماپا(nullptr),
        f_کلید(nullptr),
        f_اشاریہ(-1) {}

    ResourceTracer(ثابت UResourceBundle* resB, ثابت حرف* کلید) :
        fResB(resB),
        f_ماپا(nullptr),
        f_کلید(کلید),
        f_اشاریہ(-1) {}

    ResourceTracer(ثابت UResourceBundle* resB, int32_t اشاریہ) :
        fResB(resB),
        f_ماپا(nullptr),
        f_کلید(nullptr),
        f_اشاریہ(اشاریہ) {}

    ResourceTracer(ثابت ResourceTracer& ماپا, ثابت حرف* کلید) :
        fResB(nullptr),
        f_ماپا(&ماپا),
        f_کلید(کلید),
        f_اشاریہ(-1) {}

    ResourceTracer(ثابت ResourceTracer& ماپا, int32_t اشاریہ) :
        fResB(nullptr),
        f_ماپا(&ماپا),
        f_کلید(nullptr),
        f_اشاریہ(اشاریہ) {}

    ~ResourceTracer();

    فراغ trace(ثابت حرف* type) ثابت;
    فراغ traceOpen() ثابت;

    /**
     * Calls trace() if the resB or parent provided to the constructor was
     * non-null; otherwise, does nothing.
     */
    فراغ maybeTrace(ثابت حرف* type) ثابت {
        إذا (fResB || f_ماپا) {
            trace(type);
        }
    }

خاص:
    ثابت UResourceBundle* fResB;
    ثابت ResourceTracer* f_ماپا;
    ثابت حرف* f_کلید;
    int32_t f_اشاریہ;

    CharString& get_File_رستہ(CharString& output, UError_کوڈ& حالت) ثابت;

    CharString& get_Res_رستہ(CharString& output, UError_کوڈ& حالت) ثابت;
};

/**
 * This class provides methods to trace data file reads when ICU is built
 * with --enable-tracing.
 */
class FileTracer {
عام:
    ساكن فراغ traceOpen(ثابت حرف* رستہ, ثابت حرف* type, ثابت حرف* name);

خاص:
    ساكن فراغ trace_Open_انکڑے_File(ثابت حرف* رستہ, ثابت حرف* type, ثابت حرف* name);
    ساكن فراغ traceOpenResFile(ثابت حرف* رستہ, ثابت حرف* name);
};

U_NAMESPACE_سرا

#وإلا // U_ENABLE_TRACING

U_NAMESPACE_BEGIN

/**
 * Default trivial implementation when --enable-tracing is not used.
 */
class U_COMMON_API ResourceTracer {
عام:
    ResourceTracer() {}

    ResourceTracer(ثابت فراغ*) {}

    ResourceTracer(ثابت فراغ*, ثابت حرف*) {}

    ResourceTracer(ثابت فراغ*, int32_t) {}

    ResourceTracer(ثابت ResourceTracer&, ثابت حرف*) {}

    ResourceTracer(ثابت ResourceTracer&, int32_t) {}

    فراغ trace(ثابت حرف*) ثابت {}

    فراغ traceOpen() ثابت {}

    فراغ maybeTrace(ثابت حرف*) ثابت {}
};

/**
 * Default trivial implementation when --enable-tracing is not used.
 */
class FileTracer {
عام:
    ساكن فراغ traceOpen(ثابت حرف*, ثابت حرف*, ثابت حرف*) {}
};

U_NAMESPACE_سرا

#جے_انت // U_ENABLE_TRACING

#جے_انت //__RESTRACE_H__
