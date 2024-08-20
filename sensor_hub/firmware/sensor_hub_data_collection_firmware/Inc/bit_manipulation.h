/*
 * bit_manipulation.h
 *
 *  Created on: Aug 20, 2024
 *      Author: Fongberg
 */

#ifndef BIT_MANIPULATION_H_
#define BIT_MANIPULATION_H_

#define BIT(n)                            (1LLU << n)
#define MASK(size, offset)                ((BIT(size) - 1) << offset)
#define SET_FIELD(var, size, offset, val) (var = (var & ~MASK(size, offset)) | \
                                                 ((val & MASK(size, 0)) << offset))

#endif /* BIT_MANIPULATION_H_ */
