#define F_CPU 16000000UL     
                            
#include <Arduino.h>          
#include <LiquidCrystal.h>    
#include <avr/io.h>            
#include <util/delay.h>       
#include <stdlib.h>
#include <string.h>
#ifndef BIT_MATH_H
#define BIT_MATH_H

#define SET_BIT(REG, BIT) ((REG) |= (1 << (BIT)))  
#define CLR_BIT(REG, BIT) ((REG) &= ~(1 << (BIT))) 
#define TOG_BIT(REG, BIT) ((REG) ^= (1 << (BIT)))  
#define GET_BIT(REG, BIT) (((REG) >> (BIT)) & 1)   
#endif // BIT_MATH_H

char keypad[4][4] = {
    {'A', 'E', 'I', 'M'},
    {'1', 'F', 'J', '*'},
    {'<', '2', 'K', '-'},
    {'C', 'Y', 'N', '+'}
};

LiquidCrystal lcd(PIN_PA1, PIN_PA2, PIN_PA3, PIN_PA4, PIN_PA5, PIN_PA6);


#define RED_LED    PC0
#define GREEN_LED  PC1
#define BLUE_LED   PC2
#define BUZZER_PIN PC5

#define KEY_DOT     '*'   
#define KEY_DASH    '-'   
#define KEY_DECODE  '+'   
#define KEY_SPACE   '1'   
#define KEY_CLEAR   '<'   

#define MORSE_MAX 7   
#define MSG_MAX   16  
#define TEXT_MAX  12  

String morseBuf = "";   
String message  = "";   
String textBuf  = "";   

uint8_t current_mode = 0; 

void leds_init(void)
{
    SET_BIT(DDRC, RED_LED);   
    SET_BIT(DDRC, GREEN_LED); 
    SET_BIT(DDRC, BLUE_LED);

    CLR_BIT(PORTC, RED_LED);  
    CLR_BIT(PORTC, GREEN_LED);
    CLR_BIT(PORTC, BLUE_LED);

    DDRC  |= (1 << BUZZER_PIN); 
    PORTC &= ~(1 << BUZZER_PIN); 
}

void buzzer_on(void)
{
    SET_BIT(PORTC, BUZZER_PIN);
}

void leds_off(void)
{
    CLR_BIT(PORTC, RED_LED);
    CLR_BIT(PORTC, GREEN_LED);
    CLR_BIT(PORTC, BLUE_LED);
    CLR_BIT(PORTC, BUZZER_PIN); 
}

void led_on(char a)
{
    leds_off();
    if (a == 'R')      SET_BIT(PORTC, RED_LED);
    else if (a == 'G') SET_BIT(PORTC, GREEN_LED);
    else if (a == 'B') SET_BIT(PORTC, BLUE_LED);
}

void keypad_init()
{   //01111100
    DDRD &= ~0x3C;    
    PORTD |= 0x3C;                           
    //11110000
    DDRB |= (0xF0);   
    PORTB |= (0xF0);  
}

char keypad_getkey()
{
    for (uint8_t row = 4; row < 8; row++) {
        CLR_BIT(PORTB, row);  

        // Check every column while this row is selected
        for (uint8_t col = 2; col < 6; col++) {
            if (GET_BIT(PIND, col) == 0) {

                _delay_ms(20);            
                while (GET_BIT(PIND, col) == 0); 
                                                  

                SET_BIT(PORTB, row);       
                return keypad[row - 4][col - 2];
            }
        }
        SET_BIT(PORTB, row);  
    }
    return 0; 
}


typedef struct {
    const char *code;   // e.g. ".-"
    char letter;        // e.g. 'A'
} MorseEntry;

const MorseEntry morse_table[] = {
    {".-",   'A'}, {"-...", 'B'}, {"-.-.", 'C'}, {"-..",  'D'}, {".",    'E'},
    {"..-.", 'F'}, {"--.",  'G'}, {"....", 'H'}, {"..",   'I'}, {".---", 'J'},
    {"-.-",  'K'}, {".-..", 'L'}, {"--",   'M'}, {"-.",   'N'}, {"---",  'O'},
    {".--.", 'P'}, {"--.-", 'Q'}, {".-.",  'R'}, {"...",  'S'}, {"-",    'T'},
    {"..-",  'U'}, {"...-", 'V'}, {".--",  'W'}, {"-..-", 'X'}, {"-.--", 'Y'},
    {"--..", 'Z'},
    {"-----",'0'}, {".----",'1'}, {"..---",'2'}, {"...--",'3'}, {"....-",'4'},
    {".....",'5'}, {"-....",'6'}, {"--...",'7'}, {"---..",'8'}, {"----.",'9'}
};

#define MORSE_TABLE_SIZE 36

