//header files for both mode1 and mode2
#include <avr/io.h>
#include <util/delay.h>

//lcd define section
#define MrLCDsCrib PORTA
#define DataDir_MrLCDsCrib DDRA
#define MrLCDsControl PORTD
#define DataDir_MrLCDsControl DDRD
#define LightSwitch 6   //en pin 3
#define ReadWrite 5 	//r/w pin 2
#define BiPolarMood 4  //rs pin 1

//keypad define section
#define keypad_direc_reg DDRB      			//KEYPAD PORT B
#define keypad_port_contl PORTB				//keypad control PORTB
#define keypad_port_value PINB     			//Keypad port value is given on any pin of PORTB

//lcd prototypes
void Check_IF_MrLCD_isBusy(void);			
void Peek_A_Boo(void);
void Send_A_Command(unsigned char command);
void Send_A_Character(unsigned char character);
void Send_A_String(char *String);

void Cursorfunc(void);  //a special prototype designed for taking cursor forward in mode 2 

//keypad prototypes
void KeypadScan(void);						//keypad functions

//buzzer prototypes
void dah(void);
void dit(void); 

//mode 1 protoptypes for threshold count
unsigned long press_limit(void);
void countdown(void);
unsigned long press_limitforCzero(void);
unsigned long press_limitforCsix(void);

//variables for cursor function "Cursorfunc"
int cursor=1;
unsigned char d=0xC0;

//variables for mode1
unsigned long presslimit=0;                 //counter
unsigned char c=0x80; 						//place cursor at 1st row 1st position





