# Metope



<p align="center"> <img src="doc/asset/rdme_clk_panel.png" alt="rdme_clk_panel" width="240" height="240;" />  <img src="doc/asset/rdme_pcb_2d.png" alt="rdme_pcb_2d" width="240" height="240;" /> </p>



Do not expect me saying anything about it. Picture tells. If you don't like it, buy an Apple Watch. If you like it, probably buy me a coffee.



## Overview

### Time Setting

### USB



## Requirement

### STM32CubeF4

```bash
cd lib
git clone --recursive https://github.com/STMicroelectronics/STM32CubeF4.git
```



### STM32CubeMX





### Others

```
cmake make git arm-none-eabi-gdb
```





### CMSIS-SVD (Optional)

```bash
cd lib
git clone https://github.com/modm-ext/cmsis-svd-stm32-zip.git
```







## Compile



### Firmware

```bash
cmake -DCMAKE_BUILD_TYPE=Debug -DCHIP=STM32F411CEU6 .. && make -j12
```

>STM32F411CEU6



```bash
cmake -DCMAKE_BUILD_TYPE=Debug -DCHIP=STM32F405RGT6 .. && make -j12
```

> STM32F405RGT6





### Test Bench

```bash
cmake -DCMAKE_BUILD_TYPE=Debug -DCHIP=STM32F411CEU6 -DTEST_ONLY=1 .. && make -j12
```

> STM32F411CEU6



```bash
cmake -DCMAKE_BUILD_TYPE=Debug -DCHIP=STM32F405RGT6 -DTEST_ONLY=1 .. && make -j12
```

> STM32F405RGT6









## Utility

- [**Online GDB**](https://www.onlinegdb.com/)





## Reference

- [**Reference Manual - STM32F405RGT6**](https://www.st.com/resource/en/reference_manual/dm00031020-stm32f405-415-stm32f407-417-stm32f427-437-and-stm32f429-439-advanced-arm-based-32-bit-mcus-stmicroelectronics.pdf)
- [**Reference Manual - STM32F411CEU6**](https://www.st.com/resource/en/reference_manual/rm0383-stm32f411xce-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
- [**Datasheet - STM32F405RGT6**](https://www.st.com/resource/en/datasheet/stm32f405rg.pdf)
- [**Datasheet - STM32F411CEU6**](https://www.st.com/resource/en/datasheet/stm32f411ce.pdf)
- [**Datasheet - QMI8658A**](https://www.qstcorp.com/upload/pdf/202301/13-52-25%20QMI8658A%20Datasheet%20Rev%20A.pdf)
- [**Datasheet - W25Q32**](https://mm.digikey.com/Volume0/opasdata/d220001/medias/docus/5059/W25Q32JV_RevI_5-4-21.pdf)

- [**Datasheet - LSF0204x**](https://www.ti.com/lit/ds/symlink/lsf0204d.pdf?ts=1733701959935)
