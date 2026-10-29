/****************************************************************************
 * arch/risc-v/src/hpmicro/hpm_sdram.c
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

/****************************************************************************
 * External SDRAM bring-up for the HPM6E00EVK.
 *
 * The 32 MB part sits on the optional expansion board plugged into the
 * PPI/FEMC socket.  The FEMC window is dead until the controller is
 * programmed - a single access before that hangs the AXI bus - so this runs
 * from the earliest C code in the boot path (__hpm_start, before the clock
 * and the RTOS come up), which is where the vendor startup calls
 * _init_ext_ram() from start.S.
 *
 * The SDK board layer already knows the pads, the controller clock and the
 * timing/footprint constants of the part, so this is a thin wrapper around
 * femc_init()/femc_config_sdram() rather than a private calibration.
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdint.h>
#include <errno.h>

#include <nuttx/arch.h>

#include "hpm_soc.h"
#include "hpm_clock_drv.h"
#include "hpm_femc_drv.h"

#include "hpm_sdram.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Supplied by the SDK board layer (board.c / pinmux.c). */

extern void board_init_sdram_pins(void);
extern uint32_t board_init_femc_clock(void);

/****************************************************************************
 * Private Data
 ****************************************************************************/

static int g_sdram_size = -ENODEV;

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: hpm_sdram_preinit
 *
 * Description:
 *   Program the FEMC and start the external SDRAM.  Called from __hpm_start()
 *   before anything can touch the FEMC window.
 *
 * Returned Value:
 *   Size of the SDRAM in bytes, or a negative errno.
 *
 ****************************************************************************/

int hpm_sdram_preinit(void)
{
  femc_config_t config = {0};
  femc_sdram_config_t sdram = {0};
  uint32_t clk;

  g_sdram_size = -ENODEV;

  board_init_sdram_pins();
  clk = board_init_femc_clock();

  femc_default_config(HPM_FEMC, &config);
  femc_init(HPM_FEMC, &config);
  femc_get_typical_sdram_config(HPM_FEMC, &sdram);

  /* Parameters of the part on the expansion board, as in the SDK board code
   * (board.c: _init_ext_ram()).
   */

  sdram.bank_num = FEMC_SDRAM_BANK_NUM_4;
  sdram.prescaler = 0x3;
  sdram.burst_len_in_byte = 8;
  sdram.auto_refresh_count_in_one_burst = 1;
  sdram.col_addr_bits = FEMC_SDRAM_COLUMN_ADDR_9_BITS;
  sdram.cas_latency = FEMC_SDRAM_CAS_LATENCY_3;
  sdram.refresh_to_refresh_in_ns = 60;     /* Trc  */
  sdram.refresh_recover_in_ns = 60;        /* Trc  */
  sdram.act_to_precharge_in_ns = 42;       /* Tras */
  sdram.act_to_rw_in_ns = 18;              /* Trcd */
  sdram.precharge_to_act_in_ns = 18;       /* Trp  */
  sdram.act_to_act_in_ns = 12;             /* Trrd */
  sdram.write_recover_in_ns = 12;          /* Twr  */
  sdram.self_refresh_recover_in_ns = 72;   /* Txsr */
  sdram.cs = FEMC_SDRAM_CS0;
  sdram.base_address = HPM_SDRAM_BASE;
  sdram.size_in_byte = HPM_SDRAM_SIZE;
  sdram.port_size = FEMC_SDRAM_PORT_SIZE_16_BITS;
  sdram.refresh_count = 8192;
  sdram.refresh_in_ms = 64;
  sdram.delay_cell_disable = true;
  sdram.delay_cell_value = 0;

  if (femc_config_sdram(HPM_FEMC, clk, &sdram) != status_success)
    {
      return g_sdram_size;
    }

  /* 16-bit part: the data sits in the upper half of the 32-bit port. */

  HPM_FEMC->SDRCTRL0 |= FEMC_SDRCTRL0_HIGHBAND_MASK;

  g_sdram_size = HPM_SDRAM_SIZE;
  return g_sdram_size;
}

/****************************************************************************
 * Name: hpm_sdram_size
 *
 * Description:
 *   Size reported by hpm_sdram_preinit(), or a negative errno.
 *
 ****************************************************************************/

int hpm_sdram_size(void)
{
  return g_sdram_size;
}