//  新增一行

#include "ac78xx_gpio.h"
#include "test.h"

enum
{
    InputParaError=10,
    DCFOK=0,
    DCFOpenLoad=1,
    DCFDuanlu=2



};


/**
* GPIO_Init
*
* @param[in]  none
* @return none
*
* @brief  Init gpio
*
*/
void GPIO_Init(void)
{
    /* init gpio as output mode */

    GPIO_SetDir(LED,1);

    GPIO_SetDir(DCF,1);


}


//   库函数中没有对GPIO口的取反函数
void Toggle_GPIO(uint32_t gpioNum)
{
    unsigned int gpio_State=0;

    //  读取的结果只占一位，最低位
    gpio_State=GPIO_GetPinValue(gpioNum);

    //  根据端口的现态，取反输出下一状态
    GPIO_SetPinValue(gpioNum,!gpio_State);
}


/*                电磁阀的驱动

			输入值：电磁阀的输出控制命令

					1：打开电磁阀
					0：关闭电磁阀
					2：查询电磁阀的状态，用于不需要动作时诊断电磁阀

			返回值：电磁阀的诊断状态

					0：电磁阀正常
					1：电磁阀开路
					2：电磁阀短路
					10：输入参数错误
*/
uint8_t DCF_Control(uint8_t state)
{
    uint16_t DuanLuThreshodValue=1200;    //  对应电流600mA

    uint16_t OpenLoadThreshodValue=100;   //  对应电流50mA

    uint16_t temp=0;

    if(state>2) return InputParaError;

    switch(state)
    {

        //  关闭电磁阀
    case 0:
        GPIO_SetPinValue(DCF,0);
        return DCFOK;
        break;

        // 诊断错误时，关闭电磁阀后返回
    case 1:
        GPIO_SetPinValue(DCF,1);

        delay(50);

        temp=ADC_Sample(DCFSample);

        if(temp<OpenLoadThreshodValue)
        {
            GPIO_SetPinValue(DCF,0);
            return DCFOpenLoad;
        }

        else if (temp<DuanLuThreshodValue)
            return DCFOK;

        else
        {
            GPIO_SetPinValue(DCF,0);
            return DCFDuanlu;
        }
        break;

    case 2:
		
		//  电磁阀未工作时查询其状态
        if((GPIO_GetPinValue(DCF)==0))
        {
            GPIO_SetPinValue(DCF,1);
            delay(50);

            temp=ADC_Sample(DCFSample);

            if(temp<OpenLoadThreshodValue)
            {
                GPIO_SetPinValue(DCF,0);
                return DCFOpenLoad;
            }

            else if (temp<DuanLuThreshodValue)
            {
                GPIO_SetPinValue(DCF,0);
                return DCFOK;
            }

            else
            {
                GPIO_SetPinValue(DCF,0);
                return DCFDuanlu;
            }
        }
        else
        {

            temp=ADC_Sample(DCFSample);

            if(temp<OpenLoadThreshodValue)
            {
                GPIO_SetPinValue(DCF,0);
                return DCFOpenLoad;
            }

            else if (temp<DuanLuThreshodValue)
            {
                return DCFOK;
            }

            else
            {
                GPIO_SetPinValue(DCF,0);
                return DCFDuanlu;
            }
        }
        break;
    }

    return DCFOK;
}
