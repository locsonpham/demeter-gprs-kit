/**
  ******************************************************************************
  * @file    
  * @author
  * @version
  * @date
  * @brief
  ******************************************************************************
  */
  
/* Includes ------------------------------------------------------------------*/
#ifndef __RINGBUF_H
#define __RINGBUF_H

#include <stdint.h>
#include <stdlib.h>

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
    volatile uint32_t head;                
    volatile uint32_t tail;                
    volatile uint32_t size;                
    volatile uint8_t *pt;  					
} RINGBUF;

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
int32_t RINGBUF_Init(RINGBUF *r, uint8_t* buf, uint32_t size);
int32_t RINGBUF_Put(RINGBUF *r, uint8_t c);
int32_t RINGBUF_Get(RINGBUF *r, uint8_t* c);
int32_t RINGBUF_GetFill(RINGBUF *r);

#endif

/******************* (C) COPYRIGHT 2015 UFOTech *****END OF FILE****/
