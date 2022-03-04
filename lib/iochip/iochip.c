/*
 * Desctiption: Functions that control the W65C22 Versatile Interface Adapter
 * 
 * Author: therealjacinto
 * Date:   2022-03-03
 */

#define PORTA ((volatile unsigned char*)0x6001)  /* Top 3 bits control LCD */
#define PORTB ((volatile unsigned char*)0x6000)  /* 8 Data lines of LCD */

#define DDRA  ((volatile unsigned char*)0x6003)  /* Data direction register */
                                                 /* for Port A */
#define DDRB  ((volatile unsigned char*)0x6002)  /* Data direction register */
                                                 /* for Port B */

/*
 * Function:  set_data_direction_port_a 
 * ------------------------------------
 * Sets the data direction of each pin on Port A, 0 being input and 1 being
 * output.
 *
 *  output_pin_mask: Binary representation of each pin as input or output. For
 *                   example: 255 (11111111) for all bins as output.
 */
void set_data_direction_port_a(unsigned char output_pin_mask) {
    *DDRA = output_pin_mask;
}

/*
 * Function:  set_data_direction_port_b 
 * ------------------------------------
 * Sets the data direction of each pin on Port B, 0 being input and 1 being
 * output.
 *
 *  output_pin_mask: Binary representation of each pin as input or output. For
 *                   example: 255 (11111111) for all bins as output.
 */
void set_data_direction_port_b(unsigned char output_pin_mask) {
    *DDRB = output_pin_mask;
}

/*
 * Function:  set_data_port_a 
 * --------------------------
 * Sets the data for address PORTA.
 *
 *  value: data to set on Port A
 */
void set_data_port_a(unsigned char value) {
    *PORTA = value;
}

/*
 * Function:  set_data_port_b 
 * --------------------------
 * Sets the data for address PORTB.
 *
 *  value: data to set on Port B
 */
void set_data_port_b(unsigned char value) {
    *PORTB = value;
}

/*
 * Function:  get_data_port_b 
 * --------------------------
 * Gets the data from address PORTB.
 *
 *  returns: data at Port B
 */
unsigned char get_data_port_b()
{
    return *PORTB;
}
