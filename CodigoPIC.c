#include <16F877A.h> //Carga el PIC a utilizar.
#include <stdio.h>  // Permite usar librer�a de ATOI (convierte string a entero)
#include <stdlib.h> // idem requerido.

#define DERECHA 1
#define IZQUIERDA 0

#fuses HS,NOWDT,NOPROTECT,NOLVP,PUT,NODEBUG,NOBROWNOUT,NOCPD,NOWRT 
#use delay (clock=20000000)   //Frecuencia de cristal de 20MHz.

#USE RS232 (BAUD=9600,XMIT=pin_C6,RCV=pin_C7) // Pin C6 transmite y C7 recibe.

#use fast_io(c) // Optimiza c�digo (en vez de standard_io(c))
#use fast_io(a)
#use fast_io(b)

int move_motor_skip_sensor(int32 pasos, int dir);
int32 move_motor_with_sensor(int32 pasos, int dir,int32 velocidad);

void led_on(int32 led);
void led_off();
void motor_on(int32 motor);
void motor_off();

void main()
{
    setup_adc_ports(NO_ANALOGS);
    setup_adc(ADC_CLOCK_DIV_2);
    setup_psp(PSP_DISABLED);
    setup_spi(SPI_SS_DISABLED);
    setup_timer_0(RTCC_INTERNAL|RTCC_DIV_1);
    setup_timer_1(T1_DISABLED);
    setup_timer_2(T2_DIV_BY_16,155,1);
    setup_ccp1(CCP_PWM);
    setup_ccp2(CCP_PWM);
    set_pwm1_duty(312); // Inicia el Ciclo de Trabajo PWM1 en 50%.
    set_pwm2_duty(312); // Inicia el Ciclo de Trabajo PWM2 en 50%.
    setup_comparator(NC_NC_NC_NC);
    setup_vref(FALSE);

    set_tris_a(0b11100000); // 
    set_tris_c(0b10000000); //Pone RC7 como input y RC6 como output (y de 5 a 0 tambi�n)
    set_tris_b(0b00000000); // Habilita como salidas los pines B0, B1,...,B7
    set_tris_e(0b010);



    // ************************ CONFIGURACI�N PWM1 y PWM2: ************************
    int32 brillo=0;
    int32 exposicion=500;   //Tiempo de exposici�n de la c�mara en [ms]
    int32 der_steps=0;
    int32 izq_steps=0;
    int32 led=0;
    int32 motor=0;
    int32 pasos=0;
    int32 velocidad=0;
    int direccion=0;
    int dir=0;
    //---
    char c;
    int i=0;
    char command[60];
    char *ptr_command;
    char *ptr_instruction;
    char delim[1] = ',';
    char action=0;

    output_low(PIN_B0);
    output_low(PIN_B1);
    output_low(PIN_B2);
    output_low(PIN_B3);
    output_low(PIN_B4);
    output_high(PIN_B6); // Siempre en 5V para conectar pull up 10kOhm de RA4 para SLEEP MOTOR 3 (altura)

    set_pwm1_duty(0); // Mantiene Ciclos en 0 para reducir consumo al iniciar.
    set_pwm2_duty(0);

    //*************** INICIO ***************
    while(true)
    {

        output_low(PIN_A2);
        output_low(PIN_A3);
        output_low(PIN_A4);
        i = 0;
        while (( c = getchar() ) != '\n'){
            command[i++] = (char)c;
            if (i == 60)
                break;
        }
        command[i] = '\0';
        for (i = 1, ptr_command = command; ; i++, ptr_command = NULL)
        {
            ptr_instruction = strtok(ptr_command,delim);
            if (ptr_instruction == NULL)
                break;
            //MOTOR
            if (i == 1)
                //motor
                motor=atoi32(ptr_instruction);
            if (i == 2)
                //velocidad
                velocidad=atoi32(ptr_instruction);
            if (i == 3)
                //direccion
                direccion=atoi32(ptr_instruction);
            if (i == 4)
                //pasos
                pasos=atoi32(ptr_instruction);
            //LED
            if (i == 5)
                //led
                led=atoi32(ptr_instruction);
            if (i == 6)
                //exposicion
                exposicion=atoi32(ptr_instruction);
            if (i == 7){
                //brillo
                brillo=atoi(ptr_instruction);
                set_pwm1_duty(brillo*20000000/(100*2000*16));
                set_pwm2_duty(brillo*20000000/(100*2000*16));
            }
            if (i == 8)
                strcpy(action,ptr_instruction);
        }
        c = '0';

        switch(action)
        {
            case '1':
                led_on(led);
                printf("ok");
                break;

            case '2':
                led_off();
                printf("ok");
                break;

            case '3':
                led_on(led);
                delay_ms(exposicion);
                led_off();
                printf("ok");
                break;

            case '4':
                motor_on(motor);
                move_motor_skip_sensor(pasos,direccion);
                motor_off();
                printf("ok");
                break;

            case '5': 
                int32 pasos_restantes;
                int32 steps;
                dir = direccion;
                steps = pasos;
                pasos_restantes = pasos;
                motor_on(motor); 
                while(pasos_restantes > 0){
                    delay_us(200);
                    steps = move_motor_with_sensor(pasos_restantes,dir,velocidad);
                    pasos_restantes = pasos_restantes - steps;
                    if (pasos_restantes <=0)
                        break;
                    delay_us(200);
                    dir = (dir == 0)?1:0;
                    move_motor_skip_sensor(2000,dir);
                }
                printf("ok");
                break;

            case '6':
                int1 status=1;
                dir = direccion;
                motor_on(motor);
                move_motor_with_sensor(pasos,direccion,velocidad);
                status = input_state(PIN_A5);
                if (status == 0){
                    dir = (dir == 0)?1:0;
                    move_motor_skip_sensor(2000,dir);
                    delay_us(200);
                    move_motor_with_sensor(2147483640,dir,velocidad);
                    delay_us(200);
                    dir = (dir == 0)?1:0;
                    move_motor_skip_sensor(2000,dir); 
                }
                motor_off();
                printf("ok");
                break;

            case '8': 
                motor_on(motor); 
                move_motor_with_sensor(2147483640,DERECHA,velocidad);
                delay_us(200);
                move_motor_skip_sensor(2000,IZQUIERDA);
                delay_us(200);
                izq_steps = move_motor_with_sensor(2147483640,IZQUIERDA,velocidad);
                delay_us(200);
                move_motor_skip_sensor(2000,DERECHA);
                delay_us(200);
                der_steps = move_motor_with_sensor(2147483640,DERECHA,velocidad);
                while(true){
                    move_motor_skip_sensor(izq_steps,IZQUIERDA);
                    delay_us(200);
                    move_motor_skip_sensor(der_steps,DERECHA);
                    delay_us(200);
                }
        }

    }
}  //FIN MAIN

