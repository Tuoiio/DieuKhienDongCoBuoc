#include "main.h"

/*https://blog.csdn.net/en_Wency/article/details/123987654?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522170265503016800184198399%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fall.%2522%257D&request_id=170265503016800184198399&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~rank_v31_ecpm-4-123987654-null-null.142^v96^pc_search_result_base8&utm_term=change%20arr%20stm32
*/
_Bool 	Direct_Motor = 0;
uint8_t 	Set_Speed_Motor = 0, Set_Revolutions = 0; // Gioi han cai dat 0 < x < 255

const uint16_t PulsePerRound = 200;	// Do phan giai cua dong co 400 xung/vong

volatile uint32_t Count_Revolutions = 0;	// Dem so vong quay duoc
uint32_t Temp_Revolutions;						// So vong dong co phai quay

volatile float CountTimer = 0.0f;				
float Period = 0.0f, Count_TOP = 0.0f, Count_BOT = 0.0f;
float tempAdd = 0.0f;



uint32_t last_Delay_1Hz = 0;
_Bool Clock_1Hz;
_Bool Running = 0;

uint8_t List = 0;


void Hien_thi_LCD(void);
void KeyBoard(void);

int main(void)
{
	SystemTick_Init(SYSTICK_1MS);
	Delay_Timer_Init(TIM4);
	GPIO_Config();
	Timer_Config();
	LCD20X4_Init();
	last_Delay_1Hz = GetTick();
	while(1){
		if(GetTick() - last_Delay_1Hz > 500){
			Clock_1Hz = !Clock_1Hz;
			last_Delay_1Hz = GetTick();
		}
		Hien_thi_LCD();
		KeyBoard();
	}
}

void Hien_thi_LCD(void){
	LCD20X4_Gotoxy(0,0);
	LCD20X4_PutString("Step Motor");
	LCD20X4_Gotoxy(15, 0);
	if(Running == 1) LCD20X4_PutString("Start");
	else					LCD20X4_PutString("Stop ");
	LCD20X4_Gotoxy(0,1);
	if(List == 0){
		LCD20X4_PutString("Huong quay:");
		if(Direct_Motor == 1) LCD20X4_PutString("Thuan ");
		else						  LCD20X4_PutString("Nghich");

		LCD20X4_Gotoxy(0,2);
		LCD20X4_PutString("Toc do quay:");
		LCD20X4_SendInteger(Set_Speed_Motor);
		LCD20X4_Gotoxy(15,2);
		LCD20X4_PutString("RPM/M");

		LCD20X4_Gotoxy(0,3);
		LCD20X4_PutString("So vong quay:");
		LCD20X4_SendInteger(Set_Revolutions);
		LCD20X4_Gotoxy(16,3);
		LCD20X4_PutString("Turn");
	
	} else {
		if(Clock_1Hz){
				LCD20X4_Gotoxy(11,1);
				if(Direct_Motor == 1) LCD20X4_PutString("Thuan ");
				else						  LCD20X4_PutString("Nghich");
				LCD20X4_Gotoxy(12,2);
				LCD20X4_SendInteger((int)Set_Speed_Motor);
				LCD20X4_Gotoxy(13,3);
				LCD20X4_SendInteger((int)Set_Revolutions);
		} else {		
			if(List == 1) {
				LCD20X4_Gotoxy(11,1);
				LCD20X4_PutString("      ");
			} else if( List == 2) {
				LCD20X4_Gotoxy(12,2);
				LCD20X4_PutString("   ");
			} else if( List == 3) {
				LCD20X4_Gotoxy(13,3);
				LCD20X4_PutString("   ");
			}
		}
	}
}

