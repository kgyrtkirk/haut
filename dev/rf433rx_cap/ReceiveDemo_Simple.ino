
    #include "Arduino.h"

#define PORTA_RF  2
#define INT_RF    0
#define LED_DEBUG 13
#define DEBUG     1
#define BUFF_SIZE 180

volatile unsigned long tempo;
volatile unsigned int  intervalo;

volatile unsigned char lock;
volatile unsigned char eof;

unsigned int  buffIdx;
unsigned int buff[BUFF_SIZE];

void setup()
{
  resetVars();


  Serial.begin(115200);
  delay(500);
  Serial.println("INICIADOx!");


  pinMode(PORTA_RF, INPUT);
  digitalWrite(PORTA_RF, HIGH);

  if (DEBUG)
  {
    pinMode(LED_DEBUG, OUTPUT);
    digitalWrite(LED_DEBUG, LOW);
  }

  attachInterrupt(INT_RF, intRF, CHANGE);
}

bool	xx(int u,int v){
	return (u-40 <= v && v<=u+40);

}
void loop()
{
  if (lock && !eof)
  {
    buff[buffIdx] = intervalo;

    lock = false;

    buffIdx++;
    if (buffIdx > BUFF_SIZE - 1)
    {
      eof = true;
      buffIdx = 0;
    }
  }

  if (eof)
  {
	  if(false){
    // Imprime resultado
    Serial.println("buff:");
    for (int i = 0; i < BUFF_SIZE / 3; i++)
    {
      Serial.print(buff[i], DEC);
      if (i < BUFF_SIZE - 1) Serial.print(",");
    }
    Serial.println();
    for (int i = BUFF_SIZE / 3; i < (BUFF_SIZE / 3) * 2; i++)
    {
      Serial.print(buff[i], DEC);
      if (i < BUFF_SIZE - 1) Serial.print(",");
    }
    Serial.println();
    for (int i = (BUFF_SIZE / 3) * 2; i < BUFF_SIZE; i++)
    {
      Serial.print(buff[i], DEC);
      if (i < BUFF_SIZE - 1) Serial.print(",");
    }

	  }
	  else{
		  long	decoded=0;
		  int	l=-2;
		  for(int i=0;i<BUFF_SIZE;i+=2){
			  int a=buff[i];
				int b = buff[i+1];
				switch (l) {
				case -2:
					if (xx(676, a) && xx(10140, b)) {
						l++;
						continue;
					}
					break;
				case -1:
					if (xx(4764, a) && xx(1540, b)) {
						l++;
						continue;
					}
					break;
				default:
					if (xx(328, a) && xx(748, b)) {
						l++;
						decoded <<= 1;
						decoded |= 0;
						continue;
					}
					if (xx(684, a) && xx(396, b)) {
						l++;
						decoded <<= 1;
						decoded |= 1;
						continue;
					}
					break;
				}
				if(l>38){
					Serial.print("L");
					Serial.print(l);
					Serial.print("  C: ");
					Serial.println(decoded,2);
					for(int j=i-(l+2)*2;j<i+2;j++){
						Serial.print(",");
						Serial.print(buff[j]);
					}
					Serial.println("");
				    Serial.println();
				    Serial.println();
				}
				l = -2;
				decoded = 0;
		  }
	  }

    resetVars();

    delay(200);
  }
}

void intRF()
{
  if (!lock && !eof)
  {
    lock = true;

    long	m=micros();
    intervalo =  m - tempo;
    tempo = m;

    if (DEBUG)
      digitalWrite(LED_DEBUG, !digitalRead(LED_DEBUG));
  }
}

void resetVars()
{
  tempo = micros();
  intervalo = 0;
  lock = false;
  eof = false;

  buffIdx = 0;
  for (int i = 0; i < BUFF_SIZE; i++)
  {
    buff[i] = 0;
  }
}

