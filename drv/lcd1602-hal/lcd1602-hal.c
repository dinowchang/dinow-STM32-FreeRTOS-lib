/**
  ******************************************************************************
  * @file    lcd1602-hal.c
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

/* Includes ------------------------------------------------------------------*/
#include "global.h"
#include "lcd1602-hal.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define SUPPORT_LCD_TEST_COMMAND			0 // debug command for FreeRTOS-CLI
#define SUPPORT_LCD_RW_PIN                  0

#define LCD_NIBBLE_DELAY					5
#define LCD_INSTRUCTION_DELAY				40
#define LCD_INSTRUCTION_LONG_DELAY			(uint32_t)1600

// LCD pins
#define LCD_PW_GPIO_PORT					GPIOB
#define LCD_PW_GPIO_PIN						GPIO_PIN_6
#define LCD_PW_PIN                          LCD_PW_GPIO_PORT, LCD_PW_GPIO_PIN

#define LCD_RS_GPIO_PORT					GPIOA
#define LCD_RS_GPIO_PIN						GPIO_PIN_9
#define LCD_RS_PIN                          LCD_RS_GPIO_PORT, LCD_RS_GPIO_PIN

#if SUPPORT_LCD_RW_PIN
#define LCD_RW_GPIO_PORT					GPIOA
#define LCD_RW_GPIO_PIN						GPIO_PIN_8
#define LCD_RW_PIN                          LCD_RW_GPIO_PORT, LCD_RW_GPIO_PIN
#endif

#define LCD_EN_GPIO_PORT					GPIOC
#define LCD_EN_GPIO_PIN						GPIO_PIN_7
#define LCD_EN_PIN                          LCD_EN_GPIO_PORT, LCD_EN_GPIO_PIN

#define LCD_DB4_GPIO_PORT					GPIOB
#define LCD_DB4_GPIO_PIN					GPIO_PIN_5
#define LCD_DB4_PIN                         LCD_DB4_GPIO_PORT, LCD_DB4_GPIO_PIN

#define LCD_DB5_GPIO_PORT					GPIOB
#define LCD_DB5_GPIO_PIN					GPIO_PIN_4
#define LCD_DB5_PIN                         LCD_DB5_GPIO_PORT, LCD_DB5_GPIO_PIN

#define LCD_DB6_GPIO_PORT					GPIOB
#define LCD_DB6_GPIO_PIN					GPIO_PIN_10
#define LCD_DB6_PIN                         LCD_DB6_GPIO_PORT, LCD_DB6_GPIO_PIN

#define LCD_DB7_GPIO_PORT					GPIOA
#define LCD_DB7_GPIO_PIN					GPIO_PIN_8
#define LCD_DB7_PIN                         LCD_DB7_GPIO_PORT, LCD_DB7_GPIO_PIN


// HD44780 Commands
#define HD44780_CLEAR						0x01
#define HD44780_HOME						0x02
#define HD44780_ENTRY_MODE					0x04
#define HD44780_DISPLAY_CONTROL				0x08
#define HD44780_DISPLAY_CURSOR_SHIFT		0x10
#define HD44780_FUNCTION_SET				0x20
#define HD44780_CGRAM_SET					0x40
#define HD44780_DDRAM_SET					0x80

// Bits definition of entry mode
#define HD44780_EM_INCREMENT				0x02
#define HD44780_EM_SHIFT_DISPLAY			0x01

// Bits definition of display on/off control
#define HD44780_DISPLAY_ON					0x04
#define HD44780_CURSOR_ON					0x02
#define HD44780_CURSOR_BLINK				0x01

//Bits definition of cursor/display shift
#define HD44780_SHIFT_DISPLAY				0x08
#define HD44780_SHIFT_RIGHT					0x04

//Bits definition of function set
#define HD44780_8_BIT						0x10
#define HD44780_FONT5x10					0x04
#define HD44780_TWO_LINE					0x08

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static unsigned char m_EntryMode;
static unsigned char m_DisplayCtl;

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
 * @brief	Send nibble to LCD in 4bit mode
 * @param	nibble to send
 */
static void LCD_WriteNibble(unsigned char nibble)
{
	HAL_GPIO_WritePin(LCD_EN_PIN, GPIO_PIN_SET);

	HAL_GPIO_WritePin(LCD_DB4_PIN, (GPIO_PinState) (nibble & BIT0));
	HAL_GPIO_WritePin(LCD_DB5_PIN, (GPIO_PinState) (nibble & BIT1));
	HAL_GPIO_WritePin(LCD_DB6_PIN, (GPIO_PinState) (nibble & BIT2));
	HAL_GPIO_WritePin(LCD_DB7_PIN, (GPIO_PinState) (nibble & BIT3));

	udelay(LCD_NIBBLE_DELAY);
	HAL_GPIO_WritePin(LCD_EN_PIN, GPIO_PIN_RESET);
}

