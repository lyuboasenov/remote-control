#pragma once

#include <Arduino.h>

#define HEADER_FLAGS_BIT_TYPE_1 1
#define HEADER_FLAGS_BIT_TYPE_2 2
#define HEADER_FLAGS_BIT_TYPE_3 3
#define HEADER_FLAGS_BIT_TYPE_4 4

#define HEADER_FLAGS_BIT_MASK_TYPE              (bit(HEADER_FLAGS_BIT_TYPE_1) | bit(HEADER_FLAGS_BIT_TYPE_2) | bit(HEADER_FLAGS_BIT_TYPE_3) | bit(HEADER_FLAGS_BIT_TYPE_4))
#define HEADER_FLAGS_TYPE(flags)                (((flags) & (HEADER_FLAGS_BIT_MASK_TYPE)) >> 1)

#define HEADER_FLAGS_TYPE_IS_UINT8(flags)       (HEADER_FLAGS_TYPE(flags) == 0)
#define HEADER_FLAGS_TYPE_IS_CHANNELS(flags)    (HEADER_FLAGS_TYPE(flags) == 1)
#define HEADER_FLAGS_TYPE_IS_IMG(flags)         (HEADER_FLAGS_TYPE(flags) == 2)

typedef struct {
   /// @brief
   /// [   |   |   |TYPE|TYPE|TYPE|TYPE|MULTI_PART]
   /// MULTI_PART == 0x00 - Single part packet
   /// MULTI_PART == 0x01 - Multi part packet
   /// TYPE       == 0x00 - uint8_t*
   /// TYPE       == 0x01 - uint16_t*
   uint8_t flags;
} header_flags_t;

typedef struct {
   uint8_t size;
} single_part_header_t;

typedef struct {
   uint8_t index;
   uint8_t total;
   uint8_t size;
} multi_part_header_t;


typedef struct {
   uint8_t index;
   uint8_t total;
   uint8_t size;
   /// @brief
   /// [   |   |   |TYPE|TYPE|TYPE|TYPE|MULTI_PART]
   /// MULTI_PART == 0x00 - Single part packet
   /// MULTI_PART == 0x01 - Multi part packet
   /// TYPE       == 0x00 - uint8_t*
   uint8_t flags;
} header_t;