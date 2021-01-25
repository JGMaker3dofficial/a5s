
#include "bsp_key.h"

/*

	如果用户的按键个数小于8个，你可以将多余的按键全部定义为和第1个按键一样，并不影响程序功能
	#define KEY_COUNT    8	  这个在 bsp_key.h 文件中定义
*/

/*
	
		K1 键      : PC13   (低电平表示按下)
		K2 键      : PA0    ( --- 高电平表示按下)
		K3 键      : PG8    (低电平表示按下)
		摇杆UP键   : PG15   (低电平表示按下)
		摇杆DOWN键 : PD3    (低电平表示按下)
		摇杆LEFT键 : PG14   (低电平表示按下)
		摇杆RIGHT键: PG13   (低电平表示按下)
		摇杆OK键   : PG7    (低电平表示按下)

*/

/* 按键口对应的RCC时钟 */
#define RCC_ALL_KEY 	(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOG |RCC_APB2Periph_GPIOC |RCC_APB2Periph_GPIOF)

#define GPIO_PORT_K1    GPIOC	//X_MIN_S
#define GPIO_PIN_K1	    GPIO_Pin_6

#define GPIO_PORT_K2    GPIOG	//Y_MIN_S
#define GPIO_PIN_K2	    GPIO_Pin_8

#define GPIO_PORT_K3    GPIOG	//Z_MIN_S
#define GPIO_PIN_K3	    GPIO_Pin_7

//#define GPIO_PORT_K4    GPIOC	//X_MAX_S	
//#define GPIO_PIN_K4	    GPIO_Pin_5
#define GPIO_PORT_K4    GPIOB	//X_MAX_S	
#define GPIO_PIN_K4	    GPIO_Pin_6

//#define GPIO_PORT_K5    GPIOB//Y_MAX_S	
//#define GPIO_PIN_K5	    GPIO_Pin_0
#define GPIO_PORT_K5    GPIOC//Y_MAX_S	
#define GPIO_PIN_K5	    GPIO_Pin_6

#define GPIO_PORT_K6    GPIOB//Z_MAX_S	
#define GPIO_PIN_K6	    GPIO_Pin_1

#define GPIO_PORT_K7    GPIOC//断料检测IO
#define GPIO_PIN_K7	    GPIO_Pin_7


#define GPIO_PORT_K8    GPIOF//SD卡插入检测IO
#define GPIO_PIN_K8	    GPIO_Pin_10



static KEY_T s_tBtn[KEY_COUNT];
static KEY_FIFO_T s_tKey;		/* 按键FIFO变量,结构体 */

static void bsp_InitKeyVar(void);
static void bsp_InitKeyHard(void);
static void bsp_DetectKey(uint8_t i);

/*
*********************************************************************************************************
*	函 数 名: IsKeyDownX
*	功能说明: 判断按键是否按下
*	形    参: 无
*	返 回 值: 返回值1 表示按下，0表示未按下
*********************************************************************************************************
*/

#if 0	/* 为了区分3个事件:　K1单独按下, K2单独按下， K1和K2同时按下 */
static uint8_t IsKeyDown1(void)
{
	if ((GPIO_PORT_K1->IDR & GPIO_PIN_K1) == 0 && (GPIO_PORT_K2->IDR & GPIO_PIN_K2) == 0
		&& (GPIO_PORT_K3->IDR & GPIO_PIN_K3) != 0)
		return 1;
	else 
		return 0;
}
static uint8_t IsKeyDown2(void)
{
	if ((GPIO_PORT_K1->IDR & GPIO_PIN_K1) != 0 && (GPIO_PORT_K2->IDR & GPIO_PIN_K2) != 0
		&& (GPIO_PORT_K3->IDR & GPIO_PIN_K3) != 0)
		return 1;
	else 
		return 0;
}
static uint8_t IsKeyDown3(void)
{
	if ((GPIO_PORT_K1->IDR & GPIO_PIN_K1) != 0 && (GPIO_PORT_K2->IDR & GPIO_PIN_K2) == 0
		&& (GPIO_PORT_K3->IDR & GPIO_PIN_K3) == 0)
		return 1;
	else 
		return 0;
}
static uint8_t IsKeyDown9(void)	/* K1 K2组合键 */
{
	if ((GPIO_PORT_K1->IDR & GPIO_PIN_K1) == 0 && (GPIO_PORT_K2->IDR & GPIO_PIN_K2) != 0
		&& (GPIO_PORT_K3->IDR & GPIO_PIN_K3) != 0)
		return 1;
	else 
		return 0;
}
static uint8_t IsKeyDown10(void)	/* K2 K3组合键 */
{
	if ((GPIO_PORT_K1->IDR & GPIO_PIN_K1) != 0 && (GPIO_PORT_K2->IDR & GPIO_PIN_K2) != 0
		&& (GPIO_PORT_K3->IDR & GPIO_PIN_K3) == 0)
		return 1;
	else 
		return 0;
}
#else
	