/**
 * @brief	Write instruction
 * @param	command
 */
static void LCD_WriteInstruction(unsigned char cmd)
{
	HAL_GPIO_WritePin(LCD_RS_PIN, GPIO_PIN_RESET);
	LCD_WriteNibble(cmd >> 4);
	LCD_WriteNibble(cmd & 0x0f);
	udelay(LCD_INSTRUCTION_DELAY);
}

/**
 * @brief	Write data
 * @param	data
 */
void LCD_WriteData(unsigned char data)
{
	HAL_GPIO_WritePin(LCD_RS_PIN, GPIO_PIN_SET);
	LCD_WriteNibble(data >> 4);
	LCD_WriteNibble(data & 0x0f);
	udelay(LCD_INSTRUCTION_DELAY);
}
/**
 * @brief	Write data to CGRAM
 * @param	addr
 */
void LCD_CreateFont(uint8_t loc, char *font)
{
	if (loc >= 8) return;
	LCD_WriteInstruction(HD44780_CGRAM_SET | (loc << 3));

	int i;
	for (i = 0; i < 8; i++)
		LCD_WriteData(font[i] & 0x1f);
}

/**
 * @brief	Clears display and returns cursor to the home position (address 0).
 */
void LCD_Clear(void)
{
	LCD_WriteInstruction(HD44780_CLEAR);
	udelay(LCD_INSTRUCTION_LONG_DELAY);
}

/**
 * @brief	Returns cursor to home position.
 * 			Also returns display being shifted to the original position. DDRAM content remains unchanged.
 */
void LCD_Home(void)
{
	LCD_WriteInstruction(HD44780_HOME);
	udelay(LCD_INSTRUCTION_LONG_DELAY);
}

/**
 * @brief	Cursor move direction when write data
 * @param	newState
 */
void LCD_LeftToRightMode(FunctionalState newState)
{
	if( newState != DISABLE )
		m_EntryMode |= HD44780_EM_INCREMENT;
	else
		m_EntryMode &= ~HD44780_EM_INCREMENT;

	LCD_WriteInstruction(m_EntryMode);
}

/**
 * @brief	Scroll display when write data
 * @param	newState
 */
void LCD_AutoScroll(FunctionalState newState)
{
	if( newState != DISABLE )
		m_EntryMode |= HD44780_EM_SHIFT_DISPLAY;
	else
		m_EntryMode &= ~HD44780_EM_SHIFT_DISPLAY;

	LCD_WriteInstruction(m_EntryMode);
}

/**
 * @brief	Control display on/off
 * @param	newState
 */
void LCD_Display(FunctionalState newState)
{
	if( newState != DISABLE )
		m_DisplayCtl |= HD44780_DISPLAY_ON;
	else
		m_DisplayCtl &= ~HD44780_DISPLAY_ON;

	LCD_WriteInstruction(m_DisplayCtl);
}

/**
 * @brief	Control cursor on/off
 * @param	newState
 */
void LCD_Cursor(FunctionalState newState)
{
	if( newState != DISABLE )
		m_DisplayCtl |= HD44780_CURSOR_ON;
	else
		m_DisplayCtl &= ~HD44780_CURSOR_ON;

	LCD_WriteInstruction(m_DisplayCtl);
}

/**
 * @brief	Control blink cursor on/off
 * @param	newState
 */
void LCD_Blink(FunctionalState newState)
{
	if( newState != DISABLE )
		m_DisplayCtl |= HD44780_CURSOR_BLINK;
	else
		m_DisplayCtl &= ~HD44780_CURSOR_BLINK;

	LCD_WriteInstruction(m_DisplayCtl);
}

/**
 *
 */
void LCD_ScrollRight(void)
{
	LCD_WriteInstruction(HD44780_DISPLAY_CURSOR_SHIFT | HD44780_SHIFT_DISPLAY | HD44780_SHIFT_RIGHT);
}

/**
 *
 */
void LCD_ScrollLeft(void)
{
	LCD_WriteInstruction(HD44780_DISPLAY_CURSOR_SHIFT | HD44780_SHIFT_DISPLAY);
}

/**
 *
 */
void LCD_CursorMoveRight(void)
{
	LCD_WriteInstruction(HD44780_DISPLAY_CURSOR_SHIFT | HD44780_SHIFT_RIGHT);
}

/**
 *
 */
void LCD_CursorMoveLeft(void)
{
	LCD_WriteInstruction(HD44780_DISPLAY_CURSOR_SHIFT);
}

/**
 *
 * @param x
 * @param y
 */
void LCD_SetLoc(uint8_t x, uint8_t y)
{
	LCD_WriteInstruction(HD44780_DDRAM_SET | ((y & 0x01) << 6) | (x & 0x3f));
}

