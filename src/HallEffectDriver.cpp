
#include "Arduino.h"
#include "HallEffectDriver.h"

HallEffectDriver::HallEffectDriver(uint8_t sensorPin) : _sensorPin(sensorPin)
{
    if (HallEffectDriver::_triggerEventQueue == nullptr)
        HallEffectDriver::_triggerEventQueue = xQueueCreate(hallQueueLength, sizeof(HallEffectEvent));
}

void HallEffectDriver::start()
{
    setupISR();
}

void IRAM_ATTR HallEffectDriver::sensorTriggered_ISR(void *arg)
{
    HallEffectEvent event;
    event.triggerTimestamp = CURRENT_TIME_US(); // Capture the current timestamp

    // Attempt to post the event to the queue. Since the ISR cannot block,
    // use xQueueOverwrite which ensures the latest event is always in the queue.
    // This function can be used safely within an ISR and will overwrite the
    // oldest item in the queue if the queue is full.
    BaseType_t higherPriorityTaskWoken = pdFALSE;
    xQueueOverwriteFromISR(_triggerEventQueue, &event, &higherPriorityTaskWoken);

    // If a higher priority task was woken by this operation, request a context switch
    if (higherPriorityTaskWoken)
    {
        portYIELD_FROM_ISR();
    }
}

QueueHandle_t HallEffectDriver::getEventQueue() const
{
    return HallEffectDriver::_triggerEventQueue;
}

esp_err_t HallEffectDriver::setupISR()
{
    gpio_num_t pin = static_cast<gpio_num_t>(_sensorPin);

    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << _sensorPin);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);

    intr_handle_t gpio_isr_handle; // Declare the handle

    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    ESP_ERROR_CHECK(gpio_isr_handler_add(pin, &HallEffectDriver::sensorTriggered_ISR, nullptr));

    return ESP_OK;
}
QueueHandle_t HallEffectDriver::_triggerEventQueue = nullptr; // Or appropriate initialization
