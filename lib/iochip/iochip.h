/*
 * Desctiption: Functions that control the W65C22 Versatile Interface Adapter
 * 
 * Author: therealjacinto
 * Date:   2022-03-03
 */

#define PORTA "$6001"  /* Top 3 bits control LCD */
#define PORTB "$6000"  /* 8 Data lines of LCD */
#define DDRA  "$6003"  /* Data direction register for Port A */
#define DDRB  "$6002"  /* Data direction register for Port B */

/* Set the Data Direction Register for Port A */
void set_data_direction_port_a(__reg("a") unsigned char output_pin_mask)="\tsta\t"DDRA;

/* Set the Data Direction Register for Port B */
void set_data_direction_port_b(__reg("a") unsigned char output_pin_mask)="\tsta\t"DDRB;

/* Set data to Port A */
void set_data_port_a(__reg("a") unsigned char value)="\tsta\t"PORTA;

/* Set data to Port B */
void set_data_port_b(__reg("a") unsigned char value)="\tsta\t"PORTB;

/* Get data from Port B */
unsigned char get_data_port_b()="\tlda\t"PORTB;