char decode_morse(const String &code)
{
    for (uint8_t i = 0; i < MORSE_TABLE_SIZE; i++) {
        if (code == morse_table[i].code)
            return morse_table[i].letter;
    }
    return '?'; 
}

const char* encode_morse(char letter)
{
    for (uint8_t i = 0; i < MORSE_TABLE_SIZE; i++) {
        if (morse_table[i].letter == letter)
            return morse_table[i].code;
    }
    return NULL; // not found
}

void play_morse(const char *code)
{
    if (code == NULL) return;

    for (uint8_t i = 0; code[i] != '\0'; i++) {
        if (code[i] == '.') {
            led_on('R');
            buzzer_on();
            _delay_ms(150);  
        } else if (code[i] == '-') {
            led_on('G');
            buzzer_on();
            _delay_ms(400);  
        }
        leds_off();
        _delay_ms(150); 
    }
}


void appSetup()
{
    leds_init();
    keypad_init();

    lcd.begin(16, 2);
    lcd.setCursor(0, 0);
    lcd.print("Dual Morse Tool");
    lcd.setCursor(0, 1);
    lcd.print("Press M to Tgl");
    _delay_ms(200);
    lcd.clear();

    lcd.setCursor(0, 0); lcd.print("Code:");
    lcd.setCursor(0, 1); lcd.print("Msg:");
}

void appLoop()
{
    char key = keypad_getkey();
    if (key == 0) return; // no key pressed -> nothing to do this pass

    if (key == 'M')
    {
        current_mode = !current_mode; 
        lcd.clear();
        morseBuf = "";
        message  = "";
        textBuf  = "";

        if (current_mode == 0) {
            lcd.print("Mode: Morse->Txt");
            _delay_ms(100);
            lcd.clear();
            lcd.setCursor(0, 0); lcd.print("Code:");
            lcd.setCursor(0, 1); lcd.print("Msg:");
        } else {
            lcd.print("Mode: Txt->Morse");
            _delay_ms(100);
            lcd.clear();
            lcd.setCursor(0, 0); lcd.print("In: ");
            lcd.setCursor(0, 1); lcd.print("Morse: Ready");
        }
        return;
    }

    if (current_mode == 0)
    {
        if (key == KEY_DOT)
        {
            if (morseBuf.length() < MORSE_MAX) morseBuf += '.';
            led_on('R'); buzzer_on(); _delay_ms(150); leds_off();
        }
        else if (key == KEY_DASH)
        {
            if (morseBuf.length() < MORSE_MAX) morseBuf += '-';
            led_on('G'); buzzer_on(); _delay_ms(400); leds_off();
        }
        else if (key == KEY_DECODE)
        {
            if (morseBuf.length() > 0)
            {
                char letter = decode_morse(morseBuf);
                led_on('B'); _delay_ms(200); leds_off();
                if (message.length() >= MSG_MAX) message = ""; 
                message += letter;
            }
            morseBuf = "";
        }
        else if (key == KEY_SPACE)
        {
            if (message.length() >= MSG_MAX) message = "";
            message += ' ';
            morseBuf = "";
        }
        else if (key == KEY_CLEAR)
        {
            morseBuf = "";
            message  = "";
        }

        lcd.setCursor(5, 0);  lcd.print("           ");
        lcd.setCursor(5, 0);  lcd.print(morseBuf);
        lcd.setCursor(4, 1);  lcd.print("            ");
        lcd.setCursor(4, 1);  lcd.print(message);
    }

    else
    {
        if (key == KEY_CLEAR)
        {
            textBuf = "";
            lcd.setCursor(4, 0);  lcd.print("            ");
            lcd.setCursor(0, 1);  lcd.print("Morse: Ready    ");
            return;
        }

        if (key == KEY_DECODE) 
        {
            if (textBuf.length() > 0)
            {
                for (uint8_t i = 0; i < textBuf.length(); i++)
                {
                    char current_char = textBuf[i];
                    const char* morse_str = encode_morse(current_char);

                    lcd.setCursor(0, 1);
                    lcd.print("Playing:        ");
                    lcd.setCursor(9, 1);
                    lcd.print(current_char);
                    lcd.print(" ");
                    lcd.print(morse_str);

                    if (morse_str != NULL)
                        play_morse(morse_str);

                    _delay_ms(450); 
                }

                led_on('B');
                _delay_ms(300);
                leds_off();

                lcd.setCursor(0, 1);
                lcd.print("Morse: Sent!    ");
            }
            return;
        }

        if (key == KEY_DOT || key == KEY_DASH || key == KEY_SPACE || key == 'M')
            return;

        if (textBuf.length() < TEXT_MAX)
        {
            textBuf += key;
            lcd.setCursor(4, 0);
            lcd.print(textBuf);
        }
    }
}

int main(void)
{
    appSetup();   
    while (1)     
    {
        appLoop(); 
    }

}