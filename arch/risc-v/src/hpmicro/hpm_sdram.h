/****************************************************************************
 * arch/risc-v/src/hpmicro/hpm_sdram.h
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

#ifndef __ARCH_RISCV_SRC_HPMICRO_HPM_SDRAM_H
#define __ARCH_RISCV_SRC_HPMICRO_HPM_SDRAM_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdint.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Window the external SDRAM answers on.  Dead until the FEMC has been
 * programmed - touching it earlier hangs the AXI bus.
 */

#ifndef HPM_SDRAM_BASE
#  define HPM_SDRAM_BASE (0x40000000UL)
#endif

#ifndef HPM_SDRAM_SIZE
#  define HPM_SDRAM_SIZE (32UL * 1024UL * 1024UL)
#endif

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

int hpm_sdram_preinit(void);
int hpm_sdram_size(void);

#ifdef __cplusplus
}
#endif

#endif /* __ARCH_RISCV_SRC_HPMICRO_HPM_SDRAM_H */