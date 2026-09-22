/* SPDX-License-Identifier: Apache-2.0
 * Minimal Platform_Types.h for TI AM13 Cortex-M33 Zephyr builds.
 * Provides AUTOSAR-style boolean and TRUE/FALSE macros.
 */

#ifndef PLATFORM_TYPES_H
#define PLATFORM_TYPES_H

#include <stdint.h>
#include <stdbool.h>

typedef unsigned char boolean;

#ifndef TRUE
#define TRUE  ((boolean)1U)
#endif

#ifndef FALSE
#define FALSE ((boolean)0U)
#endif

#endif /* PLATFORM_TYPES_H */
