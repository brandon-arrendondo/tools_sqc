/*
 * Rule: DCL00-C
 * Source: testcases
 * Status: FAIL - Should trigger DCL00-C violation
 */

/*
 * CERT C DCL00-C Fail Case: device_registers.c
 *
 * This case demonstrates violations where hardware device register
 * addresses and bit field definitions are not const-qualified.
 */

#include <stdio.h>
#include <stdint.h>

void memory_mapped_registers(void) {
    /* NON-COMPLIANT: Register base addresses should be const */
    uint32_t GPIO_BASE = 0x40020000;
    uint32_t UART_BASE = 0x40013800;
    uint32_t SPI_BASE = 0x40013000;
    uint32_t I2C_BASE = 0x40005400;
    uint32_t TIMER_BASE = 0x40000000;

    /* NON-COMPLIANT: Register offsets should be const */
    uint32_t GPIO_MODE_OFFSET = 0x00;
    uint32_t GPIO_OUTPUT_OFFSET = 0x14;
    uint32_t GPIO_INPUT_OFFSET = 0x10;
    uint32_t GPIO_PUPD_OFFSET = 0x0C;

    printf("Memory-Mapped Registers:\\n");
    printf("  GPIO Base: 0x%08X\\n", GPIO_BASE);
    printf("  UART Base: 0x%08X\\n", UART_BASE);
    printf("  SPI Base: 0x%08X\\n", SPI_BASE);
    printf("  I2C Base: 0x%08X\\n", I2C_BASE);
    printf("  Timer Base: 0x%08X\\n", TIMER_BASE);

    printf("\\nGPIO Register Offsets:\\n");
    printf("  Mode: 0x%02X\\n", GPIO_MODE_OFFSET);
    printf("  Output: 0x%02X\\n", GPIO_OUTPUT_OFFSET);
    printf("  Input: 0x%02X\\n", GPIO_INPUT_OFFSET);
    printf("  Pull-up/down: 0x%02X\\n", GPIO_PUPD_OFFSET);

    /* Register addresses used for hardware access but never modified */
    uint32_t gpio_mode_reg = GPIO_BASE + GPIO_MODE_OFFSET;
    printf("\\nGPIO Mode Register: 0x%08X\\n", gpio_mode_reg);
}

void bit_field_definitions(void) {
    /* NON-COMPLIANT: Bit field masks should be const */
    uint32_t STATUS_READY_MASK = 0x00000001;
    uint32_t STATUS_BUSY_MASK = 0x00000002;
    uint32_t STATUS_ERROR_MASK = 0x00000004;
    uint32_t STATUS_OVERFLOW_MASK = 0x00000008;
    uint32_t STATUS_UNDERFLOW_MASK = 0x00000010;

    /* NON-COMPLIANT: Bit positions should be const */
    int READY_BIT_POS = 0;
    int BUSY_BIT_POS = 1;
    int ERROR_BIT_POS = 2;
    int OVERFLOW_BIT_POS = 3;
    int UNDERFLOW_BIT_POS = 4;

    /* NON-COMPLIANT: Control register bits should be const */
    uint32_t CTRL_ENABLE_MASK = 0x00000001;
    uint32_t CTRL_RESET_MASK = 0x00000002;
    uint32_t CTRL_INT_ENABLE_MASK = 0x00000004;
    uint32_t CTRL_DMA_ENABLE_MASK = 0x00000008;

    printf("\\nStatus Register Bit Fields:\\n");
    printf("  Ready: bit %d (mask 0x%08X)\\n", READY_BIT_POS, STATUS_READY_MASK);
    printf("  Busy: bit %d (mask 0x%08X)\\n", BUSY_BIT_POS, STATUS_BUSY_MASK);
    printf("  Error: bit %d (mask 0x%08X)\\n", ERROR_BIT_POS, STATUS_ERROR_MASK);
    printf("  Overflow: bit %d (mask 0x%08X)\\n", OVERFLOW_BIT_POS, STATUS_OVERFLOW_MASK);

    printf("\\nControl Register Bit Fields:\\n");
    printf("  Enable: 0x%08X\\n", CTRL_ENABLE_MASK);
    printf("  Reset: 0x%08X\\n", CTRL_RESET_MASK);
    printf("  Interrupt Enable: 0x%08X\\n", CTRL_INT_ENABLE_MASK);
    printf("  DMA Enable: 0x%08X\\n", CTRL_DMA_ENABLE_MASK);

    /* Bit masks used for register manipulation but never modified */
    uint32_t status_reg = 0x00000005;  /* Ready and Error bits set */
    if (status_reg & STATUS_READY_MASK) {
        printf("\\nDevice is ready\\n");
    }
    if (status_reg & STATUS_ERROR_MASK) {
        printf("Error flag is set\\n");
    }
}