//common
int main(void)
{  
	
	DataDir_MrLCDsControl |= 1<<LightSwitch | 1<<ReadWrite | 1<<BiPolarMood; //setting LCD to enable(light on) | read | accept data
	_delay_ms(15);
	Send_A_Command(0x01);                   //Clear Screen 0x01 = 00000001
	_delay_ms(2);
	Send_A_Command(0x38); 					//to define the lcd is 16X2
	_delay_us(50);
	Send_A_Command(0b00001110);				//cursor placed
	_delay_us(50);
	
	DDRD &= ~(1<<PIND7);     //D7 as morse code                       		 
	PORTD |= 1<<PIND7;                                 	

	DDRC &= ~(1<<PINC0);       //C0 as space button                     		 
	PORTC |= 1<<PINC0;

	DDRC &= ~(1<<PINC6);       //C6 as delete button                     		 
	PORTC |= 1<<PINC6;
	
	
	
	DDRD &= ~(1<<PIND2); //D2 as switch	
	PORTD |= (1<<PIND2);  
	
	DDRD |= (1<<PIND3); //D3 as buzzer
	
	Send_A_String("MORSE CODE");
	_delay_ms(400);//to test brightness
	Send_A_Command(0x01); //clr screen
	_delay_ms(2);
	
	Send_A_String("SELECT MODE");    //change this
	Send_A_Command(0xC0);
	_delay_ms(2);
	Send_A_String("R-mode2 L-mode1");		//confirm this
	_delay_ms(400);//to test brightness
	Send_A_Command(0x01); //clr screen
	_delay_ms(2);
	Send_A_Command(0x80);
	_delay_ms(2);
	
	
	
	while(1)
	{
		while(bit_is_set(PIND,1))
		{
			Send_A_String("Text to Morse"); 				
			_delay_ms(400);//to test brightness
			Send_A_Command(0x01); //clr screen
			_delay_ms(2);
			
			Send_A_String("Press a Key");
			_delay_ms(400);//to test brightness
			Send_A_Command(0x01); //clr screen
			_delay_ms(2);
			
			while(1)
			{  
				keypad_direc_reg = 0b00001111;    		// setting column(d7 d6 d5 d4) as input and rows(d3 d2 d1 d0) as output
				keypad_port_contl = 0b11110000; 		// setting column 5V row 0V
				KeypadScan();
				
				/*if(bit_is_clear(PIND,1))
				{
					_delay_ms(20);
					Send_A_Command(0x01);
					_delay_ms(2);
					break; //need to check where it goes //enter mode1
				}*/
				
			}	
			
		}
		
		
		while(bit_is_clear(PIND,1))
		{
				Send_A_String("Morsecode to txt"); //a
				_delay_ms(400);//to test brightness
				Send_A_Command(0x01); //clr a
				_delay_ms(2);
			 
				while(1)                                   		 
				{	
					
					/*if(bit_is_set(PIND,1))
					{
					_delay_ms(20);
					Send_A_Command(0x01);
					_delay_ms(2);
					break; //need to check where it goes //enter mode 2
					}*/
					
					if(bit_is_clear(PIND,7))  									//distinction b/w 1st dash and dot            			  
					{
						presslimit=press_limit(); 
						
						if(presslimit>90000) 									//dash 1                		                	 
						{
							presslimit=0;
							countdown();
							
							if(bit_is_clear(PIND,7))							//distinction b/w 2nd dash and dot
							{   
								presslimit=press_limit();
							 
								if(presslimit>90000) 							//dash 2
								{
									presslimit=0;
									countdown();
									
									if(bit_is_clear(PIND,7))					//distinction b/w 3rd dash and dot branch M
									{
										presslimit=press_limit();
										
										if(presslimit>90000)				//dash 3
										{
											Send_A_Command(c);
											_delay_ms(2);
											Send_A_String("O");  //display O //deadend
											c++;
											
										}
										else								//dot 3
										{	
											presslimit=0;
											countdown();
											
											if(bit_is_clear(PIND,7))			//distinction b/w 4th dash and dot branch G
											{	
												presslimit=press_limit();
												
												if(presslimit>90000)			//dash 4
												{	
													Send_A_Command(c);
													_delay_ms(2);
													Send_A_String("Q");
													c++;//display Q //deadend
												}
												else							//dot 4
												{
													Send_A_Command(c);
													_delay_ms(2);
													Send_A_String("Z");//display Z //deadend
													c++;
												}
											}
											else
											{
												Send_A_Command(c);
												_delay_ms(2);
												Send_A_String("G");//display G
												c++;
											}
										}
									}
									else
									{
										Send_A_Command(c);
										_delay_ms(2);
										Send_A_String("M");//display M
										c++;
									}
									
								}
								
								else											//dot 2 entering branch N
								{	
									presslimit=0;
									countdown();
									
									if(bit_is_clear(PIND,7))				//distinction between 3rd dash and dot branch N
									{
										presslimit=press_limit();
										
										if(presslimit>90000)				//dash 3
										{
											
											presslimit=0;
											countdown();
											
											if(bit_is_clear(PIND,7))				//distinction between 4th dash and dot branch K
											{
												presslimit=press_limit();
												
												if(presslimit>90000)
												{
													Send_A_Command(c);
													_delay_ms(2);
													Send_A_String("Y");//display Y //deadend
													c++;
												}
												else
												{
													Send_A_Command(c);
													_delay_ms(2);
													Send_A_String("C");//display C //deadend
													c++;
												}
												
											}
											else
											{
												Send_A_Command(c);
												_delay_ms(2);
												Send_A_String("K");//display K
												c++;
											}
										}
										
										
										else								//dot 3
										{
											presslimit=0;
											countdown();
											
											if(bit_is_clear(PIND,7)) //distinction between 4th dash and dot branch D
											{
												presslimit=press_limit();
												
												if(presslimit>90000)	//dash 4
												{
													Send_A_Command(c);
													_delay_ms(2);
													Send_A_String("X");//display X deadend
													c++;
												}
												else					//dot 4
												{
													Send_A_Command(c);
													_delay_ms(2);
													Send_A_String("B");//display B deadend
													c++;
												}
												
											}
											
											else
											{
												Send_A_Command(c);
												_delay_ms(2);
												Send_A_String("D");//display D
												c++;
											}
										}
									}
									else
									{
										Send_A_Command(c);
										_delay_ms(2);
										Send_A_String("N");//display N
										c++;
									}
									
								}    
							}
							
							else												//end
							{
								Send_A_Command(c);
								_delay_ms(2);
								Send_A_String("T");//display T
								c++;
							}
							
						}
						
						
						
						//--------------------------------------------------
						
						else                               		 //dot 1
						{
							presslimit=0;
							countdown();
							
							
							if(bit_is_clear(PIND,7))    		//distinction between dash 2 and dot 2     
							{
								presslimit=press_limit();
							 
								if(presslimit>90000)            		 //dash 2
								{
									
									presslimit=0;
									countdown();
									
									if(bit_is_clear(PIND,7))    		//distinction between dash 2 and dot 2     branch A
									{
										presslimit=press_limit();
										
										if(presslimit>90000)
										{
											presslimit=0;
											countdown();
											
											if(bit_is_clear(PIND,7)) //distinction between dash 3 and dot 3 branch W
											{
												presslimit=press_limit();
												if(presslimit>90000)
												{
													Send_A_Command(c);
													_delay_ms(2);
													Send_A_String("J"); //display J deadend
													c++;
												}
												else
												{ 	
													Send_A_Command(c);
													_delay_ms(2);
													Send_A_String("P"); // display P deadend
													c++;
												}
											}
											else
											{
												Send_A_Command(c);
												_delay_ms(2);
												Send_A_String("W");//display W
												c++;
										
											}
										}
										
										else
										{
											presslimit=0;
											countdown();
											if(bit_is_clear(PIND,7))							//distinction between dash and dot for R branch 
											{
												presslimit=press_limit();
												if(presslimit<90000) //******
												{
													Send_A_Command(c);
													_delay_ms(2);
													Send_A_String("L");// display L deadend
													c++;
												}
											}
											else
											{
												Send_A_Command(c);
												_delay_ms(2);
												Send_A_String("R");//display R
												c++;
											}
										}
									}
									else
									{
										Send_A_Command(c);
										_delay_ms(2);
										Send_A_String("A");//display A 
										c++;
									}
								}
								
								
								
								
								else
								{
									presslimit=0;
									countdown();
									if(bit_is_clear(PIND,7))							//distinction between dash and dot for I branch
									{
										presslimit=press_limit();
										if(presslimit>90000)
										{
											presslimit=0;
											countdown();
											if(bit_is_clear(PIND,7))                     //distinction between dash and dot for branch U  
											{
												presslimit=press_limit();
												if(presslimit<90000) //********
												{
													Send_A_Command(c);
													_delay_ms(2);
													Send_A_String("F");//display F deadend
													c++;
												}
												
											}
											else
											{
												Send_A_Command(c);
												_delay_ms(2);
												Send_A_String("U");//display U
												c++;
											}
										}
										else
										{
											presslimit=0;
											countdown();
											if(bit_is_clear(PIND,7))                     //distinction between dash and dot for branch S
											{
												presslimit=press_limit();
												if(presslimit>90000)
												{
													Send_A_Command(c);
													_delay_ms(2);
													Send_A_String("V");//display V deadend
													c++;
												}
												else
												{
													Send_A_Command(c);
													_delay_ms(2);
													Send_A_String("H");//display H deadend
													c++;
												}
											}
											else
											{
												Send_A_Command(c);
												_delay_ms(2);
												Send_A_String("S");//display S
												c++;
											}
										}
										
									}
									
									else
									{
										Send_A_Command(c);
										_delay_ms(2);
										Send_A_String("I");//display I 
										c++;
									}
								}    
							}
							
							else
							{
								Send_A_Command(c);
								_delay_ms(2);
								Send_A_String("E");//display E 
								c++;
							} 
						}
					}
					else if(bit_is_clear(PINC,0)) 
					{
						
						presslimit=press_limitforCzero();
						if(presslimit>30000)
						{
							Send_A_String(" ");
							presslimit=0;
							c++;
						}
						
					}
					while(bit_is_clear(PINC,6))
					{
						presslimit=press_limitforCsix();
						if(presslimit>30000)
						{
						Send_A_Command(0x10);
						_delay_ms(2);
						Send_A_String(" ");
						c--;
						presslimit=0;
						}
					}
				 
				}
		}	
	}
}



