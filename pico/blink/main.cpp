#include <pico/stdlib.h>

#define LED_PIN 25

int main()
{
    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    while (true)
    {
        gpio_put(LED_PIN, 0x01);
        sleep_ms(500);

        gpio_put(LED_PIN, 0x00);
        sleep_ms(500);
    }

    return 0;
}

// EOF
