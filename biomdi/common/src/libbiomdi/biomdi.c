/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <biomdi.h>
#include <biomdimacro.h>

int
inIntSet(biomdiIntSet S, uint32_t val)
{
	int i;

	if (S.is_size > MAX_INT_SET_SIZE)
		ERR_EXIT("Set size exceeds maximum");
	for (i = 0; i < S.is_size; i++)
		if (S.is_values[i] == val)
			return (1);
	return (0);
}