void KeypadScan()
{ 
	while(bit_is_clear(PIND,2))
    {
	
	
	if(keypad_port_value == 0b11110000) return;
	
	_delay_ms(10);
	
	uint8_t keycode = keypad_port_value; 
	
	keypad_direc_reg ^= 0b11111111;  	//row as input
	keypad_port_contl ^= 0b11111111; 	//row  as 5v
	
	_delay_ms(20); 					//you need to work on this
	
	keycode |= keypad_port_value; 		//old + new value
	
		switch(keycode)											//the matching game
       {
			case 0b11101110:Send_A_String("P");
							dit();
							dah();
							dah();
							dit();
							cursor++;
							Cursorfunc();
							break;
			case 0b11011110:Send_A_String("Q");
							dah();
							dah();
							dit();
							dah();
							cursor++;
							Cursorfunc();
							break;	
			case 0b10111110:Send_A_String("R");
							dit();
							dah();
							dit();
							cursor++;
							Cursorfunc();
							break;
			case 0b01111110:Send_A_String("S");
							dit();
							dit();
							dit();
							cursor++;
							Cursorfunc();
							break;
			case 0b11101101:Send_A_String("T");
							dah();
							cursor++;
							Cursorfunc();
							break;
			case 0b11011101:Send_A_String("U");
							dit();
							dit();
							dah();
							cursor++;
							Cursorfunc();
							break;
			case 0b10111101:Send_A_String("V");
							dit();
							dit();
							dit();
							dah();
							cursor++;
							Cursorfunc();
							break;
			case 0b01111101:Send_A_String("W");
							dit();
							dah();
							dah();
							cursor++;
							Cursorfunc();
							break;  
			case 0b11101011:Send_A_String("X");
							dah();
							dit();
							dit();
							dah();
							cursor++;
							Cursorfunc();
							break;
			case 0b11011011:Send_A_String("Y");
							dah();
							dit();
							dah();
							dah();
							cursor++;
							Cursorfunc();
							break;
			case 0b10111011:Send_A_String("Z");
							dah();
							dah();
							dit();
							dit();
							cursor++;
							Cursorfunc();
							break;
			case 0b01111011:Send_A_String("1");
							dit();
							dah();
							dah();
							dah();
							dah();
							cursor++;
							Cursorfunc();
							break;
			case 0b11100111:Send_A_String("2");
							dit();
							dit();
							dah();
							dah();
							dah();
							cursor++;
							Cursorfunc();
							break;
			case 0b11010111:Send_A_String("3");
							dit();
							dit();
							dit();
							dah();
							dah();
							cursor++;
							Cursorfunc();
							break;
			case 0b10110111:Send_A_String("4");
							dit();
							dit();
							dit();
							dit();
							dah();
							cursor++;
							Cursorfunc();
							break;
			case 0b01110111:Send_A_String(" ");
							dit();
							dit();
							dit();
							dit();
							dit();
							dit();
							dit();
							Cursorfunc();
							break;
					default:break;
		}
	}
	 
	 
	while(bit_is_set(PIND,2))									//should be while, we need to press it again and again
	{  
		_delay_ms(10);											//to avoid debounce of switch
		
		if(keypad_port_value == 0b11110000) return; 			// no key on keypad is pressed.
		
	    _delay_ms(10);											//to avoid debouncing of keypad button
		
	    uint8_t keycode = keypad_port_value;  					//column value
	    keypad_direc_reg ^= 0b11111111;  						//row as input
	    keypad_port_contl ^= 0b11111111; 						//row  as 5v
		
	    _delay_ms(20);											//you need to work on this
	    
	    keycode |= keypad_port_value; 							// prev column+ new row value
		
		switch(keycode)											//the matching game
       {
			case 0b11101110:Send_A_String("A");
							dit();
							dah();
							cursor++;
							Cursorfunc();
							break;
			case 0b11011110:Send_A_String("B");
							dah();
							dit();
							dit();
							dit();
							cursor++;
							Cursorfunc();
							break;	
			case 0b10111110:Send_A_String("C");
							dah();
							dit();
							dah();
							dit();
							cursor++;
							Cursorfunc();
							break;
			case 0b01111110:Send_A_String("D");
							dah();
							dit();
							dit();
							cursor++;
							Cursorfunc();
							break;
			case 0b11101101:Send_A_String("E");
							dit();
							Cursorfunc();
							break;
			case 0b11011101:Send_A_String("F");
							dit();
							dit();
							dah();
							dit();
							cursor++;
							Cursorfunc();
							break;
			case 0b10111101:Send_A_String("G");
							dah();
							dah();
							dit();
							cursor++;
							Cursorfunc();
							break;
			case 0b01111101:Send_A_String("H");
							dit();
							dit();
							dit();
							dit();
							cursor++;
							Cursorfunc();
							break;  
			case 0b11101011:Send_A_String("I");
							dit();
							dit();
							cursor++;
							Cursorfunc();
							break;
			case 0b11011011:Send_A_String("J");
							dit();
							dah();
							dah();
							dah();
							cursor++;
							Cursorfunc();
							break;
			case 0b10111011:Send_A_String("K");
							dah();
							dit();
							dah();
							cursor++;
							Cursorfunc();
							break;
			case 0b01111011:Send_A_String("L");
							dit();
							dah();
							dit();
							dit();
							cursor++;
							Cursorfunc();
							break;
			case 0b11100111:Send_A_String("M");
							dah();
							dah();
							cursor++;
							Cursorfunc();
							break;
			case 0b11010111:Send_A_String("N");
							dah();
							dit();
							cursor++;
							Cursorfunc();
							break;
			case 0b10110111:Send_A_String("O");
							dah();
							dah();
							dah();
							cursor++;
							Cursorfunc();
							break;
			case 0b01110111:Send_A_String(" ");
							dit();
							dit();
							dit();
							dit();
							dit();
							dit();
							dit();
							Cursorfunc();
							break;
					default:break;
		}

    }
 
	
}




