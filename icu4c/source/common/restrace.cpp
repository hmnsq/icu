// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ضمن "unicode/utypes.h"

#إذا U_ENABLE_TRACING

#ضمن "restrace.h"
#ضمن "charstr.h"
#ضمن "cstring.h"
#ضمن "utracimp.h"
#ضمن "uresimp.h"
#ضمن "uassert.h"
#ضمن "util.h"

U_NAMESPACE_BEGIN

ResourceTracer::~ResourceTracer() = افتراضي;

فراغ ResourceTracer::trace(ثابت حرف* resType) ثابت {
    U_ASSERT(fResB || f_پلار);
    UTRACE_ثبت(UTRACE_UDATA_RESOURCE);
    UError_کوډ حالت = U_ZERO_ERROR;

    CharString file_لار;
    get_File_لار(file_لار, حالت);

    CharString res_لار;
    get_Res_لار(res_لار, حالت);

    // The longest type ("intvector") is 9 chars
    ثابت حرف kSpaces[] = "         ";
    CharString format;
    format.append(kSpaces, حجم(kSpaces) - 1 - uprv_strlen(resType), حالت);
    format.append("(%s) %s @ %s", حالت);

    UTRACE_DATA3(UTRACE_VERBOSE,
        format.معلومات(),
        resType,
        file_لار.معلومات(),
        res_لار.معلومات());
    UTRACE_EXIT_حالت(حالت);
}

فراغ ResourceTracer::traceOpen() ثابت {
    U_ASSERT(fResB);
    UTRACE_ثبت(UTRACE_UDATA_BUNDLE);
    UError_کوډ حالت = U_ZERO_ERROR;

    CharString file_لار;
    UTRACE_DATA1(UTRACE_VERBOSE, "%s", get_File_لار(file_لار, حالت).معلومات());
    UTRACE_EXIT_حالت(حالت);
}

CharString& ResourceTracer::get_File_لار(CharString& output, UError_کوډ& حالت) ثابت {
    إذا (fResB) {
        // Note: if you get a segfault around here, check that ResourceTable and
        // ResourceArray instances outlive ResourceValue instances referring to
        // their contents:
        output.append(fResB->f_معلومات->f_لار, حالت);
        output.append('/', حالت);
        output.append(fResB->f_معلومات->fName, حالت);
        output.append(".res", حالت);
    } وإلا {
        f_پلار->get_File_لار(output, حالت);
    }
    أرجع output;
}

CharString& ResourceTracer::get_Res_لار(CharString& output, UError_کوډ& حالت) ثابت {
    إذا (fResB) {
        output.append('/', حالت);
        output.append(fResB->f_Res_لار, حالت);
        // removing the trailing /
        U_ASSERT(output[output.length()-1] == '/');
        output.truncate(output.length()-1);
    } وإلا {
        f_پلار->get_Res_لار(output, حالت);
    }
    إذا (f_کلی) {
        output.append('/', حالت);
        output.append(f_کلی, حالت);
    }
    إذا (f_شاخص != -1) {
        output.append('[', حالت);
        UnicodeString شاخص_String;
        ICU_Utility::append_شمېره(شاخص_String, f_شاخص);
        output.appendInvariantChars(شاخص_String, حالت);
        output.append(']', حالت);
    }
    أرجع output;
}

فراغ FileTracer::traceOpen(ثابت حرف* لار, ثابت حرف* type, ثابت حرف* name) {
    إذا (uprv_strcmp(type, "res") == 0) {
        traceOpenResFile(لار, name);
    } وإلا {
        trace_Open_معلومات_File(لار, type, name);
    }
}

فراغ FileTracer::trace_Open_معلومات_File(ثابت حرف* لار, ثابت حرف* type, ثابت حرف* name) {
    UTRACE_ثبت(UTRACE_UDATA_معلومات_FILE);
    UError_کوډ حالت = U_ZERO_ERROR;

    CharString file_لار;
    file_لار.append(لار, حالت);
    file_لار.append('/', حالت);
    file_لار.append(name, حالت);
    file_لار.append('.', حالت);
    file_لار.append(type, حالت);

    UTRACE_DATA1(UTRACE_VERBOSE, "%s", file_لار.معلومات());
    UTRACE_EXIT_حالت(حالت);
}

فراغ FileTracer::traceOpenResFile(ثابت حرف* لار, ثابت حرف* name) {
    UTRACE_ثبت(UTRACE_UDATA_RES_FILE);
    UError_کوډ حالت = U_ZERO_ERROR;

    CharString file_لار;
    file_لار.append(لار, حالت);
    file_لار.append('/', حالت);
    file_لار.append(name, حالت);
    file_لار.append(".res", حالت);

    UTRACE_DATA1(UTRACE_VERBOSE, "%s", file_لار.معلومات());
    UTRACE_EXIT_حالت(حالت);
}

U_NAMESPACE_پای_ټکی

#د_که_پای // U_ENABLE_TRACING
