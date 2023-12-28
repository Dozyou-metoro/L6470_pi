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
    std::vector<uint8_t> ss_pin_list;
    std::vector<uint8_t> busy_pin_list;
    int spi_handle = 0;

    template <typename... type> // ピンの入出力を設定
    void gpio_set(uint8_t set_ss_pin, uint8_t set_busy_pin,type... ss_pins);
    void gpio_set(void);



public:
    template <typename... type>
    stepper_motor(uint32_t spi_speed,uint8_t spi_ch,type...);
    ~stepper_motor();

    template <typename... type> // SPIを送信
    void spi_write(int SS_pin, uint8_t front_data, type... data);

    void spi_write(int SS_pin);

    template <typename... type> // 任意のモーターにSPIを送信
    void send_data(uint8_t motor_no, type... data);

    uint8_t get_motor_num(void);
};

template <typename... type>
stepper_motor::stepper_motor(uint32_t spi_speed,uint8_t spi_ch,type... pins)
{
    system("sudo killall pigpiod");

    if (gpioInitialise() < 0)
    {
        ERROR_PRINT("GPIOの初期化に失敗しました。", -1);
    }

    gpio_set(pins...);

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
void stepper_motor::gpio_set(uint8_t set_ss_pin, uint8_t set_busy_pin,type... ss_pins)
{
    gpioSetMode(set_ss_pin, PI_OUTPUT);
    gpioSetMode(set_busy_pin, PI_INPUT);

    gpioWrite(set_ss_pin, PI_HIGH);

    ss_pin_list.push_back(set_ss_pin);
    busy_pin_list.push_back(set_busy_pin);

    printf("SS_PIN_SET:%d",set_ss_pin);
    printf("BUSY_PIN_SET:%d",set_busy_pin);

    gpio_set(ss_pins...);
}

void stepper_motor::gpio_set(void)
{
}

template <typename... type> // ピンの入出力を設定
inline void stepper_motor::send_data(uint8_t motor_no, type... data)
{   
    /*
    while(!gpioRead(busy_pin_list[motor_no])){
    }
    */

    if (ss_pin_list.size() > motor_no)
    { // 実在しないモーターが指定されたら何もしない
        spi_write(ss_pin_list[motor_no], data...);
    }
    else
    {
    }
}

template <typename... type>
inline void stepper_motor::spi_write(int SS_pin, uint8_t front_data, type... data)
{   


    gpioWrite(SS_pin, PI_LOW);
    spiWrite(spi_handle, (char*)&front_data, sizeof(char));
    gpioWrite(SS_pin, PI_HIGH);

    spi_write(SS_pin,data...);
}

inline void stepper_motor::spi_write(int SS_pin){

}

uint8_t stepper_motor::get_motor_num(void){
    return ss_pin_list.size();
}

#endif