void dit()
{
	PORTD |= (1<<PIND3); //set bit 5v for buzzer
	_delay_ms(10);
	PORTD &= ~(1<<PIND3);
	_delay_ms(50);
}
void dah()
{
	PORTD |= (1<<PIND3); //set bit 5v for buzzer
	_delay_ms(30);
	PORTD &= ~(1<<PIND3);
	_delay_ms(50);
}

void Cursorfunc()
{

if(cursor>16)
	{	
		Send_A_Command(d);
		_delay_ms(2);
		d++;
	}
}

void Check_IF_MrLCD_isBusy()
{
	DataDir_MrLCDsCrib = 0;
	MrLCDsControl |= 1<<ReadWrite;
	MrLCDsControl &= ~1<<BiPolarMood;

	while (MrLCDsCrib >= 0x80)
	{
		Peek_A_Boo();
	}
	DataDir_MrLCDsCrib = 0xFF; //0xFF means 0b11111111
}

void Peek_A_Boo()
{
	MrLCDsControl |= 1<<LightSwitch;
	asm volatile ("nop");
	asm volatile ("nop");
	MrLCDsControl &= ~1<<LightSwitch;
}

void Send_A_Command(unsigned char command)
{
	Check_IF_MrLCD_isBusy();
	MrLCDsCrib = command;
	MrLCDsControl &= ~ ((1<<ReadWrite)|(1<<BiPolarMood));
	Peek_A_Boo();
	MrLCDsCrib = 0;
}

