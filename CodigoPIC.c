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

void motor_off();
void motores(int32 pasos, int dir);
int motores2(int32 pasos, int dir);
int32 motores3(int32 pasos, int dir);

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
    int v1=50;
    int v2=50;
    int retardo=1; // Retardo por defecto de 1[ms]
    int t_on=100;  //Tiempo en [ms] que se mantiene encendido el trigger CCD (0 si se desea on-off instant�neo)
    char ciclo[3];
    char ciclo2[3];
    char expo[5];
    int16 exposicion=500;   //Tiempo de exposici�n de la c�mara en [ms]
    int aux=1;
//    int1 status=1;
//    int direccion=1;
//    int32 error=0;
//    int32 steps=0;
    int32 der_steps=0;
    int32 izq_steps=0;

    output_low(PIN_B0);
    output_low(PIN_B1);
    output_low(PIN_B2);
    output_low(PIN_B3);
    output_low(PIN_B4);
    output_high(PIN_B6); // Siempre en 5V para conectar pull up 10kOhm de RA4 para SLEEP MOTOR 3 (altura)

    set_pwm1_duty(0); // Mantiene Ciclos en 0 para reducir consumo al iniciar.
    set_pwm2_duty(0);

    //*************** INICIO ***************

    int32 pasos1;
    //int dir1;



    char darpasos[5];


    while(true)
    {

        char seleccionar=0;
        output_low(PIN_A2);
        output_low(PIN_A3);
        output_low(PIN_A4);


        printf("Seleccione a, b, e, 1, 2, 3, 4, 5, 6 , 7. 8 =test, 9= cal\n\r");
        seleccionar=getc();



        switch(seleccionar)
        {

            case 'e': // define tiempo de exposici�n.

                printf("Ingrese tiempo de exposicion en [ms] y pulse ENTER:\n\r");
                fgets(expo);
                exposicion=atoi32(expo);
                printf("Exposicion en [ms]: ");
                printf("%Ld \n\r",exposicion);
                break;

            case 'a': // Define Ciclo PWM1.

inicia1: //Label usado para redirigir el programa ante error en ingreso de Ciclo.

                printf("Ingrese Ciclo de Trabajo para PWM1 (0-100) y pulse ENTER:\n\r");
                fgets(ciclo);
                v1=atoi(ciclo);
                if(v1>100 || v1<=0)
                {
                    printf("Ingrese un n�mero entero valido\n\r");
                    goto inicia1;
                }
                set_pwm1_duty(v1*20000000/(100*2000*16));
                printf("Ciclo de Trabajo PWM1 es; %d",v1);
                printf("\n\r");
                break;

            case 'b':

inicia2: //Label usado para redirigir el programa ante error en ingreso de Ciclo.

                printf("Ingrese Ciclo de Trabajo para PWM2 (0-100) y pulse ENTER:\n\r");
                fgets(ciclo2);
                v2=atoi(ciclo2);

                if(v2>100 || v2<=0)
                {
                    printf("Ingrese un n�mero entero valido\n\r");
                    goto inicia2;
                }

                set_pwm2_duty(v2*20000000/(100*2000*16));
                printf("Ciclo de Trabajo PWM2 es; %d",v2);
                printf("\n\r");
                break;


            case '1': //motor 1 a la izquierda.
                printf("Ingrese pasos\n\r");
                fgets(darpasos);
                pasos1=atoi32(darpasos);
                printf("caso 1 izquierda");
                output_high(PIN_A2); // Activa motor 1.  
                motores(pasos1,IZQUIERDA);
                break;

            case '2': //motor 1 a la derecha.
                printf("Ingrese pasos\n\r");
                fgets(darpasos);
                pasos1=atoi32(darpasos);
                printf("caso 2 derecha");
                output_high(PIN_A2); // Activa motor 1.
                motores(pasos1,DERECHA);
                break;

            case '3': //motor 2 a la izquierda.
                printf("Ingrese pasos\n\r");
                fgets(darpasos);
                pasos1=atoi32(darpasos);
                printf("caso 3 izquierda");
                output_high(PIN_A3); // Activa motor 1.  
                motores(pasos1,IZQUIERDA);
                break;

            case '4': //motor 2 a la derecha.
                printf("Ingrese pasos\n\r");
                fgets(darpasos);
                pasos1=atoi32(darpasos);
                printf("caso 4 derecha");
                output_high(PIN_A3); // Activa motor 1.
                motores(pasos1,DERECHA);
                break;

            case '5': //motor 3 a la izquierda.
                printf("Ingrese pasos\n\r");
                fgets(darpasos);
                pasos1=atoi32(darpasos);
                printf("caso 5 izquierda");
                output_high(PIN_A4); // Activa motor 1.  
                motores(pasos1,IZQUIERDA);
                break;

            case '6': //motor 3 a la derecha.
                printf("Ingrese pasos\n\r");
                fgets(darpasos);
                pasos1=atoi32(darpasos);
                printf("caso 6 derecha");
                output_high(PIN_A4); // Activa motor 1.
                motores(pasos1,DERECHA);

                break;

            case '8': 
                printf("Setup calibracion quick\n\r");
                der_steps = 500;
                izq_steps = 500;
                output_high(PIN_A4); 
                motores3(2147483640,DERECHA);
                delay_us(200);
                motores2(100,IZQUIERDA);
                delay_us(200);
                izq_steps = motores3(2147483640,IZQUIERDA);
                delay_us(200);
                motores2(100,DERECHA);
                delay_us(200);
                der_steps = motores3(2147483640,IZQUIERDA);
                printf("izq_steps ->%Ld<-  \n\r",izq_steps);
                printf("der_steps ->%Ld<-  \n\r",der_steps);
                goto loopInfinito;
loopInfinito:
                motores2(izq_steps,IZQUIERDA);
                delay_us(200);
                motores2(der_steps,DERECHA);
                delay_us(200);
                goto loopInfinito;

            case '7': // PWMs-LEDs

                //INICIA LEDs:


                output_low(PIN_B0);
                output_low(PIN_B1);
                output_low(PIN_B2);
                output_low(PIN_B3);
                output_low(PIN_B4);
                output_low(PIN_B5);
                delay_ms(100);    //delay de 100ms para luego iniciar secuencia de control del DEMUX
                //                printf("Corriendo Matriz A y B\n\r");
                output_high(PIN_B3); // Activa Enable para iniciar el demux en 000.

                for(aux=0;aux<3;aux++)
                {

                    //                    printf("for");
                    delay_ms(retardo);   // Delay para tener LED encendido y luego iniciar trigger CCD.
                    //                    printf("pasa retardo");
                    output_high(PIN_B5); //Activa trigger CCD.
                    delay_ms(t_on);
                    output_low(PIN_B5);  //Desactiva trigger CCD.
                    //                    printf("antes de morir");
                    delay_ms(exposicion+retardo-t_on); // Mantiene control en LLL=> Salida Y0=1 (High) y el resto 0 (Low).

                    //                    printf("for1");
                    output_high(PIN_B0);
                    delay_ms(retardo);
                    output_high(PIN_B5); //Activa trigger CCD.
                    delay_ms(t_on);
                    output_low(PIN_B5);
                    delay_ms(exposicion+retardo-t_on);
                    output_low(PIN_B0);
                    //                    printf("for2");
                    output_high(PIN_B1);
                    delay_ms(retardo);
                    output_high(PIN_B5); //Activa trigger CCD.
                    delay_ms(t_on);
                    output_low(PIN_B5);
                    delay_ms(exposicion+retardo-t_on);

                    //                    printf("for3");
                    output_high(PIN_B0);
                    delay_ms(retardo);
                    output_high(PIN_B5); //Activa trigger CCD.
                    delay_ms(t_on);
                    output_low(PIN_B5);
                    delay_ms(exposicion+retardo-t_on);

                    //                    printf("for4");
                    output_low(PIN_B0);
                    output_low(PIN_B1);
                    output_high(PIN_B2);
                    delay_ms(retardo);
                    output_high(PIN_B5); //Activa trigger CCD.
                    delay_ms(t_on);
                    output_low(PIN_B5);
                    delay_ms(exposicion+retardo-t_on);

                    //                    printf("for5");
                    output_high(PIN_B0);
                    delay_ms(retardo);
                    output_high(PIN_B5); //Activa trigger CCD.
                    delay_ms(t_on);
                    output_low(PIN_B5);
                    delay_ms(exposicion+retardo-t_on);

                    //                    printf("for6");
                    output_low(PIN_B0);
                    output_high(PIN_B1);
                    delay_ms(retardo);
                    output_high(PIN_B5); //Activa trigger CCD.
                    delay_ms(t_on);
                    output_low(PIN_B5);
                    delay_ms(exposicion+retardo-t_on);

                    //                    printf("for7");
                    output_high(PIN_B0);
                    delay_ms(retardo);
                    output_high(PIN_B5); //Activa trigger CCD.
                    delay_ms(t_on);
                    output_low(PIN_B5);
                    delay_ms(exposicion+retardo-t_on);

                    //                    printf("for setea a cero\n\r");
                    output_low(PIN_B0);  //Setea a 0 los pines para dejarlos como al inicio.
                    output_low(PIN_B1);
                    output_low(PIN_B2);
                    output_low(PIN_B3);
                    output_low(PIN_B4);

                    //                    printf("Ha operado una matriz");
                    //                    printf("\n\r");
                    if(aux==2)
                    {
                        goto salir;
                    }

                    output_high(PIN_B4); // Activa Enable Matriz Bpara iniciar el demux en 000.
                    //                    printf("un B4 high\n\r");
                }
salir:
                output_low(PIN_B3);
                output_low(PIN_B4);
                set_pwm1_duty(0); // Mantiene Ciclos en 0 para reducir consumo.
                set_pwm2_duty(0);
                aux=1;

                break;
        }

    }



}  //FIN MAIN
void motor_off(){
    output_low(PIN_A1); 
    delay_ms(100);
    output_low(PIN_A2);
    output_low(PIN_A3);
    output_low(PIN_A4);
}

