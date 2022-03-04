/*
 * Desctiption: Functions that control the W65C22 Versatile Interface Adapter
 * 
 * Author: therealjacinto
 * Date:   2022-03-03
 */

/* Set the Data Direction Register for Port A */
void set_data_direction_port_a(unsigned char output_pin_mask);

/* Set the Data Direction Register for Port B */
void set_data_direction_port_b(unsigned char output_pin_mask);

/* Set data to Port A */
void set_data_port_a(unsigned char value);

/* Set data to Port B */
void set_data_port_b(unsigned char value);

/* Get data from Port B */
unsigned char get_data_port_b();
