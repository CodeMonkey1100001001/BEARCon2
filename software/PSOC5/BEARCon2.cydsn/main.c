

#include <project.h>

void setFrequency0(double freqRequired);
void setFrequency1(double freqRequired);
void debugStringNumberFloat(char8* theStr,float theNum);
void getStrFloat(char8* str, float fval);
void getStrFloatNeg(char8* str, float fval);
void getStrFloatSmall(char8* str, float fval);

void debugNOCR(char *outStr);
void updateDisplay();
void updateLED();

double leftFreq=500.0;
double rightFreq=500.0;
double offsetFreq=4.3;
double bf=0.0;
int main()
{
    Counter_1_Start();
    Counter_2_Start();
    
    QuadDec1_Start();
    QuadDec2_Start();
    QuadDec3_Start();
    
    LCD_Char1_Start();
    LCD_Char1_DisplayOn();
    LCD_Char1_Enable();
    LCD_Char1_PrintString("B.E.A.R. Con. v2.0");
    CyDelay(2000);

    PWM_Start();
    
    WaveDAC8_1_Start(); 
    WaveDAC8_2_Start();
    //double baseFreq=500;
    setFrequency0(500.0+offsetFreq); QuadDec1_SetCounter(500.0*10);
    setFrequency1(500.0+offsetFreq); QuadDec2_SetCounter(500.0*10);
    //setFrequency3(1.0);
    
    int16 quadCount1=0;
    int16 oldQCount1=0;
    int16 quadCount2=0;
    int16 oldQCount2=0;
    int16 quadCount3=0;
    int16 oldQCount3=0;

    QuadDec3_SetCounter(43);
    
    
    
    
    updateDisplay();
    
    for (;;)
    {
        quadCount1=QuadDec1_GetCounter();
        if (quadCount1!=oldQCount1)
        {
            if (CoarsePIN_Read()==0) //rapid tune pressed
            {
                int theDifference=quadCount1-oldQCount1;
                if (theDifference<0) quadCount1=quadCount1-10;
                else quadCount1=quadCount1+10;
                QuadDec1_SetCounter(quadCount1);
            }
            
            double newFreq=(((double)(quadCount1))/10);
            setFrequency0(newFreq+offsetFreq);
            oldQCount1=quadCount1;
            leftFreq=newFreq;
            updateDisplay();
            updateLED();    
        }
        quadCount2=QuadDec2_GetCounter();
        if (quadCount2!=oldQCount2)
        {
            if (CoarsePIN_Read()==0) //rapid tune pressed
            {
                int theDifference=quadCount2-oldQCount2;
                if (theDifference<0) quadCount2=quadCount2-10;
                else quadCount2=quadCount2+10;
                QuadDec2_SetCounter(quadCount2);
            }
            
            double newFreq=(((double)(quadCount2))/10);
            setFrequency1(newFreq+offsetFreq);
            oldQCount2=quadCount2;
            rightFreq=newFreq;
            updateDisplay();
            updateLED();
        }
        quadCount3=QuadDec3_GetCounter();
        if (quadCount3!=oldQCount3)
        {
            double newFreq=(((double)(quadCount3))/10);
            offsetFreq=newFreq;
            oldQCount3=quadCount3;

            newFreq= (((double)(oldQCount1))/10);
            setFrequency0(newFreq+offsetFreq);

            newFreq= (((double)(oldQCount2))/10);
            setFrequency1(newFreq+offsetFreq);
            
            updateDisplay();
            updateLED();
        
        }
    }
    
    
}
void updateLED()
{
    double f1=(double)(QuadDec1_GetCounter());
    double f2=(double)(QuadDec2_GetCounter());
    f1=f1/10.0;
    f2=f2/10.0;
    
    double difference=f1-f2;
    if (difference<0)
    {
        difference=difference*(-1.0);
    }
    if (difference<1) difference=1.0;
    //difference=difference+0.1;

    int wc=(1000/difference);
    int wp=wc/2;
    
    PWM_WritePeriod(wc);
    PWM_WriteCompare(wp);
    bf=difference;//global
    
}
void updateDisplay()
{
    char8 theBuf[32];
    
    
    
    LCD_Char1_ClearDisplay();

    LCD_Char1_Position(0u,0u);
    LCD_Char1_PrintString("L");
    LCD_Char1_Position(0u,1u);
    getStrFloat(theBuf,leftFreq);
    LCD_Char1_PrintString(theBuf);

    LCD_Char1_Position(0u,15u);
    LCD_Char1_PrintString("R");
    LCD_Char1_Position(0u,9u);
    getStrFloat(theBuf,rightFreq);
    LCD_Char1_PrintString(theBuf);

    LCD_Char1_Position(1u,0u);
    LCD_Char1_PrintString("O:");
    LCD_Char1_Position(1u,2u);
    getStrFloatNeg(theBuf,offsetFreq);
    LCD_Char1_PrintString(theBuf);

    //LCD_Char1_Position(1u,9u);
    //LCD_Char1_PrintString("BF:");
    //LCD_Char1_Position(1u,12u);
    //getStrFloatSmall(theBuf,bf);
    //LCD_Char1_PrintString(theBuf);


}
void setFrequency0(double freqRequired0)
{
    int writePeriod0; // clock of 12Mhz
    writePeriod0=(int)(24000000.0/(freqRequired0*100.0));//100 samples in wavdac
    Counter_1_WritePeriod(writePeriod0);
}
void setFrequency1(double freqRequired1)
{
    int writePeriod1; // clock of 12Mhz
    writePeriod1=(int)(24000000.0/(freqRequired1*100.0));//100 samples in wavdac
    Counter_2_WritePeriod(writePeriod1);
}

void debugStringNumberFloat(char8* theStr,float theNum)
{
    char8 theBuf[32];
    debugNOCR(theStr);
    getStrFloat(theBuf,theNum);
    debugNOCR(theBuf);
    //SWTXUART_PutString("\r\n");

}
void getStrFloat(char8* str, float fval)
{
    int val=fval*100;
    str[6]=0u;//null terminate the string.
    str[5]='0'+(val%10); /// .x
    val=val/10;
    str[4]='0'+(val%10); /// .x
    val=val/10;
    str[3]='.';
    str[2]='0'+(val%10);
    val=val/10;
    str[1]='0'+(val%10);
    val=val/10;
    str[0]='0'+(val%10);
}
void getStrFloatSmall(char8* str, float fval)
{
    int val=fval*10;
    str[4]=0u;//null terminate the string.
    str[3]='0'+(val%10); /// .x
    val=val/10;
    str[2]='.';
    str[1]='0'+(val%10);
    val=val/10;
    str[0]='0'+(val%10);
}
void getStrFloatNeg(char8* str, float fval)
{
    int val=fval*100;
    str[7]=0u;//null terminate the string.
    if (val<0)
    {
        str[6]='-';
        val=val*(-1);
    }
    else str[6]='+';
    
    str[5]='0'+(val%10); /// .x
    val=val/10;
    str[4]='0'+(val%10); /// .x
    val=val/10;
    str[3]='.';
    str[2]='0'+(val%10);
    val=val/10;
    str[1]='0'+(val%10);
    val=val/10;
    str[0]='0'+(val%10);
}
void debugNOCR(char *outStr)
{
    LCD_Char1_ClearDisplay();
    LCD_Char1_Position(0u,0u);
    LCD_Char1_PrintString(outStr);
}

/* [] END OF FILE */
