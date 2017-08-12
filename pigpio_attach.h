static inline void hw_attachInterrupt(uint8_t interrupt_pin, void (*ic)(void)) {
        _hw_interruptCallback = ic;
//        gpioSetISRFunc(interrupt_pin,EITHER_EDGE,0,NULL);
        _hw_interrupt_pin = interrupt_pin;
}