void Send_A_Character(unsigned char character)
{
	Check_IF_MrLCD_isBusy();
	MrLCDsCrib = character;
	MrLCDsControl &= ~ (1<<ReadWrite);
	MrLCDsControl |= 1<<BiPolarMood;
	Peek_A_Boo();
	MrLCDsCrib = 0;
}

void Send_A_String(char *String)
{
	while(*String > 0)
	{
		Send_A_Character(*String++);
	}
}

unsigned long press_limitforCzero()
{
	unsigned long a=0;
	while(bit_is_clear(PINC,0))
	{     
		a++;
	}
	return a;
}
unsigned long press_limitforCsix()
{
	unsigned long a=0;
	while(bit_is_clear(PINC,6))
	{     
		a++;
	}
	return a;
}

unsigned long press_limit()
{
    unsigned long a=0;
    while(bit_is_clear(PIND,7))
    {     
   	 a++;
    }
    return a;
}

void countdown()
{
unsigned char b=0X31; 
	for( int i=1;i<4;i++)
	{
		/*Send_A_Command(0x01);
		_delay_ms(2);*/
		Send_A_Command(0xC0);
		_delay_ms(2);
		Send_A_Character(b);
		_delay_ms(100);
		b++;
	}
	Send_A_Command(0x80);
	_delay_ms(2);
}


