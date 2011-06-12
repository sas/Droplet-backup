/*
**
** Copyright (c) 2011, Stephane Sezer
** All rights reserved.
** 
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**     * Neither the name of Stephane Sezer nor the names of its contributors
**       may be used to endorse or promote products derived from this software
**       without specific prior written permission.
** 
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL Stephane Sezer BE LIABLE FOR ANY DIRECT,
** INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
** (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
** LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
** ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
*/

#include <stdint.h>
#include <string.h>

#include <utils/logger.h>

#include "stats.h"

static struct
{
  uint64_t rx_bytes;
  uint64_t tx_bytes;
  uint64_t transactions;
} stats;

void stats_init(void)
{
  memset(&stats, 0, sizeof (stats));
}

void stats_log_rx_bytes(unsigned int count)
{
  stats.rx_bytes += count;
}

void stats_log_tx_bytes(unsigned int count)
{
  stats.tx_bytes += count;
}

void stats_log_transaction(void)
{
  stats.transactions += 1;
}

void stats_print(void)
{
  logger(LOG_INFO, "          RX bytes: %llu", stats.rx_bytes);
  logger(LOG_INFO, "          TX bytes: %llu", stats.tx_bytes);
  logger(LOG_INFO, "Total transactions: %llu", stats.transactions);
}
