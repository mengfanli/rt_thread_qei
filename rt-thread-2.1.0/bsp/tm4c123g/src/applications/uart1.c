#include "uart1.h"


#define FR_HEAD1    0xfe   // ֡ͷ1
#define FR_HEAD2    0x80  // ֡ͷ2
uint8_t  receive_buffer[8]  = {0};
uint8_t  command=0;
volatile bool data_complete_flag = false;

static void UART1IntHandler(void)
{
    uint32_t ui32Status;
    ui32Status = UARTIntStatus(UART1_BASE, true);
    UARTIntClear(UART1_BASE, ui32Status);

    static uint16_t now_i = 8;

    // Loop while there are no characters in the receive FIFO.
    // UARTCharPutNonBlocking(UART1_BASE,UARTCharGetNonBlocking(UART1_BASE));
    while(UARTCharsAvail(UART1_BASE))
    {
        //UARTCharPutNonBlocking(UART1_BASE,UARTCharGetNonBlocking(UART1_BASE));
        receive_buffer[now_i%8] = UARTCharGetNonBlocking(UART1_BASE);
        now_i++;
//      if(i == 3) {
//          if((receive_buffer[0] != FR_HEAD1) || (receive_buffer[2] != FR_HEAD2)) {
//                      i = 0;
//                  }
//      }

        if((receive_buffer[(now_i-1)%8] == FR_HEAD2) && (receive_buffer[(now_i-2)%8] == FR_HEAD1))
        {
            command=receive_buffer[now_i%8];
            data_complete_flag = true;
            break;
        }
    }

    while(UARTCharsAvail(UART1_BASE)) {
        UARTCharGetNonBlocking(UART1_BASE);
    }
}
static void UART1_Configuer(uint32_t baud)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinConfigure(GPIO_PB0_U1RX);
    GPIOPinConfigure(GPIO_PB1_U1TX);
    GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), baud,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));
    UARTFIFOEnable(UART1_BASE);
    // Set recive fifo-level 8 bytes
    UARTFIFOLevelSet(UART1_BASE, UART_FIFO_TX4_8, UART_FIFO_RX4_8);

    UARTIntRegister(UART1_BASE, UART1IntHandler);

    IntEnable(INT_UART1);

    // lowest int-priority
    IntPrioritySet (INT_UART1, 0x08);

    // Enable recive��recive timeout interrupt
    UARTIntEnable(UART1_BASE, UART_INT_RX);
}
void uart1_thread_entry(void *param)
{
    UART1_Configuer(115200);
    while(data_complete_flag)
    {

    }
}
void uart1_app_init(void)
{
    rt_thread_t tid;
    tid = rt_thread_create("srl1_t",
            uart1_thread_entry, RT_NULL,
            RT_THREAD_STACK_SIZE_NORMAL, RT_THREAD_PRIORITY_NORMAL, RT_THREAD_TICK_NORMAL);
    if (tid != RT_NULL)
        rt_thread_startup(tid);
}
