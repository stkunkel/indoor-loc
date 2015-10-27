################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
LD_SRCS += \
../src/lscript.ld 

C_SRCS += \
../src/helloworld.c \
../src/iicTest.c \
../src/iic_utils.c \
../src/platform.c \
../src/pwmTest.c 

OBJS += \
./src/helloworld.o \
./src/iicTest.o \
./src/iic_utils.o \
./src/platform.o \
./src/pwmTest.o 

C_DEPS += \
./src/helloworld.d \
./src/iicTest.d \
./src/iic_utils.d \
./src/platform.d \
./src/pwmTest.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM gcc compiler'
	arm-xilinx-eabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MT"$@" -I../../standalone_bsp_0/ps7_cortexa9_0/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


