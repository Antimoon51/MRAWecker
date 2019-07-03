
#include <msp430.h>
#include <stdint.h>

#include "matrix.h"
#include "setup.h"


void array_input();

int k;

uint8_t zahlen[10][3] = { { 0b1111, 0b1001, 0b1111 },    //0
        { 0b1010, 0b1111, 0b1000 },    //1
        { 0b1001, 0b1101, 0b1011 },    //2
        { 0b1001, 0b1011, 0b1111 },    //3
        { 0b0111, 0b0100, 0b1110 },    //4
        { 0b1011, 0b1011, 0b1101 },    //5
        { 0b1111, 0b1011, 0b1101 },    //6
        { 0b0001, 0b0101, 0b1111 },    //7
        { 0b1111, 0b1011, 0b1111 },    //8
        { 0b1011, 0b1011, 0b1111 }    //9

};

uint8_t output[25];


static const uint8_t MCP_ADDRESS = 0x20;    //!> 7-bit Adresse des MCP23008
static const uint8_t MCP_IODIR = 0x00; //!> Adresse des MCP23008-Registers IODIR
static const uint8_t MCP_GPIO = 0x09;   //!> Adresse des MCP23008-Registers GPIO

static char i2c_data[2];             //!> Datenpuffer
static volatile uint8_t i2c_counter; //!> Index der Daten
static uint8_t matrix_buffer[4];         //!> Werte von GPIO beim Multiplexing
static uint8_t current_row;          //!> Aktueller Index von matrix_buffer

/**
 * Initialisierung des I2C-Moduls
 * Initialsierung des MCP23008
 * Löschen des Matrix-Puffers
 */
void matrix_init()
{
    uint8_t row;
    P3SEL |= BIT1 + BIT2;   // I2C-Pins

    UCB0CTL1 |= UCSWRST;                // Soft-Reset des I2C-Moduls
    UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;   // Master, I2C, Synchron
    UCB0BR0 = 2;    // Teiler: 1 MHz /2 = 500 kHz
    UCB0BR1 = 0;

    UCB0CTL1 = UCSSEL_2;
    IE2 |= UCB0TXIE;

    // Löschen der Anzeige
    for (row = 0; row < 4; row++)
    {
        matrix_write_row(row, 0);
    }
    current_row = 0;

    matrix_mcp_write(MCP_IODIR, 0);     // Alle Pins des MCP23008 als Ausgang
}

/**
 * Senden von Daten an den MCP23008
 *
 * @param reg Register, das geschrieben werden soll
 * @param data Daten, die ins Register geschrieben werden sollen
 */
void matrix_mcp_write(uint8_t reg, uint8_t data)
{
    // Warten, bis Modul frei
    while (UCB0STAT & UCBBUSY)
        ;

    // Puffer füllen
    i2c_data[0] = reg;
    i2c_data[1] = data;
    i2c_counter = 0;

    UCB0I2CSA = MCP_ADDRESS; // Slave Adresse setzen
    UCB0CTL1 |= UCTR;        // W-Mode
    UCB0CTL1 |= UCTXSTT;     // Transmition Start
}

/**
 * Schreiben des Buffers einer Spalte
 *
 * @param row Spalte 0...3
 * @param data Daten, Zustand der LEDs [3:0]
 */
void matrix_write_row(uint8_t row, uint8_t data)
{
    matrix_buffer[row] = ((~data) << 4) + (1 << row);
}

/**
 * Funktion, die durch eine Timer aufgerufen werden muss, um das Multiplexing der Matrix druchzuführen
 */
void matrix_update()
{
    matrix_mcp_write(MCP_GPIO, matrix_buffer[current_row]);
    current_row = (current_row + 1) & 0x03;
}

void matrix_clear()
{
    matrix_write_row(0, 0b0000);
    matrix_write_row(1, 0b0000);
    matrix_write_row(2, 0b0000);
    matrix_write_row(3, 0b0000);
}

void matrix_on(){
    matrix_write_row(0, 0b1111);
    matrix_write_row(1, 0b1111);
    matrix_write_row(2, 0b1111);
    matrix_write_row(3, 0b1111);


}


void array_input()
{
    output[0] = 0b0000;
    output[1] = 0b0000;
    output[2] = 0b0000;
    output[3] = 0b0000;
    output[4] = zahlen[(time.hour - (time.hour % 10)) / 10][0];
    output[5] = zahlen[(time.hour - (time.hour % 10)) / 10][1];
    output[6] = zahlen[(time.hour - (time.hour % 10)) / 10][2];
    output[7] = 0b0000;
    output[8] = zahlen[time.hour % 10][0];
    output[9] = zahlen[time.hour % 10][1];
    output[10] = zahlen[time.hour % 10][2];
    output[11] = 0b0000;
    output[12] = 0b1010;
    output[13] = 0b0000;
    output[14] = zahlen[(time.min - (time.min % 10)) / 10][0];
    output[15] = zahlen[(time.min - (time.min % 10)) / 10][1];
    output[16] = zahlen[(time.min - (time.min % 10)) / 10][2];
    output[17] = 0b0000;
    output[18] = zahlen[time.min % 10][0];
    output[19] = zahlen[time.min % 10][1];
    output[20] = zahlen[time.min % 10][2];
//    output[21] = 0b0000;
//    output[22] = 0b0000;
//    output[23] = 0b0000;
//    output[24] = 0b0000;
}

void array_output()
{

    if (k > 20)
    {
        k = 0;
    }

    array_input();

    matrix_write_row(3, output[k + 3]);
    matrix_write_row(2, output[k + 2]);
    matrix_write_row(1, output[k + 1]);
    matrix_write_row(0, output[k]);
    k++;

}


/**
 * I2C-Transmit-Interrupt
 */
#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCIAB0TX_ISR(void)
{
    if (i2c_counter < 2)
    {
        // Nächtes Byte senden
        UCB0TXBUF = i2c_data[i2c_counter];
        i2c_counter++;
    }
    else
    {
        // Alles Bytes versendet
        UCB0CTL1 |= UCTXSTP;    // Stop-Senden
        IFG2 &= ~UCB0TXIFG;
    }
}
