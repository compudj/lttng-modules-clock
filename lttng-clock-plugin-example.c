/*
 * lttng-clock-plugin-example.c
 *
 * Copyright (C) 2014 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; only
 * version 2.1 of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <linux/module.h>
#include <linux/ktime.h>
#include <linux/hrtimer.h>
#include <linux/time.h>

#include <lttng-clock.h>	/* From lttng-modules */

/*
 * For sake of example, transform time into a coarse clock (freq: 1KHz).
 * Note that division can be slow on some architectures. Should be
 * avoided. Use shift and multiplication instead. e.g.:
 *
 * Operation: / 1000000ULL
 * 1/1000000ULL = .000001
 * 2^19 < 1000000 < 2^20
 * Add a 10 bits shift to increase accuracy:
 * 2^(19+10) = 536870912
 * x * 1 / 2^(19+10) ~= .000001
 * 537 * 1 / 2^29 = .00000100024044513702
 * 537 (multiplication factor) is between 2^9 and 2^10.
 *
 * In order not to overflow, first right shift by 10, multiply, and right
 * shift by 19.
 */
#define DIV_CLOCK_SHIFT1	10
#define DIV_CLOCK_MUL		537
#define DIV_CLOCK_SHIFT2	19

/*
 * Read your own clock source here. Note that this operation needs to be
 * reentrant for NMI (no mutual exclusion locking!).
 */
static u64 trace_clock_read64_example(void)
{
	ktime_t ktime;

	/*
	 * ktime_get() in this example is based on a seqlock, which is
	 * not nmi-reentrant. Therefore, we return -EIO when called from
	 * NMI context.
	 */
	if (in_nmi())
		return (u64) -EIO;

	ktime = ktime_get();
	return (DIV_CLOCK_MUL * ((u64) ktime_to_ns(ktime) >> DIV_CLOCK_SHIFT1))
			>> DIV_CLOCK_SHIFT2;
}

static u64 trace_clock_freq_example(void)
{
	return 1000;	/* 1KHz */
}

static int trace_clock_uuid_example(char *uuid)
{
	const char myuuid[] = "123456789012345678901234567890123456";

	/*
	 * Should read some unique identifier for this clock shared
	 * across all components of the system using this clock for
	 * tracing.
	 */
	memcpy(uuid, myuuid, LTTNG_MODULES_UUID_STR_LEN);
	return 0;
}

static const char *trace_clock_name_example(void)
{
	return "my_example_clock";
}

static const char *trace_clock_description_example(void)
{
	return "Coarse monotonic clock at 1KHz";
}

static
struct lttng_trace_clock ltc = {
	.read64 = trace_clock_read64_example,
	.freq = trace_clock_freq_example,
	.uuid = trace_clock_uuid_example,
	.name = trace_clock_name_example,
	.description = trace_clock_description_example,
};

static __init
int lttng_clock_plugin_init(void)
{
	return lttng_clock_register_plugin(&ltc, THIS_MODULE);
}
module_init(lttng_clock_plugin_init);

static __exit
void lttng_clock_plugin_exit(void)
{
	lttng_clock_unregister_plugin(&ltc, THIS_MODULE);
}
module_exit(lttng_clock_plugin_exit);

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers <mathieu.desnoyers@efficios.com>");
MODULE_DESCRIPTION("LTTng Clock Plugin Example");
