/**
  ******************************************************************************
  * @file    lcd1602-hal.h
  * @author  dinow
  * @version V0.0.1
  * @date    2016-07-15
  * @brief
  ******************************************************************************
  * @attention
  *
  *
  *
  ******************************************************************************
  */

#ifndef __DRV_LCD1602_HAL_LCD1602_HAL_H_
#define __DRV_LCD1602_HAL_LCD1602_HAL_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/
#define LCD_Put(c)			LCD_WriteData(c)

/* Exported functions ------------------------------------------------------- */

void LCD_Init(void);
void LCD_Clear(void);
void LCD_Home(void);
void LCD_LeftToRightMode(FunctionalState newState);
void LCD_AutoScroll(FunctionalState newState);
void LCD_Display(FunctionalState newState);
void LCD_Cursor(FunctionalState newState);
void LCD_Blink(FunctionalState newState);
void LCD_ScrollRight(void);
void LCD_ScrollLeft(void);
void LCD_CursorMoveRight(void);
void LCD_CursorMoveLeft(void);
void LCD_SetLoc(uint8_t x, uint8_t y);
void LCD_WriteData(unsigned char data);
void LCD_CreateFont(uint8_t loc, char *font);
void LCD_Print(char *str);
void LCD_Enable(void);
void LCD_Disable(void);

void LCD_Test(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __DRV_LCD1602_HAL_LCD1602_HAL_H_ */