void KeyBoard(void){
	static _Bool Old_State_Ok, Old_State_Mode, Old_State_Up, Old_State_Down;
	static uint8_t Time_Break_Up = 0, Time_Break_Down = 0;
	
	/************************************** Button Mode ***************************************/
	if(Button_Mode == 0){
		Delay_ms(10);
		if( !Button_Mode && Old_State_Mode){
			// do thing
			List++;
			if(List > 3) List = 0;
		}
	}
	Old_State_Mode = Button_Mode;

	
	/************************************** Button Up ***************************************/
	if(Button_Up == 0){
		Delay_ms(10);
		Time_Break_Up++;
		if(Time_Break_Up > 40){
			Time_Break_Up = 37; 
			Old_State_Up = 1;
		}
		if( !Button_Up && Old_State_Up){
			// do thing
			Clock_1Hz = 1;
			switch(List) {
				case 1: { Direct_Motor = 1;		break; }
				case 2: { Set_Speed_Motor++;	break; }
				case 3: { Set_Revolutions++;		break; }
				default: { 								break; }
			}
		}
	} else {
		Time_Break_Up = 0;
	}
	Old_State_Up = Button_Up;
	
	
	/************************************** Button Down ***************************************/
	if(Button_Down == 0){
		Delay_ms(10);
		Time_Break_Down++;
		if(Time_Break_Down > 40){
			Time_Break_Down = 37; 
			Old_State_Down = 1;
		}
		if( !Button_Down && Old_State_Down){
			// do thing
			Clock_1Hz = 1;
			switch(List) {
				case 1: { Direct_Motor = 0; 		break; }
				case 2: { Set_Speed_Motor--;		break; }
				case 3: { Set_Revolutions--;		break; }
				default:{ 								break; }
			}
		}
	} else {
		Time_Break_Down = 0;
	}
	Old_State_Down = Button_Down;
	
	
	/************************************** Button Ok ***************************************/
	if(Button_OK == 0){
		Delay_ms(10);
		if( !Button_OK && Old_State_Ok && !Running){
			if(Set_Revolutions != 0 && Set_Speed_Motor != 0){
				// do thing
				List = 0;
				Running = 1;
				GPIO_WriteBit(GPIOB, Pin_Enable, Bit_RESET);												// Cho phep dong co chay
				GPIO_WriteBit(GPIOB, Pin_Direct_Motor, (BitAction)Direct_Motor);					// Huong motor quay
				Temp_Revolutions = (uint32_t)Set_Revolutions * (uint32_t)PulsePerRound;						// So vong motor quay
				Period = (1000000.0f / ((float)Set_Speed_Motor / 60.0f * (float)PulsePerRound));			// chuyen tu vong/phut -> thoi gian delay
				Count_BOT = Period;
				Count_TOP = Period / 2.0f;
				tempAdd = (float)(Count_TOP / (uint32_t)(Count_TOP / 10.0f));
				TIM_Cmd(TIM2, ENABLE);
			}
		}
	}
	Old_State_Ok = Button_OK;
}



void TIM2_IRQHandler(void){
	float Temp_TOP, Temp_BOT;
	
	if(TIM_GetITStatus(TIM2, TIM_IT_Update != RESET))
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		if(Count_Revolutions < Temp_Revolutions)
		{
			CountTimer = CountTimer + tempAdd; 
			if(Set_Speed_Motor > 180 && Count_Revolutions < 400){
				Temp_TOP = Count_TOP + 75.0f;
				Temp_BOT = Count_BOT + 75.0f;
			} else {
				Temp_TOP = Count_TOP;
				Temp_BOT = Count_BOT;
			}
			if(CountTimer <= Temp_TOP)  GPIOA->ODR |= (1 << 8);
			else 									  GPIOA->ODR &= ~(1 << 8);
		
			if(CountTimer > Temp_BOT)	
			{
				CountTimer = 0;
				Count_Revolutions++;
			}
		} else if (Count_Revolutions == Temp_Revolutions) {
			Count_Revolutions = 0;
			TIM_Cmd(TIM2, DISABLE);
			GPIO_WriteBit(GPIOB, Pin_Enable, Bit_SET);
			Running = 0;
		}
	}
}

