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
    U_ASSERT(fResB || f_والد);
    UTRACE_داخلا(UTRACE_UDATA_RESOURCE);
    UError_ڪوڊ حالت = U_ZERO_ERROR;

    CharString file_رستو;
    get_File_رستو(file_رستو, حالت);

    CharString res_رستو;
    get_Res_رستو(res_رستو, حالت);

    // The longest type ("intvector") is 9 chars
    ثابت حرف kSpaces[] = "         ";
    CharString format;
    format.append(kSpaces, حجم(kSpaces) - 1 - uprv_strlen(resType), حالت);
    format.append("(%s) %s @ %s", حالت);

    UTRACE_DATA3(UTRACE_VERBOSE,
        format.معلومات(),
        resType,
        file_رستو.معلومات(),
        res_رستو.معلومات());
    UTRACE_EXIT_حالت(حالت);
}

فراغ ResourceTracer::traceOpen() ثابت {
    U_ASSERT(fResB);
    UTRACE_داخلا(UTRACE_UDATA_BUNDLE);
    UError_ڪوڊ حالت = U_ZERO_ERROR;

    CharString file_رستو;
    UTRACE_DATA1(UTRACE_VERBOSE, "%s", get_File_رستو(file_رستو, حالت).معلومات());
    UTRACE_EXIT_حالت(حالت);
}

CharString& ResourceTracer::get_File_رستو(CharString& output, UError_ڪوڊ& حالت) ثابت {
    إذا (fResB) {
        // Note: if you get a segfault around here, check that ResourceTable and
        // ResourceArray instances outlive ResourceValue instances referring to
        // their contents:
        output.append(fResB->f_معلومات->f_رستو, حالت);
        output.append('/', حالت);
        output.append(fResB->f_معلومات->fName, حالت);
        output.append(".res", حالت);
    } وإلا {
        f_والد->get_File_رستو(output, حالت);
    }
    أرجع output;
}

CharString& ResourceTracer::get_Res_رستو(CharString& output, UError_ڪوڊ& حالت) ثابت {
    إذا (fResB) {
        output.append('/', حالت);
        output.append(fResB->f_Res_رستو, حالت);
        // removing the trailing /
        U_ASSERT(output[output.length()-1] == '/');
        output.truncate(output.length()-1);
    } وإلا {
        f_والد->get_Res_رستو(output, حالت);
    }
    إذا (f_ڪنجي) {
        output.append('/', حالت);
        output.append(f_ڪنجي, حالت);
    }
    إذا (f_اشاريو != -1) {
        output.append('[', حالت);
        UnicodeString اشاريو_String;
        ICU_Utility::append_عدد(اشاريو_String, f_اشاريو);
        output.appendInvariantChars(اشاريو_String, حالت);
        output.append(']', حالت);
    }
    أرجع output;
}

فراغ FileTracer::traceOpen(ثابت حرف* رستو, ثابت حرف* type, ثابت حرف* name) {
    إذا (uprv_strcmp(type, "res") == 0) {
        traceOpenResFile(رستو, name);
    } وإلا {
        trace_Open_معلومات_File(رستو, type, name);
    }
}

فراغ FileTracer::trace_Open_معلومات_File(ثابت حرف* رستو, ثابت حرف* type, ثابت حرف* name) {
    UTRACE_داخلا(UTRACE_UDATA_معلومات_FILE);
    UError_ڪوڊ حالت = U_ZERO_ERROR;

    CharString file_رستو;
    file_رستو.append(رستو, حالت);
    file_رستو.append('/', حالت);
    file_رستو.append(name, حالت);
    file_رستو.append('.', حالت);
    file_رستو.append(type, حالت);

    UTRACE_DATA1(UTRACE_VERBOSE, "%s", file_رستو.معلومات());
    UTRACE_EXIT_حالت(حالت);
}

فراغ FileTracer::traceOpenResFile(ثابت حرف* رستو, ثابت حرف* name) {
    UTRACE_داخلا(UTRACE_UDATA_RES_FILE);
    UError_ڪوڊ حالت = U_ZERO_ERROR;

    CharString file_رستو;
    file_رستو.append(رستو, حالت);
    file_رستو.append('/', حالت);
    file_رستو.append(name, حالت);
    file_رستو.append(".res", حالت);

    UTRACE_DATA1(UTRACE_VERBOSE, "%s", file_رستو.معلومات());
    UTRACE_EXIT_حالت(حالت);
}

U_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو // U_ENABLE_TRACING
