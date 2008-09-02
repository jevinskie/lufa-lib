/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com
*/

/*
  Copyright 2008  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, and distribute this software
  and its documentation for any purpose and without fee is hereby
  granted, provided that the above copyright notice appear in all
  copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/
#include "USBMode.h"
#if defined(USB_CAN_BE_HOST)

#include "Pipe.h"

uint8_t USB_ControlPipeSize = PIPE_CONTROLPIPE_DEFAULT_SIZE;

void Pipe_ConfigurePipe_P(const uint8_t  PipeNum,
                          const uint16_t PipeSize,
                          const uint8_t  UPCFG0Xdata,
                          const uint8_t  UPCFG1Xdata)
{
	Pipe_SelectPipe(PipeNum);
	Pipe_EnablePipe();
	
	UPCFG0X = UPCFG0Xdata;
	UPCFG1X = ((UPCFG1X & (1 << ALLOC)) | UPCFG1Xdata | Pipe_BytesToEPSizeMask(PipeSize));
	UPCFG2X = 0;
	
	Pipe_AllocateMemory();
}

void Pipe_ClearPipes(void)
{
	UPINT = 0;

	for (uint8_t PNum = 0; PNum < PIPE_MAX_PIPES; PNum++)
	{
		Pipe_ResetPipe(PNum);
		Pipe_SelectPipe(PNum);
		UPIENX = 0;
		UPINTX = 0;
		Pipe_ClearError();
		Pipe_ClearErrorFlags();
		Pipe_DeallocateMemory();
		Pipe_DisablePipe();
	}
}

uint8_t Pipe_Write_Stream_LE(const void* Data, uint16_t Length)
{
	uint8_t* DataStream = (uint8_t*)Data;
	uint16_t PacketLen = 0;
	
	while (Length)
	{
		if (!(Pipe_ReadWriteAllowed()))
		{
			Pipe_ClearCurrentBank();
				
			while (!(Pipe_ReadWriteAllowed()))
			{
				if (!(USB_IsConnected))
				  return PIPE_RWSTREAM_ERROR_DeviceDisconnected;
				else if (Pipe_IsStalled())
				  return PIPE_RWSTREAM_ERROR_PipeStalled;
			}
			
			PacketLen = 0;
		}

		Pipe_Write_Byte(*(DataStream++));
		Length--;
		
		if (!(USB_IsConnected))
		  return PIPE_RWSTREAM_ERROR_DeviceDisconnected;
		else if (Pipe_IsStalled())
		  return PIPE_RWSTREAM_ERROR_PipeStalled;
	}

	return PIPE_RWSTREAM_ERROR_NoError;
}

uint8_t Pipe_Write_Stream_BE(const void* Data, uint16_t Length)
{
	uint8_t* DataStream = (uint8_t*)(Data + Length - 1);
	
	while (Length)
	{
		if (!(Pipe_ReadWriteAllowed()))
		{
			Pipe_ClearCurrentBank();
				
			while (!(Pipe_ReadWriteAllowed()))
			{
				if (!(USB_IsConnected))
				  return PIPE_RWSTREAM_ERROR_DeviceDisconnected;
				else if (Pipe_IsStalled())
				  return PIPE_RWSTREAM_ERROR_PipeStalled;
			}
		}

		Pipe_Write_Byte(*(DataStream--));
		Length--;
		
		if (!(USB_IsConnected))
		  return PIPE_RWSTREAM_ERROR_DeviceDisconnected;
		else if (Pipe_IsStalled())
		  return PIPE_RWSTREAM_ERROR_PipeStalled;
	}

	return PIPE_RWSTREAM_ERROR_NoError;
}

uint8_t Pipe_Read_Stream_LE(void* Buffer, uint16_t Length)
{
	uint8_t* DataStream = (uint8_t*)Buffer;
	
	while (Length)
	{
		if (!(Pipe_ReadWriteAllowed()))
		{
			Pipe_ClearCurrentBank();
				
			while (!(Pipe_ReadWriteAllowed()))
			{
				if (!(USB_IsConnected))
				  return PIPE_RWSTREAM_ERROR_DeviceDisconnected;
				else if (Pipe_IsStalled())
				  return PIPE_RWSTREAM_ERROR_PipeStalled;
			}
		}

		*(DataStream++) = Pipe_Read_Byte();
		Length--;
		
		if (!(USB_IsConnected))
		  return PIPE_RWSTREAM_ERROR_DeviceDisconnected;
		else if (Pipe_IsStalled())
		  return PIPE_RWSTREAM_ERROR_PipeStalled;
	}

	return PIPE_RWSTREAM_ERROR_NoError;
}

uint8_t Pipe_Read_Stream_BE(void* Buffer, uint16_t Length)
{
	uint8_t* DataStream = (uint8_t*)(Buffer + Length - 1);
	
	while (Length)
	{
		if (!(Pipe_ReadWriteAllowed()))
		{
			Pipe_ClearCurrentBank();
				
			while (!(Pipe_ReadWriteAllowed()))
			{
				if (!(USB_IsConnected))
				  return PIPE_RWSTREAM_ERROR_DeviceDisconnected;
				else if (Pipe_IsStalled())
				  return PIPE_RWSTREAM_ERROR_PipeStalled;
			}
		}

		*(DataStream--) = Pipe_Read_Byte();
		Length--;
		
		if (!(USB_IsConnected))
		  return PIPE_RWSTREAM_ERROR_DeviceDisconnected;
		else if (Pipe_IsStalled())
		  return PIPE_RWSTREAM_ERROR_PipeStalled;
	}
	
	return PIPE_RWSTREAM_ERROR_NoError;
}

#endif