void peripheral_configuration(void) {
    /* NON-COMPLIANT: Configuration values should be const */
    uint32_t UART_BAUD_9600 = 0x1D4C;
    uint32_t UART_BAUD_115200 = 0x016D;
    uint32_t UART_8N1_CONFIG = 0x0000;
    uint32_t UART_8E1_CONFIG = 0x0400;
    uint32_t UART_8O1_CONFIG = 0x0600;

    /* NON-COMPLIANT: SPI configuration should be const */
    uint32_t SPI_MODE_0 = 0x00;  /* CPOL=0, CPHA=0 */
    uint32_t SPI_MODE_1 = 0x01;  /* CPOL=0, CPHA=1 */
    uint32_t SPI_MODE_2 = 0x02;  /* CPOL=1, CPHA=0 */
    uint32_t SPI_MODE_3 = 0x03;  /* CPOL=1, CPHA=1 */

    /* NON-COMPLIANT: Clock divider values should be const */
    uint32_t CLK_DIV_2 = 0x00;
    uint32_t CLK_DIV_4 = 0x01;
    uint32_t CLK_DIV_8 = 0x02;
    uint32_t CLK_DIV_16 = 0x03;
    uint32_t CLK_DIV_32 = 0x04;

    printf("\\nUART Configuration:\\n");
    printf("  Baud rates: 9600=0x%04X, 115200=0x%04X\\n", UART_BAUD_9600, UART_BAUD_115200);
    printf("  Data formats: 8N1=0x%04X, 8E1=0x%04X, 8O1=0x%04X\\n",
           UART_8N1_CONFIG, UART_8E1_CONFIG, UART_8O1_CONFIG);

    printf("\\nSPI Configuration:\\n");
    printf("  Modes: 0=%d, 1=%d, 2=%d, 3=%d\\n",
           SPI_MODE_0, SPI_MODE_1, SPI_MODE_2, SPI_MODE_3);

    printf("\\nClock Dividers:\\n");
    printf("  Div2=%d, Div4=%d, Div8=%d, Div16=%d, Div32=%d\\n",
           CLK_DIV_2, CLK_DIV_4, CLK_DIV_8, CLK_DIV_16, CLK_DIV_32);

    /* Configuration values used for peripheral setup but never modified */
    uint32_t uart_config = UART_BAUD_115200 | UART_8N1_CONFIG;
    printf("\\nUART setup: 0x%08X\\n", uart_config);
}

void interrupt_vectors(void) {
    /* NON-COMPLIANT: Interrupt vector numbers should be const */
    int IRQ_TIMER0 = 16;
    int IRQ_TIMER1 = 17;
    int IRQ_UART0 = 18;
    int IRQ_UART1 = 19;
    int IRQ_SPI0 = 20;
    int IRQ_I2C0 = 21;
    int IRQ_GPIO = 22;
    int IRQ_ADC = 23;

    /* NON-COMPLIANT: Priority levels should be const */
    int PRIORITY_HIGHEST = 0;
    int PRIORITY_HIGH = 1;
    int PRIORITY_MEDIUM = 2;
    int PRIORITY_LOW = 3;
    int PRIORITY_LOWEST = 4;

    /* NON-COMPLIANT: Interrupt enable masks should be const */
    uint32_t INT_ENABLE_TIMER = 0x00010000;
    uint32_t INT_ENABLE_UART = 0x00020000;
    uint32_t INT_ENABLE_SPI = 0x00040000;
    uint32_t INT_ENABLE_I2C = 0x00080000;
    uint32_t INT_ENABLE_GPIO = 0x00100000;

    printf("\\nInterrupt Configuration:\\n");
    printf("  Vector numbers: Timer0=%d, UART0=%d, SPI0=%d, GPIO=%d\\n",
           IRQ_TIMER0, IRQ_UART0, IRQ_SPI0, IRQ_GPIO);

    printf("\\nPriority Levels:\\n");
    printf("  Highest=%d, High=%d, Medium=%d, Low=%d, Lowest=%d\\n",
           PRIORITY_HIGHEST, PRIORITY_HIGH, PRIORITY_MEDIUM, PRIORITY_LOW, PRIORITY_LOWEST);

    printf("\\nInterrupt Enable Masks:\\n");
    printf("  Timer: 0x%08X\\n", INT_ENABLE_TIMER);
    printf("  UART: 0x%08X\\n", INT_ENABLE_UART);
    printf("  SPI: 0x%08X\\n", INT_ENABLE_SPI);
    printf("  GPIO: 0x%08X\\n", INT_ENABLE_GPIO);

    /* Interrupt configuration used for system setup but never modified */
    uint32_t enabled_interrupts = INT_ENABLE_TIMER | INT_ENABLE_UART | INT_ENABLE_GPIO;
    printf("\\nEnabled interrupts: 0x%08X\\n", enabled_interrupts);
}

int main(void) {
    /* NON-COMPLIANT: Device identifiers should be const */
    uint32_t DEVICE_ID = 0x12345678;
    uint32_t REVISION_ID = 0x00010001;
    char chip_name[] = "STM32F407";
    char package_type[] = "LQFP100";

    /* NON-COMPLIANT: Memory layout should be const */
    uint32_t FLASH_BASE = 0x08000000;
    uint32_t RAM_BASE = 0x20000000;
    uint32_t PERIPH_BASE = 0x40000000;
    uint32_t FLASH_SIZE = 0x00100000;  /* 1MB */
    uint32_t RAM_SIZE = 0x00020000;    /* 128KB */

    printf("Device Information:\\n");
    printf("  Chip: %s %s\\n", chip_name, package_type);
    printf("  Device ID: 0x%08X\\n", DEVICE_ID);
    printf("  Revision: 0x%08X\\n", REVISION_ID);

    printf("\\nMemory Layout:\\n");
    printf("  Flash: 0x%08X - 0x%08X (%dKB)\\n",
           FLASH_BASE, FLASH_BASE + FLASH_SIZE - 1, FLASH_SIZE / 1024);
    printf("  RAM: 0x%08X - 0x%08X (%dKB)\\n",
           RAM_BASE, RAM_BASE + RAM_SIZE - 1, RAM_SIZE / 1024);
    printf("  Peripherals: 0x%08X\\n", PERIPH_BASE);

    memory_mapped_registers();
    bit_field_definitions();
    peripheral_configuration();
    interrupt_vectors();

    return 0;
}