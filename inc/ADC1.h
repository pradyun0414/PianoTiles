/* ADC1.h
 * Students put your names here
 * Modified: put the date here
 * 12-bit ADC input on ADC1 channel 5, PB18
 */

/**
 * @file      ADC1.h
 * @brief     12-bit ADC input
 * @details   12-bit ADC input on ADC1 channel 5, PB18<br>
 * @version   ECE319K Spring 2024
 * @author    Your names go here
 * @copyright Lab solution, do not post,
 * @warning   AS-IS
 * @note      For more information see  http://users.ece.utexas.edu/~valvano/
 * @date      December 29, 2023
  <table>
<caption id="Lab7pins">Pins for the 12-bit ADC</caption>
<tr><th>Pin <th>GPIO<th>Hardware
<tr><td>PB18 <td>input<td>connected to slide-pot sensor
</table>
  ******************************************************************************/
#ifndef __ADC1_H__
#define __ADC1_H__

/**
 * Initialize 12-bit ADC
 *    - PB18 analog input
 * @param none
 * @return none
 * @brief  Initialize ADC
 * @note LaunchPad_Init has been called; this program should not reset Port B
 */
void ADCinit(void);
// write code to initialize ADC1 channel 5, PB18
// Your measurement will be connected to PB18
// 12-bit mode, 0 to 3.3V, right justified
// software trigger, no averaging

/**
 * Input from 12-bit ADC
 * @param none
 * @return sample 12-bit conversion
 * @brief  Input from PB18
 */
uint32_t ADCin(void);
  // write code to sample ADC1 channel 5, PB18 once
  // return digital result (0 to 4095)


/**
 * Convert raw 12-bit ADC to fixed point distance
 * @param input 12-bit ADC raw sample
 * @return distance as 0.001cm fixed point number
 * @brief  Input from PB18
 */
uint32_t Convert(uint32_t input);
//    determine constants k1 k2 to fit Position=(k1*Data + k2)>>12


/**
 * Convert raw 12-bit ADC to distance
 * @param input 12-bit ADC raw sample
 * @return distance as a floating-point number in cm
 * @brief  Input from PB18
 */
float FloatConvert(uint32_t input);
// do not use this function
// it is added just to show you how SLOW floating point in on a Cortex M0+

#endif // __ADC1_H__