static uint8_t IsKeyDown1(void) {if ((GPIO_PORT_K1->IDR & GPIO_PIN_K1) == 0) return 1;else return 0;}
static uint8_t IsKeyDown2(void) {if ((GPIO_PORT_K2->IDR & GPIO_PIN_K2) == 0) return 1;else return 0;}
static uint8_t IsKeyDown3(void) {if ((GPIO_PORT_K3->IDR & GPIO_PIN_K3) == 0) return 1;else return 0;}
static uint8_t IsKeyDown4(void) {if ((GPIO_PORT_K4->IDR & GPIO_PIN_K4) == 0) return 1;else return 0;}

//static uint8_t IsKeyDown9(void) {if (IsKeyDown1() && IsKeyDown2()) return 1;else return 0;}		/* K1 K2组合键 */
//static uint8_t IsKeyDown10(void) {if (IsKeyDown2() && IsKeyDown3()) return 1;else return 0;}	/* K2 K3组合键 */
#endif

///* 5方向摇杆 */
//static uint8_t IsKeyDown4(void) {if ((GPIO_PORT_K4->IDR & GPIO_PIN_K4) == 0) return 1;else return 0;}
static uint8_t IsKeyDown5(void) {if ((GPIO_PORT_K5->IDR & GPIO_PIN_K5) == 0) return 1;else return 0;}
static uint8_t IsKeyDown6(void) {if ((GPIO_PORT_K6->IDR & GPIO_PIN_K6) == 0) return 1;else return 0;}
static uint8_t IsKeyDown7(void) {if ((GPIO_PORT_K7->IDR & GPIO_PIN_K7) == 0) return 0;else return 1;}//检测高电平
static uint8_t IsKeyDown8(void) {if ((GPIO_PORT_K8->IDR & GPIO_PIN_K8) == 0) return 1;else return 0;}

/*
*********************************************************************************************************
*	函 数 名: bsp_InitKey
*	功能说明: 初始化按键. 该函数被 bsp_Init() 调用。
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitKey(void)
{
	bsp_InitKeyVar();		/* 初始化按键变量 */
	bsp_InitKeyHard();		/* 初始化按键硬件 */
}

/*
*********************************************************************************************************
*	函 数 名: bsp_PutKey
*	功能说明: 将1个键值压入按键FIFO缓冲区。可用于模拟一个按键。
*	形    参:  _KeyCode : 按键代码
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_PutKey(uint8_t _KeyCode)
{
	s_tKey.Buf[s_tKey.Write] = _KeyCode;

	if (++s_tKey.Write  >= KEY_FIFO_SIZE)
	{
		s_tKey.Write = 0;
	}
}

/*
*********************************************************************************************************
*	函 数 名: bsp_GetKey
*	功能说明: 从按键FIFO缓冲区读取一个键值。
*	形    参:  无
*	返 回 值: 按键代码
*********************************************************************************************************
*/
uint8_t bsp_GetKey(void)
{
	uint8_t ret;

	if (s_tKey.Read == s_tKey.Write)
	{
		return KEY_NONE;
	}
	else
	{
		ret = s_tKey.Buf[s_tKey.Read];

		if (++s_tKey.Read >= KEY_FIFO_SIZE)
		{
			s_tKey.Read = 0;
		}
		return ret;
	}
}

/*
*********************************************************************************************************
*	函 数 名: bsp_GetKey2
*	功能说明: 从按键FIFO缓冲区读取一个键值。独立的读指针。
*	形    参:  无
*	返 回 值: 按键代码
*********************************************************************************************************
*/
uint8_t bsp_GetKey2(void)
{
	uint8_t ret;

	if (s_tKey.Read2 == s_tKey.Write)
	{
		return KEY_NONE;
	}
	else
	{
		ret = s_tKey.Buf[s_tKey.Read2];

		if (++s_tKey.Read2 >= KEY_FIFO_SIZE)
		{
			s_tKey.Read2 = 0;
		}
		return ret;
	}
}

