/*
 * helpers.h
 *
 *  Created on: Feb 22, 2021
 *      Author: Ian Boll
 */

#ifndef HELPERS_H_
#define HELPERS_H_

/**
 * Usage: sizeof_member(ComputeMessage_t, some_field_name)
 *
 * This returns the size of a particular member of a struct.
 * This is especially useful when that member is a character
 * array, e.g. `char some_string_member[255]`.
 */
#define sizeof_member(type, member) sizeof(((type){0}).member)

#endif /* HELPERS_H_ */
