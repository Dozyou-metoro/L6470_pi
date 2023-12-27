#ifndef stepper
#define stepper

#include <cstdint>
#include <thread>
#include <cmath>
#include <vector>

#include "../pigpio.h"

#include "MESSAGE_PRINT.h"

class stepper_motor
{
private:
    uint32_t spi_speed = 1000000;
    uint8_t spi_ch = 0;
    std::vector<uint8_t> ss_pin;
    int spi_handle = 0;

    template <typename... type> // ピンの入出力を設定
    void gpio_set(uint8_t set_ss_pin, type... ss_pins);
    void gpio_set(void);

    template <typename... type> // ピンの入出力を設定
    void send_data(uint8_t motor_no, type... data);

public:
    template <typename... type>
    stepper_motor(uint32_t spi_speed,uint8_t spi_ch,type...);
    ~stepper_motor();

    template <typename... type> // SPIを送信
    void spi_write(int SS_pin, uint8_t front_data, type... data);
};

template <typename... type>
stepper_motor::stepper_motor(uint32_t spi_speed,uint8_t spi_ch,type... ss_pins)
{
    system("sudo killall pigpiod");

    if (gpioInitialise() < 0)
    {
        ERROR_PRINT("GPIOの初期化に失敗しました。", -1);
    }

    gpio_set(ss_pins...);

    spi_handle = spiOpen(spi_ch, spi_speed, 0);

    if (spi_handle < 0)
    {
        ERROR_PRINT("SPIの初期化に失敗しました。", -2);
    }
}

stepper_motor::~stepper_motor()
{
}

template <typename... type>
void stepper_motor::gpio_set(uint8_t set_ss_pin, type... ss_pins)
{
    gpioSetMode(set_ss_pin, PI_OUTPUT);
    gpioWrite(set_ss_pin, PI_HIGH);

    ss_pin.push_back(set_ss_pin);

    gpio_set(ss_pins);
}

void stepper_motor::gpio_set(void)
{
}

template <typename... type> // ピンの入出力を設定
void stepper_motor::send_data(uint8_t motor_no, type... data)
{
    if (ss_pin.size() > motor_no)
    { // 実在しないモーターが指定されたら何もしない
        spi_write(ss_pin[motor_no], data...);
    }
    else
    {
    }
}

#endif

template <typename... type>
void stepper_motor::spi_write(int SS_pin, uint8_t front_data, type... data)
{
    gpioWrite(SS_pin, PI_LOW);
    spiWrite(spi_handle, &front_data, sizeof(char));
    gpioWrite(SS_pin, PI_HIGH);

    spi_write(SS_pin, data...);
}