void motor_on(int32 motor){
    (motor == 1) ? output_high(PIN_A2):output_low(PIN_A2);
    (motor == 2) ? output_high(PIN_A3):output_low(PIN_A3);
    (motor == 3) ? output_high(PIN_A4):output_low(PIN_A4);
}

void led_on(int32 led){
    //Apagar todos los pin de leds.
    output_low(PIN_B0);
    output_low(PIN_B1);
    output_low(PIN_B2);
    output_low(PIN_B3);
    output_low(PIN_B4);
    //Matriz A Enable
    output_high(PIN_B3);
    //Descomposicion LED para encender demux
    ((led & 1) == 1) ? output_high(PIN_B0):output_low(PIN_B0);
    ((led & 2) == 2) ? output_high(PIN_B1):output_low(PIN_B1);
    ((led & 4) == 4) ? output_high(PIN_B2):output_low(PIN_B2);
}

void led_off(){
    output_low(PIN_B0);
    output_low(PIN_B1);
    output_low(PIN_B2);
    output_low(PIN_B3);
    output_low(PIN_B4);
}

void motor_off(){
    output_low(PIN_A1); 
    delay_ms(100);
    output_low(PIN_A2);
    output_low(PIN_A3);
    output_low(PIN_A4);
}

int32 move_motor_with_sensor(int32 pasos, int dir,int32 velocidad)
{
    int32 y=0;
    int1 status=1;
    output_low(PIN_A1);
    (dir == 1)?output_high(PIN_A0):output_low(PIN_A0);
    delay_ms(velocidad);
    for(y=0;y<pasos;y++)
    {
        output_low(PIN_A1);
        output_high(PIN_A1);
        status = input_state(PIN_A5);
        if (status == 0){
            return y;
        }
        delay_us(velocidad);
    }
    return y;
}

int move_motor_skip_sensor(int32 pasos, int dir)
{
    int32 y=0;
    output_low(PIN_A1);
    (dir == 1)?output_high(PIN_A0):output_low(PIN_A0);
    delay_ms(100);
    for(y=0;y<pasos;y++)
    {
        output_low(PIN_A1);
        output_high(PIN_A1);
        delay_us(200);
    }
    return 0;
}
