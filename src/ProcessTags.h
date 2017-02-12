#ifndef PROCESS_TAGS_H
#define PROCESS_TAGS_H

#include "TagInfo.h"
#include <stdio.h>

int selectAndProcessTag(unsigned char tagType, TagInfo tagInfo);
int processTagString(TagInfo tagInfo);
int processTagLoadClass(TagInfo tagInfo);
int processTagUnloadClass(TagInfo tagInfo);
int processTagStackFrame(TagInfo tagInfo);
int processTagStackTrace(FILE * f, int dataLength);
int processTagAllocSites(FILE * f, int dataLength);
int processTagHeapSummary(TagInfo tagInfo);
int processTagStartThread(TagInfo tagInfo);
int processTagEndThread(TagInfo tagInfo);
int processTagHeapDump(FILE * f, int dataLength);
int processTagHeapSegment(FILE * f, int dataLength);
int processTagHeapDumpEnd(TagInfo tagInfo);
int processTagCpuSamples(FILE * f, int dataLength);
int processTagControlSettings(TagInfo tagInfo);

#endif