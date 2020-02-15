
/* Copyright Statement:
 *
 * This software/firmware and related documentation ("AutoChips Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to AutoChips Inc. and/or its licensors. Without
 * the prior written permission of AutoChips inc. and/or its licensors,  any
 * reproduction,  modification,  use or disclosure of AutoChips Software,  and
 * information contained herein,  in whole or in part,  shall be strictly
 * prohibited.
 *
 * AutoChips Inc. (C) 2018. All rights reserved.
 *
 * BY OPENING THIS FILE,  RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("AUTOCHIPS SOFTWARE")
 * RECEIVED FROM AUTOCHIPS AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. AUTOCHIPS EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES,  EXPRESS OR IMPLIED,  INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY,  FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES AUTOCHIPS PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, 
 * INCORPORATED IN,  OR SUPPLIED WITH THE AUTOCHIPS SOFTWARE,  AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN AUTOCHIPS
 * SOFTWARE. AUTOCHIPS SHALL ALSO NOT BE RESPONSIBLE FOR ANY AUTOCHIPS SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AUTOCHIPS'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE AUTOCHIPS SOFTWARE
 * RELEASED HEREUNDER WILL BE,  AT AUTOCHIPS'S OPTION,  TO REVISE OR REPLACE THE
 * AUTOCHIPS SOFTWARE AT ISSUE,  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO AUTOCHIPS FOR SUCH AUTOCHIPS SOFTWARE AT ISSUE.
 */

/*************<start>******************/


/*************<include>****************/
#include "includes.h"


/*************<macro>******************/
/*下面3个配置,可任意配置,但同时只能打开其中一个*/
#define ONLY_RECV_STD_FRM		(FALSE)//只接收标准帧数据
#define ONLY_RECV_EXT_FRM		(FALSE)//只接收扩展帧数据
#define RECV_STD_EXT_BOTH_FRM	(TRUE)//标准与扩展帧都接收

#define CAN_CLEAR_ALL_ID_BIT	(0x60000000)//是清零所有ID位,保留控制过滤ID类型位


/*************<enum>*******************/


/*************<union>******************/


/*************<struct>*****************/


/*************<variable>***************/
CAN_MSG_INFO 			g_sendCANMsgInfo;
CAN_MSG_INFO 			g_recvCANMsgInfo;
CAN_FilterControl 		g_canFilterTab[CAN_MAX_FILTER_NUM] = 
{
	{ 0, 0, 0x00000000, 0x00000000},    
    { 1, 0, 0x00000000, 0x00000000},    
    { 2, 0, 0x00000000, 0x00000000},
    { 3, 0, 0x00000000, 0x00000000},
    { 4, 0, 0x00000000, 0x00000000},
    { 5, 0, 0x00000000, 0x00000000},
    { 6, 0, 0x00000000, 0x00000000},
    { 7, 0, 0x00000000, 0x00000000},
    { 8, 0, 0x00000000, 0x00000000},    
    { 9, 0, 0x00000000, 0x00000000},    
    {10, 0, 0x00000000, 0x00000000},   
    {11, 0, 0x00000000, 0x00000000},
    {12, 0, 0x00000000, 0x00000000},
    {13, 0, 0x00000000, 0x00000000},
    {14, 0, 0x00000000, 0x00000000},
    {15, 0, 0x00000000, 0x00000000},
};
uint32_t 				g_canFilterMask[3] = {0x5FFFFFFF,  0x7FFFFFFF,  0x1FFFFFFF};
//											 只接收标准帧 只接收扩展帧 接收两种数据帧


/*************<prototype>**************/

int32_t CAN1_IRQnCallBack(uint32_t event, uint32_t wparam, uint32_t lparam);



 


/**
* @prototype CAN_InitHardwr(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 Initalize CAN module.
*			 初始化CAN模块.
*/
void CAN_InitHardwr(void)
{
	CAN_Config canConfig = {0};
	
	CAN_BaudrateConfig canBandrateConfig = {0};
	
	GPIO_SetFunc(CAN1_TX, 2);//设置CAN1引脚复用功能
	GPIO_SetFunc(CAN1_RX, 2);

	/*
	设置波特率为500K,采样点为81.25%.
	
	tSeg1 = (S_SEG_1 + 2); tSeg2 = (S_SEG_2 + 1).
	
	BandRate 	= (48M / (S_PRESC + 1) / ((S_SEG_1 + 2) + (S_SEG_2 + 1)))
	
	SamplePoint = (tSeg1 / (tSeg1 + tSeg2)).
	
	在已经知道波特率与采样点的情况下,经过计算后:tSeg1 = 13,tSeg2 = 3.
	 */
	canBandrateConfig.S_PRESC 	= 11;//分频后时钟为8M.
	canBandrateConfig.S_SEG_1 	= 11;
	canBandrateConfig.S_SEG_2 	= 2;
	canBandrateConfig.S_SJW 	= 2;//满足S_SJW <= tSeg2即可.
	
	/* 
	16个过滤器中,可随意选择使用哪个,并制定其过滤模式,制定其过滤ID的类型.
	如果一个过滤器只需要接收一个ID,直接可将其设置CODE模式,然后根据ID类型设置过滤类型.
	如果一个过滤器需要接收多个ID，将其设置为MASK模式,并根据这几个ID的实际类型设置过滤ID类型,如果既有STD又有EXT,则应选择FILTER_IDE_STD_EXT_BOTH.
	
	 */


	CAN_SetFilterParam( 0, 1, CAN_FILTER_CODE_MODE, FILTER_IDE_EXT_ONLY, CAN1_RECV_DATA_ID11);//使用CODE模式只接收EXT帧
	CAN_SetFilterParam( 1, 1, CAN_FILTER_CODE_MODE, FILTER_IDE_EXT_ONLY, CAN1_RECV_DATA_ID12);//使用CODE模式只接收EXT帧

	canConfig.interruptEnable =1;//使能中断
	canConfig.TSSS = 1;//使用次发送缓冲区发送
	canConfig.canMode = CAN_MODE_NORMAL;
	canConfig.autoReset = 1;//CAN BUSOFF后,自动恢复
	canConfig.filterList = g_canFilterTab;//赋值过滤器设置list
	
	CAN_SetEventCallBack(CAN1, CAN1_IRQnCallBack);
	
	CAN_Initialize(CAN1, &canConfig, &canBandrateConfig);
}

