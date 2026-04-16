# Copyright (c) 2023, ithinx GmbH
# Copyright (c) 2023, Tonies GmbH
# Copyright (c) 2026, Texas Instruments Incorporated
#
# SPDX-License-Identifier: Apache-2.0

zephyr_library_sources_ifdef(CONFIG_BQ27Z8XX_COMMON bq27z8xx_common.c)
zephyr_library_sources_ifdef(CONFIG_BQ27Z758        bq27z758.c)
zephyr_library_sources_ifdef(CONFIG_BQ27Z855        bq27z855.c)

if(CONFIG_BQ27Z758 OR CONFIG_BQ27Z855)
    zephyr_library_sources(bq27z8xx.c)
endif()

if(CONFIG_EMUL_BQ27Z8XX OR CONFIG_EMUL_BQ27Z758)
    zephyr_library_sources(emul_bq27z8xx.c)
endif()

if(CONFIG_BQ27Z8XX_SHELL OR CONFIG_BQ27Z758_SHELL)
    zephyr_library_sources(bq27z8xx_shell.c)
endif()
