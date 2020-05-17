#include <Wire.h>

long  d_old = -1;		// Variable stockant la distance lorsque trop loin ou trop pres

#define LONG    60		// Distance LONG
#define MED     40		// Distance MED
#define SHORT   20		// Distance SHORT

#define S_LONG  2000	// Son LONG
#define S_MED   4000	// Son MED
#define S_SHORT 6000	// Son SHORT [ Sauf pour la partie trop pres ]

#define US_E 6	//	Recepteur Ultra Sons
#define US_R 7	//	Emeteur Ultra Sons

#define V 13	// LED Verte	: Ok
#define O 12	// LED Orange	: Prudence
#define R 11	// LED Rouge	: Stop/Trop pres

#define Z 3		// BUZZER pin
#define E -1
#define TIME  500	// Delay avant actualisation

void  ft_revon(long n)
{
	//
	//	Rev ft_on
	//

	digitalWrite(R, HIGH);
	delay(n);
	digitalWrite(O, HIGH);
	delay(n);
	digitalWrite(R, LOW);
	delay(n);
	digitalWrite(V, HIGH);
	delay(n);
	digitalWrite(O, LOW);
	delay(n);
	digitalWrite(V, LOW);
}

void  ft_on(long n)
{
	//
	//	Fonction qui fait clignote V, O, R
	//

	digitalWrite(V, HIGH);
	delay(n);
	digitalWrite(O, HIGH);
	delay(n);
	digitalWrite(V, LOW);
	delay(n);
	digitalWrite(R, HIGH);
	delay(n);
	digitalWrite(O, LOW);
	delay(n);
	digitalWrite(R, LOW);
	delay(n);
}

void setup(void)
{
	//
	//  Setup
	//

	Serial.begin (9600);
	Serial.println("Radar de recul buzzer + LED");
	pinMode(US_E, OUTPUT);
	digitalWrite(US_E,LOW);
	pinMode(US_R, INPUT);
	pinMode(V, OUTPUT);
	pinMode(O, OUTPUT);
	pinMode(R, OUTPUT);
	pinMode(Z, OUTPUT);
	digitalWrite(Z,LOW);
	ft_on(100);
	Serial.println("Operationnel !");
}

long  x = 0;

void  ft_serial_clear(void)
{
	//
	// Clear serial terminal
	//

	Serial.write(27);
	Serial.print("[2J");
	Serial.write(27);
	Serial.print("[H");
}

void loop(void)
{
	//
	//	Loop
	//

	long  duree;
	long  distance;
	long  d_old;
	long  sound;

	digitalWrite(US_E, LOW);		// Extinction US_E
	delayMicroseconds(10);
	digitalWrite(US_E, HIGH);		// Envoie d'une salve US_E
	delayMicroseconds(10);
	digitalWrite(US_E, LOW);		// Extinction US_E
	duree = pulseIn(US_R, HIGH);	// Reception US_R
	distance = (duree / 60);		// Conversion duree en distance
	digitalWrite(V, LOW);		// Extinction V
	digitalWrite(O, LOW);		// Extinction O
	digitalWrite(R, LOW);		// Extinction R
	ft_serial_clear();
	if (distance >= 1000)
	{
		//
		//	Erreur distance ou distance trop grande
		//

		Serial.println("!distance");
		Serial.println("EEE | !distance");
		digitalWrite(Z, HIGH);
		sound = S_MED;
		distance = -1;
	}
	else
	{
		//
		//	Affiche la distance
		//

		Serial.print(distance);
		Serial.println(" cm");
	}
	delay(10);
	d_old = 0;
	if (distance > MED)
	{
		//
		//	Distance OK
		//

		Serial.println("V-- | Good");
		digitalWrite(V, HIGH);
	}
	if (distance <= MED && distance > SHORT)
	{
		//
		//	Distance Attention
		//

		Serial.println("-O- | Warning");
		digitalWrite(O, HIGH);
		digitalWrite(Z, HIGH);
		sound = S_MED;
	}
	if (distance <= SHORT && distance >= 0)
	{
		//
		//	Distance trop pres
		//

		Serial.println("--R | Stop");
		digitalWrite(R, HIGH);
		digitalWrite(Z, LOW);
		sound = (distance % 2 == 0) ? ((distance % 3) == 0 ? 3000 : 4000 ) : ((distance % 5 == 0) ? 5000 : 6000);
	}
	if (distance == -1)
	{
		//
		//	Distance erreur
		//

		ft_on((TIME / 2) / 6);
		delay((TIME / 2) % 6);
		ft_revon((TIME / 2) / 6);
		digitalWrite(Z, HIGH);
		sound = S_MED;
		tone(Z, sound);
	}
	else
	{
		if (distance <= MED)
			tone(Z, sound);
		else
			noTone(Z);
		delay(TIME);//500
	}
}