/**
* @prototype CAN_SetFilterParam(uint8_t filterNum, uint8_t filterEn, CAN_FilterMode_TypeDef filterMode, CAN_FilterMask_TypeDef filterMask, uint32_t ID)
*
* @param[in] filterNum:过滤器编码,0到15可用.
* @param[in] filterEn:过滤器使能状态,1使能,0禁能
* @param[in] filterMode:CAN_FilterMode_TypeDef已定义.
* @param[in] filterMask:参考CAN_FilterMask_TypeDef.
* @param[in] ID
* @return	 void
*
* @brief  	 Initalize CAN filter.
*			 初始化CAN过滤器,当filterMode选择FILTER_CODE_MODE时,每个过滤器过滤一个ID,否则,使用MASK模式,过滤的多个ID的组合.
*/
void CAN_SetFilterParam(uint8_t filterNum, uint8_t filterEn, CAN_FilterMode_TypeDef filterMode, CAN_FilterMask_TypeDef filterMask, uint32_t ID)
{
	g_canFilterTab[filterNum].enable = filterEn;
	g_canFilterTab[filterNum].code = ID;
	
	if (filterMode == CAN_FILTER_MASK_MODE)
	{
		g_canFilterTab[filterNum].mask = (g_canFilterMask[filterMask] & ~ID);//进行掩码屏蔽
	}
	else
	{
		g_canFilterTab[filterNum].mask = (g_canFilterMask[filterMask] & CAN_CLEAR_ALL_ID_BIT);//进行ID屏蔽,就是ID必须与设置的ID一模一样,才接收
	}
}

/**
* @prototype Sdk_CAN_SendMsg(uint32_t ID, CAN_IDE_TypeDef IDE, CAN_RTR_TypeDef RTR, uint8_t *pDataBuf, uint8_t dataLen)
*
* @param[in] ID: send data CAN ID. 
* @param[in] IDE: Choose between CAN_IDE_STD and CAN_IDE_EXT.
* @param[in] RTR: Choose between CAN_RTR_STD and CAN_RTR_RMT.
* @param[in] pDataBuf: The pointer of the data to be sended.
* @param[in] dataLen: Send data length.
* @return	 CAN_SendSts_TypeDef,send fail or success.
*
* @brief  	 Send CAN message.
*			 发送CAN数据.
*/
CAN_SendSts_TypeDef CAN_SendMsg(uint32_t ID, CAN_IDE_TypeDef IDE, CAN_RTR_TypeDef RTR, uint8_t *pDataBuf, uint8_t dataLen)
{
	g_sendCANMsgInfo.ID = ID;
	g_sendCANMsgInfo.IDE = IDE;
	g_sendCANMsgInfo.RTR = RTR;
	g_sendCANMsgInfo.DLC = dataLen;
	memcpy(g_sendCANMsgInfo.Data, pDataBuf, dataLen);
	
	if(!CAN_IsTransmitBusy(CAN1, TRANSMIT_SECONDARY))
	{
		if (!CAN_MessageSend(CAN1, &g_sendCANMsgInfo, TRANSMIT_SECONDARY))
		{
			return CAN_SEND_MSG_SUCCS;
		}
		else
		{
			return CAN_SEND_MSG_FILED;
		}
	}
	
	return CAN_SEND_MSG_FILED;
}


/**
* @prototype CAN1_IRQnCallBack(uint32_t event, uint32_t wparam, uint32_t lparam)
*
* @param[in] ...
* @return	 ...
*
* @brief  	 CAN1 module interrupt handler.
*			 CAN1中断处理函数.
*/
int32_t CAN1_IRQnCallBack(uint32_t event, uint32_t wparam, uint32_t lparam)
{
	if (event & CAN_EVENT_RECVMSG)
	{
		if (CAN_IsMsgInReceiveBuf((CAN_Type*)lparam))
        {
            CAN_MessageRead((CAN_Type*)lparam, &g_recvCANMsgInfo);
			
			switch(g_recvCANMsgInfo.ID)
			{
				//  获取CAN总线上的胎压
				case 0x18ff1133:
					
					subSlaver[1].Data[8]=g_recvCANMsgInfo.Data[0];
				
				    subSlaver[6].Data[8]=g_recvCANMsgInfo.Data[0];
								
					subSlaver[2].Data[8]=g_recvCANMsgInfo.Data[3];
				
					subSlaver[5].Data[8]=g_recvCANMsgInfo.Data[3];
				
					break;
				
				//  获取CAN总线上的胎压
				case 0x18ff1233:
					
					subSlaver[3].Data[8]=g_recvCANMsgInfo.Data[0];
				
					subSlaver[4].Data[8]=g_recvCANMsgInfo.Data[0];
					
					break;
			}
			
			
        }
	}
	
	return 1;
}


/*************<end>********************/
