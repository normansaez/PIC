#include <16F877A.h> //Carga el PIC a utilizar.
#include <stdio.h>  // Permite usar librería de ATOI (convierte string a entero)
#include <stdlib.h> // idem requerido.

#define DERECHA 1
#define IZQUIERDA 0

#fuses HS,NOWDT,NOPROTECT,NOLVP,PUT,NODEBUG,NOBROWNOUT,NOCPD,NOWRT 
#use delay (clock=20000000)   //Frecuencia de cristal de 20MHz.

#USE RS232 (BAUD=9600,XMIT=pin_C6,RCV=pin_C7) // Pin C6 transmite y C7 recibe.

#use fast_io(c) // Optimiza código (en vez de standard_io(c))
#use fast_io(a)
#use fast_io(b)

void motor_on(int32 motor);
void motor_off();

int motores2(int32 pasos, int dir);
int32 motores4(int32 pasos, int dir,int32 velocidad);

void led_on(int32 led);
void led_off();

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
    set_tris_c(0b10000000); //Pone RC7 como input y RC6 como output (y de 5 a 0 también)
    set_tris_b(0b00000000); // Habilita como salidas los pines B0, B1,...,B7
    set_tris_e(0b010);



    // ************************ CONFIGURACIÓN PWM1 y PWM2: ************************
    int32 brillo=0;
    int32 exposicion=500;   //Tiempo de exposición de la cámara en [ms]
    int32 led=0;
    int32 motor=0;
    int32 direccion=0;
    int32 pasos=0;
    int32 velocidad=0;
    char leido_pantalla[5];

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

        char seleccionar=0;
        output_low(PIN_A2);
        output_low(PIN_A3);
        output_low(PIN_A4);


        seleccionar=getc();

        switch(seleccionar)
        {

            case 'v':
                fgets(leido_pantalla);
                velocidad=atoi32(leido_pantalla);
                break;

            case 'e': 
                fgets(leido_pantalla);
                exposicion=atoi32(leido_pantalla);
                break;

            case 'b':
                fgets(leido_pantalla);
                brillo=atoi(leido_pantalla);
                set_pwm1_duty(brillo*20000000/(100*2000*16));
                set_pwm2_duty(brillo*20000000/(100*2000*16));
                break;

            case 'l':
                fgets(leido_pantalla);
                led=atoi32(leido_pantalla);
                break;

            case 'd':
                fgets(leido_pantalla);
                direccion = atoi32(leido_pantalla);               
                break;

            case 'p':
                fgets(leido_pantalla);
                pasos = atoi32(leido_pantalla);               
                break;

            case 'm':
                fgets(leido_pantalla);
                motor = atoi32(leido_pantalla);               
                break;

            case '1':
                led_on(led);
                break;

            case '2':
                led_off();
                break;

            case '3':
                motor_on(motor);
                motores4(pasos,direccion,velocidad);
                break;

            case '4':
                motor_off();
                break;

            case '5': 
                int32 pasos_restantes;
                int32 steps;
                int dir;
                dir = direccion;
                steps = pasos;
                pasos_restantes = pasos;
                motor_on(motor); 
                while(pasos_restantes > 0){
                    delay_us(200);
                    steps = motores4(pasos_restantes,dir,velocidad);
                    pasos_restantes = pasos_restantes - steps;
                    if (pasos_restantes <=0)
                        break;
                    delay_us(200);
                    dir = (dir == 0)?1:0;
                    motores2(2000,dir);
                }
                break;

            case '6': 
                int32 stepsmmm;
                motor_on(motor); 
                motores4(pasos,direccion,velocidad);
                motor_off();
                printf("%Ld",stepsmmm);
                break;

            case '7':
                motor_on(motor); 
                motores2(pasos,direccion);
                motor_off(); 
                break;
        }

    }



}  //FIN MAIN

void motor_on(int32 motor){
    (motor == 1) ? output_high(PIN_A2):output_low(PIN_A2);
    (motor == 2) ? output_high(PIN_A3):output_low(PIN_A3);
    (motor == 3) ? output_high(PIN_A4):output_low(PIN_A4);
}

void led_off(){
    output_low(PIN_B0);
    output_low(PIN_B1);
    output_low(PIN_B2);
    output_low(PIN_B3);
    output_low(PIN_B4);
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

void motor_off(){
    output_low(PIN_A1); 
    delay_ms(100);
    output_low(PIN_A2);
    output_low(PIN_A3);
    output_low(PIN_A4);
}

int32 motores4(int32 pasos, int dir,int32 velocidad)
{
    int32 y=0;
    int1 status=1;
    output_low(PIN_A1);  //STEP
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

int motores2(int32 pasos, int dir)
{
    int32 y=0;
    output_low(PIN_A1);  //STEP
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
