#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <endian.h>
#include <assert.h>

typedef struct {
    uint32_t sfntVersion;
    uint16_t numTables;
    uint16_t searchRange;
    uint16_t entrySelector;
    uint16_t rangeShift;
} TableDirectory;

// Data types
typedef uint8_t     Tag[4];
typedef uint32_t    Offset32;
typedef uint16_t    Offset16;
typedef int16_t     FWORD;
typedef uint16_t    UFWORD;

typedef struct {
    Tag tableTag;
    uint32_t checksum;
    Offset32 offset;
    uint32_t length;
} TableRecord;

// cmap
typedef struct {
    uint16_t platformID;
    uint16_t encordingID;
    Offset32 subtableOffset;
} EncordingRecord;

typedef struct {
    uint16_t version;
    uint16_t numTables; // number of encording tables that follw
    EncordingRecord encordingRecords[];
} CmapHeader;

typedef struct {
    uint16_t format;
    uint16_t length;
    uint16_t language;
    uint16_t segCountX2;
    uint16_t searchRange;
    uint16_t entrySelector;
    uint16_t rangeShift;
    uint16_t endCode[];
    /*
    uint16_t reservedPad;
    uint16_t startCode[segCount];
    int16_t idDelta[SegCount];
    uint16_t idRangeOffsets[segCount];
    uint16_t glyphldArray[]   
    */ 
}SubtableFormat4;

// maxp
typedef uint32_t    Version16Dot16; // TODO: fix this
typedef int64_t     LONGDATETIME;

typedef struct {
    Version16Dot16 version; 
    uint16_t numGlyphs;
} Maxp;

// head
typedef int32_t Fixed;
typedef struct {
    uint16_t        majorVersion;
    uint16_t        minorVersion;
    Fixed           fontRevision;
    uint32_t        checksumAdjustment;
    uint32_t        magicNUmber; // set to 0x5f0f3cf5
    uint16_t        flag;
    uint16_t        unitsPerEm;
    LONGDATETIME    created;
    LONGDATETIME    modified;
    int16_t         xMin;
    int16_t         yMin;
    int16_t         xMax;
    int16_t         yMax;
    uint16_t        macStyle;
    uint16_t        lowestRecPPEM;
    int16_t         fontDirectionHint;
    int16_t         indexToLocFormat;
    int16_t         glyphDataFormat;
} Head;

// glyf - Glyph Data 
typedef struct {
    int16_t numberOfContours; // if >= 0, this is a simple flyph. if negative, this is a composite glyph
    int16_t xMin;
    int16_t yMin;
    int16_t xMax;
    int16_t yMax;
} GlyphHeader;

/*  
    ON_CURVE_POINT:  
        if set, point is on the　curve otherwise, off the curve.
    
    X_SHORT_VECTOR:  X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR:
        | X_SHORT_VECTOR| X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR | Explanation |
        | ---- | ---- | ---- |
        | 1 | 1 | corresponding x-coordinate is 1 byte long positive number |
        | 1 | 0 | corresponding x-coordinate is 1 byte long negative number |
        | 0 | 1 | x-coordinate is same as previous x-coordinate and no element is added to xCoordinates /
        | 0 | 0 | corresponding x-coordinate is 2 byte long  |

    Y_SHORT_VECTOR:  Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR:
        same as above
    
    REPEAT_FLAG:
        if set, the next byte indicates how many times "this" flag is to be applied repeatedly.

    RESERVED:
        set to zero.
*/

#define ON_CURVE_POINT                          0x01 
#define X_SHORT_VECTOR                          0x02 
#define Y_SHORT_VECTOR                          0x04
#define REPEAT_FLAG                             0x08
#define X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR    0x10
#define Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR    0x20
#define OVERLAP_SIMPLE                          0x40
#define RESERVED                                0x80

/*
typedef struct {
    uint16_t    endPtsOfContours[];
    uint16_t    instructionLength;
    uint8_t     instructions[instructionLength];
    uint8_t     flags[variable];
    uint8_t or int16_t xCoordinates[variable];
    uint8_t or int16_t yCoordinates[variable];
} SimpleGlyphTable; 
*/

