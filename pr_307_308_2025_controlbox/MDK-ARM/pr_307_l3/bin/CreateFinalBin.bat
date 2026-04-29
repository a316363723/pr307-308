@echo off
goto start
/********************************************************************************/
/********************************************************************************/
:start
set OFFSET=256
::set BIN_1_NAME=security_encrypt.bin
set BIN_1_NAME=ER_ROM1
set BIN_2_NAME=ER_IROM1
set BIN_3_NAME=ER_ROM2
::set BIN_2_NAME=app.bin
cd /d %~dp0
::set FirmwareVersion=%2
::set HardwareVersion=%3

set BOOT_OFFSET=0x16000
set SECURITY_OFFSET=0x2000
set ProductEraseTime=6500
set ProductIntervalTime=200
set ProductBmpIntervalTime=50
cd /d %~dp0
.\VersionGet.exe PROJECT_NUM .\version.h
set /a projectnum=%errorlevel%
echo %projectnum%
if %projectnum% equ 1 (
		set BOOT_NAME=pr_307_boot.bin
		set OutPutName="ES_CS15"
		set ProductName="PR_307_L3"
        	              set ProductBmpName=PR_307_L3_BMP
		set ProductUUID="000K5"
		set ProductUpgradeUUID=0x00000901
		set ProductUpgradeBmmpUUID=0x00000903
	) else (
		set BOOT_NAME=pr_308_boot.bin
		set OutPutName="ES_XT26"
		set ProductName="PR_308_L2"
                               set ProductBmpName="PR_308_L2_BMP"
		set ProductUUID="000J5"
		set ProductUpgradeUUID=0x00001001
		set ProductUpgradeBmmpUUID=0x00001003
	)

cd /d %~dp0
.\VersionGet.exe SOFTWARE_VERSION .\version.h
set /a FirmwareVersion=%errorlevel%
set /a ver_H=%FirmwareVersion% / 16
set /a ver_L=%FirmwareVersion% %% 16
set SoftVer_Float=%ver_H%.%ver_L%
.\VersionGet.exe HARDWARE_VERSION .\version.h
set /a HardwareVersion=%errorlevel%
set /a ver_H=%HardwareVersion% / 16
set /a ver_L=%HardwareVersion% %% 16
set HardVer_Float=%ver_H%.%ver_L%
::*********************************************

::合并加密区和app区
.\Merge1.exe %BIN_1_NAME% %BIN_2_NAME% %SECURITY_OFFSET% security_and_app.bin >nul
if %errorlevel% NEQ 0 ( 
    echo Merge1 %BIN_1_NAME% and %BIN_2_NAME% fialed!
    exit /b -1 
    )
	
::生成未加密的带头updata区文件，产线烧录自动升级
::Firmware_Encrypt.exe ProductName FirmwareVersion HardwareVersion FirmwareType InputFile OutputFile
::.\Firmware_Encrypt.exe %ProductName% %FirmwareVersion% %HardwareVersion% 0 security_and_app.bin addHead_no_encrypt.bin no_encrypt >nul
::.\Firmware_Encrypt.exe %ProductBmpName% %FirmwareVersion% %HardwareVersion% 3 %BIN_3_NAME% "%OutPutName%_Bmp_HardwareV%HardVer_Float%_SoftwareV%SoftVer_Float%.bin" no_encrypt
.\packing_firmware.exe -p %ProductBmpName% -s %FirmwareVersion% -h %HardwareVersion% -t 3 -I %ProductIntervalTime% -u %ProductUpgradeBmmpUUID% -E %ProductEraseTime% -i %BIN_3_NAME% 
move %ProductName%_BMP_HardwareV%HardVer_Float%_SoftwareV%SoftVer_Float%.bin %OutPutName%_Bmp_HardwareV%HardVer_Float%_SoftwareV%SoftVer_Float%.bin
::合并boot分区,生成生产烧录文件BOOT_OFFSET,BOOT_NAME的大小
::.\Merge1.exe %BOOT_NAME% addHead_no_encrypt.bin %BOOT_OFFSET% "%ProductName%_V%SoftVer_Float%.%HardVer_Float%_boot+app.bin" 
.\Merge1.exe %BOOT_NAME% security_and_app.bin %BOOT_OFFSET% "%ProductName%_V%SoftVer_Float%.%HardVer_Float%_boot+app.bin" 
::del temp.bin >nul


::生成蓝牙OTA升级文件，加密
::.\Firmware_Encrypt.exe %ProductName% %FirmwareVersion% %HardwareVersion% 0 security_and_app.bin "%OutPutName%_Ctr_HardwareV%HardVer_Float%_SoftwareV%SoftVer_Float%.bin"
.\packing_firmware.exe -p %ProductName% -s %FirmwareVersion% -h %HardwareVersion% -t 0 -I %ProductIntervalTime% -u %ProductUpgradeUUID% -E %ProductEraseTime%  -i security_and_app.bin -e
copy %ProductName%_HardwareV%HardVer_Float%_SoftwareV%SoftVer_Float%.bin %OutPutName%_Ctr_HardwareV%HardVer_Float%_SoftwareV%SoftVer_Float%.bin
move %ProductName%_HardwareV%HardVer_Float%_SoftwareV%SoftVer_Float%.bin %OutPutName%_%ProductUUID%_Ctr_HardwareV%HardVer_Float%_SoftwareV%SoftVer_Float%.bin

::del security_and_app.bin >nul
::del addHead_no_encrypt.bin >nul
::del %BIN_1_NAME%
::del %BIN_2_NAME%
