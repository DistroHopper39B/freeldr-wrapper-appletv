/*
 * PROJECT:     FreeLoader wrapper for Apple TV
 * LICENSE:     MIT (https://spdx.org/licenses/MIT)
 * PURPOSE:     Mach-O parser for finding FreeLoader for the original Apple TV
 * COPYRIGHT:   Copyright 2023-2024 DistroHopper39B (distrohopper39b.business@gmail.com)
 */

/* INCLUDES *******************************************************************/

#include <fldrwrapper.h>

/* FUNCTIONS ******************************************************************/

static
PMACHO_SECTION GetSectionByNameFromHeader(PMACHO_HEADER Header, const char *SegmentName,
                                          const char *SectionName) {
    PMACHO_SEGMENT_COMMAND Segment;
    PMACHO_SECTION Section;

    debug_printf("Scanning header for %s,%s...", SegmentName, SectionName);

    Segment = (PMACHO_SEGMENT_COMMAND) ((char *) Header + sizeof(MACHO_HEADER));
    for (int i = 0; i < Header->NumberOfCmds; i++) {
        if (Segment->Command == MACHO_LC_SEGMENT) {
            if (strncmp(Segment->SegmentName, SegmentName, strlen(Segment->SegmentName)) == 0 ||
                Header->FileType == MACHO_OBJECT) {
                /* We found the matching segment */
                Section = (PMACHO_SECTION) ((char *) Segment + sizeof(MACHO_SEGMENT_COMMAND));
                for (int j = 0; j < Segment->NumberOfSections; j++) {
                    if (strncmp(Section->SegmentName, SegmentName, strlen(Section->SegmentName)) == 0 &&
                        strncmp(Section->SectionName, SectionName, strlen(Section->SectionName)) == 0) {
                        /* We found the matching section */
                        debug_printf("Found %s,%s @ 0x%08X size %d\n", SegmentName, SectionName,
                                     Section->Address, Section->Size);
                        return (Section);
                    }

                    Section = (PMACHO_SECTION)((char *) Section + sizeof(MACHO_SECTION));
                }
            }
        }
        Segment = (PMACHO_SEGMENT_COMMAND)((char *) Segment + sizeof(MACHO_SEGMENT_COMMAND));
    }
    /* Segment does not exist */
    return (PMACHO_SECTION) 0;
}

u8 *GetSectionDataFromHeader(PMACHO_HEADER Header, const char *SegmentName, const char *SectionName,
                             u32 *Size) {
    PMACHO_SECTION Section;

    Section = GetSectionByNameFromHeader(Header, SegmentName, SectionName);
    if (Section == (PMACHO_SECTION) 0) {
        *Size = 0;
        printf("FATAL: Could not find %s,%s!\n", SegmentName, SectionName);
        fail();

        return ((u8 *) 0);
    }
    *Size = Section->Size;

    return ((u8 *) (Section->Address));
}