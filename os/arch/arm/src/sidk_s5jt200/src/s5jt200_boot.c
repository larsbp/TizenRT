/****************************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the License.
 *
 ****************************************************************************/
/****************************************************************************
 * arch/arm/src/sidk_s5jt200/src/s5jt200_boot.c
 *
 *   Copyright (C) 2009, 2011, 2013, 2015 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *           Laurent Latil <laurent@latil.nom.fr>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <tinyara/config.h>
#include <tinyara/spi/spi.h>
#ifdef CONFIG_MTD_PARTITION
#include <tinyara/fs/mtd.h>
#include <tinyara/fs/ioctl.h>
#endif
#include <debug.h>
#include <assert.h>

#include <tinyara/board.h>
#include <arch/board/board.h>

#include <sys/mount.h>
#ifdef CONFIG_SCSC_WLAN
#include <net/lwip/opt.h>
#include <net/lwip/netif.h>
#include <net/lwip/tcpip.h>
#endif

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Public Functions Prototypes
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: up_flashinitialize
 *
 * Description:
 *   Create an initialized MTD device instance for internal flash.
 *   Initialize MTD master & each of partitions here.
 *   Each of partition classfied according to TAG number
 *
 ****************************************************************************/
FAR struct mtd_dev_s *up_flashinitialize(void)
{
	return NULL;
}

#ifdef CONFIG_SCSC_WLAN
err_t wlan_init(struct netif *netif);

struct netif *wlan_netif;
void wlan_initup(struct netif *dev)
{
	struct ip_addr ipaddr;
	struct ip_addr netmask;
	struct ip_addr gw;

	/* Start LWIP network thread */
	ipaddr.addr = inet_addr("0.0.0.0");
	netmask.addr = inet_addr("255.255.255.255");
	gw.addr = inet_addr("0.0.0.0");

	netif_set_default(dev);

	wlan_netif = netif_add(dev, &ipaddr, &netmask, &gw,
					NULL, wlan_init, tcpip_input);
	wlan_netif->flags |= NETIF_FLAG_IGMP;
}
#endif

/****************************************************************************
 * Name: s5j_board_initialize
 *
 * Description:
 *   All S5J architectures must provide the following entry point. This entry
 *   point is called early in the initialization -- after all memory has been
 *   configured and mapped but before any devices have been initialized.
 *
 *   This function must perform low level initialization including:
 *
 *   - Initialization of board-specific memory resources (e.g., SDRAM)
 *   - Configuration of board-specific resources (GPIOs, LEDs, etc).
 *   - Setup of the console UART. This UART done early so that the serial
 *     console is available for debugging very early in the boot sequence.
 *
 *   Special precautions must be taken if .data/.bss lie in SRAM. In that
 *   case, the boot logic cannot initialize .data or .bss. The function
 *   must then:
 *
 *   - Take precautions to assume that logic does not access any global
 *     data that might lie in SDRAM.
 *   - Call the function arm_data_initialize() as soon as SDRAM has been
 *     properly configured for use.
 *
 ****************************************************************************/
void s5j_board_initialize(void)
{
#ifdef CONFIG_S5J_SFLASH
	s5j_sflash_init();
#endif
}

/****************************************************************************
 * Name: board_initialize
 *
 * Description:
 *   If CONFIG_BOARD_INITIALIZE is selected, then an additional
 *   initialization call will be performed in the boot-up sequence to a
 *   function called board_initialize(). board_initialize() will be
 *   called immediately after up_intiialize() is called and just before the
 *   initial application is started.  This additional initialization phase
 *   may be used, for example, to initialize board-specific device drivers.
 *
 ****************************************************************************/

#ifdef CONFIG_BOARD_INITIALIZE
void board_initialize(void)
{
	/* Perform app-specific initialization here instead of from the TASH. */
	board_app_initialize();

#ifdef CONFIG_SCSC_WLAN
	slldbg("Initialize SCSC driver\n");
	slsi_driver_initialize();
#endif
}
#endif /* CONFIG_BOARD_INITIALIZE */
