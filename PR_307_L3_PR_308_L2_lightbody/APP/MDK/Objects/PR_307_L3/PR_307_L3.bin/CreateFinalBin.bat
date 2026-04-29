@echo off
goto start
/********************************************************************************/

/***************************ROM**********************************
华大HC32F4A0SITB 单片机 Flash ： 1M ；扇区：128个（每个8K）; 
编程单位是 :4bytes
扇区擦除单位为 :8Kbytes
______________________________________________________________（共2M，只用1M）
/********************************************************************************/

/***************************ROM**********************************
 _____________________________________________________________________
|     boot    |    base data   |   app area   |  update area |
|-----16k------|------24k-------|-----432k-----|------xxxk------|
0x00000000    0x00004000        0x00010000      0x0007C000      0x00200000
*****************************************************************/

/***************************RAM********************************
 ________________________________________________________
|                  |LaunchStatus    | Security Encrypt.o |
|-----------------------------------|--------256Byte-----|
0x20000000        0x20017E00       0x20017F00            0x20018000
*****************************************************************/
set SECURITY_AREA_SIZE=256
set BOOT_AREA_SIZE=0x16000
set BASEDATA_AREA_SIZE=0x20000
set BASEDATA_AREA_OFFSET=0x9C000
set UPDATE_AREA_OFFSET=0xBC000
/********************************************************************************/
:start

set arg1=%1

set SECURITY_AREA_SIZE=256
set BOOT_AREA_SIZE=0x1A000
set BASEDATA_AREA_SIZE=0x20000
set APP_AREA_OFFSET=0x1A000
set BASEDATA_AREA_OFFSET=0x9C000
set UPDATE_AREA_OFFSET=0xBC000

set BaseDataBinName=base_data.bin
set SecurityBinName=security_encrypt.bin
set AppBinName=app.bin
set ProductEraseTime=8000
set ProductIntervalTime=300


if %arg1% EQU PR_307_L3 (
    set UUID=000K5
    set ProductName="PR_307_L3"
    set OutPutName="ES_CS15"
    set BOOT_NAME="PR_307_L3_BOOT.bin"
    set ProductUpgradeUUID=0x00000902
) else if %arg1% EQU PR_308_L2 (
    set UUID=000J5
    set ProductName="PR_308_L2"
    set OutPutName="ES_XT26"
    set BOOT_NAME="PR_308_L2_BOOT.bin"
    set ProductUpgradeUUID=0x00001002
) else (
    echo error argument!
    goto out
)

cd /d %~dp0
::从version.h获取文件信息
..\..\VersionGet.exe LAMP_SOFTWART_VER ..\..\..\project_config.h
set /a FirmwareVersion=%errorlevel%
set /a ver_H=%FirmwareVersion% / 16
set /a ver_L=%FirmwareVersion% %% 16
set SoftVer_Float=%ver_H%.%ver_L%
..\..\VersionGet.exe LAMP_HARDWARE_VER ..\..\..\project_config.h
set /a HardwareVersion=%errorlevel%
set /a ver_H=%HardwareVersion% / 16
set /a ver_L=%HardwareVersion% %% 16
set HardVer_Float=%ver_H%.%ver_L%
::*********************************************


::生成未加密的带头updata区文件，产线烧录自动升级security_encrypt.bin
::Firmware_Encrypt.exe ProductName FirmwareVersion HardwareVersion FirmwareType InputFile OutputFile
..\..\Firmware_Encrypt.exe %ProductName% %FirmwareVersion% %HardwareVersion% 0 %SecurityBinName% security_ota.bin no_encrypt >nul
::..\..\packing_firmware.exe  -p %ProductName% -s %FirmwareVersion% -h %HardwareVersion% -t 0 -I %ProductIntervalTime% -u %ProductUpgradeUUID% -E %ProductEraseTime% -i  %SecurityBinName%
::合并boot+app
..\..\File_Merge.exe %BOOT_NAME% %AppBinName% %BOOT_AREA_SIZE% boot_and_app.bin >nul
::合并boot+app+base_data
..\..\File_Merge.exe boot_and_app.bin %BaseDataBinName% %BASEDATA_AREA_OFFSET% boot_and_app_and_basedata.bin >nul
::在Update区放置一个升级security_encrypt区带头信息升级文件,生成产线烧录文件，烧录后开机自动完成升级security_encrypt区
..\..\File_Merge.exe boot_and_app_and_basedata.bin security_ota.bin %UPDATE_AREA_OFFSET% "%ProductName%123_HardwareV%HardVer_Float%_SoftwareV%SoftVer_Float%.bin" >nul
del security_ota.bin >nul
del boot_and_app.bin >nul
del boot_and_app_and_basedata.bin >nul

