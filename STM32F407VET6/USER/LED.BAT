SET PATH=F:\Keil5\ARM\ARMCC\Bin;F:\VM\bin\;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Windows\System32\OpenSSH\;C:\Program Files\WIDCOMM\Bluetooth Software\;C:\Program Files\WIDCOMM\Bluetooth Software\syswow64;;C:\Program Files (x86)\NVIDIA Corporation\PhysX\Common;C:\Users\River\AppData\Local\Microsoft\WindowsApps
SET ARMCC5_ASMOPT=--diag_suppress=9931
SET ARMCC5_CCOPT=--diag_suppress=9931
SET ARMCC5_LINKOPT=--diag_suppress=9931
SET CPU_TYPE=STM32F407VE
SET CPU_VENDOR=STMicroelectronics
SET UV2_TARGET=LED
SET CPU_CLOCK=0x00B71B00
"F:\Keil5\ARM\ARMCC\Bin\ArmCC" --Via "..\obj\main.__i"
"F:\Keil5\ARM\ARMCC\Bin\ArmCC" --Via "..\obj\stm32f4xx_it.__i"
"F:\Keil5\ARM\ARMCC\Bin\ArmCC" --Via "..\obj\system_stm32f4xx.__i"
"F:\Keil5\ARM\ARMCC\Bin\ArmCC" --Via "..\obj\led.__i"
"F:\Keil5\ARM\ARMCC\Bin\ArmCC" --Via "..\obj\oled.__i"
"F:\Keil5\ARM\ARMCC\Bin\ArmCC" --Via "..\obj\delay.__i"
"F:\Keil5\ARM\ARMCC\Bin\ArmCC" --Via "..\obj\sys.__i"
"F:\Keil5\ARM\ARMCC\Bin\ArmCC" --Via "..\obj\usart.__i"
"F:\Keil5\ARM\ARMCC\Bin\ArmAsm" --Via "..\obj\startup_stm32f40_41xxx._ia"
"F:\Keil5\ARM\ARMCC\Bin\ArmCC" --Via "..\obj\misc.__i"
"F:\Keil5\ARM\ARMCC\Bin\ArmCC" --Via "..\obj\stm32f4xx_gpio.__i"
"F:\Keil5\ARM\ARMCC\Bin\ArmCC" --Via "..\obj\stm32f4xx_rcc.__i"
"F:\Keil5\ARM\ARMCC\Bin\ArmCC" --Via "..\obj\stm32f4xx_syscfg.__i"
"F:\Keil5\ARM\ARMCC\Bin\ArmCC" --Via "..\obj\stm32f4xx_usart.__i"
"F:\Keil5\ARM\ARMCC\Bin\ArmLink" --Via "..\OBJ\LED.lnp"
"F:\Keil5\ARM\ARMCC\Bin\fromelf.exe" "..\OBJ\LED.axf" --i32combined --output "..\OBJ\LED.hex"
