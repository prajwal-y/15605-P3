/** @file asm.h
 *
 *  Contains the asm function declarations
 *
 */

#ifndef __ASM_H
#define __ASM_H

#include<stdint.h>

/** @brief Function to set the %cs register
 *
 *  @param val Value of the %cs register
 *
 *  @return void 
 */
void set_cs(uint16_t val);

/** @brief Function to set the %ss register
 *
 *  @param val Value of the %ss register
 *
 *  @return void 
 */
void set_ss(uint16_t val);

/** @brief Function to set the %ds register
 *
 *  @param val Value of the %ds register
 *
 *  @return void 
 */
void set_ds(uint16_t val);

/** @brief Function to set the %es register
 *
 *  @param val Value of the %es register
 *
 *  @return void 
 */
void set_es(uint16_t val);

/** @brief Function to set the %fs register
 *
 *  @param val Value of the %fs register
 *
 *  @return void 
 */
void set_fs(uint16_t val);

/** @brief Function to set the %gs register
 *
 *  @param val Value of the %gs register
 *
 *  @return void 
 */
void set_gs(uint16_t val);

#endif
