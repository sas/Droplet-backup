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

#ifndef ROLLSUM_H_
# define ROLLSUM_H_

# define ROLLSUM_CHECKBITS  21
# define ROLLSUM_CHECKMASK  ((1 << ROLLSUM_CHECKBITS) - 1)
# define ROLLSUM_BOUNDVAL   0x42
# define ROLLSUM_MINSIZE    (1024 * 1024 / 2)
# define ROLLSUM_MAXSIZE    (1024 * 1024 * 5)

struct rollsum
{
  unsigned long  count;
  unsigned short a;
  unsigned short b;
};

static inline void rollsum_init(struct rollsum *srp)
{
  srp->count = 0;
  srp->a = 0;
  srp->b = 0;
}

static inline unsigned int rollsum_hash(struct rollsum *srp)
{
  return srp->a << 16 | srp->b;
}

static inline int rollsum_onbound(struct rollsum *srp)
{
  return srp->count >= ROLLSUM_MAXSIZE || (
      srp->count > ROLLSUM_MINSIZE &&
      (rollsum_hash(srp) & ROLLSUM_CHECKMASK) == ROLLSUM_BOUNDVAL
      );
}

static inline void rollsum_roll(struct rollsum *srp, unsigned char data)
{
  srp->count += 1;
  srp->a += data;
  srp->b += srp->a;
}

#endif /* !ROLLSUM_H_ */