/**
 * @brief	print a string
 * @param 	str : string to print
 */
void LCD_Print(char *str)
{
	while(*str) LCD_WriteData(*str++);
}

/**
 * @brief Configure flow of 4bits mode after power on
 */
static void LCD_SetTo4BitsMode(void)
{
	LCD_WriteNibble(0x03);
	udelay(LCD_INSTRUCTION_DELAY);
	LCD_WriteNibble(0x03);
	udelay(LCD_INSTRUCTION_DELAY);
	LCD_WriteNibble(0x03);
	udelay(LCD_INSTRUCTION_DELAY);
	LCD_WriteNibble(0x02);
	udelay(LCD_INSTRUCTION_DELAY);
}

/**
 * @brief Initialize flow after power on
 */
void LCD_PowerOnFlow(void)
{
	udelay(40000);

	m_EntryMode = HD44780_ENTRY_MODE;
	m_DisplayCtl = HD44780_DISPLAY_CONTROL;

	LCD_SetTo4BitsMode();
	LCD_WriteInstruction(HD44780_FUNCTION_SET | HD44780_TWO_LINE); // set LCD to font 5x7, 2 line
	LCD_Display(DISABLE);
	LCD_Clear();
	LCD_LeftToRightMode(ENABLE);
	LCD_Display(ENABLE);
}

/**
 * @brief Configure all pins and initialize LCD control IC
 */
void LCD_Enable(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	GPIO_InitStruct.Pin = LCD_PW_GPIO_PIN;
	HAL_GPIO_Init(LCD_PW_GPIO_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = LCD_RS_GPIO_PIN;
	HAL_GPIO_Init(LCD_RS_GPIO_PORT, &GPIO_InitStruct);
#if SUPPORT_LCD_RW_PIN
	GPIO_InitStruct.Pin = LCD_RW_GPIO_PIN;
	HAL_GPIO_Init(LCD_RW_GPIO_PORT, &GPIO_InitStruct);
#endif
	GPIO_InitStruct.Pin = LCD_EN_GPIO_PIN;
	HAL_GPIO_Init(LCD_EN_GPIO_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = LCD_DB4_GPIO_PIN;
	HAL_GPIO_Init(LCD_DB4_GPIO_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = LCD_DB5_GPIO_PIN;
	HAL_GPIO_Init(LCD_DB5_GPIO_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = LCD_DB6_GPIO_PIN;
	HAL_GPIO_Init(LCD_DB6_GPIO_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = LCD_DB7_GPIO_PIN;
	HAL_GPIO_Init(LCD_DB7_GPIO_PORT, &GPIO_InitStruct);

	HAL_GPIO_WritePin(LCD_PW_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LCD_RS_PIN, GPIO_PIN_RESET);
#if SUPPORT_LCD_RW_PIN
	HAL_GPIO_WritePin(LCD_RW_PIN, GPIO_PIN_RESET);
#endif
	HAL_GPIO_WritePin(LCD_EN_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_DB4_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_DB5_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_DB6_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_DB7_PIN, GPIO_PIN_RESET);

	LCD_PowerOnFlow();
}

/**
 * @brief Power off the LCD module and configure all pins to analog mode
 */
void LCD_Disable(void)
{
	HAL_GPIO_WritePin(LCD_PW_PIN, GPIO_PIN_RESET);

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;

	GPIO_InitStruct.Pin = LCD_PW_GPIO_PIN;
	HAL_GPIO_Init(LCD_PW_GPIO_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = LCD_RS_GPIO_PIN;
	HAL_GPIO_Init(LCD_RS_GPIO_PORT, &GPIO_InitStruct);
#if SUPPORT_LCD_RW_PIN
	GPIO_InitStruct.Pin = LCD_RW_GPIO_PIN;
	HAL_GPIO_Init(LCD_RW_GPIO_PORT, &GPIO_InitStruct);
#endif
	GPIO_InitStruct.Pin = LCD_EN_GPIO_PIN;
	HAL_GPIO_Init(LCD_EN_GPIO_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = LCD_DB4_GPIO_PIN;
	HAL_GPIO_Init(LCD_DB4_GPIO_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = LCD_DB5_GPIO_PIN;
	HAL_GPIO_Init(LCD_DB5_GPIO_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = LCD_DB6_GPIO_PIN;
	HAL_GPIO_Init(LCD_DB6_GPIO_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = LCD_DB7_GPIO_PIN;
	HAL_GPIO_Init(LCD_DB7_GPIO_PORT, &GPIO_InitStruct);
}


void LCD_Init(void)
{
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

	LCD_Enable();

#if SUPPORT_LCD_TEST_COMMAND
	LCD_Test();
#endif

}