int32 motores3(int32 pasos, int dir)
{
    int32 y=0;
    int1 status=1;
    output_low(PIN_A1);  //STEP
    if(dir==0)
    {
        output_low(PIN_A0);
    }
    if(dir==1)
    {
        output_high(PIN_A0);
    }
    delay_ms(100);
    for(y=0;y<pasos;y++)
    {
        output_low(PIN_A1);
        output_high(PIN_A1);
        status = input_state(PIN_A5);
        if (status == 0){
            printf("status sensor ->%d<-  \n\r",status);
            return y;
        }
        delay_us(200);
    }
    return y;
}

int motores2(int32 pasos, int dir)
{
    int32 y=0;
    output_low(PIN_A1);  //STEP
    if(dir==0)
    {
        output_low(PIN_A0);
    }
    if(dir==1)
    {
        output_high(PIN_A0);
    }
    delay_ms(100);
    for(y=0;y<pasos;y++)
    {
        output_low(PIN_A1);
        output_high(PIN_A1);
        delay_us(200);
    }
    return 0;
}



void motores(int32 pasos, int dir)
{
    // PAR�METROS:

    int32 y=0;
    int32 y2;

    // ACCIONES:


    output_low(PIN_A1);  //STEP



    if(dir==0)
    {
        output_low(PIN_A0);
        //        printf("bajo\n\r");
    }
    if(dir==1)
    {
        output_high(PIN_A0);
        //        printf("alto\n\r");
    }



    //if(input_state(PIN_A5)==0)
    //{
    //   printf("Sensor bloqueado. No se permite giro del motor.");
    //   goto apagamotor;  //No permite iniciar giro por estar bloqueado el sensor.
    //}



    delay_ms(400);

    for (y=0;y<pasos;y++)   //Rota motor.
    {
        y2=y;
        if(input_state(PIN_A5)==0) //Si se bloquea cualquier sensor.
        {
            //            printf("cambio giro\n\r");
            if(dir==0)   //Si giraba hacia la izquierda,
                {
                    output_high(PIN_A0); //ahora gira hacia la derecha, invirtiendo sentido.
                    goto invertir;
                }
            if(dir==1) // Si giraba hacia la derecha,
                {
                    output_low(PIN_A0); // ahora gira hacia la izquierda, invirtiendo sentido.
                }
invertir:
            delay_ms(500);
            for(y2=0;y2<pasos;y2++)
            {
                output_low(PIN_A1);
                output_high(PIN_A1);
                delay_us(200);
            }
            goto apagamotor;

        }

        output_low(PIN_A1);
        output_high(PIN_A1);
        delay_us(200);
    }

apagamotor:

    output_low(PIN_A1); 
    delay_ms(100);
    output_low(PIN_A2);
    output_low(PIN_A3);
    output_low(PIN_A4);
}