// hhea
typedef struct {
    uint16_t    majorVersion;
    uint16_t    minorVersion;
    FWORD       ascender;
    FWORD       descender;
    FWORD       lineGap;
    UFWORD      advancedWidthMax;
    FWORD       minLeftSideBearing;
    FWORD       minRightSideBearing;
    FWORD       xMaxExtent;
    int16_t     caretSlopeRise;
    int16_t     caretSlopeRun;
    int16_t     caretOffset;
    int16_t     reserved[4];
    int16_t     metricDataFormat;
    uint16_t    numberOfHMetrics; // Number of hMetric entries in 'hmtx' table
} Hhea;

// hmtx
typedef struct{
    uint16_t    advanceWidth;
    int16_t     lsb; // Glyoh left side bearing in font design units
} longHorMetric;

// global variable
typedef struct {
    uint16_t endCode;
    uint16_t startCode;
    uint16_t idDelata;
    uint16_t idRangeOffset;
} CmapEntry;

typedef struct {
    uint16_t num_entry;
    CmapEntry *entries;
} Cmap;

Cmap gcmap;

Offset16 *gloca; // TODO: fix this

uint8_t *gglyph;

uint16_t *gadvancedWidth;

void readTableRecord(TableRecord *tr) {
    printf(
        "%.4s\t\t%#x\t%#x\t%#x\n", 
        tr->tableTag,
        be32toh(tr->checksum), 
        be32toh(tr->offset), 
        be32toh(tr->length));
}

void readEncordingRecord(EncordingRecord er) {
     printf(
        "%#x\t\t%#x\t\t%#x\n", 
        be16toh(er.platformID), 
        be16toh(er.encordingID), 
        be32toh(er.subtableOffset));
}

void readSubtableFormat4(SubtableFormat4 *sf4) {
    uint16_t segCount = be16toh(sf4->segCountX2) / 2;
    uint16_t *startCode, *idDelta, *idRangeOffsets;
    CmapEntry *entry;

    puts("\e[32m[+] Reading Unicode Encording Record...(Format 4)\e[m");

    startCode = sf4->endCode + segCount + 1;
    idDelta = startCode + segCount;
    idRangeOffsets = idDelta + segCount;

    // init global variable
    gcmap.num_entry = segCount;
    gcmap.entries = calloc(segCount, sizeof(CmapEntry));
    
    for(int i = 0; i < segCount; i++) {
        entry = &gcmap.entries[i];
        entry->endCode = be16toh(sf4->endCode[i]);
        entry->startCode = be16toh(startCode[i]);
        entry->idDelata = be16toh(idDelta[i]);
        entry->idRangeOffset = be16toh(idRangeOffsets[i]);
    }
    
    puts("\e[32m[+] Initializing gcmap...(Format 4)\e[m");
    puts("---- gcmap----");
    puts("index\tendCode\t\tstartCode\tidDelta\t\tidRangeOffset");
    for(int i = 0; i < gcmap.num_entry; i++){
        entry = &gcmap.entries[i];
        printf(
            "%d\t%#x\t\t%#x\t\t%#x\t\t%#x\n", 
            i,
            entry->endCode,
            entry->startCode,
            entry->idDelata,
            entry->idRangeOffset);
    }
}

void readSubtable(uint8_t *st) {
    uint16_t format;
    format = be16toh(*(uint16_t *)st);

    switch(format) {
        case 4:
            readSubtableFormat4((SubtableFormat4 *)st);
            break;
        default:
            puts("unrecognized format");
            exit(1);
    }
}

void readCmap(CmapHeader *cmh) {
    EncordingRecord er, unicodeER;
    puts("---- cmap ----");
    puts("platformID\tencordingID\tsubtableOffset");
    for(int i = 0; i < be16toh(cmh->numTables); i++){
        er = cmh->encordingRecords[i];
        if(be16toh(er.platformID) == 0) unicodeER = er;
        readEncordingRecord(er);
    }

    // read subtable of Unicode Encording Record
    readSubtable((uint8_t *)cmh + be32toh(unicodeER.subtableOffset));
}

