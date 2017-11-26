################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/pigpio_functions.cpp \
../src/pigpio_rfcontrol.cpp \
../src/virtualhomeduino.cpp 

OBJS += \
./src/pigpio_functions.o \
./src/pigpio_rfcontrol.o \
./src/virtualhomeduino.o 

CPP_DEPS += \
./src/pigpio_functions.d \
./src/pigpio_rfcontrol.d \
./src/virtualhomeduino.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	$(CROSS_PREFIX)g++ -DRF_CONTROL_VARDUINO=1 -O3 -I $(PIGPIO_SRC) -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


