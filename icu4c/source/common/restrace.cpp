// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#অন্তর্ভাব "unicode/utypes.h"

#যদি U_ENABLE_TRACING

#অন্তর্ভাব "restrace.h"
#অন্তর্ভাব "charstr.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "utracimp.h"
#অন্তর্ভাব "uresimp.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "util.h"

U_NAMESPACE_BEGIN

ResourceTracer::~ResourceTracer() = পূর্বনির্ধারিত;

শূন্য ResourceTracer::trace(ধ্রুবক অক্ষর* resType) ধ্রুবক {
    U_ASSERT(fResB || fParent);
    UTRACE_ENTRY(UTRACE_UDATA_RESOURCE);
    UErrorCode status = U_ZERO_ERROR;

    CharString filePath;
    getFilePath(filePath, status);

    CharString resPath;
    getResPath(resPath, status);

    // The longest type ("intvector") is 9 chars
    ধ্রুবক অক্ষর kSpaces[] = "         ";
    CharString format;
    format.append(kSpaces, আকার(kSpaces) - 1 - uprv_strlen(resType), status);
    format.append("(%s) %s @ %s", status);

    UTRACE_DATA3(UTRACE_VERBOSE,
        format.তথ্য(),
        resType,
        filePath.তথ্য(),
        resPath.তথ্য());
    UTRACE_EXIT_STATUS(status);
}

শূন্য ResourceTracer::traceOpen() ধ্রুবক {
    U_ASSERT(fResB);
    UTRACE_ENTRY(UTRACE_UDATA_BUNDLE);
    UErrorCode status = U_ZERO_ERROR;

    CharString filePath;
    UTRACE_DATA1(UTRACE_VERBOSE, "%s", getFilePath(filePath, status).তথ্য());
    UTRACE_EXIT_STATUS(status);
}

CharString& ResourceTracer::getFilePath(CharString& output, UErrorCode& status) ধ্রুবক {
    যদি (fResB) {
        // Note: if you get a segfault around here, check that ResourceTable and
        // ResourceArray instances outlive ResourceValue instances referring to
        // their contents:
        output.append(fResB->f_তথ্য->fPath, status);
        output.append('/', status);
        output.append(fResB->f_তথ্য->fName, status);
        output.append(".res", status);
    } অন্যথা {
        fParent->getFilePath(output, status);
    }
    ফেরত output;
}

CharString& ResourceTracer::getResPath(CharString& output, UErrorCode& status) ধ্রুবক {
    যদি (fResB) {
        output.append('/', status);
        output.append(fResB->fResPath, status);
        // removing the trailing /
        U_ASSERT(output[output.length()-1] == '/');
        output.truncate(output.length()-1);
    } অন্যথা {
        fParent->getResPath(output, status);
    }
    যদি (f_চাবি) {
        output.append('/', status);
        output.append(f_চাবি, status);
    }
    যদি (f_সূচক != -1) {
        output.append('[', status);
        UnicodeString সূচক_String;
        ICU_Utility::append_সংখ্যা(সূচক_String, f_সূচক);
        output.appendInvariantChars(সূচক_String, status);
        output.append(']', status);
    }
    ফেরত output;
}

শূন্য FileTracer::traceOpen(ধ্রুবক অক্ষর* path, ধ্রুবক অক্ষর* type, ধ্রুবক অক্ষর* name) {
    যদি (uprv_strcmp(type, "res") == 0) {
        traceOpenResFile(path, name);
    } অন্যথা {
        trace_Open_তথ্য_File(path, type, name);
    }
}

শূন্য FileTracer::trace_Open_তথ্য_File(ধ্রুবক অক্ষর* path, ধ্রুবক অক্ষর* type, ধ্রুবক অক্ষর* name) {
    UTRACE_ENTRY(UTRACE_UDATA_তথ্য_FILE);
    UErrorCode status = U_ZERO_ERROR;

    CharString filePath;
    filePath.append(path, status);
    filePath.append('/', status);
    filePath.append(name, status);
    filePath.append('.', status);
    filePath.append(type, status);

    UTRACE_DATA1(UTRACE_VERBOSE, "%s", filePath.তথ্য());
    UTRACE_EXIT_STATUS(status);
}

শূন্য FileTracer::traceOpenResFile(ধ্রুবক অক্ষর* path, ধ্রুবক অক্ষর* name) {
    UTRACE_ENTRY(UTRACE_UDATA_RES_FILE);
    UErrorCode status = U_ZERO_ERROR;

    CharString filePath;
    filePath.append(path, status);
    filePath.append('/', status);
    filePath.append(name, status);
    filePath.append(".res", status);

    UTRACE_DATA1(UTRACE_VERBOSE, "%s", filePath.তথ্য());
    UTRACE_EXIT_STATUS(status);
}

U_NAMESPACE_END

#endif // U_ENABLE_TRACING
