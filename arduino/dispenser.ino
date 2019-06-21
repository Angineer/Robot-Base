/*
  Dispenser

  This sketch controls the dispensers in Robie's vending machine.

  Author: Andy Tracy <adtme11@gmail.com>
*/

const int d1_pin = 2;
const int d2_pin = 3;

void setup()
{
    // Start up the serial connection
    Serial.begin ( 9600 );

    // Motor control pins
    pinMode ( d1_pin, OUTPUT );
    pinMode ( d2_pin, OUTPUT );
}

void dispense ( int slot, int quant ) 
{
    // Convert quant to time
    unsigned long time = quant * 2000;

    // Run the motor for that amount of time
    digitalWrite ( slot, HIGH );
    delay ( time );
    digitalWrite ( slot, LOW );
}

void loop()
{
    // Check for serial input
    if ( Serial.available() > 0 ) {
        // Read in the directive byte
        char directive = Serial.read();

        if ( directive == 'd' ) { // dispense
            // Send ACK
            Serial.write ( 'a' );

            // Read the two data bytes
            int slot = Serial.read();
            int quant = Serial.read();

            // Dispense based on input
            if ( slot == 1 ) slot = d1_pin;
            else if ( slot == 2 ) slot = d2_pin;
            else return;

            dispense ( slot, quant );
        } else {
            // Send NACK
            Serial.write ( 'n' );
        }
    }
}
