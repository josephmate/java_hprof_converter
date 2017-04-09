#ifndef PROCESS_TAGS_H
#define PROCESS_TAGS_H

#include "TagInfo.h"
#include <stdio.h>

int selectAndProcessTag(unsigned char tagType, TagInfo tagInfo);
int processTagString(TagInfo tagInfo);
int processTagLoadClass(TagInfo tagInfo);
int processTagUnloadClass(TagInfo tagInfo);
int processTagStackFrame(TagInfo tagInfo);
int processTagStackTrace(TagInfo tagInfo);
int processTagAllocSites(TagInfo tagInfo);
int processTagHeapSummary(TagInfo tagInfo);
int processTagStartThread(TagInfo tagInfo);
int processTagEndThread(TagInfo tagInfo);
int processTagHeapDump(TagInfo tagInfo);
int processTagHeapSegment(TagInfo tagInfo);
int processTagHeapDumpEnd(TagInfo tagInfo);
int processTagCpuSamples(TagInfo tagInfo);
int processTagControlSettings(TagInfo tagInfo);

#endif