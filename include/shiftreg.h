//==========shift register function===========
void transmit_data(unsigned char data, unsigned char data2)
{
	int i;
	for(i = 0; i < 8; ++i)
	{
		PORTB = 0x08;
		PORTC = 0x08;
		PORTB |= ((data >> i) & 0x01);
		PORTC |= ((data2 >> i) & 0x01);
		PORTB |= 0x02;
		PORTC |= 0x02;
	}
	PORTB |= 0x04;
	PORTC |= 0x04;
	PORTB = 0x00;
	PORTC = 0x00;
}