/*
  Dispenser

  This sketch controls the dispensers in Robie's vending machine.

  Author: Andy Tracy <adtme11@gmail.com>
*/

const int d1p_pin = 2; // Dispenser 1 positive
const int d1n_pin = 3; // Dispenser 1 negative
const int d2p_pin = 4; // Dispenser 2 positive
const int d2n_pin = 5; // Dispenser 2 negative

void setup()
{
    // Start up the serial connection
    Serial.begin ( 9600 );

    // Motor control pins
    pinMode ( d1p_pin, OUTPUT );
    pinMode ( d1n_pin, OUTPUT );
    pinMode ( d2p_pin, OUTPUT );
    pinMode ( d2n_pin, OUTPUT );

    digitalWrite ( d1p_pin, LOW );
    digitalWrite ( d1n_pin, LOW );
    digitalWrite ( d2p_pin, LOW );
    digitalWrite ( d2n_pin, LOW );
}

void dispense ( int slot, int quant ) 
{
    // Convert quant to time
    unsigned long time = quant * 10000;

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
            while ( Serial.available() < 2 ) {}
            int slot = Serial.read();
            int quant = Serial.read();

            // Dispense based on input
            if ( slot == 1 ) slot = d1n_pin;
            else if ( slot == 0 ) slot = d2n_pin;
            else return;

            dispense ( slot, quant );

            // Send another signal to indicate dispense is complete
            Serial.write ( 'a' );
        } else {
            // Send NACK
            Serial.write ( 'n' );
        }
    }
}
