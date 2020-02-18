//  ����һ��

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


//   �⺯����û�ж�GPIO�ڵ�ȡ������
void Toggle_GPIO(uint32_t gpioNum)
{
    unsigned int gpio_State=0;

    //  ��ȡ�Ľ��ֻռһλ�����λ
    gpio_State=GPIO_GetPinValue(gpioNum);

    //  ���ݶ˿ڵ���̬��ȡ�������һ״̬
    GPIO_SetPinValue(gpioNum,!gpio_State);
}


/*                ��ŷ�������

			����ֵ����ŷ��������������

					1���򿪵�ŷ�
					0���رյ�ŷ�
					2����ѯ��ŷ���״̬�����ڲ���Ҫ����ʱ��ϵ�ŷ�

			����ֵ����ŷ������״̬

					0����ŷ�����
					1����ŷ���·
					2����ŷ���·
					10�������������
*/
uint8_t DCF_Control(uint8_t state)
{
    uint16_t DuanLuThreshodValue=1200;    //  ��Ӧ����600mA

    uint16_t OpenLoadThreshodValue=100;   //  ��Ӧ����50mA

    uint16_t temp=0;

    if(state>2) return InputParaError;

    switch(state)
    {

        //  �رյ�ŷ�
    case 0:
        GPIO_SetPinValue(DCF,0);
        return DCFOK;
        break;

        // ��ϴ���ʱ���رյ�ŷ��󷵻�
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
		
		//  ��ŷ�δ����ʱ��ѯ��״̬
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
