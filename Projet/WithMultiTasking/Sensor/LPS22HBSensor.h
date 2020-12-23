/**
 ******************************************************************************
 * @file    LPS22HBSensor.h
 * @author  CLab
 * @version V1.0.0
 * @date    5 August 2016
 * @brief   Abstract Class of an LPS22HB Pressure sensor.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */


/* Prevent recursive inclusion -----------------------------------------------*/

#ifndef __LPS22HBSensor_H__
#define __LPS22HBSensor_H__


/* Includes ------------------------------------------------------------------*/

#include "DevI2C.h"
#include "LPS22HB_driver.h"
#include <assert.h>

/* Class Declaration ---------------------------------------------------------*/

/**
 * Abstract class of an LPS22HB Pressure sensor.
 */
class LPS22HBSensor 
{
  public:
  
    enum SPI_type_t {SPI3W, SPI4W};  
    LPS22HBSensor(SPI *spi, PinName cs_pin, PinName int_pin=NC, SPI_type_t spi_type=SPI4W);     
    LPS22HBSensor(DevI2C *i2c, uint8_t address=LPS22HB_ADDRESS_HIGH, PinName int_pin=NC);
    virtual int init(void *init);
    virtual int read_id(uint8_t *id);
    virtual int get_pressure(float *pfData);
    virtual int get_temperature(float *pfData);
    int enable(void);
    int disable(void);
    int reset(void);
    int get_odr(float *odr);
    int set_odr(float odr);
    int read_reg(uint8_t reg, uint8_t *data);
    int write_reg(uint8_t reg, uint8_t data);
    uint8_t io_read(uint8_t *pBuffer, uint8_t RegisterAddr, uint16_t NumByteToRead);
    uint8_t io_write(uint8_t *pBuffer, uint8_t RegisterAddr, uint16_t NumByteToWrite);
   

  private:
    int Set_ODR_When_Enabled(float odr);
    int Set_ODR_When_Disabled(float odr);

    /* Helper classes. */
    DevI2C *_dev_i2c;
    SPI    *_dev_spi;     
    
    /* Configuration */
    uint8_t _address;
    DigitalOut  _cs_pin; 
    InterruptIn _int_pin;    
    SPI_type_t _spi_type;    
    
    uint8_t _is_enabled;
    float _last_odr;
};

#ifdef __cplusplus
 extern "C" {
#endif
uint8_t LPS22HB_io_write( void *handle, uint8_t WriteAddr, uint8_t *pBuffer, uint16_t nBytesToWrite );
uint8_t LPS22HB_io_read( void *handle, uint8_t ReadAddr, uint8_t *pBuffer, uint16_t nBytesToRead );
#ifdef __cplusplus
  }
#endif

#endif