/*
*********************************************************************************************************
*	函 数 名: bsp_GetKeyState
*	功能说明: 读取按键的状态
*	形    参:  _ucKeyID : 按键ID，从0开始
*	返 回 值: 1 表示按下， 0 表示未按下
*********************************************************************************************************
*/
uint8_t bsp_GetKeyState(KEY_ID_E _ucKeyID)
{
	return s_tBtn[_ucKeyID].State;
}

/*
*********************************************************************************************************
*	函 数 名: bsp_SetKeyParam
*	功能说明: 设置按键参数
*	形    参：_ucKeyID : 按键ID，从0开始
*			_LongTime : 长按事件时间
*			 _RepeatSpeed : 连发速度
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_SetKeyParam(uint8_t _ucKeyID, uint16_t _LongTime, uint8_t  _RepeatSpeed)
{
	s_tBtn[_ucKeyID].LongTime = _LongTime;			/* 长按时间 0 表示不检测长按键事件 */
	s_tBtn[_ucKeyID].RepeatTime = _RepeatSpeed;			/* 按键连发的速度，0表示不支持连发 */
	s_tBtn[_ucKeyID].RepeatCount = 0;						/* 连发计数器 */
}


/*
*********************************************************************************************************
*	函 数 名: bsp_ClearKey
*	功能说明: 清空按键FIFO缓冲区
*	形    参：无
*	返 回 值: 按键代码
*********************************************************************************************************
*/
void bsp_ClearKey(void)
{
	s_tKey.Read = s_tKey.Write;
}