const char * tell(uint8_t flag, uint8_t c) {
    if(flag & (X_SHORT_VECTOR << (c & 1))){
        if(flag & (X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR << (c & 1)))
            return "+1b";
        else 
            return "-1b";
    }else if(!(flag & (X_SHORT_VECTOR << (c & 1))) && !(flag & (X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR << (c & 1))))
        return "2b";
    else 
        return "same";
}

void readGlyph(uint16_t glyphId) {
    // indexTolocFormatが0の場合、glyphの相対位置を2で割った値が入る。
    GlyphHeader *gh = (GlyphHeader *)(gglyph + gloca[glyphId] * 2);
    int16_t numberOfContours = be16toh(gh->numberOfContours);
    
    puts("---- GlyphHeader ----");
    puts("numberOfContours\txMin\tyMin\txMax\tyMax");
    printf(
        "%#x\t\t\t%#x\t%#x\t%#x\t%#x\n",
        numberOfContours,
        be16toh(gh->xMin),
        be16toh(gh->yMin),
        be16toh(gh->xMax),
        be16toh(gh->yMax));

    assert(numberOfContours >= 0); // error for now
    
     puts("---- SimpleGlyfTable ----");
    uint8_t *simpleGlyfTable = (uint8_t *)gh + sizeof(GlyphHeader);
    uint16_t *endPtsOfContours = (uint16_t *)simpleGlyfTable;
    uint16_t instructionLength = be16toh(*(uint16_t *)(simpleGlyfTable + sizeof(uint16_t) * (numberOfContours)));
    uint8_t *flags = simpleGlyfTable + sizeof(uint16_t) * (numberOfContours + 1) + instructionLength;

    // The last entry of endPtsOfContours represents the number of points
    uint16_t numPoints = be16toh(endPtsOfContours[numberOfContours - 1]) + 1;
    
    // count flag number and calculate XCoordinateSize
    uint8_t flag;
    uint16_t numFlags = numPoints, XCoordinatesSize = 0;
    int fl = 0, repeat = 0;
    for(int i = 0; i < numPoints; i++) {
        flag = 0 < repeat-- ? flag : flags[fl++];
        if(flag & X_SHORT_VECTOR) XCoordinatesSize++;
        else if(!(flag & X_SHORT_VECTOR) && !(flag & X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR)) XCoordinatesSize += 2;
            
        if(flag & REPEAT_FLAG){
            numFlags -= (repeat = flags[fl++]) - 1;
            flag ^= REPEAT_FLAG; // disable REPEAT_FLAG
        }
    }

    puts("numPoints\tnumFlags\tinstructionLength\tXCoordinateSize");
    printf(
        "%#x\t\t%#x\t\t%#x\t\t\t%#x\n", 
        numPoints, 
        numFlags, 
        instructionLength,
        XCoordinatesSize);
    puts("flags:");
    puts("index\tflag\tOnCURVE\tREPEAT\tx\ty");
    for(int i = 0; i < numFlags; i++) {
        flag = flags[i];
        printf(
            "%d\t%#x\t%d\t%d\t(%d, %d)\t(%d, %d)\n",
            i,
            flag,
            flag & ON_CURVE_POINT,
            (flag & REPEAT_FLAG) != 0,
            (flag & X_SHORT_VECTOR) != 0,
            (flag & X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR) != 0,
            (flag & Y_SHORT_VECTOR) != 0,
            (flag & Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR) != 0);
    }
    
    uint8_t *xCoordinates = flags + numFlags;
    uint8_t *yCoordinates = xCoordinates + XCoordinatesSize;

    flag = 0;
    int16_t xcoordinate16 = 0, ycoordinate16 = 0; // abcolute coordinates
    int16_t xabs = 0, yabs = 0;
    printf("\e[32m[+] Reading GlyphData...(GID: %#x)\n\e[m", glyphId);
    puts("---- Glyph Data ----");
    puts("idx\tflag\tOnCurve\tx\ty\tδx\tδy\tAbsoluteCoordinate");
    int x = 0, y = 0;
    repeat = 0; fl = 0;
    for(int i = 0; i < numPoints; i++) {
        flag = 0 < repeat-- ? flag : flags[fl++];

        printf(
            "%d\t%#x\t%d\t%s\t%s\t", 
            i, 
            flag, 
            (flag & ON_CURVE_POINT) != 0,
            tell(flag, 'x'),
            tell(flag, 'y'));
        if(flag & X_SHORT_VECTOR){
            // prevent overflow
            if(flag & X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR)
                xcoordinate16 = xCoordinates[x++];
            else
                xcoordinate16 = -xCoordinates[x++];
        }
        else if(!(flag & X_SHORT_VECTOR) & !(flag & X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR)){
            xcoordinate16 = (int16_t)be16toh(*(uint16_t *)&xCoordinates[x]);
            x += 2;
        } else
            xcoordinate16 = 0;
        
        printf("%+d\t", xcoordinate16);
        xabs += xcoordinate16;

        if(flag & Y_SHORT_VECTOR){
            // prevent overflow
            if(flag & Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR)
                ycoordinate16 = yCoordinates[y++];
            else 
                ycoordinate16 = -yCoordinates[y++];
        }
        else if(!(flag & Y_SHORT_VECTOR) & !(flag & Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR)){
            ycoordinate16 = (int16_t)be16toh(*(uint16_t *)&yCoordinates[y]);
            y += 2;
        } else
            ycoordinate16 = 0;

        printf("%+d\t", ycoordinate16);
        yabs += ycoordinate16;
        
        if(flag & REPEAT_FLAG){
            numFlags -= (repeat = flags[fl++]) - 1;
            flag ^= REPEAT_FLAG; // disable REPEAT_FLAG
        }

        printf("(%+d, %+d)\n", xabs, yabs);
    }
    printf(
        "advencedWidth: %#x (%d)\n", 
        gadvancedWidth[glyphId],
        gadvancedWidth[glyphId]);
    /*
    typedef struct {
        uint16_t    endPtsOfContours[];
        uint16_t    instructionLength
        uint8_t     instructions[instructionLength]
        uint8_t     flags[variable]
        uint8_t or int16_t xCoordinates[variable]
        uint8_t or int16_t yCoordinates[variable]
    } SimpleGlyphTable; 
    */
}

