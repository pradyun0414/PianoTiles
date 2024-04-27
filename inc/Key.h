/**
 * @file      Key.h
 * @brief     4-bit keyboard connected to PB19-PB16
 * @details   Lab 5. Digital Piano using a Digital to Analog Converter<br>
 * For more information see<br> https://docs.google.com/document/d/1DVmHjqsx24FvLp4x8KKyr5UmDeDlBlfeQPsVvKFQF_k/edit<br>
 * @version   ECE319K Spring 2024
 * @author    Your names go here
 * @copyright Lab solution, do not post,
 * @warning   AS-IS
 * @date      October 2, 2023
  <table>
<caption id="Lab5inputpins">Pins on the 4-bit Keyboard</caption>
<tr><th>Pin <th>GPIO<th>Hardware
<tr><td>PB19 <td>input<td>bit 3 (note with highest frequency)
<tr><td>PB18 <td>input<td>bit 2
<tr><td>PB17 <td>input<td>bit 1
<tr><td>PB16 <td>input<td>bit 0 (note with lowest frequency)
</table>

  ******************************************************************************/
#ifndef __KEY_H__
#define __KEY_H__

/**
 * Initialize 4-bit keyboard
 *    - Piano key 3 PB19 (note with highest frequency)
 *    - Piano key 2 PB18
 *    - Piano key 1 PB17
 *    - Piano key 0 PB16 (note with lowest frequency)
 * @param none
 * @return none
 * @brief  Initialize keyboard
 * @note LaunchPad_Init has been called; this program should not reset Port B
 */
void Key_Init(void);

/**
 * Input from piano key inputs on PB19-PB16
 * @param none
 * @return 0 to 15 depending on keys
 * 0x01 is just Key0, 0x02 is just Key1, 0x04 is just Key2, 0x08 is just Key3
 * */
uint32_t Key_In(void);

#endif // __KEY_H__
