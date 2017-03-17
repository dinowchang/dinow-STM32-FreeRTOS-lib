/**
  ******************************************************************************
  * @file    lcd1602-hal-test.c
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

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
char customChar[8] = {
	0b00100,
	0b01110,
	0b10101,
	0b00100,
	0b00100,
	0b00100,
	0b00100,
	0b00000
};

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
 * @param	pcWriteBuffer
 * @param	xWriteBufferLen
 * @param	pcCommandString
 * @return
 */
static BaseType_t LCD_EnableCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	LCD_Enable();
	sprintf(pcWriteBuffer, "Enable LCD\n");
	return pdFALSE;

}

static const CLI_Command_Definition_t xLcdEnable =
{
	"lcd-enable",
	"lcd-enable:\n    Enable LCD\n",
	LCD_EnableCommand,
	0
};

/**
 * @param	pcWriteBuffer
 * @param	xWriteBufferLen
 * @param	pcCommandString
 * @return
 */
static BaseType_t LCD_DisableCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	LCD_Disable();
	sprintf(pcWriteBuffer, "Disable LCD\n");
	return pdFALSE;

}

static const CLI_Command_Definition_t xLcdDisable =
{
	"lcd-disable",
	"lcd-disable:\n    Disable LCD\n",
	LCD_DisableCommand,
	0
};

/**
 * @param	pcWriteBuffer
 * @param	xWriteBufferLen
 * @param	pcCommandString
 * @return
 */
static BaseType_t LCD_ClearCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	LCD_Clear();
	sprintf(pcWriteBuffer, "LCD clear display\n");
	return pdFALSE;

}

static const CLI_Command_Definition_t xLcdClear =
{
	"lcd-clear",
	"lcd-clear:\n    Clear display\n",
	LCD_ClearCommand,
	0
};

/**
 * @param	pcWriteBuffer
 * @param	xWriteBufferLen
 * @param	pcCommandString
 * @return
 */
static BaseType_t LCD_HomeCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	LCD_Home();
	sprintf(pcWriteBuffer, "LCD move cursor home\n");
	return pdFALSE;

}

static const CLI_Command_Definition_t xLcdHome =
{
	"lcd-home",
	"lcd-home:\n    Move cursor to home position\n",
	LCD_HomeCommand,
	0
};

/**
 * @param	pcWriteBuffer
 * @param	xWriteBufferLen
 * @param	pcCommandString
 * @return
 */
static BaseType_t LCD_CursorCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	const char *parameterPtr;
	int32_t paramterLen;
	uint32_t mode;

	parameterPtr = FreeRTOS_CLIGetParameter(pcCommandString, 1, &paramterLen);
	mode = DecToInt((char *) parameterPtr, paramterLen);

	LCD_Cursor(mode & BIT0);
	LCD_Blink(mode & BIT1);

	sprintf(pcWriteBuffer, "Cursor set to mode: %d\n", (uint16_t)(mode & 0x03));

	return pdFALSE;

}

static const CLI_Command_Definition_t xLcdCursor =
{
	"lcd-cur",
	"lcd-cur <mode>:\n    Cursor mode 0~3\n",
	LCD_CursorCommand,
	1
};

/**
 * @param	pcWriteBuffer
 * @param	xWriteBufferLen
 * @param	pcCommandString
 * @return
 */
static BaseType_t LCD_MoveCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	const char *parameterPtr;
	int32_t paramterLen;
	uint32_t mode;

	parameterPtr = FreeRTOS_CLIGetParameter(pcCommandString, 1, &paramterLen);
	mode = DecToInt((char *) parameterPtr, paramterLen);

	switch(mode)
	{
		case 3:
			LCD_ScrollRight();
			break;

		case 2:
			LCD_ScrollLeft();
			break;

		case 1:
			LCD_CursorMoveRight();
			break;

		case 0:
			LCD_CursorMoveLeft();
			break;

		default:
			break;
	}

	sprintf(pcWriteBuffer, "\n");

	return pdFALSE;

}

static const CLI_Command_Definition_t xLcdMove =
{
	"lcd-mv",
	"lcd-mv <mode>:\n    move 0~3\n",
	LCD_MoveCommand,
	1
};

/**
 * @param	pcWriteBuffer
 * @param	xWriteBufferLen
 * @param	pcCommandString
 * @return
 */
static BaseType_t LCD_LocCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	const char *parameterPtr;
	int32_t paramterLen;
	uint32_t x, y;

	parameterPtr = FreeRTOS_CLIGetParameter(pcCommandString, 1, &paramterLen);
	x = DecToInt((char *) parameterPtr, paramterLen);

	parameterPtr = FreeRTOS_CLIGetParameter(pcCommandString, 2, &paramterLen);
	y = DecToInt((char *) parameterPtr, paramterLen);

	LCD_SetLoc(x, y);

	sprintf(pcWriteBuffer, "\n");

	return pdFALSE;

}

static const CLI_Command_Definition_t xLcdLoc =
{
	"lcd-loc",
	"lcd-loc <x> <y>:\n    move to x,y\n",
	LCD_LocCommand,
	2
};

/**
 * @param	pcWriteBuffer
 * @param	xWriteBufferLen
 * @param	pcCommandString
 * @return
 */
static BaseType_t LCD_PutCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	const char *parameterPtr;
	int32_t paramterLen;
	uint32_t num;

	parameterPtr = FreeRTOS_CLIGetParameter(pcCommandString, 1, &paramterLen);

	if (paramterLen > 3 && parameterPtr[0] == '0' && parameterPtr[1] == 'x')
		num = HexToInt((char *) parameterPtr, paramterLen);
	else
		num = DecToInt((char *) parameterPtr, paramterLen);

	LCD_Put(num);

	sprintf(pcWriteBuffer, "\n");
	return pdFALSE;
}

static const CLI_Command_Definition_t xLcdPut =
{
	"lcd-put",
	"lcd-put <num> :\n    put char of ascii <num>\n",
	LCD_PutCommand,
	1
};

/**
 * @param	pcWriteBuffer
 * @param	xWriteBufferLen
 * @param	pcCommandString
 * @return
 */
static BaseType_t LCD_PrintCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	const char *parameterPtr;
	int32_t paramterLen;
	uint8_t i = 0;

	parameterPtr = FreeRTOS_CLIGetParameter(pcCommandString, 1, &paramterLen);
	while(i < paramterLen)
	{
		LCD_WriteData(parameterPtr[i]);
		i++;
	}
	sprintf(pcWriteBuffer, "\n");

	return pdFALSE;

}

static const CLI_Command_Definition_t xLcdPrint =
{
	"lcd-print",
	"lcd-print <message>:\n    print message\n",
	LCD_PrintCommand,
	1
};

/**
 * @brief	Register CLI command
 */
void LCD_Test(void)
{
	LCD_CreateFont(1, customChar);
	LCD_SetLoc(0, 0);
	FreeRTOS_CLIRegisterCommand(&xLcdEnable);
	FreeRTOS_CLIRegisterCommand(&xLcdDisable);
	FreeRTOS_CLIRegisterCommand(&xLcdClear);
	FreeRTOS_CLIRegisterCommand(&xLcdHome);
	FreeRTOS_CLIRegisterCommand(&xLcdCursor);
	FreeRTOS_CLIRegisterCommand(&xLcdMove);
	FreeRTOS_CLIRegisterCommand(&xLcdLoc);
	FreeRTOS_CLIRegisterCommand(&xLcdPut);
	FreeRTOS_CLIRegisterCommand(&xLcdPrint);
}