// TODO : check algo works for now
int16_t uint2int(uint16_t i) {
    return (i >> 15) ? i - 0x10000 : i;
}

uint16_t getGlyphId(uint16_t c) {
    int i;
    uint16_t glyphId;
    int16_t delata;

    printf("\e[32m[+] Calculating GlyphId of %#x\n\e[m", c);
    
    for(i = 0; i< gcmap.num_entry; i++)
        if(c <= gcmap.entries[i].endCode) break;

    if(c < gcmap.entries[i].startCode) {
        printf("\e[31m[Error] Mapping not found for \'%c\' (%#x)\n\e[m", c, c);
        return 0;
    }
    if(!(gcmap.entries[i].idRangeOffset)){
        // TODO: fix this
        delata = uint2int(gcmap.entries[i].idDelata);
        glyphId = c + delata;
    }else 
       exit(1); // error for now
    
    printf("\e[32m[+] ----> GlyphId of %#x is %#x\n\e[m", c, glyphId);

    return glyphId;
}

void readotf(uint8_t *base) {
    TableDirectory *td = (TableDirectory *)base;
    TableRecord *tr, *cmap, *loca, *maxp, *head, *glyf, *hhea, *hmtx;

    uint16_t n = be16toh(td->numTables);
    puts("---- Table Directory ----");
    printf("sfntVersion: %#x\n", be32toh(td->sfntVersion));
    printf("numTables: %#x\n", n);
    printf("searchRange: %#x\n", be16toh(td->searchRange));
    printf("entrySelector: %#x\n", be16toh(td->entrySelector));
    printf("rangeShift: %#x\n", be16toh(td->rangeShift));

    puts("---- Table Records ----");
    puts("tableTag\tchecksum\toffset\tlength");
    for(int i = 0; i < n; i++){
        tr = (TableRecord *)(base + sizeof(TableDirectory) + sizeof(TableRecord) * i);
        if(!strncmp(tr->tableTag, "cmap", 4)) cmap = tr;
        else if(!strncmp(tr->tableTag, "loca", 4)) loca = tr;
        else if(!strncmp(tr->tableTag, "maxp", 4)) maxp = tr;
        else if(!strncmp(tr->tableTag, "head", 4)) head = tr;
        else if(!strncmp(tr->tableTag, "glyf", 4)) glyf = tr;
        else if(!strncmp(tr->tableTag, "hhea", 4)) hhea = tr;
        else if(!strncmp(tr->tableTag, "hmtx", 4)) hmtx = tr;
        readTableRecord(tr);
    }

    // read cmap
    readCmap((CmapHeader *)(base + be32toh(cmap->offset)));

    // TODO: 関数に切り出す
    // read maxp 
    puts("---- maxp ----");
    uint16_t numGlyphs = be16toh(((Maxp *)(base + be32toh(maxp->offset)))->numGlyphs);
    printf("numGlyps: %#x\n", numGlyphs);

    // read head
    puts("---- head ----");
    int16_t indexToLocFormat = be16toh(((Head *)(base + be32toh(head->offset)))->indexToLocFormat);
    printf("indexTolocFormat: %#x\n", indexToLocFormat); // 0 for short

    // read hhea
    puts("---- hhea ----");
    uint16_t numberOfHMetrics = be16toh(((Hhea *)(base + be32toh(hhea->offset)))->numberOfHMetrics);
    printf("numberOfHMetrics: %#x\n", numberOfHMetrics);

    // read hmtx table and init gadvancedWidth
    puts("\e[32m[+] Reading hmtx table...\e[m");
    puts("\e[32m[+] Initializing gadvancedWidth...\e[m");
    gadvancedWidth = calloc(numberOfHMetrics, sizeof(uint16_t));
    longHorMetric *p = (longHorMetric *)(base + be32toh(hmtx->offset));
    puts("index\tadbanceWidth\tlsb");
    for(int i = 0; i < numberOfHMetrics; i++) {
        gadvancedWidth[i] = be16toh(p[i].advanceWidth);
        printf(
            "%d\t%#x\t\t%#x\n",
            i,
            gadvancedWidth[i],
            be16toh(p[i].lsb));
    }
    printf("leftSideBearings has %d entries\n", numGlyphs - numberOfHMetrics);

    // read loca table and init gloca
    puts("\e[32m[+] Reading loca table...\e[m");
    puts("\e[32m[+] Initializing gloca...\e[m");
    gloca = calloc(numGlyphs + 1, sizeof(Offset16));
    Offset16 *offset = (uint16_t *)(base + be32toh(loca->offset));
    for(int i = 0; i < numGlyphs + 1; i++)
        gloca[i] = be16toh(offset[i]);

    puts("---- gloca ----");
    printf("\t\e[36m0\t1\t2\t3\t4\t5\t6\t7\t8\t9\e[m");
    for(int i = 0; i < numGlyphs + 1; i++){
        if(i % 10 == 0) printf("\e[36m\n%d\t\e[m", i);
        printf("%#x\t", gloca[i]);
    }
    putchar('\n');

    // read glyf 
    puts("\e[32m[+] Initializing gglyph...\e[m");
    gglyph = base + be32toh(glyf->offset);
}   

#define FILE_NAME_NAX_LEN 100

int main(int argc, char *argv[]) {
    FILE *fp;
    long fsize;
    uint8_t *file_buf;

    if(argc == 1) {
        puts("Usage: ./a.out [ttf format file] [char(OPTIONAL)]");
        return 1;
    }

    fp = fopen(argv[1], "rb");
    
    if(fp == NULL) {
        perror("fopen");
        return 1;
    }

    fseek(fp, 0, SEEK_END);
    fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    file_buf = calloc(1, fsize);

    fread(file_buf, 1, fsize, fp);

    // read info
    readotf(file_buf);
    
    if(argv[2]){
        // calculate Glyph ID
        // TODO: fix here
        uint16_t glyphId = getGlyphId(argv[2][0]);
        // read Glyph
        readGlyph(glyphId);
    }

    fclose(fp);

    // TODO: 描画出来るようにする
    return 0;
}