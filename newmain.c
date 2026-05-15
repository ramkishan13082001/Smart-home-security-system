#include <xc.h>
#include <stdio.h>
#include <string.h>

#define _XTAL_FREQ 20000000

// --- Global Multitasking State Flags ---
volatile int intrusion_alert = 0;
char entered_password[5];
const char master_password[] = "1234";

// --- Function Prototypes ---
void lcd_init();
void lcd_command(unsigned char cmd);
void lcd_data(unsigned char data);
void lcd_string(const char *str);
void uart_init(long baud_rate);
void uart_send_string(const char *str);
void pwm_init();
void pwm_set_duty(unsigned int duty);
void timer1_delay_5s();
char keypad_scan();
void adc_init();
int adc_read();

// ====================================================================
// HARDWARE INTERRUPT SERVICE ROUTINE (PIR / EMERGENCY)
// ====================================================================
void __interrupt() ISR() {
    if (INTCONbits.INT0IF) {          // If PIR button pressed (RB0 / INT0)
        intrusion_alert = 1;          // Trigger global intrusion flag
        INTCONbits.INT0IF = 0;        // Clear hardware interrupt flag
    }
}

// ====================================================================
// MAIN OPERATING SYSTEM LOOP
// ====================================================================
void main() {
    // --- 1. Pin Configurations ---
    TRISA = 0x01;  // RA0 Input (LDR), RA1-RA5 Output
    TRISB = 0xE1;  // RB0 Input (INT0), RB1-RB4 Output (Rows), RB5-RB7 Input (Cols)
    TRISC = 0x80;  // RC7 RX In, others Output (RC0 Bzr, RC1/3 LEDs, RC2 PWM, RC4 Porch, RC6 TX)
    TRISD = 0x00;  // PORTD Output (LCD Data)
    TRISE = 0x00;  // PORTE Output (LCD RS/EN)
    
    // Default Hardware States
    PORTC = 0x00;
    PORTCbits.RC1 = 1; // System starts Locked (Red LED ON)
    
    // --- 2. Subsystem Initialization ---
    lcd_init();
    adc_init();
    pwm_init();
    uart_init(9600);
    pwm_set_duty(0); // Ensure Door Motor is firmly locked
    
    // --- 3. Interrupt Configuration ---
    INTCONbits.INT0IE = 1;     // Enable INT0
    INTCON2bits.INTEDG0 = 0;   // Trigger on Falling Edge (Button Press)
    INTCONbits.GIE = 1;        // Enable Global Interrupts
    INTCONbits.PEIE = 1;       // Enable Peripheral Interrupts
    
    uart_send_string("\r\n============================\r\n");
    uart_send_string("SMART HOME SECURITY BOOTED\r\n");
    uart_send_string("============================\r\n");
    
    while(1) {
        // --- TASK A. Emergency Intrusion Check ---
        if (intrusion_alert) {
            PORTCbits.RC0 = 1; // Blast Buzzer
            PORTCbits.RC1 = 1; // Red LED ON
            PORTCbits.RC3 = 0; // Green LED OFF
            pwm_set_duty(0);   // Force door locked
            
            lcd_command(0x01);
            lcd_string("INTRUDER ALERT!");
            uart_send_string("CRITICAL: INTRUSION DETECTED!!\r\n");
            
            while(intrusion_alert); // Lock system permanently until physical reboot
        }
        
        // --- TASK B. Keypad Password Collection ---
        lcd_command(0x80);
        lcd_string("ENTER PASSWORD: ");
        lcd_command(0xC0); // Move cursor to line 2
        
        int i = 0;
        // Loop runs while we collect 4 digits, aborts instantly if intruder detected
        while(i < 4 && !intrusion_alert) {
            
            // Background Task: Poll ADC for Porch Light while waiting for keys!
            int light_level = adc_read();
            if (light_level < 300) PORTCbits.RC4 = 1; // Dark -> Light ON
            else PORTCbits.RC4 = 0;                   // Bright -> Light OFF
            
            char key = keypad_scan();
            if (key != 0) {
                entered_password[i] = key;
                lcd_data('*'); // Mask input for security
                uart_send_string("Key press received.\r\n");
                i++;
                __delay_ms(300); // Debounce physical keypad bounce
            }
        }
        
        entered_password[4] = '\0'; // Null-terminate string
        if (intrusion_alert) continue; // Abort processing if intruder tripped
        
        // --- TASK C. Password Verification & Control ---
        if (strcmp(entered_password, master_password) == 0) {
            
            // SUCCESSFUL AUTHENTICATION
            uart_send_string("ACCESS GRANTED. Door unlocking.\r\n");
            lcd_command(0x01); lcd_string("ACCESS GRANTED");
            
            PORTCbits.RC1 = 0;  // Red OFF
            PORTCbits.RC3 = 1;  // Green ON
            pwm_set_duty(1023); // Actuate motor to OPEN door
            
            lcd_command(0xC0); lcd_string("DOOR OPEN (5s)");
            
            // 5 Second Auto-Lock Delay (Can be interrupted by PIR!)
            timer1_delay_5s(); 
            if (intrusion_alert) continue; 
            
            // AUTO LOCK
            uart_send_string("DOOR AUTO-LOCKED.\r\n");
            lcd_command(0x01); lcd_string("DOOR LOCKED");
            
            PORTCbits.RC1 = 1; // Red ON
            PORTCbits.RC3 = 0; // Green OFF
            pwm_set_duty(0);   // Actuate motor to CLOSE door
            __delay_ms(2000);
            
        } else {
            
            // FAILED AUTHENTICATION
            uart_send_string("WARNING: WRONG PASSWORD ENTERED!\r\n");
            lcd_command(0x01); lcd_string("WRONG PASSWORD");
            
            // Emit 3 warning beeps
            for(int j=0; j<3; j++) {
                PORTCbits.RC0 = 1; __delay_ms(100);
                PORTCbits.RC0 = 0; __delay_ms(100);
            }
            __delay_ms(1500);
        }
        
        lcd_command(0x01); // Clear for next cycle
    }
}

