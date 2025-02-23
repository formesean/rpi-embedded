#include "../../lib/SPICO/SPICO.hpp"

#define LED_PIN 25

int main()
{
    stdio_init_all();
    SPICO spico;

    spico.pinMode(LED_PIN, SPICO::DIRECTION::OUTPUT);

    while (true)
    {
      spico.digitalWrite(LED_PIN, 1);
      spico.delay(500);

      spico.digitalWrite(LED_PIN, 0);
      spico.delay(500);
    }

}

// EOF
