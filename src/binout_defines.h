#ifndef BINOUT_DEFINES_H
#define BINOUT_DEFINES_H

#define BINOUT_HEADER_BIG_ENDIAN 0
#define BINOUT_HEADER_LITTLE_ENDIAN 1
#define BINOUT_HEADER_FLOAT_IEEE 0

#define BINOUT_COMMAND_NULL 1
#define BINOUT_COMMAND_CD 2
#define BINOUT_COMMAND_DATA 3
#define BINOUT_COMMAND_VARIABLE 4
#define BINOUT_COMMAND_BEGINSYMBOLTABLE 5
#define BINOUT_COMMAND_ENDSYMBOLTABLE 6
#define BINOUT_COMMAND_SYMBOLTABLEOFFSET 7

#define BINOUT_TYPE_INT8 1
#define BINOUT_TYPE_INT16 2
#define BINOUT_TYPE_INT32 3
#define BINOUT_TYPE_INT64 4
#define BINOUT_TYPE_UINT8 5
#define BINOUT_TYPE_UINT16 6
#define BINOUT_TYPE_UINT32 7
#define BINOUT_TYPE_UINT64 8
#define BINOUT_TYPE_FLOAT32 9
#define BINOUT_TYPE_FLOAT64 10

#define BINOUT_DATA_NAME_LENGTH 1

#endif