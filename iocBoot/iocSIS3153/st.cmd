#!../../bin/linux-x86_64/SIS3153App

< envPaths

epicsEnvSet("PREFIX", "SIS1:")
epicsEnvSet("PORT", "SIS3153")

## Register all support components
dbLoadDatabase "../../dbd/SIS3153App.dbd"
SIS3153App_registerRecordDeviceDriver pdbbase

drvSIS3153Configure("$(PORT)")

dbLoadRecords("$(SIS3153)/db/Zygo1404.template", "P=$(PREFIX),PORT=$(PORT)")
dbLoadRecords("$(SIS3153)/db/Joerger.template", "P=$(PREFIX),PORT=$(PORT)")

asynSetTraceIOMask($(PORT),0,ESCAPE)
asynSetTraceMask($(PORT),0,ERROR|DRIVER)

iocInit