/*
*********************************************************************************************************
*	函 数 名: bsp_InitKeyHard
*	功能说明: 配置按键对应的GPIO
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
static void bsp_InitKeyHard(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* 第1步：打开GPIO时钟 */
	RCC_APB2PeriphClockCmd(RCC_ALL_KEY, ENABLE);

	/* 第2步：配置所有的按键GPIO为浮动输入模式(实际上CPU复位后就是输入状态) */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	/* 输入浮空模式 */
	
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_K1;
	GPIO_Init(GPIO_PORT_K1, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_K2;
	GPIO_Init(GPIO_PORT_K2, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_K3;
	GPIO_Init(GPIO_PORT_K3, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_K4;
	GPIO_Init(GPIO_PORT_K4, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_K5;
	GPIO_Init(GPIO_PORT_K5, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_K6;
	GPIO_Init(GPIO_PORT_K6, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_K7;
	GPIO_Init(GPIO_PORT_K7, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_K8;
	GPIO_Init(GPIO_PORT_K8, &GPIO_InitStructure);
}

/*
*********************************************************************************************************
*	函 数 名: bsp_InitKeyVar
*	功能说明: 初始化按键变量
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
static void bsp_InitKeyVar(void)
{
	uint8_t i;

	/* 对按键FIFO读写指针清零 */
	s_tKey.Read = 0;
	s_tKey.Write = 0;
	s_tKey.Read2 = 0;

	/* 给每个按键结构体成员变量赋一组缺省值 */
	for (i = 0; i < KEY_COUNT; i++)
	{
		s_tBtn[i].State = 0;							/* 按键缺省状态，0为未按下 */
		
		s_tBtn[i].FilterCount = KEY_FILTER_TIME / 2;	/* 计数器设置为滤波时间的一半 */
		s_tBtn[i].FilterTime = KEY_FILTER_TIME;	
		
		s_tBtn[i].LongCount = 0;	
		s_tBtn[i].LongTime = KEY_LONG_TIME;				/* 长按时间 0 表示不检测长按键事件 */
		
		s_tBtn[i].RepeatCount = 0;						/* 连发计数器 */
		s_tBtn[i].RepeatTime = 0;						/* 按键连发的速度，0表示不支持连发 */
		
		//s_tBtn[i].KeyCodeDown = 3 * i + 1;				/* 按键按下的键值代码 */
		//s_tBtn[i].KeyCodeUp   = 3 * i + 2;				/* 按键弹起的键值代码 */
		//s_tBtn[i].KeyCodeLong = 3 * i + 3;				/* 按键被持续按下的键值代码 */
	}

//	/* 如果需要单独更改某个按键的参数，可以在此单独重新赋值 */
//	/* 比如，我们希望按键1按下超过1秒后，自动重发相同键值 */

	// 断料检测
	s_tBtn[KID_JOY_R].FilterCount	= 150;
	s_tBtn[KID_JOY_R].FilterTime	= 300;/* 断料检测开关质量差存在误触发现象，把过滤时间设为3s降低误触发几率 */
	s_tBtn[KID_JOY_R].LongTime = 60;
	s_tBtn[KID_JOY_R].RepeatTime = 30;

	// SD卡插入检测
	s_tBtn[KID_JOY_OK].FilterCount = 5;
	s_tBtn[KID_JOY_OK].FilterTime = 5;
	s_tBtn[KID_JOY_OK].LongTime = 100;
	s_tBtn[KID_JOY_OK].RepeatTime = 100;
	
	/* 判断按键按下的函数 */
	s_tBtn[0].IsKeyDownFunc = IsKeyDown1;
	s_tBtn[1].IsKeyDownFunc = IsKeyDown2;
	s_tBtn[2].IsKeyDownFunc = IsKeyDown3;
	s_tBtn[3].IsKeyDownFunc = IsKeyDown4;
	s_tBtn[4].IsKeyDownFunc = IsKeyDown5;
	s_tBtn[5].IsKeyDownFunc = IsKeyDown6;
	s_tBtn[6].IsKeyDownFunc = IsKeyDown7;
	s_tBtn[7].IsKeyDownFunc = IsKeyDown8;

//	/* 组合键 */
//	s_tBtn[8].IsKeyDownFunc = IsKeyDown9;
//	s_tBtn[9].IsKeyDownFunc = IsKeyDown10;
}

/*
*********************************************************************************************************
*	函 数 名: bsp_DetectKey
*	功能说明: 检测一个按键。非阻塞状态，必须被周期性的调用。
*	形    参:  按键结构变量指针
*	返 回 值: 无
*********************************************************************************************************
*/
static void bsp_DetectKey(uint8_t i)
{
	KEY_T *pBtn;

	/*
		如果没有初始化按键函数，则报错
		if (s_tBtn[i].IsKeyDownFunc == 0)
		{
			printf("Fault : DetectButton(), s_tBtn[i].IsKeyDownFunc undefine");
		}
	*/

	pBtn = &s_tBtn[i];
	if (pBtn->IsKeyDownFunc())//
	{
		if (pBtn->FilterCount < pBtn->FilterTime)
		{
			pBtn->FilterCount = pBtn->FilterTime;
		}
		else if(pBtn->FilterCount < 2 * pBtn->FilterTime)
		{
			pBtn->FilterCount++;
		}
		else
		{
			if (pBtn->State == 0)
			{
				pBtn->State = 1;	if(i==7) DBG("SD IN\r\n");

				/* 发送按钮按下的消息 */
				bsp_PutKey((uint8_t)(3 * i + 1));
			}

			if (pBtn->LongTime > 0)
			{
				if (pBtn->LongCount < pBtn->LongTime)
				{
					/* 发送按钮持续按下的消息 */
					if (++pBtn->LongCount == pBtn->LongTime)
					{
						/* 键值放入按键FIFO */
						bsp_PutKey((uint8_t)(3 * i + 3));
					}
				}
				else
				{
					if (pBtn->RepeatTime > 0)
					{
						if (++pBtn->RepeatCount >= pBtn->RepeatTime)
						{
							pBtn->RepeatCount = 0;
							/* 常按键后，每隔10ms发送1个按键 */
							bsp_PutKey((uint8_t)(3 * i + 1));
						}
					}
				}
			}
		}
	}
	else
	{
		if(pBtn->FilterCount > pBtn->FilterTime)
		{
			pBtn->FilterCount = pBtn->FilterTime;
		}
		else if(pBtn->FilterCount != 0)
		{
			pBtn->FilterCount--;
		}
		else
		{
			if (pBtn->State == 1)
			{
				pBtn->State = 0;	if(i==7) DBG("SD OUT\r\n");

				/* 发送按钮弹起的消息 */
				bsp_PutKey((uint8_t)(3 * i + 2));
			}
		}

		pBtn->LongCount = 0;
		pBtn->RepeatCount = 0;
	}
}

/*
*********************************************************************************************************
*	函 数 名: bsp_KeyScan
*	功能说明: 扫描所有按键。非阻塞，被systick中断周期性的调用
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_KeyScan(void)
{
	uint8_t i;

	for (i = 0; i < KEY_COUNT; i++)
	{
		bsp_DetectKey(i);
	}
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