// ====================================================================
// PERIPHERAL DRIVERS BELOW
// ====================================================================

// --- Keypad Scanner (4x3 Telephone Matrix) ---
char keypad_scan() {
    // We explicitly set RB1-RB4 without touching RB0 (INT0)
    
    PORTBbits.RB1 = 0; PORTBbits.RB2 = 1; PORTBbits.RB3 = 1; PORTBbits.RB4 = 1;
    if (PORTBbits.RB5 == 0) return '1';
    if (PORTBbits.RB6 == 0) return '2';
    if (PORTBbits.RB7 == 0) return '3';
    
    PORTBbits.RB1 = 1; PORTBbits.RB2 = 0; PORTBbits.RB3 = 1; PORTBbits.RB4 = 1;
    if (PORTBbits.RB5 == 0) return '4';
    if (PORTBbits.RB6 == 0) return '5';
    if (PORTBbits.RB7 == 0) return '6';
    
    PORTBbits.RB1 = 1; PORTBbits.RB2 = 1; PORTBbits.RB3 = 0; PORTBbits.RB4 = 1;
    if (PORTBbits.RB5 == 0) return '7';
    if (PORTBbits.RB6 == 0) return '8';
    if (PORTBbits.RB7 == 0) return '9';
    
    PORTBbits.RB1 = 1; PORTBbits.RB2 = 1; PORTBbits.RB3 = 1; PORTBbits.RB4 = 0;
    if (PORTBbits.RB5 == 0) return '*';
    if (PORTBbits.RB6 == 0) return '0';
    if (PORTBbits.RB7 == 0) return '#';
    
    return 0; // No key pressed
}

// --- Custom Timer1 Auto-Lock Delay (Interrupt Aware) ---
void timer1_delay_5s() {
    int loops = 5000; // 5000ms = 5 seconds
    while (loops > 0 && intrusion_alert == 0) {
        // Generate 1ms precision hardware delay
        TMR1H = 0xFD;
        TMR1L = 0x8F;
        T1CON = 0x31; 
        while (!PIR1bits.TMR1IF && intrusion_alert == 0); 
        PIR1bits.TMR1IF = 0; 
        T1CONbits.TMR1ON = 0; 
        loops--;
    }
}

// --- UART Serial Communication ---
void uart_init(long baud_rate) {
    SPBRG = (unsigned char)((_XTAL_FREQ / (64 * baud_rate)) - 1);
    TXSTAbits.TXEN = 1; 
    RCSTAbits.SPEN = 1; 
}
void uart_send_char(char c) {
    while (!TXSTAbits.TRMT); 
    TXREG = c;
}
void uart_send_string(const char *str) {
    while (*str) uart_send_char(*str++);
}

// --- Analog-to-Digital Converter (LDR Porch Light) ---
void adc_init() {
    ADCON0 = 0x01; 
    ADCON1 = 0x0E; // AN0 Analog
    ADCON2 = 0xA9; 
}
int adc_read() {
    ADCON0bits.GO = 1;
    while (ADCON0bits.GO);
    return ((ADRESH << 8) + ADRESL);
}

// --- Hardware PWM (Door Motor Actuator) ---
void pwm_init() {
    PR2 = 0xFF;    
    CCP1CON = 0x0C;
    T2CON = 0x06;  
}
void pwm_set_duty(unsigned int duty) {
    CCPR1L = duty >> 2;
    CCP1CON = (CCP1CON & 0xCF) | ((duty & 0x03) << 4);
}

// --- LCD Display Output (PORTE Ctrl, PORTD Data) ---
void lcd_init() {
    __delay_ms(15);
    lcd_command(0x38); // 8-bit mode
    lcd_command(0x0C); // Display ON
    lcd_command(0x06); // Auto-increment
    lcd_command(0x01); // Clear display
    __delay_ms(2);
}
void lcd_command(unsigned char cmd) {
    PORTD = cmd;
    PORTEbits.RE0 = 0; // RS = 0
    PORTEbits.RE1 = 1; // EN = 1
    __delay_ms(1);
    PORTEbits.RE1 = 0; // EN = 0
}
void lcd_data(unsigned char data) {
    PORTD = data;
    PORTEbits.RE0 = 1; // RS = 1
    PORTEbits.RE1 = 1; // EN = 1
    __delay_ms(1);
    PORTEbits.RE1 = 0; // EN = 0
}
void lcd_string(const char *str) {
    while(*str) lcd_data(*str++);
}