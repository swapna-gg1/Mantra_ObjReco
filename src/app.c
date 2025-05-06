/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It 
    implements the logic of the application's state machine and it may call 
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "app.h"
// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************
//#define DEBUG 1


#define MAX_IMAGE_BUFFER 270000 //RGB 300x300
#define MAX_IMAGE_BUFFER_HEADER 270054

//uint8_t __attribute__ ((section(".region_nocache"), aligned (32))) bmpOrig[MAX_IMAGE_BUFFER] = { 0 };
uint8_t bmpOrig[MAX_IMAGE_BUFFER_HEADER] = { 0 };
uint8_t  fb0[MAX_IMAGE_BUFFER] = { 0 };
uint32_t width ;
uint32_t height ;
uint32_t testbytes_num = 400;
void _readBMP(uint8_t *buffer, long n_bytes);
// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.
    
    Application strings and buffers are be defined outside this structure.
*/

APP_DATA appData USB_ALIGN;


// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

void _readBMP(uint8_t *buffer, long n_bytes)
{
    uint16_t imgType;
    size_t nBytes;
    uint8_t *ObufferPtr = buffer;
    size_t x, y=0;
    /*Reference: https://en.wikipedia.org/wiki/BMP_file_format*/
    /* First 2 bytes should read 'BM' for bmp file format */
    imgType = (*buffer <<8) | (*(++buffer));
    if(imgType != 0x424D ) /* BM in hex */
    {
        printf("error: Not a BMP pic format\r\n");
    }
    else
    {
        /* 19 through 26 bytes of the header has the W and H info.
         * We have already read first 2 bytes */
        width = *(buffer+=17) | *(++buffer)<<8 | *(++buffer)<<16 | *(++buffer)<<24 ;
        height = *(++buffer) | *(++buffer)<<8 | *(++buffer)<<16 | *(++buffer)<<24 ;
        printf("W:%d H:%d \r\n", width, height);
        
        /* We also need to make sure that the image is 24-bit RGB format which is 
         * the 28th bit in the header */
        if(*(buffer+= 3) == 24)
        {        
            /* First 54 bytes of the image are BMP headers. The image data starts from 54th byte
             * onwards.  */
            nBytes = width*height*3;
            if(nBytes != ( n_bytes-54))
            {
                printf("Not sure why there are additional bytes in the pic. \r\n"
                        "This may not work!\r\n");
            }
            ObufferPtr = ObufferPtr+54;
                  
            int rowStride = (width * 3 + 3) & ~3; // Each row is padded to a multiple of 4 bytes
            printf("RowStride:%d\r\n", rowStride);
            for ( y = 0; y < height; y++) 
            {
                for ( x = 0; x < width; x++) 
                {
                    int bmpIndex = y * rowStride + x * 3; // 3 bytes per pixel (BGR)
                    int bgrIndex = (y * width + x) * 3;  // BGR in framebuffer
                    // Copy BGR from BMP directly to framebuffer
                    fb0[bgrIndex + 0] = ObufferPtr[bmpIndex];      // Blue
                    fb0[bgrIndex + 1] = ObufferPtr[bmpIndex + 1];  // Green
                    fb0[bgrIndex + 2] = ObufferPtr[bmpIndex + 2];  // Red                    
                }
            }
           
            /* Now appData.fbPtr is raw ARGB(8888) image buffer of bmp  */
            printf("successfully copied pic to fb0\r\n");
        }
        else
        {
             printf("error: Not 24 bit RGB format\r\n");
        }
   

    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_BUS_ENABLE;
    appData.deviceIsConnected = false;
    

}

USB_HOST_EVENT_RESPONSE APP_USBHostEventHandler (USB_HOST_EVENT event, void * eventData, uintptr_t context)
{
    switch (event)
    {
        case USB_HOST_EVENT_DEVICE_UNSUPPORTED:
            break;
        default:
            break;
                    
    }
    
    return(USB_HOST_EVENT_RESPONSE_NONE);
}

void APP_SYSFSEventHandler(SYS_FS_EVENT event, void * eventData, uintptr_t context)
{
    switch(event)
    {
        case SYS_FS_EVENT_MOUNT:
            appData.deviceIsConnected = true;
            
            break;
            
        case SYS_FS_EVENT_UNMOUNT:
            appData.deviceIsConnected = false;
            LED1_Off();
            break;
            
        default:
            break;
    }
}
/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{
    SYS_FS_ERROR err;
    size_t bytes_read;
    switch(appData.state)
    {
        case APP_STATE_BUS_ENABLE:
                      
           /* Set the event handler and enable the bus */
            SYS_FS_EventHandlerSet((void *)APP_SYSFSEventHandler, (uintptr_t)NULL);
            USB_HOST_EventHandlerSet(APP_USBHostEventHandler, 0);
            USB_HOST_BusEnable(USB_HOST_BUS_ALL);
            appData.state = APP_STATE_WAIT_FOR_BUS_ENABLE_COMPLETE;
            break;
            
        case APP_STATE_WAIT_FOR_BUS_ENABLE_COMPLETE:
            if(USB_HOST_BusIsEnabled(USB_HOST_BUS_ALL))
            {
                appData.state = APP_STATE_WAIT_FOR_DEVICE_ATTACH;
            }
            break;
       
        case APP_STATE_WAIT_FOR_DEVICE_ATTACH:

            /* Wait for device attach. The state machine will move
             * to the next state when the attach event
             * is received.  */
            if(appData.deviceIsConnected)
            {
                appData.state = APP_STATE_DEVICE_CONNECTED;
            }

            break;

        case APP_STATE_DEVICE_CONNECTED:

            /* Device was connected. We can try mounting the disk */
            appData.state = APP_STATE_OPEN_FILE;
            break;

        case APP_STATE_OPEN_FILE:

            /* Try opening the file for append */
            appData.fileHandle = SYS_FS_FileOpen("/mnt/myDrive1/image.bmp", (SYS_FS_FILE_OPEN_READ));
            if(appData.fileHandle == SYS_FS_HANDLE_INVALID)
            {
                /* Could not open the file. Error out*/
                err = SYS_FS_Error();
                printf("error:%d \r\n", err);   
                /* Could not open the file. Error out*/
                appData.state = APP_STATE_ERROR;

            }
            else
            {
                /* File opened successfully.Get file size */
                appData.state = APP_STATE_GET_RFILE_SIZE;

            }
            break;

        case APP_STATE_GET_RFILE_SIZE:

            appData.fileSize = SYS_FS_FileSize(appData.fileHandle);
            if(appData.fileSize == -1)
            {
                /* Could not get file size. Error out*/
                err = SYS_FS_Error();
                printf("error:%d \r\n", err);                  
                appData.state = APP_STATE_ERROR;
            }
            else
            {
                printf("File Size = %d \r\n", appData.fileSize);
                /* File size read successfully. Read bmp file */
                appData.state = APP_STATE_READ_FROM_FILE;
            }
                    
            break;

        case APP_STATE_READ_FROM_FILE:            
            /* Try reading from file */
            bytes_read = SYS_FS_FileRead(appData.fileHandle, bmpOrig, appData.fileSize );
#if defined(DEBUG)        
            printf("Number of bytes read: %d\r\n", bytes_read);
            /* Lets just print the BMP header bytes_read*/
            int i;
            for(i=0;i< 54; i++)
            {
                printf("bmpOrig[%d]: %d \r\n", i, bmpOrig[i]);
            }
#endif
            if(bytes_read == -1)
            {
                /* Read was not successful. Close the file
                 * and error out.*/
                err = SYS_FS_Error();
                printf("error:%d \r\n", err);                  
                SYS_FS_FileClose(appData.fileHandle);
                appData.state = APP_STATE_ERROR;
            }
            else
            {
                appData.state = APP_STATE_OBTAIN_IMAGE;
                      
            }
                     
            

            break;
        case APP_STATE_OBTAIN_IMAGE:
            /* Close the read file handle first */
            SYS_FS_FileClose(appData.fileHandle);
            
            /* Lets copy the file from bmp to fb0 */             
            _readBMP((uint8_t*)bmpOrig, appData.fileSize ); 
#if defined(DEBUG)        
            printf("Printing first %d bytes \r\n", testbytes_num);
            /* Lets just print the BMP header bytes_read*/
            int j;
            for(j=0;j< testbytes_num; j++)
            {
                printf("fb0[%d]: %d \r\n", j, fb0[j]);
            }
#endif            
            appData.state = APP_APPLY_MLMODEL_ON_IMAGE;
            
            break;
        case APP_APPLY_MLMODEL_ON_IMAGE:
            printf("Applying ML Model on image\r\n");
            /* Indicate User that File operation has been completed */
            LED1_On(); 
            
            appData.state = APP_STATE_IDLE;
            break;
        case APP_STATE_IDLE:

            /* The application comes here when the demo has completed
             * successfully. Provide LED indication. Wait for device detach
             * and if detached, wait for attach. */

            if(appData.deviceIsConnected == false)
            {
                appData.state = APP_STATE_WAIT_FOR_DEVICE_ATTACH;
            }
            break;

        case APP_STATE_ERROR:

            /* The application comes here when the demo
             * has failed. Provide LED indication .*/

            if(SYS_FS_Unmount("/mnt/myDrive1") != 0)
            {
                /* The disk could not be un mounted. Try
                 * un mounting again untill success. */

                appData.state = APP_STATE_ERROR;
            }
            else
            {
                /* UnMount was successful. Wait for device attach */
                appData.state =  APP_STATE_WAIT_FOR_DEVICE_ATTACH;
                appData.deviceIsConnected = false; 

            }
            break;

        default:
            break;

    }
}
 

/*******************************************************************************
 End of File
 */
