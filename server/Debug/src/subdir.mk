################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/EchoServer.cpp \
../src/epollserver.cpp \
../src/ringbuffer.cpp \
../src/user.cpp 

CPP_DEPS += \
./src/EchoServer.d \
./src/epollserver.d \
./src/ringbuffer.d \
./src/user.d 

OBJS += \
./src/EchoServer.o \
./src/epollserver.o \
./src/ringbuffer.o \
./src/user.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/EchoServer.d ./src/EchoServer.o ./src/epollserver.d ./src/epollserver.o ./src/ringbuffer.d ./src/ringbuffer.o ./src/user.d ./src/user.o

.PHONY: clean-src