::合并加密区和app区
..\..\File_Merge.exe %SecurityBinName% %AppBinName% %SECURITY_AREA_SIZE% security_and_app.bin >nul
if %errorlevel% NEQ 0 ( 
    echo Merge %SecurityBinName% and %AppBinName% fialed!
    exit /b -1 
    )

::生成蓝牙OTA升级文件，加密
::..\..\Firmware_Encrypt.exe %ProductName% %FirmwareVersion% %HardwareVersion% 0 security_and_app.bin "%OutPutName%_Drv_HardwareV%HardVer_Float%_SoftwareV%SoftVer_Float%.bin"
..\..\packing_firmware.exe  -p %ProductName% -s %FirmwareVersion% -h %HardwareVersion% -t 0 -I %ProductIntervalTime% -u %ProductUpgradeUUID% -E %ProductEraseTime% -i security_and_app.bin -e
copy %ProductName%_HardwareV%HardVer_Float%_SoftwareV%SoftVer_Float%.bin %OutPutName%_Drv_HardwareV%HardVer_Float%_SoftwareV%SoftVer_Float%.bin
move %ProductName%_HardwareV%HardVer_Float%_SoftwareV%SoftVer_Float%.bin %OutPutName%_%UUID%_Dr_HardwareV%HardVer_Float%_SoftwareV%SoftVer_Float%.bin
::生成基础数据OTA文件
::..\..\Firmware_Encrypt.exe %ProductName% %FirmwareVersion% %HardwareVersion% 1 %BaseDataBinName% "%OutPutName%1_Drv_HardwareV%HardVer_Float%_SoftwareV%SoftVer_Float%.bin" >nul
..\..\packing_firmware.exe  -p %ProductName% -s %FirmwareVersion% -h %HardwareVersion% -t 1 -I %ProductIntervalTime% -u %ProductUpgradeUUID% -E %ProductEraseTime% -i %BaseDataBinName% -e
copy %ProductName%_HardwareV%HardVer_Float%_SoftwareV%SoftVer_Float%.bin %OutPutName%1_Drv_HardwareV%HardVer_Float%_SoftwareV%SoftVer_Float%.bin
move %ProductName%_HardwareV%HardVer_Float%_SoftwareV%SoftVer_Float%.bin %OutPutName%_%UUID%1_Dr_HardwareV%HardVer_Float%_SoftwareV%SoftVer_Float%.bin
::合并base_data.bin和security_and_app.bin,一次性升级两块区域
..\..\File_Merge.exe %BaseDataBinName% security_and_app.bin %BASEDATA_AREA_SIZE% temp.bin >nul
::..\..\Firmware_Encrypt.exe %ProductName% %FirmwareVersion% %HardwareVersion% 2 temp.bin "%OutPutName%2_Drv_HardwareV%HardVer_Float%_SoftwareV%SoftVer_Float%.bin" >nul
..\..\packing_firmware.exe  -p %ProductName% -s %FirmwareVersion% -h %HardwareVersion% -t 2 -I %ProductIntervalTime% -u %ProductUpgradeUUID% -E %ProductEraseTime% -i  temp.bin -e
copy %ProductName%_HardwareV%HardVer_Float%_SoftwareV%SoftVer_Float%.bin %OutPutName%2_Drv_HardwareV%HardVer_Float%_SoftwareV%SoftVer_Float%.bin
move %ProductName%_HardwareV%HardVer_Float%_SoftwareV%SoftVer_Float%.bin %OutPutName%_%UUID%2_Dr_HardwareV%HardVer_Float%_SoftwareV%SoftVer_Float%.bin
del temp.bin >nul
del security_and_app.bin >nul
:out
::del %SecurityBinName%
::del %AppBinName%

